"""Boot emulation command: where the program's RAM image comes from."""
from __future__ import annotations

from .. import boot, emu
from ..cli import command
from ..rom import Rom


@command("boot-emu", "Emulate the entry code and map the loaded RAM image back to its ROM sources")
def boot_emu(args):
    rom = Rom.load(args.rom)
    b = boot.analyse(rom)
    res = emu.run(rom, max_steps=args.max_steps, stop_after_handoff=args.after)
    out = {"static": {"stack_top": b.stack_top and f"0x{b.stack_top:08X}",
                      "bss": b.bss_start and f"0x{b.bss_start:08X}..0x{b.bss_end:08X}",
                      "hand_over": b.main_call and f"0x{b.main_call:08X}",
                      "staged": b.staged},
           "emulation": res.summary(),
           "dmas": [{"rom": f"0x{d.rom:06X}..0x{d.rom + d.size:06X}", "ram": f"0x{d.ram:08X}",
                     "size": d.size, "pc": f"0x{d.pc:08X}", "step": d.step} for d in res.dmas[:args.dma_limit]],
           "segments": res.segments()}
    if len(res.dmas) > args.dma_limit:
        out["dmas_omitted"] = len(res.dmas) - args.dma_limit
    return out


def _args(p):
    p.add_argument("rom", help="ROM image (z64, v64 or n64 byte order)")
    p.add_argument("--max-steps", type=int, default=100_000_000, help="instruction budget (default 100M)")
    p.add_argument("--after", type=int, default=200_000,
                   help="instructions to keep running after the first handoff or thread dispatch")
    p.add_argument("--dma-limit", type=int, default=64, help="DMAs to list")


boot_emu.arguments = _args
