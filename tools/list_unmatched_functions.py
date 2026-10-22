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
    ROOT / "config/us/matching_kmc_as_units.tsv",
    ROOT / "config/us/matching_sn_units.tsv",
    ROOT / "config/us/matching_ido_units.tsv",
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


def load_matched_vrams(paths: tuple[Path, ...] = MANIFESTS) -> set[int]:
    matched = set()
    for path in paths:
        for line in path.read_text().splitlines():
            if not line or line.startswith("#"):
                continue
            rom_start = int(line.split("\t", 1)[0], 16)
            matched.add(rom_start + VRAM_ROM_DELTA)
    return matched


def unmatched_functions(
    functions: list[Function], matched_vrams: set[int], kind: str | None = None
) -> list[Function]:
    result = [function for function in functions if function.vram not in matched_vrams]
    if kind:
        result = [function for function in result if function.kind == kind]
    return sorted(result, key=lambda function: (function.size, function.vram))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--limit", type=int, default=40)
    parser.add_argument("--kind", help="restrict to one survey classification")
    args = parser.parse_args()

    queue = unmatched_functions(load_functions(), load_matched_vrams(), args.kind)
    print("VRAM       SIZE  KIND  FUNCTION")
    for function in queue[: args.limit]:
        print(
            f"0x{function.vram:08X}  0x{function.size:04X}  "
            f"{function.kind:<5} {function.name}"
        )
    print(f"shown={min(args.limit, len(queue))} unmatched={len(queue)}")


if __name__ == "__main__":
    main()
