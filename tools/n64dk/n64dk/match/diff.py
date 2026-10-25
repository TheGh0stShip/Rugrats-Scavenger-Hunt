"""Aligned instruction diff between compiled and retail code, with mismatch classification.

Instructions are aligned on their mnemonic sequence (``difflib``) and every difference gets
one class:

``register``      same opcode and immediates, different registers (register allocation)
``operand-order`` same opcode and the same registers in a different order
``immediate``     same opcode and registers, different immediate (constants, relocations)
``branch``        branch/jump whose only difference is its target (usually a size knock-on)
``hazard-nop``    a ``nop`` present on one side only
``scheduling``    the instruction exists on the other side within a few slots (reordering)
``instruction``   anything else
``size``          reported once when the code sizes differ

The register mapping of all ``register`` differences is checked for a consistent bijection
(``register_swap``), the classic allocation swap.
"""
from __future__ import annotations

import difflib
import re
import struct
from collections import Counter
from dataclasses import dataclass, field

import rabbitizer

REG = re.compile(r"\$\w+")
WEIGHTS = {"register": 1, "operand-order": 1, "immediate": 1, "branch": 1, "hazard-nop": 2,
           "scheduling": 2, "instruction": 3}
WINDOW = 8


@dataclass
class Entry:
    kind: str
    offset: int | None        # byte offset in the compiled code
    target_offset: int | None  # byte offset in the retail code
    compiled: str
    retail: str


@dataclass
class Report:
    entries: list[Entry] = field(default_factory=list)
    compiled_size: int = 0
    target_size: int = 0
    register_swap: dict[str, str] | None = None

    @property
    def counts(self) -> Counter:
        c = Counter(e.kind for e in self.entries)
        if self.compiled_size != self.target_size:
            c["size"] += 1
        return c

    @property
    def score(self) -> int:
        """0 for identical code; grows with the number and severity of differences."""
        return sum(WEIGHTS.get(e.kind, 1) for e in self.entries) + abs(self.compiled_size - self.target_size) // 4

    def summary(self) -> str:
        if not self.entries and self.compiled_size == self.target_size:
            return "identical"
        parts = [f"{k} {v}" for k, v in sorted(self.counts.items())]
        if self.register_swap:
            parts.append("swap " + ",".join(f"{a}<->{b}" for a, b in sorted(self.register_swap.items())))
        return "; ".join(parts) + f" (score {self.score})"

    def as_dict(self, limit: int = 64) -> dict:
        return {"score": self.score, "counts": dict(self.counts), "compiled_size": self.compiled_size,
                "target_size": self.target_size, "register_swap": self.register_swap,
                "entries": [vars(e) for e in self.entries[:limit]]}

    def lines(self, limit: int = 40) -> list[str]:
        out = []
        for e in self.entries[:limit]:
            loc = f"{e.offset:#06x}" if e.offset is not None else "   -  "
            out.append(f"{loc}  {e.kind:<13} {e.compiled:<36} | {e.retail}")
        return out


def _decode(code: bytes, vram: int) -> list[tuple[str, str, str, bool]]:
    """(mnemonic, full text, text without branch targets, is_branch) per word."""
    out = []
    for i in range(0, len(code) - len(code) % 4, 4):
        word, = struct.unpack_from(">I", code, i)
        ins = rabbitizer.Instruction(word, vram + i)
        text = " ".join(ins.disassemble().split())
        name = ins.getOpcodeName()
        branch = ins.isBranch() or ins.isJumpWithAddress()
        bare = text.rsplit(",", 1)[0] if branch and "," in text else (name if branch else text)
        out.append((name, text, bare, branch))
    return out


def _operands(text: str) -> tuple[list[str], str]:
    ops = text.split(None, 1)[1] if " " in text else ""
    return REG.findall(ops), REG.sub("R", ops)


def _classify(a: tuple, b: tuple) -> str:
    regs_a, rest_a = _operands(a[1])
    regs_b, rest_b = _operands(b[1])
    if rest_a == rest_b:
        return "operand-order" if sorted(regs_a) == sorted(regs_b) else "register"
    if regs_a == regs_b:
        return "branch" if a[3] else "immediate"
    return "instruction"


def diff(compiled: bytes, target: bytes, vram: int) -> Report:
    a, b = _decode(compiled, vram), _decode(target, vram)
    rep = Report(compiled_size=len(compiled), target_size=len(target))
    sm = difflib.SequenceMatcher(None, [x[0] for x in a], [x[0] for x in b], autojunk=False)
    mapping: dict[str, Counter] = {}
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            for k in range(i2 - i1):
                x, y = a[i1 + k], b[j1 + k]
                if x[1] == y[1]:
                    continue
                kind = _classify(x, y)
                if kind == "register":
                    for ra, rb in zip(_operands(x[1])[0], _operands(y[1])[0]):
                        if ra != rb:
                            mapping.setdefault(ra, Counter())[rb] += 1
                rep.entries.append(Entry(kind, 4 * (i1 + k), 4 * (j1 + k), x[1], y[1]))
            continue
        left, right = list(range(i1, i2)), list(range(j1, j2))
        b_bare = Counter(y[2] for y in b[max(0, j1 - WINDOW):j2 + WINDOW])
        a_bare = Counter(x[2] for x in a[max(0, i1 - WINDOW):i2 + WINDOW])
        for n in range(max(len(left), len(right))):
            x = a[left[n]] if n < len(left) else None
            y = b[right[n]] if n < len(right) else None
            if x is not None and y is not None and x[0] == y[0]:
                kind = _classify(x, y)
            elif (x is None or x[0] == "nop") and (y is None or y[0] == "nop") and (x or y):
                kind = "hazard-nop"
            elif x is not None and y is None:
                kind = "hazard-nop" if x[0] == "nop" else ("scheduling" if b_bare[x[2]] else "instruction")
            elif y is not None and x is None:
                kind = "hazard-nop" if y[0] == "nop" else ("scheduling" if a_bare[y[2]] else "instruction")
            else:
                kind = "scheduling" if (b_bare[x[2]] and a_bare[y[2]]) else "instruction"
            rep.entries.append(Entry(kind, 4 * left[n] if x else None, 4 * right[n] if y else None,
                                     x[1] if x else "", y[1] if y else ""))
    if mapping:
        flat = {ra: c.most_common(1)[0][0] for ra, c in mapping.items()}
        consistent = all(len(c) == 1 for c in mapping.values()) and len(set(flat.values())) == len(flat)
        if consistent:
            rep.register_swap = flat
    return rep
