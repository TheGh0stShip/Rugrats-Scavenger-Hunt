#!/usr/bin/env python3
"""Compose the full ROM ledger 0x000000..0x1000000 from every area's roundtrip(rom) rows.

Top-level (partition) sources: tools/platform.py, tools/archive_rt.py, tools/audio.py,
tools/datasrc.py, and program text rows proven by `make verify-matching` (verification/code_units.json).  Content-level
sources (tools/mort.py, tools/res2.py) describe structures nested inside top-level rows and are
written to inventory/nested_ledger.tsv; they are checked for containment, not for partitioning.

Missing or failing modules do not abort: their expected ranges are emitted as 'unresolved'.
Outputs: inventory/full_rom_ledger.tsv, inventory/nested_ledger.tsv, verification/gaps.tsv,
verification/overlaps.tsv, verification/unknowns.tsv.
"""
import argparse
import hashlib
import json
import importlib
import importlib.util
import os
import sys
import traceback

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)

ROM_DEFAULT = os.environ.get("ROM", "baseroms/us/baserom.z64")
ROM_SHA1 = "fb2a62f1625630d6f0beb5fd00a32e12155d50e8"
PROJECT = os.environ.get("PROJECT_ROOT", ".")
MANIFESTS = ["matching_units.tsv", "matching_kmc_as_units.tsv", "matching_kmc_normalized_units.tsv",
             "matching_sn_units.tsv", "matching_sn_normalized_units.tsv", "matching_ido_units.tsv"]
ROM_SIZE = 0x1000000

TEXT_SECTIONS = [(0x1000, 0x103C), (0x45F0, 0x53474)]
# text found inside the linker data ranges (assembled exactly by the data module)
EMBEDDED_TEXT = [(0x103C, 0x10F8), (0x53474, 0x535A0)]
CLONE = os.path.join(ROOT, "private_extracted", "verify_clone")
VERIFY_LOG = os.path.join(ROOT, "verification", "verify_matching.log")
CODE_CACHE = os.path.join(ROOT, "verification", "code_units.json")
ROM_TO_VRAM = 0x7FFFF400
DATA_SECTIONS = [(0x103C, 0x45E0, ".rodata"), (0x45E0, 0x45F0, ".data"), (0x53474, 0x57E80, ".data")]

# module name -> (layer, expected top-level ranges if missing)
MODULES = {
    "platform": (0, [(0x0, 0x1000), (0x57E80, 0x57FD0), (0xF4D840, 0x1000000)]),
    "archive_rt": (0, [(0x21AC00, 0xF4D840)]),
    "audio": (0, [(0x57FD0, 0x21AC00)]),
    "datasrc": (0, [(a, b) for a, b, _ in DATA_SECTIONS]),
    "mort": (1, []),
    "res2": (1, []),
}

COLS = ["start", "end", "size", "category", "subtype", "owner", "format", "extraction", "rebuild",
        "verification", "evidence"]


def _clean(v) -> str:
    return str(v).replace("\t", " ").replace("\n", " ").rstrip()


def load_module(name: str, rom: bytes):
    path = os.path.join(HERE, name + ".py")
    if not os.path.exists(path):
        return None, "module not present"
    try:
        spec = importlib.util.spec_from_file_location(f"rom_tools_{name}", path)
        mod = importlib.util.module_from_spec(spec)
        sys.modules[spec.name] = mod  # dataclasses and similar need the module registered
        spec.loader.exec_module(mod)
        rows = mod.roundtrip(rom)
        return [dict(r, owner=r.get("owner") or f"tools/{name}.py") for r in rows], None
    except Exception as e:  # tolerate broken worker modules
        return None, f"{type(e).__name__}: {e} @ {traceback.format_exc().splitlines()[-2].strip()[:120]}"


