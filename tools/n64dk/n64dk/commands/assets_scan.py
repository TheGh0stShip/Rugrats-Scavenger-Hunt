"""``assets-scan``: compressed streams, loader evidence and a ROM interval ledger in one call."""
from __future__ import annotations

import json
from pathlib import Path

from .. import boot
from ..assets import compress, loaders
from ..cli import command
from ..ledger import Interval, Ledger
from ..rom import Rom

IPL3_END = 0x1000


def _overlaps(taken: list[tuple[int, int]], a: int, b: int) -> bool:
    return any(x < b and a < y for x, y in taken)


def scan_rom(rom: Rom, *, lzari_brute: bool = True, deflate_align: int = 0,
             verify_reencode: bool = False, program=None) -> dict:
    """Run loader tracing and stream scanning; return streams, evidence and ledger items.

    ``program`` (an :class:`n64dk.image.ProgramImage`) supplies the code to trace when the
    program is unpacked by a loader: its references to ROM ranges are in that code, not in
    the boot copy."""
    data = rom.data
    b = boot.analyse(rom)
    main_size = min(b.copy_size, len(data) - b.main_rom)
    segment = None
    if program is not None and program.main().kind == "packed":
        main = program.main()
        segment, main_vram, main_size = main.data, main.vram, len(main.data)
    else:
        main_vram = b.main_vram
    first = loaders.trace(data, b.main_rom, main_vram, main_size, segment=segment)
    seeds = {r.start for r in first.ranges}
    streams = compress.scan(data, lzari_seeds=seeds, lzari_brute=lzari_brute, deflate_align=deflate_align,
                            verify_reencode=verify_reencode)
    starts = {s.rom for s in streams}
    ev = loaders.trace(data, b.main_rom, main_vram, main_size, starts,
                       exclude=[(s.rom, s.end) for s in streams], segment=segment)

    by_start: dict[int, list[loaders.RomRange]] = {}
    for r in ev.ranges:
        by_start.setdefault(r.start, []).append(r)
    stream_rows = []
    for s in streams:
        row = s.as_dict()
        match = [r for r in by_start.get(s.rom, []) if 0 <= r.end - s.end < 16]
        row["loader"] = [r.as_dict() for r in match]
        stream_rows.append(row)

    led = Ledger(0, len(data))
    taken: list[tuple[int, int]] = []

    def add(a, b_, cls, **kw):
        if a < b_ and not _overlaps(taken, a, b_):
            led.add(a, b_, cls, **kw)
            taken.append((a, b_))
            return True
        return False

    add(0, 0x40, "rom_metadata", kind="header", name="rom_header", evidence="rom header")
    add(0x40, IPL3_END, "rom_metadata", kind="ipl3", name="ipl3", evidence=f"cic {rom.cic}")
    for s, row in zip(streams, stream_rows):
        loader = row["loader"][0] if row["loader"] else None
        evidence = f"decoded 0x{s.size:X} bytes"
        if loader:
            evidence += f"; {loader['source']} {loader['site']}"
            if loader["loader"]:
                evidence += f" in {loader['loader']}"
        add(s.rom, s.end, "payload", category="compressed", kind=s.codec,
            name=f"{s.codec}_{s.rom:X}", evidence=evidence)
    rank = {"call": 0, "wrapper": 1, "size": 2, "store": 3, "pairs": 4, "records": 5, "table": 6}
    for r in sorted(ev.ranges, key=lambda r: (rank.get(r.source, 9), r.size)):
        where = f"{r.source} 0x{r.site:08X}" + (f" in 0x{r.loader:08X}" if r.loader else "")
        if r.end <= len(data):
            add(r.start, r.end, "payload", category="loader_range", kind=r.form,
                name=f"rom_{r.start:X}", evidence=where)
    items = led.complete(data)
    return {
        "rom": rom.summary(),
        "main_segment": {"rom": None if segment is not None else b.main_rom, "vram": main_vram, "size": main_size,
                         "source": "program image (unpacked)" if segment is not None else "IPL3 copy"},
        "streams": stream_rows,
        "stream_objects": streams,
        "loader": ev.as_dict(),
        "ledger": items,
    }


def _summary(res: dict) -> dict:
    from collections import Counter
    codecs = Counter(s["codec"] for s in res["streams"])
    sources = Counter(r["source"] for r in res["loader"]["ranges"])
    items: list[Interval] = res["ledger"]
    covered = Ledger.totals(items, "category")
    return {
        "streams": dict(codecs),
        "streams_with_loader": sum(1 for s in res["streams"] if s["loader"]),
        "loader_ranges": dict(sources),
        "dma_wrappers": list(res["loader"]["wrappers"]),
        "ledger_bytes": {k: v for k, v in covered.items()},
        "unknown_bytes": Ledger.totals(items).get("unknown", 0),
    }


@command("assets-scan", "Find compressed streams and loader-referenced ROM ranges; write an interval ledger")
def assets_scan(args):
    rom = Rom.load(args.rom)
    res = scan_rom(rom, lzari_brute=not args.no_lzari_brute, deflate_align=args.deflate_align,
                   verify_reencode=args.verify)
    if args.out:
        out = Path(args.out)
        out.mkdir(parents=True, exist_ok=True)
        Ledger.write_tsv(res["ledger"], out / "ledger.tsv")
        doc = {k: v for k, v in res.items() if k not in ("stream_objects", "ledger")}
        (out / "assets_scan.json").write_text(json.dumps(doc, indent=1, default=str) + "\n")
        if args.extract:
            sdir = out / "streams"
            sdir.mkdir(exist_ok=True)
            for s in res["stream_objects"]:
                if s.data is not None:
                    (sdir / f"{s.rom:08X}.{s.codec}.bin").write_bytes(s.data)
    if args.format == "json":
        doc = {k: v for k, v in res.items() if k not in ("stream_objects",)}
        doc["ledger"] = [
            {"start": f"0x{i.start:X}", "end": f"0x{i.end:X}", "class": i.cls, "category": i.category,
             "kind": i.kind, "name": i.name, "evidence": i.evidence} for i in res["ledger"]]
        doc["summary"] = _summary(res)
        return doc
    return _summary(res)


def _arguments(p):
    p.add_argument("rom", help="ROM image (z64, v64 or n64 byte order)")
    p.add_argument("--out", help="directory for ledger.tsv, assets_scan.json and extracted streams")
    p.add_argument("--extract", action="store_true", help="write each decoded stream to OUT/streams/")
    p.add_argument("--verify", action="store_true", help="re-encode every stream and report identical/roundtrip")
    p.add_argument("--deflate-align", type=lambda v: int(v, 0), default=0,
                   help="try raw deflate at every N-byte boundary (0 = off)")
    p.add_argument("--no-lzari-brute", action="store_true",
                   help="only probe LZARI at loader-evidenced starts and stream chains")


assets_scan.arguments = _arguments
