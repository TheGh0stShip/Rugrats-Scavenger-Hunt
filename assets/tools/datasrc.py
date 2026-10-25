#!/usr/bin/env python3
"""Initialized program data: typed source representation, materializer, verifier.

Assigned ROM ranges (VRAM = ROM + 0x7FFFF400), 32,704 bytes:
  0x0103C..0x045E0  surveyed as .rodata; 0x103C..0x10F8 is code (func_8000043C, func_80000498)
  0x045E0..0x045F0  .data
  0x53474..0x57E80  surveyed as .data; 0x53474..0x535A0 is code (tail of func_80052680, bcopy)
Code bytes are rebuilt from src/asm/*.s (assembled and compared); data bytes from src/data/*.

Subcommands:
  generate   analyse ROM + project analysis (read-only) and write src/data/*.c,*.h,*.json
  verify     materialize src/data/*.json (+ assemble src/asm) and compare with the ROM
  compile    compile src/data/*.c with KMC GCC 2.7.2 and src/asm/*.s with GNU as, link at VRAM, compare
  inventory  write inventory/initialized_data.tsv
"""
import argparse, bisect, collections, glob, json, os, re, struct, subprocess, sys, tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import data_access

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "src", "data")
ASM = os.path.join(ROOT, "src", "asm")
ROM_PATH = os.environ.get("ROM", "baseroms/us/baserom.z64")
PROJ = os.environ.get("PROJECT_ROOT", ".")
FULLTEXT = os.environ.get("DATASRC_FULLTEXT") or os.path.join(ROOT, "private_extracted", "disasm", "fulltext.s")
TOOLCHAIN = os.environ.get("N64DK_TOOLCHAIN", ".toolchain")
ROM2VRAM = 0x7FFFF400

RANGES = [(0x103C, 0x45E0, ".rodata"), (0x45E0, 0x45F0, ".data"), (0x53474, 0x57E80, ".data")]
# code found inside the assigned ranges: (vram start, end, asm source, owner)
CODE = [(0x8000043C, 0x800004F8, 'ctors_8000043C.s', 'func_8000043C+func_80000498 (+4 nop pad)'),
        (0x80052874, 0x800529A0, 'bcopy_tail_80052874.s', 'func_80052680 tail (bcopy) (+4 nop pad)')]
DATA = [(0x800004F8, 0x800039E0, '.rodata'), (0x800039E0, 0x800039F0, '.data'), (0x800529A0, 0x80057280, '.data')]
TEXT_VRAM = [(0x80000400, 0x800004F8), (0x800039F0, 0x800529A0)]
BSS = (0x80057280, 0x800C7E30)


def v2r(v): return v - ROM2VRAM
def r2v(r): return r + ROM2VRAM
def in_owned(v): return any(a <= v < b for a, b, _ in DATA)
def in_text(v): return any(a <= v < b for a, b in TEXT_VRAM)
def is_ptrval(w): return 0x80000400 <= w < BSS[1]


# ---------------------------------------------------------------- C literal helpers
def c_escape(bs):
    out = []
    for b in bs:
        c = chr(b)
        if c == '"': out.append('\\"')
        elif c == '\\': out.append('\\\\')
        elif c == '?': out.append('\\?')
        elif c == '\n': out.append('\\n')
        elif c == '\t': out.append('\\t')
        elif 0x20 <= b < 0x7F: out.append(c)
        else: out.append('\\%03o' % b)
    return '"' + ''.join(out) + '"'


def c_unescape(lit):
    assert lit[0] == '"' and lit[-1] == '"'
    s = lit[1:-1]; out = bytearray(); i = 0
    simple = {'n': 10, 't': 9, '"': 34, '\\': 92, '?': 63, 'r': 13, '0': None}
    while i < len(s):
        c = s[i]
        if c != '\\':
            out.append(ord(c)); i += 1; continue
        n = s[i + 1]
        if n in '01234567':
            j = i + 1
            while j < len(s) and j < i + 4 and s[j] in '01234567': j += 1
            out.append(int(s[i + 1:j], 8)); i = j
        elif n in simple:
            out.append(simple[n]); i += 2
        else:
            raise ValueError("bad escape " + n)
    return bytes(out)


def f32_literal(bits):
    f = struct.unpack('>f', struct.pack('>I', bits))[0]
    e = (bits >> 23) & 0xFF
    if e == 0xFF or (e == 0 and bits & 0x7FFFFF): return None  # nan/inf/denormal
    lit = repr(f)
    if 'e' not in lit and '.' not in lit: lit += '.0'
    if struct.unpack('>I', struct.pack('>f', float(lit)))[0] != bits: return None
    return lit + 'f'


def f64_literal(bits):
    f = struct.unpack('>d', struct.pack('>Q', bits))[0]
    e = (bits >> 52) & 0x7FF
    if e == 0x7FF or (e == 0 and bits & ((1 << 52) - 1)): return None
    lit = repr(f)
    if 'e' not in lit and '.' not in lit: lit += '.0'
    if struct.unpack('>Q', struct.pack('>d', float(lit)))[0] != bits: return None
    return lit


# ---------------------------------------------------------------- analysis
def load_funcs():
    funcs, jtbls = {}, {}
    for line in open(os.path.join(PROJ, "analysis/survey/symbol_addrs.txt")):
        m = re.match(r'(\w+) = 0x([0-9A-Fa-f]+); // type:(\w+) size:0x([0-9A-Fa-f]+)', line)
        if not m: continue
        name, addr, typ, size = m.group(1), int(m.group(2), 16), m.group(3), int(m.group(4), 16)
        (funcs if typ == 'func' else jtbls)[addr] = (name, size)
    return funcs, jtbls


ACC = {'lw': 'w', 'sw': 'w', 'll': 'w', 'lh': 'h', 'lhu': 'h', 'sh': 'h', 'lb': 'b', 'lbu': 'b', 'sb': 'b',
       'lwc1': 'f32', 'swc1': 'f32', 'ldc1': 'f64', 'sdc1': 'f64', 'ld': 'd', 'sd': 'd',
       'lwl': 'u', 'lwr': 'u', 'swl': 'u', 'swr': 'u', 'lwc2': 'u', 'swc2': 'u'}
NOWRITE = {'sw', 'sh', 'sb', 'swc1', 'sdc1', 'sd', 'swl', 'swr', 'swc2', 'j', 'jal', 'jr', 'jalr', 'b',
           'beq', 'bne', 'beqz', 'bnez', 'blez', 'bgez', 'bltz', 'bgtz', 'beql', 'bnel', 'bc1f', 'bc1t',
           'mtc1', 'mtc0', 'mult', 'multu', 'div', 'divu', 'break', 'nop', '.word', 'cache', 'sync'}


def string_ok(bs):
    """one or more NUL-terminated printable strings followed by NUL padding."""
    if not bs or bs[0] == 0 or bs[-1] != 0: return False
    for b in bs:
        if not (b == 0 or b in (9, 10) or 0x20 <= b < 0x7F): return False
    # each string at least 1 printable char before NUL padding run; require mostly printable
    printable = sum(1 for b in bs if b)
    return printable >= 2 and printable * 2 >= len(bs) - 3




