"""Boot emulation: run a game's entry code until it hands over to the loaded program.

Many games do not run directly from the 1 MiB that IPL3 copies: they DMA further code
from the cartridge, decompress it, or relocate it.  This module interprets the R4300
integer instruction set over RDRAM, the cartridge (read-only) and the PI DMA registers,
logging every DMA, every cartridge range the CPU reads directly, and every written RAM
range, and records the first time control reaches code that was written after boot (a
"handoff") and every ``eret`` (the libultra thread dispatcher entering a thread).  The
result is the real initial memory image of the program, which the static analyses can
use instead of assuming "ROM 0x1000.. copied to the header entry".

Instructions are translated once per address into small Python functions (cached until
the page holding them is written), which keeps long decompression loops affordable.

Out of scope by design: interrupts, TLB, FPU arithmetic, RSP/RDP.  Execution stops with
a reason when it reaches any of these.
"""
from __future__ import annotations

import struct
from collections import defaultdict
from dataclasses import dataclass, field

from .rom import Rom

M32 = 0xFFFFFFFF
M64 = 0xFFFFFFFFFFFFFFFF
RDRAM_SIZE = 0x800000
CART_BASE = 0x10000000
PI_BASE = 0x04600000
SP_MEM = 0x04000000
LINE = 8       # log2 of the write-tracking granule (256 bytes)

SKIP = -1      # likely branch not taken: skip the delay slot
NODELAY = -2   # jump without a delay slot (eret); target in Machine.jump_now

_U16 = struct.Struct(">H").unpack_from
_U32 = struct.Struct(">I").unpack_from
_U64 = struct.Struct(">Q").unpack_from
_P8 = struct.Struct(">B").pack_into
_P16 = struct.Struct(">H").pack_into
_P32 = struct.Struct(">I").pack_into
_P64 = struct.Struct(">Q").pack_into


def sx16(v: int) -> int:
    return v - 0x10000 if v & 0x8000 else v


def sx32(v: int) -> int:
    v &= M32
    return v | 0xFFFFFFFF00000000 if v & 0x80000000 else v


def _s32(v: int) -> int:
    v &= M32
    return v - 0x100000000 if v & 0x80000000 else v


def _s64(v: int) -> int:
    v &= M64
    return v - (1 << 64) if v >> 63 else v


def _sext(v: int, bits: int) -> int:
    return (v - (1 << bits)) & M64 if v >> (bits - 1) else v


def _tdiv(x: int, y: int) -> int:
    q = abs(x) // abs(y)
    return q if (x < 0) == (y < 0) else -q


class Stop(Exception):
    def __init__(self, reason: str, pc: int):
        super().__init__(reason)
        self.reason, self.pc = reason, pc


@dataclass
class Dma:
    rom: int
    ram: int
    size: int
    pc: int
    step: int


