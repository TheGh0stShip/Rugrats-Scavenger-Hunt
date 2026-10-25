#!/usr/bin/env python3
"""Inventory every executable-section byte outside catalogued CPU functions."""

from __future__ import annotations

import argparse
import hashlib
import tomllib
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
FUNCTIONS = ROOT / "analysis/survey/functions.toml"
OUTPUT = ROOT / "analysis/executable_nonfunction_inventory.tsv"
ROM_BASE = 0x1000
VRAM_BASE = 0x80000400
EXECUTABLE_RANGES = (
    (0x80000400, 0x800004F8),
    (0x800039F0, 0x800529A0),
)

KNOWN_EMBEDDED_DATA = (
    (0x800367EC, 0x800367F0),
    (0x80045888, 0x80045A54),
    (0x800485E0, 0x800485F4),
    (0x800488F0, 0x80048900),
)

KNOWN_MICROCODE = (
    (0x800367F0, 0x800368C0, "rsp_boot_text"),
    (0x800368C0, 0x80037CF0, "f3dex_1_23_text"),
    (0x80037CF0, 0x80038950, "n_aspmain_text"),
)


def rom_offset(vram: int) -> int:
    return ROM_BASE + vram - VRAM_BASE


def classify(start: int, end: int, data: bytes) -> tuple[str, str]:
    if not any(data):
        return "alignment_padding", "all bytes are zero"
    for left, right, name in KNOWN_MICROCODE:
        if left <= start and end <= right:
            evidence = "standard RSP microcode region"
            if name == "f3dex_1_23_text":
                evidence = "exact local SDK F3DEX binary match"
            return name, evidence
    if any(left <= start and end <= right for left, right in KNOWN_EMBEDDED_DATA):
        return "embedded_rodata", "survey-confirmed data inside executable section"
    return "inline_rodata", "non-instruction bytes between catalogued functions"


def gaps() -> list[tuple[int, int]]:
    document = tomllib.loads(FUNCTIONS.read_text())
    functions = sorted(
        (item["vram"], item["vram"] + item["size"])
        for section in document["section"]
        for item in section["functions"]
    )
    result: list[tuple[int, int]] = []
    for section_start, section_end in EXECUTABLE_RANGES:
        cursor = section_start
        for start, end in functions:
            if end <= section_start or start >= section_end:
                continue
            if start > cursor:
                result.append((cursor, start))
            cursor = max(cursor, min(end, section_end))
        if cursor < section_end:
            result.append((cursor, section_end))
    boundaries = sorted({value for left, right, _name in KNOWN_MICROCODE
                         for value in (left, right)})
    split: list[tuple[int, int]] = []
    for start, end in result:
        points = [start, *(point for point in boundaries if start < point < end), end]
        split.extend(zip(points, points[1:]))
    return split


def build(rom: bytes) -> str:
    lines = ["vram_start\tvram_end\trom_start\tsize\tclassification\tsha256\tevidence"]
    total = 0
    for start, end in gaps():
        offset = rom_offset(start)
        data = rom[offset:offset + end - start]
        if len(data) != end - start:
            raise ValueError(f"ROM is too short for 0x{start:X}..0x{end:X}")
        kind, evidence = classify(start, end, data)
        lines.append(
            f"0x{start:08X}\t0x{end:08X}\t0x{offset:X}\t0x{end-start:X}\t"
            f"{kind}\t{hashlib.sha256(data).hexdigest()}\t{evidence}"
        )
        total += end - start
    if total != 9448:
        raise ValueError(f"expected 9448 non-function executable bytes, found {total}")
    return "\n".join(lines) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()
    generated = build(args.rom.read_bytes())
    if args.write:
        OUTPUT.write_text(generated)
    elif not OUTPUT.exists() or OUTPUT.read_text() != generated:
        raise SystemExit(f"stale executable inventory: run {Path(__file__).name} ROM --write")
    print("verified 9,448 non-function executable bytes with no gaps or overlaps")


if __name__ == "__main__":
    main()