# ---------------------------------------------------------------- types
SIZES = {'u32': 4, 's32': 4, 'u16': 2, 's16': 2, 'u8': 1, 's8': 1, 'f32': 4, 'f64': 8, 'char': 1, 'ptr': 4}
PACK = {'u32': '>I', 's32': '>i', 'u16': '>H', 's16': '>h', 'u8': '>B', 's8': '>b', 'ptr': '>I'}
CNAME = {'ptr': 'void *', 'char': 'char'}
KW = {'w': 4, 'h': 2, 'b': 1, 'f32': 4, 'f64': 8}

VI_NAMES = ['Lpn1', 'Lpf1', 'Lan1', 'Laf1', 'Lpn2', 'Lpf2', 'Lan2', 'Laf2',
            'Hpn1', 'Hpf1', 'Han1', 'Haf1', 'Hpn2', 'Hpf2']
VI_FIELDS = (['type'] + ['comRegs.' + n for n in ('ctrl', 'width', 'burst', 'vSync', 'hSync', 'leap', 'hStart',
                                                  'xScale', 'vCurrent')] +
             ['fldRegs[%d].%s' % (i, n) for i in range(2) for n in ('origin', 'yScale', 'vStart', 'vBurst', 'vIntr')])


# RSP microcode data images (DMEM initial contents), identified by OSTask setup code
UCODE = {
    0x80054BC0: (0x800, 'D_80054BC0', 'F3DEX 1.23 RSP data image (contains "RSP Gfx ucode F3DEX 1.23" at +0x2B0); '
                 'stored to OSTask.ucode_data at 0x80013F90 with ucode_data_size 0x800'),
    0x800553C0: (0x2B0, 'D_800553C0', 'audio RSP ucode data image; stored to OSTask.ucode_data (sp+0x1C) at '
                 '0x80042DC4 next to ucode text 0x80037CF0'),
}


