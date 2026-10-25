"""Build pipelines: C source -> relocatable MIPS ELF object with an original toolchain.

Pipelines
    kmc+hook     KMC GCC -S -> project normalizer hook -> KMC GNU as
    kmc+builtin  KMC GCC -S -> built-in normalizer -> KMC GNU as
    kmc+as       KMC GCC -S -> KMC GNU as (assembler reordering left on)
    kmc+asn64    KMC GCC -S -> ASN64-compatible converter -> modern GNU as --no-pad-sections
    ido5.3       IDO 5.3 cc -c
    ido7.1       IDO 7.1 cc -c
    as           assembly source -> KMC GNU as
    sn           host cpp -> SN cc1n64 -> asn64.exe -> psyq-obj-parser (via wibo)

Pseudo flags understood in flag lists (manifest conventions):
    --section-base=SECTION:ADDR  --symbol=NAME:ADDR  --assembler=asn64|normalizer
    --normalizer-NAME            forwarded to the normalizer hook as --NAME
"""
from __future__ import annotations

import hashlib
import os
import subprocess
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

from . import asn64, normalize
from .toolchain import Config

PIPELINES = ("kmc+hook", "kmc+builtin", "kmc+as", "kmc+asn64", "ido5.3", "ido7.1", "sn", "as")
TOOLCHAIN_OF = {"ido5.3": "ido53", "ido7.1": "ido71", "sn": "sn"}


class BuildError(RuntimeError):
    pass


@dataclass
class BuildSpec:
    pipeline: str
    flags: list[str]                       # compiler flags only
    section_bases: dict[str, int] = field(default_factory=dict)
    symbols: dict[str, int] = field(default_factory=dict)
    normalizer_args: list[str] = field(default_factory=list)

    def manifest_flags(self) -> str:
        """Flag column for a five-column matching manifest row."""
        out = []
        if self.pipeline == "kmc+asn64":
            out.append("--assembler=asn64")
        out += [f"--section-base={k}:0x{v:08X}" for k, v in self.section_bases.items() if k != ".text"]
        out += [f"--symbol={k}:0x{v:08X}" for k, v in self.symbols.items()]
        out += [f"--normalizer-{a.lstrip('-')}" for a in self.normalizer_args]
        return " ".join(out + self.flags)


def split_flags(flags: list[str], pipeline: str) -> BuildSpec:
    spec = BuildSpec(pipeline, [])
    for flag in flags:
        if flag.startswith("--section-base="):
            sec, addr = flag.split("=", 1)[1].split(":", 1)
            spec.section_bases[sec] = int(addr, 0)
        elif flag.startswith("--symbol="):
            name, addr = flag.split("=", 1)[1].split(":", 1)
            spec.symbols[name] = int(addr, 0)
        elif flag.startswith("--assembler="):
            lane = flag.split("=", 1)[1]
            if lane == "asn64":
                spec.pipeline = "kmc+asn64"
            elif lane != "normalizer":
                raise BuildError(f"unsupported assembler lane {lane!r}")
        elif flag.startswith("--normalizer-"):
            spec.normalizer_args.append("--" + flag[len("--normalizer-"):])
        else:
            spec.flags.append(flag)
    return spec


def _run(argv, cwd, env=None, stdin=None, retries: int = 0):
    """Run a tool; ``retries`` re-runs it after a crash (a pass reported as killed by a signal),
    which the statically recompiled IDO binaries occasionally suffer under parallel load."""
    full_env = dict(os.environ)
    if env:
        full_env.update(env)
    for _attempt in range(retries + 1):
        proc = subprocess.run([str(a) for a in argv], cwd=cwd, env=full_env, input=stdin,
                              capture_output=True, text=True)
        crashed = proc.returncode < 0 or "died due to signal" in (proc.stderr or "")
        if not (proc.returncode and crashed):
            break
    if proc.returncode:
        raise BuildError(f"{Path(str(argv[0])).name} failed: {(proc.stderr or proc.stdout).strip()[-600:]}")
    return proc


def _as_flags(flags: list[str]) -> list[str]:
    # The assembler always runs at -G0: small-data placement is decided by the compiler's -G
    # (assembler -G would turn absolute %hi/%lo accesses into $gp-relative ones).
    return ["-mips3", "-G0"]


