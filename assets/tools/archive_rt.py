#!/usr/bin/env python3
"""roundtrip() adapter for tools/archive.py: one ledger row per archive index, record and fill.

Each record is decoded (rlz) and re-encoded in memory and compared with its stored bytes; the index
is rebuilt from the parsed counts/offsets/sizes; fills are byte-preserved alignment gaps.
Aliased records (sharing the bytes of an earlier record) are reported with subtype 'alias' and
category 'alias' so the ledger can exclude them from coverage.
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archive  # noqa: E402
import rlz  # noqa: E402

OWNER = "tools/archive.py"


def _row(start, end, cat, sub, fmt, ext, reb, ok, ev):
    return {"start": start, "end": end, "category": cat, "subtype": sub, "owner": OWNER, "format": fmt,
            "extraction": ext, "rebuild": reb, "verified": bool(ok), "evidence": ev}


def roundtrip(rom: bytes) -> list[dict]:
    rows = []
    for name, (start, end) in archive.ARCHIVES.items():
        meta = archive.parse(rom, start, end)
        idx = bytearray(meta["index_end"])
        struct.pack_into(">II", idx, 0, meta["count"], meta["reserved"])
        for r in meta["records"]:
            struct.pack_into(">III", idx, 8 + r["index"] * 12, r["offset"], r["logical_size"], r["stored_size"])
        rows.append(_row(start, start + meta["index_end"], "archive_index", f"{name}.index",
                         "u32 count,u32 reserved,{u32 off,u32 logical,u32 stored}[]", "archive.parse",
                         "pack from parsed fields", bytes(idx) == rom[start:start + meta["index_end"]],
                         f"{meta['count']} records"))
        for r in meta["records"]:
            a, b = r["rom_start"], r["rom_end"]
            if a == b:
                continue
            stored = rom[a:b]
            if r["codec"] == "rlz":
                payload, _ = rlz.decode(rom, a)
                again = rlz.encode(payload) + bytes.fromhex(r.get("stream_tail_hex", ""))
                ext, reb = "rlz.decode", "rlz.encode + preserved stream tail"
            else:
                payload = again = stored
                ext, reb = "byte copy", "byte copy"
            alias = r["index"] in meta["aliases"] or str(r["index"]) in meta["aliases"]
            rows.append(_row(a, b, "alias" if alias else "archive_record",
                             "alias" if alias else f"{name}.{r['index']:03d}",
                             f"{r['codec']} logical={r['logical_size']:#x}", ext, reb, again == stored,
                             f"{name} record {r['index']} sha256={archive.sha(stored)[:16]}"
                             + (f" alias of {meta['aliases'].get(r['index'])}" if alias else "")))
        for f in meta["fills"]:
            fb = bytes.fromhex(f["hex"])
            a = start + f["offset"]
            rows.append(_row(a, a + len(fb), "fill", f"{name}.fill", "alignment fill (preserved)",
                             "byte copy", "byte copy", fb == rom[a:a + len(fb)],
                             f"distinct={sorted(set(fb))[:4]}"))
    return rows


def main() -> int:
    rom = open(sys.argv[1] if len(sys.argv) > 1 else os.environ.get("ROM", "baseroms/us/baserom.z64"), "rb").read()
    rows = roundtrip(rom)
    bad = [r for r in rows if not r["verified"]]
    cov = sum(r["end"] - r["start"] for r in rows if r["category"] != "alias")
    print(f"rows {len(rows)} failed {len(bad)} covered {cov:#x}")
    return 0 if not bad else 1


if __name__ == "__main__":
    sys.exit(main())
