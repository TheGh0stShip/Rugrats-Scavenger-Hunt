"""Matching commands: one-shot relocated compare / configuration sweep, manifest batches and
the permuter driver."""
from __future__ import annotations

import shlex
import sys
from pathlib import Path

from ..cli import command
from ..match import manifest as manmod
from ..match import symbols as symmod
from ..match.engine import Target, batch, default_pipelines, manifest_row, match_source, sweep
from ..match.pipeline import PIPELINES, BuildSpec, split_flags, scratch_dir
from ..match.toolchain import PROFILES, load_config
from ..rom import Rom


def _common(p, rom_required: bool):
    p.add_argument("--rom", required=rom_required, help="retail ROM image"
                   + ("" if rom_required else " (default: n64dk.toml [match] rom or baserom*.z64 in the project)"))
    p.add_argument("--project", help="project directory: working directory for builds, "
                   "default include/, .toolchain/ and symbol files (default: current directory)")
    p.add_argument("--config", help="n64dk.toml (default: <project>/n64dk.toml)")
    p.add_argument("--toolchain-dir", help="directory holding kmc-gcc-2.7.2/, ido5.3/, ... ")
    p.add_argument("--symbols", action="append", default=[],
                   help="symbol file: splat symbol_addrs.txt or catalogue TOML (repeatable)")
    p.add_argument("-I", "--include", action="append", default=[], help="include directory (repeatable)")
    p.add_argument("--normalizer", help="assembly normalizer hook, e.g. 'python3 tools/norm.py {input} {output}'")
    p.add_argument("--vram-delta", type=lambda s: int(s, 0), help="VRAM - ROM offset of the code segment")
    p.add_argument("--jobs", type=int, default=None, help="parallel builds (default: CPU count)")


def _setup(args):
    cfg = load_config(args.project, args.config, args.toolchain_dir)
    if args.normalizer:
        cfg.normalizer = args.normalizer
    if args.include:
        cfg.include = list(dict.fromkeys([Path(cfg.project, i).resolve() for i in args.include] + cfg.include))
    rom_path = args.rom or cfg.find_rom()
    if rom_path is None:
        raise SystemExit("no ROM: pass --rom or set [match] rom in n64dk.toml")
    image = Rom.load(rom_path)
    if image.checksum_ok() is False:
        raise SystemExit(f"{rom_path}: header checksum mismatch; not a clean base image")
    target = Target.from_rom(image, args.vram_delta if args.vram_delta is not None else cfg.vram_delta)
    sym_files = [Path(s) for s in args.symbols] or cfg.symbols or symmod.discover(cfg.project)
    syms = symmod.load(sym_files)
    return cfg, target, syms, sym_files


def _spec(args, cfg) -> BuildSpec:
    pipe = args.pipeline
    if pipe is None:
        avail = default_pipelines(cfg)
        pipe = avail[0] if avail else "kmc+builtin"
    flags = shlex.split(args.flags) if args.flags is not None else None
    if flags is None:
        key = {"ido5.3": "ido53", "ido7.1": "ido71", "sn": "sn"}.get(pipe, "kmc")
        flags = list(PROFILES[key].default_flags)
    return split_flags(flags, pipe)


def _match_args(p):
    p.add_argument("source", help="C (or assembly) source file")
    _common(p, rom_required=True)
    p.add_argument("--addr", type=lambda s: int(s, 16),
                   help="VRAM of the unit's .text (hex; default: search the ROM for the compiled code)")
    p.add_argument("--size", type=lambda s: int(s, 16), help="text size in bytes (hex; default: compiled size)")
    p.add_argument("--pipeline", choices=PIPELINES, help="build pipeline (default: first available)")
    p.add_argument("--flags", help="compiler flags (default: the toolchain profile's standard flags)")
    p.add_argument("--sweep", action="store_true",
                   help="search pipelines x flags x rodata placement for the first exact configuration")
    p.add_argument("--pipelines", help="comma-separated pipelines for --sweep (default: all available)")
    p.add_argument("--all", action="store_true", help="with --sweep: do not stop at the first exact result")
    p.add_argument("--no-infer", action="store_true",
                   help="do not recover unresolved external symbols from the retail code")
    p.add_argument("--lines", type=int, default=24, help="diff lines shown in text output")


