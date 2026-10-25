"""Program image: where each byte of the program's initial RAM comes from.

A decompilation splits the ROM by where its bytes end up in RAM.  For most games that is
the IPL3 copy (ROM 0x1000.. to the header entry) plus a BSS the entry routine clears, and
the static boot analysis is enough.  Games whose entry routine runs a loader first (a
staged boot, or calls before the hand-over: decompressing or DMAing the real program) are
emulated up to the hand-over; the written RAM is then attributed to its source.

Segments are ``rom`` (copied verbatim: ``rom`` is the ROM offset of the first byte),
``packed`` (written by a loader: ``rom``..``rom_end`` is the packed source when it could
be traced, ``codec`` names a known codec that reproduces it) or ``bss`` (zero-filled).
``ProgramImage.boot_info()`` and ``main_data()`` give the main segment in the form the
analyses (``analysis.functions``, ``analysis.datamap``, ``sdk``) take; for a packed
segment the offsets they report are offsets into the unpacked segment, not ROM offsets.
"""
from __future__ import annotations

from dataclasses import dataclass, field

from . import boot as bootmod
from . import emu
from .rom import Rom

LOW_MEMORY = 0x400        # exception vectors and IPL3 boot globals: runtime state, not program


@dataclass
class Segment:
    vram: int
    size: int
    kind: str                     # rom | packed | bss
    rom: int | None = None
    rom_end: int | None = None
    codec: str | None = None
    unpacked_size: int | None = None   # codec output size, when a codec reproduces the segment
    game_codec: str | None = None      # the game's own routine that unpacks it (n64dk.gamecodec)
    evidence: str = ""
    data: bytes = field(default=b"", repr=False)

    @property
    def end(self) -> int:
        return self.vram + self.size

    def as_dict(self) -> dict:
        d = {"vram": f"0x{self.vram:08X}", "end": f"0x{self.end:08X}", "size": self.size, "kind": self.kind}
        if self.rom is not None:
            d["rom"] = f"0x{self.rom:X}"
            d["rom_end"] = f"0x{self.rom_end:X}"
        if self.codec:
            d["codec"] = self.codec
        if self.game_codec:
            d["game_codec"] = self.game_codec
        if self.evidence:
            d["evidence"] = self.evidence
        return d


@dataclass
class ProgramImage:
    method: str                   # static | emulated
    entry: int                    # where IPL3 jumps
    hand_over: int | None         # first program code after the entry routine / loader
    stack_top: int | None
    segments: list[Segment]
    boot: bootmod.BootInfo        # static analysis of the entry routine
    loader: dict = field(default_factory=dict)   # emulation facts (emulated images only)
    call_args: dict = field(default_factory=dict, repr=False)  # call target -> arguments on entry

    def main(self) -> Segment:
        """The code segment holding the hand-over (static images: the IPL3 copy)."""
        target = self.hand_over if self.method == "emulated" and self.hand_over else self.entry
        for s in self.segments:
            if s.kind != "bss" and s.vram <= target < s.end:
                return s
        return next(s for s in self.segments if s.kind != "bss")

    def bss(self) -> Segment | None:
        main = self.main()
        return next((s for s in self.segments if s.kind == "bss" and s.vram == main.end), None)

    def boot_info(self) -> bootmod.BootInfo:
        """BootInfo describing the main segment, for the analyses."""
        if self.method == "static":
            return self.boot
        main = self.main()
        bss = next((s for s in self.segments if s.kind == "bss"), None)
        return bootmod.BootInfo(
            entry=self.hand_over or main.vram, main_rom=main.rom if main.kind == "rom" else 0,
            main_vram=main.vram, copy_size=main.size, stack_top=self.stack_top,
            bss_start=bss.vram if bss else None, bss_end=bss.end if bss else None,
            main_call=self.hand_over, staged=True)

    def main_data(self, rom: Rom) -> bytes:
        """Bytes indexed the way ``boot_info().main_rom`` expects."""
        main = self.main()
        return rom.data if self.method == "static" or main.kind == "rom" else main.data

    def summary(self) -> dict:
        out = {"method": self.method, "entry": f"0x{self.entry:08X}",
               "hand_over": self.hand_over and f"0x{self.hand_over:08X}",
               "stack_top": self.stack_top and f"0x{self.stack_top:08X}",
               "main": self.main().as_dict(), "segments": [s.as_dict() for s in self.segments]}
        if self.loader:
            out["loader"] = self.loader
        return out


def needs_emulation(b: bootmod.BootInfo) -> bool:
    """A loader runs before the program: the hand-over leaves the boot image, or the entry
    routine calls anything before handing over (a plain libultra entry only clears BSS)."""
    return b.staged or len(b.calls) > 1


