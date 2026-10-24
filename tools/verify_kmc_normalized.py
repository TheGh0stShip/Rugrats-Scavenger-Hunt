#!/usr/bin/env python3
"""Compile and verify KMC C units requiring reviewed assembly normalization."""

from __future__ import annotations

import argparse
import hashlib
import shlex
import subprocess
import tempfile
from pathlib import Path

from n64dk.elf import Elf
from n64dk.match.compare import compare_object
from n64dk.match.symbols import load
from n64dk.match.toolchain import load_config

from normalize_kmc_schedule import normalize


ROM_TO_VRAM = 0x7FFFF400


def run(command: list[str], cwd: Path) -> None:
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True)
    if result.returncode:
        detail = (result.stderr or result.stdout).strip()
        raise RuntimeError(f"{Path(command[0]).name} failed: {detail}")


def verify(manifest: Path, project: Path, rom: Path, symbols: Path) -> None:
    config = load_config(project)
    kmc = config.toolchain("kmc")
    if kmc is None:
        raise SystemExit("KMC GCC 2.7.2 toolchain not found")
    symbol_map = load([symbols])
    rom_data = rom.read_bytes()
    checked = 0
    checked_bytes = 0

    with tempfile.TemporaryDirectory(prefix="rugrats-kmc-normalized-") as tmp:
        work = Path(tmp)
        for number, line in enumerate(manifest.read_text().splitlines(), 1):
            if not line or line.startswith("#"):
                continue
            fields = line.split("\t")
            if len(fields) != 5:
                raise ValueError(f"{manifest}:{number}: expected five fields")
            start, size = int(fields[0], 0), int(fields[1], 0)
            source = project / fields[2]
            raw_flags = shlex.split(fields[3])
            flags: list[str] = []
            bases: dict[str, int] = {}
            extra_symbols: dict[str, int] = {}
            unit = source.stem
            for flag in raw_flags:
                if flag.startswith("--normalizer-unit="):
                    unit = flag.split("=", 1)[1]
                elif flag.startswith("--section-base="):
                    section, value = flag.split("=", 1)[1].split(":", 1)
                    bases[section] = int(value, 0)
                elif flag.startswith("--symbol="):
                    name, value = flag.split("=", 1)[1].split(":", 1)
                    extra_symbols[name] = int(value, 0)
                else:
                    flags.append(flag)

            expected = hashlib.sha256(rom_data[start:start + size]).hexdigest()
            if expected != fields[4]:
                raise ValueError(f"{manifest}:{number}: retail text digest differs")

            stem = work / f"unit_{checked}"
            raw = stem.with_suffix(".raw.s")
            assembly = stem.with_suffix(".s")
            obj = stem.with_suffix(".o")
            run([
                str(kmc / "gcc"), f"-B{kmc}/", "-S", *flags,
                *[f"-I{path}" for path in config.include],
                "-o", str(raw), str(source),
            ], project)
            assembly.write_text(normalize(raw.read_text(errors="replace"), unit))
            run([str(kmc / "as"), "-mips3", "-G0", "-o", str(obj), str(assembly)], project)

            vram = start + ROM_TO_VRAM
            row_symbols = symbol_map.copy()
            row_symbols.update(extra_symbols)
            result = compare_object(
                Elf(obj.read_bytes()), rom_data,
                lambda address: address - ROM_TO_VRAM,
                vram, size, row_symbols, bases=bases,
                infer=True, rodata_search=False, with_diff=False,
            )
            if not result.exact:
                first = result.text.first_diff
                raise ValueError(
                    f"{source}: mismatch at +0x{first or 0:X}; "
                    f"compiled 0x{result.compiled_size:X}, expected 0x{size:X}"
                )
            checked += 1
            checked_bytes += size

    print(f"verified {checked} normalized KMC unit(s), {checked_bytes} exact bytes")


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