def fx_params_ok(b):
    if len(b) < 40 or len(b) % 4: return False
    ws = struct.unpack('>%di' % (len(b) // 4), b)
    return 1 <= ws[0] <= 8 and len(ws) >= 2 + 8 * ws[0] and not any(ws[2 + 8 * ws[0]:]) and 0 < ws[1] <= 0x10000 and \
        all(-0x10000 <= w <= 0x10000 for w in ws) and all(0 <= ws[2 + 8 * k] <= ws[1] and 0 <= ws[3 + 8 * k] <= ws[1]
                                                          for k in range(ws[0]))


def vi_mode_ok(b):
    if len(b) != 0x50 or b[0] >= 56 or b[1:4] != b'\0\0\0': return False
    ctrl, width = struct.unpack('>II', b[4:12])
    return ctrl < 0x10000 and (ctrl & 3) in (2, 3) and width in (0x140, 0x280, 0x500)


def vi_mode_name(t):
    return 'osViMode%s%s' % (['Ntsc', 'Pal', 'Mpal', 'Fpal'][t // 14], VI_NAMES[t % 14])


def infer_layout(s, size, stride, recs, rom):
    """Field layout from code accesses (rel offsets mod stride). Returns (fields, notes) or (None, reason)."""
    per = collections.defaultdict(list)
    for r in recs:
        if r['kind'] not in KW: continue
        rel = r['addr'] - s
        if rel < 0 or rel >= size: continue
        f = rel % stride
        if f % KW[r['kind']] or f + KW[r['kind']] > stride: continue
        per[f].append(r)
    if not per: return None, 'no load/store traced into the object'
    cands = []
    for f, rs in per.items():
        kinds = {r['kind'] for r in rs}
        w = max(KW[k] for k in kinds)
        if w == 8: ct = 'f64'
        elif w == 4: ct = 'f32' if kinds & {'w', 'f32'} == {'f32'} else 'u32'
        elif w == 2: ct = 's16' if any(r['signed'] == 1 for r in rs if r['kind'] == 'h') else 'u16'
        else: ct = 's8' if any(r['signed'] == 1 for r in rs if r['kind'] == 'b') else 'u8'
        use = {r['use'] for r in rs if r['use']}
        cands.append((f, w, ct, use, kinds))
    cands.sort(key=lambda c: (c[0], -c[1]))
    fields = []; cur = 0; notes = []
    for f, w, ct, use, kinds in cands:
        if f < cur:
            notes.append('dropped overlapping %s access at +0x%X' % (ct, f)); continue
        if f > cur: fields.append({'off': cur, 'ctype': 'u8', 'name': 'unk_%02X' % cur, 'n': f - cur, 'unk': True})
        n_el = size // stride
        vals = [rom[v2r(s + k * stride + f):v2r(s + k * stride + f) + w] for k in range(n_el)]
        if ct == 'u32':
            ws = [struct.unpack('>I', v)[0] for v in vals]
            ads = [s + k * stride + f for k in range(n_el)]
            if any(ws) and all(x == 0 or (is_ptrval(x) and ad in CW) for x, ad in zip(ws, ads)):
                ct = 'ptr'; notes.append('+0x%X pointer (use confirmed)' % f)
        if ct == 'f32' and not all(f32_literal(struct.unpack('>I', v)[0]) for v in vals):
            ct = 'u32'; notes.append('+0x%X f32 access but non-literal bits' % f)
        if ct == 'f64' and (not all(f64_literal(struct.unpack('>Q', v)[0]) for v in vals) or f % 8 or s % 8 or stride % 8):
            ct = 'u32'; w = 4; notes.append('+0x%X f64 kept as words' % f)
            fields.append({'off': f, 'ctype': 'u32', 'name': 'field_%02X' % f, 'n': 2}); cur = f + 8; continue
        fields.append({'off': f, 'ctype': ct, 'name': 'field_%02X' % f, 'n': 1})
        cur = f + w
    if cur < stride: fields.append({'off': cur, 'ctype': 'u8', 'name': 'unk_%02X' % cur, 'n': stride - cur, 'unk': True})
    align = max(SIZES[x['ctype']] for x in fields)
    if s % align or stride % align: return None, 'layout alignment %d does not fit address/stride' % align
    # u8 unk fills: widen to u32/u16 words where aligned, keeping explicit layout
    out = []
    for fl in fields:
        if fl.get('unk') and fl['off'] % 4 == 0 and fl['n'] % 4 == 0:
            fl = dict(fl, ctype='u32', n=fl['n'] // 4)
        elif fl.get('unk') and fl['off'] % 2 == 0 and fl['n'] % 2 == 0:
            fl = dict(fl, ctype='u16', n=fl['n'] // 2)
        out.append(fl)
    return out, notes


CW = set()          # data word addresses whose loaded value is used as a pointer
LOADED = set()      # data word addresses loaded by traced code
PTRSTAT = collections.Counter()


def word_val(addr, w):
    """u32 data word: relocation if its pointer use is confirmed, else integer."""
    if is_ptrval(w):
        if addr in CW:
            PTRSTAT['confirmed'] += 1; return {'reloc': w}
        PTRSTAT['demoted_loaded_no_deref' if addr in LOADED else 'demoted_never_loaded'] += 1
    return '0x%08X' % w


def field_values(s, stride, count, fields, rom, reloc_ok):
    els = []
    for k in range(count):
        el = []
        for fl in fields:
            sz = SIZES[fl['ctype']]
            vs = []
            for j in range(fl['n']):
                a = v2r(s + k * stride + fl['off'] + j * sz)
                b = rom[a:a + sz]
                ct = fl['ctype']
                if ct == 'f32': vs.append(f32_literal(struct.unpack('>I', b)[0]))
                elif ct == 'f64': vs.append(f64_literal(struct.unpack('>Q', b)[0]))
                elif ct == 'ptr':
                    w = struct.unpack('>I', b)[0]
                    vs.append({'reloc': w} if w else 0)
                elif ct == 'u32':
                    w = struct.unpack('>I', b)[0]
                    vs.append(word_val(s + k * stride + fl['off'] + j * sz, w))
                elif ct[0] == 's': vs.append(struct.unpack(PACK[ct], b)[0])
                else: vs.append('0x%0*X' % (sz * 2, struct.unpack(PACK[ct], b)[0]))
            el.append(vs if fl['n'] > 1 or fl.get('unk') else vs[0])
        els.append(el)
    return els


def generate():
    rom = open(ROM_PATH, 'rb').read()
    funcs, jtbls = load_funcs()
    fstarts = sorted(funcs)
    acc = data_access.accesses(FULLTEXT, funcs)
    func_of = lambda pc: fstarts[bisect.bisect_right(fstarts, pc) - 1]
    def word(v): return struct.unpack('>I', rom[v2r(v):v2r(v) + 4])[0]

    ptr_words = {}
    for a, b, _ in DATA:
        for v in range(a, b, 4):
            w = word(v)
            if is_ptrval(w): ptr_words[v] = w
    jt_spans = {a: a + s for a, (n, s) in jtbls.items()}
    import ptrflow
    fl = ptrflow.Flow(FULLTEXT, funcs, rom, v2r, in_owned, is_ptrval).run()
    CW.clear(); LOADED.clear(); PTRSTAT.clear()
    def table_words(X, S, span=None):
        # words at X + n*S while the table continues: a non-pointer word is tolerated only
        # while a pointer was seen within the last `span` bytes (composite index strides)
        span = max(span or S, S)
        out = []; last = X; k = 0
        while in_owned(X + k * S) and k < 2048:
            a = X + k * S; w = word(a)
            if w == 0 or is_ptrval(w):
                if is_ptrval(w): last = a
                out.append(a)
            elif span == S or a - last >= span:
                break
            k += 1
        return [a for a in out if a <= last]
    def prov_words(p):
        return [p[1]] if p[0] == 'w' else table_words(p[1], p[2], p[3] if len(p) > 3 else None)
    for p in fl.confirmed:
        if p[0] in ('w', 'x'): CW.update(prov_words(p))
    for p in fl.loaded:
        if p[0] in ('w', 'x'): LOADED.update(prov_words(p))
    extra = list(fl.records)
    for key, offs in fl.idx_deref.items():
        for wa in table_words(*key):
            t = word(wa)
            if not t or not in_owned(t): continue
            for off, kind, sg, indexed in offs:
                extra.append({'pc': -1, 'addr': t + off, 'kind': kind, 'op': 'via-table', 'signed': sg, 'stride': 0,
                              'arr': indexed,
                              'use': None, 'base': t, 'off': off, 'via': 'table 0x%08X' % key[0]})
    for r in acc:   # direct loads whose value is dereferenced (data_access lookahead)
        if r['op'] == 'lw' and in_owned(r['addr']):
            LOADED.add(r['addr'])
            if r['use']: CW.add(r['addr'])
    # text runs (NUL-terminated printable strings padded to 4): pointer-flow evidence is ignored inside them
    def pr(x): return x in (9, 10, 13) or 0x20 <= x < 0x7F
    text = set()
    for ra, rb, _ in DATA:
        v = ra
        while v < rb:
            if v % 4 or not pr(rom[v2r(v)]): v += 1; continue
            e = v; npr = 0
            while e < rb and pr(rom[v2r(e)]):
                k = e
                while k < rb and pr(rom[v2r(k)]): k += 1
                if k >= rb or rom[v2r(k)] != 0 or k - e < 2: break
                npr += k - e
                k += 1
                while k < rb and k % 4 and rom[v2r(k)] == 0: k += 1
                e = k
            if npr >= 4 and e > v: text.update(range(v, e)); v = e
            else: v += 4
    extra = [r for r in extra if r['addr'] not in text]
    formed = [r for r in extra if r.get('formed')]
    acc = acc + [r for r in extra if not r.get('formed')]
    own_acc = [r for r in acc if in_owned(r['addr']) or in_owned(r['base'])]
    formed_at = collections.defaultdict(list)
    for r in formed:
        if in_owned(r['addr']): formed_at[r['addr']].append(r)

    hard = set()
    for a, b, _ in DATA: hard |= {a, b}
    for a, e in jt_spans.items(): hard |= {a, e}
    soft = set()
    for r in own_acc:
        if in_owned(r['addr']): soft.add(r['addr'])
    for w in ptr_words.values():
        if in_owned(w): soft.add(w)

    # libultra OSViMode runs
    vi = []
    for a, b, _ in DATA:
        v = a
        while v + 0x50 <= b:
            if v % 4 == 0 and vi_mode_ok(rom[v2r(v):v2r(v) + 0x50]):
                vi.append(v); v += 0x50
            else: v += 4
    for v in vi: hard |= {v, v + 0x50}
    for v, (n, nm, note) in UCODE.items(): hard |= {v, v + n}
    vi_set = set(vi)

    # struct-array candidates: base -> Counter(stride)
    cand = collections.defaultdict(collections.Counter)
    evid = collections.defaultdict(set)
    groups = collections.defaultdict(list)
    for r in own_acc:
        st = abs(r['stride'] or 0)
        if st < 2 or r['kind'] == 'addr': continue
        if r['base'] & 0xFFFF and in_owned(r['base']):
            cand[r['base']][st] += 1; evid[r['base']].add(id(r))
        elif in_owned(r['addr']):
            groups[(func_of(r['pc']), st)].append(r)
    for (fn, st), rs in groups.items():
        lo = min(r['addr'] for r in rs)
        if max(r['addr'] for r in rs) - lo < st:
            cand[lo][st] += len(rs); evid[lo] |= {id(r) for r in rs}
    for r in own_acc:   # records relative to a candidate base (field accesses through its pointer)
        if r['base'] in cand: evid[r['base']].add(id(r))
    at = collections.defaultdict(list)
    for r in own_acc: at[r['addr']].append(r)
    data_targets = {w for w in ptr_words.values() if in_owned(w)}
    arrays = {}
    taken_until = 0
    allsplits = sorted(hard | soft)
    for B in sorted(cand):
        if B < taken_until or B in vi_set or any(a <= B < e for a, e in jt_spans.items()): continue
        S = cand[B].most_common(1)[0][0]
        if S > 0x1000 or (S % 4 and S != 2): continue
        region_end = next(b for a, b, _ in DATA if a <= B < b)
        end = B
        while end + S <= region_end:
            stop = False
            for p in allsplits:
                if p <= end or p >= end + S:
                    continue
                if p in hard or p in data_targets or any(id(r) not in evid[B] for r in at[p]):
                    stop = True; break
            if stop: break
            nx = end + S
            if nx < region_end and (nx in hard or nx in data_targets or any(id(r) not in evid[B] for r in at[nx])):
                end = nx; break
            end = nx
        if end - B >= 2 * S or (end - B >= S and len(evid[B]) >= 2):
            arrays[B] = (S, end)
            taken_until = end
    # splits: drop soft splits inside arrays
    splits = set(hard)
    for p in soft:
        inside = any(B < p < E for B, (S, E) in arrays.items()) or any(a < p < e for a, e in jt_spans.items()) or any(v < p < v + n for v, (n, a, b) in UCODE.items())
        if not inside: splits.add(p)
    for B, (S, E) in arrays.items(): splits |= {B, E}
    splits = sorted(p for p in splits if p % 4 == 0 or p in hard)

    jt_names = {a: n for a, (n, s) in jtbls.items()}
    func_name = {a: n for a, (n, s) in funcs.items()}
    objs = []
    for ra, rb, sec in DATA:
        pts = [p for p in splits if ra <= p <= rb]
        for s, e in zip(pts, pts[1:]):
            objs.append((s, e, sec))

    out_objs = []
    for s, e, sec in objs:
        size = e - s
        data = rom[v2r(s):v2r(e)]
        recs = [r for r in own_acc if s <= r['addr'] < e or r['base'] == s]
        kinds = {r['kind'] for r in recs if r['addr'] == s}
        o = {'vram': s, 'rom': v2r(s), 'size': size, 'section': sec, 'name': 'D_%08X' % s}
        if s in UCODE and UCODE[s][0] == size:
            o.update(name=UCODE[s][1], rep='ucode-data', ctype='u32', note=UCODE[s][2],
                     values=['0x%08X' % x for x in struct.unpack('>%dI' % (size // 4), data)])
            out_objs.append(o); continue
        if fx_params_ok(data) and not [r for r in recs if r['kind'] in ('h', 'b', 'f32', 'f64')]:
            n = struct.unpack('>i', data[:4])[0]
            o.update(rep='typed-array', ctype='s32', values=list(struct.unpack('>%di' % (size // 4), data)),
                     note='libaudio ALFx reverb parameter list: s32 {sectionCount=%d, length, then per section input, '
                          'output, fbcoef, ffcoef, gain, chorusRate, chorusDepth, lowpass}; layout proven by size == '
                          '(2 + 8*sectionCount) words%s' % (n, '' if size == 4 * (2 + 8 * n) else
                          ', followed by %d zero words' % (size // 4 - 2 - 8 * n)))
            out_objs.append(o); continue
        if s in vi_set and size == 0x50:
            fl = [{'off': 0, 'ctype': 'u8', 'name': 'type', 'n': 1}, {'off': 1, 'ctype': 'u8', 'name': 'pad', 'n': 3, 'implicit': True}]
            fl += [{'off': 4 + 4 * i, 'ctype': 'u32', 'name': n, 'n': 1} for i, n in enumerate(VI_FIELDS[1:])]
            nm = vi_mode_name(data[0])
            if any(x['name'] == nm for x in out_objs): nm = 'D_%08X' % s
            o.update(name=nm, rep='struct', stype='OSViMode', stride=0x50, count=1, fields=fl,
                     values=field_values(s, 0x50, 1, fl, rom, False),
                     note='libultra OSViMode (type %d); identified by layout: type byte, zero pad, VI ctrl/width' % data[0])
            out_objs.append(o); continue
        if s in jt_names and jt_spans[s] == e and all(in_text(word(s + i)) for i in range(0, size, 4)):
            o.update(name=jt_names[s], rep='jtbl', ctype='u32', values=[{'reloc': word(s + i)} for i in range(0, size, 4)])
            out_objs.append(o); continue
        if size >= 2 and not recs and s not in ptr_words.values() and data == bytes(size) and \
                (e % 8 == 0 or any(e == b for a, b, _ in DATA)):
            o.update(name='pad_%08X' % s, rep='padding', ctype='u8', values=[0] * size); out_objs.append(o); continue
        if string_ok(data) and not ({r['kind'] for r in recs} & {'f32', 'f64', 'w', 'h', 'd'}):
            o.update(rep='string', ctype='char', literal=c_escape(data),
                     note='text; byte accesses only' if recs else '')
            out_objs.append(o); continue
        if s in arrays:
            S, E = arrays[s]
            fields, notes = infer_layout(s, size, S, recs, rom)
            if fields:
                single = len(fields) == 1 and fields[0]['n'] == 1 and not fields[0].get('unk')
                ct = fields[0]['ctype'] if single else None
                if single and ct in ('f32', 'f64'):
                    o.update(rep=ct, ctype=ct, values=[x[0] for x in field_values(s, S, size // S, fields, rom, True)])
                elif single and ct != 'ptr':
                    o.update(rep='typed-array', ctype=ct, values=[x[0] for x in field_values(s, S, size // S, fields, rom, True)],
                             note='element type from %d traced accesses, stride %d' % (len(recs), S))
                elif single and ct == 'ptr':
                    o.update(rep='pointer', ctype='u32', values=[x[0] for x in field_values(s, S, size // S, fields, rom, True)],
                             note='pointer use confirmed: ' + '; '.join(notes))
                else:
                    o.update(rep='struct', stype='Struct_%08X' % s, stride=S, count=size // S, fields=fields,
                             values=field_values(s, S, size // S, fields, rom, True),
                             note='stride 0x%X from traced loop/index accesses; %s' % (S, '; '.join(notes) or 'no conflicts'))
                if 'rep' in o:
                    out_objs.append(o); continue
        if 'f64' in kinds and s % 8 == 0 and size % 8 == 0:
            lits = [f64_literal(x) for x in struct.unpack('>%dQ' % (size // 8), data)]
            if all(lits):
                o.update(rep='f64', ctype='f64', values=lits); out_objs.append(o); continue
        if 'f32' in kinds and size % 4 == 0:
            lits = [f32_literal(x) for x in struct.unpack('>%dI' % (size // 4), data)]
            if all(lits) and not any(s + 4 * i in ptr_words for i in range(size // 4)):
                o.update(rep='f32', ctype='f32', values=lits); out_objs.append(o); continue
        if string_ok(data) and not (kinds & {'f32', 'f64', 'w', 'h', 'd'}):
            o.update(rep='string', ctype='char', literal=c_escape(data)); out_objs.append(o); continue
        if not [r for r in recs if r['kind'] in KW]:
            # addresses formed in code and carried across join points (only used for objects
            # that have no other access evidence; they never create object boundaries)
            fr = [r for a in range(s, e) for r in formed_at.get(a, []) if r['kind'] in KW]
            if fr:
                recs = recs + fr
                o['note'] = 'accesses through an address formed in code and carried across a join point'
        # indexed accesses through a loaded/passed pointer: element array
        arr = [r for r in recs if r.get('arr') and r['kind'] in KW]
        kw = {r['kind'] for r in recs if r['kind'] in KW}
        if arr and len(kw) == 1 and size % KW[kw.pop()] == 0:
            k = arr[0]['kind']; w = KW[k]
            ct = {'b': 'u8', 'h': 'u16', 'w': 'u32'}.get(k)
            if ct and s % w == 0:
                if any(r['signed'] == 1 for r in recs if r['kind'] == k) and ct != 'u32': ct = 's' + ct[1:]
                vals = []
                for i in range(0, size, w):
                    x = struct.unpack(PACK[ct], data[i:i + w])[0]
                    vals.append(word_val(s + i, x) if w == 4 else (x if ct[0] == 's' else '0x%0*X' % (2 * w, x)))
                o.update(rep='typed-array', ctype=ct, values=vals,
                         note='%s elements: indexed accesses through a pointer loaded from data / passed in a0-a3 (%s)' % (
                             ct, ', '.join(sorted({r.get('via', 'code') for r in arr}))[:80]))
                out_objs.append(o); continue
        # single (non-array) object: element/field types from direct accesses
        if recs and size <= 0x1000:
            fields, notes = infer_layout(s, size, size, recs, rom)
            if fields:
                kinds_f = [f for f in fields if not f.get('unk')]
                elem = kinds_f[0]['ctype'] if kinds_f else None
                if len(fields) == 1 and fields[0]['n'] == 1:
                    if elem == 'ptr':
                        o.update(rep='pointer', ctype='u32', values=[{'reloc': word(s)} if word(s) else '0x00000000'],
                                 note='pointer use confirmed: ' + '; '.join(notes))
                    elif elem in ('f32', 'f64'):
                        o.update(rep=elem, ctype=elem, values=field_values(s, size, 1, fields, rom, True)[0])
                    else:
                        o.update(rep='typed-array', ctype=elem, values=field_values(s, size, 1, fields, rom, True)[0],
                                 note='scalar type from %d traced accesses' % len(recs))
                    out_objs.append(o); continue
                # homogeneous accesses of one width: array of that element type
                ws = {SIZES[f['ctype']] for f in kinds_f}
                if len(ws) == 1 and len({f['ctype'] for f in kinds_f}) == 1 and elem not in ('ptr',):
                    w = ws.pop(); ct = elem
                    if size % w == 0 and s % w == 0 and ct not in ('f32', 'f64'):
                        fmt = PACK[ct]
                        vals = []
                        for i in range(0, size, w):
                            x = struct.unpack(fmt, data[i:i + w])[0]
                            if w == 4: vals.append(word_val(s + i, x))
                            elif ct[0] == 's': vals.append(x)
                            else: vals.append('0x%0*X' % (2 * w, x))
                        o.update(rep='typed-array', ctype=ct, values=vals,
                                 note='element %s from %d traced accesses at %d offsets' % (ct, len(recs), len(kinds_f)))
                        out_objs.append(o); continue
                o.update(rep='struct', stype='Struct_%08X' % s, stride=size, count=1, fields=fields,
                         values=field_values(s, size, 1, fields, rom, True),
                         note='single struct from direct accesses; ' + ('; '.join(notes) or 'no conflicts'))
                out_objs.append(o); continue
            reason = notes
        elif not recs:
            reason = 'no lui/lo-traced code access (reached only through pointers held in data/registers)'
        else:
            reason = 'too large for a single-struct layout without stride evidence'
        if not [r for r in recs if r['kind'] in KW] and recs:
            reason = 'address taken only; contents read through computed pointers the tracer does not follow'
        vals = []; nrel = 0
        if size % 4 == 0:
            for i in range(0, size, 4):
                vals.append(word_val(s + i, word(s + i)))
                nrel += isinstance(vals[-1], dict)
            pure = nrel and all(isinstance(v, dict) or v == '0x00000000' for v in vals)
            if pure:
                o.update(rep='pointer', ctype='u32', values=vals, note='all words pointers with confirmed use')
            else:
                o.update(rep='typed-raw', ctype='u32', values=vals, reason=reason)
                if nrel: o['note'] = '%d words are use-confirmed pointers' % nrel
        else:
            o.update(rep='typed-raw', ctype='u8', values=['0x%02X' % x for x in data], reason=reason)
        out_objs.append(o)

    # post-pass on remaining typed-raw objects
    starts_l = [o['vram'] for o in out_objs]
    def obj_at(v):
        i = bisect.bisect_right(starts_l, v) - 1
        return out_objs[i] if i >= 0 and v < out_objs[i]['vram'] + out_objs[i]['size'] else None
    ref_pcs = collections.defaultdict(set)
    for r in own_acc:
        if r['pc'] >= 0: ref_pcs[r['addr']].add(func_of(r['pc']))
    def owner_near(o):
        i = starts_l.index(o['vram']); names = []
        for j in (i - 1, i + 1):
            if 0 <= j < len(out_objs):
                q = out_objs[j]
                fs = {f for a in range(q['vram'], q['vram'] + q['size']) for f in ref_pcs.get(a, ())}
                names += [func_name[f] for f in sorted(fs)][:2]
        return ', '.join(sorted(set(names))) or 'unknown'
    for o in out_objs:
        if o['rep'] != 'typed-raw': continue
        s, size = o['vram'], o['size']; e = s + size
        data = rom[v2r(s):v2r(e)]
        # (A) derived pointer: code forms s + d (d a multiple of a neighbouring struct's stride) and accesses it
        done = False
        for r in own_acc:
            if r['kind'] != 'addr' or r['base'] != s or r['addr'] == s: continue
            n = obj_at(r['addr'])
            if not n or n['rep'] != 'struct' or n.get('stype') == 'OSViMode': continue
            S = n['stride']; d = r['addr'] - s
            if d % S or size % S or s % max(SIZES[f['ctype']] for f in n['fields']): continue
            # pointer fields keep pointer type only when every element's word is use-confirmed in place
            flds = [dict(f, ctype='u32') if f['ctype'] == 'ptr' and not all(
                word(s + k * S + f['off']) == 0 or (s + k * S + f['off']) in CW for k in range(size // S))
                else dict(f) for f in n['fields']]
            o.pop('reason', None)
            o.update(rep='struct', stype='Struct_%08X' % s, stride=S, count=size // S, fields=flds,
                     values=field_values(s, S, size // S, flds, rom, True),
                     note='same layout as %s (stride 0x%X): code at 0x%08X forms 0x%08X + 0x%X = element %d of one '
                          'array spanning both objects' % (n['name'], S, r['pc'], s, d, d // S))
            done = True; break
        if not done:
            # (A') address formed from another object's base: same element type when the distance is a
            # whole number of that object's elements
            for r in own_acc:
                if r['kind'] != 'addr' or not (s <= r['addr'] < e) or r['base'] == s or not in_owned(r['base']):
                    continue
                n = obj_at(r['base'])
                if not n or n is o: continue
                d = r['addr'] - r['base']
                if n['rep'] == 'typed-array' and n['ctype'] in PACK:
                    w = SIZES[n['ctype']]; ct = n['ctype']
                    if d % w or size % w or s % w: continue
                    vals = []
                    for i in range(0, size, w):
                        x = struct.unpack(PACK[ct], data[i:i + w])[0]
                        vals.append(word_val(s + i, x) if w == 4 else (x if ct[0] == 's' else '0x%0*X' % (2 * w, x)))
                    o.pop('reason', None)
                    o.update(rep='typed-array', ctype=ct, values=vals,
                             note='%s elements: code at 0x%08X forms this address as %s %+d (whole elements)' % (
                                 ct, r['pc'], n['name'], d))
                    done = True; break
                if n['rep'] == 'struct' and n.get('stype') != 'OSViMode':
                    S = n['stride']
                    if (r['addr'] - n['vram']) % S or size % S or s % max(SIZES[f['ctype']] for f in n['fields']):
                        continue
                    flds = [dict(f, ctype='u32') if f['ctype'] == 'ptr' and not all(
                        word(s + k * S + f['off']) == 0 or (s + k * S + f['off']) in CW for k in range(size // S))
                        else dict(f) for f in n['fields']]
                    o.pop('reason', None)
                    o.update(rep='struct', stype='Struct_%08X' % s, stride=S, count=size // S, fields=flds,
                             values=field_values(s, S, size // S, flds, rom, True),
                             note='same layout as %s: code at 0x%08X forms this address as its base %+d' % (
                                 n['name'], r['pc'], d))
                    done = True; break
        if not done:
            why = []
            for r in own_acc:
                if r['kind'] == 'addr' and s <= r['addr'] < e and r['base'] != s and in_owned(r['base']):
                    n = obj_at(r['base'])
                    if n: why.append('address formed at 0x%08X as %s%+d (%s %s); the distance/size is not a whole '
                                     'element of that layout and the derived pointer is not dereferenced in traced code'
                                     % (r['pc'], n['name'], r['addr'] - r['base'], n['rep'], n.get('stype', n.get('ctype'))))
            if why: o['reason'] = why[0]
            elif not [r for r in own_acc if s <= r['addr'] < e] and \
                    not any(s <= w < e for w in ptr_words.values()):
                prev = obj_at(s - 1)
                fl = ', '.join(repr(struct.unpack('>f', data[i:i + 4])[0]) for i in range(0, size - size % 4, 4))
                o['reason'] = ('no reference anywhere: no data word points into it and no lui/lo pair or immediate in the '
                               'program text forms its address; as f32 the words read (%s); possibly trailing element(s) '
                               'of %s, not provable' % (fl, prev['name'] if prev else '-'))
        if done or o['section'] != '.rodata': continue
        # (B) unreferenced .rodata items typed by value, with the neighbouring constant-pool owner
        own = owner_near(o)
        if not any(data):
            o.pop('reason'); o.update(rep='padding', ctype='u8', values=[0] * size, name='pad_%08X' % s,
                                      note='zero gap between literals of %s (alignment padding or an unused zero/empty-string literal)' % own)
        elif size == 4 and f32_literal(word(s)) and (word(s) >> 23) & 0xFF not in (0,):
            o.pop('reason'); o.update(rep='f32', ctype='f32', values=[f32_literal(word(s))],
                                      note='unreferenced f32 literal by value; constant pool of %s' % own)
        elif sum(1 for x in data if 0x20 <= x < 0x7F) * 2 >= len(data.rstrip(b'\0')):
            o.pop('reason'); o.update(rep='string', ctype='char', literal=c_escape(data),
                                      note='char data by value (text bytes, not NUL-terminated); constant pool of %s' % own)
        else:
            o['reason'] = 'unreferenced; values are neither text, f32/f64 literals nor zero (%s); constant pool of %s' % (
                data.hex(), own)

    # relocation targets
    obj_starts = sorted(o['vram'] for o in out_objs)
    by_start = {o['vram']: o for o in out_objs}
    externs = {}
    def target(v):
        if in_owned(v):
            i = bisect.bisect_right(obj_starts, v) - 1
            base = obj_starts[i]; return by_start[base]['name'], v - base
        if in_text(v):
            i = bisect.bisect_right(fstarts, v) - 1
            base = fstarts[i]; nm = func_name[base]; externs[nm] = base; return nm, v - base
        nm = 'D_%08X' % v; externs[nm] = v; return nm, 0
    def fix(val):
        if isinstance(val, dict) and 'reloc' in val:
            nm, off = target(val['reloc']); val.update(sym=nm, off=off, reloc='0x%08X' % val['reloc'])
        elif isinstance(val, list):
            for x in val: fix(x)
    for o in out_objs:
        fix(o.get('values', []))
        o['vram'] = '0x%08X' % o['vram']; o['rom'] = '0x%X' % o['rom']

    files = []; cur = None
    region_starts = {a for a, b, _ in DATA}
    for o in out_objs:
        v = int(o['vram'], 16)
        if cur is None or cur['section'] != o['section'] or v in region_starts or \
                (v - int(cur['vram'], 16) >= 0x800 and v % 16 == 0 and o['rep'] != 'padding'):
            cur = {'section': o['section'], 'vram': o['vram'], 'objects': []}; files.append(cur)
        cur['objects'].append(o)
    os.makedirs(SRC, exist_ok=True)
    for f in glob.glob(os.path.join(SRC, '*')): os.remove(f)
    json.dump({'externs': {k: '0x%08X' % v for k, v in sorted(externs.items())},
               'objects': {o['name']: o['vram'] for o in out_objs}},
              open(os.path.join(SRC, 'symbols.json'), 'w'), indent=1)
    write_headers(out_objs, externs)
    for f in files:
        base = '%s_%s' % (f['section'].lstrip('.'), f['vram'][2:])
        last = f['objects'][-1]
        f['end'] = '0x%08X' % (int(last['vram'], 16) + last['size'])
        f['size'] = int(f['end'], 16) - int(f['vram'], 16)
        f['c_file'] = base + '.c'
        json.dump(f, open(os.path.join(SRC, base + '.json'), 'w'), indent=1)
        open(os.path.join(SRC, base + '.c'), 'w').write(emit_c(f))
    json.dump(dict(PTRSTAT), open(os.path.join(SRC, 'pointer_stats.json'), 'w'))
    print('pointer words:', dict(PTRSTAT), 'flow records', len(extra))
    print('objects', len(out_objs), 'files', len(files), 'struct arrays', len(arrays), 'vi modes', len(vi))


def elem_count(o):
    return o['size'] // SIZES[o['ctype']]


def decl(o):
    q = 'const ' if o['section'] == '.rodata' else ''
    if o['rep'] == 'struct':
        if o['stype'] == 'OSViMode': return '%sOSViMode %s' % (q, o['name'])
        return '%s%s %s[%d]' % (q, o['stype'], o['name'], o['count'])
    return '%s%s %s[%d]' % (q, CNAME.get(o['ctype'], o['ctype']), o['name'], elem_count(o))


def struct_typedef(o):
    L = ['typedef struct %s { /* size 0x%X */' % (o['stype'], o['stride'])]
    for fl in o['fields']:
        t = CNAME.get(fl['ctype'], fl['ctype'] + ' ')
        if not t.endswith(' ') and not t.endswith('*'): t += ' '
        arr = '[%d]' % fl['n'] if fl['n'] > 1 or fl.get('unk') else ''
        L.append('    /* 0x%02X */ %s%s%s;' % (fl['off'], t, fl['name'], arr))
    L.append('} %s;' % o['stype'])
    return '\n'.join(L)


OSVI_TYPEDEF = '''#ifndef _OS_VI_H_ /* libultra layout (os_vi.h) */
typedef struct { u32 ctrl, width, burst, vSync, hSync, leap, hStart, xScale, vCurrent; } OSViCommonRegs;
typedef struct { u32 origin, yScale, vStart, vBurst, vIntr; } OSViFieldRegs;
typedef struct { u8 type; OSViCommonRegs comRegs; OSViFieldRegs fldRegs[2]; } OSViMode;
#endif'''


def write_headers(objs, externs):
    h = ['/* Initialized program data: object declarations (generated by tools/datasrc.py) */',
         '#ifndef INITIALIZED_DATA_H', '#define INITIALIZED_DATA_H', '#include "data_types.h"', '', OSVI_TYPEDEF, '']
    for o in objs:
        if o['rep'] == 'struct' and o['stype'] != 'OSViMode':
            h += [struct_typedef(o), '']
    for o in objs:
        h.append('extern %s; /* %s %s %s */' % (decl(o), o['vram'], o['section'], o['rep']))
    h += ['', '#endif', '']
    open(os.path.join(SRC, 'initialized_data.h'), 'w').write('\n'.join(h))
    t = ['#ifndef DATA_TYPES_H', '#define DATA_TYPES_H', 'typedef unsigned int u32;', 'typedef int s32;',
         'typedef unsigned short u16;', 'typedef short s16;', 'typedef unsigned char u8;', 'typedef signed char s8;',
         'typedef float f32;', 'typedef double f64;', '',
         '/* code labels, BSS and foreign-block symbols referenced by initialized data */']
    for k, v in sorted(externs.items(), key=lambda kv: kv[1]):
        t.append('extern char %s[]; /* 0x%08X */' % (k, v))
    t += ['#endif', '']
    open(os.path.join(SRC, 'data_types.h'), 'w').write('\n'.join(t))


def c_val(v, ct='u32'):
    if isinstance(v, dict):
        cast = '(void *)' if ct == 'ptr' else '(u32)'
        if v['off']: return '%s((const char *)%s + 0x%X)' % (cast, v['sym'], v['off'])
        return '%s%s' % (cast, v['sym'])
    if isinstance(v, list): return '{ ' + ', '.join(c_val(x, ct) for x in v) + ' }'
    return str(v)


def emit_c(f):
    L = ['/* %s slice VRAM %s..%s (ROM 0x%X..0x%X); generated by tools/datasrc.py */' % (
        f['section'], f['vram'], f['end'], v2r(int(f['vram'], 16)), v2r(int(f['end'], 16))),
        '/* placement: section %s at VRAM %s, size 0x%X; compile: kmc gcc -G0 -O2 -mips2 */' % (
        f['section'], f['vram'], f['size']), '#include "initialized_data.h"', '']
    for o in f['objects']:
        L.append('/* %s ROM %s size 0x%X %s%s%s */' % (o['vram'], o['rom'], o['size'], o['rep'],
                 (' : ' + o['note']) if o.get('note') else '', (' ; untyped: ' + o['reason']) if o.get('reason') else ''))
        if o['rep'] == 'string':
            L.append('%s = %s;' % (decl(o), o['literal']))
        elif o['rep'] == 'struct' and o['stype'] == 'OSViMode':
            v = o['values'][0]
            L.append('%s = {\n    %s, /* type */\n    { %s },\n    { { %s },\n      { %s } },\n};' % (
                decl(o), v[0], ', '.join(v[2:11]), ', '.join(v[11:16]), ', '.join(v[16:21])))
        elif o['rep'] == 'struct':
            rows = []
            for el in o['values']:
                rows.append('{ ' + ', '.join(c_val(x, fl['ctype']) for x, fl in zip(el, o['fields'])) + ' }')
            L.append('%s = {\n    %s,\n};' % (decl(o), ',\n    '.join(rows)))
        else:
            ct = o['ctype']
            vals = [c_val(v, ct) for v in o['values']]
            per = 4 if any(isinstance(v, dict) for v in o['values']) or ct in ('f32', 'f64') else 8
            if ct in ('u8', 's8'): per = 16
            rows = [', '.join(vals[i:i + per]) for i in range(0, len(vals), per)]
            L.append('%s = {\n    %s,\n};' % (decl(o), ',\n    '.join(rows)))
        L.append('')
    return '\n'.join(L)


# ---------------------------------------------------------------- materializer
def load_units():
    syms = json.load(open(os.path.join(SRC, 'symbols.json')))
    table = {k: int(v, 16) for k, v in syms['externs'].items()}
    table.update({k: int(v, 16) for k, v in syms['objects'].items()})
    units = [json.load(open(p)) for p in sorted(glob.glob(os.path.join(SRC, '*.json'))) if not p.endswith(('symbols.json', 'pointer_stats.json'))]
    return units, table


def pack_val(v, ct, table):
    if isinstance(v, dict): return struct.pack('>I', (table[v['sym']] + v['off']) & 0xFFFFFFFF)
    if ct == 'f32': return struct.pack('>f', float(v[:-1]))
    if ct == 'f64': return struct.pack('>d', float(v))
    if ct == 'char': return struct.pack('>B', v)
    x = int(v, 0) if isinstance(v, str) else v
    return struct.pack(PACK[ct], x)


def materialize_obj(o, table):
    if o['rep'] == 'string':
        b = c_unescape(o['literal']); return b + bytes(o['size'] - len(b))
    out = bytearray()
    if o['rep'] == 'struct':
        for el in o['values']:
            base = len(out)
            for fl, v in zip(o['fields'], el):
                while len(out) < base + fl['off']: out.append(0)   # implicit C padding (zero)
                for x in (v if isinstance(v, list) else [v]):
                    out += pack_val(x, fl['ctype'], table)
            while len(out) < base + o['stride']: out.append(0)
        return bytes(out)
    for v in o['values']: out += pack_val(v, o['ctype'], table)
    return bytes(out)


def assemble(src, td):
    tc = TOOLCHAIN
    env = dict(os.environ, LD_LIBRARY_PATH=tc + '/mips-binutils/usr/lib/x86_64-linux-gnu')
    B = tc + '/mips-binutils/usr/bin/mips-linux-gnu-'
    o = os.path.join(td, os.path.basename(src) + '.o'); bn = o + '.bin'
    subprocess.run([B + 'as', '-EB', '-mips2', '-mabi=32', '-mno-pdr', '-o', o, src], check=True, capture_output=True, env=env)
    subprocess.run([B + 'objcopy', '-O', 'binary', '-j', '.text', o, bn], check=True, capture_output=True, env=env)
    return open(bn, 'rb').read()


def verify_code(rom):
    rows = []
    with tempfile.TemporaryDirectory() as td:
        for a, b, src, owner in CODE:
            try:
                got = assemble(os.path.join(ASM, src), td)
                n = b - a
                ok = got[:n] == rom[v2r(a):v2r(b)] and not any(got[n:])
            except (subprocess.CalledProcessError, FileNotFoundError):
                ok = False
            rows.append((a, b, src, owner, ok))
    return rows


def verify(rom):
    units, table = load_units()
    res = []
    for u in units:
        for o in u['objects']:
            b = materialize_obj(o, table)
            r = int(o['rom'], 16)
            res.append((o, u, b == rom[r:r + o['size']] and len(b) == o['size']))
    return res

# ---------------------------------------------------------------- compile check
def compile_units(rom, units=None):
    tc = TOOLCHAIN
    env = dict(os.environ, LD_LIBRARY_PATH=tc + '/mips-binutils/usr/lib/x86_64-linux-gnu')
    B = tc + '/mips-binutils/usr/bin/mips-linux-gnu-'
    allu, table = load_units()
    if units is None: units = allu
    results = {}
    with tempfile.TemporaryDirectory() as td:
        for u in units:
            base = os.path.splitext(u['c_file'])[0]
            o = os.path.join(td, base + '.o'); elf = os.path.join(td, base + '.elf'); bn = os.path.join(td, base + '.bin')
            sec = '.text' if u['section'] == '.text' else u['section']
            try:
                subprocess.run([tc + '/kmc-gcc-2.7.2/gcc', '-B' + tc + '/kmc-gcc-2.7.2/', '-G0', '-O2', '-mips2',
                                '-I' + SRC, '-c', os.path.join(SRC, u['c_file']), '-o', o],
                               check=True, capture_output=True, env=env)
                # the slice starts at its ROM address, which may be only 4-aligned; the
                # object's section alignment (2**4) is lowered to 4 as a matching build would
                subprocess.run([B + 'objcopy', '--set-section-alignment', sec + '=4', o], check=True,
                               capture_output=True, env=env)
                defined = {ob['name'] for ob in u['objects']}
                ld = ['SECTIONS { %s %s : { %s(%s) } /DISCARD/ : { *(*) } }' % (sec, u['vram'], o, sec)]
                ld += ['%s = %s;' % (k, '0x%08X' % v) for k, v in table.items() if k not in defined]
                lds = os.path.join(td, base + '.ld'); open(lds, 'w').write('\n'.join(ld) + '\n')
                subprocess.run([B + 'ld', '-T', lds, '-o', elf, o], check=True, capture_output=True, env=env)
                subprocess.run([B + 'objcopy', '-O', 'binary', '-j', sec, elf, bn], check=True, capture_output=True, env=env)
                got = open(bn, 'rb').read()
                r = v2r(int(u['vram'], 16))
                n = u['size']
                ok = got[:n] == rom[r:r + n] and (len(got) == n or (len(got) - n < 16 and not any(got[n:])))
                results[u['c_file']] = (ok, len(got), n)
            except subprocess.CalledProcessError as ex:
                results[u['c_file']] = (False, 0, u['size'], (ex.stderr or b'').decode()[-400:])
    return results


# ---------------------------------------------------------------- roundtrip API


# ---------------------------------------------------------------- roundtrip API
def roundtrip(rom: bytes) -> list:
    rows = []
    for a, b, src, owner, ok in (verify_code(rom) if os.path.isdir(ASM) else []):
        # code rows only when the local assembly evidence is present (it is not part of the integration package)
        rows.append({'start': v2r(a), 'end': v2r(b), 'category': 'program_code', 'subtype': '.text',
                     'owner': 'code_decompilation (asm evidence only; not in the integration patch)', 'format': 'mips asm', 'extraction': 'src/asm/' + src,
                     'rebuild': 'GNU as -mips2 (src/asm/%s); %s' % (src, owner), 'verified': ok,
                     'evidence': 'assembled bytes == ROM' if ok else 'mismatch'})
    for o, u, ok in verify(rom):
        r = int(o['rom'], 16)
        rows.append({'start': r, 'end': r + o['size'], 'category': 'program_data', 'subtype': o['section'],
                     'owner': 'initialized_data', 'format': o['rep'] + ':' + o.get('stype', o.get('ctype', '')),
                     'extraction': 'src/data/' + os.path.splitext(u['c_file'])[0] + '.json',
                     'rebuild': 'tools/datasrc.py materializer (src/data/' + u['c_file'] + ')',
                     'verified': ok, 'evidence': 'materialized bytes == ROM' if ok else 'mismatch'})
    return sorted(rows, key=lambda r: r['start'])


def unk_bytes(o):
    if o['rep'] != 'struct': return 0
    return o['count'] * sum(SIZES[f['ctype']] * f['n'] for f in o['fields'] if f.get('unk'))


def stats(rom, comp=None):
    reps = collections.Counter(); unk = 0; ptrc = collections.Counter()
    for o, u, ok in verify(rom):
        reps[o['rep']] += o['size']; unk += unk_bytes(o)
    return reps, unk


def write_inventory(rom, comp):
    p = os.path.join(ROOT, 'inventory', 'initialized_data.tsv')
    os.makedirs(os.path.dirname(p), exist_ok=True)
    with open(p, 'w') as f:
        f.write('rom_start\tvram\tsize\tsection\tsymbol\ttype\trepresentation\tverified\tsource\tnotes\n')
        for a, b, src, owner, ok in verify_code(rom):
            f.write('0x%X\t0x%08X\t%d\t.text\t%s\tasm\tcode\t%s\tsrc/asm/%s\t%s; NOT DATA: owned by the code '
                    'decompilation effort; local evidence only, excluded from the integration patch\n' % (
                v2r(a), a, b - a, src.split('_')[-1][:-2], 'assembled' if ok else 'no', src, owner))
        for o, u, ok in verify(rom):
            c = comp.get(u['c_file'])
            how = 'compiled+materialized' if (ok and c and c[0]) else ('materialized' if ok else 'no')
            note = o.get('note', '') + ((' | untyped: ' + o['reason']) if o.get('reason') else '')
            f.write('%s\t%s\t%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' % (o['rom'], o['vram'], o['size'], o['section'], o['name'],
                    o.get('stype', o.get('ctype')), o['rep'], how, 'src/data/' + u['c_file'], note.replace('\t', ' ')))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['generate', 'verify', 'compile', 'inventory', 'all'])
    ap.add_argument('--rom', default=ROM_PATH)
    a = ap.parse_args()
    rom = open(a.rom, 'rb').read()
    if a.cmd in ('generate', 'all'): generate()
    if a.cmd in ('verify', 'all', 'inventory'):
        rows = roundtrip(rom)
        code = [r for r in rows if r['category'] == 'program_code']
        data = [r for r in rows if r['category'] == 'program_data']
        sz = lambda rs: sum(r['end'] - r['start'] for r in rs)
        print('code: %d/%d bytes assembled exact; data: %d/%d bytes materialized exact; total %d' % (
            sz([r for r in code if r['verified']]), sz(code), sz([r for r in data if r['verified']]), sz(data), sz(rows)))
        reps, unk = stats(rom)
        print('data by representation:', dict(reps), '| unk bytes inside structs:', unk)
    if a.cmd in ('compile', 'all', 'inventory'):
        comp = compile_units(rom)
        okb = sum(v[2] for v in comp.values() if v[0]); tot = sum(v[2] for v in comp.values())
        print('compiled: %d/%d data bytes reproduced by KMC GCC, %d/%d files' % (okb, tot, sum(1 for v in comp.values() if v[0]), len(comp)))
        for k, v in comp.items():
            if not v[0]: print('  FAIL', k, v)
        if a.cmd in ('inventory', 'all'): write_inventory(rom, comp)


if __name__ == '__main__':
    main()
