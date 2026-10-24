#!/usr/bin/env python3
"""Generate an objdiff v2 progress report from the reviewed manifests."""

from __future__ import annotations

import argparse
import json
import tomllib
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
FUNCTIONS = ROOT / "analysis/survey/functions.toml"
DATA_PROGRESS = ROOT / "config/us/data_progress.toml"
ASSET_PROGRESS = ROOT / "config/us/asset_progress.toml"
MANIFESTS = (
    ROOT / "config/us/matching_units.tsv",
    ROOT / "config/us/matching_kmc_normalized_units.tsv",
    ROOT / "config/us/matching_kmc_as_units.tsv",
    ROOT / "config/us/matching_sn_units.tsv",
    ROOT / "config/us/matching_sn_normalized_units.tsv",
    ROOT / "config/us/matching_ido_units.tsv",
)
DEFAULT_OUTPUT = ROOT / "build/report.json"
PROGRAM_ROM_END = 0x57E80
PROGRAM_TEXT_BYTES = 0x4EEC0
PROGRAM_INITIALIZED_DATA_BYTES = 0x7FC0
ROM_BYTES = 0x1000000


def manifest_ranges() -> list[tuple[int, int, str]]:
    ranges = []
    for path in MANIFESTS:
        for line_number, line in enumerate(path.read_text().splitlines(), 1):
            if not line or line.startswith("#"):
                continue
            fields = line.split("\t")
            if len(fields) != 5:
                raise ValueError(f"{path}:{line_number}: expected five fields")
            start = int(fields[0], 0)
            size = int(fields[1], 0)
            ranges.append((start, start + size, fields[2]))
    return ranges


def covered_bytes(start: int, end: int,
                  ranges: list[tuple[int, int, str]]) -> int:
    spans = sorted(
        (max(start, left), min(end, right))
        for left, right, _source in ranges
        if left < end and right > start
    )
    total = 0
    cursor = start
    for left, right in spans:
        if right <= cursor:
            continue
        left = max(left, cursor)
        total += right - left
        cursor = right
    return total


def source_for(start: int, end: int,
               ranges: list[tuple[int, int, str]]) -> str | None:
    for left, right, source in ranges:
        if left <= start and end <= right:
            return source
    return None


def inventory_progress() -> tuple[int, int, int, int, int]:
    """Validate the physical ledger and return truthful inventory totals.

    Asset extraction is deliberately independent from identification: knowing
    the byte boundaries of an archive or audio bank does not mean its payload
    has been extracted and published.
    """
    progress = tomllib.loads(ASSET_PROGRESS.read_text())
    region = progress["region"]
    asset_start, asset_end = region["rom_start"], region["rom_end"]
    extracted = progress["extraction"]["matched"]
    ledger = ROOT / progress["ledger"]["path"]
    rows = ledger.read_text().splitlines()
    if not rows or rows[0].split("\t")[:4] != ["start", "end", "size", "kind"]:
        raise ValueError(f"{ledger}: unexpected physical-ledger header")

    cursor = 0
    audio_bytes = 0
    resource_bytes = 0
    for line_number, line in enumerate(rows[1:], 2):
        fields = line.split("\t")
        start, end, size = int(fields[0], 0), int(fields[1], 0), int(fields[2], 0)
        kind = fields[3]
        if start != cursor or end <= start or end - start != size:
            raise ValueError(f"{ledger}:{line_number}: gap, overlap or invalid size")
        cursor = end
        if kind.startswith("audio_"):
            audio_bytes += size
        if kind in {"archive_index", "archive_resource", "alignment_padding"}:
            resource_bytes += size

    if cursor != ROM_BYTES:
        raise ValueError(f"{ledger}: covers {cursor} bytes, expected {ROM_BYTES}")
    asset_bytes = asset_end - asset_start
    if asset_start != PROGRAM_ROM_END or asset_end <= asset_start:
        raise ValueError(f"{ASSET_PROGRESS}: invalid asset region")
    if not 0 <= extracted <= asset_bytes:
        raise ValueError(f"{ASSET_PROGRESS}: invalid extracted byte count")
    return asset_bytes, extracted, ROM_BYTES, audio_bytes, resource_bytes


def measures(total_code: int, matched_code: int, total_functions: int,
             matched_functions: int, total_data: int = 0,
             matched_data: int = 0) -> dict:
    code_percent = matched_code * 100.0 / total_code if total_code else 0.0
    data_percent = matched_data * 100.0 / total_data if total_data else 0.0
    function_percent = (
        matched_functions * 100.0 / total_functions if total_functions else 0.0
    )
    return {
        "fuzzy_match_percent": code_percent,
        "total_code": str(total_code),
        "matched_code": str(matched_code),
        "matched_code_percent": code_percent,
        "total_data": str(total_data),
        "matched_data": str(matched_data),
        "matched_data_percent": data_percent,
        "total_functions": total_functions,
        "matched_functions": matched_functions,
        "matched_functions_percent": function_percent,
        "complete_code": str(matched_code),
        "complete_code_percent": code_percent,
        "complete_data": str(matched_data),
        "complete_data_percent": data_percent,
        "total_units": total_functions,
        "complete_units": matched_functions,
    }


