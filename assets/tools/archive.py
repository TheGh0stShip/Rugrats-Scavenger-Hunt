#!/usr/bin/env python3
"""Resource archive parser, extractor and lossless rebuilder.

Archive layout (big-endian):

    +0x00 u32 record count
    +0x04 u32 reserved (zero in both retail archives)
    +0x08 count x { u32 offset; u32 logical_size; u32 stored_size }
          offset is relative to the archive start; stored_size == 0 means the payload is stored
          directly (logical_size bytes), otherwise it is an rlz stream of stored_size bytes that
          expands to logical_size bytes.
    payloads at their offsets, 16-byte aligned; the bytes between the index, payloads and the
    archive end are alignment fill and are preserved verbatim in the metadata.
"""
import argparse
import hashlib
import json
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rlz  # noqa: E402

ARCHIVES = {
    "resource_archive_1": (0x21AC00, 0x4DB6D0),
    "resource_archive_2": (0x4DB6D0, 0xF4D840),
}


def sha(b: bytes) -> str:
    return hashlib.sha256(b).hexdigest()


def parse(rom: bytes, start: int, end: int) -> dict:
    count, reserved = struct.unpack_from(">II", rom, start)
    index_end = 8 + count * 12
    records = []
    for i in range(count):
        off, logical, stored = struct.unpack_from(">III", rom, start + 8 + i * 12)
        size = stored or logical
        rec = {"index": i, "offset": off, "logical_size": logical, "stored_size": stored,
               "rom_start": start + off, "rom_end": start + off + size,
               "codec": "rlz" if stored else "direct"}
        if stored:
            out, stop = rlz.decode(rom, start + off)
            if len(out) != logical:
                raise ValueError(f"record {i}: rlz output {len(out)} != logical {logical}")
            rec["stream_bytes"] = stop - (start + off)
            rec["stream_tail_hex"] = rom[stop:start + off + stored].hex()
        records.append(rec)
    # coverage: index, payloads, fill
    spans = sorted((r["offset"], r["offset"] + (r["stored_size"] or r["logical_size"]), r["index"])
                   for r in records if (r["stored_size"] or r["logical_size"]))
    fills = []
    cursor = index_end
    shared = {}
    for a, b, i in spans:
        if a < cursor:
            # identical offsets are allowed only as exact aliases of an earlier record
            prev = [r for r in records if r["offset"] == a and r["index"] != i]
            if not prev:
                raise ValueError(f"record {i} overlaps preceding data at {a:#x}")
            shared[i] = prev[0]["index"]
            continue
        if a > cursor:
            fills.append({"offset": cursor, "hex": rom[start + cursor:start + a].hex()})
        cursor = b
    if start + cursor < end:
        fills.append({"offset": cursor, "hex": rom[start + cursor:end].hex()})
    return {"start": start, "end": end, "count": count, "reserved": reserved,
            "index_end": index_end, "records": records, "fills": fills, "aliases": shared}


def extract(rom: bytes, name: str, outdir: str) -> dict:
    start, end = ARCHIVES[name]
    meta = parse(rom, start, end)
    rdir = os.path.join(outdir, name)
    os.makedirs(rdir, exist_ok=True)
    for r in meta["records"]:
        stored = rom[r["rom_start"]:r["rom_end"]]
        r["stored_sha256"] = sha(stored)
        if r["codec"] == "rlz":
            payload, _ = rlz.decode(rom, r["rom_start"])
        else:
            payload = stored
        r["payload_sha256"] = sha(payload)
        r["file"] = f"{r['index']:03d}.bin"
        with open(os.path.join(rdir, r["file"]), "wb") as fh:
            fh.write(payload)
    meta["archive_sha256"] = sha(rom[start:end])
    with open(os.path.join(rdir, "archive.json"), "w") as fh:
        json.dump(meta, fh, indent=1)
    return meta


def build(meta: dict, payload_dir: str) -> tuple[bytes, list[dict]]:
    size = meta["end"] - meta["start"]
    buf = bytearray(size)
    struct.pack_into(">II", buf, 0, meta["count"], meta["reserved"])
    report = []
    for r in meta["records"]:
        struct.pack_into(">III", buf, 8 + r["index"] * 12, r["offset"], r["logical_size"], r["stored_size"])
        with open(os.path.join(payload_dir, r["file"]), "rb") as fh:
            payload = fh.read()
        if r["codec"] == "rlz":
            stored = rlz.encode(payload)
            stored += bytes.fromhex(r.get("stream_tail_hex", ""))
            if len(stored) != r["stored_size"]:
                raise ValueError(f"record {r['index']}: re-encoded size {len(stored)} != {r['stored_size']}")
        else:
            stored = payload
        if str(r["index"]) in meta["aliases"] or r["index"] in meta["aliases"]:
            pass  # bytes are written by the aliased record; equality is checked below
        buf[r["offset"]:r["offset"] + len(stored)] = stored
        report.append({"index": r["index"], "codec": r["codec"], "rom_start": r["rom_start"],
                       "rom_end": r["rom_end"], "rebuilt_sha256": sha(stored),
                       "original_sha256": r["stored_sha256"], "match": sha(stored) == r["stored_sha256"]})
    for f in meta["fills"]:
        b = bytes.fromhex(f["hex"])
        buf[f["offset"]:f["offset"] + len(b)] = b
    return bytes(buf), report


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("rom")
    ap.add_argument("outdir", help="private extraction directory")
    a = ap.parse_args()
    rom = open(a.rom, "rb").read()
    ok = True
    for name, (start, end) in ARCHIVES.items():
        meta = extract(rom, name, a.outdir)
        rebuilt, rep = build(meta, os.path.join(a.outdir, name))
        same = rebuilt == rom[start:end]
        bad = [r for r in rep if not r["match"]]
        first = next((i for i in range(len(rebuilt)) if rebuilt[i] != rom[start + i]), None)
        print(f"{name}: records {meta['count']} rlz {sum(r['codec'] == 'rlz' for r in meta['records'])} "
              f"record mismatches {len(bad)} archive {'MATCH' if same else 'DIFF'}"
              + ("" if same else f" first diff {start + first:#x}"))
        ok &= same and not bad
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