def parse_verify_log(log_text: str, clone: str) -> dict:
    """Units proven by `make verify-matching` in the pinned clone.

    match-batch prints one 'MATCH [tool] src: VRAM (SIZE bytes)' line per unit.  The SN
    normalized verifier prints only a total and raises on the first mismatch, so a completed run
    proves every entry of that manifest; the summed sizes are checked against its printed total.
    """
    import re
    units = []
    for m in re.finditer(r"^MATCH \[([^\]]+)\] (\S+): 0x([0-9A-Fa-f]+) \(0x([0-9A-Fa-f]+) bytes\)", log_text, re.M):
        va, n = int(m.group(3), 16), int(m.group(4), 16)
        units.append({"rom_start": va - ROM_TO_VRAM, "size": n, "source": m.group(2), "tool": m.group(1),
                      "evidence": "MATCH line"})
    summarized_manifests = (
        (r"verified (\d+) normalized KMC unit\(s\), (\d+) exact bytes",
         "matching_kmc_normalized_units.tsv", "kmc+normalized"),
        (r"verified (\d+) padded KMC unit\(s\), (\d+) exact bytes",
         "matching_kmc_padded_units.tsv", "kmc+padded"),
        (r"verified (\d+) normalized SN unit\(s\), (\d+) exact bytes",
         "matching_sn_normalized_units.tsv", "sn+normalized"),
    )
    for pattern, filename, tool in summarized_manifests:
        t = re.search(pattern, log_text)
        if not t:
            continue
        man = os.path.join(clone, "config/us", filename)
        ents = [l.rstrip("\n").split("\t") for l in open(man) if l.strip() and not l.startswith("#")]
        if len(ents) == int(t.group(1)) and sum(int(e[1], 16) for e in ents) == int(t.group(2)):
            for e in ents:
                units.append({"rom_start": int(e[0], 16), "size": int(e[1], 16), "source": e[2],
                              "tool": tool, "evidence": f"{tool} verifier completed run"})
    head = ""
    try:
        import subprocess
        head = subprocess.run(["git", "-C", clone, "rev-parse", "HEAD"], capture_output=True, text=True).stdout.strip()
    except Exception:
        pass
    return {"clone_head": head, "command": "make verify-matching N64DK_ROOT=$N64DK_ROOT "
            "N64DK_TOOLCHAIN=$N64DK_TOOLCHAIN",
            "summaries": re.findall(r"^(Verified .*|verified \d+ normalized.*)$", log_text, re.M), "units": units}


def manifest_units(rom: bytes) -> list[dict]:
    """Code units proven by verify-matching (cached in verification/code_units.json)."""
    log_is_newer = (
        os.path.exists(VERIFY_LOG)
        and (
            not os.path.exists(CODE_CACHE)
            or os.path.getmtime(VERIFY_LOG) > os.path.getmtime(CODE_CACHE)
        )
    )
    if log_is_newer:
        data = parse_verify_log(open(VERIFY_LOG).read(), PROJECT)
        json.dump(data, open(CODE_CACHE, "w"), indent=1)
    elif os.path.exists(CODE_CACHE):
        data = json.load(open(CODE_CACHE))
    else:
        return []
    units = {}
    for u in data["units"]:
        key = (u["rom_start"], u["rom_start"] + u["size"])
        if key in units:
            continue
        units[key] = {"start": key[0], "end": key[1], "category": "code", "subtype": "matched_unit",
                      "owner": f"verify-matching [{u['tool']}]", "format": "MIPS text", "extraction": u["source"],
                      "rebuild": f"compile {u['source']} ({u['tool']})", "verified": True,
                      "evidence": f"{u['evidence']} @ clone {data['clone_head'][:12]}"}
    return sorted(units.values(), key=lambda r: r["start"])


