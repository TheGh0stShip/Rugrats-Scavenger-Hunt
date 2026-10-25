"""Build the SDK signature database by compiling library sources.

Inputs are described by a recipe (TOML):

    [toolchains]                # directories holding the compiler drivers
    ido = "/path/ido5.3"        # contains ``cc``
    kmc = "/path/kmc-gcc-2.7.2" # contains ``gcc`` and ``as``
    sn = "/path/sn-gcc-2.8"     # optional: wibo, cc1n64.exe, asn64.exe, psyq-obj-parser

    [ultralib]                  # an ultralib source tree
    path = "/path/ultralib"
    versions = ["I", "J", "K", "L"]
    targets = ["libultra_rom", "libgultra_rom"]

    [[sources]]                 # a project manifest of matched library units
    tag = "project"
    root = "/path/project"
    manifest = "config/us/matching_units.tsv"   # START SIZE SOURCE FLAGS [SHA]
    pipeline = "ido"                            # ido | kmc | sn
    include = ["src/libultra"]                  # source path prefixes to keep
    cflags = "-Iinclude"

    [[sources]]                 # or a glob of sources compiled with one flag set
    tag = "project"
    root = "/path/project"
    glob = ["src/libmus/*.c"]
    pipeline = "kmc"
    cflags = "-O2 -G0 -mips3 -mgp32 -mfp32 -Iinclude"

    [[objects]]                 # relocatable objects or ar archives
    tag = "vendor"
    path = "/path/libgultra_rom.a"

Objects are written under a scratch directory; only masked signature hashes end
up in the database.  The ultralib flag table mirrors ultralib's makefiles.
"""
from __future__ import annotations

import fnmatch
import os
import shlex
import subprocess
import tomllib
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass, field
from pathlib import Path

from .db import Database
from .signature import signatures_from_file

ULTRALIB_VERSIONS = "DEFGHIJKL"
MGU_MATRIX = ("mtxcatf", "normalize", "scale", "translate")


@dataclass
class Job:
    tag: str            # database source tag, e.g. "ultralib:2.0L:libultra_rom"
    compiler: str       # ido | kmc | sn
    source: Path
    output: Path
    argv: list[str]
    cwd: Path
    env: dict = field(default_factory=dict)
    sn: bool = False