@dataclass
class EmuResult:
    steps: int
    stop_reason: str
    stop_pc: int
    dmas: list[Dma] = field(default_factory=list)
    written: list[tuple[int, int]] = field(default_factory=list)    # physical RAM ranges written after boot
    handoffs: list[tuple[int, int]] = field(default_factory=list)   # (step, pc) first execution of written code
    erets: list[tuple[int, int]] = field(default_factory=list)      # (step, target) thread dispatches
    cart_reads: list[tuple[int, int]] = field(default_factory=list)  # ROM ranges read by the CPU (4 KiB pages)
    probes: list[int] = field(default_factory=list)                  # open-bus physical addresses read
    calls: dict[int, int] = field(default_factory=dict)             # call target -> count
    ram: bytearray | None = None
    # The program image: state when control first reached written code or a thread
    # (before the game itself writes into its BSS).  None when neither happened.
    image_step: int | None = None
    image_ram: bytes | None = None
    image_written: list[tuple[int, int]] = field(default_factory=list)
    image_reads: list[tuple[int, int]] = field(default_factory=list)     # RAM read (track_reads only)
    image_executed: list[tuple[int, int]] = field(default_factory=list)  # RAM lines executed
    first_args: dict[int, tuple] = field(default_factory=dict)  # call target -> ($a0..$a3, $sp, step) on entry

    def pieces(self, at: str = "image") -> list[tuple[int, int, str, int | None]]:
        """Written RAM as (phys start, phys end, kind, ROM source) runs; see :meth:`segments`."""
        use_image = at == "image" and self.image_ram is not None
        ram = self.image_ram if use_image else self.ram
        written = self.image_written if use_image else self.written
        dmas = [d for d in self.dmas if not use_image or d.step < self.image_step]
        pieces: list[list] = []
        for a, b in written:
            for lo, hi, src in _split_by_dma(a, b, dmas):
                if src is not None:
                    pieces.append([lo, hi, "dma", src])
                    continue
                for x in range(lo, hi, 1 << LINE):
                    y = min(hi, x + (1 << LINE))
                    kind = "zero" if ram is not None and not any(ram[x:y]) else "generated"
                    if pieces and pieces[-1][2] == kind and pieces[-1][1] == x:
                        pieces[-1][1] = y
                    else:
                        pieces.append([x, y, kind, None])
        # A run shorter than a page between two contiguous neighbours of one kind takes
        # their kind: zero gaps inside decompressed output, and variables the game has
        # already written inside its BSS.
        merged: list[list] = []
        for i, p in enumerate(pieces):
            nxt = pieces[i + 1] if i + 1 < len(pieces) else None
            if (p[2] != "dma" and p[1] - p[0] < 0x1000 and merged and nxt and merged[-1][1] == p[0]
                    and nxt[0] == p[1] and merged[-1][2] == nxt[2] != "dma"):
                p = [p[0], p[1], nxt[2], None]
            if merged and p[2] == merged[-1][2] != "dma" and merged[-1][1] == p[0]:
                merged[-1][1] = p[1]
            else:
                merged.append(p)
        return [(lo, hi, kind, src) for lo, hi, kind, src in merged]

    def segments(self, at: str = "image") -> list[dict]:
        """Written RAM ranges with their origin, for laying out a decompilation's split.

        ``dma``: copied straight from the ROM range given.  ``zero``: cleared (BSS or
        heap).  ``generated``: written by the CPU with non-zero data, usually a loader's
        decompressed or relocated output.  ``at="image"`` (default) judges the state at the
        first handoff or thread dispatch; ``at="end"`` the state when the run stopped.
        """
        out = []
        for lo, hi, kind, src in self.pieces(at):
            seg = {"ram": f"0x{lo | 0x80000000:08X}..0x{hi | 0x80000000:08X}", "size": hi - lo, "kind": kind}
            if src is not None:
                seg["rom"] = f"0x{src:06X}..0x{src + hi - lo:06X}"
            out.append(seg)
        return out

    def summary(self) -> dict:
        return {"steps": self.steps, "stop": self.stop_reason, "stop_pc": f"0x{self.stop_pc:08X}",
                "dmas": len(self.dmas), "dma_bytes": sum(d.size for d in self.dmas),
                "cart_reads": [f"0x{a:06X}..0x{b:06X}" for a, b in self.cart_reads[:16]],
                "written_ranges": [f"0x{a | 0x80000000:08X}..0x{b | 0x80000000:08X}" for a, b in self.written[:16]],
                "handoffs": [f"step {s}: 0x{pc:08X}" for s, pc in self.handoffs[:16]],
                "erets": [f"step {s}: 0x{pc:08X}" for s, pc in self.erets[:16]],
                "image_step": self.image_step,
                "probes": [f"0x{p:08X}" for p in self.probes[:8]]}


