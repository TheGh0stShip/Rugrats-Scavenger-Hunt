"""Symbol maps for relocating compiled objects at their retail addresses.

Sources: splat ``symbol_addrs.txt`` (``name = 0xADDR; // ...``), function catalogues in TOML
(any table carrying ``name`` plus ``vram``/``address``/``addr``, e.g. ``[[section]]`` with a
``functions`` array), ``/* LDSYM name=0xADDR */`` source markers, and ``__asm__("label")``
aliases on declarations.  Names of the form ``func_XXXXXXXX``/``D_XXXXXXXX`` resolve to
their embedded address when no explicit entry exists.
"""
from __future__ import annotations

import fnmatch
import os
import re
import tomllib
from pathlib import Path

INFERRED = re.compile(r"(?:D|func|jtbl|sym|data|B|L)_([0-9A-Fa-f]{6,8})")
SPLAT_LINE = re.compile(r"^\s*([A-Za-z_.$][\w.$]*)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*;(.*)$")
LDSYM = re.compile(r"\bLDSYM\s+([A-Za-z_.$][\w.$]*)\s*=\s*(0x[0-9A-Fa-f]+|\d+)")
ASM_ALIAS = re.compile(r"\b([A-Za-z_]\w*)\s*(?:\[[^\]]*\]\s*)*(?:\([^;{}]*\)\s*)?__asm__\s*\(\s*\"([^\"]+)\"\s*\)")


class SymbolMap(dict):
    """``dict`` of name -> address that also answers inferable ``func_``/``D_`` names."""

    sizes: dict[str, int]

    def __init__(self, *args, **kw):
        super().__init__(*args, **kw)
        self.sizes = {}

    def __contains__(self, name) -> bool:  # used by n64dk.elf.Elf.relocate
        return dict.__contains__(self, name) or bool(INFERRED.fullmatch(str(name)))

    def __missing__(self, name):
        m = INFERRED.fullmatch(str(name))
        if m:
            return int(m.group(1), 16)
        raise KeyError(name)

    def resolve(self, name: str) -> int | None:
        try:
            return self[name]
        except KeyError:
            return None

    def copy(self) -> "SymbolMap":
        out = SymbolMap(self)
        out.sizes = dict(self.sizes)
        return out

    def by_address(self) -> dict[int, str]:
        return {v: k for k, v in self.items()}


def load_splat(text: str, into: SymbolMap) -> None:
    for line in text.splitlines():
        m = SPLAT_LINE.match(line)
        if not m:
            continue
        name, addr = m.group(1), int(m.group(2), 0)
        into[name] = addr
        size = re.search(r"\bsize:(0x[0-9A-Fa-f]+|\d+)", m.group(3))
        if size:
            into.sizes[name] = int(size.group(1), 0)


def load_toml(data, into: SymbolMap) -> None:
    if isinstance(data, dict):
        name = data.get("name")
        addr = next((data[k] for k in ("vram", "address", "addr") if isinstance(data.get(k), int)), None)
        if isinstance(name, str) and addr is not None and not isinstance(data.get("functions"), list):
            into[name] = addr
            if isinstance(data.get("size"), int):
                into.sizes[name] = data["size"]
        for value in data.values():
            if isinstance(value, (dict, list)):
                load_toml(value, into)
    elif isinstance(data, list):
        for value in data:
            load_toml(value, into)


def load(paths, into: SymbolMap | None = None) -> SymbolMap:
    out = into if into is not None else SymbolMap()
    for path in paths:
        path = Path(path)
        text = path.read_text(errors="replace")
        if path.suffix == ".toml":
            load_toml(tomllib.loads(text), out)
        else:
            load_splat(text, out)
    return out


def from_source(text: str, known: SymbolMap) -> dict[str, int]:
    """Extra symbols from ``LDSYM`` markers and ``__asm__`` aliases resolvable via ``known``."""
    extra = {m.group(1): int(m.group(2), 0) for m in LDSYM.finditer(text)}
    for m in ASM_ALIAS.finditer(text):
        addr = extra.get(m.group(2))
        if addr is None:
            addr = known.resolve(m.group(2))
        if addr is not None:
            extra.setdefault(m.group(2), addr)
    return extra


def discover(project: Path) -> list[Path]:
    """Conventional symbol files inside a project (splat symbol_addrs and function catalogues)."""
    found: list[Path] = []
    for pattern in ("symbol_addrs*.txt", "*function_boundaries*.toml", "*symbols*.toml"):
        for root in (project, project / "config"):
            if root.is_dir():
                found += sorted(p for p in _walk(root, pattern)
                                if not {".toolchain", "build"} & set(p.relative_to(project).parts))
    return list(dict.fromkeys(found))


def _walk(root: Path, pattern: str):
    """Files under ``root`` matching ``pattern``, following linked directories (a pinned
    ground-truth snapshot links the project's ignored directories)."""
    for dirpath, dirnames, filenames in os.walk(root, followlinks=True):
        dirnames[:] = [d for d in dirnames if d not in (".git", ".toolchain", "build", "__pycache__")]
        for name in filenames:
            if fnmatch.fnmatch(name, pattern):
                yield Path(dirpath, name)