def build(rom: Rom, mode: str = "auto", max_steps: int = 150_000_000) -> ProgramImage:
    """``mode``: ``auto`` (emulate only when the entry routine runs a loader), ``static``
    or ``emulated``."""
    b = bootmod.analyse(rom)
    if mode == "static" or (mode == "auto" and not needs_emulation(b)):
        return static_image(rom, b)
    return emulated_image(rom, b, max_steps)


def static_image(rom: Rom, b: bootmod.BootInfo) -> ProgramImage:
    size = min(b.copy_size, rom.size - b.main_rom)
    if b.bss_start and b.main_vram < b.bss_start < b.main_vram + size:
        size = b.bss_start - b.main_vram
    segs = [Segment(b.main_vram, size, "rom", b.main_rom, b.main_rom + size, evidence="IPL3 copy",
                    data=rom.data[b.main_rom:b.main_rom + size])]
    if b.bss_start and b.bss_end and b.bss_end > b.bss_start:
        segs.append(Segment(b.bss_start, b.bss_end - b.bss_start, "bss", evidence="entry routine clear loop"))
    return ProgramImage("static", b.entry, b.main_call, b.stack_top, segs, b)


def emulated_image(rom: Rom, b: bootmod.BootInfo, max_steps: int = 150_000_000) -> ProgramImage:
    res = emu.run(rom, max_steps=max_steps, stop_after_handoff=20_000, track_reads=True)
    hand_over = res.handoffs[0][1] if res.handoffs else (res.erets[0][1] if res.erets else b.main_call)
    loader = {"steps": res.steps, "image_step": res.image_step, "stop": res.stop_reason,
              "dmas": [{"rom": f"0x{d.rom:X}", "rom_end": f"0x{d.rom + d.size:X}", "ram": f"0x{d.ram:08X}"}
                       for d in res.dmas if res.image_step is None or d.step < res.image_step][:32],
              "calls": [f"0x{t:08X}" for _s, t in b.calls]}
    if res.image_ram is None:
        loader["warning"] = "no hand-over reached; falling back to the static image"
        img = static_image(rom, b)
        img.loader = loader
        return img
    ram = res.image_ram
    pieces = [p for p in res.pieces("image") if p[1] > LOW_MEMORY]
    pieces = [(max(lo, LOW_MEMORY), hi, kind, src) for lo, hi, kind, src in pieces]
    target = (hand_over or 0) & 0x1FFFFFFF
    idx = next((i for i, p in enumerate(pieces) if p[0] <= target < p[1] and p[2] != "zero"), None)
    if idx is None:
        loader["warning"] = "hand-over is not in written RAM; using the static image"
        img = static_image(rom, b)
        img.loader = loader
        img.hand_over = hand_over
        return img
    lo, hi, kind, src = pieces[idx]
    # contiguous zero runs after the main piece form its BSS
    bss_hi = hi
    for p in pieces[idx + 1:]:
        if p[0] != bss_hi or p[2] != "zero":
            break
        bss_hi = p[1]
    vram = lo | 0x80000000
    if kind == "dma":
        main = Segment(vram, hi - lo, "rom", src, src + hi - lo, evidence="PI DMA by the loader",
                       data=bytes(ram[lo:hi]))
    else:
        main = Segment(vram, hi - lo, "packed", evidence="written by the loader", data=bytes(ram[lo:hi]))
        _trace_source(rom, b, res, main)
    # Exact end and BSS: the loader's own BSS bounds, else the second stage's entry routine
    # (the same walk over the unpacked code at the hand-over), else the codec output size,
    # else trailing zeros trimmed.
    stage2 = bootmod.analyse_segment(main.data, main.vram, hand_over) if hand_over else None
    loader["stage2"] = stage2 and {
        "stack_top": stage2.stack_top and f"0x{stage2.stack_top:08X}",
        "bss": stage2.bss_start and f"0x{stage2.bss_start:08X}..0x{stage2.bss_end:08X}",
        "hand_over": stage2.main_call and f"0x{stage2.main_call:08X}"}
    data_end = main.vram + len(main.data.rstrip(b"\0"))
    bss: tuple[int, int, str] | None = None
    if b.bss_start and main.vram < b.bss_start <= main.end:
        end, how = b.bss_start, "BSS start from the loader"
        bss = (b.bss_start, b.bss_end, "loader clear loop")
    elif stage2 and stage2.bss_start and data_end <= stage2.bss_start <= main.end + 0x10000:
        end, how = min(stage2.bss_start, main.end), "BSS start from the second stage's entry routine"
        bss = (stage2.bss_start, stage2.bss_end, "second-stage clear loop")
    elif main.unpacked_size:
        end, how = main.vram + main.unpacked_size, "codec output size"
    else:
        end, how = (data_end + 15) & ~15, "trailing zero bytes trimmed (16-byte aligned)"
    if end < main.end:
        main.size = end - main.vram
        main.data = main.data[:main.size]
        if main.kind == "rom":
            main.rom_end = main.rom + main.size
    main.evidence += f"; end: {how}"
    if bss is None and bss_hi > hi:
        bss = (main.end, bss_hi | 0x80000000, "zero at the hand-over")
    segs = [main]
    if bss and bss[1] > bss[0]:
        segs.append(Segment(bss[0], bss[1] - bss[0], "bss", evidence=bss[2]))
    other = [f"0x{a | 0x80000000:08X}..0x{z | 0x80000000:08X} {k}" for a, z, k, _s in pieces
             if not (lo <= a < bss_hi)]
    if other:
        loader["other_writes"] = other[:16]
    # ROM ranges of the loader code that ran from the boot copy (256-byte granularity)
    boot_lo = b.main_vram & 0x1FFFFFFF
    loader["code_rom"] = [(b.main_rom + max(a, boot_lo) - boot_lo, b.main_rom + min(z, boot_lo + b.copy_size) - boot_lo)
                          for a, z in res.image_executed if z > boot_lo and a < boot_lo + b.copy_size]
    stack = (stage2.stack_top if stage2 and stage2.stack_top else None) or b.stack_top
    prog = ProgramImage("emulated", b.entry, hand_over, stack, segs, b, loader, res.first_args)
    if main.kind == "packed" and not main.codec:
        # The routine that unpacked it, called by the loader with the packed source as an
        # argument: that argument is the exact start (read tracking alone is line-granular).
        from . import gamecodec
        codec = gamecodec.find(prog, res.first_args)
        if codec is not None:
            main.game_codec = codec.describe()
            if codec.source_rom is not None and (main.rom is None or abs(codec.source_rom - main.rom) < 0x1000):
                main.rom = codec.source_rom
                main.evidence = f"packed source passed to the game routine 0x{codec.func:08X}; " + main.evidence
    return prog


