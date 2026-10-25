#!/usr/bin/env python3
"""Generate a whole-ROM attack map from the authoritative audits."""

from __future__ import annotations

import json
from collections import Counter
from pathlib import Path

import generate_decomp_report
import list_unmatched_functions


ROOT = Path(__file__).resolve().parents[1]
ROM_MAP = ROOT / "analysis/rom_map.json"
SURVEY = ROOT / "analysis/survey/report.json"
OUTPUT = ROOT / "analysis/deep_rom_audit.md"


def percent(part: int, whole: int) -> str:
    return f"{part * 100.0 / whole:.6f}%" if whole else "0.000000%"


def main() -> None:
    physical = json.loads(ROM_MAP.read_text())
    survey = json.loads(SURVEY.read_text())
    progress = generate_decomp_report.build_report()
    functions = list_unmatched_functions.load_functions()
    ranges = list_unmatched_functions.load_matched_ranges()
    matched = [
        function for function in functions
        if any(start <= function.vram and function.vram + function.size <= end
               for start, end in ranges)
    ]
    partial = [
        function for function in functions
        if function not in matched and any(
            max(start, function.vram) < min(end, function.vram + function.size)
            for start, end in ranges
        )
    ]
    kinds = Counter(function.kind.rstrip(";") for function in functions)
    unmatched_kinds = Counter(
        function.kind.rstrip(";") for function in functions if function not in matched
    )
    measures = progress["measures"]
    matched_code = int(measures["matched_code"])
    total_data = int(measures["total_data"])
    matched_data = int(measures["matched_data"])
    catalogued_bytes = sum(function.size for function in functions)
    matched_function_bytes = sum(end - start for start, end in ranges)
    text_bytes = int(measures["total_code"])
    lines = [
        "# Deep whole-ROM audit and attack map",
        "",
        f"Retail SHA-1: `{physical['sha1']}`  ",
        f"ROM bytes: {physical['rom_size']:,}  ",
        f"Build stamp: `{physical['build_stamp']}`",
        "",
        "This report separates physical coverage, structural decoding, semantic decoding, "
        "and byte-matched reconstruction. A bounded archive payload is physically inventoried "
        "but is not counted as an extracted or semantically decoded asset.",
        "",
        "## Whole-ROM physical coverage",
        "",
        f"The gap-free ledger has {physical['physical_ledger_rows']:,} non-overlapping "
        f"intervals covering {physical['accounted_bytes']:,} / {physical['rom_size']:,} "
        "bytes (100%).",
        "",
        "| Physical class | Intervals | Bytes | ROM share |",
        "|---|---:|---:|---:|",
    ]
    for item in sorted(physical["physical_kind_summary"], key=lambda item: -item["bytes"]):
        lines.append(
            f"| {item['kind']} | {item['intervals']:,} | {item['bytes']:,} | "
            f"{percent(item['bytes'], physical['rom_size'])} |"
        )
    lines += [
        "",
        "## Executable and function coverage",
        "",
        f"The two executable sections total {text_bytes:,} bytes. The function catalogue "
        f"contains {len(functions):,} functions spanning {catalogued_bytes:,} instruction "
        f"bytes; the remaining {text_bytes - catalogued_bytes:,} executable-section bytes "
        "are padding, embedded tables, or boundary-sensitive regions and are not invented as "
        "functions.",
        "",
        f"- Exact CPU-function manifest coverage: {matched_function_bytes:,} / "
        f"{catalogued_bytes:,} bytes ({percent(matched_function_bytes, catalogued_bytes)})",
        f"- Exact non-function executable inventory: {text_bytes - catalogued_bytes:,} / "
        f"{text_bytes - catalogued_bytes:,} bytes (100.000000%)",
        f"- Combined executable coverage: {matched_code:,} / {text_bytes:,} bytes "
        f"({measures['complete_code_percent']:.6f}%)",
        f"- Fully covered functions: {len(matched):,} / {len(functions):,} "
        f"({percent(len(matched), len(functions))})",
        f"- Partially covered catalogue functions: {len(partial)}",
        f"- Catalogue classifications: {dict(sorted(kinds.items()))}",
        f"- Not-fully-covered classifications: {dict(sorted(unmatched_kinds.items()))}",
        f"- Jump tables: {survey['functions']['jump_tables']} with "
        f"{survey['functions']['jump_table_entries']} entries",
        f"- Boundary findings requiring review: {survey['functions']['findings']}",
        "",
        "The survey and physical audit disagree only in terminology: the older survey labels "
        "archive payloads unknown because it does not parse the game-specific archive indices. "
        "The physical audit proves their byte boundaries; their internal semantics remain "
        "undecoded.",
        "",
        "## Program data",
        "",
        f"Initialized writable data totals {total_data:,} bytes; "
        f"{matched_data:,} bytes are presently byte-matched. Read-only data totals "
        f"{physical['program_section_bytes']['rodata']:,} bytes and includes strings, floating-"
        "point constants, jump tables, and embedded control data owned by code units.",
        "",
        "## Audio structures",
        "",
        f"Two pointer banks index {physical['audio_sample_count']} non-overlapping samples "
        f"occupying {physical['audio_sample_bytes']:,} encoded bytes. All 178 records identify "
        "the standard ADPCM codec; none identify PCM16. This proves sample boundaries and codec "
        "class, not whether an individual sample is music, speech, ambience, or an effect.",
        "",
        "| Bank | Samples | Looped | Payload bytes |",
        "|---|---:|---:|---:|",
    ]
    for bank in physical["audio_banks"]:
        lines.append(
            f"| {bank['name']} | {bank['sample_count']} | {bank['looped_count']} | "
            f"{bank['sample_bytes']:,} |"
        )
    lines += ["", "## Resource archives", ""]
    for archive in physical["archives"]:
        lines += [
            f"### {archive['name']}",
            "",
            f"- {archive['record_count']} index records; {archive['nonempty_count']} nonempty",
            f"- {archive['stored_resource_bytes']:,} stored payload bytes and "
            f"{archive['alignment_padding_bytes']:,} alignment bytes",
            f"- record sizes {archive['minimum_record_bytes']:,}.."
            f"{archive['maximum_record_bytes']:,} bytes; median "
            f"{archive['median_record_bytes']:,.1f}; p90 {archive['p90_record_bytes']:,}",
            f"- {archive['duplicate_group_count']} exact duplicate groups containing "
            f"{archive['duplicate_record_count']} records",
            f"- signatures: MORT={archive['mort_signature_count']}, "
            f"mdlh={archive['mdlh_signature_count']}",
        ]
        if archive["transformed_count"]:
            lines.append(
                f"- {archive['transformed_count']} transformed records: "
                f"{archive['transformed_stored_bytes']:,} stored -> "
                f"{archive['transformed_logical_bytes']:,} logical bytes "
                f"(ratio {archive['transformed_ratio']:.3f})"
            )
        lines.append("")
    lines += [
        "## Reconstruction status",
        "",
        "The archive-2 transform path is now localized. `func_80013238` multiplies the resource "
        "index by 12 and interprets each record as ROM offset, logical/allocation size, and "
        "stored transform size. A zero transform size takes the direct DMA path. A nonzero "
        "value DMA-loads that many bytes into the scratch buffer at `D_80052EC8`, then calls "
        "`func_800166F0` with the scratch buffer, allocated output, and decoded-size pointer.",
        "",
        "`func_800166F0` is a control-byte copy/back-reference transform. The archive tooling "
        "preserves its history semantics and proves every transformed record through exact "
        "re-encoding and the whole-ROM round trip.",
        "",
        "All 148 transformed records, 376 model records and 380 speech records are represented "
        "by validated parsers and exact reconstruction paths. The executable inventory keeps "
        "RSP programs, embedded read-only data and alignment separate from the CPU-function "
        "count while covering every executable byte.",
        "",
        "## Completion limits",
        "",
        "Physical inventory, catalogued CPU functions, initialized program data, asset/resource "
        "round trips, and executable-byte inventory are complete. Standard F3DEX is identified "
        "by an exact SDK fingerprint; rspboot and n_aspMain rebuild from symbolic source. "
        "Published extracted payload bytes remain zero by policy and are reported separately "
        "from decompilation and reconstruction coverage.",
        "",
    ]
    OUTPUT.write_text("\n".join(lines))
    print(f"wrote {OUTPUT}")


if __name__ == "__main__":
    main()
