"""Scan a ROM code region for library functions from a signature database."""
from __future__ import annotations

import struct
from collections import Counter, defaultdict
from dataclasses import dataclass, field

from .db import Database
from .signature import KEY_WORDS, canon_keep, digest

MIN_WORDS = 2


@dataclass
class Match:
    rom: int
    vram: int
    size: int
    name: str
    confidence: float
    kind: str                       # exact | variant | call
    alternatives: list[str] = field(default_factory=list)
    sources: list[str] = field(default_factory=list)
    compilers: list[str] = field(default_factory=list)
    calls: list = field(default_factory=list)      # (word index, callee) of the matched signature

    def to_json(self) -> dict:
        return {"rom": f"0x{self.rom:X}", "vram": f"0x{self.vram:08X}", "size": f"0x{self.size:X}",
                "name": self.name, "confidence": round(self.confidence, 3), "kind": self.kind,
                "alternatives": self.alternatives}


@dataclass
class _Hit:
    entry: dict
    exact: bool


def _confidence(words: int, exact: bool) -> float:
    if words >= 12:
        c = 0.99
    elif words >= 8:
        c = 0.85
    elif words >= 5:
        c = 0.7
    elif words >= 3:
        c = 0.5
    else:
        c = 0.3
    return c if exact else c * 0.75


