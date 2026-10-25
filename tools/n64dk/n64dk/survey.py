"""One-shot ROM survey: every analysis over the program image, one ROM interval ledger.

``run(rom)`` builds the program image (``n64dk.image``), runs function discovery, the
data map and SDK identification on its main segment, scans audio, graphics (display-list
pools and lists), compressed streams and loader-referenced ranges over the whole ROM, and
composes one ledger in which every ROM byte has a class.  Intervals are placed in priority
order (ROM metadata, the program and its loader, audio, compressed streams, graphics,
loader ranges); a lower-priority interval that
overlaps a placed one is dropped and counted, except loader ranges (reference evidence
only) and data-map sections (around RSP code blocks), which are clipped to the bytes
nothing else claims.  Bytes nothing claims become padding (zero
or 0xFF fill) or ``unknown``; ``complete`` is true only when no byte is unknown.
"""
from __future__ import annotations

import time
import traceback
from collections import Counter

from . import image as imagemod
from .ledger import Interval, Ledger
from .rom import Rom


def run(rom: Rom, *, mode: str = "auto", lzari_brute: bool = True) -> dict:
    timings: dict[str, float] = {}
    errors: dict[str, str] = {}
    out: dict = {"rom": rom.summary(), "timings": timings, "errors": errors}

    def stage(name, fn):
        t = time.time()
        try:
            return fn()
        except Exception as exc:  # one failing analysis must not hide the others
            errors[name] = f"{type(exc).__name__}: {exc}"
            errors[name + "_trace"] = traceback.format_exc(limit=4)
            return None
        finally:
            timings[name] = round(time.time() - t, 2)

    img = stage("program_image", lambda: imagemod.build(rom, mode))
    if img is None:
        img = imagemod.build(rom, "static")
    out["program"] = img.summary()
    main = img.main()
    packed = main.kind == "packed"

    from .analysis import datamap, functions
    cat = stage("functions", lambda: functions.discover(rom, program=img))
    dm = stage("datamap", lambda: datamap.analyse(rom, program=img))
    from . import sdk
    rep = stage("sdk", lambda: sdk.analyse(rom, program=img))
    from .assets import audio
    au = stage("audio", lambda: audio.scan(rom.data))
    gfx = stage("gfx", lambda: scan_graphics(rom.data, rep.microcode if rep else []))
    from .commands.assets_scan import scan_rom
    assets = stage("assets", lambda: scan_rom(rom, lzari_brute=lzari_brute, program=img))

    out["functions"] = functions.summary(cat) if cat else None
    out["datamap"] = datamap.summary(dm) if dm else None
    out["sdk"] = rep.summary() if rep else None
    if au is not None:
        inv = au.inventory()
        out["audio"] = {"counts": inv.get("counts"), "bytes": inv.get("bytes")}
    if gfx is not None:
        out["gfx"] = gfx["summary"]
    if assets is not None:
        from .commands.assets_scan import _summary
        out["assets"] = _summary(assets)
    game = stage("game_codec", lambda: try_game_codec(rom, img, assets)) if assets and main.game_codec else None
    if game is not None:
        out["game_codec"] = game["summary"]
        assets["ledger"] = assets["ledger"] + game["intervals"]
    out["offsets"] = ("segment offsets into the unpacked main segment" if packed else "ROM offsets")
    rsp = stage("rsp_in_text", lambda: rsp_in_main(rom, img))
    out["rsp_in_text"] = [f"0x{a:08X}..0x{z:08X}" for a, z in rsp or []]
    out["cross_checks"] = _cross_checks(img, cat, dm, rsp or [])

    items, dropped = _ledger(rom, img, dm, au, gfx, assets, rsp or [])
    out["ledger_totals"] = Ledger.totals(items)
    out["ledger_categories"] = Ledger.totals(items, "category")
    unknown = [it for it in items if it.cls == "unknown"]
    out["unknown_bytes"] = sum(it.size for it in unknown)
    out["referenced_only_bytes"] = sum(it.size for it in items if it.cls == "referenced")
    out["unknown_ranges"] = len(unknown)
    out["largest_unknown"] = [f"0x{it.start:X}..0x{it.end:X} ({it.size:#x})"
                              for it in sorted(unknown, key=lambda i: -i.size)[:12]]
    out["dropped_overlaps"] = dict(Counter(d.category for d in dropped))
    if gfx is not None:  # what the graphics level alone resolves, in this same run
        plain, _ = _ledger(rom, img, dm, au, None, assets, rsp or [])
        out["unknown_without_graphics"] = sum(it.size for it in plain if it.cls == "unknown")
    out["complete"] = out["unknown_bytes"] == 0
    out["next_steps"] = next_steps(rom, img, cat, out, unknown)
    return {"report": out, "ledger": items, "catalogue": cat, "program": img}


