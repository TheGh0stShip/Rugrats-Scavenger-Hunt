#!/usr/bin/env python3
"""BSS layout builder/validator for VRAM 0x80057280..0x800C7E30.

scan:     derive referenced symbols from the text disassembly (lui/addiu,
          lui/load/store, lui/addu-index/load/store pairs) and from 32-bit
          words in .rodata/.data that point into BSS; size every symbol up to
          the next referenced address; emit inventory/bss_layout.tsv,
          src/bss/bss.h, src/bss/bss.c.
validate: check inventory/bss_layout.tsv for gap-free, overlap-free coverage
          and print a summary JSON.
BSS owns no ROM bytes, so roundtrip() returns an empty row list.
"""
import json
import os
import re
import sys
from collections import defaultdict

BSS_START = 0x80057280
BSS_END = 0x800C7E30
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DISASM = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "private_extracted", "disasm", "fulltext.s")
SYMS = os.path.join(os.environ.get("PROJECT_ROOT", "."), "analysis/survey/symbol_addrs.txt")
CAND = os.environ.get("BSS_CANDIDATES", "")
ROM = os.environ.get("ROM", "baseroms/us/baserom.z64")
TSV = os.path.join(ROOT, "inventory", "bss_layout.tsv")
# ROM ranges holding initialized words (.rodata + both .data sections)
DATA_RANGES = [(0x103C, 0x45E0), (0x45E0, 0x45F0), (0x53474, 0x57E80)]
ROM_TO_VRAM = 0x7FFFF400

MEM = {"lb": (1, "s8"), "lbu": (1, "u8"), "sb": (1, "u8"),
       "lh": (2, "s16"), "lhu": (2, "u16"), "sh": (2, "s16"),
       "lw": (4, "s32"), "sw": (4, "s32"), "lwc1": (4, "f32"), "swc1": (4, "f32"),
       "ld": (8, "u64"), "sd": (8, "u64"), "ldc1": (8, "f64"), "sdc1": (8, "f64"),
       "lwl": (4, "u8"), "lwr": (4, "u8"), "swl": (4, "u8"), "swr": (4, "u8")}
NO_DEST = set(MEM) - {"lb", "lbu", "lh", "lhu", "lw", "ld", "lwl", "lwr"} | {
    "beq", "bne", "beql", "bnel", "blez", "bgtz", "bltz", "bgez", "blezl", "bgtzl",
    "bltzl", "bgezl", "j", "jr", "jal", "jalr", "mtc1", "mthi", "mtlo", "mult",
    "multu", "div", "divu", "ddiv", "ddivu", "dmult", "dmultu", "teq", "tne",
    "c.eq.s", "c.lt.s", "c.le.s", "c.eq.d", "c.lt.d", "c.le.d", "bc1t", "bc1f",
    "bc1tl", "bc1fl", "nop", "cache", "sync", "break", "syscall", "ctc1", "mtc0",
    "eret", "tlbwi", "tlbp", "tlbr", "tlbwr", "lwc1", "ldc1"}
CLOB = ("$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2",
        "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9", "$at")
LINE = re.compile(r"^([0-9A-F]{8}):\s+[0-9A-F]{8}\s+(\S+)\s*(.*)$")


def s16(x):
    return x - 0x10000 if x & 0x8000 else x


def imm(t):
    return int(t, 16) if t.lstrip("-").startswith("0x") and not t.startswith("-") else (
        -int(t[1:], 16) if t.startswith("-0x") else int(t))


def load_funcs():
    fs = []
    for ln in open(SYMS):
        m = re.match(r"(\w+) = 0x([0-9A-Fa-f]+);.*type:func", ln)
        if m:
            fs.append((int(m.group(2), 16), m.group(1)))
    return sorted(fs)


def load_candidates():
    out = {}
    for ln in open(CAND):
        m = re.match(r"extern\s+(.+?)\s*\b(D_[0-9A-F]{8})(\[\])?;\s*/\* 0x([0-9A-F]{8}) \.bss", ln)
        if m:
            out[int(m.group(4), 16)] = (m.group(1).replace("void*", "void *").strip(), bool(m.group(3)))
    return out