def scan(data: bytes, start: int, end: int, vram_base: int, db: Database,
         min_words: int = MIN_WORDS) -> list[Match]:
    """Find database functions in ``data[start:end]`` (4-byte aligned); ``vram_base`` is the VRAM of ``start``."""
    start -= start % 4
    end = min(end, len(data)) // 4 * 4
    words = struct.unpack_from(f">{(end - start) // 4}I", data, start)
    canon = [w & canon_keep(w) for w in words]
    n = len(words)
    by_key: dict[int, dict[str, list[dict]]] = defaultdict(lambda: defaultdict(list))
    for e in db.entries.values():
        nw = e["size"] // 4
        if nw < min_words:
            continue
        by_key[min(nw, KEY_WORDS)][e["key"]].append(e)
    hits: dict[int, list[_Hit]] = defaultdict(list)
    for klen, table in by_key.items():
        for i in range(n - klen + 1):
            cands = table.get(digest(canon[i:i + klen]))
            if not cands:
                continue
            for e in cands:
                nw = e["size"] // 4
                if i + nw > n:
                    continue
                keep = dict(e["masks"])
                seg = words[i:i + nw]
                if digest([w & keep.get(j, 0xFFFFFFFF) for j, w in enumerate(seg)], nw * 4) == e["exact"]:
                    hits[i].append(_Hit(e, True))
                elif digest([w & keep.get(j, 0xFFFFFFFF) & canon_keep(w) for j, w in enumerate(seg)],
                            nw * 4) == e["canon"]:
                    hits[i].append(_Hit(e, False))
    # Names that occur at many places are generic code (stubs); keep only their best context.
    name_sites = Counter()
    for i, hs in hits.items():
        for name in {h.entry["name"] for h in hs}:
            name_sites[name] += 1

    candidates = []
    for i, hs in hits.items():
        best_exact = any(h.exact for h in hs)
        hs = [h for h in hs if h.exact == best_exact]
        size = max(h.entry["size"] for h in hs)
        hs = [h for h in hs if h.entry["size"] == size]
        names = sorted({h.entry["name"] for h in hs}, key=lambda x: (x.startswith("_"), x))
        conf = _confidence(size // 4, best_exact)
        if name_sites[names[0]] > 1 and size < 48:
            conf *= 0.5
        candidates.append((i, size, names, conf, best_exact, hs))
    # Resolve overlaps: prefer higher confidence then longer.
    candidates.sort(key=lambda c: (-c[3], -c[1], c[0]))
    taken: list[tuple[int, int]] = []
    chosen = []
    for c in candidates:
        a, b = c[0], c[0] + c[1] // 4
        if any(a < tb and ta < b for ta, tb in taken):
            continue
        taken.append((a, b))
        chosen.append(c)
    chosen.sort()
    vram_of = lambda i: vram_base + 4 * i
    named = {vram_of(c[0]): c[2][0] for c in chosen if len(c[2]) == 1}

    def call_score(i, entry) -> int:
        s = 0
        for j, callee in entry["calls"]:
            w = words[i + j]
            target = (vram_of(i + j) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
            got = named.get(target)
            if got is not None:
                s += 1 if got == callee else -1
        return s

    # Callers that matched unambiguously vote for the names of their call targets.
    votes: dict[int, Counter] = defaultdict(Counter)
    for i, size, names, conf, exact, hs in chosen:
        if len(names) == 1 and conf >= 0.8 and exact:
            for j, callee in hs[0].entry["calls"]:
                w = words[i + j]
                votes[(vram_of(i + j) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)][callee] += 1

    out = []
    for i, size, names, conf, exact, hs in chosen:
        alts = []
        voted = [nm for nm, _c in votes.get(vram_of(i), Counter()).most_common() if nm in names]
        if len(names) > 1 and voted and len(set(votes[vram_of(i)]) & set(names)) == 1:
            name = voted[0]
            alts = [x for x in names if x != name]
        elif len(names) > 1:
            scored = sorted(((call_score(i, h.entry), h.entry["name"]) for h in hs), reverse=True)
            best = scored[0]
            ties = sorted({nm for sc, nm in scored if sc == best[0]})
            name = best[1] if len(ties) == 1 else ties[0]
            alts = [x for x in names if x != name]
            conf = conf if len(ties) == 1 and best[0] > 0 else conf / len(ties)
        else:
            name = names[0]
            if any(call_score(i, h.entry) > 0 for h in hs):
                conf = min(0.99, conf + 0.1)
        mine = [h.entry for h in hs if h.entry["name"] == name]
        srcs = sorted({t for h in hs if h.entry["name"] == name for t in h.entry["sources"]})
        comps = sorted({c for h in hs if h.entry["name"] == name for c in h.entry["compilers"]})
        out.append(Match(start + 4 * i, vram_of(i), size, name, conf, "exact" if exact else "variant",
                         alts, srcs, comps, mine[0]["calls"] if exact else []))
    out.extend(_infer_calls(out, words, start, vram_base, db))
    out.sort(key=lambda m: m.rom)
    return out


def _infer_calls(matches: list[Match], words, start: int, vram_base: int, db: Database) -> list[Match]:
    """Name call targets of confidently matched functions from the callee recorded in the signature."""
    known = {m.vram for m in matches}
    votes: dict[int, Counter] = defaultdict(Counter)
    for m in matches:
        if m.confidence < 0.9 or m.kind != "exact":
            continue
        i = (m.rom - start) // 4
        for j, callee in m.calls:
            w = words[i + j]
            target = (m.vram & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
            if target not in known:
                votes[target][callee] += 1
    out = []
    end_vram = vram_base + 4 * len(words)
    for target, c in votes.items():
        if not vram_base <= target < end_vram:
            continue
        name, n = c.most_common(1)[0]
        if len(c) > 1:
            continue
        out.append(Match(start + (target - vram_base), target, 0, name, min(0.9, 0.5 + 0.1 * n), "call"))
    return out


def version_candidates(matches: list[Match], db: Database, prefix: str = "ultralib:",
                       within: float = 0.05) -> list[dict]:
    """Rank library builds by how many matched functions they contain with the matched hash.

    ``support`` counts matched functions whose hash is in the build; ``contradict`` counts
    matched functions whose name is in the build with a different hash.  Builds with
    identical contents are merged (``2.0D/2.0E:libultra_rom``).  ``candidate`` marks builds
    whose score is within ``within`` of the best.
    """
    names_in = defaultdict(set)
    content = defaultdict(set)
    for k, e in db.entries.items():
        for t in e["sources"]:
            if t.startswith(prefix):
                names_in[t].add(e["name"])
                content[t].add(k)
    groups: dict[frozenset, list[str]] = defaultdict(list)
    for t in sorted(content):
        groups[frozenset(content[t])].append(t)
    support, against = Counter(), Counter()
    for m in matches:
        if m.kind != "exact" or m.confidence < 0.8:
            continue
        have = {t for t in m.sources if t.startswith(prefix)}
        for t, names in names_in.items():
            if t in have:
                support[t] += 1
            elif m.name in names:
                against[t] += 1
    rows = []
    for members in groups.values():
        t = members[0]
        if not (support[t] or against[t]):
            continue
        label = _group_label(members, prefix)
        rows.append({"build": label, "support": support[t], "contradict": against[t],
                     "score": support[t] - against[t]})
    rows.sort(key=lambda r: (-r["score"], r["build"]))
    if rows:
        top = rows[0]["score"]
        for r in rows:
            r["candidate"] = top > 0 and r["score"] >= top - max(1, abs(top) * within)
    return rows


def _group_label(tags: list[str], prefix: str) -> str:
    parts = [t[len(prefix):].split(":", 1) for t in tags]
    targets = sorted({p[1] for p in parts if len(p) > 1})
    versions = sorted({p[0] for p in parts})
    return f"{prefix}{'/'.join(versions)}:{'/'.join(targets)}"
