"""Compare SDK matches with a project's symbol table (precision / recall)."""
from __future__ import annotations

import re
import tomllib
from pathlib import Path

_FUNC_RE = re.compile(r'name\s*=\s*"([^"]+)"\s*,\s*vram\s*=\s*(0x[0-9A-Fa-f]+)\s*,\s*size\s*=\s*(0x[0-9A-Fa-f]+)')


def load_truth(path: str | Path) -> dict[int, tuple[str, int]]:
    """``vram -> (name, size)`` from a recomp-style ``[[section]] functions = [...]`` TOML file."""
    text = Path(path).read_text()
    try:
        doc = tomllib.loads(text)
        out = {}
        for sec in doc.get("section", []):
            for f in sec.get("functions", []):
                out[int(f["vram"])] = (f["name"], int(f.get("size", 0)))
        if out:
            return out
    except tomllib.TOMLDecodeError:
        pass
    return {int(v, 16): (n, int(s, 16)) for n, v, s in _FUNC_RE.findall(text)}


def _norm(name: str) -> str:
    """Drop an address suffix and leading underscores (``__cosf`` == ``cosf``, ``_bcopy`` == ``bcopy``)."""
    return re.sub(r"_[0-9A-Fa-f]{8}$", "", name).lstrip("_")


def evaluate(matches, truth: dict[int, tuple[str, int]], sdk_names: set[str], min_confidence: float = 0.0) -> dict:
    """Precision over matches whose address carries an SDK name in the truth, recall over
    truth functions whose (normalised) name is in the database."""
    names = {_norm(n) for n in sdk_names}
    found = {m.vram: m for m in matches if m.confidence >= min_confidence}
    correct = wrong = unverifiable = 0
    errors = []
    for v, m in found.items():
        t = truth.get(v)
        if t is None or _norm(t[0]) not in names:
            unverifiable += 1
            continue
        if _norm(t[0]) == _norm(m.name):
            correct += 1
        else:
            wrong += 1
            errors.append((f"0x{v:08X}", m.name, t[0]))
    expected = {v: t for v, t in truth.items() if _norm(t[0]) in names}
    hit = sum(1 for v, t in expected.items() if v in found and _norm(found[v].name) == _norm(t[0]))
    missed = sorted((f"0x{v:08X}", t[0]) for v, t in expected.items() if not (v in found and _norm(found[v].name) == _norm(t[0])))
    return {
        "matches": len(found), "correct": correct, "wrong": wrong, "unverifiable": unverifiable,
        "precision": round(correct / (correct + wrong), 4) if correct + wrong else None,
        "expected": len(expected), "recalled": hit,
        "recall": round(hit / len(expected), 4) if expected else None,
        "errors": errors, "missed": missed,
    }
