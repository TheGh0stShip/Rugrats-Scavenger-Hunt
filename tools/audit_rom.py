#!/usr/bin/env python3
"""Audit the retail ROM's physical layout without extracting its resources."""

from __future__ import annotations

import argparse
import hashlib
import json
import struct
from collections import Counter
from pathlib import Path


EXPECTED_SHA1 = "fb2a62f1625630d6f0beb5fd00a32e12155d50e8"
ROM_SIZE = 0x1000000
PROGRAM_START = 0x1000
PROGRAM_END = 0x57E80
ARCHIVES = (
    ("resource_archive_1", 0x21AC00, 0x4DB6D0),
    ("resource_archive_2", 0x4DB6D0, 0xF4D840),
)
PROGRAM_SECTIONS = (
    ("text", 0x1000, 0x103C),
    ("rodata", 0x103C, 0x45E0),
    ("data", 0x45E0, 0x45F0),
    ("text", 0x45F0, 0x53474),
    ("data", 0x53474, 0x57E80),
)
AUDIO_GRAPHICS_RANGES = (
    ("display_lists_and_loader_block", 0x57E80, 0x57FD0),
    ("audio_pointer_bank_1", 0x57FD0, 0x5B470),
    ("audio_sample_bank_1", 0x5B470, 0x111310),
    ("audio_sample_bank_2", 0x111310, 0x215C80),
    ("audio_pointer_bank_2", 0x215C80, 0x21AC00),
)
AUDIO_BANKS = (
    ("audio_bank_1", 0x57FD0, 0x5B470, 0x111310),
    ("audio_bank_2", 0x215C80, 0x111310, 0x215C80),
)
PTR_MAGIC = b"N64 PtrTablesV2\0"
WAVE_MAGIC = b"N64 WaveTables \0"


def be32(data: bytes, offset: int) -> int:
    return struct.unpack_from(">I", data, offset)[0]


def find_all(data: bytes, signature: bytes) -> list[int]:
    offsets = []
    cursor = 0
    while True:
        cursor = data.find(signature, cursor)
        if cursor < 0:
            return offsets
        offsets.append(cursor)
        cursor += 1


def parse_audio_bank(data: bytes, name: str, pointer_start: int, wave_start: int,
                     wave_end: int) -> dict:
    assert data[pointer_start:pointer_start + 16] == PTR_MAGIC
    assert data[wave_start:wave_start + 16] == WAVE_MAGIC
    count = be32(data, pointer_start + 0x20)
    wave_list = be32(data, pointer_start + 0x2C)
    samples = []
    for index in range(count):
        wave = pointer_start + be32(data, pointer_start + wave_list + index * 4)
        base, size = be32(data, wave), be32(data, wave + 4)
        codec_id, loop = data[wave + 8], be32(data, wave + 12)
        assert codec_id in (0, 1)
        start, end = wave_start + base, wave_start + base + size
        assert wave_start + 16 <= start <= end <= wave_end
        samples.append({
            "index": index, "rom_start": start, "rom_end": end,
            "stored_size": size, "codec": "adpcm" if codec_id == 0 else "pcm16",
            "looped": bool(loop and struct.unpack_from(">i", data, pointer_start + loop + 8)[0]),
        })
    samples.sort(key=lambda item: item["rom_start"])
    assert all(left["rom_end"] <= right["rom_start"] for left, right in zip(samples, samples[1:]))
    return {
        "name": name, "pointer_start": pointer_start, "wave_start": wave_start,
        "wave_end": wave_end, "sample_count": count,
        "sample_bytes": sum(item["stored_size"] for item in samples),
        "adpcm_count": sum(item["codec"] == "adpcm" for item in samples),
        "pcm16_count": sum(item["codec"] == "pcm16" for item in samples),
        "looped_count": sum(item["looped"] for item in samples), "samples": samples,
    }