BRANCH_RE = re.compile(r"\. \+ 4 \+ \((-?0x[0-9A-Fa-f]+) << 2\)")
COND = {"beq", "bne", "beqz", "bnez", "blez", "bgtz", "bltz", "bgez", "bc1t", "bc1f",
        "bltzal", "bgezal"}
LIKELY = {"beql", "bnel", "beqzl", "bnezl", "blezl", "bgtzl", "bltzl", "bgezl", "bc1tl", "bc1fl"}
MEMSET = {"func_80038E90": ("$a0", "$a2", "memset"), "func_80051050": ("$a0", "$a1", "bzero")}
LOADS = {"lb", "lbu", "lh", "lhu", "lw", "ld", "lwl", "lwr", "lwc1", "ldc1"}


def _read_funcs():
    funcs = load_funcs()
    starts = {a: n for a, n in funcs}
    out = []
    cur = None
    for ln in open(DISASM):
        m = LINE.match(ln)
        if not m:
            continue
        pc = int(m.group(1), 16)
        if pc in starts or cur is None:
            cur = (starts.get(pc, "func_%08X" % pc), [])
            out.append(cur)
        op = m.group(2)
        rest = m.group(3)
        ops = [o.strip() for o in rest.split(",")] if rest else []
        cur[1].append((pc, op, ops, rest))
    return out


def _target(pc, op, ops, rest):
    m = BRANCH_RE.search(rest or "")
    if m:
        return pc + 4 + (imm(m.group(1)) << 2)
    if op in ("j", "b") and ops:
        mm = re.search(r"([0-9A-F]{8})", ops[-1])
        return int(mm.group(1), 16) if mm else None
    return None


def _step(st, pc, op, ops, hit):
    """Abstract-interpret one instruction on st: reg -> ('c', value) | ('i', base)."""
    def c(r):
        if r == "$zero":
            return 0
        v = st.get(r)
        return v[1] if v and v[0] == "c" else None
    d = ops[0] if ops else None
    if op in MEM and len(ops) == 2:
        mm = re.match(r"(-?0x[0-9A-Fa-f]+|-?\d+)\((\$\w+)\)", ops[1])
        if mm:
            off = s16(imm(mm.group(1)) & 0xFFFF); b = st.get(mm.group(2))
            if b:
                hit((b[1] + off) & 0xFFFFFFFF, "mem" if b[0] == "c" else "indexed", *MEM[op])
        if op in LOADS:
            st.pop(d, None)
        return
    if not ops or op in NO_DEST or op in COND or op in LIKELY or op in ("b", "bal", "bnez", "beqz", ".word"):
        return
    nv = None
    if op == "lui":
        nv = ("c", (imm(ops[1]) << 16) & 0xFFFFFFFF)
    elif op in ("addiu", "daddiu", "addi", "daddi") and len(ops) == 3:
        a = c(ops[1])
        if a is not None:
            nv = ("c", (a + s16(imm(ops[2]) & 0xFFFF)) & 0xFFFFFFFF)
            if ops[1] != "$zero" and (a & 0xFFFF) == 0:
                hit(nv[1], "addr")
        elif st.get(ops[1], ("x",))[0] == "i":
            nv = ("i", (st[ops[1]][1] + s16(imm(ops[2]) & 0xFFFF)) & 0xFFFFFFFF)
    elif op == "ori" and len(ops) == 3:
        a = c(ops[1])
        if a is not None:
            nv = ("c", a | (imm(ops[2]) & 0xFFFF))
            if ops[1] != "$zero" and a and (a & 0xFFFF) == 0:
                hit(nv[1], "addr")
    elif op in ("addu", "daddu", "or", "add") and len(ops) == 3:
        a, b = c(ops[1]), c(ops[2])
        if a is not None and b is not None:
            nv = ("c", (a + b) & 0xFFFFFFFF if op != "or" else a | b)
        elif op != "or" and (a is not None) != (b is not None) and (a or b):
            nv = ("i", a if a is not None else b)
    elif op == "move" and len(ops) == 2:
        nv = st.get(ops[1])
    elif op == "li" and len(ops) == 2:
        nv = ("c", imm(ops[1]) & 0xFFFFFFFF)
    if nv is None:
        st.pop(d, None)
    else:
        st[d] = nv


