"""Pointer flow over the text disassembly (linear, per function, with call-argument seeding).

Values held in registers / stack slots:
  ('c', addr, stride, prov)   concrete address (stride: index stride if an index was added)
  ('i', X, S, delta, prov)    word loaded from an indexed table: any of word(X + k*S), plus delta
prov is where the value was loaded from: ('w', A) a concrete data word, ('x', X, S) an indexed table field,
or None for an address formed by lui/addiu.

Results:
  records   access records (same shape as data_access) at concrete addresses reached through loaded pointers
            or through pointers passed into callees
  confirmed set of provenances whose value was dereferenced, called through, or passed to a callee that did
  idx_deref {(X, S): [(off, kind, signed)]} accesses through values loaded from indexed tables
  loaded    set of provenances that were loaded at all
"""
import collections, math, re, struct


def _gcd(a, b): return math.gcd(a, b) or max(a, b)
from data_access import ACC, NOWRITE, MEM_RE, _int, parse

BR_RE = re.compile(r'\. \+ 4 \+ \((-?0x[0-9A-Fa-f]+|-?\d+) << 2\)')
ARGS = ('$a0', '$a1', '$a2', '$a3')
CLOBBER = ['$at', '$v0', '$v1', '$a0', '$a1', '$a2', '$a3'] + ['$t%d' % i for i in range(10)]


