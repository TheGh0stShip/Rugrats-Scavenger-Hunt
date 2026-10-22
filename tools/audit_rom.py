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


def be32(data: bytes, offset: int) -> int:
    return struct.unpack_from(">I", data, offset)[0]


def parse_archive(data: bytes, name: str, start: int, end: int) -> dict:
    count = be32(data, start)
    table_end = start + 8 + count * 12
    records = []
    for index in range(count):
        offset, logical_size, transform_size = struct.unpack_from(
            ">III", data, start + 8 + index * 12
        )
        stored_size = transform_size or logical_size
        records.append(
            {
                "index": index,
                "rom_start": start + offset,
                "rom_end": start + offset + stored_size,
                "logical_size": logical_size,
                "stored_size": stored_size,
                "transform_size": transform_size,
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
    direct = sum(record["transform_size"] == 0 and record["stored_size"] > 0 for record in records)
    transformed = sum(record["transform_size"] > 0 for record in records)
    stored_bytes = sum(record["stored_size"] for record in records)
    logical_bytes = sum(record["logical_size"] for record in records)
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
        "alignment_padding_bytes": padding,
        "mort_signature_count": mort_count,
        "mdlh_signature_count": mdlh_count,
        "prefix_counts": dict(sorted(prefixes.items())),
        "records": records,
    }


def audit(path: Path) -> dict:
    data = path.read_bytes()
    digest = hashlib.sha1(data).hexdigest()
    if digest != EXPECTED_SHA1:
        raise SystemExit(f"wrong ROM SHA-1: {digest}")
    if len(data) != ROM_SIZE:
        raise SystemExit(f"wrong ROM size: {len(data)}")

    archives = [parse_archive(data, *spec) for spec in ARCHIVES]
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
    return {
        "sha1": digest,
        "rom_size": len(data),
        "ranges": ranges,
        "program_sections": program_sections,
        "program_section_bytes": {
            kind: sum(item["size"] for item in program_sections if item["kind"] == kind)
            for kind in ("text", "rodata", "data")
        },
        "audio_graphics_ranges": media_ranges,
        "audio_sample_count": 178,
        "archives": archives,
        "resource_records": sum(item["record_count"] for item in archives),
        "nonempty_resources": sum(item["nonempty_count"] for item in archives),
    }


def markdown(report: dict) -> str:
    lines = [
        "# ROM physical-layout audit",
        "",
        f"SHA-1: `{report['sha1']}`  ",
        f"ROM size: {report['rom_size']:,} bytes (`0x{report['rom_size']:X}`)",
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
        f"The two pointer banks index {report['audio_sample_count']} audio samples in total.",
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
            f"- Inter-record/alignment padding: {archive['alignment_padding_bytes']:,} bytes",
            f"- Recognized leading signatures: MORT={archive['mort_signature_count']}, "
            f"mdlh={archive['mdlh_signature_count']}",
            "",
        ]
    lines += [
        "## Important limits",
        "",
        "The archive indices establish exact physical boundaries and counts. Resource semantics "
        "(models, textures, animation, and related content) require format-specific parsing; "
        "prefixes alone are not used to claim semantic counts.",
        "",
    ]
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    parser.add_argument("--json", type=Path)
    parser.add_argument("--markdown", type=Path)
    args = parser.parse_args()
    report = audit(args.rom)
    if args.json:
        args.json.write_text(json.dumps(report, indent=2) + "\n")
    if args.markdown:
        args.markdown.write_text(markdown(report))
    if not args.json and not args.markdown:
        print(markdown(report), end="")


if __name__ == "__main__":
    main()