def _meet(states):
    states = [s for s in states if s is not None]
    if not states:
        return None
    out = dict(states[0])
    for s in states[1:]:
        for k in list(out):
            if s.get(k) != out[k]:
                del out[k]
    return out


def scan_code():
    refs = defaultdict(lambda: {"funcs": set(), "widths": set(), "types": set(), "kinds": set(), "clears": set()})
    for fname, ins in _read_funcs():
        pcs = [x[0] for x in ins]
        index = {p: i for i, p in enumerate(pcs)}
        leaders = {0}
        edges = defaultdict(list)   # leader idx -> [(succ idx, use_pre_ds)]
        for i, (pc, op, ops, rest) in enumerate(ins):
            if op in COND or op in LIKELY or op in ("b", "j", "jr") or (op == "bal"):
                if i + 2 < len(ins):
                    leaders.add(i + 2)
                t = _target(pc, op, ops, rest)
                if t in index:
                    leaders.add(index[t])
        lead = sorted(leaders)
        blocks = []
        for k, s in enumerate(lead):
            e = lead[k + 1] if k + 1 < len(lead) else len(ins)
            blocks.append((s, e))
        bidx = {s: n for n, (s, e) in enumerate(blocks)}
        succ = defaultdict(list)
        for n, (s, e) in enumerate(blocks):
            # find branch in block (second-to-last instr normally)
            br = None
            for i in range(s, e):
                op = ins[i][1]
                if op in COND or op in LIKELY or op in ("b", "j", "jr"):
                    br = i
            if br is None:
                if e < len(ins):
                    succ[n].append((bidx[e], False))
                continue
            pc, op, ops, rest = ins[br]
            t = _target(pc, op, ops, rest)
            if t in index and index[t] in bidx:
                succ[n].append((bidx[index[t]], False))
            if (op in COND or op in LIKELY) and e < len(ins):
                succ[n].append((bidx[e], op in LIKELY))
        n_blocks = len(blocks)
        entry = [None] * n_blocks
        entry[0] = {}
        exits = [None] * n_blocks
        preds = defaultdict(list)
        for n in succ:
            for m, pre in succ[n]:
                preds[m].append((n, pre))

        def run(n, record):
            st = dict(entry[n])
            s, e = blocks[n]
            pending = 0
            pre_ds = None
            calls = []
            for i in range(s, e):
                pc, op, ops, rest = ins[i]
                if pending == 1:
                    for r in CLOB:
                        st.pop(r, None)
                if pending:
                    pending -= 1
                if op in COND or op in LIKELY:
                    pre_ds = dict(st)

                def hit(a, kind, w=None, t=None):
                    if record and BSS_START <= a < BSS_END:
                        r = refs[a]; r["funcs"].add(fname); r["kinds"].add(kind)
                        if w:
                            r["widths"].add(w); r["types"].add(t)
                _step(st, pc, op, ops, hit)
                if op in ("jal", "jalr"):
                    pending = 2
                    calls.append((ops[0] if ops else "", i))
                if pending == 1 and calls and calls[-1][1] == i - 1:
                    tgt = calls[-1][0]
                    if record and tgt in MEMSET:
                        ra, rs, nm = MEMSET[tgt]
                        a, sz = st.get(ra), st.get(rs)
                        if a and a[0] == "c" and BSS_START <= a[1] < BSS_END and sz and sz[0] == "c":
                            refs[a[1]]["clears"].add((sz[1], nm, fname))
                            refs[a[1]]["funcs"].add(fname); refs[a[1]]["kinds"].add("addr")
            if pending == 1:
                for r in CLOB:
                    st.pop(r, None)
            return st, (pre_ds if pre_ds is not None else st)

        changed = True
        it = 0
        while changed and it < 50:
            changed = False; it += 1
            for n in range(n_blocks):
                if n:
                    ins_states = []
                    for p, pre in preds[n]:
                        if exits[p] is not None:
                            ins_states.append(exits[p][1] if pre else exits[p][0])
                    new = _meet(ins_states)
                    if new is None:
                        new = {} if not preds[n] else None
                    if new is None:
                        continue
                    if new == entry[n] and exits[n] is not None:
                        continue
                    entry[n] = new
                ex = run(n, False)
                if ex != exits[n]:
                    exits[n] = ex; changed = True
        for n in range(n_blocks):
            if entry[n] is None:
                entry[n] = {}
            run(n, True)
    return refs