class Flow:
    def __init__(self, path, funcs, rom, v2r, owned, is_ptrval):
        self.ins = parse(path)
        self.pcidx = {pc: i for i, (pc, op, a) in enumerate(self.ins)}
        self.funcs = funcs            # addr -> (name, size)
        self.rom, self.v2r, self.owned, self.is_ptrval = rom, v2r, owned, is_ptrval
        self.records = []
        self.confirmed = set()
        self.loaded = set()
        self.idx_deref = collections.defaultdict(set)
        self.seeds = collections.defaultdict(set)   # func -> {(reg, value-without-index)}
        self.done = set()

    def word(self, a):
        r = self.v2r(a)
        if not self.owned(a): return None
        return struct.unpack('>I', self.rom[r:r + 4])[0]

    def confirm(self, v):
        if v is None: return
        p = v[3] if v[0] == 'c' else (v[4] if v[0] == 'i' else v[1])
        if p: self.confirmed.add(p)

    def run(self, rounds=4):
        for f in sorted(self.funcs):
            self.analyse(f, ())
        for _ in range(rounds):
            todo = [(f, tuple(sorted(s, key=repr))) for f, s in list(self.seeds.items())]
            new = False
            for f, seed in todo:
                for item in seed:
                    key = (f, item)
                    if key in self.done or f not in self.funcs: continue
                    self.done.add(key); new = True
                    self.analyse(f, (item,))
            if not new: break
        return self

    def analyse(self, f, seed, start=None, init=None, depth=0):
        name, size = self.funcs[f]
        i0 = self.pcidx.get(f if start is None else start)
        if i0 is None: return
        st = dict(init or {}); coef = {}; slot = {}
        extra_joins = []
        for reg, val in seed:
            if val[0] == 'c' and val[3] is None:
                val = ('c', val[1], val[2], ('a', val[1])) + tuple(val[4:])
            st[reg] = val
        seeded = {reg for reg, val in seed}
        i = i0
        pending = {}; snap = None; snap_i = -1; restore = False
        while i < len(self.ins) and (self.ins[i][0] < f + size or
                                     (i >= 2 and not (self.ins[i - 2][1] == 'jr' and self.ins[i - 2][2] == ['$ra'])
                                      and self.ins[i][0] < f + size + 0x400 and self.ins[i - 1][1] != 'j')):
            if snap is not None and i == snap_i:
                pending.setdefault(snap, dict(st)); snap = None
            pc, op, a = self.ins[i]
            if restore:
                st = dict(pending.get(pc) or {}); coef = {}; restore = False
            elif pending.get(pc) and depth < 2 and len(extra_joins) < 64 and \
                    any(v and v[0] in ('c', 'i') for v in pending[pc].values()):
                # join reached by fall-through: analyse the jumped-in state separately
                extra_joins.append((pc, pending[pc]))
            i += 1
            if not a and op != 'nop': continue
            if a:
                bm = BR_RE.search(a[-1])
                if bm and op not in ('j', 'b') and not (op == 'beq' and a[:2] == ['$zero', '$zero']) \
                        and not (op == 'bgez' and a[0] == '$zero'):
                    snap = pc + 4 + (_int(bm.group(1)) << 2); snap_i = i + 1
                    continue
            mm = MEM_RE.match(a[-1]) if a else None
            if op == 'lui':
                st[a[0]] = ('c', _int(a[1]) << 16, 0, None); coef.pop(a[0], None); continue
            if mm and op in ACC:
                off, base = _int(mm.group(1)), mm.group(2)
                if base == '$sp':
                    if op == 'sw' and a[0] in st: slot[off] = st[a[0]]
                    elif op == 'sw': slot.pop(off, None)
                    elif op == 'lw':
                        if off in slot: st[a[0]] = slot[off]
                        else: st.pop(a[0], None)
                    elif op not in NOWRITE: st.pop(a[0], None)
                    continue
                bv = st.get(base)
                kind, sg = ACC[op]
                res = None
                if bv and bv[0] == 'c':
                    addr = (bv[1] + off) & 0xFFFFFFFF
                    if True:
                        # access through a loaded or passed pointer: new evidence
                        if bv[3] is not None: self.confirm(bv)
                        if self.owned(addr):
                            self.records.append({'pc': pc, 'addr': addr, 'kind': kind, 'op': op, 'signed': sg,
                                                 'stride': bv[2], 'use': None, 'base': bv[1], 'off': off,
                                                 'via': 'ptrflow', 'arr': bv[2] == 1,
                                                 'formed': bv[3] is None and base not in seeded})
                    if op == 'lw':
                        if bv[2] and bv[2] > 1 and self.owned(addr):
                            span = bv[4] if len(bv) > 4 else bv[2]
                            prov = ('x', addr, bv[2], span); self.loaded.add(prov)
                            res = ('i', addr, bv[2], 0, prov)
                        elif not bv[2]:
                            w = self.word(addr)
                            if w is not None:
                                prov = ('w', addr); self.loaded.add(prov)
                                if self.is_ptrval(w): res = ('c', w, 0, prov)
                                else: res = ('n', prov)
                elif bv and bv[0] == 'i':
                    self.confirm(bv)
                    self.idx_deref[bv[4][1:]].add((bv[3] + off, kind, sg, len(bv) > 5))
                elif bv and bv[0] == 'n':
                    pass
                if op in NOWRITE: continue
                if res: st[a[0]] = res
                else: st.pop(a[0], None)
                coef.pop(a[0], None)
                continue
            if op in ('j', 'b') or (op == 'beq' and a[:2] == ['$zero', '$zero']) or (op == 'bgez' and a[0] == '$zero'):
                # unconditional transfer: the instruction after the delay slot is reached from a branch
                bm = BR_RE.search(a[-1])
                if bm: pending.setdefault(pc + 4 + (_int(bm.group(1)) << 2), None)
                if op == 'j' and a[0].startswith('func_'):
                    # jump to a survey 'function' that is a join point of this one: carry live pointers
                    if i < len(self.ins): self.step_simple(st, coef, self.ins[i][1], self.ins[i][2])
                    tgt = int(a[0][5:], 16)
                    if tgt not in self.funcs:
                        if pending.get(tgt) is None: pending[tgt] = dict(st)
                        elif depth < 2 and len(extra_joins) < 64 and \
                                any(v and v[0] in ('c', 'i') for v in st.values()):
                            extra_joins.append((tgt, dict(st)))
                        i += 1; restore = True
                        continue
                    for r, v in st.items():
                        if v and v[0] in ('c', 'i') and (v[0] == 'i' or v[3] is not None or self.owned(v[1])):
                            self.seeds[tgt].add((r, v))
                    i += 1; restore = True
                    continue
                if i < len(self.ins):
                    self.step_simple(st, coef, self.ins[i][1], self.ins[i][2])
                    if bm and pending.get(pc + 4 + (_int(bm.group(1)) << 2)) is None:
                        pending[pc + 4 + (_int(bm.group(1)) << 2)] = dict(st)
                i += 1
                restore = True
                continue
            if op in ('jalr', 'jr') and a and a[-1] != '$ra':
                self.confirm(st.get(a[-1]))
                if op == 'jalr':
                    for r in CLOBBER: st.pop(r, None)
                continue
            if op == 'jal':
                tgt = int(a[0].replace('func_', ''), 16) if a[0].startswith('func_') else _int(a[0])
                # delay slot first
                if i < len(self.ins):
                    pc2, op2, a2 = self.ins[i]
                    self.step_simple(st, coef, op2, a2)
                for r in ARGS:
                    v = st.get(r)
                    if v and v[0] in ('c', 'i') and (v[0] == 'i' or v[3] is not None or self.owned(v[1])):
                        self.seeds[tgt].add((r, v))
                for r in CLOBBER: st.pop(r, None)
                i += 1
                continue
            self.step_simple(st, coef, op, a)
        for tgt, state in extra_joins:
            self.analyse(f, seed, start=tgt, init=state, depth=depth + 1)

    def step_simple(self, st, coef, op, a):
        if not a: return
        if op in ('addiu', 'ori') and len(a) == 3 and a[1] in st and st[a[1]][0] in ('c', 'i'):
            v = st[a[1]]; imm = _int(a[2])
            if v[0] == 'c':
                if a[0] == a[1] and v[1] & 0xFFFF:
                    st[a[0]] = ('c', v[1], abs(imm) if imm else v[2], v[3])
                else:
                    nv = (v[1] | (imm & 0xFFFF)) if op == 'ori' else v[1] + imm
                    st[a[0]] = ('c', nv & 0xFFFFFFFF, v[2], v[3])
            else:
                st[a[0]] = ('i', v[1], v[2], v[3] + imm, v[4])
            coef.pop(a[0], None); return
        if op in ('addu', 'or') and len(a) == 3:
            if a[2] == '$zero' and a[1] in st: st[a[0]] = st[a[1]]; return
            if a[1] == '$zero' and a[2] in st: st[a[0]] = st[a[2]]; return
            ia = a[1] in st and st[a[1]][0] == 'i'
            ib = a[2] in st and st[a[2]][0] == 'i'
            if op == 'addu' and ia != ib and not ((a[1] in st and not ia) or (a[2] in st and not ib)):
                v = st[a[1]] if ia else st[a[2]]
                st[a[0]] = ('i', v[1], v[2], v[3], v[4], 'indexed'); coef.pop(a[0], None); return
            pa = a[1] in st and st[a[1]][0] == 'c'
            pb = a[2] in st and st[a[2]][0] == 'c'
            if op == 'addu' and pa != pb:
                v = st[a[1]] if pa else st[a[2]]
                idx = a[2] if pa else a[1]
                c = abs(coef.get(idx, 1))
                st[a[0]] = ('c', v[1], c if not v[2] else _gcd(v[2], c), v[3], max(v[2] or 0, c) if len(v) < 5 else max(v[4], c))
                coef.pop(a[0], None); return
        if op == 'sll' and len(a) == 3:
            c = coef.get(a[1], 1) << _int(a[2]); st.pop(a[0], None); coef[a[0]] = c; return
        if op in ('addu', 'subu') and len(a) == 3 and a[1] not in st and a[2] not in st and '$zero' not in a[1:]:
            c1, c2 = coef.get(a[1], 1), coef.get(a[2], 1)
            st.pop(a[0], None); coef[a[0]] = c1 + c2 if op == 'addu' else c1 - c2; return
        if op not in NOWRITE and a[0].startswith('$') and not a[0].startswith('$f'):
            st.pop(a[0], None); coef.pop(a[0], None)
