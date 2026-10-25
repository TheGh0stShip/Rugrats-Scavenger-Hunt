#!/usr/bin/env python3
"""Rebuild symbolic RSP sources and verify all embedded microcode bytes."""

from __future__ import annotations

import argparse
import hashlib
import subprocess
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
ROM_BASE = 0x1000
VRAM_BASE = 0x80000400


def rom_offset(vram: int) -> int:
    return ROM_BASE + vram - VRAM_BASE


def build(armips: Path, source: Path, output: Path) -> bytes:
    subprocess.run(
        [str(armips), "-strequ", "CODE_FILE", str(output), str(source)],
        cwd=ROOT,
        check=True,
    )
    return output.read_bytes()


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    parser.add_argument("--armips", type=Path, required=True)
    args = parser.parse_args()
    rom = args.rom.read_bytes()

    with tempfile.TemporaryDirectory(prefix="rugrats-rsp-") as directory:
        temporary = Path(directory)
        boot = build(args.armips, ROOT / "rsp/rspboot.s", temporary / "rspboot.bin")
        audio = build(
            args.armips, ROOT / "rsp/n_aspMain.s", temporary / "n_aspMain.text.bin"
        )

    expected_boot = rom[rom_offset(0x800367F0):rom_offset(0x800368C0)]
    if boot != expected_boot[:len(boot)] or expected_boot[len(boot):] != bytes(4):
        raise SystemExit("rspboot source does not reproduce retail plus its alignment word")
    expected_audio = rom[rom_offset(0x80037CF0):rom_offset(0x80038950)]
    if audio != expected_audio:
        raise SystemExit("n_aspMain source does not reproduce retail")

    graphics = rom[rom_offset(0x800368C0):rom_offset(0x80037CF0)]
    graphics_hash = hashlib.sha256(graphics).hexdigest()
    if graphics_hash != "9c19d2f0cba40449f00af9a0ca47d211f41032fa1ef574d8d1e03d9c1787e433":
        raise SystemExit("retail F3DEX 1.23 fingerprint changed")

    print("verified 208 rspboot bytes (204 source + 4 alignment bytes)")
    print("verified 5,168 SDK F3DEX 1.23 bytes")
    print("verified 3,168 symbolic n_aspMain bytes")


if __name__ == "__main__":
    main()
