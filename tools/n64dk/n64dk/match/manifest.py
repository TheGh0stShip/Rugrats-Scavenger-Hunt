"""Matching-unit manifests.

Two tab-separated layouts are recognised per row:

* source rows ``ROM_START  SIZE  SOURCE  FLAGS  [SHA256_TEXT]`` (assembly rows may omit FLAGS);
  the pipeline comes from the header comment (``IDO``, ``SN``, ``KMC``/``GCC``) unless given;
* unit rows ``UNIT  VRAM  SIZE  [STATUS]`` where the source path is derived from a template
  (default: ``{unit}``, ``src/{unit}.c``, ``src/code/{unit}.c`` -- first that exists) and the
  flags are the toolchain defaults or ``flags``; ``status_flags`` maps a status value to extra
  compiler flags.

An optional section map (``UNIT_OR_SOURCE  VRAM  SIZE``) fixes the ``.rodata`` placement of
units, as a project's own ownership table does.
"""
from __future__ import annotations

import re
import shlex
from dataclasses import dataclass, field
from pathlib import Path

from .pipeline import BuildSpec, split_flags
from .toolchain import PROFILES

HEX = re.compile(r"^(0x)?[0-9A-Fa-f]+$")
DIGEST = re.compile(r"^[0-9a-f]{64}$")


@dataclass
class Unit:
    name: str
    source: Path
    vram: int
    size: int
    spec: BuildSpec
    digest: str | None = None
    section_sizes: dict[str, int] = field(default_factory=dict)
    line: int = 0


def header_pipeline(text: str) -> str | None:
    for line in text.splitlines():
        if not line.startswith("#"):
            break
        up = line.upper()
        if "IDO" in up:
            return "ido7.1" if "7.1" in up else "ido5.3"
        if "SN_" in up or "SN GCC" in up:
            return "sn"
        if "KMC" in up or "GCC" in up:
            return "kmc"
    return None


def default_flags(pipeline: str) -> list[str]:
    key = {"ido5.3": "ido53", "ido7.1": "ido71", "sn": "sn"}.get(pipeline, "kmc")
    return list(PROFILES[key].default_flags)


def load_section_map(path: Path) -> dict[str, tuple[int, int]]:
    out = {}
    for line in Path(path).read_text().splitlines():
        if not line.strip() or line.startswith("#"):
            continue
        parts = line.split("\t") if "\t" in line else line.split()
        if len(parts) >= 3 and HEX.match(parts[1]) and HEX.match(parts[2]):
            out[parts[0]] = (int(parts[1], 16), int(parts[2], 16))
    return out


def parse(path: Path, project: Path, rom_to_vram, pipeline: str | None = None,
          kmc_lane: str = "kmc+builtin", source_template: str | None = None,
          flags: list[str] | None = None, status_flags: dict[str, list[str]] | None = None,
          rodata_map: dict[str, tuple[int, int]] | None = None) -> list[Unit]:
    text = Path(path).read_text()
    pipe = pipeline or header_pipeline(text) or "kmc"
    units: list[Unit] = []
    for lineno, line in enumerate(text.splitlines(), 1):
        if not line.strip() or line.startswith("#"):
            continue
        cols = line.split("\t")
        if len(cols) < 3:
            cols = line.split()
        if HEX.match(cols[0]) and not HEX.match(cols[2]):
            start, size, src = int(cols[0], 16), int(cols[1], 16), cols[2]
            rest = cols[3:]
            digest = rest[-1] if rest and DIGEST.match(rest[-1]) else None
            flag_text = rest[0] if rest and not DIGEST.match(rest[0]) else ""
            source = project / src
            row_pipe = pipe
            if source.suffix in (".s", ".S") and not flag_text:
                row_pipe = "as"
            spec = split_flags(shlex.split(flag_text), "kmc" if row_pipe == "kmc" else row_pipe)
            if spec.pipeline == "kmc":
                spec.pipeline = kmc_lane
            units.append(Unit(src, source, rom_to_vram(start), size, spec, digest, line=lineno))
            continue
        if not (HEX.match(cols[1]) and HEX.match(cols[2])):
            continue  # column header
        name, vram, size = cols[0], int(cols[1], 16), int(cols[2], 16)
        status = cols[3] if len(cols) > 3 else ""
        candidates = [source_template.format(unit=name)] if source_template else \
            [name, f"src/{name}.c", f"src/code/{name}.c"]
        source = next((project / c for c in candidates if (project / c).is_file()), project / candidates[-1])
        row_flags = list(flags) if flags is not None else default_flags(pipe)
        row_flags += (status_flags or {}).get(status, [])
        spec = split_flags(row_flags, pipe)
        if spec.pipeline == "kmc":
            spec.pipeline = kmc_lane
        unit = Unit(name, source, vram, size, spec, line=lineno)
        if rodata_map:
            hit = rodata_map.get(name) or rodata_map.get(str(source.relative_to(project)) if source.is_relative_to(project) else "")
            if hit:
                spec.section_bases.setdefault(".rodata", hit[0])
                unit.section_sizes[".rodata"] = hit[1]
        units.append(unit)
    return units
