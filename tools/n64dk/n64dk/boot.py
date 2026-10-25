"""Boot analysis: the IPL3 copy of the first MiB and the game's entry routine.

The entry routine is read by a short constant-propagating walk rather than by pattern:
registers are tracked through ``lui``/``ori``/``addiu``/``addu`` and through ``lw`` from
the boot image (some games keep their segment table there), and forward branches whose
condition is known are followed.  The walk finds the
stack pointer, the BSS-clearing loop (a backward branch around a store of ``$zero``) and
the call or jump that hands over to the game (``jr`` to a known address always counts as
the hand-over; ``j`` is followed only as a short forward skip over inline data).  Games whose entry routine first runs a
loader (decompressing or copying the program elsewhere) are reported as ``staged``.
"""
from __future__ import annotations

from dataclasses import dataclass, field

from . import mips
from .rom import Rom

IPL3_COPY_ROM = 0x1000
IPL3_COPY_SIZE = 0x100000
M32 = 0xFFFFFFFF
SP, RA = 29, 31
CALLEE_SAVED = {16, 17, 18, 19, 20, 21, 22, 23, 28, 29, 30}


@dataclass
class BootInfo:
    entry: int
    main_rom: int
    main_vram: int
    copy_size: int
    stack_top: int | None = None
    bss_start: int | None = None
    bss_end: int | None = None
    main_call: int | None = None
    staged: bool = False                                     # hands over to code outside the boot image
    calls: list[tuple[int, int]] = field(default_factory=list)  # (site, target) calls and exits, in order
    refs: list = field(default_factory=list)

    def rom_to_vram(self, rom: int) -> int:
        return rom - self.main_rom + self.main_vram

    def vram_to_rom(self, vram: int) -> int:
        return vram - self.main_vram + self.main_rom

    def in_image(self, vram: int) -> bool:
        return self.main_vram <= vram < self.main_vram + self.copy_size


def analyse(rom: Rom, window: int = 0x100, max_steps: int = 512) -> BootInfo:
    """Read the entry routine: stack set-up, BSS clear bounds and the hand-over call or jump.

    The IPL3 copies ROM 0x1000.. (1 MiB) to the header entry address, so the
    first instructions at ROM 0x1000 are the entry routine.
    """
    entry = rom.header.entry
    info = BootInfo(entry=rom.entry_vram, main_rom=IPL3_COPY_ROM, main_vram=entry, copy_size=IPL3_COPY_SIZE)
    info.refs = mips.scan_refs(rom.data[IPL3_COPY_ROM:IPL3_COPY_ROM + window], entry)
    _walk(rom, info, max_steps)
    return info


def analyse_segment(data: bytes, vram: int, entry: int, max_steps: int = 512) -> BootInfo:
    """The same walk over code that is not in the ROM as is (a loader's unpacked output):
    ``data`` is loaded at ``vram`` and the walk starts at ``entry``."""
    info = BootInfo(entry=entry, main_rom=0, main_vram=vram, copy_size=len(data))
    _walk(_Bytes(data), info, max_steps)
    return info


class _Bytes:
    def __init__(self, data: bytes):
        self.data = data


def _word(rom: Rom, info: BootInfo, vram: int) -> int | None:
    vram &= M32
    if not info.in_image(vram) or vram & 3:
        return None
    off = info.vram_to_rom(vram)
    return int.from_bytes(rom.data[off:off + 4], "big") if off + 4 <= len(rom.data) else None


def _sx16(v: int) -> int:
    return v - 0x10000 if v & 0x8000 else v


