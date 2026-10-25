"""``n64dk datamap``: xrefs, section split, data ownership, initialized-data ledger, typed units."""
from __future__ import annotations

import csv
from pathlib import Path

from ..analysis import datamap as dmap
from ..cli import command
from ..ledger import Ledger
from ..rom import Rom


@command("datamap", "Cross-reference, split and attribute the initialized data image; ledger and typed data units")
def datamap(args):
    rom = Rom.load(args.rom)
    bss = int(args.bss_start, 16) if args.bss_start else None
    dm = dmap.analyse(rom, bss)
    matched: list[dmap.MatchedRow] = []
    for path in args.matched or ():
        matched += dmap.read_matched(path)
    ledger, rejected = dmap.build_ledger(dm, rom.data, matched)
    out = dmap.summary(dm, ledger)
    out["matched_rows"] = len(matched)
    out["rejected_rows"] = [f"0x{r.rom:X}+0x{r.size:X} {r.source}" for r in rejected[:20]]
    data_rows = [r for r in matched if r.section != ".text"]
    if data_rows and any(r.section == ".text" for r in matched):
        b = dm.boot
        res = dmap.ownership_agreement(
            dm, [(b.rom_to_vram(r.rom), b.rom_to_vram(r.rom + r.size), r.source) for r in data_rows],
            dmap.code_ranges(dm, matched))
        out["ownership"] = {k: v for k, v in res.items() if k != "mismatches"}
        out["ownership_mismatches"] = res["mismatches"][:args.limit]
        sec_ok = sum(1 for r in data_rows if dm.section_of(b.rom_to_vram(r.rom)) == r.section)
        out["section_agreement"] = f"{sec_ok}/{len(data_rows)}"
    if args.ledger:
        Ledger.write_tsv(ledger, args.ledger)
        out["ledger_tsv"] = args.ledger
    if args.objects:
        b = dm.boot
        with open(args.objects, "w", newline="") as f:
            w = csv.writer(f, delimiter="\t", lineterminator="\n")
            w.writerow(["vram", "rom", "size", "section", "type", "owners", "access", "written"])
            for o in dm.objects:
                w.writerow([f"0x{o.start:08X}", f"0x{b.vram_to_rom(o.start):X}", f"0x{o.end - o.start:X}", o.section, o.type,
                            ",".join(f"0x{x:08X}" for x in o.owners) or "-", ",".join(o.accesses) or "-", int(o.written)])
        out["objects_tsv"] = args.objects
    if args.xrefs:
        with open(args.xrefs, "w", newline="") as f:
            w = csv.writer(f, delimiter="\t", lineterminator="\n")
            w.writerow(["address", "site", "func", "opcode", "access", "kind", "via"])
            for adr in sorted(dm.xrefs):
                for x in dm.xrefs[adr]:
                    w.writerow([f"0x{adr:08X}", f"0x{x.site:08X}", f"0x{x.func:08X}", x.opcode, x.access, x.kind, x.via])
        out["xrefs_tsv"] = args.xrefs
    if args.emit_units:
        symbols = dmap.read_symbols(args.symbols) if args.symbols else None
        rows = dmap.emit_units(dm, rom.data, ledger, args.emit_units, symbols)
        out["units"] = len(rows)
        out["unit_bytes"] = hex(sum(r["size"] for r in rows))
        out["units_dir"] = str(Path(args.emit_units))
    return out


def _args(p):
    p.add_argument("rom", help="ROM image")
    p.add_argument("--matched", action="append", metavar="TSV",
                   help="matched rows: ROM_START SIZE SOURCE <SECTION|FLAGS> ... (repeatable)")
    p.add_argument("--symbols", metavar="FILE", help="symbol map; pointer words become relocations")
    p.add_argument("--emit-units", metavar="DIR", help="write typed C units for unmatched data runs")
    p.add_argument("--ledger", metavar="TSV", help="write the initialized-image ledger")
    p.add_argument("--objects", metavar="TSV", help="write data objects with owners and types")
    p.add_argument("--xrefs", metavar="TSV", help="write every data reference site")
    p.add_argument("--bss-start", metavar="VRAM", help="override the image end (hex)")
    p.add_argument("--limit", type=int, default=20, help="mismatches to list")


datamap.arguments = _args