def data_category_measures(total_data: int, matched_data: int) -> dict:
    """Expose data-only progress in category-oriented tracker views.

    Aggregate code fields remain code-only. Category-local code fields mirror
    the data ratio because the tracker currently renders categories using the
    code percentage even when a category contains only data.
    """
    return measures(
        total_data, matched_data, 0, 0, total_data, matched_data
    )


def build_report() -> dict:
    section = tomllib.loads(FUNCTIONS.read_text())["section"][0]
    functions = section["functions"]
    ranges = manifest_ranges()
    rom_delta = section["vram"] - section["rom"]
    units = []
    matched_code = 0
    matched_functions = 0

    for function in functions:
        size = function["size"]
        vram = function["vram"]
        rom_start = vram - rom_delta
        rom_end = rom_start + size
        matched = covered_bytes(rom_start, rom_end, ranges)
        complete = matched == size
        source = source_for(rom_start, rom_end, ranges)
        unit_measures = measures(size, matched, 1, int(complete))
        units.append({
            "name": function["name"],
            "measures": unit_measures,
            "functions": [{
                "name": function["name"],
                "size": str(size),
                "fuzzy_match_percent": unit_measures["fuzzy_match_percent"],
                "address": str(vram),
                "metadata": {"virtual_address": str(vram)},
            }],
            "metadata": {
                "complete": complete,
                "source_path": source or f"asm/us/{function['name']}.s",
                "progress_categories": ["code"],
                "auto_generated": source is None,
            },
        })
        matched_code += matched
        matched_functions += int(complete)

    catalogued_function_bytes = sum(function["size"] for function in functions)
    total_code = PROGRAM_TEXT_BYTES
    total_program_data = PROGRAM_INITIALIZED_DATA_BYTES
    data_progress = tomllib.loads(DATA_PROGRESS.read_text())["initialized_data"]
    matched_data = data_progress["matched"]
    audited_program_data = matched_data + data_progress["unmatched"]
    if matched_code != sum(right - left for left, right, _source in ranges):
        raise ValueError("matching manifests include bytes outside the function catalogue")
    if audited_program_data != total_program_data:
        raise ValueError("data-progress manifest does not cover initialized program data")
    if section["rom"] + total_code + total_program_data != PROGRAM_ROM_END:
        raise ValueError("code and initialized data do not cover the program image")
    if catalogued_function_bytes > total_code:
        raise ValueError("function catalogue exceeds executable section size")

    overall = measures(
        total_code,
        matched_code,
        len(functions),
        matched_functions,
        total_program_data,
        matched_data,
    )
    code = measures(
        total_code,
        matched_code,
        len(functions),
        matched_functions,
    )
    asset_bytes, extracted_assets, rom_bytes, audio_bytes, resource_bytes = (
        inventory_progress()
    )
    return {
        "measures": overall,
        "units": units,
        "version": 2,
        "categories": [
            {
                "id": "code",
                "name": "Program code",
                "measures": code,
            },
            {
                "id": "program_data",
                "name": "Byte-matched initialized data",
                "measures": data_category_measures(
                    total_program_data, matched_data
                ),
            },
            {
                "id": "assets_extracted",
                "name": "Extracted assets",
                "measures": data_category_measures(
                    asset_bytes, extracted_assets
                ),
            },
            {
                "id": "asset_inventory",
                "name": "Inventoried asset containers",
                "measures": data_category_measures(asset_bytes, asset_bytes),
            },
            {
                "id": "audio_inventory",
                "name": "Inventoried audio banks and samples",
                "measures": data_category_measures(audio_bytes, audio_bytes),
            },
            {
                "id": "resource_inventory",
                "name": "Inventoried resource archives",
                "measures": data_category_measures(
                    resource_bytes, resource_bytes
                ),
            },
            {
                "id": "rom_inventory",
                "name": "Physical ROM inventory coverage",
                "measures": data_category_measures(rom_bytes, rom_bytes),
            },
        ],
    }


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    args = parser.parse_args()
    report = build_report()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n")
    result = report["measures"]
    print(
        f"wrote {args.output}: {result['matched_code']} / "
        f"{result['total_code']} code bytes"
    )


if __name__ == "__main__":
    main()