def scan_data(refs):
    if not os.path.exists(ROM):
        return
    rom = open(ROM, "rb").read()
    for a, b in DATA_RANGES:
        for o in range(a, b - 3, 4):
            v = int.from_bytes(rom[o:o + 4], "big")
            if BSS_START <= v < BSS_END:
                r = refs[v]
                r["funcs"].add("data@%08X" % (o + ROM_TO_VRAM)); r["kinds"].add("dataptr")


def build():
    refs = scan_code()
    scan_data(refs)
    cand = load_candidates()
    # candidate_globals entries never reached from code are not added (see formats/bss.md)
    addrs = sorted(refs)
    rows = []
    aliases = []
    cover = BSS_START
    parent = None
    for i, a in enumerate(addrs):
        r = refs[a]
        if a < cover:
            ws = sorted(r["widths"]); w = ws[-1] if ws else 1
            at = sorted(r["types"])[0] if len(r["types"]) == 1 and min(a + w, cover) - a == w else None
            aliases.append(dict(vram_start=a, vram_end=min(a + w, cover), size=min(a + w, cover) - a,
                                symbol="D_%08X" % a, type=at or ("u8[%d]" % (min(a + w, cover) - a) if w > 1 else "u8"),
                                align=1, kind="alias", referenced_by=",".join(sorted(r["funcs"])),
                                evidence="inside %s;kinds=%s" % (parent, "+".join(sorted(r["kinds"])))))
            continue
        if a > cover:
            rows.append(gap(cover, a))
        end = addrs[i + 1] if i + 1 < len(addrs) else BSS_END
        clear = None
        wmax = max(r["widths"]) if r["widths"] else 1
        if a + wmax > end:
            end = min(a + wmax, BSS_END)   # widest access overlaps the next reference
        if r["clears"]:
            clear = max(r["clears"])
            end = min(a + clear[0], BSS_END)
        cover = end
        parent = "D_%08X" % a
        size = end - a
        ws = sorted(r["widths"])
        maxw = ws[-1] if ws else 1
        align = 1
        for al in (16, 8, 4, 2):
            if a % al == 0:
                align = min(al, 8); break
        # element type
        types = sorted(r["types"])
        if len(types) == 1:
            et = types[0]
        elif ws and len(set(ws)) == 1:
            et = {1: "u8", 2: "s16", 4: "s32", 8: "u64"}[ws[0]]
        else:
            et = "u8"
        ew = {"s8": 1, "u8": 1, "s16": 2, "u16": 2, "s32": 4, "f32": 4, "u64": 8, "f64": 8}[et]
        if a % ew or size % ew:
            et, ew = "u8", 1
        ev = []
        if a in cand:
            ct, carr = cand[a]
            ev.append("candidate_globals:%s%s" % (ct, "[]" if carr else ""))
        ev.append("kinds=" + "+".join(sorted(r["kinds"])))
        if ws:
            ev.append("widths=" + "/".join(map(str, ws)))
        if clear:
            ev.append("size=%s(0x%X) in %s" % (clear[1], clear[0], ",".join(sorted({c[2] for c in r["clears"]}))))
        else:
            ev.append("size=next_ref")
        if not clear and r["kinds"] <= {"mem"} and len(ws) == 1 and size > ws[0] and ew == ws[0]:
            # scalar-only access: symbol is one element, remainder is unreferenced
            rows.append(dict(vram_start=a, vram_end=a + ew, size=ew, symbol="D_%08X" % a, type=et,
                             align=align, kind="referenced", referenced_by=",".join(sorted(r["funcs"])),
                             evidence=";".join(ev[:-1] + ["size=access_width"])))
            rows.append(gap(a + ew, end))
            cover = end
            continue
        dims = size // ew
        ctype = et if dims == 1 else "%s[%d]" % (et, dims)
        rows.append(dict(vram_start=a, vram_end=end, size=size, symbol="D_%08X" % a,
                         type=ctype, align=align,
                         kind="referenced", referenced_by=",".join(sorted(r["funcs"])),
                         evidence=";".join(ev)))
    if cover < BSS_END:
        rows.append(gap(cover, BSS_END))
    return rows + aliases