def _trace_source(rom: Rom, b: bootmod.BootInfo, res: emu.EmuResult, main: Segment) -> None:
    """Find the packed source of a loader-written segment: RAM the loader read (not code it
    ran, not the output itself) mapped back to ROM through the DMAs and the boot copy."""
    executed = res.image_executed
    out_lo, out_hi = main.vram & 0x1FFFFFFF, main.end & 0x1FFFFFFF
    dmas = [d for d in res.dmas if d.step < (res.image_step or 0)]
    boot_lo = b.main_vram & 0x1FFFFFFF
    boot_hi = boot_lo + b.copy_size
    best = None
    for a, z in _subtract(res.image_reads, executed + [(out_lo, out_hi)]):
        dma = next((d for d in reversed(dmas) if (d.ram & 0x1FFFFFFF) <= a < (d.ram & 0x1FFFFFFF) + d.size), None)
        if dma is not None:
            cand = (dma.size, dma.rom, dma.rom + dma.size, f"loader read the DMA buffer at 0x{dma.ram:08X}")
        elif boot_lo <= a < boot_hi:
            z = min(z, boot_hi)
            cand = (z - a, b.main_rom + a - boot_lo, b.main_rom + z - boot_lo,
                    "loader read the boot copy (256-byte granularity)")
        else:
            continue
        if best is None or cand[0] > best[0]:
            best = cand
    if best is None:
        main.evidence += "; packed source not traced"
        return
    _, main.rom, main.rom_end, why = best
    main.evidence = why
    _identify_codec(rom, main)


def _subtract(ranges: list[tuple[int, int]], cuts: list[tuple[int, int]]) -> list[tuple[int, int]]:
    out = []
    for a, z in ranges:
        pieces = [(a, z)]
        for x, y in cuts:
            pieces = [p for lo, hi in pieces for p in ((lo, min(hi, x)), (max(lo, y), hi)) if p[0] < p[1]]
        out += pieces
    return out


def _identify_codec(rom: Rom, main: Segment) -> None:
    from .assets import compress
    probe = main.data[:0x100]
    if not probe:
        return
    for start in sorted({main.rom, main.rom + 4, main.rom + 8, main.rom + 0x10}):
        for name in compress.REGISTRY:
            try:
                d = compress.decode(name, rom.data, start, max_output=main.size + 0x10000)
            except Exception:
                continue
            if d.data[:len(probe)] == probe:
                main.codec, main.unpacked_size = name, len(d.data)
                main.evidence = f"decoded {len(d.data):X} bytes with {name} at ROM 0x{start:X}; " + main.evidence
                return