def _ultralib_jobs(path: Path, version: str, target: str, tc: dict, scratch: Path) -> list[Job]:
    gcc = "libgultra" in target
    rom, debug = "_rom" in target, "_d" in target
    if gcc and "kmc" not in tc or not gcc and "ido" not in tc:
        return []
    members = None
    listing = path / "base" / version / f"{target}.txt"
    if listing.is_file():
        members = {line.strip() for line in listing.read_text().split() if line.strip()}
    early = version in "DEFGHI"
    files = sorted(p for p in (path / "src").rglob("*") if p.suffix in (".c", ".s"))
    out = []
    for src in files:
        rel = src.relative_to(path)
        sub = rel.parts[1] if len(rel.parts) > 2 else ""
        stem = src.stem
        if sub == "voice":
            continue  # needs Shift-JIS preprocessing
        if version in "DEFGHIJ":
            if sub == "mgu" and stem in MGU_MATRIX and src.suffix == ".s":
                continue
        elif sub == "gu" and stem in MGU_MATRIX and src.suffix == ".c":
            continue
        if members is not None and f"{stem}.o" not in members:
            continue
        gbi = "-DF3DEX_GBI"
        if (sub, stem) in (("gu", "parse_gbi"), ("sp", "sprite")):
            gbi = "-DF3D_GBI"
        elif (sub, stem) in (("gu", "us2dex_emu"), ("gu", "us2dex2_emu"), ("sp", "spriteex"), ("sp", "spriteex2")):
            gbi = ""
        defs = [f"-DBUILD_VERSION=VERSION_{version}", f'-DBUILD_VERSION_STRING="2.0{version}"',
                "-D_MIPS_SZLONG=32", "-D_DEBUG" if debug else "-DNDEBUG"]
        if gbi:
            defs.append(gbi)
        if rom:
            defs.append("-D_FINALROM")
        output = scratch / "ultralib" / version / target / rel.with_suffix(".o")
        tag = f"ultralib:2.0{version}:{target}"
        if gcc:
            kmc = Path(tc["kmc"])
            inc = ["-I", ".", "-I", str(path / "include"), "-I", str(path / "include/compiler/gcc"),
                   "-I", str(path / "include/PR")]
            env = {"COMPILER_PATH": str(kmc), "VR4300MUL": "OFF" if sub == "mgu" and stem != "rotate" else "ON"}
            if src.suffix == ".s":
                argv = [str(kmc / "gcc"), "-x", "assembler-with-cpp", "-w", "-nostdinc", "-c", "-G", "0",
                        "-mgp32", "-mfp32", "-DMIPSEB", "-D_LANGUAGE_ASSEMBLY", "-D_MIPS_SIM=1", "-D_ULTRA64",
                        "-mips3", *defs, *inc]
                if sub in ("debug", "error", "log", "os", "gu", "libc", "rmon"):
                    argv.append("-P")
            else:
                opt = "-O0" if debug else "-O3"
                if (sub, stem) == ("os", "initialize_isv"):
                    opt = "-O2"
                elif (sub, stem) == ("os", "assert") or ((sub, stem) == ("os", "seterrorhandler") and not early):
                    opt = "-O0"
                argv = [str(kmc / "gcc"), "-w", "-nostdinc", "-c", "-G", "0", "-mgp32", "-mfp32",
                        "-D_LANGUAGE_C", "-mips3", "-D__USE_ISOC99", opt, *defs, *inc]
                if version in "DEFGHIJ":
                    argv.append("-funsigned-char")
                if (sub, stem) == ("host", "host_ptn64"):
                    argv.append("-fno-builtin")
            out.append(Job(tag, "kmc", src, output, argv + [src.name, "-o", str(output)], src.parent, env))
        else:
            ido = Path(tc["ido"])
            inc = ["-I", str(path / "include"), "-I", str(path / "include/compiler/ido"), "-I", str(path / "include/PR")]
            mips = ["-mips2", "-o32"]
            if (sub, stem) in (("libc", "ll"), ("libc", "llbit"), ("libc", "llcvt"), ("os", "exceptasm")):
                mips = ["-mips3", "-32"]
            elif (sub, stem) == ("log", "delay"):
                mips = ["-mips1", "-o32"]
            pic = ["-KPIC"] if (sub, stem) == ("log", "delay") else ["-non_shared"]
            if src.suffix == ".s":
                opt = "-O0 -g2" if debug else ("-O2" if sub in ("libc", "mgu") else "-O1")
                argv = [str(ido / "cc"), "-c", "-Wab,-r4300_mul", "-G", "0", "-nostdinc", "-woff",
                        "516,649,838,712", *mips, *defs, *pic, *opt.split(), *inc]
            else:
                if debug:
                    opt = "-O1 -g2"
                elif sub in ("debug", "host", "os", "rmon", "log") or (sub, stem) in (
                        ("libc", "ll"), ("libc", "llbit"), ("libc", "llcvt")):
                    opt = "-O1"
                elif version in "DEFGHI" and sub in ("libc", "sched", "gu", "mgu", "sp", "audio", "rg", "gt"):
                    opt = "-O3"
                else:
                    opt = "-O1" if version in "DEFGHI" else "-O2"
                if (sub, stem) == ("os", "initialize_isv"):
                    opt = "-O2"
                argv = [str(ido / "cc"), "-c", "-Wab,-r4300_mul", "-G", "0", "-nostdinc", "-Xcpluscomm",
                        "-fullwarn", "-woff", "516,649,838,712", *mips, *defs, *pic, *opt.split(), *inc]
            out.append(Job(tag, "ido", src, output, argv + [src.name, "-o", str(output)], src.parent))
    return out


def _manifest_rows(root: Path, spec: dict) -> list[tuple[Path, list[str]]]:
    rows = []
    include = spec.get("include") or [""]
    for line in (root / spec["manifest"]).read_text().splitlines():
        if not line.strip() or line.startswith("#"):
            continue
        fields = line.split("\t")
        if len(fields) < 4:
            continue
        source = fields[2]
        if not any(source.startswith(p) for p in include):
            continue
        flags = [f for f in shlex.split(fields[3]) if not f.startswith("--")]
        rows.append((root / source, flags))
    return rows