def gap(a, b):
    return dict(vram_start=a, vram_end=b, size=b - a, symbol="bss_gap_%08X" % a,
                type="u8[%d]" % (b - a) if b - a > 1 else "u8", align=1, kind="inferred-gap", referenced_by="",
                evidence="no reference from code or initialized data")


COLS = ["vram_start", "vram_end", "size", "symbol", "type", "align", "kind", "referenced_by", "evidence"]


def write_all(rows):
    os.makedirs(os.path.dirname(TSV), exist_ok=True)
    with open(TSV, "w") as f:
        f.write("\t".join(COLS) + "\n")
        for r in rows:
            f.write("\t".join(("0x%08X" % r[c]) if c in ("vram_start", "vram_end") else
                              ("0x%X" % r[c]) if c == "size" else str(r[c]) for c in COLS) + "\n")
    d = os.path.join(ROOT, "src", "bss"); os.makedirs(d, exist_ok=True)

    def decl(r, ext):
        t = r["type"]; m = re.match(r"(\w+)\[(\d+)\]", t)
        base, dim = (m.group(1), "[0x%X]" % int(m.group(2))) if m else (t, "")
        if dim and base in ("u8", "s8") and r["vram_start"] % 4:
            # GCC word-aligns char arrays and structs: emit leading byte scalars up to the
            # next word boundary, then the remaining bytes as an aligned array
            out = []
            st, n = r["vram_start"], int(m.group(2))
            lead = min((-st) % 4, n)
            for k in range(lead):
                nm = r["symbol"] if k == 0 else "%s_b%X" % (r["symbol"], k)
                out.append("%s%s %s;" % ("extern " if ext else "", base, nm))
            if n > lead:
                rest = n - lead
                out.append("%s%s %s_b%X%s;" % ("extern " if ext else "", base, r["symbol"], lead,
                                              "[0x%X]" % rest if rest > 1 else ""))
            return " ".join(out)
        return "%s%s %s%s;" % ("extern " if ext else "", base, r["symbol"], dim)
    with open(os.path.join(d, "bss.h"), "w") as f:
        f.write("/* BSS 0x%08X..0x%08X (0x%X bytes), %d symbols. Generated by tools/bss.py. */\n"
                % (BSS_START, BSS_END, BSS_END - BSS_START, len(rows)))
        f.write("#ifndef BSS_H\n#define BSS_H\n#ifdef BSS_STANDALONE\n"
                "typedef signed char s8; typedef unsigned char u8; typedef short s16;\n"
                "typedef unsigned short u16; typedef int s32; typedef unsigned int u32;\n"
                "typedef unsigned long long u64; typedef float f32; typedef double f64;\n"
                "#else\n#include \"ultra64.h\"\n#endif\n\n")
        main = [r for r in rows if r["kind"] != "alias"]
        for r in rows:
            if r["kind"] == "alias":
                par = [m for m in main if m["vram_start"] <= r["vram_start"] < m["vram_end"]][0]
                base = re.match(r"\w+", r["type"]).group(0)
                f.write("#define %s (*(%s *)((u8 *)&%s + 0x%X)) /* alias, 0x%08X */\n"
                        % (r["symbol"], base, par["symbol"], r["vram_start"] - par["vram_start"], r["vram_start"]))
                continue
            f.write("%-44s /* 0x%08X size 0x%X %s */\n" % (decl(r, True), r["vram_start"], r["size"], r["kind"]))
        f.write("\n#endif\n")
    with open(os.path.join(d, "bss.c"), "w") as f:
        f.write("/* BSS definitions, address order, explicit sizes.\n"
                " * Linker placement: compile with KMC GCC 2.7.2 -G0 -mips3 -mgp32 -mfp32 -O2 -fno-common\n"
                " * (-fno-common makes GCC 2.7.2 emit the objects, in source order, as zero-filled\n"
                " * .data; COMMON order is not stable). Place this unit's .data then .bss as the\n"
                " * only input of a NOLOAD output section at 0x%08X; it must end at 0x%08X\n"
                " * (see src/bss/bss.ld, checked by tests/test_bss.py). Unaligned byte arrays are\n"
                " * split into leading u8 scalars plus a word-aligned _bN array because GCC\n"
                " * word-aligns char arrays and structs. Alias views are macros in bss.h. */\n"
                "#include \"bss.h\"\n\n" % (BSS_START, BSS_END))
        for r in rows:
            if r["kind"] != "alias":
                f.write("%s\n" % decl(r, False))


