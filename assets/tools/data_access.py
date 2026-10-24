"""Code-side access analysis for initialized data.

Walks the text disassembly and records every lui/lo-formed data access:
  addr      absolute address formed by %hi + %lo
  kind      w h b f32 f64 d u (unaligned/cop2) addr (address taken)
  signed    for loads: lb/lh/lw signed, lbu/lhu unsigned
  stride    element stride when the base register was combined (addu) with an index register
            whose scale is known (sll/addu/subu chains), else None for indexed, 0 for direct
  use       for loaded words: 'deref' (used as a base register / jalr target),
            'arg' (moved into $a0-$a3 before a jal), or None
"""
import re

ACC = {'lw': ('w', 1), 'sw': ('w', None), 'll': ('w', 1), 'lh': ('h', 1), 'lhu': ('h', 0), 'sh': ('h', None),
       'lb': ('b', 1), 'lbu': ('b', 0), 'sb': ('b', None), 'lwc1': ('f32', None), 'swc1': ('f32', None),
       'ldc1': ('f64', None), 'sdc1': ('f64', None), 'ld': ('d', None), 'sd': ('d', None),
       'lwl': ('u', None), 'lwr': ('u', None), 'swl': ('u', None), 'swr': ('u', None),
       'lwc2': ('u', None), 'swc2': ('u', None)}
NOWRITE = {'sw', 'sh', 'sb', 'swc1', 'sdc1', 'sd', 'swl', 'swr', 'swc2', 'j', 'jal', 'jr', 'jalr', 'b',
           'beq', 'bne', 'beqz', 'bnez', 'blez', 'bgez', 'bltz', 'bgtz', 'beql', 'bnel', 'bc1f', 'bc1t',
           'bc1fl', 'bc1tl', 'bgezl', 'bltzl', 'blezl', 'bgtzl', 'bnezl', 'beqzl', 'bgezal', 'bltzal',
           'mtc1', 'mtc0', 'ctc1', 'mult', 'multu', 'div', 'divu', 'dmult', 'dmultu', 'ddiv', 'ddivu',
           'break', 'nop', '.word', 'cache', 'sync', 'mthi', 'mtlo', 'syscall', 'eret', 'tlbwi', 'tlbp',
           'tlbr', 'tlbwr', 'mtc2', 'ctc2', 'c.eq.s', 'c.lt.s', 'c.le.s', 'c.eq.d', 'c.lt.d', 'c.le.d',
           'teq', 'tne'}
CLOBBER = ['$at', '$v0', '$v1', '$a0', '$a1', '$a2', '$a3'] + ['$t%d' % i for i in range(10)]
MEM_RE = re.compile(r'(-?0x[0-9A-Fa-f]+|-?\d+)\((\$\w+)\)')


def _int(s):
    return int(s, 16 if 'x' in s else 10)


def parse(path):
    ins = []
    for line in open(path):
        m = re.match(r'([0-9A-F]{8}): [0-9A-F]{8}\s+(\S+)\s*(.*)', line)
        if not m: continue
        args = m.group(3).split('#')[0].strip()
        a = [x.strip() for x in args.split(',')] if args else []
        ins.append((int(m.group(1), 16), m.group(2), a))
    return ins


def _ptr_use(ins, i, reg, limit=12):
    for j in range(i + 1, min(len(ins), i + 1 + limit)):
        pc, op, a = ins[j]
        if not a: continue
        mm = MEM_RE.match(a[-1])
        if mm and mm.group(2) == reg and op in ACC:
            return 'deref'
        if op in ('jalr', 'jr') and a[-1] == reg:
            return 'deref'
        if op in ('addu', 'or', 'move') and len(a) >= 2 and a[0] in ('$a0', '$a1', '$a2', '$a3') and reg in a[1:] \
                and (len(a) < 3 or '$zero' in a[1:]):
            for k in range(j, min(len(ins), j + 6)):
                if ins[k][1] == 'jal': return 'arg'
        if op not in NOWRITE and a[0] == reg and j != i:
            return None
    return None


def accesses(path, func_starts):
    ins = parse(path)
    out = []
    hi = {}       # reg -> (hi value, stride or 0 for direct / None unknown)
    coef = {}     # reg -> integer scale of an index variable
    starts = set(func_starts)
    for i, (pc, op, a) in enumerate(ins):
        if pc in starts: hi = {}; coef = {}
        if op == 'lui' and len(a) == 2:
            hi[a[0]] = (_int(a[1]) << 16, 0); coef.pop(a[0], None); continue
        mm = MEM_RE.match(a[-1]) if a else None
        if mm and op in ACC and mm.group(2) in hi:
            base, stride = hi[mm.group(2)]
            kind, sg = ACC[op]
            rec = {'pc': pc, 'addr': (base + _int(mm.group(1))) & 0xFFFFFFFF, 'kind': kind, 'op': op,
                   'signed': sg, 'stride': stride, 'use': None, 'base': base & 0xFFFFFFFF,
                   'off': _int(mm.group(1))}
            if op == 'lw':
                rec['use'] = _ptr_use(ins, i, a[0])
            out.append(rec)
        elif op in ('addiu', 'ori') and len(a) == 3 and a[1] in hi:
            base, stride = hi[a[1]]
            v = _int(a[2])
            addr = (base | (v & 0xFFFF)) if op == 'ori' else base + v
            if a[0] == a[1] and base & 0xFFFF:
                # pointer increment of an already formed address: element stride evidence
                hi[a[0]] = (base, v)
                out.append({'pc': pc, 'addr': base & 0xFFFFFFFF, 'kind': 'inc', 'op': op, 'signed': None,
                            'stride': v, 'use': None, 'base': base & 0xFFFFFFFF, 'off': 0})
                continue
            out.append({'pc': pc, 'addr': addr & 0xFFFFFFFF, 'kind': 'addr', 'op': op, 'signed': None,
                        'stride': stride, 'use': None, 'base': base & 0xFFFFFFFF, 'off': v})
            hi[a[0]] = (addr & 0xFFFFFFFF, stride); coef.pop(a[0], None); continue
        elif op == 'addu' and len(a) == 3 and (a[1] in hi) != (a[2] in hi):
            h = a[1] if a[1] in hi else a[2]
            idx = a[2] if h == a[1] else a[1]
            hi[a[0]] = (hi[h][0], coef.get(idx, 1)); coef.pop(a[0], None); continue
        elif op == 'sll' and len(a) == 3:
            c = coef.get(a[1], 1) << _int(a[2])
            hi.pop(a[0], None); coef[a[0]] = c; continue
        elif op in ('addu', 'subu') and len(a) == 3 and a[1] not in hi and a[2] not in hi and '$zero' not in a[1:]:
            c1, c2 = coef.get(a[1], 1), coef.get(a[2], 1)
            hi.pop(a[0], None); coef[a[0]] = c1 + c2 if op == 'addu' else c1 - c2; continue
        if op in ('jal', 'jalr'):
            for r in CLOBBER: hi.pop(r, None); coef.pop(r, None)
        if op not in NOWRITE and a and a[0].startswith('$') and not a[0].startswith('$f'):
            hi.pop(a[0], None); coef.pop(a[0], None)
    return out
