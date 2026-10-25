"""``survey``: run every analysis and write one report and one ROM interval ledger."""
from __future__ import annotations

import json
from pathlib import Path

from .. import survey as surveymod
from ..analysis import functions
from ..cli import command, emit
from ..ledger import Ledger
from ..rom import Rom


@command("survey", "Run every analysis over the program image; report.md, report.json and a ROM ledger")
def survey(args):
    rom = Rom.load(args.rom)
    res = surveymod.run(rom, mode=args.image, lzari_brute=not args.no_lzari_brute)
    rep = res["report"]
    out = Path(args.out)
    out.mkdir(parents=True, exist_ok=True)
    Ledger.write_tsv(res["ledger"], out / "ledger.tsv")
    (out / "report.json").write_text(json.dumps(rep, indent=1, default=str) + "\n")
    (out / "report.md").write_text(surveymod.render_markdown(rep))
    cat = res["catalogue"]
    if cat is not None:
        (out / "functions.toml").write_text(functions.to_toml(cat, rep["rom"].get("title", "")))
        (out / "symbol_addrs.txt").write_text(functions.to_symbol_addrs(cat))
    # The main segment as loaded in RAM: for a packed program this is the loader's output,
    # the input a decompilation splits (the ROM holds only the packed form).
    main = res["program"].main()
    if main.data:
        (out / "main_segment.bin").write_bytes(main.data)
    summary = {"out": str(out), "program": rep["program"]["method"], "main": rep["program"]["main"],
               "functions": rep["functions"] and rep["functions"].get("functions"),
               "sdk_named": rep["sdk"] and rep["sdk"].get("functions_named"),
               "unknown_bytes": rep["unknown_bytes"], "complete": rep["complete"],
               "next_steps": rep["next_steps"][:5],
               "cross_checks": rep["cross_checks"],
               "errors": {k: v for k, v in rep["errors"].items() if not k.endswith("_trace")}}
    if args.require_complete and not rep["complete"]:
        emit(dict(summary, gate="failed: unknown bytes remain"), args.format)
        return 1
    return summary


def _args(p):
    p.add_argument("rom", help="ROM image (z64, v64 or n64 byte order)")
    p.add_argument("--out", required=True, help="output directory")
    p.add_argument("--image", choices=("auto", "static", "emulated"), default="auto",
                   help="program image: emulate the boot only when a loader runs (auto), never, or always")
    p.add_argument("--no-lzari-brute", action="store_true", help="probe LZARI only at loader-evidenced starts")
    p.add_argument("--require-complete", action="store_true",
                   help="exit with status 1 when any ROM byte is unknown (zero-unknown gate)")


survey.arguments = _args
