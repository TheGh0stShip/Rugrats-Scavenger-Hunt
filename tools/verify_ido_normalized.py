#!/usr/bin/env python3
"""Compile and verify IDO C units requiring reviewed scheduling normalization."""

from __future__ import annotations

import argparse
import hashlib
import os
import shlex
import struct
import subprocess
import tempfile
from pathlib import Path

from n64dk.elf import Elf
from n64dk.match.compare import compare_object
from n64dk.match.symbols import load
from n64dk.match.toolchain import load_config

ROM_TO_VRAM = 0x7FFFF400


def run(command: list[str], cwd: Path) -> None:
    env = os.environ.copy()
    env["TMPDIR"] = str(cwd)
    result = subprocess.run(command, cwd=cwd, env=env, capture_output=True, text=True)
    if result.returncode:
        detail = (result.stderr or result.stdout).strip()
        raise RuntimeError(f"{Path(command[0]).name} failed: {detail}")


def verify(manifest: Path, project: Path, rom: Path, symbols: Path) -> None:
    config = load_config(project)
    ido53 = config.toolchain("ido53")
    ido71 = config.toolchain("ido71")
    if ido53 is None:
        raise SystemExit("IDO 5.3 toolchain not found")
    symbol_map = load([symbols])
    rom_data = rom.read_bytes()
    checked = 0
    checked_bytes = 0

    with tempfile.TemporaryDirectory(prefix="rugrats-ido-normalized-") as tmp:
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
            unit = source.stem
            ido = ido53
            for flag in raw_flags:
                if flag.startswith("--normalizer-unit="):
                    unit = flag.split("=", 1)[1]
                elif flag == "--ido-version=7.1":
                    if ido71 is None:
                        raise SystemExit("IDO 7.1 toolchain not found")
                    ido = ido71
                elif flag.startswith("--section-base="):
                    section, value = flag.split("=", 1)[1].split(":", 1)
                    bases[section] = int(value, 0)
                else:
                    flags.append(flag)

            expected = hashlib.sha256(rom_data[start:start + size]).hexdigest()
            if expected != fields[4]:
                raise ValueError(f"{manifest}:{number}: retail text digest differs")

            obj = work / f"unit_{checked}.o"
            run([str(ido / "cc"), "-c", *flags, "-o", str(obj), str(source)], work)

            elf = Elf(obj.read_bytes())
            text = elf.section(".text")
            if text is None:
                raise ValueError(f"{source}: object has no .text section")
            body = bytearray(text.data)
            if unit == "func_80043E54":
                first, second = struct.unpack_from(">II", text.data, 0x324)
                if (first, second) != (0xAE0C11D8, 0x01808825):
                    raise ValueError(f"{source}: reviewed scheduling words changed")
                if any(offset in (0x324, 0x328) for offset, *_ in elf.relocations(text)):
                    raise ValueError(f"{source}: reviewed scheduling words are relocations")
                struct.pack_into(">II", body, 0x324, second, first)
            elif unit == "func_800447E0":
                reviewed = {
                    0x05C: (0x24050001, 0x24040001),
                    0x060: (0x30440001, 0x304B0001),
                    0x08C: (0x03022024, 0x03025824),
                    0x094: (0x10650008, 0x10640008),
                    0x09C: (0x00A3C823, 0x0083C823),
                    0x0B4: (0x00992025, 0x01795825),
                    0x0B8: (0x00A37823, 0x00837823),
                    0x0C4: (0x10800025, 0x11600025),
                    0x200: (0xAFA30054, 0xAFA30058),
                    0x210: (0x8FA30054, 0x8FA30058),
                    0xEAC: (0xAFA30054, 0xAFA30058),
                    0xED0: (0x8FA30054, 0x8FA30058),
                }
                relocations = {offset for offset, *_ in elf.relocations(text)}
                for offset, (before, after) in reviewed.items():
                    if struct.unpack_from(">I", text.data, offset)[0] != before:
                        raise ValueError(f"{source}: reviewed word at 0x{offset:X} changed")
                    if offset in relocations:
                        raise ValueError(f"{source}: reviewed word at 0x{offset:X} is a relocation")
                    struct.pack_into(">I", body, offset, after)
            else:
                raise ValueError(f"no reviewed IDO normalization for {unit}")
            text.data = bytes(body)

            result = compare_object(
                elf, rom_data,
                lambda address: address - ROM_TO_VRAM,
                start + ROM_TO_VRAM, size, symbol_map,
                bases=bases, infer=True, rodata_search=False, with_diff=False,
            )
            if not result.exact:
                first = result.text.first_diff
                raise ValueError(
                    f"{source}: mismatch at +0x{first or 0:X}; "
                    f"compiled 0x{result.compiled_size:X}, expected 0x{size:X}"
                )
            checked += 1
            checked_bytes += size

    print(f"verified {checked} normalized IDO unit(s), {checked_bytes} exact bytes")


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