def text_rows(rom: bytes, occupied: list[tuple[int, int]]) -> list[dict]:
    """Manifest units inside text sections, remainder as unmatched code; skip ranges owned elsewhere."""
    rows = []
    units = [u for u in manifest_units(rom) if any(a <= u["start"] and u["end"] <= b for a, b in TEXT_SECTIONS)]
    # drop nested/overlapping manifest units (keep the first, report the rest via overlap check)
    kept, cur = [], -1
    for u in units:
        if u["start"] >= cur:
            kept.append(u)
            cur = u["end"]
    for a, b in TEXT_SECTIONS:
        pos = a
        for u in [u for u in kept if a <= u["start"] < b] + [None]:
            nxt = u["start"] if u else b
            if nxt > pos:
                rows.extend(_split_free(pos, nxt, occupied, {
                    "category": "code", "subtype": "code (not in matching manifests)",
                    "owner": "program image", "format": "MIPS text", "extraction": "byte range",
                    "rebuild": "none (no matching source)", "verified": False,
                    "evidence": "no matching manifest unit covers this range"}))
            if u:
                rows.append(u)
                pos = u["end"]
    return rows


def _split_free(a, b, occupied, tmpl):
    out, pos = [], a
    for oa, ob in sorted(occupied):
        if ob <= pos or oa >= b:
            continue
        if oa > pos:
            out.append(dict(tmpl, start=pos, end=oa))
        pos = max(pos, ob)
    if pos < b:
        out.append(dict(tmpl, start=pos, end=b))
    return out


def compose(rom: bytes):
    status, top, nested, missing = {}, [], [], []
    for name, (layer, expected) in MODULES.items():
        rows, err = load_module(name, rom)
        status[name] = err or f"{len(rows)} rows"
        if rows is None:
            missing += [(a, b, name, err) for a, b in expected]
            continue
        (top if layer == 0 else nested).extend(rows)
    for r in top:
        if any(a <= r["start"] and r["end"] <= b for a, b in EMBEDDED_TEXT) and r["category"] == "program_code":
            r["category"], r["subtype"] = "code", "text in data range (" + str(r.get("subtype", "")) + ")"
    occupied = [(r["start"], r["end"]) for r in top if r["category"] != "alias"]
    top += text_rows(rom, occupied)
    occupied = [(r["start"], r["end"]) for r in top if r["category"] != "alias"]
    for a, b, name, err in missing:
        for r in _split_free(a, b, occupied, {
                "category": "unresolved", "subtype": f"awaiting tools/{name}.py", "owner": f"tools/{name}.py",
                "format": "", "extraction": "", "rebuild": "", "verified": False, "evidence": err}):
            top.append(r)
    occupied = [(r["start"], r["end"]) for r in top if r["category"] != "alias"]
    for a, b in EMBEDDED_TEXT:  # text inside the old data ranges, when no assembly evidence claims it
        top += _split_free(a, b, occupied, {
            "category": "code", "subtype": "text in data range (not in matching manifests)", "owner": "program image",
            "format": "MIPS text", "extraction": "byte range", "rebuild": "none (no matching source)",
            "verified": False, "evidence": "hand-written code found inside the section-list data range"})
    occupied = [(r["start"], r["end"]) for r in top if r["category"] != "alias"]
    for a, b, sec in DATA_SECTIONS:  # program data not claimed by anyone
        top += _split_free(a, b, occupied, {
            "category": "program_data", "subtype": sec, "owner": "program image", "format": "",
            "extraction": "", "rebuild": "", "verified": False, "evidence": "not claimed by a data module"})
    part = sorted([r for r in top if r["category"] != "alias"], key=lambda r: (r["start"], r["end"]))
    aliases = [r for r in top if r["category"] == "alias"]

    gaps, overlaps, pos = [], [], 0
    prev = None
    for r in part:
        if r["start"] > pos:
            gaps.append((pos, r["start"]))
        elif r["start"] < pos and prev is not None:
            overlaps.append((prev, r))
        if r["end"] > pos:
            pos, prev = r["end"], r
    if pos < ROM_SIZE:
        gaps.append((pos, ROM_SIZE))
    for g in gaps:
        part.append({"start": g[0], "end": g[1], "category": "unknown", "subtype": "gap", "owner": "",
                     "format": "", "extraction": "", "rebuild": "", "verified": False,
                     "evidence": f"distinct bytes {sorted(set(rom[g[0]:g[1]]))[:6]}"})
    part.sort(key=lambda r: (r["start"], r["end"]))
    # nested containment
    for r in nested:
        host = [t for t in part if t["start"] <= r["start"] and r["end"] <= t["end"]]
        r["evidence"] = str(r.get("evidence", "")) + ("" if host else " [NOT CONTAINED IN A TOP-LEVEL ROW]")
    return part, aliases, nested, gaps, overlaps, status


