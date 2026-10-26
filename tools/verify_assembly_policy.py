#!/usr/bin/env python3
"""Restrict game assembly to the audited low-level source units."""

from __future__ import annotations

import argparse
from pathlib import Path


AUTHENTIC_GAME_ASSEMBLY = {
    (0x20450, 0x1A4, "src/game/emit_scaled_textured_rectangle_8001f850.S"),
    (0x20640, 0x144, "src/game/emit_textured_rectangle_8001fa40.S"),
    (0x58BC, 0x534, "src/game/debug_stub_exception_handler.S"),
}


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "manifest", type=Path, nargs="?",
        default=Path("config/us/matching_asm_units.tsv"),
    )
    args = parser.parse_args()

    actual: set[tuple[int, int, str]] = set()
    for number, line in enumerate(args.manifest.read_text().splitlines(), 1):
        if not line or line.startswith("#"):
            continue
        fields = line.split("\t")
        if len(fields) != 5:
            raise SystemExit(f"{args.manifest}:{number}: expected five fields")
        row = (int(fields[0], 0), int(fields[1], 0), fields[2])
        if fields[2].startswith("src/game/"):
            actual.add(row)

    unexpected = actual - AUTHENTIC_GAME_ASSEMBLY
    if unexpected:
        details = "\n".join(
            f"  0x{start:X} 0x{size:X} {source}"
            for start, size, source in sorted(unexpected)
        )
        raise SystemExit(f"unexpected game assembly unit(s):\n{details}")

    missing = AUTHENTIC_GAME_ASSEMBLY - actual
    if missing:
        details = "\n".join(
            f"  0x{start:X} 0x{size:X} {source}"
            for start, size, source in sorted(missing)
        )
        raise SystemExit(f"missing audited low-level assembly unit(s):\n{details}")

    print(
        f"assembly policy verified: {len(actual)} audited low-level game unit(s); "
        "no ordinary game fallback remains"
    )


if __name__ == "__main__":
    main()
