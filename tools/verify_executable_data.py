#!/usr/bin/env python3
"""Reconstruct executable-section non-microcode data from its C declarations."""

from __future__ import annotations

import argparse
import csv
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
INVENTORY = ROOT / "analysis/executable_nonfunction_inventory.tsv"
SOURCE = ROOT / "src/data/executable_nonfunction_data.c"
MICROCODE = {"rsp_boot_text", "f3dex_1_23_text", "n_aspmain_text"}
ARRAY = re.compile(
    r"const u32 exec_data_(?P<address>[0-9A-F]{8})\[\] = \{"
    r"(?P<body>.*?)\n\};",
    re.DOTALL,
)
WORD = re.compile(r"0x([0-9A-F]{8})")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    args = parser.parse_args()
    rom = args.rom.read_bytes()
    declarations = {
        int(match.group("address"), 16): b"".join(
            int(word, 16).to_bytes(4, "big")
            for word in WORD.findall(match.group("body"))
        )
        for match in ARRAY.finditer(SOURCE.read_text())
    }

    expected_addresses: set[int] = set()
    total = 0
    with INVENTORY.open() as stream:
        for row in csv.DictReader(stream, delimiter="\t"):
            if row["classification"] in MICROCODE:
                continue
            address = int(row["vram_start"], 0)
            offset = int(row["rom_start"], 0)
            size = int(row["size"], 0)
            expected_addresses.add(address)
            data = declarations.get(address)
            if data is None:
                raise SystemExit(f"missing C declaration for 0x{address:08X}")
            if len(data) != size:
                raise SystemExit(
                    f"0x{address:08X}: declaration has {len(data)} bytes, expected {size}"
                )
            if data != rom[offset:offset + size]:
                raise SystemExit(f"0x{address:08X}: reconstructed bytes differ from retail")
            total += size
    extras = declarations.keys() - expected_addresses
    if extras:
        raise SystemExit("unexpected C declarations: " + ", ".join(hex(x) for x in extras))
    if total != 904:
        raise SystemExit(f"expected 904 reconstructed data bytes, found {total}")
    print("verified 904 executable-section data/alignment bytes from C declarations")


if __name__ == "__main__":
    main()
