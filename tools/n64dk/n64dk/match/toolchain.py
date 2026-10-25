"""Toolchain profiles and project configuration for the matching engine.

A profile names one original compiler (KMC GCC 2.7.2, IDO 5.3, IDO 7.1, SN GCC 2.8) or the
modern GNU binutils used by the ASN64-compatible assembler lane, together with its standard
flags.  Paths come from ``n64dk.toml``::

    [toolchains]
    root = ".toolchain"                 # directory holding the default layout below
    kmc = ".toolchain/kmc-gcc-2.7.2"    # optional per-toolchain overrides
    ido53 = "..."; ido71 = "..."; sn = "..."; binutils = "..."

    [match]
    include = ["include"]
    symbols = ["config/symbol_addrs.txt"]
    normalizer = "python3 tools/normalize.py {input} {output}"
    vram_delta = 0x80070000             # optional; otherwise derived from the ROM header
    rom = "baserom.z64"                 # optional; otherwise baserom*.z64 in the project

Relative paths resolve against the directory holding the configuration file.
"""
from __future__ import annotations

import os
import shutil
import tomllib
from dataclasses import dataclass, field
from pathlib import Path

# Default directory names inside a toolchain root.
LAYOUT = {
    "kmc": "kmc-gcc-2.7.2",
    "ido53": "ido5.3",
    "ido71": "ido7.1",
    "sn": "sn-gcc-2.8",
    "binutils": "mips-binutils",
}


@dataclass(frozen=True)
class Profile:
    key: str            # configuration key ("kmc", "ido53", ...)
    name: str           # display name
    family: str         # "gcc", "ido" or "sn"
    default_flags: tuple[str, ...]
    required: tuple[str, ...]  # files that must exist inside the toolchain directory


PROFILES: dict[str, Profile] = {
    "kmc": Profile("kmc", "KMC GCC 2.7.2", "gcc", ("-O2", "-G0", "-mips3", "-mgp32", "-mfp32"),
                   ("gcc", "cc1", "as")),
    "ido53": Profile("ido53", "IDO 5.3", "ido", ("-O2", "-mips2", "-non_shared", "-G", "0"), ("cc",)),
    "ido71": Profile("ido71", "IDO 7.1", "ido", ("-O2", "-mips2", "-non_shared", "-G", "0"), ("cc",)),
    "sn": Profile("sn", "SN GCC 2.8", "sn",
                  ("-O2", "-G0", "-mcpu=vr4300", "-mips3", "-mhard-float", "-meb"),
                  ("wibo", "cc1n64.exe", "asn64.exe", "psyq-obj-parser")),
}


@dataclass
class Config:
    """Resolved project configuration (paths absolute)."""
    project: Path
    toolchain_root: Path | None = None
    toolchains: dict[str, Path] = field(default_factory=dict)
    include: list[Path] = field(default_factory=list)
    symbols: list[Path] = field(default_factory=list)
    normalizer: str | None = None
    vram_delta: int | None = None
    rom: Path | None = None

    def find_rom(self) -> Path | None:
        """Configured ROM, else a conventional ``baserom*.z64`` inside the project."""
        if self.rom is not None:
            return self.rom
        for pattern in ("baserom*.z64", "baseroms/*/baserom*.z64", "baseroms/baserom*.z64", "*.z64"):
            hits = sorted(self.project.glob(pattern))
            if hits:
                return hits[0]
        return None

    def toolchain(self, key: str) -> Path | None:
        """Directory of toolchain ``key`` or None when it is not installed."""
        path = self.toolchains.get(key)
        if path is None and self.toolchain_root is not None:
            path = self.toolchain_root / LAYOUT[key]
        if path is None or not path.is_dir():
            return None
        profile = PROFILES.get(key)
        if profile and not all((path / f).is_file() for f in profile.required):
            return None
        return path

    def available(self) -> list[str]:
        return [k for k in PROFILES if self.toolchain(k) is not None]

    def gnu_as(self) -> tuple[str, dict[str, str]] | None:
        """Modern MIPS GNU as (path, extra environment) for the ASN64 lane."""
        root = self.toolchains.get("binutils")
        if root is None and self.toolchain_root is not None:
            root = self.toolchain_root / LAYOUT["binutils"]
        if root is not None:
            for prefix in (root / "usr", root):
                exe = prefix / "bin" / "mips-linux-gnu-as"
                if exe.is_file():
                    env = {}
                    lib = prefix / "lib" / "x86_64-linux-gnu"
                    if lib.is_dir():
                        env["LD_LIBRARY_PATH"] = str(lib)
                    return str(exe), env
        for name in ("mips-linux-gnu-as", "mips64-elf-as", "mips-elf-as"):
            exe = shutil.which(name)
            if exe:
                return exe, {}
        return None


def _resolve(base: Path, value: str | os.PathLike) -> Path:
    path = Path(value).expanduser()
    return path if path.is_absolute() else (base / path).resolve()


def load_config(project: str | Path | None = None, config: str | Path | None = None,
                toolchain_dir: str | Path | None = None) -> Config:
    """Build a :class:`Config` from ``n64dk.toml`` (explicit path, the project directory or
    the current directory), the ``N64DK_TOOLCHAIN`` environment variable and the
    ``<project>/.toolchain`` convention, in that order of precedence after explicit flags."""
    project_dir = Path(project).resolve() if project else Path.cwd()
    cfg = Config(project=project_dir)
    candidates = [Path(config)] if config else [project_dir / "n64dk.toml", Path.cwd() / "n64dk.toml"]
    for path in candidates:
        if path.is_file():
            base = path.resolve().parent
            data = tomllib.loads(path.read_text())
            tc = data.get("toolchains", {})
            if "root" in tc:
                cfg.toolchain_root = _resolve(base, tc["root"])
            for key in LAYOUT:
                if key in tc:
                    cfg.toolchains[key] = _resolve(base, tc[key])
            m = data.get("match", {})
            cfg.include = [_resolve(base, p) for p in m.get("include", [])]
            cfg.symbols = [_resolve(base, p) for p in m.get("symbols", [])]
            cfg.normalizer = m.get("normalizer")
            cfg.vram_delta = m.get("vram_delta")
            if "rom" in m:
                cfg.rom = _resolve(base, m["rom"])
            if "project" in m:
                cfg.project = _resolve(base, m["project"])
            break
    if toolchain_dir:
        cfg.toolchain_root = Path(toolchain_dir).resolve()
    elif cfg.toolchain_root is None:
        env = os.environ.get("N64DK_TOOLCHAIN")
        for cand in ([Path(env)] if env else []) + [cfg.project / ".toolchain", Path.cwd() / ".toolchain"]:
            if cand.is_dir():
                cfg.toolchain_root = cand.resolve()
                break
    if not cfg.include and (cfg.project / "include").is_dir():
        cfg.include = [cfg.project / "include"]
    return cfg
