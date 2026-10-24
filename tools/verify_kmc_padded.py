#!/usr/bin/env python3
"""Verify KMC C units whose catalogue ranges include trailing zero padding."""

from __future__ import annotations

import argparse
import hashlib
import os
import shlex
import subprocess
import sys
import tempfile
from pathlib import Path


def verify(manifest: Path, project: Path, rom: Path, symbols: Path) -> None:
    rom_data = rom.read_bytes()
    prefix_rows: list[str] = []
    checked_bytes = 0

    for number, line in enumerate(manifest.read_text().splitlines(), 1):
        if not line or line.startswith("#"):
            continue
        fields = line.split("\t")
        if len(fields) != 5:
            raise ValueError(f"{manifest}:{number}: expected five fields")
        start, size = int(fields[0], 0), int(fields[1], 0)
        flags = shlex.split(fields[3])
        padding_flags = [f for f in flags if f.startswith("--trailing-padding=")]
        if len(padding_flags) != 1:
            raise ValueError(f"{manifest}:{number}: missing trailing-padding flag")
        padding = int(padding_flags[0].split("=", 1)[1], 0)
        compile_flags = [f for f in flags if f not in padding_flags]
        if padding <= 0 or padding >= size or padding % 4:
            raise ValueError(f"{manifest}:{number}: invalid trailing padding")
        if not (project / fields[2]).is_file():
            raise ValueError(f"{manifest}:{number}: source does not exist")

        full = rom_data[start:start + size]
        if hashlib.sha256(full).hexdigest() != fields[4]:
            raise ValueError(f"{manifest}:{number}: retail range digest differs")
        if full[-padding:] != bytes(padding):
            raise ValueError(f"{manifest}:{number}: trailing range is not zero padding")

        prefix_size = size - padding
        prefix_digest = hashlib.sha256(full[:prefix_size]).hexdigest()
        prefix_rows.append("\t".join((
            fields[0], hex(prefix_size), fields[2],
            shlex.join(compile_flags), prefix_digest,
        )))
        checked_bytes += size

    with tempfile.NamedTemporaryFile("w", suffix=".tsv", delete=False) as handle:
        handle.write("\n".join(prefix_rows) + "\n")
        temporary = Path(handle.name)
    try:
        command = [
            sys.executable, "-m", "n64dk", "match-batch",
            f"{temporary}=kmc+as", "--project", str(project),
            "--rom", str(rom), "--symbols", str(symbols),
        ]
        result = subprocess.run(
            command, cwd=project, env=os.environ.copy(), text=True,
            capture_output=True,
        )
        if result.returncode:
            raise RuntimeError((result.stdout + result.stderr).strip())
    finally:
        temporary.unlink(missing_ok=True)

    print(f"verified {len(prefix_rows)} padded KMC unit(s), {checked_bytes} exact bytes")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("manifest", type=Path)
    parser.add_argument("--project", type=Path, default=Path.cwd())
    parser.add_argument("--rom", type=Path, required=True)
    parser.add_argument("--symbols", type=Path, required=True)
    args = parser.parse_args()
    verify(
        args.manifest.resolve(), args.project.resolve(),
        args.rom.resolve(), args.symbols.resolve(),
    )


if __name__ == "__main__":
    main()
