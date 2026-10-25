"""MIPS III decoding and lightweight data-flow helpers built on rabbitizer."""
from __future__ import annotations

import struct
from dataclasses import dataclass
from typing import Iterable, Iterator

import rabbitizer

RA, SP, FP, ZERO = 31, 29, 30, 0
JR_RA = 0x03E00008


def reg(r) -> int:
    return int(r.value) if r is not None else -1


def decode(word: int, vram: int) -> rabbitizer.Instruction:
    return rabbitizer.Instruction(word, vram=vram)


def instructions(code: bytes, vram: int) -> Iterator[rabbitizer.Instruction]:
    for i in range(0, len(code) - len(code) % 4, 4):
        yield decode(struct.unpack_from(">I", code, i)[0], vram + i)


def disassemble(code: bytes, vram: int) -> list[str]:
    return [f"{ins.vram:08X} {ins.getRaw():08X} {ins.disassemble()}" for ins in instructions(code, vram)]


@dataclass(frozen=True)
class Ref:
    """A resolved %hi/%lo address use: ``site`` is the low-half instruction."""
    site: int
    address: int
    opcode: str
    kind: str  # "load", "store", "addr" (address taken), "call", "jump"


def is_loadstore(ins) -> bool:
    return ins.doesLoad() or ins.doesStore()


def scan_refs(code: bytes, vram: int, call_targets: bool = True) -> list[Ref]:
    """Resolve lui/addiu|ori|load|store pairs and absolute jal/j targets in straight-line order.

    Register state is invalidated when a register is overwritten; this deliberately
    over-approximates across branches, which is the standard trade-off for
    reference discovery (splat/spimdisasm use the same idea with CFG refinement).
    """
    hi: dict[int, tuple[int, int]] = {}
    refs: list[Ref] = []
    for ins in instructions(code, vram):
        name = ins.getOpcodeName()
        if ins.isJumpWithAddress():
            target = ins.getInstrIndexAsVram()
            refs.append(Ref(ins.vram, target, name, "call" if ins.doesLink() else "jump"))
        if ins.canBeHi():  # lui
            hi[reg(ins.rt)] = (ins.vram, ins.getImmediate() << 16)
            continue
        if ins.canBeLo() and reg(ins.rs) in hi:
            base = hi[reg(ins.rs)][1]
            imm = ins.getImmediate()
            if name == "ori":
                address = base | imm
            else:
                address = (base + (imm - 0x10000 if imm & 0x8000 else imm)) & 0xFFFFFFFF
            kind = "load" if ins.doesLoad() else "store" if ins.doesStore() else "addr"
            refs.append(Ref(ins.vram, address, name, kind))
        dest = ins.getDestinationGpr()
        if dest is not None and reg(dest) in hi and not (ins.canBeLo() and kind_keeps_hi(ins)):
            hi.pop(reg(dest), None)
        if ins.isJrRa() or ins.isUnconditionalBranch():
            hi.clear()
    return refs


def kind_keeps_hi(ins) -> bool:
    """A load/store through a %hi base does not clobber the base register unless it is the target."""
    return ins.doesStore() or (ins.doesLoad() and reg(ins.rt) != reg(ins.rs))


def returns(code: bytes, vram: int) -> list[int]:
    """Addresses of ``jr $ra`` instructions."""
    return [vram + i for i in range(0, len(code) - 3, 4) if struct.unpack_from(">I", code, i)[0] == JR_RA]


def call_targets(code: bytes, vram: int) -> set[int]:
    return {r.address for r in scan_refs(code, vram) if r.kind == "call"}


def looks_like_code(code: bytes, vram: int, min_valid: float = 0.97) -> bool:
    """Cheap classifier: share of words that decode to valid, implemented, compiler-emittable instructions."""
    total = valid = 0
    for ins in instructions(code, vram):
        total += 1
        if ins.getRaw() == 0 or (ins.isValid() and ins.isImplemented() and not ins.notEmittedByCompilers()):
            valid += 1
    return total > 0 and valid / total >= min_valid


def iter_words(data: bytes, start: int = 0) -> Iterable[tuple[int, int]]:
    for i in range(start, len(data) - 3, 4):
        yield i, struct.unpack_from(">I", data, i)[0]
