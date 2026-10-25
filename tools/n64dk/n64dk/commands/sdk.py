"""SDK identification commands: library function names, SDK version, microcode, audio and compiler."""
from __future__ import annotations

from pathlib import Path

from ..cli import command
from ..rom import Rom


def _hex(x: str) -> int:
    return int(x, 16)


def _report(args) -> dict:
    from .. import sdk
    from ..sdk.db import Database
    rom = Rom.load(args.rom)
    db = Database.load(args.db)
    if args.exclude_source:
        db = db.without_sources(args.exclude_source)
    region = None
    if args.range:
        a, _, b = args.range.partition(":")
        start, end = _hex(a), _hex(b)
        base = sdk.code_region(rom)
        region = (start, end, _hex(args.vram) if args.vram else base[2] + start - base[0])
    rep = sdk.analyse(rom, db, region, args.min_confidence)
    out = {
        "rom": str(args.rom),
        "region": {"rom_start": f"0x{rep.region[0]:X}", "rom_end": f"0x{rep.region[1]:X}",
                   "vram": f"0x{rep.region[2]:08X}"},
        "summary": rep.summary(),
        "versions": rep.versions,
        "functions": [m.to_json() for m in rep.matches],
        "microcode": [{"rom": f"0x{u.rom:X}", "text": u.text, "family": u.family, "version": u.version,
                       "variant": u.variant,
                       "nearest_rsp_text": u.text_range and [f"0x{u.text_range[0]:X}", f"0x{u.text_range[1]:X}"]}
                      for u in rep.microcode],
        "rsp_text": [[f"0x{a:X}", f"0x{b:X}"] for a, b in sdk.fingerprint.rsp_text_ranges(rom.data)],
        "audio": [{"rom": f"0x{a.rom:X}", "kind": a.kind, "count": a.count} for a in rep.audio],
        "libmus": [{"rom": f"0x{o:X}", "tag": s} for o, s in rep.libmus],
        "compiler": {k: v for k, v in rep.compiler.items() if k != "functions"},
    }
    if args.functions_compiler:
        out["compiler"]["functions"] = rep.compiler["functions"]
    if args.truth:
        from ..sdk.validate import evaluate, load_truth
        ev = evaluate(rep.matches, load_truth(args.truth), db.names())
        out["validation"] = ev
    return out


@command("sdk", "Name SDK library functions and report SDK version, microcode, audio library and compiler")
def sdk_cmd(args):
    out = _report(args)
    if args.format == "json":
        return out
    s = out["summary"]
    print(f"region      ROM {out['region']['rom_start']}..{out['region']['rom_end']} @ {out['region']['vram']}")
    print(f"functions   {s['functions_named']} named ({s['high_confidence']} with confidence >= 0.8)")
    print(f"sdk version {', '.join(s['sdk_version']) or 'unknown'}")
    print(f"library cc  {s['library_compiler']}")
    print(f"game cc     {s['game_compiler']} {s['game_compiler_share']}")
    print(f"microcode   {'; '.join(s['microcode']) or 'none found'}")
    print(f"audio       {s['audio'] or 'no ALBankFile/ALSeqFile'}; libmus {s['libmus'] or 'absent'}")
    for f in out["functions"]:
        alt = f" (or {', '.join(f['alternatives'])})" if f["alternatives"] else ""
        print(f"  {f['vram']} {f['rom']:>9} {f['size']:>6} {f['confidence']:.2f} {f['kind']:<7} {f['name']}{alt}")
    if "validation" in out:
        v = out["validation"]
        print(f"validation  precision {v['precision']} ({v['correct']}/{v['correct'] + v['wrong']}), "
              f"recall {v['recall']} ({v['recalled']}/{v['expected']})")
    return 0


def _sdk_args(p):
    p.add_argument("rom", help="ROM image")
    p.add_argument("--db", help="signature database (default: bundled)")
    p.add_argument("--min-confidence", type=float, default=0.5)
    p.add_argument("--range", help="ROM range START:END (hex) to scan instead of the main segment")
    p.add_argument("--vram", help="VRAM of the range start (hex; default: main-segment mapping)")
    p.add_argument("--exclude-source", action="append", default=[],
                   help="ignore signatures only present in sources with this tag prefix (repeatable)")
    p.add_argument("--truth", help="symbol TOML ([[section]] functions=[{name,vram,size}]) to score against")
    p.add_argument("--functions-compiler", action="store_true", help="include per-function compiler verdicts")


sdk_cmd.arguments = _sdk_args


@command("sdk-build-db", "Compile library sources from a recipe and write the SDK signature database")
def sdk_build_db(args):
    from ..sdk import build
    from ..sdk.db import DEFAULT_PATH
    recipe = build.load_recipe(args.recipe)
    db = build.build(recipe, Path(args.scratch), args.jobs, log=lambda m: None if args.format == "json" else print(m))
    out = Path(args.out or DEFAULT_PATH)
    db.save(out)
    return {"output": str(out), "entries": len(db), "names": len(db.names()), "builds": db.versions(),
            "compiler_model": db.model.totals}


def _build_args(p):
    p.add_argument("recipe", help="TOML recipe (toolchains, ultralib, sources, objects)")
    p.add_argument("--out", help="output database (default: the bundled database)")
    p.add_argument("--scratch", default="build/sdk", help="directory for compiled objects")
    p.add_argument("--jobs", type=int, help="parallel compile jobs")


sdk_build_db.arguments = _build_args