def build(source: Path, spec: BuildSpec, cfg: Config, workdir: Path,
          includes: list[Path] | None = None, defines: list[str] | None = None) -> Path:
    """Compile ``source`` and return the object path inside ``workdir``.  Commands run in the
    project directory so relative ``-I`` flags behave as in the project's own build."""
    source = Path(source).resolve()
    cwd = cfg.project
    inc = [f"-I{p}" for p in (includes if includes is not None else cfg.include)]
    inc += list(defines or [])
    tag = hashlib.sha1(f"{source}|{spec.pipeline}|{spec.flags}|{spec.normalizer_args}".encode()).hexdigest()[:12]
    unit_dir = workdir / tag   # private directory: compilers drop fixed-name temporaries
    unit_dir.mkdir(parents=True, exist_ok=True)
    stem = unit_dir / source.stem
    obj = Path(f"{stem}.o")
    pipe = spec.pipeline
    if pipe.startswith("kmc"):
        kmc = cfg.toolchain("kmc")
        if kmc is None:
            raise BuildError("KMC GCC 2.7.2 toolchain not found")
        raw = Path(f"{stem}.raw.s")
        _run([kmc / "gcc", f"-B{kmc}/", "-S", *spec.flags, *inc, "-o", raw, source], cwd)
        if pipe == "kmc+asn64":
            gas = cfg.gnu_as()
            if gas is None:
                raise BuildError("modern MIPS GNU as not found")
            text = asn64.convert(raw.read_text(errors="replace"))
            _run([gas[0], *asn64.GNU_AS_FLAGS, "-", "-o", obj], cwd, env=gas[1], stdin=text)
            return obj
        asm = raw
        if pipe == "kmc+hook":
            if not cfg.normalizer:
                raise BuildError("no normalizer hook configured")
            asm = Path(f"{stem}.s")
            normalize.run_hook(cfg.normalizer, raw, asm, cwd, spec.normalizer_args)
        elif pipe == "kmc+builtin":
            asm = Path(f"{stem}.s")
            asm.write_text(normalize.builtin(raw.read_text(errors="replace")))
        _run([kmc / "as", *_as_flags(spec.flags), "-o", obj, asm], cwd)
        return obj
    if pipe == "as":
        kmc = cfg.toolchain("kmc")
        if kmc is None:
            raise BuildError("KMC GNU as not found")
        _run([kmc / "as", *_as_flags(spec.flags), *inc, "-o", obj, source], cwd)
        return obj
    if pipe in ("ido5.3", "ido7.1"):
        ido = cfg.toolchain(TOOLCHAIN_OF[pipe])
        if ido is None:
            raise BuildError(f"{pipe} toolchain not found")
        # IDO's -O3 pass (uld) writes <stem>.u into the working directory: run in the private
        # build directory and make relative include paths absolute.
        flags = [f"-I{(cwd / f[2:]).resolve()}" if f.startswith("-I") and not Path(f[2:]).is_absolute()
                 else f for f in spec.flags]
        _run([ido / "cc", "-c", *flags, *inc, "-o", obj, source], unit_dir,
             env={"TMPDIR": str(unit_dir)}, retries=4)
        return obj
    if pipe == "sn":
        sn = cfg.toolchain("sn")
        if sn is None:
            raise BuildError("SN GCC 2.8 toolchain not found")
        pre, asm, psyq = Path(f"{stem}.i"), Path(f"{stem}.s"), Path(f"{stem}.obj")
        _run(["gcc", "-E", "-P", "-undef", "-D__GNUC__=2", "-D__GNUC_MINOR__=8", *inc, source, "-o", pre], cwd)
        _run([sn / "wibo", sn / "cc1n64.exe", "-quiet", *spec.flags, pre, "-o", asm], cwd)
        _run([sn / "wibo", sn / "asn64.exe", "-o", psyq, asm], cwd)
        _run([sn / "psyq-obj-parser", psyq, "-b", "-n", "-o", obj], cwd)
        return obj
    raise BuildError(f"unknown pipeline {pipe!r}")


def scratch_dir() -> tempfile.TemporaryDirectory:
    return tempfile.TemporaryDirectory(prefix="n64dk-match-")