class Machine:
    def __init__(self, rom: Rom, ram_size: int = RDRAM_SIZE, track_reads: bool = False):
        self.rom = rom.data
        self.track_reads = track_reads
        self.reads = bytearray(ram_size >> LINE)   # 256-byte lines loaded from (track_reads only)
        self.snapshot: tuple | None = None
        self.ram = bytearray(ram_size)
        self.ram_size = ram_size
        self.gpr = [0] * 32
        self.fpr = [0] * 32
        self.hi = self.lo = 0
        self.cop0 = [0] * 32
        self.count = 0
        self.pi = [0] * 16
        self.sp_mem = bytearray(0x2000)
        self.dmas: list[Dma] = []
        self.dirty = bytearray(ram_size >> LINE)  # 256-byte lines written after boot (stores and DMA)
        self.cart_pages: set[int] = set()      # ROM pages read directly by the CPU
        self.probes: dict[int, None] = {}
        self.calls: defaultdict[int, int] = defaultdict(int)
        self.last_call: int | None = None        # set by call instructions, consumed on entry
        self.first_args: dict[int, tuple] = {}  # call target -> (a0, a1, a2, a3, sp, step) on first entry
        self.steps = 0
        self.jump_now = 0
        self.erets: list[tuple[int, int]] = []
        self.decoded: dict[int, object] = {}
        self.code_pages: dict[int, list[int]] = {}
        self._templates: dict[tuple[int, int], object] = {}
        entry = rom.header.entry
        phys = entry & 0x1FFFFFFF
        image = rom.data[0x1000:0x101000]
        self.ram[phys:phys + len(image)] = image
        self.boot_pages = set(range(phys >> 12, (phys + len(image) + 0xFFF) >> 12))
        self.sp_mem[0:0x1000] = rom.data[0:0x1000]  # IPL3 leaves the header and boot code in DMEM
        # Boot globals IPL3 leaves at 0x80000300: osTvType, osRomType, osRomBase,
        # osResetType, osCicId, osVersion, osMemSize.
        struct.pack_into(">7I", self.ram, 0x300, 1, 0, 0xB0000000, 0, 6102, 0, min(ram_size, 0x400000))
        # Register state left by IPL3 (6102): stack in SP memory, boot flags.
        self.gpr[29] = sx32(0xA4001FF0)
        self.gpr[31] = sx32(0xA4001550)
        self.gpr[20] = 1        # s4: TV type NTSC
        self.gpr[22] = 0x3F     # s6: CIC seed
        self.cop0[12] = 0x34000000  # Status: CU0 | CU1 | FR
        self.pc = rom.entry_vram

    # ---- memory (slow path; the translated code handles plain RDRAM itself) ------
    def _phys(self, vaddr: int) -> int:
        v = vaddr & M32
        if 0x80000000 <= v < 0xC0000000:
            return v & 0x1FFFFFFF
        raise Stop(f"unmapped address 0x{v:08X} (TLB)", self.pc)

    def read(self, vaddr: int, size: int) -> int:
        p = self._phys(vaddr)
        if p < self.ram_size:
            if self.track_reads:
                self.reads[p >> LINE] = 1
            return int.from_bytes(self.ram[p:p + size], "big")
        if CART_BASE <= p < CART_BASE + len(self.rom):
            o = p - CART_BASE
            self.cart_pages.add(o >> 12)
            return int.from_bytes(self.rom[o:o + size], "big")
        if SP_MEM <= p < SP_MEM + 0x2000:
            o = p - SP_MEM
            return int.from_bytes(self.sp_mem[o:o + size], "big")
        if PI_BASE <= p < PI_BASE + 0x40:
            return 0 if (p & 0x3F) == 0x10 else self.pi[(p & 0x3F) >> 2]
        if 0x03F00000 <= p < 0x05000000 or 0x1FC00000 <= p < 0x1FC00800:
            return 0  # RDRAM/RCP registers and PIF: idle values
        if len(self.probes) < 64:
            self.probes[p] = None
        return 0      # open bus (development hardware probes, unused cartridge domains)

    def write(self, vaddr: int, size: int, value: int) -> None:
        p = self._phys(vaddr)
        if p < self.ram_size:
            self.ram[p:p + size] = (value & ((1 << (8 * size)) - 1)).to_bytes(size, "big")
            self.note_write(p, size)
            return
        if PI_BASE <= p < PI_BASE + 0x40:
            reg = (p & 0x3F) >> 2
            self.pi[reg] = value & M32
            if reg == 3:  # PI_WR_LEN: cartridge -> RDRAM
                self._pi_dma()
            return
        if SP_MEM <= p < SP_MEM + 0x2000:
            o = p - SP_MEM
            self.sp_mem[o:o + size] = (value & ((1 << (8 * size)) - 1)).to_bytes(size, "big")
            return
        # other hardware registers, PIF, cartridge space (save probes), open bus: ignored

    def note_write(self, p: int, size: int) -> None:
        lo, hi = p >> LINE, ((p + size - 1) >> LINE) + 1
        self.dirty[lo:hi] = b"\x01" * (hi - lo)
        for page in range(p >> 12, ((p + size - 1) >> 12) + 1):
            if page in self.code_pages:
                self.invalidate_page(page)

    def invalidate_page(self, page: int) -> None:
        for pc in self.code_pages.pop(page, ()):
            self.decoded.pop(pc, None)

    def _pi_dma(self) -> None:
        dram = self.pi[0] & 0x00FFFFFF & ~7
        cart = (self.pi[1] & 0x1FFFFFFF) & ~1
        size = (self.pi[3] & 0x00FFFFFF) + 1
        rom_off = cart - CART_BASE
        if not 0 <= rom_off < len(self.rom):
            raise Stop(f"PI DMA from non-ROM address 0x{cart:08X}", self.pc)
        chunk = self.rom[rom_off:rom_off + size]
        chunk = chunk[:max(0, self.ram_size - dram)]
        self.ram[dram:dram + len(chunk)] = chunk
        if chunk:
            self.note_write(dram, len(chunk))
        self.dmas.append(Dma(rom_off, dram | 0x80000000, len(chunk), self.pc, self.steps))

    def lwlr(self, op: int, addr: int, old: int) -> int:
        mem = self.read(addr & ~3, 4)
        k = addr & 3
        old &= M32
        if op == 34:  # lwl
            v = ((mem << (8 * k)) & M32) | (old & ((1 << (8 * k)) - 1))
        else:         # lwr
            v = (mem >> (8 * (3 - k))) | (old & ~((1 << (8 * (k + 1))) - 1) & M32)
        return sx32(v)

    def swlr(self, op: int, addr: int, val: int) -> None:
        aligned = addr & ~3
        mem = self.read(aligned, 4)
        k = addr & 3
        val &= M32
        if op == 42:  # swl
            v = (mem & (~(M32 >> (8 * k)) & M32)) | (val >> (8 * k))
        else:         # swr
            keep = (M32 >> (8 * (k + 1))) if k < 3 else 0
            v = (mem & keep) | ((val << (8 * (3 - k))) & M32)
        self.write(aligned, 4, v)

    def mfc0(self, reg: int) -> int:
        if reg == 9:  # Count: advance on every read so timing loops terminate
            self.count = (self.count + 0x400) & M32
            return self.count
        return self.cop0[reg]

    def eret(self, pc: int) -> int:
        target = self.cop0[14] & M32
        self.cop0[12] &= ~2
        self.erets.append((self.steps, target))
        self.jump_now = target
        return NODELAY

    # ---- translation ---------------------------------------------------------------
    def translate(self, pc: int):
        tracking, self.track_reads = self.track_reads, False   # fetches are not data reads
        word = self.read(pc, 4)
        self.track_reads = tracking
        key = (pc & M32, word)
        fn = self._templates.get(key)
        if fn is None:
            src = _translate(pc & M32, word, self.track_reads)
            ns = {"g": self.gpr, "m": self, "ram": self.ram, "RS": self.ram_size,
                  "dirty": self.dirty, "reads": self.reads, "code": self.code_pages, "calls": self.calls,
                  "X": sx32, "s32": _s32, "s64": _s64, "sext": _sext, "tdiv": _tdiv,
                  "U16": _U16, "U32": _U32, "U64": _U64, "P8": _P8, "P16": _P16, "P32": _P32,
                  "P64": _P64, "Stop": Stop, "M32": M32, "M64": M64}
            exec(src, ns)
            fn = ns["f"]
            self._templates[key] = fn
        self.decoded[pc] = fn
        self.code_pages.setdefault((pc & 0x1FFFFFFF) >> 12, []).append(pc)
        return fn

    RETURN_TRAP = 0x80000010   # inside the TLB-miss vector: never a call's legitimate return address

    def call(self, func: int, args: tuple[int, ...] = (), sp: int = 0x803FFF00, max_steps: int = 20_000_000) -> str:
        """Call ``func`` with ``args`` in $a0..$a3 and run until it returns; the stop reason
        (``"returned"`` on success)."""
        g = self.gpr
        for i, v in enumerate(args[:4]):
            g[4 + i] = sx32(v)
        g[29] = sx32(sp)
        g[31] = sx32(self.RETURN_TRAP)
        self.pc = func & M32
        self.last_call = None
        start = self.steps
        res = self.run(max_steps=start + max_steps, stop_after_handoff=1 << 62, stop_at=self.RETURN_TRAP)
        return res.stop_reason

    def take_snapshot(self, steps: int) -> None:
        """Record the program image (first handoff or thread dispatch)."""
        if self.snapshot is None:
            executed = bytearray(len(self.dirty))
            for pc in self.decoded:
                p = pc & 0x1FFFFFFF
                if p < self.ram_size:
                    executed[p >> LINE] = 1
            self.snapshot = (steps, bytes(self.ram), bytes(self.dirty), bytes(self.reads), bytes(executed))

    # ---- execution -------------------------------------------------------------
    def run(self, max_steps: int = 50_000_000, stop_after_handoff: int = 200_000,
            stop_at: int | None = None) -> EmuResult:
        """Run from ``self.pc``.  ``stop_at``: end when control transfers to this address
        (the return address of :meth:`call`)."""
        handoffs: list[tuple[int, int]] = []
        seen: set[int] = set()
        decoded, translate, dirty = self.decoded, self.translate, self.dirty
        nlines = len(dirty)
        reason = "step budget exhausted"
        deadline = max_steps
        pc = self.pc & M32
        pending = None
        cur_line = -1
        steps = self.steps
        try:
            while steps < deadline:
                line = (pc & 0x1FFFFFFF) >> LINE
                if line != cur_line:
                    cur_line = line
                    if line < nlines and dirty[line] and line >> (12 - LINE) not in seen:
                        seen.add(line >> (12 - LINE))
                        if not handoffs:
                            self.take_snapshot(steps)
                        handoffs.append((steps, pc))
                        deadline = min(deadline, steps + stop_after_handoff)
                fn = decoded.get(pc)
                if fn is None:
                    self.pc = pc
                    fn = translate(pc)
                self.steps = steps
                r = fn()
                steps += 1
                if pending is not None:
                    if r == NODELAY:
                        pc = self.jump_now
                    else:
                        pc = pending
                    pending = None
                    if self.last_call is not None:   # entering a called function
                        if pc == self.last_call and pc not in self.first_args:
                            g = self.gpr
                            self.first_args[pc] = (g[4] & M32, g[5] & M32, g[6] & M32, g[7] & M32,
                                                   g[29] & M32, steps)
                        self.last_call = None
                    if pc == stop_at:
                        reason = "returned"
                        break
                elif r is None:
                    pc = (pc + 4) & M32
                elif r == SKIP:
                    pc = (pc + 8) & M32
                elif r == NODELAY:
                    pc = self.jump_now
                    self.take_snapshot(steps)
                    deadline = min(deadline, steps + stop_after_handoff)
                    cur_line = -1
                else:
                    pending = r
                    pc = (pc + 4) & M32
            if reason == "returned":
                pass
            elif steps >= max_steps:
                reason = "step budget exhausted"
            elif handoffs or self.erets:
                reason = "ran past the first handoff"
        except Stop as stop:
            reason = stop.reason  # pc still holds the faulting instruction
        self.steps, self.pc = steps, pc
        res = EmuResult(steps, reason, pc & M32, self.dmas, _line_ranges(dirty), handoffs, self.erets,
                        _ranges(self.cart_pages), list(self.probes), dict(self.calls), self.ram)
        res.first_args = dict(self.first_args)
        if self.snapshot is not None:
            step, ram, written, reads, executed = self.snapshot
            res.image_step, res.image_ram = step, ram
            res.image_written = _line_ranges(bytearray(written))
            res.image_reads = _line_ranges(bytearray(reads))
            res.image_executed = _line_ranges(bytearray(executed))
        return res


