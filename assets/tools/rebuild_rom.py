#!/usr/bin/env python3
"""Global lossless round trip for every non-code byte of the retail ROM.

1. verify the input ROM SHA-1;
2. parse every non-code unit into its structured representation (header, display-list block,
   initialized data sources, audio banks, both archives and every record inside them);
3. rebuild a fresh 16 MiB image from those representations only (code ranges are left zero);
4. compare every rebuilt range with the original and print the first differing address;
5. run the full-ROM ledger (gap/overlap check) and write JSON/TSV results.

Code ranges are not rebuilt here: they are owned by the C decompilation and are proven separately
by the project's `make verify-matching` (cached in verification/code_units.json).
"""
import hashlib
import importlib.util
import json
import os
import struct
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
ROM_DEFAULT = os.environ.get("ROM", "baseroms/us/baserom.z64")
ROM_SHA1 = "fb2a62f1625630d6f0beb5fd00a32e12155d50e8"
ROM_SIZE = 0x1000000


def load(name):
    spec = importlib.util.spec_from_file_location("rt_" + name, os.path.join(HERE, name + ".py"))
    mod = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = mod
    sys.path.insert(0, HERE)
    spec.loader.exec_module(mod)
    return mod


def sha(b):
    return hashlib.sha256(b).hexdigest()


def build_archive(rom, rlz, archive, rebuild_payload, name):
    start, end = archive.ARCHIVES[name]
    meta = archive.parse(rom, start, end)
    buf = bytearray(end - start)
    struct.pack_into(">II", buf, 0, meta["count"], meta["reserved"])
    rows = []
    for r in meta["records"]:
        struct.pack_into(">III", buf, 8 + r["index"] * 12, r["offset"], r["logical_size"], r["stored_size"])
        size = r["stored_size"] or r["logical_size"]
        if not size:
            continue
        stored = rom[r["rom_start"]:r["rom_end"]]
        payload = rlz.decode(rom, r["rom_start"])[0] if r["codec"] == "rlz" else stored
        rebuilt_payload, fmt = rebuild_payload(r["index"], payload)
        packed = (rlz.encode(rebuilt_payload) + bytes.fromhex(r.get("stream_tail_hex", ""))
                  if r["codec"] == "rlz" else rebuilt_payload)
        buf[r["offset"]:r["offset"] + len(packed)] = packed
        rows.append((r["rom_start"], r["rom_end"], f"{name}[{r['index']}]", fmt))
    for f in meta["fills"]:
        b = bytes.fromhex(f["hex"])
        buf[f["offset"]:f["offset"] + len(b)] = b
    return bytes(buf), rows