@command("match", "Build a source with an original toolchain and compare it relocated against the ROM")
def match(args):
    cfg, target, syms, _files = _setup(args)
    source = Path(args.source).resolve()
    if args.sweep:
        pipes = args.pipelines.split(",") if args.pipelines else None
        base = split_flags(shlex.split(args.flags), "kmc+builtin") if args.flags else None
        res = sweep(source, cfg, target, args.addr, args.size, syms, pipes, args.jobs,
                    stop_at_first=not args.all, base_spec=base)
        out = {"source": str(source), "vram": f"0x{(res.exact.vram if res.exact else args.addr or 0):08X}",
               "tried": len(res.tried),
               "pipelines": pipes or default_pipelines(cfg), "exact": res.exact is not None}
        if res.exact is not None:
            o = res.exact
            out.update({"pipeline": o.spec.pipeline, "flags": " ".join(o.spec.flags),
                        "bases": {k: f"0x{v:08X}" for k, v in o.result.bases.items()},
                        "manifest_row": manifest_row(o, cfg.project, source, target, syms)})
            if args.all:
                out["all_exact"] = [f"{x.spec.pipeline} {' '.join(x.spec.flags)}" for x in res.tried if x.exact]
        else:
            best = res.best()
            if best is not None:
                out.update({"best_pipeline": best.spec.pipeline, "best_flags": " ".join(best.spec.flags),
                            "best_diff": best.result.diff.summary() if best.result.diff else best.error})
        if args.format == "json":
            out["results"] = [{"pipeline": o.spec.pipeline, "flags": " ".join(o.spec.flags), "exact": o.exact,
                               "score": (o.result.diff.score if o.result and o.result.diff else
                                         (0 if o.exact else None)), "error": o.error} for o in res.tried]
        return out
    spec = _spec(args, cfg)
    with scratch_dir() as tmp:
        o = match_source(source, spec, cfg, target, args.addr, args.size, syms, Path(tmp),
                         infer_externals=not args.no_infer)
    if args.format == "json":
        d = o.as_dict()
        if o.exact:
            d["manifest_row"] = manifest_row(o, cfg.project, source, target, syms)
        return d
    lines = [f"{'MATCH' if o.exact else 'DIFF'} {source.name} @ 0x{o.vram:08X} "
             f"[{spec.pipeline} {' '.join(spec.flags)}]"]
    if o.error:
        lines.append(f"error: {o.error}")
    if o.result:
        r = o.result
        lines.append(f".text 0x{r.text.size:X} bytes (compiled 0x{r.compiled_size:X})"
                     + (f", first difference at +0x{r.text.first_diff:X}" if r.text.first_diff is not None else ""))
        for s in r.sections:
            lines.append(f"{s.name} @ 0x{s.vram:08X} 0x{s.size:X} bytes: {'match' if s.match else 'DIFF'}")
        if r.inferred_symbols:
            lines.append("symbols recovered from the ROM: " +
                         ", ".join(f"{k}=0x{v:08X}" for k, v in sorted(r.inferred_symbols.items())))
        if r.error:
            lines.append(r.error)
        if r.diff:
            lines.append("diff: " + r.diff.summary())
            lines += r.diff.lines(args.lines)
        if o.exact:
            lines.append("manifest row: " + manifest_row(o, cfg.project, source, target, syms))
    return "\n".join(lines)


match.arguments = _match_args


def _batch_args(p):
    p.add_argument("manifests", nargs="+", metavar="MANIFEST[=PIPELINE]",
                   help="manifest TSV; optional =PIPELINE overrides the pipeline taken from its header")
    _common(p, rom_required=False)
    p.add_argument("--kmc-lane", choices=("hook", "builtin", "as", "asn64"), default=None,
                   help="assembler lane for KMC rows without --assembler= (default: hook when a "
                        "normalizer is configured, else builtin)")
    p.add_argument("--source-template", help="unit-row source path, e.g. 'src/code/{unit}.c'")
    p.add_argument("--flags", help="compiler flags for unit rows (default: toolchain profile)")
    p.add_argument("--status-flags", action="append", default=[], metavar="STATUS=FLAGS",
                   help="extra compiler flags for unit rows with this status (repeatable)")
    p.add_argument("--rodata-map", help="TSV UNIT VRAM SIZE fixing .rodata placement of unit rows")
    p.add_argument("--infer-externals", action="store_true",
                   help="recover symbols no symbol file or batch unit defines from the retail code")
    p.add_argument("--only", action="append", default=[], help="verify only units whose name contains this")


