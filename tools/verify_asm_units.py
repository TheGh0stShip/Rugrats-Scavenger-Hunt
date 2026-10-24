#!/usr/bin/env python3
"""Preprocess and verify reviewed handwritten assembly units."""

from __future__ import annotations

import argparse
import hashlib
import os
import shlex
import subprocess
import sys
import tempfile
from pathlib import Path


ROM_TO_VRAM = 0x7FFFF400


def run(command: list[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        command, cwd=cwd, env=os.environ.copy(), capture_output=True, text=True
    )
    if result.returncode:
        raise RuntimeError((result.stdout + result.stderr).strip())
    return result


def verify(manifest: Path, project: Path, rom: Path, symbols: Path) -> None:
    checked = 0
    checked_bytes = 0

    with tempfile.TemporaryDirectory(prefix="rugrats-asm-") as temporary:
        work = Path(temporary)
        for number, line in enumerate(manifest.read_text().splitlines(), 1):
            if not line or line.startswith("#"):
                continue
            fields = line.split("\t")
            if len(fields) != 5:
                raise ValueError(f"{manifest}:{number}: expected five fields")
            start, size = int(fields[0], 0), int(fields[1], 0)
            source = project / fields[2]
            if hashlib.sha256(source.read_bytes()).hexdigest() != fields[4]:
                raise ValueError(f"{source}: SHA-256 differs from manifest")

            preprocessed = work / f"unit_{checked}.s"
            run([
                "gcc", "-E", "-P", "-x", "assembler-with-cpp",
                str(source), "-o", str(preprocessed),
            ], project)
            result = run([
                sys.executable, "-m", "n64dk", "match", str(preprocessed),
                "--rom", str(rom), "--project", str(project),
                "--symbols", str(symbols), "--addr",
                f"0x{start + ROM_TO_VRAM:08X}", "--size", hex(size),
                "--pipeline", "as", "--flags", fields[3],
            ], project)
            if not result.stdout.startswith("MATCH "):
                raise ValueError(f"{source}: did not match retail ROM")
            checked += 1
            checked_bytes += size

    print(f"verified {checked} handwritten assembly unit(s), {checked_bytes} exact bytes")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("manifest", type=Path)
    parser.add_argument("--project", type=Path, default=Path.cwd())
    parser.add_argument("--rom", type=Path, required=True)
    parser.add_argument("--symbols", type=Path, required=True)
    args = parser.parse_args()
    verify(
        args.manifest.resolve(), args.project.resolve(), args.rom.resolve(),
        args.symbols.resolve(),
    )


if __name__ == "__main__":
    main()