def _walk(rom: Rom, info: BootInfo, max_steps: int) -> None:
    regs: dict[int, int] = {0: 0}
    snap: dict[int, dict[int, int]] = {}     # register state on first arrival at each pc
    exits: list[tuple[int, int]] = []        # (site, target) calls and non-local transfers
    bss_exit_index = None
    pc = info.entry & M32
    visited: set[int] = set()

    def get(r: int):
        return regs.get(r)

    def put(r: int, v):
        if r:
            if v is None:
                regs.pop(r, None)
            else:
                regs[r] = v & M32

    def execute(pc: int, w: int) -> None:
        """Effects of one non-control instruction on the tracked registers."""
        op, rs, rt, rd = w >> 26, (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31
        imm, a, b = w & 0xFFFF, get((w >> 21) & 31), get((w >> 16) & 31)
        if op == 15:
            put(rt, imm << 16)
        elif op in (8, 9, 24, 25):
            put(rt, None if a is None else a + _sx16(imm))
        elif op == 13:
            put(rt, None if a is None else a | imm)
        elif op == 12:
            put(rt, None if a is None else a & imm)
        elif op == 35:
            put(rt, None if a is None else _word(rom, info, a + _sx16(imm)))
        elif op == 0:
            fn = w & 63
            if fn in (0x20, 0x21, 0x2C, 0x2D):
                put(rd, None if a is None or b is None else a + b)
            elif fn in (0x22, 0x23, 0x2E, 0x2F):
                put(rd, None if a is None or b is None else a - b)
            elif fn == 0x25:
                put(rd, None if a is None or b is None else a | b)
            elif fn == 0x24:
                put(rd, None if a is None or b is None else a & b)
            elif fn not in (0x08, 0x09, 0x0C, 0x0D, 0x0F, 0x11, 0x13, 0x18, 0x19, 0x1A, 0x1B):
                put(rd, None)
        elif 32 <= op <= 39 or op in (26, 27, 55):
            put(rt, None)   # other loads
        elif op in (10, 11, 14):
            put(rt, None)
        elif op in (16, 17) and (rs in (0, 1, 2)):
            put(rt, None)   # mfc0/mfc1/cfc1

    def zero_store_base(lo: int, hi: int):
        for addr in range(lo, hi + 8, 4):
            w = _word(rom, info, addr)
            if w is not None and (w >> 26) in (43, 63) and (w >> 16) & 31 == 0:
                return (w >> 21) & 31
        return None

    def loop_bounds(start_pc: int, branch_pc: int, base: int):
        """Start and end of a zero-store loop from the registers on loop entry."""
        state = snap.get(start_pc, {})
        start = state.get(base)
        if start is None:
            return None
        others: list[int] = []
        for addr in range(start_pc, branch_pc + 8, 4):
            w = _word(rom, info, addr) or 0
            op, rs, rt = w >> 26, (w >> 21) & 31, (w >> 16) & 31
            regs_used = []
            if op in (4, 5, 20, 21):
                regs_used = [rs, rt]
            elif op in (1, 6, 7, 22, 23):
                regs_used = [rs]
            elif op == 0 and (w & 63) in (0x2A, 0x2B):
                regs_used = [rs, rt]
            others += [r for r in regs_used if r not in (0, base) and r not in others]
        for r in others:
            v = state.get(r)
            if v is not None and v:
                return start, (v if v > start else start + v)
        return None

    for _ in range(max_steps):
        if pc in visited:
            break
        visited.add(pc)
        snap.setdefault(pc, dict(regs))
        w = _word(rom, info, pc)
        if w is None:
            break
        op, rs, rt, rd = w >> 26, (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31
        simm = _sx16(w & 0xFFFF)
        a, b = get(rs), get(rt)
        delay = _word(rom, info, pc + 4)

        def run_delay():
            if delay is not None:
                execute(pc + 4, delay)

        # conditional branches
        if op in (1, 4, 5, 6, 7, 20, 21, 22, 23) and not (op == 1 and rt not in (0, 1, 2, 3)):
            target = (pc + 4 + (simm << 2)) & M32
            likely = op >= 20 or (op == 1 and rt in (2, 3))
            if op == 1:
                cond = None if a is None else ((a >> 31) == 1) == (rt in (0, 2))
            elif op & ~16 == 4:
                cond = None if a is None or b is None else a == b
            elif op & ~16 == 5:
                cond = None if a is None or b is None else a != b
            elif op & ~16 == 6:
                cond = None if a is None else (a >> 31 == 1 or a == 0)
            else:
                cond = None if a is None else (a >> 31 == 0 and a != 0)
            if target <= pc:
                # backward branch: a loop; record a BSS clear, then fall through
                base = zero_store_base(target, pc)
                if base is not None and info.bss_start is None:
                    bounds = loop_bounds(target, pc, base)
                    if bounds:
                        info.bss_start, info.bss_end = bounds
                        bss_exit_index = len(exits)
                if not likely:
                    run_delay()
                pc += 8
                continue
            if cond:
                run_delay()
                pc = target
            else:
                if not likely:
                    run_delay()
                pc += 8
            continue
        if op in (2, 3):
            target = ((pc + 4) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
            run_delay()
            if op == 3:
                exits.append((pc, target))
                for r in list(regs):
                    if r not in CALLEE_SAVED and r != 0:
                        regs.pop(r)
                put(RA, pc + 8)
                pc += 8
            elif 0 < target - pc <= 0x100:
                pc = target   # a short forward jump over data inside the entry routine
            else:
                exits.append((pc, target))
                break
            continue
        if op == 0 and (w & 63) in (0x08, 0x09):
            target = a
            run_delay()
            if (w & 63) == 0x09:
                if target is not None:
                    exits.append((pc, target))
                for r in list(regs):
                    if r not in CALLEE_SAVED and r != 0:
                        regs.pop(r)
                put(rd or RA, pc + 8)
                pc += 8
                continue
            if target is None or rs == RA:
                break
            exits.append((pc, target))   # jr to a known address hands over
            break
        if op == 0 and (w & 63) in (0x0C, 0x0D):
            break
        execute(pc, w)
        pc += 4

    info.calls = exits
    info.stack_top = _stack(snap, regs, exits)
    if bss_exit_index is not None and bss_exit_index < len(exits):
        info.main_call = exits[bss_exit_index][1]
    else:
        outside = [t for _, t in exits if not info.in_image(t)]
        info.main_call = outside[0] if outside else (exits[-1][1] if exits else None)
    info.staged = bool(info.main_call is not None and not info.in_image(info.main_call))


def _stack(snap: dict[int, dict[int, int]], regs: dict[int, int], exits: list[tuple[int, int]]):
    """The stack pointer as set when the routine first calls out (after any delay slot)."""
    for site, _ in exits:
        nxt = snap.get(site + 8)
        if nxt and SP in nxt:
            return nxt[SP]
    return regs.get(SP)