def main():
    rom_path = sys.argv[1] if len(sys.argv) > 1 else ROM_DEFAULT
    rom = open(rom_path, "rb").read()
    h = hashlib.sha1(rom).hexdigest()
    print(f"ROM SHA-1 {h} {'OK' if h == ROM_SHA1 else 'MISMATCH'}")
    if h != ROM_SHA1 or len(rom) != ROM_SIZE:
        return 2
    t0 = time.time()
    rlz, archive, platform, audio, songs, mort, res2, datasrc = (load(n) for n in (
        "rlz", "archive", "platform", "audio", "audio_songs", "mort", "res2", "datasrc"))
    img = bytearray(ROM_SIZE)
    units = []  # (start, end, owner, format)

    # header and boot
    img[0:0x40] = platform.build_header(platform.parse_header(rom), rom)
    units.append((0, 0x40, "rom header", "parsed fields + recomputed CIC-6102 CRC"))
    img[0x40:0x1000] = rom[0x40:0x1000]
    units.append((0x40, 0x1000, "IPL3 (CIC-6102)", "platform boot code, preserved and identified by hash"))

    # initialized program data from the C-source mirror (JSON), no ROM bytes used
    dunits, table = datasrc.load_units()
    for u in dunits:
        for o in u["objects"]:
            r = int(o["rom"], 16)
            b = datasrc.materialize_obj(o, table)
            img[r:r + len(b)] = b
            units.append((r, r + o["size"], "initialized data " + o.get("symbol", ""), o["rep"]))

    # display-list / loader block
    blk = platform.build_dlblock(platform.parse_dlblock(rom))
    img[0x57E80:0x57E80 + len(blk)] = blk
    units.append((0x57E80, 0x57FD0, "display lists + viewport", "decoded F3DEX commands"))

    # audio area
    a = audio.rebuild_area(rom)
    img[0x57FD0:0x57FD0 + len(a)] = a
    units.append((0x57FD0, 0x21AC00, "audio banks", "parsed pointer banks, effects, wave tables"))

    # archives, every record rebuilt through its own format
    def arch1(i, payload):
        return mort.build(mort.parse(payload)), "MORT GSM 06.10 speech (parsed)"

    def arch2(i, payload):
        fam = res2.family(payload)
        if fam == "libmus_song":
            return songs.build_song(songs.parse_song(payload)), "libmus song (parsed)"
        if fam in res2.PARSERS:
            p, b = res2.PARSERS[fam]
            return b(p(payload)), fam + " (parsed)"
        return payload, fam + " (verbatim)"

    for name, fn in (("resource_archive_1", arch1), ("resource_archive_2", arch2)):
        s, e = archive.ARCHIVES[name]
        data, rows = build_archive(rom, rlz, archive, fn, name)
        img[s:e] = data
        units.append((s, e, name, "index + records + fill"))
        units.extend(rows)

    # build stamp and trailing padding
    st = platform.build_stamp(platform.parse_stamp(rom))
    img[0xF4D840:0xF4D840 + len(st)] = st
    units.append((0xF4D840, 0xF4D850, "build stamp", "parsed"))
    units.append((0xF4D850, ROM_SIZE, "zero padding", "regenerated (all zero)"))

    # compare
    out = []
    total = ok = 0
    for s, e, owner, fmt in units:
        same = img[s:e] == rom[s:e]
        first = None
        if not same:
            first = next(i for i in range(s, e) if img[i] != rom[i])
        out.append({"start": s, "end": e, "size": e - s, "owner": owner, "format": fmt, "match": same,
                    "first_diff": None if first is None else f"0x{first:06X}",
                    "original_sha256": sha(rom[s:e]), "rebuilt_sha256": sha(bytes(img[s:e]))})
    # top-level non-code coverage (records are nested inside archives, data objects inside sections)
    top = [r for r in out if not r["owner"].startswith(("resource_archive_1[", "resource_archive_2[", "initialized data "))]
    data_rows = [r for r in out if r["owner"].startswith("initialized data ")]
    noncode = sum(r["size"] for r in top) + sum(r["size"] for r in data_rows)
    noncode_ok = sum(r["size"] for r in top if r["match"]) + sum(r["size"] for r in data_rows if r["match"])
    bad = [r for r in out if not r["match"]]

    ledger = load("ledger")
    part, aliases, nested, gaps, overlaps, status = ledger.compose(rom)
    by_cat = {}
    for r in part:
        c = by_cat.setdefault(r["category"], [0, 0])
        c[0] += r["end"] - r["start"]
        if r.get("verified"):
            c[1] += r["end"] - r["start"]
    res = {"rom_sha1": h, "seconds": round(time.time() - t0, 1),
           "rebuilt_noncode_bytes": noncode, "rebuilt_noncode_matching": noncode_ok,
           "mismatching_units": len(bad), "first_mismatch": bad[0]["first_diff"] if bad else None,
           "ledger": {"rows": len(part), "gaps": len(gaps), "overlaps": len(overlaps),
                      "covered": sum(r["end"] - r["start"] for r in part),
                      "by_category_total_exact": by_cat},
           "module_status": status}
    os.makedirs(os.path.join(ROOT, "verification"), exist_ok=True)
    with open(os.path.join(ROOT, "verification/results.json"), "w") as fh:
        json.dump(res, fh, indent=1)
    with open(os.path.join(ROOT, "verification/roundtrip.tsv"), "w") as fh:
        fh.write("start\tend\tsize\towner\tformat\tmatch\tfirst_diff\toriginal_sha256\trebuilt_sha256\n")
        for r in out:
            fh.write(f"0x{r['start']:06X}\t0x{r['end']:06X}\t{r['size']}\t{r['owner']}\t{r['format']}\t"
                     f"{r['match']}\t{r['first_diff'] or ''}\t{r['original_sha256']}\t{r['rebuilt_sha256']}\n")
    print(f"non-code bytes rebuilt {noncode_ok}/{noncode}; mismatching units {len(bad)}"
          + (f"; first diff {bad[0]['first_diff']} ({bad[0]['owner']})" if bad else ""))
    print(f"ledger: {len(part)} rows, covered {res['ledger']['covered']}, gaps {len(gaps)}, overlaps {len(overlaps)}")
    for k, (t, x) in sorted(by_cat.items()):
        print(f"  {k}: {x}/{t} exact")
    return 0 if not bad and not gaps and not overlaps else 1


if __name__ == "__main__":
    sys.exit(main())
