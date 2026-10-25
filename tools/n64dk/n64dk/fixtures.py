"""Locate ROM images and ground truth for tests and tools without ever tracking them.

ROMs: the ``N64DK_ROMS`` environment variable (``name=path`` pairs separated by
``os.pathsep``) overrides ``roms.toml`` in the repository root (ignored by git):

    [roms]
    example_game = "/path/to/baserom.z64"

Ground truth (a matched decompilation project, or a file or directory inside one):

    [truth]
    example_game = "/path/to/project"                       # the live working tree
    pinned_game = {path = "/path/to/project", rev = "8bfdc4b"}  # pinned to a commit

``N64DK_TRUTH_<NAME>`` and ``N64DK_TRUTH_<NAME>_REV`` override the file.  A pinned entry
resolves to a snapshot of the project's tracked files at that commit, extracted once under
``build/truth/`` (read with ``git archive``; the project itself is only read).  The
snapshot links the project's git-ignored files (toolchains, generated headers, build
outputs, base ROMs) to the live ones; untracked files that are not ignored (work in
progress) are left out.  Pinning keeps tests stable while the project is edited.
"""
from __future__ import annotations

import io
import os
import shutil
import subprocess
import tarfile
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SNAPSHOTS = ROOT / "build" / "truth"
SNAPSHOT_FORMAT = 2   # bump when the snapshot layout changes, so cached ones are rebuilt

# Known retail images used as regression fixtures (SHA-1 of the big-endian image).
KNOWN = {
    "mk64": "579c48e211ae952530ffc8738709f078d5dd215e",
    # No public decompilation: used to test generality, never as ground truth.
    "madden64": "b0de34b759f18ad86d39a4c68c9840d35ce25809",
    "nagano98": "eaf543e195731031a84f4136b506449209e3a8f5",
    "wwf_warzone": "8700f89cd7cfa873452117f6b2a648d32796111c",
}


def rom_paths() -> dict[str, Path]:
    out: dict[str, Path] = {}
    config = ROOT / "roms.toml"
    if config.is_file():
        for name, path in tomllib.loads(config.read_text()).get("roms", {}).items():
            out[name] = Path(path)
    for pair in filter(None, os.environ.get("N64DK_ROMS", "").split(os.pathsep)):
        name, _, path = pair.partition("=")
        out[name] = Path(path)
    return {k: v for k, v in out.items() if v.is_file()}


def rom(name: str):
    from .rom import Rom
    path = rom_paths().get(name)
    if path is None:
        return None
    image = Rom.load(path)
    if name in KNOWN and image.sha1 != KNOWN[name]:
        raise ValueError(f"{name}: SHA-1 {image.sha1} does not match the known image")
    return image


def truth_entry(name: str) -> tuple[Path | None, str | None]:
    """The configured (path, revision) of a ground-truth entry; either may be None."""
    path = os.environ.get(f"N64DK_TRUTH_{name.upper()}")
    rev = os.environ.get(f"N64DK_TRUTH_{name.upper()}_REV")
    config = ROOT / "roms.toml"
    if config.is_file():
        value = tomllib.loads(config.read_text()).get("truth", {}).get(name)
        if isinstance(value, dict):
            path = path or value.get("path")
            rev = rev or value.get("rev")
        elif value:
            path = path or value
    return (Path(path) if path else None), (rev or None)


def truth(name: str) -> Path | None:
    """Ground-truth path for ``name`` (a pinned snapshot when a revision is set); None when absent."""
    path, rev = truth_entry(name)
    if path is None or not path.exists():
        return None
    return snapshot(path, rev) if rev else path


def _git(repo: Path, *args: str) -> bytes:
    return subprocess.run(["git", "-C", str(repo), *args], check=True, capture_output=True).stdout


def snapshot(path: Path, rev: str) -> Path:
    """``path`` as of commit ``rev``: the same location inside a read-only extract of the
    repository's tracked files (cached per commit under ``build/truth/``)."""
    path = path.resolve()
    base = path if path.is_dir() else path.parent
    top = Path(_git(base, "rev-parse", "--show-toplevel").decode().strip())
    commit = _git(top, "rev-parse", "--verify", f"{rev}^{{commit}}").decode().strip()
    rel = path.relative_to(top)
    if rel.parts and not _git(top, "ls-tree", "--name-only", commit, "--", rel.as_posix()).strip():
        raise ValueError(f"{path} is not tracked at {rev}; pin only paths inside the repository's history")
    dest = SNAPSHOTS / f"{top.name}-{commit[:12]}"
    marker = f"format {SNAPSHOT_FORMAT}\n{top}\n{commit}\n"
    stamp = dest / ".n64dk-snapshot"
    if not (stamp.is_file() and stamp.read_text() == marker):
        tmp = dest.with_name(dest.name + ".partial")
        shutil.rmtree(tmp, ignore_errors=True)
        tmp.mkdir(parents=True)
        archive = tarfile.open(fileobj=io.BytesIO(_git(top, "archive", commit)))
        kw = {"filter": "data"} if hasattr(tarfile, "data_filter") else {}
        archive.extractall(tmp, **kw)
        # Ignored files are the project's environment (toolchains, generated headers, build
        # outputs, base ROMs), not its history: link them live, at any depth.
        ignored = _git(top, "ls-files", "--others", "--ignored", "--exclude-standard", "--directory", "-z")
        for rel_name in filter(None, ignored.decode().split("\0")):
            rel_name = rel_name.rstrip("/")
            link = tmp / rel_name
            parts = rel_name.split("/")
            # bytecode caches stay local: Python running in the snapshot must not write
            # through a link into the project
            if parts[0] == ".git" or "__pycache__" in parts or rel_name.endswith(".pyc") \
                    or link.exists() or link.is_symlink():
                continue
            link.parent.mkdir(parents=True, exist_ok=True)
            link.symlink_to(top / rel_name)
        (tmp / ".n64dk-snapshot").write_text(marker)
        shutil.rmtree(dest, ignore_errors=True)
        tmp.rename(dest)
    return dest / rel