def _source_jobs(spec: dict, tc: dict, scratch: Path) -> list[Job]:
    root = Path(spec["root"])
    pipeline = spec.get("pipeline", "ido")
    extra = shlex.split(spec.get("cflags", ""))
    if "manifest" in spec:
        rows = _manifest_rows(root, spec)
    else:
        rows = [(p, []) for pattern in spec.get("glob", []) for p in sorted(root.glob(pattern))]
    jobs, seen = [], set()
    for src, flags in rows:
        if src in seen or not src.is_file():
            continue
        seen.add(src)
        output = scratch / "sources" / spec["tag"] / src.relative_to(root).with_suffix(".o")
        tag = spec["tag"]
        if pipeline == "ido" and "ido" in tc:
            argv = [str(Path(tc["ido"]) / "cc"), "-c", *flags, *extra, "-o", str(output), str(src)]
            jobs.append(Job(tag, "ido", src, output, argv, root))
        elif pipeline == "kmc" and "kmc" in tc:
            kmc = Path(tc["kmc"])
            if src.suffix == ".s":
                argv = [str(kmc / "as"), "-mips3", "-G0", "-o", str(output), str(src)]
            else:
                argv = [str(kmc / "gcc"), f"-B{kmc}/", "-c", *flags, *extra, "-o", str(output), str(src)]
            jobs.append(Job(tag, "kmc", src, output, argv, root, {"COMPILER_PATH": str(kmc)}))
        elif pipeline == "sn" and "sn" in tc:
            jobs.append(Job(tag, "sn", src, output, [*flags, *extra], root, sn=True))
    return jobs


def _run_sn(job: Job, sn: Path) -> None:
    pre, asm, psyq = job.output.with_suffix(".i"), job.output.with_suffix(".s"), job.output.with_suffix(".obj")
    run = dict(cwd=job.cwd, check=True, capture_output=True)
    subprocess.run(["gcc", "-E", "-P", "-undef", "-D__GNUC__=2", "-D__GNUC_MINOR__=8",
                    *[a for a in job.argv if a.startswith(("-I", "-D"))], str(job.source), "-o", str(pre)], **run)
    subprocess.run([str(sn / "wibo"), str(sn / "cc1n64.exe"), "-quiet",
                    *[a for a in job.argv if not a.startswith(("-I", "-D"))], str(pre), "-o", str(asm)], **run)
    subprocess.run([str(sn / "wibo"), str(sn / "asn64.exe"), "-o", str(psyq), str(asm)], **run)
    subprocess.run([str(sn / "psyq-obj-parser"), str(psyq), "-b", "-n", "-o", str(job.output)], **run)


def _run(job: Job, tc: dict) -> tuple[Job, str | None]:
    job.output.parent.mkdir(parents=True, exist_ok=True)
    try:
        if job.sn:
            _run_sn(job, Path(tc["sn"]))
        else:
            env = dict(os.environ, **job.env)
            subprocess.run(job.argv, cwd=job.cwd, env=env, check=True, capture_output=True, timeout=300)
        return job, None
    except (subprocess.CalledProcessError, subprocess.TimeoutExpired, OSError) as exc:
        err = getattr(exc, "stderr", b"") or b""
        return job, (err.decode("latin-1", "replace").strip().splitlines() or [str(exc)])[-1]


def build(recipe: dict, scratch: Path, jobs_n: int | None = None, log=print) -> Database:
    tc = {k: v for k, v in recipe.get("toolchains", {}).items() if v}
    scratch = Path(scratch).resolve()
    scratch.mkdir(parents=True, exist_ok=True)
    jobs: list[Job] = []
    ul = recipe.get("ultralib")
    if ul:
        path = Path(ul["path"])
        for version in ul.get("versions", list("IJKL")):
            for target in ul.get("targets", ["libultra_rom", "libgultra_rom"]):
                jobs.extend(_ultralib_jobs(path, version, target, tc, scratch))
    for spec in recipe.get("sources", []):
        jobs.extend(_source_jobs(spec, tc, scratch))
    db = Database()
    failures = 0
    with ThreadPoolExecutor(max_workers=jobs_n or os.cpu_count() or 4) as pool:
        for job, err in pool.map(lambda j: _run(j, tc), jobs):
            if err is not None:
                failures += 1
                continue
            try:
                sigs = signatures_from_file(job.output.read_bytes(), job.source.name)
            except (ValueError, OSError, IndexError):
                failures += 1
                continue
            db.add(sigs, job.tag, job.compiler)
    log(f"compiled {len(jobs) - failures}/{len(jobs)} units")
    for spec in recipe.get("objects", []):
        p = Path(spec["path"])
        files = [p] if p.is_file() else sorted(q for q in p.rglob("*") if q.suffix in (".o", ".a"))
        for f in files:
            if fnmatch.fnmatch(f.name, spec.get("pattern", "*")):
                try:
                    db.add(signatures_from_file(f.read_bytes(), f.name), spec["tag"], spec.get("compiler", "unknown"))
                except (ValueError, IndexError):
                    pass
    return db


def load_recipe(path: str | Path) -> dict:
    return tomllib.loads(Path(path).read_text())