@command("match-batch", "Rebuild and verify every unit of one or more matching manifests in parallel")
def match_batch(args):
    cfg, target, syms, sym_files = _setup(args)
    lane = args.kmc_lane or ("hook" if cfg.normalizer else "builtin")
    status_flags = {}
    for item in args.status_flags:
        k, _, v = item.partition("=")
        status_flags[k] = shlex.split(v)
    rodata = manmod.load_section_map(Path(cfg.project, args.rodata_map)) if args.rodata_map else None
    units = []
    for spec in args.manifests:
        path, _, pipe = spec.partition("=")
        if pipe and pipe not in PIPELINES and pipe != "kmc":
            raise SystemExit(f"unknown pipeline {pipe!r}")
        units += manmod.parse(Path(cfg.project, path), cfg.project, target.rom_to_vram, pipe or None,
                              f"kmc+{lane}", args.source_template,
                              shlex.split(args.flags) if args.flags else None, status_flags, rodata)
    if args.only:
        units = [u for u in units if any(o in u.name for o in args.only)]
    done = [0]

    def progress(_o):
        done[0] += 1
        if args.format != "json" and sys.stderr.isatty():
            print(f"\r{done[0]}/{len(units)}", end="", file=sys.stderr, flush=True)

    outs = batch(units, cfg, target, syms, args.jobs, args.infer_externals, progress)
    exact = [o for o in outs if o.exact]
    by_pipe: dict[str, list[int]] = {}
    for o in outs:
        by_pipe.setdefault(o.spec.pipeline, [0, 0])
        by_pipe[o.spec.pipeline][0] += 1
        by_pipe[o.spec.pipeline][1] += o.exact
    summary = {"units": len(outs), "exact": len(exact), "failed": len(outs) - len(exact),
               "text_bytes": sum(o.size for o in exact),
               "data_bytes": sum(s.size for o in exact for s in (o.result.sections if o.result else [])),
               "by_pipeline": {k: f"{v[1]}/{v[0]}" for k, v in sorted(by_pipe.items())},
               "symbol_files": [str(f) for f in sym_files]}
    if args.format == "json":
        summary["results"] = [o.as_dict() for o in outs]
        return summary
    if sys.stderr.isatty():
        print(file=sys.stderr)
    lines = [f"{'MATCH' if o.exact else 'FAIL '} [{o.spec.pipeline}] {o.name}: 0x{o.vram:08X} (0x{o.size:X} bytes)"
             + ("" if o.exact else " -- " + (o.error or (o.result.error if o.result and o.result.error else "")
                                             or (o.result.diff.summary() if o.result and o.result.diff else
                                                 "digest mismatch" if o.digest_ok is False else "section mismatch")))
             for o in outs]
    lines.append(f"Verified {summary['exact']}/{summary['units']} units, {summary['text_bytes']} code bytes, "
                 f"{summary['data_bytes']} data bytes ({', '.join(f'{k} {v}' for k, v in summary['by_pipeline'].items())})")
    print("\n".join(lines))
    return 0 if not summary["failed"] else 1


match_batch.arguments = _batch_args


def _permute_args(p):
    p.add_argument("source", help="C source containing the function")
    _common(p, rom_required=True)
    p.add_argument("--addr", required=True, type=lambda s: int(s, 16), help="VRAM of the unit's .text (hex)")
    p.add_argument("--size", type=lambda s: int(s, 16), help="text size (hex)")
    p.add_argument("--function", help="C name of the function to permute (default: the one at --addr)")
    p.add_argument("--pipeline", choices=PIPELINES)
    p.add_argument("--flags")
    p.add_argument("--iterations", type=int, default=200)
    p.add_argument("--seed", type=int)
    p.add_argument("--max-mutations", type=int, default=3)
    p.add_argument("--temperature", type=float, default=0.1)
    p.add_argument("--out", help="directory for best.c")


@command("match-permute", "Random AST permutation search scored by the relocated instruction diff")
def match_permute(args):
    from ..match import permuter
    if not permuter.available():
        raise SystemExit("pycparser is not importable; install it or add it to PYTHONPATH")
    cfg, target, syms, _files = _setup(args)
    spec = _spec(args, cfg)
    res = permuter.permute(Path(args.source).resolve(), spec, cfg, target, args.addr, args.size, syms,
                           cfg.include, args.function, args.iterations, args.jobs or 4, args.seed,
                           args.max_mutations, args.temperature, Path(args.out) if args.out else None)
    return {"base_score": res.base_score, "best_score": res.best_score, "exact": res.exact,
            "iterations": res.iterations, "improvements": res.history,
            **({"best_source": res.best_source} if args.format == "json" else {})}


match_permute.arguments = _permute_args