def _write(path, rows):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as fh:
        fh.write("\t".join(COLS) + "\n")
        for r in rows:
            v = "exact" if r.get("verified") else ("unresolved" if r["category"] in ("unresolved", "unknown") else "not verified")
            fh.write("\t".join([f"0x{r['start']:06X}", f"0x{r['end']:06X}", f"0x{r['end'] - r['start']:X}"]
                               + [_clean(r.get(c, "")) for c in COLS[3:9]] + [v, _clean(r.get("evidence", ""))]) + "\n")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("rom", nargs="?", default=ROM_DEFAULT)
    a = ap.parse_args()
    rom = open(a.rom, "rb").read()
    if hashlib.sha1(rom).hexdigest() != ROM_SHA1 or len(rom) != ROM_SIZE:
        print("unexpected ROM")
        return 2
    part, aliases, nested, gaps, overlaps, status = compose(rom)
    _write(os.path.join(ROOT, "inventory/full_rom_ledger.tsv"), part)
    _write(os.path.join(ROOT, "inventory/nested_ledger.tsv"), nested + aliases)
    with open(os.path.join(ROOT, "verification/gaps.tsv"), "w") as fh:
        fh.write("start\tend\tsize\n")
        for g in gaps:
            fh.write(f"0x{g[0]:06X}\t0x{g[1]:06X}\t0x{g[1] - g[0]:X}\n")
    with open(os.path.join(ROOT, "verification/overlaps.tsv"), "w") as fh:
        fh.write("a_start\ta_end\ta_owner\ta_subtype\tb_start\tb_end\tb_owner\tb_subtype\n")
        for x, y in overlaps:
            fh.write(f"0x{x['start']:06X}\t0x{x['end']:06X}\t{x['owner']}\t{_clean(x['subtype'])}\t"
                     f"0x{y['start']:06X}\t0x{y['end']:06X}\t{y['owner']}\t{_clean(y['subtype'])}\n")
    _write(os.path.join(ROOT, "verification/unknowns.tsv"), [r for r in part if not r.get("verified")])
    exact = sum(r["end"] - r["start"] for r in part if r.get("verified"))
    by = {}
    for r in part:
        k = (r["category"], bool(r.get("verified")))
        by[k] = by.get(k, 0) + r["end"] - r["start"]
    for name, s in status.items():
        print(f"module {name:10} {s}")
    for (c, v), n in sorted(by.items()):
        pass
    code_proven = sum(r["end"] - r["start"] for r in part if r["subtype"] == "matched_unit" and r.get("verified"))
    print(f"code bytes proven by verify-matching {code_proven:#x} ({code_proven})")
    for (c, v), n in sorted(by.items()):
        print(f"  {c:18} {'exact' if v else 'open ':5} {n:#10x} ({n})")
    print(f"rows {len(part)} gaps {len(gaps)} overlaps {len(overlaps)} exact bytes {exact:#x} / {ROM_SIZE:#x} "
          f"({100 * exact / ROM_SIZE:.3f}%)")
    return 0 if not gaps and not overlaps else 1


if __name__ == "__main__":
    sys.exit(main())
