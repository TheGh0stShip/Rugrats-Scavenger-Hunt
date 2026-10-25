"""``functions``: function catalogue, jump tables and boundary findings for the main segment."""
from __future__ import annotations

import json
from collections import Counter
from pathlib import Path

from ..analysis import functions as fn
from ..cli import command
from ..rom import Rom


def _args(p):
    p.add_argument("rom", help="ROM image (z64, v64 or n64 byte order)")
    p.add_argument("--out", help="directory for functions.toml, symbol_addrs.txt, jump_tables.tsv, "
                                 "findings.tsv and summary.json")
    p.add_argument("--truth", action="append", default=[],
                   help="reference catalogue (recomp TOML or symbol_addrs.txt) to score against; repeatable")
    p.add_argument("--audit", action="append", default=[],
                   help="catalogue whose entries are checked for bad boundaries; repeatable")
    p.add_argument("--list", type=int, default=20, help="missed/extra addresses to show per reference")


@command("functions", "Discover functions, jump tables and bad boundaries in the main code segment")
def functions(args):
    rom = Rom.load(args.rom)
    cat = fn.discover(rom)
    out = fn.summary(cat)
    for path in args.truth:
        score = fn.compare(cat, fn.load_catalogue(path))
        score["missed"] = [f"0x{v:08X}" for v in score["missed"][:args.list]]
        score["extra"] = [f"0x{v:08X}" for v in score["extra"][:args.list]]
        out.setdefault("scores", {})[path] = score
    audits = {}
    for path in args.audit:
        entries = [(v, s) for _, v, s in fn.load_catalogue(path)]
        found = fn.audit(cat.image, entries, cat)
        audits[path] = found
        out.setdefault("audits", {})[path] = {
            "entries": len(entries), "findings": dict(Counter(f.kind for f in found)),
            "flagged_entries": len({f.function for f in found})}
    if args.out:
        d = Path(args.out)
        d.mkdir(parents=True, exist_ok=True)
        (d / "functions.toml").write_text(fn.to_toml(cat, rom.header.title))
        (d / "symbol_addrs.txt").write_text(fn.to_symbol_addrs(cat))
        (d / "jump_tables.tsv").write_text(fn.to_jump_tables_tsv(cat))
        (d / "findings.tsv").write_text(fn.to_findings_tsv(cat.findings))
        for i, (path, found) in enumerate(audits.items()):
            (d / f"audit_{i}_{Path(path).stem}.tsv").write_text(fn.to_findings_tsv(found))
        (d / "summary.json").write_text(json.dumps(out, indent=1) + "\n")
        out["written"] = str(d)
    return out


functions.arguments = _args
