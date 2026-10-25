"""Canonical interval ledger: every byte of a span in exactly one classified interval."""
from __future__ import annotations

import csv
from dataclasses import dataclass, asdict
from pathlib import Path

# ``referenced``: bytes a loader table or call points at, whose content is not otherwise understood.
CLASSES = ("payload", "container", "padding", "unused", "rom_metadata", "program_image", "code", "data",
           "referenced", "unknown")


@dataclass(frozen=True, order=True)
class Interval:
    start: int
    end: int
    cls: str
    category: str = "-"
    kind: str = "-"
    name: str = "-"
    evidence: str = "-"

    @property
    def size(self) -> int:
        return self.end - self.start


class LedgerError(ValueError):
    pass


class Ledger:
    def __init__(self, start: int, end: int):
        self.start, self.end = start, end
        self.items: list[Interval] = []

    def add(self, start: int, end: int, cls: str, **kw) -> None:
        if cls not in CLASSES:
            raise LedgerError(f"unknown class {cls!r}")
        if not self.start <= start < end <= self.end:
            raise LedgerError(f"interval 0x{start:X}..0x{end:X} outside 0x{self.start:X}..0x{self.end:X}")
        self.items.append(Interval(start, end, cls, **kw))

    def overlaps(self) -> list[tuple[Interval, Interval]]:
        items = sorted(self.items)
        return [(a, b) for a, b in zip(items, items[1:]) if b.start < a.end]

    def complete(self, data: bytes | None = None, fill_name: str = "gap") -> list[Interval]:
        """Sorted intervals with every hole filled: zero/0xFF fill becomes padding, anything else unknown."""
        bad = self.overlaps()
        if bad:
            a, b = bad[0]
            raise LedgerError(f"overlap: {a.name} 0x{a.start:X}..0x{a.end:X} / {b.name} 0x{b.start:X}..0x{b.end:X}")
        out, cur = [], self.start
        for it in sorted(self.items):
            if it.start > cur:
                out.append(self._hole(cur, it.start, data, fill_name))
            out.append(it)
            cur = it.end
        if cur < self.end:
            out.append(self._hole(cur, self.end, data, fill_name))
        return out

    @staticmethod
    def _hole(a: int, b: int, data: bytes | None, name: str) -> Interval:
        if data is not None:
            fill = set(data[a:b])
            if fill <= {0} or fill <= {0xFF}:
                return Interval(a, b, "padding", kind=f"fill_{min(fill) if fill else 0:02X}", name=f"{name}_{a:X}")
        return Interval(a, b, "unknown", name=f"{name}_{a:X}")

    @staticmethod
    def totals(items: list[Interval], key: str = "cls") -> dict[str, int]:
        out: dict[str, int] = {}
        for it in items:
            out[getattr(it, key)] = out.get(getattr(it, key), 0) + it.size
        return out

    @staticmethod
    def coalesce(items: list[Interval], fields=("cls", "category")) -> list[Interval]:
        out: list[Interval] = []
        for it in items:
            if out and out[-1].end == it.start and all(getattr(out[-1], f) == getattr(it, f) for f in fields):
                prev = out[-1]
                out[-1] = Interval(prev.start, it.end, prev.cls, prev.category, prev.kind, prev.name, prev.evidence)
            else:
                out.append(it)
        return out

    @staticmethod
    def write_tsv(items: list[Interval], path: str | Path) -> None:
        with open(path, "w", newline="") as f:
            w = csv.writer(f, delimiter="\t", lineterminator="\n")
            w.writerow(["start", "end", "size", "class", "category", "kind", "name", "evidence"])
            for it in items:
                w.writerow([f"0x{it.start:X}", f"0x{it.end:X}", it.size, it.cls, it.category, it.kind, it.name, it.evidence])

    @staticmethod
    def read_tsv(path: str | Path) -> list[Interval]:
        rows = csv.DictReader(open(path), delimiter="\t")
        return [Interval(int(r["start"], 16), int(r["end"], 16), r["class"], r["category"], r["kind"], r["name"], r["evidence"])
                for r in rows]
