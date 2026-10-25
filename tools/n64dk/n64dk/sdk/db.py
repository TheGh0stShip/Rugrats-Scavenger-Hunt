"""Signature database: (name, masked hash) entries tagged with the library builds that contain them.

Stored as gzip-compressed JSON.  Each entry holds only hashes, sizes, mask
positions and callee names; no instruction bytes.
"""
from __future__ import annotations

import gzip
import json
import re
from pathlib import Path

from .fingerprint import CompilerModel
from .signature import Signature

DEFAULT_PATH = Path(__file__).resolve().parent / "data" / "signatures.json.gz"
FORMAT = 1
_PLACEHOLDER = re.compile(r"^(func|sub|fn|code|D|jtbl|L)_[0-9A-Fa-f]{6,8}$")


class Database:
    def __init__(self):
        self.entries: dict[tuple[str, str], dict] = {}
        self.model = CompilerModel()

    def add(self, sigs: list[Signature], tag: str, compiler: str) -> None:
        for s in sigs:
            if s.features is not None and compiler in ("ido", "kmc", "sn") and s.obj.endswith(".c") and s.size >= 24:
                self.model.add(compiler, s.features)
            if _PLACEHOLDER.match(s.name) or s.size < 8:
                continue
            key = (s.name, s.exact)
            e = self.entries.get(key)
            if e is None:
                e = self.entries[key] = {
                    "name": s.name, "size": s.size, "exact": s.exact, "canon": s.canon, "key": s.key,
                    "prefix": s.prefix, "masks": [list(m) for m in s.masks], "calls": [list(c) for c in s.calls],
                    "local": s.local, "obj": s.obj, "sources": [], "compilers": []}
            if tag not in e["sources"]:
                e["sources"].append(tag)
            if compiler not in e["compilers"]:
                e["compilers"].append(compiler)

    def __len__(self):
        return len(self.entries)

    def without_sources(self, prefixes) -> "Database":
        """A view excluding entries found only in sources whose tag starts with one of ``prefixes``."""
        prefixes = tuple(prefixes)
        out = Database()
        out.model = self.model
        for k, e in self.entries.items():
            keep = [t for t in e["sources"] if not t.startswith(prefixes)]
            if keep:
                out.entries[k] = dict(e, sources=keep)
        return out

    def names(self) -> set[str]:
        return {e["name"] for e in self.entries.values()}

    def versions(self) -> list[str]:
        return sorted({t for e in self.entries.values() for t in e["sources"]})

    def save(self, path: str | Path = DEFAULT_PATH) -> None:
        path = Path(path)
        path.parent.mkdir(parents=True, exist_ok=True)
        entries = sorted(self.entries.values(), key=lambda e: (e["name"], e["exact"]))
        for e in entries:
            e["sources"].sort()
            e["compilers"].sort()
        doc = {"format": FORMAT, "model": self.model.to_json(), "entries": entries}
        raw = json.dumps(doc, separators=(",", ":"), sort_keys=True).encode()
        with open(path, "wb") as fh:
            fh.write(gzip.compress(raw, mtime=0))

    @classmethod
    def load(cls, path: str | Path | None = None) -> "Database":
        path = Path(path or DEFAULT_PATH)
        raw = path.read_bytes()
        if raw[:2] == b"\x1f\x8b":
            raw = gzip.decompress(raw)
        doc = json.loads(raw)
        if doc.get("format") != FORMAT:
            raise ValueError(f"{path}: unsupported signature database format {doc.get('format')}")
        db = cls()
        db.model = CompilerModel.from_json(doc.get("model"))
        for e in doc["entries"]:
            e["masks"] = [tuple(m) for m in e["masks"]]
            e["calls"] = [tuple(c) for c in e["calls"]]
            db.entries[(e["name"], e["exact"])] = e
        return db