def load_tsv(path=TSV):
    rows = []
    with open(path) as f:
        hdr = f.readline().rstrip("\n").split("\t")
        for ln in f:
            v = dict(zip(hdr, ln.rstrip("\n").split("\t")))
            for c in ("vram_start", "vram_end", "size", "align"):
                v[c] = int(v[c], 0)
            rows.append(v)
    return rows


def validate(rows):
    errs = []
    main = [r for r in rows if r["kind"] != "alias"]
    pos = BSS_START
    for r in main:
        if r["vram_start"] != pos:
            errs.append("%s at 0x%08X expected 0x%08X" % (r["symbol"], r["vram_start"], pos))
        if r["vram_end"] - r["vram_start"] != r["size"] or r["size"] <= 0:
            errs.append("%s bad size" % r["symbol"])
        if r["vram_start"] % r["align"]:
            errs.append("%s misaligned" % r["symbol"])
        pos = r["vram_end"]
    if pos != BSS_END:
        errs.append("coverage ends at 0x%08X" % pos)
    names = [r["symbol"] for r in rows]
    if len(set(names)) != len(names):
        errs.append("duplicate names")
    ref = [r for r in main if r["kind"] == "referenced"]
    gp = [r for r in main if r["kind"] == "inferred-gap"]
    return dict(range=["0x%08X" % BSS_START, "0x%08X" % BSS_END], total_bytes=BSS_END - BSS_START,
                covered_bytes=sum(r["size"] for r in main), referenced_symbols=len(ref),
                referenced_bytes=sum(r["size"] for r in ref), gap_symbols=len(gp),
                gap_bytes=sum(r["size"] for r in gp), aliases=len(rows) - len(main),
                gap_free=not any("expected" in e or "coverage" in e for e in errs),
                overlap_free=not errs, errors=errs, ok=not errs)


def roundtrip(rom: bytes) -> list:
    """BSS occupies no ROM bytes; nothing to claim."""
    return []


if __name__ == "__main__":
    cmd = sys.argv[1] if len(sys.argv) > 1 else "validate"
    if cmd == "scan":
        write_all(build())
    s = validate(load_tsv())
    print(json.dumps(s, indent=1))
    sys.exit(0 if s["ok"] else 1)
