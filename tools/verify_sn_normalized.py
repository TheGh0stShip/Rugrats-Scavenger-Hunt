#!/usr/bin/env python3
"""Compile and verify SN C units requiring reviewed assembler scheduling."""

from __future__ import annotations

import argparse
import hashlib
import shlex
import subprocess
import tempfile
from pathlib import Path

from n64dk.elf import Elf
from n64dk.match.compare import relocate
from n64dk.match.symbols import load
from n64dk.match.toolchain import load_config

from normalize_sn_schedule import normalize


ROM_TO_VRAM = 0x7FFFF400


def run(command: list[str], cwd: Path) -> None:
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True)
    if result.returncode:
        detail = (result.stderr or result.stdout).strip()
        raise RuntimeError(f"{Path(command[0]).name} failed: {detail}")


def verify(manifest: Path, project: Path, rom: Path, symbols: Path) -> None:
    config = load_config(project)
    sn = config.toolchain("sn")
    if sn is None:
        raise SystemExit("SN Systems toolchain not found")
    symbol_map = load([symbols])
    rom_data = rom.read_bytes()
    checked = 0
    checked_bytes = 0

    with tempfile.TemporaryDirectory(prefix="rugrats-sn-normalized-") as tmp:
        work = Path(tmp)
        for number, line in enumerate(manifest.read_text().splitlines(), 1):
            if not line or line.startswith("#"):
                continue
            fields = line.split("\t")
            if len(fields) != 5:
                raise ValueError(f"{manifest}:{number}: expected five fields")
            start, size = int(fields[0], 0), int(fields[1], 0)
            source = project / fields[2]
            flags = shlex.split(fields[3])
            digest = hashlib.sha256(source.read_bytes()).hexdigest()
            if digest != fields[4]:
                raise ValueError(f"{source}: SHA-256 differs from manifest")

            stem = work / f"unit_{checked}"
            preprocessed = stem.with_suffix(".i")
            assembly = stem.with_suffix(".s")
            normalized = stem.with_name(stem.name + "_normalized.s")
            psyq = stem.with_suffix(".obj")
            obj = stem.with_suffix(".o")
            run([
                "gcc", "-E", "-P", "-undef", "-D__GNUC__=2",
                "-D__GNUC_MINOR__=8", str(source), "-o", str(preprocessed),
            ], project)
            run([
                str(sn / "wibo"), str(sn / "cc1n64.exe"), "-quiet",
                *flags, str(preprocessed), "-o", str(assembly),
            ], project)
            normalized.write_bytes(
                normalize(assembly.read_text()).replace("\n", "\r\n").encode()
            )
            run([
                str(sn / "wibo"), str(sn / "asn64.exe"), "-o",
                str(psyq), str(normalized),
            ], project)
            run([
                str(sn / "psyq-obj-parser"), str(psyq), "-b", "-n",
                "-o", str(obj),
            ], project)

            vram = start + ROM_TO_VRAM
            actual = relocate(Elf(obj.read_bytes()), ".text", {".text": vram}, symbol_map)
            expected = rom_data[start:start + size]
            if actual != expected:
                first = next(
                    (index for index, pair in enumerate(zip(actual, expected))
                     if pair[0] != pair[1]),
                    min(len(actual), len(expected)),
                )
                raise ValueError(
                    f"{source}: mismatch at +0x{first:X}; "
                    f"compiled 0x{len(actual):X}, expected 0x{size:X}"
                )
            checked += 1
            checked_bytes += size

    print(f"verified {checked} normalized SN unit(s), {checked_bytes} exact bytes")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("manifest", type=Path)
    parser.add_argument("--project", type=Path, default=Path.cwd())
    parser.add_argument("--rom", type=Path, required=True)
    parser.add_argument("--symbols", type=Path, required=True)
    args = parser.parse_args()
    project = args.project.resolve()
    verify(
        args.manifest.resolve(), project, args.rom.resolve(), args.symbols.resolve()
    )


if __name__ == "__main__":
    main()
