"""A game's own decompressor, run in the boot emulator, as a codec of last resort.

When a loader unpacks the program with a codec n64dk does not implement, the routine that
did it is still in the boot image.  :func:`find` identifies it from the boot emulation: the
call (made by the entry routine) whose arguments point at the packed source and at the
program's destination.  :func:`decode` then calls that routine on any other bytes of the
ROM, so packed assets that share the program's codec can be recognised and unpacked
without knowing the format.  A decode is accepted only when the routine returns, reads its
input, and writes more bytes than it read.
"""
from __future__ import annotations

from dataclasses import dataclass

from . import emu
from .rom import Rom

SRC_AREA = 0x80600000   # input buffer (2 MiB below the end of 8 MiB RDRAM)
DST_AREA = 0x80300000   # output buffer
MAX_IN = 0x200000
MAX_OUT = 0x300000


@dataclass
class GameCodec:
    func: int                 # the decompressor routine
    src_arg: int              # argument index of the packed input
    dst_arg: int              # argument index of the output buffer
    args: tuple[int, ...]     # the arguments of the loader's call (others are kept as observed)
    size_arg: int | None = None   # argument index carrying the packed size, if any
    size_bias: int = 0            # that argument = size + bias (e.g. -1 for "last offset")
    source_rom: int | None = None  # exact ROM offset of the program's packed source

    def describe(self) -> str:
        size = f", size in $a{self.size_arg}" + (f"{self.size_bias:+d}" if self.size_bias else "") \
            if self.size_arg is not None else ""
        return (f"game routine 0x{self.func:08X} (input in $a{self.src_arg}, output in $a{self.dst_arg}{size}, "
                f"as the loader called it)")


@dataclass
class Decoded:
    data: bytes
    consumed: int             # input bytes read (rounded up to the read-tracking granule)
    steps: int


def find(img, calls: dict[int, tuple]) -> GameCodec | None:
    """The loader routine that unpacked ``img``'s main segment, from the first-entry
    arguments recorded by the emulator (``Machine.first_args``)."""
    main = img.main()
    if main.kind != "packed" or main.rom is None:
        return None
    srcs = _source_addresses(img)
    for _site, target in img.boot.calls:
        a = calls.get(target)
        if a is None or target == img.hand_over:
            continue
        regs = [x & 0xFFFFFFFF for x in a[:4]]
        dst = next((i for i, r in enumerate(regs) if main.vram - 0x10 <= (r | 0x80000000) <= main.vram + 0x10), None)
        src = next((i for i, r in enumerate(regs)
                    if i != dst and any(lo <= (r | 0x80000000) < hi for lo, hi, _rom in srcs)), None)
        if dst is not None and src is not None:
            codec = GameCodec(target, src, dst, tuple(regs))
            codec.source_rom = _ram_to_rom(img, regs[src] | 0x80000000)
            packed = main.rom_end - (codec.source_rom or main.rom)
            for i, r in enumerate(regs):
                if i not in (src, dst) and -2 <= r - packed <= 1:
                    codec.size_arg, codec.size_bias = i, r - packed
                    break
            return codec
    return None


def _source_addresses(img) -> list[tuple[int, int, int]]:
    """RAM ranges (lo, hi, rom of lo) that held the packed source: a DMA buffer filled from
    it, or the boot copy."""
    main, b = img.main(), img.boot
    out = []
    for d in img.loader.get("dmas", []):
        rom_lo, ram = int(d["rom"], 16), int(d["ram"], 16)
        rom_hi = int(d["rom_end"], 16)
        if rom_lo <= main.rom < rom_hi:
            out.append((ram, ram + (rom_hi - rom_lo), rom_lo))
    if b.main_rom <= main.rom < b.main_rom + b.copy_size:
        out.append((b.main_vram, b.main_vram + b.copy_size, b.main_rom))
    return out


def _ram_to_rom(img, addr: int) -> int | None:
    return next((rom + addr - lo for lo, hi, rom in _source_addresses(img) if lo <= addr < hi), None)


def decode(rom: Rom, codec: GameCodec, start: int, max_in: int = MAX_IN, max_steps: int = 30_000_000,
           src_offset: int = 0) -> Decoded | None:
    """Run the game's routine on ROM bytes from ``start``; None unless it returns having
    expanded its input.  ``src_offset`` places the input as far into the buffer as the
    loader's own argument pointed past a header it reads separately (normally 0)."""
    m = emu.Machine(rom, track_reads=True)
    chunk = rom.data[start:start + max_in]
    s = (SRC_AREA & 0x1FFFFFFF)
    m.ram[s:s + len(chunk)] = chunk
    d = DST_AREA & 0x1FFFFFFF
    m.ram[d:d + MAX_OUT] = bytes(MAX_OUT)
    before = bytes(m.dirty)
    args = list(codec.args)
    args[codec.src_arg] = SRC_AREA + src_offset
    args[codec.dst_arg] = DST_AREA
    if codec.size_arg is not None:
        args[codec.size_arg] = (len(chunk) + codec.size_bias) & 0xFFFFFFFF
    start_steps = m.steps
    reason = m.call(codec.func, tuple(args), max_steps=max_steps)
    if reason != "returned":
        return None
    # output: lines written in the destination area; input: lines read in the source area
    lines = 1 << emu.LINE
    written = [i for i in range(d // lines, (d + MAX_OUT) // lines) if m.dirty[i] and not before[i]]
    read = [i for i in range(s // lines, (s + len(chunk)) // lines) if m.reads[i]]
    if not written or not read:
        return None
    v0 = m.gpr[2] & 0xFFFFFFFF
    out_end = (written[-1] + 1) * lines
    size = v0 if 0 < v0 <= out_end - d and v0 > out_end - d - lines else out_end - d
    consumed = (read[-1] + 1) * lines - s
    data = bytes(m.ram[d:d + size])
    if size <= consumed or not any(data):
        return None
    return Decoded(data, min(consumed, len(chunk)), m.steps - start_steps)