def parse_archive(data: bytes, name: str, start: int, end: int) -> dict:
    count = be32(data, start)
    table_end = start + 8 + count * 12
    records = []
    for index in range(count):
        offset, logical_size, transform_size = struct.unpack_from(
            ">III", data, start + 8 + index * 12
        )
        stored_size = transform_size or logical_size
        prefix = data[start + offset : start + offset + min(4, stored_size)].hex()
        records.append(
            {
                "index": index,
                "rom_start": start + offset,
                "rom_end": start + offset + stored_size,
                "logical_size": logical_size,
                "stored_size": stored_size,
                "transform_size": transform_size,
                "prefix_hex": prefix,
                "storage": "transformed" if transform_size else "direct",
            }
        )

    nonempty = [record for record in records if record["stored_size"]]
    assert table_end <= start + records[0]["rom_start"] - start
    assert all(start <= record["rom_start"] <= record["rom_end"] <= end for record in records)
    assert all(record["rom_start"] % 16 == 0 for record in records)
    assert all(
        left["rom_end"] <= right["rom_start"]
        for left, right in zip(nonempty, nonempty[1:])
    )

    occupied = [(start, table_end)] + [
        (record["rom_start"], record["rom_end"]) for record in nonempty
    ]
    padding = sum(next_start - current_end for (_, current_end), (next_start, _) in zip(occupied, occupied[1:]))
    padding += end - occupied[-1][1]
    prefixes = Counter(
        data[record["rom_start"] : record["rom_start"] + min(4, record["stored_size"])].hex()
        for record in nonempty
    )
    prefix_bytes = Counter()
    for record in nonempty:
        prefix_bytes[record["prefix_hex"]] += record["stored_size"]
    direct = sum(record["transform_size"] == 0 and record["stored_size"] > 0 for record in records)
    transformed = sum(record["transform_size"] > 0 for record in records)
    stored_bytes = sum(record["stored_size"] for record in records)
    logical_bytes = sum(record["logical_size"] for record in records)
    sizes = sorted(record["stored_size"] for record in nonempty)
    transformed_records = [record for record in records if record["transform_size"]]
    transformed_stored = sum(record["stored_size"] for record in transformed_records)
    transformed_logical = sum(record["logical_size"] for record in transformed_records)
    hashes: dict[str, list[int]] = {}
    for record in nonempty:
        digest = hashlib.sha256(data[record["rom_start"]:record["rom_end"]]).hexdigest()
        hashes.setdefault(digest, []).append(record["index"])
    duplicate_groups = sorted(
        (indices for indices in hashes.values() if len(indices) > 1),
        key=lambda indices: indices[0],
    )
    mort_count = prefixes.get(b"MORT".hex(), 0)
    mdlh_count = prefixes.get(b"mdlh".hex(), 0)
    return {
        "name": name,
        "start": start,
        "end": end,
        "span_bytes": end - start,
        "record_count": count,
        "nonempty_count": len(nonempty),
        "empty_count": count - len(nonempty),
        "direct_count": direct,
        "transformed_count": transformed,
        "index_bytes": table_end - start,
        "stored_resource_bytes": stored_bytes,
        "logical_resource_bytes": logical_bytes,
        "minimum_record_bytes": sizes[0],
        "median_record_bytes": (sizes[(len(sizes) - 1) // 2] + sizes[len(sizes) // 2]) / 2,
        "p90_record_bytes": sizes[(len(sizes) - 1) * 90 // 100],
        "maximum_record_bytes": sizes[-1],
        "transformed_stored_bytes": transformed_stored,
        "transformed_logical_bytes": transformed_logical,
        "transformed_ratio": (
            transformed_stored / transformed_logical if transformed_logical else None
        ),
        "duplicate_groups": duplicate_groups,
        "duplicate_group_count": len(duplicate_groups),
        "duplicate_record_count": sum(len(group) for group in duplicate_groups),
        "alignment_padding_bytes": padding,
        "mort_signature_count": mort_count,
        "mdlh_signature_count": mdlh_count,
        "prefix_counts": dict(sorted(prefixes.items())),
        "prefix_stored_bytes": dict(sorted(prefix_bytes.items())),
        "records": records,
    }


def physical_ledger(report: dict) -> list[dict]:
    """Return a gap-free physical-ROM ledger at the finest proven granularity."""
    rows = [
        {"start": 0, "end": 0x40, "kind": "rom_header", "name": "ROM header"},
        {"start": 0x40, "end": PROGRAM_START, "kind": "boot_code", "name": "IPL3 boot code"},
    ]
    rows.extend(
        {"start": item["start"], "end": item["end"], "kind": f"program_{item['kind']}",
         "name": item["kind"]}
        for item in report["program_sections"]
    )
    media = report["audio_graphics_ranges"]
    rows.append({"start": media[0]["start"], "end": media[0]["end"],
                 "kind": media[0]["kind"], "name": media[0]["kind"]})
    for bank in report["audio_banks"]:
        pointer_range = next(item for item in media if item["start"] == bank["pointer_start"])
        rows.append({"start": pointer_range["start"], "end": pointer_range["end"],
                     "kind": pointer_range["kind"], "name": bank["name"]})
        cursor = bank["wave_start"]
        for sample in bank["samples"]:
            if cursor < sample["rom_start"]:
                rows.append({"start": cursor, "end": sample["rom_start"],
                             "kind": "audio_bank_metadata_or_padding", "name": bank["name"]})
            rows.append({"start": sample["rom_start"], "end": sample["rom_end"],
                         "kind": "audio_sample", "name": bank["name"],
                         "record_index": sample["index"], "storage": sample["codec"]})
            cursor = sample["rom_end"]
        if cursor < bank["wave_end"]:
            rows.append({"start": cursor, "end": bank["wave_end"],
                         "kind": "audio_bank_metadata_or_padding", "name": bank["name"]})
    for archive in report["archives"]:
        cursor = archive["start"]
        index_end = cursor + archive["index_bytes"]
        rows.append({"start": cursor, "end": index_end, "kind": "archive_index",
                     "name": archive["name"]})
        cursor = index_end
        for record in archive["records"]:
            if not record["stored_size"]:
                continue
            if cursor < record["rom_start"]:
                rows.append({"start": cursor, "end": record["rom_start"],
                             "kind": "alignment_padding", "name": archive["name"]})
            signature = record["prefix_hex"]
            rows.append({"start": record["rom_start"], "end": record["rom_end"],
                         "kind": "archive_resource", "name": archive["name"],
                         "record_index": record["index"], "storage": record["storage"],
                         "logical_size": record["logical_size"], "prefix_hex": signature})
            cursor = record["rom_end"]
        if cursor < archive["end"]:
            rows.append({"start": cursor, "end": archive["end"],
                         "kind": "alignment_padding", "name": archive["name"]})
    rows.extend((
        {"start": 0xF4D840, "end": 0xF4D850, "kind": "build_stamp", "name": "build stamp"},
        {"start": 0xF4D850, "end": ROM_SIZE, "kind": "zero_padding", "name": "unused ROM fill"},
    ))
    rows.sort(key=lambda row: row["start"])
    assert rows[0]["start"] == 0 and rows[-1]["end"] == ROM_SIZE
    assert all(left["end"] == right["start"] for left, right in zip(rows, rows[1:]))
    for row in rows:
        row["size"] = row["end"] - row["start"]
    return rows


def write_ledger(path: Path, rows: list[dict]) -> None:
    fields = ("start", "end", "size", "kind", "name", "record_index", "storage",
              "logical_size", "prefix_hex")
    with path.open("w") as stream:
        stream.write("\t".join(fields) + "\n")
        for row in rows:
            rendered = dict(row)
            rendered["start"] = f"0x{row['start']:06X}"
            rendered["end"] = f"0x{row['end']:06X}"
            stream.write("\t".join(str(rendered.get(field, "")) for field in fields).rstrip("\t") + "\n")


def audit(path: Path) -> dict:
    data = path.read_bytes()
    digest = hashlib.sha1(data).hexdigest()
    if digest != EXPECTED_SHA1:
        raise SystemExit(f"wrong ROM SHA-1: {digest}")
    if len(data) != ROM_SIZE:
        raise SystemExit(f"wrong ROM size: {len(data)}")

    archives = [parse_archive(data, *spec) for spec in ARCHIVES]
    audio_banks = [parse_audio_bank(data, *spec) for spec in AUDIO_BANKS]
    ranges = [
        {"start": 0, "end": PROGRAM_START, "kind": "rom_header_and_boot"},
        {"start": PROGRAM_START, "end": PROGRAM_END, "kind": "initial_program_image"},
        {"start": PROGRAM_END, "end": 0x21AC00, "kind": "audio_and_display_data"},
        {"start": 0x21AC00, "end": 0x4DB6D0, "kind": "resource_archive_1"},
        {"start": 0x4DB6D0, "end": 0xF4D840, "kind": "resource_archive_2"},
        {"start": 0xF4D840, "end": 0xF4D850, "kind": "build_stamp"},
        {"start": 0xF4D850, "end": ROM_SIZE, "kind": "zero_padding"},
    ]
    assert all(left["end"] == right["start"] for left, right in zip(ranges, ranges[1:]))
    assert ranges[0]["start"] == 0 and ranges[-1]["end"] == len(data)
    assert not any(data[0xF4D850:])

    for item in ranges:
        item["size"] = item["end"] - item["start"]
    program_sections = [
        {"kind": kind, "start": start, "end": end, "size": end - start}
        for kind, start, end in PROGRAM_SECTIONS
    ]
    assert sum(item["size"] for item in program_sections) == PROGRAM_END - PROGRAM_START
    media_ranges = [
        {"kind": kind, "start": start, "end": end, "size": end - start}
        for kind, start, end in AUDIO_GRAPHICS_RANGES
    ]
    assert sum(item["size"] for item in media_ranges) == 0x21AC00 - PROGRAM_END
    report = {
        "sha1": digest,
        "rom_size": len(data),
        "ranges": ranges,
        "program_sections": program_sections,
        "program_section_bytes": {
            kind: sum(item["size"] for item in program_sections if item["kind"] == kind)
            for kind in ("text", "rodata", "data")
        },
        "audio_graphics_ranges": media_ranges,
        "audio_banks": audio_banks,
        "audio_sample_count": sum(bank["sample_count"] for bank in audio_banks),
        "audio_sample_bytes": sum(bank["sample_bytes"] for bank in audio_banks),
        "archives": archives,
        "resource_records": sum(item["record_count"] for item in archives),
        "nonempty_resources": sum(item["nonempty_count"] for item in archives),
        "build_stamp": data[0xF4D840:0xF4D850].rstrip(b"\0*").decode("ascii"),
        "range_sha256": {
            item["kind"]: hashlib.sha256(data[item["start"]:item["end"]]).hexdigest()
            for item in ranges
        },
        "signature_occurrences": {
            signature.decode("ascii").rstrip(" \0"): find_all(data, signature)
            for signature in (b"MORT", b"mdlh", PTR_MAGIC, WAVE_MAGIC)
        },
    }
    report["physical_ledger"] = physical_ledger(report)
    report["physical_ledger_rows"] = len(report["physical_ledger"])
    report["accounted_bytes"] = sum(item["size"] for item in report["physical_ledger"])
    kinds = sorted({item["kind"] for item in report["physical_ledger"]})
    report["physical_kind_summary"] = [
        {
            "kind": kind,
            "intervals": sum(item["kind"] == kind for item in report["physical_ledger"]),
            "bytes": sum(item["size"] for item in report["physical_ledger"] if item["kind"] == kind),
        }
        for kind in kinds
    ]
    assert report["accounted_bytes"] == len(data)
    return report


def markdown(report: dict) -> str:
    lines = [
        "# ROM physical-layout audit",
        "",
        f"SHA-1: `{report['sha1']}`  ",
        f"ROM size: {report['rom_size']:,} bytes (`0x{report['rom_size']:X}`)",
        f"Build stamp: `{report['build_stamp']}`",
        "",
        "All ranges are half-open. The table covers every physical ROM byte exactly once.",
        "",
        "| Start | End | Bytes | Classification |",
        "|---:|---:|---:|---|",
    ]
    for item in report["ranges"]:
        lines.append(
            f"| `0x{item['start']:06X}` | `0x{item['end']:06X}` | "
            f"{item['size']:,} | {item['kind']} |"
        )
    lines += [
        "",
        f"The machine-readable physical ledger contains {report['physical_ledger_rows']:,} "
        f"non-overlapping rows and accounts for {report['accounted_bytes']:,} of "
        f"{report['rom_size']:,} bytes. It subdivides both archive indices, every resource "
        "record, and every alignment gap.",
        "",
        "| Ledger classification | Intervals | Bytes |",
        "|---|---:|---:|",
    ]
    for item in sorted(report["physical_kind_summary"], key=lambda item: -item["bytes"]):
        lines.append(f"| {item['kind']} | {item['intervals']:,} | {item['bytes']:,} |")
    lines += [
        "",
        "## Initial program image",
        "",
        "| Start | End | Bytes | Section |",
        "|---:|---:|---:|---|",
    ]
    for item in report["program_sections"]:
        lines.append(
            f"| `0x{item['start']:06X}` | `0x{item['end']:06X}` | "
            f"{item['size']:,} | {item['kind']} |"
        )
    totals = report["program_section_bytes"]
    lines += [
        "",
        f"Executable sections total {totals['text']:,} bytes; read-only data totals "
        f"{totals['rodata']:,} bytes; initialized writable data totals {totals['data']:,} bytes. "
        "These are physical section sizes, not the smaller sum of function-candidate extents.",
        "",
        "## Audio and display data",
        "",
        "| Start | End | Bytes | Classification |",
        "|---:|---:|---:|---|",
    ]
    for item in report["audio_graphics_ranges"]:
        lines.append(
            f"| `0x{item['start']:06X}` | `0x{item['end']:06X}` | "
            f"{item['size']:,} | {item['kind']} |"
        )
    lines += [
        "",
        f"The two pointer banks index {report['audio_sample_count']} audio samples containing "
        f"{report['audio_sample_bytes']:,} bytes of encoded sample payload in total.",
        "",
        "| Bank | Samples | ADPCM | PCM16 | Looped | Sample bytes |",
        "|---|---:|---:|---:|---:|---:|",
    ]
    for bank in report["audio_banks"]:
        lines.append(
            f"| {bank['name']} | {bank['sample_count']} | {bank['adpcm_count']} | "
            f"{bank['pcm16_count']} | {bank['looped_count']} | {bank['sample_bytes']:,} |"
        )
    lines += [
        "",
        "## Resource archives",
        "",
    ]
    for archive in report["archives"]:
        lines += [
            f"### {archive['name']}",
            "",
            f"- Range: `0x{archive['start']:06X}..0x{archive['end']:06X}` "
            f"({archive['span_bytes']:,} bytes)",
            f"- Index: {archive['record_count']} records, {archive['index_bytes']:,} bytes",
            f"- Resources: {archive['nonempty_count']} nonempty, {archive['empty_count']} empty",
            f"- Storage paths: {archive['direct_count']} direct, "
            f"{archive['transformed_count']} transformed",
            f"- Indexed storage: {archive['stored_resource_bytes']:,} bytes",
            f"- Logical/allocation sizes: {archive['logical_resource_bytes']:,} bytes",
            f"- Record sizes: minimum {archive['minimum_record_bytes']:,}, median "
            f"{archive['median_record_bytes']:,.1f}, p90 {archive['p90_record_bytes']:,}, "
            f"maximum {archive['maximum_record_bytes']:,} bytes",
            f"- Exact duplicate payloads: {archive['duplicate_group_count']} groups, "
            f"{archive['duplicate_record_count']} records",
            f"- Inter-record/alignment padding: {archive['alignment_padding_bytes']:,} bytes",
            f"- Recognized leading signatures: MORT={archive['mort_signature_count']}, "
            f"mdlh={archive['mdlh_signature_count']}",
            "- Exact prefix families (records / stored bytes): "
            + ", ".join(
                f"{prefix or '(empty)'}={count}/{archive['prefix_stored_bytes'][prefix]:,}"
                for prefix, count in sorted(archive["prefix_counts"].items(),
                                            key=lambda item: (-item[1], item[0]))
            ),
            "",
        ]
        if archive["transformed_count"]:
            lines += [
                f"The {archive['transformed_count']} transformed records occupy "
                f"{archive['transformed_stored_bytes']:,} bytes and expand to "
                f"{archive['transformed_logical_bytes']:,} bytes "
                f"(stored/logical ratio {archive['transformed_ratio']:.3f}).",
                "",
            ]
        if archive["duplicate_groups"]:
            lines += [
                "Duplicate record-index groups: "
                + ", ".join("/".join(str(index) for index in group)
                            for group in archive["duplicate_groups"]),
                "",
            ]
    lines += [
        "## Signature census",
        "",
        "Counts below are full-ROM exact byte searches. Archive membership is still the "
        "stronger structural evidence.",
        "",
        "| Signature | Occurrences | Offsets |",
        "|---|---:|---|",
    ]
    for signature, offsets in report["signature_occurrences"].items():
        lines.append(
            f"| `{signature}` | {len(offsets)} | "
            + ", ".join(f"`0x{offset:06X}`" for offset in offsets[:8])
            + (" ..." if len(offsets) > 8 else "")
            + " |"
        )
    lines += [
        "## Important limits",
        "",
        "The archive indices establish exact physical boundaries and counts. Resource semantics "
        "(models, textures, animation, and related content) require format-specific parsing; "
        "prefixes alone are not used to claim semantic counts.",
        "In particular, an indexed sample is not automatically a music track or sound effect, "
        "and an archive record is not automatically one texture, model, or animation. The audit "
        "reports those container-level counts separately until their internal formats are decoded.",
        "",
    ]
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    parser.add_argument("--json", type=Path)
    parser.add_argument("--markdown", type=Path)
    parser.add_argument("--ledger", type=Path)
    args = parser.parse_args()
    report = audit(args.rom)
    if args.json:
        args.json.write_text(json.dumps(report, indent=2) + "\n")
    if args.markdown:
        args.markdown.write_text(markdown(report))
    if args.ledger:
        write_ledger(args.ledger, report["physical_ledger"])
    if not args.json and not args.markdown and not args.ledger:
        print(markdown(report), end="")


if __name__ == "__main__":
    main()