def _entropy(data: bytes) -> float:
    import math
    if not data:
        return 0.0
    counts = Counter(data)
    n = len(data)
    return -sum(c / n * math.log2(c / n) for c in counts.values())


def describe_bytes(data: bytes, samples: int = 4, size: int = 0x4000) -> str:
    """A hint about unclassified bytes: entropy, and how much zlib can still shrink samples
    spread over the range (packed data no longer shrinks; tables and raw media do)."""
    import zlib
    if not data:
        return "empty"
    step = max(size, len(data) // samples)
    chunks = [data[i:i + size] for i in range(0, len(data), step)][:samples]
    raw = sum(len(c) for c in chunks)
    ratio = sum(len(zlib.compress(c, 6)) for c in chunks) / raw
    h = sum(_entropy(c) * len(c) for c in chunks) / raw
    if h < 1.0 and ratio < 0.15:
        hint = "nearly constant fill (padding or an unused area)"
    elif ratio > 0.97:
        hint = "already compressed (zlib cannot shrink it): a codec n64dk does not know; find its loader"
    elif ratio < 0.6:
        hint = "raw structured data (tables, text, level data)"
    else:
        hint = "raw media or lightly packed data (audio, textures, animation)"
    return f"{h:.1f} bits/byte, zlib ratio {ratio:.2f}: {hint}"


def next_steps(rom: Rom, img, cat, out: dict, unknown: list[Interval]) -> list[str]:
    """What to look at next, most bytes (or failures) first, each with a concrete hint."""
    steps: list[tuple[int, str]] = []
    for name, err in out["errors"].items():
        if not name.endswith("_trace"):
            steps.append((1 << 40, f"{name} failed ({err}); the rest of the survey ran without it, see "
                                   f"report.json errors.{name}_trace"))
    main = img.main()
    if main.kind == "packed" and main.rom is None:
        steps.append((main.size, f"the program at 0x{main.vram:08X} ({main.size:#x} bytes) was written by the loader "
                                 f"from an untraced source; main_segment.bin holds the unpacked bytes"))
    elif main.kind == "packed" and not main.codec:
        calls = ", ".join(img.loader.get("calls", [])[:4])
        how = (f"n64dk runs the {main.game_codec} through the emulator (n64dk.gamecodec), so it can decode "
               f"other data packed the same way; decompile that routine to add a native codec"
               if main.game_codec else f"decompile the loader routines ({calls}) to add one")
        steps.append((main.rom_end - main.rom,
                      f"packed program ROM 0x{main.rom:X}..0x{main.rom_end:X} ({main.rom_end - main.rom:#x} bytes) "
                      f"uses a codec n64dk does not implement: {how}; split main_segment.bin "
                      f"(the unpacked program, {main.size:#x} bytes)"))
    for c in out["cross_checks"]:
        if "unexplained" in c:
            steps.append((1 << 20, f"investigate: {c}"))
    for it in sorted(unknown, key=lambda i: -i.size)[:6]:
        steps.append((it.size, f"ROM 0x{it.start:X}..0x{it.end:X} ({it.size:#x} bytes) is unclassified; "
                               + describe_bytes(rom.data[it.start:it.end])))
    if cat is not None:
        flagged = Counter(f.kind for f in cat.findings if f.kind in ("falls_through", "branch_outside",
                                                                      "cross_branch", "interior_alias"))
        if flagged:
            steps.append((sum(flagged.values()) * 64,
                          "review function boundaries flagged in functions.toml: "
                          + ", ".join(f"{n} {k}" for k, n in flagged.most_common())))
    return [text for _w, text in sorted(steps, key=lambda s: -s[0])]


def try_game_codec(rom: Rom, img, assets: dict, budget: float = 60.0, max_size: int = 0x100000,
                   min_size: int = 0x1000) -> dict:
    """Decode loader-referenced ranges with the game's own unpacking routine.

    A range counts as packed with the game's codec only when two independent sources agree:
    a loader table or call says an item of N bytes starts there, and the routine, run on
    it, consumes N bytes (to the 256-byte read-tracking granule) and expands them.  A decode
    from an arbitrary offset is not trusted: LZ-style routines expand random input too.  Items
    smaller than ``min_size`` are skipped: at 256-byte granularity they agree too easily.
    """
    from . import gamecodec
    codec = gamecodec.find(img, img.call_args)
    taken = {(s.rom, s.end) for s in assets.get("stream_objects", [])}
    cands = sorted({(it.start, it.end) for it in assets["ledger"]
                    if it.category == "loader_range" and min_size <= it.end - it.start <= max_size
                    and (it.start, it.end) not in taken}, key=lambda r: r[1] - r[0])
    tried, found, t0 = 0, [], time.time()
    for a, z in cands:
        if time.time() - t0 > budget:
            break
        data = rom.data[a:z]
        if len(zlib_compress(data)) < 0.9 * len(data):   # compressible: not packed
            continue
        tried += 1
        dec = gamecodec.decode(rom, codec, a, max_in=z - a, max_steps=40_000_000)
        if dec is not None and abs(dec.consumed - (z - a)) <= 1 << 8:
            found.append((a, z, len(dec.data)))
    intervals = [Interval(a, z, "payload", "compressed", kind="game_codec", name=f"game_codec_{a:X}",
                          evidence=f"unpacked to {n:#x} bytes by {codec.describe()}; size from a loader reference")
                 for a, z, n in found]
    summary = {"routine": codec.describe(), "candidates": len(cands), "tried": tried, "decoded": len(found),
               "decoded_bytes": sum(z - a for a, z, _ in found), "seconds": round(time.time() - t0, 1),
               "budget_exhausted": time.time() - t0 > budget}
    return {"summary": summary, "intervals": intervals}


def zlib_compress(data: bytes) -> bytes:
    import zlib
    return zlib.compress(data[:0x10000], 6)


def rsp_in_main(rom: Rom, img) -> list[tuple[int, int]]:
    """RSP code blocks (microcode text, 64-byte granularity) inside the main segment, as VRAM."""
    from .sdk.fingerprint import rsp_text_ranges
    b, data = img.boot_info(), img.main_data(rom)
    main = img.main()
    lo = b.vram_to_rom(main.vram)
    return [(b.rom_to_vram(a), b.rom_to_vram(z)) for a, z in rsp_text_ranges(data, lo, lo + main.size)]


def _cross_checks(img, cat, dm, rsp=()) -> list[str]:
    notes = []
    b = img.boot
    if img.method == "emulated" and b.main_call and img.hand_over and b.main_call != img.hand_over:
        notes.append(f"static hand-over 0x{b.main_call:08X} differs from the emulated one 0x{img.hand_over:08X}")
    if cat and dm and dm.text:
        f_end, d_end = cat.text_end, dm.text[-1][1]
        lo, hi = sorted((f_end, d_end))
        covered = sum(max(0, min(hi, z) - max(lo, a)) for a, z in rsp)
        if abs(f_end - d_end) <= 4:
            pass   # the same end, one counting the alignment word
        elif f_end < d_end and covered >= (hi - lo) // 2:
            notes.append(f"text end: functions 0x{f_end:08X}, datamap 0x{d_end:08X}; the difference is RSP code "
                         f"linked into .text ({covered:#x} of {hi - lo:#x} bytes): CPU code ends at 0x{f_end:08X}, "
                         f"the .text section at 0x{d_end:08X}")
        else:
            notes.append(f"text end: functions 0x{f_end:08X}, datamap 0x{d_end:08X} (unexplained)")
    ext = [f.detail for f in (cat.findings if cat else []) if f.kind == "text_extended"]
    for e in ext:
        notes.append(f"functions recovered code past an embedded block: {e}")
    main = img.main()
    if main.kind == "packed" and main.rom is None:
        notes.append("packed main segment: source not traced")
    elif main.kind == "packed" and not main.codec and main.game_codec:
        notes.append(f"packed main segment ROM 0x{main.rom:X}..0x{main.rom_end:X}: no native codec; unpacked by the "
                     f"{main.game_codec}, which n64dk can run on other data")
    elif main.kind == "packed" and not main.codec:
        notes.append(f"packed main segment ROM 0x{main.rom:X}..0x{main.rom_end:X}: codec not identified "
                     f"(loader calls {', '.join(img.loader.get('calls', [])[:4])})")
    return notes


_UCODE_FAMILY = (("F3DEX2", "f3dex2"), ("S2DEX2", "f3dex2"), ("F3DEX", "f3dex"), ("F3DLX", "f3dex"),
                 ("F3DLP", "f3dex"), ("S2DEX", "f3dex"), ("F3D", "f3d"))


def scan_graphics(data: bytes, microcode=()) -> dict:
    """Display-list pools and standalone display lists over the whole ROM.

    The GBI family is the one whose pools cover the most bytes, then the one the ROM's
    microcode names (F3DEX 2.xx counts as F3DEX2), then the one that finds the most lists.  Pools of self-relative blobs (a list with its
    vertices, textures and palettes) are placed whole; other lists only cover their commands
    (their data is addressed through runtime segments and cannot be placed from the ROM).
    """
    from .assets import gbi
    hinted: dict[str, str] = {}
    for u in microcode:
        name = (u.family or "").upper()
        fam = next((f for prefix, f in _UCODE_FAMILY if name.startswith(prefix)), None)
        if fam == "f3dex" and str(u.version or "").startswith("2"):
            fam = "f3dex2"   # F3DEX 2.xx is the F3DEX2 GBI
        if fam:
            hinted.setdefault(fam, f"microcode {u.family} {u.version or ''} at ROM 0x{u.rom:X}".replace("  ", " "))
    found = {}
    for f in ("f3dex", "f3dex2", "f3d"):
        lists = gbi.find_display_lists(data, f)
        found[f] = (lists, gbi.find_blob_pools(data, f, lists=lists))
    # The microcode is a hint; what decodes decides (several games carry more than one).
    family = max(found, key=lambda f: (sum(p.end - p.start for p in found[f][1]), f in hinted,
                                       len(found[f][0]), f == "f3dex"))
    how = hinted.get(family, "pool bytes, then lists decoded")
    lists, pools = found[family]
    in_pool = [(p.start, p.end) for p in pools]
    loose = [r for r in lists if not any(a <= r.start < z for a, z in in_pool)]
    blobs = [b for p in pools for b in p.blobs]
    summary = {"family": family, "family_from": how, "display_lists": len(lists),
               "pools": [f"0x{p.start:X}..0x{p.end:X} ({len(p.blobs)} blobs)" for p in pools],
               "blobs": len(blobs), "textured_blobs": sum(1 for b in blobs if b.walk.textures),
               "pool_bytes": sum(p.end - p.start for p in pools), "standalone_lists": len(loose)}
    return {"summary": summary, "pools": pools, "loose": loose}


def _ledger(rom: Rom, img, dm, au, gfx, assets, rsp=()) -> tuple[list[Interval], list[Interval]]:
    size = len(rom.data)
    led = Ledger(0, size)
    taken: list[tuple[int, int]] = []
    dropped: list[Interval] = []

    def add(a, z, cls, category, clip=False, **kw):
        """Place [a, z) unless it overlaps a placed interval: then drop it, or with ``clip``
        place only the parts nothing claims yet (for evidence-only ranges)."""
        a, z = max(0, a), min(size, z)
        if a >= z:
            return
        hits = sorted((x, y) for x, y in taken if x < z and a < y)
        if hits and not clip:
            dropped.append(Interval(a, z, cls, category, **kw))
            return
        cur = a
        for x, y in hits + [(z, z)]:
            if x > cur:
                led.add(cur, x, cls, category=category, **kw)
                taken.append((cur, x))
            cur = max(cur, y)

    add(0, 0x40, "rom_metadata", "rom_metadata", kind="header", name="rom_header", evidence="rom header")
    add(0x40, 0x1000, "rom_metadata", "rom_metadata", kind="ipl3", name="ipl3", evidence=f"cic {rom.cic}")

    main = img.main()
    if img.method == "emulated":
        for a, z in img.loader.get("code_rom", []):
            add(a, z, "code", "program", kind="loader", name=f"loader_{a:X}", evidence="executed before the hand-over")
    if main.kind == "packed":
        if main.rom is not None:
            add(main.rom, main.rom_end, "program_image", "program",
                kind=main.codec or ("packed_game_codec" if main.game_codec else "packed"),
                name=f"program_{main.rom:X}", evidence=main.evidence)
    elif dm is not None and dm.sections:
        b = dm.boot
        for a, z in rsp:   # RSP microcode text linked into the CPU sections
            add(b.vram_to_rom(a), b.vram_to_rom(z), "code", "program", kind="rsp_text",
                name=f"rsp_{a:08X}", evidence="RSP opcodes only, vector-heavy (64-byte granularity)")
        for a, z, sec in dm.sections:
            cls = "code" if sec == ".text" else "data"
            add(b.vram_to_rom(a), b.vram_to_rom(z), cls, "program", clip=True, kind=sec,
                name=f"{sec.lstrip('.')}_{a:08X}", evidence="datamap section")
    else:
        add(main.rom, main.rom_end, "program_image", "program", kind="main", name=f"main_{main.rom:X}",
            evidence=main.evidence)
    if au is not None:
        for it in au.intervals:
            if it.cls in ("padding", "unknown"):
                continue
            add(it.start, it.end, it.cls, "audio", kind=it.kind, name=it.name, evidence=it.evidence)
    asset_items = [it for it in (assets["ledger"] if assets is not None else [])
                   if it.cls not in ("padding", "unknown", "rom_metadata")]
    # verified codec streams outrank pattern-matched display lists
    for it in asset_items:
        if it.category == "compressed":
            add(it.start, it.end, it.cls, "compressed", kind=it.kind, name=it.name, evidence=it.evidence)
    if gfx is not None:
        fam = gfx["summary"]["family"]
        for p in gfx["pools"]:
            for bl in p.blobs:
                w = bl.walk
                add(bl.start, bl.end, "payload", "graphics", kind="dl_blob", name=f"gfx_{bl.start:X}",
                    evidence=f"{fam} self-relative blob in pool 0x{p.start:X}: {len(w.commands)} commands, "
                             f"{len(w.vertices)} vertex loads, {len(w.textures)} textures")
        for r in gfx["loose"]:
            add(r.start, r.end, "payload", "graphics", kind="display_list", name=f"dl_{r.start:X}",
                evidence=f"{fam} display list, {len(r.commands)} commands (data not resolvable)")
    for it in asset_items:
        if it.category != "compressed":   # a reference, not an understanding of the content
            add(it.start, it.end, "referenced", "loader_range", clip=True, kind=it.kind, name=it.name,
                evidence=it.evidence)
    return led.complete(rom.data), dropped


def render_markdown(rep: dict) -> str:
    r = rep
    rom = r["rom"]
    lines = [f"# Survey: {rom.get('title', '?')} ({rom.get('game_code', '?')})", ""]
    size = rom.get("size") or 1
    fn = (r.get("functions") or {}).get("functions", "?")
    sdk_named = (r.get("sdk") or {}).get("functions_named", "?")
    m = r["program"]["main"]
    lines += [f"- **Program**: {r['program']['method']} image, main segment {m['vram']}..{m['end']} ({m['kind']}); "
              f"{fn} functions, {sdk_named} named from the SDK",
              f"- **ROM understood**: {100 - 100 * (r['unknown_bytes'] + r.get('referenced_only_bytes', 0)) / size:.1f} % "
              f"by content, plus {100 * r.get('referenced_only_bytes', 0) / size:.1f} % only referenced by loader "
              f"tables; {r['unknown_bytes']:,} bytes unknown in {r['unknown_ranges']} ranges"]
    lines += [f"- **Next**: {s}" for s in r.get("next_steps", [])[:3]]
    lines += [""]
    lines += ["## ROM", ""] + [f"- **{k}**: {v}" for k, v in rom.items()] + [""]
    p = r["program"]
    lines += ["## Program image", "",
              f"- method: {p['method']}; entry 0x{int(p['entry'], 16):08X}; hand-over {p['hand_over']}; "
              f"stack {p['stack_top']}", "",
              "| vram | end | size | kind | ROM | codec | evidence |", "|---|---|---|---|---|---|---|"]
    for s in p["segments"]:
        rom_rng = f"{s['rom']}..{s['rom_end']}" if "rom" in s else "-"
        lines.append(f"| {s['vram']} | {s['end']} | 0x{s['size']:X} | {s['kind']} | {rom_rng} | "
                     f"{s.get('codec', '-')} | {s.get('evidence', '')} |")
    if p.get("loader"):
        ld = p["loader"]
        lines += ["", f"- loader: {ld.get('steps')} instructions emulated, hand-over at step {ld.get('image_step')}; "
                  f"calls {', '.join(ld.get('calls', []))}"]
        if ld.get("stage2"):
            lines.append(f"- second stage: {ld['stage2']}")
        for w in ld.get("other_writes", []):
            lines.append(f"- other write: {w}")
    lines += ["", f"Offsets below are {r['offsets']}.", ""]
    for key, title in (("functions", "Functions"), ("datamap", "Data map"), ("sdk", "SDK"),
                       ("audio", "Audio"), ("gfx", "Graphics"),
                       ("assets", "Compressed streams and loader ranges")):
        lines += [f"## {title}", ""]
        if r.get(key) is None:
            lines += [f"- failed: {r['errors'].get(key, 'not run')}", ""]
            continue
        lines += [f"- **{k}**: {v}" for k, v in r[key].items()] + [""]
    lines += ["## ROM ledger", "",
              f"- complete: **{r['complete']}** (unknown bytes: {r['unknown_bytes']:,} in {r['unknown_ranges']} ranges)",
              "", "| class | bytes |", "|---|---|"]
    lines += [f"| {k} | {v:,} |" for k, v in sorted(r["ledger_totals"].items(), key=lambda kv: -kv[1])]
    lines += ["", "| category | bytes |", "|---|---|"]
    lines += [f"| {k} | {v:,} |" for k, v in sorted(r["ledger_categories"].items(), key=lambda kv: -kv[1])]
    if r["largest_unknown"]:
        lines += ["", "Largest unknown ranges:", ""] + [f"- {u}" for u in r["largest_unknown"]]
    if r["dropped_overlaps"]:
        lines += ["", f"Overlapping lower-priority intervals dropped: {r['dropped_overlaps']}"]
    if r["cross_checks"]:
        lines += ["", "## Cross-checks", ""] + [f"- {c}" for c in r["cross_checks"]]
    if r.get("next_steps"):
        lines += ["", "## Next steps", "", "Most bytes (or failures) first.", ""]
        lines += [f"{i}. {s}" for i, s in enumerate(r["next_steps"], 1)]
    lines += ["", "## Timings (s)", "", ", ".join(f"{k} {v}" for k, v in r["timings"].items())]
    errs = {k: v for k, v in r["errors"].items() if not k.endswith("_trace")}
    if errs:
        lines += ["", "## Errors", ""] + [f"- {k}: {v}" for k, v in errs.items()]
    return "\n".join(lines) + "\n"
