"""Core commands: ROM information and disassembly."""
from __future__ import annotations

from .. import boot, mips
from ..cli import command
from ..rom import Rom


def _rom_arg(p):
    p.add_argument("rom", help="ROM image (z64, v64 or n64 byte order)")


@command("info", "ROM header, boot code, checksum and entry/BSS analysis")
def info(args):
    rom = Rom.load(args.rom)
    b = boot.analyse(rom)
    out = rom.summary()
    out.update({"main_segment": f"ROM 0x{b.main_rom:X} -> 0x{b.main_vram:08X} (0x{b.copy_size:X} bytes)",
                "stack_top": b.stack_top and f"0x{b.stack_top:08X}",
                "bss": b.bss_start and f"0x{b.bss_start:08X}..0x{b.bss_end:08X}",
                "hand_over": b.main_call and f"0x{b.main_call:08X}",
                "staged": b.staged})
    return out


info.arguments = _rom_arg


@command("disasm", "Disassemble a VRAM range of the main segment")
def disasm(args):
    rom = Rom.load(args.rom)
    b = boot.analyse(rom)
    start = int(args.vram, 16)
    size = int(args.size, 16)
    off = b.vram_to_rom(start)
    return "\n".join(mips.disassemble(rom.data[off:off + size], start))


def _disasm_args(p):
    _rom_arg(p)
    p.add_argument("vram", help="start VRAM (hex)")
    p.add_argument("size", help="byte count (hex)")


disasm.arguments = _disasm_args