def _translate(pc: int, word: int, track_reads: bool = False) -> str:
    """Python source for one instruction, as ``def f():`` returning the control-flow result."""
    op = word >> 26
    rs, rt, rd = (word >> 21) & 31, (word >> 16) & 31, (word >> 11) & 31
    sa, fn = (word >> 6) & 31, word & 63
    imm = word & 0xFFFF
    simm = sx16(imm)
    body: list[str]
    A, B = f"g[{rs}]", f"g[{rt}]"
    target = (pc + 4 + (simm << 2)) & M32

    def setr(r: int, expr: str) -> list[str]:
        return [f"g[{r}] = {expr}"] if r else []

    def stop(why: str) -> list[str]:
        return [f"raise Stop({why!r}, {pc})"]

    def branch(cond: str, likely: bool, link: bool = False) -> list[str]:
        out = [f"c = {cond}"]
        if link:
            out.append(f"g[31] = {sx32(pc + 8)}")
        out.append(f"return {target} if c else {SKIP if likely else None}")
        return out

    if op == 0:
        alu = {
            0x00: f"X(({B} << {sa}) & M32)", 0x02: f"X(({B} & M32) >> {sa})", 0x03: f"X(s32({B}) >> {sa})",
            0x04: f"X(({B} << ({A} & 31)) & M32)", 0x06: f"X(({B} & M32) >> ({A} & 31))",
            0x07: f"X(s32({B}) >> ({A} & 31))",
            0x14: f"({B} << ({A} & 63)) & M64", 0x16: f"{B} >> ({A} & 63)", 0x17: f"(s64({B}) >> ({A} & 63)) & M64",
            0x20: f"X({A} + {B})", 0x21: f"X({A} + {B})", 0x22: f"X({A} - {B})", 0x23: f"X({A} - {B})",
            0x24: f"{A} & {B}", 0x25: f"{A} | {B}", 0x26: f"{A} ^ {B}", 0x27: f"~({A} | {B}) & M64",
            0x2A: f"int(s64({A}) < s64({B}))", 0x2B: f"int({A} < {B})",
            0x2C: f"({A} + {B}) & M64", 0x2D: f"({A} + {B}) & M64", 0x2E: f"({A} - {B}) & M64", 0x2F: f"({A} - {B}) & M64",
            0x38: f"({B} << {sa}) & M64", 0x3A: f"{B} >> {sa}", 0x3B: f"(s64({B}) >> {sa}) & M64",
            0x3C: f"({B} << {sa + 32}) & M64", 0x3E: f"{B} >> {sa + 32}", 0x3F: f"(s64({B}) >> {sa + 32}) & M64",
            0x10: "m.hi", 0x12: "m.lo",
        }
        if fn in alu:
            body = setr(rd, alu[fn])
        elif fn == 0x08:
            body = [f"return {A} & M32"]
        elif fn == 0x09:
            body = [f"t = {A} & M32"] + setr(rd, str(sx32(pc + 8))) + ["calls[t] += 1", "m.last_call = t",
                                                                        "return t"]
        elif fn in (0x0C, 0x0D):
            body = stop("syscall" if fn == 0x0C else "break")
        elif fn == 0x0F or 0x30 <= fn <= 0x36:
            body = []  # sync, traps (assumed not taken)
        elif fn == 0x11:
            body = [f"m.hi = {A}"]
        elif fn == 0x13:
            body = [f"m.lo = {A}"]
        elif fn in (0x18, 0x19):
            x, y = (f"s32({A})", f"s32({B})") if fn == 0x18 else (f"({A} & M32)", f"({B} & M32)")
            body = [f"p = {x} * {y}", "m.lo = X(p)", "m.hi = X(p >> 32)"]
        elif fn in (0x1A, 0x1B):
            x, y = (f"s32({A})", f"s32({B})") if fn == 0x1A else (f"({A} & M32)", f"({B} & M32)")
            body = [f"x = {x}", f"y = {y}", "if y:", " q = tdiv(x, y)", " m.lo = X(q)", " m.hi = X(x - q * y)"]
        elif fn in (0x1C, 0x1D):
            x, y = (f"s64({A})", f"s64({B})") if fn == 0x1C else (A, B)
            body = [f"p = {x} * {y}", "m.lo = p & M64", "m.hi = (p >> 64) & M64"]
        elif fn in (0x1E, 0x1F):
            x, y = (f"s64({A})", f"s64({B})") if fn == 0x1E else (A, B)
            body = [f"x = {x}", f"y = {y}", "if y:", " q = tdiv(x, y)", " m.lo = q & M64",
                    " m.hi = (x - q * y) & M64"]
        else:
            body = stop(f"unimplemented SPECIAL 0x{fn:02X}")
    elif op == 1:  # REGIMM
        if rt in (0x00, 0x02, 0x10, 0x12):
            body = branch(f"{A} >> 63", rt in (0x02, 0x12), rt >= 0x10)
        elif rt in (0x01, 0x03, 0x11, 0x13):
            body = branch(f"not {A} >> 63", rt in (0x03, 0x13), rt >= 0x10)
        elif 0x08 <= rt <= 0x0E:
            body = []  # trap immediates
        else:
            body = stop(f"unimplemented REGIMM 0x{rt:02X}")
    elif op in (2, 3):
        dest = ((pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
        body = [f"g[31] = {sx32(pc + 8)}", f"calls[{dest}] += 1", f"m.last_call = {dest}"] if op == 3 else []
        body.append(f"return {dest}")
    elif op in (4, 5, 6, 7, 20, 21, 22, 23):
        base = op & ~16
        cond = {4: f"{A} == {B}", 5: f"{A} != {B}", 6: f"({A} >> 63) or not {A}",
                7: f"not ({A} >> 63) and {A} != 0"}[base]
        body = branch(cond, op >= 20)
    elif op in (8, 9):
        body = setr(rt, f"X({A} + {simm})")
    elif op in (24, 25):
        body = setr(rt, f"({A} + {simm}) & M64")
    elif op == 10:
        body = setr(rt, f"int(s64({A}) < {simm})")
    elif op == 11:
        body = setr(rt, f"int({A} < {simm & M64})")
    elif op == 12:
        body = setr(rt, f"{A} & {imm}")
    elif op == 13:
        body = setr(rt, f"{A} | {imm}")
    elif op == 14:
        body = setr(rt, f"{A} ^ {imm}")
    elif op == 15:
        body = setr(rt, str(sx32(imm << 16)))
    elif op == 16:  # COP0
        if rs in (0, 1):
            body = setr(rt, f"X(m.mfc0({rd}))")
        elif rs in (4, 5):
            body = [f"m.cop0[{rd}] = {B} & M32"]
        elif rs == 16 and fn == 0x18:
            body = [f"return m.eret({pc})"]
        else:
            body = []  # TLB maintenance, wait
    elif op == 17:  # COP1: moves and condition branches only
        if rs == 0:
            body = setr(rt, f"X(m.fpr[{rd}])")
        elif rs == 1:
            body = setr(rt, f"m.fpr[{rd}] & M64")
        elif rs == 2:
            body = setr(rt, "0")
        elif rs == 4:
            body = [f"m.fpr[{rd}] = {B} & M32"]
        elif rs == 5:
            body = [f"m.fpr[{rd}] = {B}"]
        elif rs == 6:
            body = []
        elif rs == 8:  # bc1f/bc1t/bc1fl/bc1tl with the condition flag clear
            body = branch("True" if not rt & 1 else "False", bool(rt & 2))
        else:
            body = stop("FPU arithmetic")
    elif op == 47:
        body = []  # cache
    elif op in (32, 33, 35, 36, 37, 39, 55, 48, 49, 53):
        size = {32: 1, 36: 1, 33: 2, 37: 2, 35: 4, 39: 4, 48: 4, 49: 4, 55: 8, 53: 8}[op]
        fast = {1: "ram[p]", 2: "U16(ram, p)[0]", 4: "U32(ram, p)[0]", 8: "U64(ram, p)[0]"}[size]
        conv = {32: "sext(v, 8)", 33: "sext(v, 16)", 35: "X(v)", 48: "X(v)"}.get(op, "v")
        if op in (49, 53):
            dst = f"m.fpr[{rt}] = v"
        else:
            dst = f"g[{rt}] = {conv}" if rt else "pass"
        body = [f"a = ({A} + {simm}) & M32", "p = a & 0x1FFFFFFF",
                f"v = {fast} if a >> 30 == 2 and p < RS - {size - 1} else m.read(a, {size})", dst]
        if track_reads:
            body.append(f"if p < RS: reads[p >> {LINE}] = 1")
    elif op in (34, 38):
        body = [f"a = ({A} + {simm}) & M32", f"v = m.lwlr({op}, a, {B})"] + setr(rt, "v")
    elif op in (40, 41, 43, 63, 57, 61, 56):
        size = {40: 1, 41: 2, 43: 4, 56: 4, 57: 4, 63: 8, 61: 8}[op]
        src = f"m.fpr[{rt}]" if op in (57, 61) else B
        mask = {1: "0xFF", 2: "0xFFFF", 4: "M32", 8: "M64"}[size]
        pack = {1: "P8", 2: "P16", 4: "P32", 8: "P64"}[size]
        body = [f"a = ({A} + {simm}) & M32", "p = a & 0x1FFFFFFF", f"v = {src} & {mask}",
                f"if a >> 30 == 2 and p < RS - {size - 1}:",
                f" {pack}(ram, p, v)", f" dirty[p >> {LINE}] = 1",
                " if p >> 12 in code: m.invalidate_page(p >> 12)",
                "else:", f" m.write(a, {size}, v)"]
        if op == 56:
            body += setr(rt, "1")
    elif op in (42, 46):
        body = [f"m.swlr({op}, ({A} + {simm}) & M32, {B})"]
    elif op in (26, 27, 44, 45):
        body = stop("ldl/ldr/sdl/sdr")
    else:
        body = stop(f"unimplemented opcode 0x{op:02X}")
    lines = ["def f():"] + ["    " + ln for ln in (body or ["pass"])]
    return "\n".join(lines) + "\n"


def _ranges(pages: set[int]) -> list[tuple[int, int]]:
    out: list[list[int]] = []
    for p in sorted(pages):
        if out and out[-1][1] == p << 12:
            out[-1][1] = (p + 1) << 12
        else:
            out.append([p << 12, (p + 1) << 12])
    return [(a, b) for a, b in out]


def _split_by_dma(a: int, b: int, dmas: list[Dma]) -> list[tuple[int, int, int | None]]:
    """Cut physical range [a, b) into pieces covered by the last DMA to each byte, or none."""
    cuts = {a, b}
    for d in dmas:
        lo, hi = d.ram & 0x1FFFFFFF, (d.ram & 0x1FFFFFFF) + d.size
        cuts.update(x for x in (lo, hi) if a < x < b)
    edges = sorted(cuts)
    out = []
    for lo, hi in zip(edges, edges[1:]):
        src = None
        for d in reversed(dmas):
            dlo = d.ram & 0x1FFFFFFF
            if dlo <= lo and hi <= dlo + d.size:
                src = d.rom + lo - dlo
                break
        out.append((lo, hi, src))
    return out


def _line_ranges(lines: bytearray) -> list[tuple[int, int]]:
    out: list[tuple[int, int]] = []
    i, n = 0, len(lines)
    while (i := lines.find(1, i)) >= 0:
        j = lines.find(0, i)
        j = n if j < 0 else j
        out.append((i << LINE, j << LINE))
        i = j
    return out


def run(rom: Rom, max_steps: int = 50_000_000, stop_after_handoff: int = 200_000,
        track_reads: bool = False) -> EmuResult:
    return Machine(rom, track_reads=track_reads).run(max_steps, stop_after_handoff)
