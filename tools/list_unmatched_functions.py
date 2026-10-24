#!/usr/bin/env python3
"""List unmatched functions in a stable, size-ranked work queue."""

from __future__ import annotations

import argparse
import re
from pathlib import Path
from typing import NamedTuple


ROOT = Path(__file__).resolve().parents[1]
FUNCTIONS = ROOT / "analysis/survey/functions.toml"
MANIFESTS = (
    ROOT / "config/us/matching_units.tsv",
    ROOT / "config/us/matching_kmc_normalized_units.tsv",
    ROOT / "config/us/matching_kmc_as_units.tsv",
    ROOT / "config/us/matching_kmc_padded_units.tsv",
    ROOT / "config/us/matching_sn_units.tsv",
    ROOT / "config/us/matching_sn_normalized_units.tsv",
    ROOT / "config/us/matching_ido_units.tsv",
    ROOT / "config/us/matching_ido_normalized_units.tsv",
    ROOT / "config/us/matching_asm_units.tsv",
)
VRAM_ROM_DELTA = 0x7FFFF400
FUNCTION_RE = re.compile(
    r'name = "(?P<name>[^"]+)", vram = (?P<vram>0x[0-9a-f]+), '
    r'size = (?P<size>0x[0-9a-f]+) }, # (?P<kind>\S+)'
)


class Function(NamedTuple):
    name: str
    vram: int
    size: int
    kind: str


def load_functions(path: Path = FUNCTIONS) -> list[Function]:
    functions = []
    for line in path.read_text().splitlines():
        match = FUNCTION_RE.search(line)
        if match:
            functions.append(
                Function(
                    match["name"],
                    int(match["vram"], 16),
                    int(match["size"], 16),
                    match["kind"],
                )
            )
    return functions


def load_matched_ranges(
    paths: tuple[Path, ...] = MANIFESTS,
) -> list[tuple[int, int]]:
    matched = []
    for path in paths:
        for line in path.read_text().splitlines():
            if not line or line.startswith("#"):
                continue
            fields = line.split("\t")
            rom_start = int(fields[0], 16)
            size = int(fields[1], 16)
            vram_start = rom_start + VRAM_ROM_DELTA
            matched.append((vram_start, vram_start + size))
    return matched


def unmatched_functions(
    functions: list[Function], matched_ranges: list[tuple[int, int]],
    kind: str | None = None,
) -> list[Function]:
    result = [
        function
        for function in functions
        if not any(
            start <= function.vram
            and function.vram + function.size <= end
            for start, end in matched_ranges
        )
    ]
    if kind:
        result = [function for function in result if function.kind == kind]
    return sorted(result, key=lambda function: (function.size, function.vram))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--limit", type=int, default=40)
    parser.add_argument("--kind", help="restrict to one survey classification")
    args = parser.parse_args()

    queue = unmatched_functions(load_functions(), load_matched_ranges(), args.kind)
    print("VRAM       SIZE  KIND  FUNCTION")
    for function in queue[: args.limit]:
        print(
            f"0x{function.vram:08X}  0x{function.size:04X}  "
            f"{function.kind:<5} {function.name}"
        )
    print(f"shown={min(args.limit, len(queue))} unmatched={len(queue)}")


if __name__ == "__main__":
    main()
