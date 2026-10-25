"""Random source-permutation search on top of the matching engine (needs ``pycparser``).

Preparation makes a source parseable and relocatable on its own: ``__asm__("label")``
aliases are removed from declarations and the C names they covered are recorded as symbols at
the label's address (the ``LDSYM`` idea), then the file is run through the host C
preprocessor.  Each iteration applies 1..k random AST mutations to one function, renders C,
builds it with the configured pipeline and scores it with the classified instruction diff
(0 = exact).  Candidates are evaluated ``jobs`` at a time; a candidate replaces the current
base when its score is lower, or equal with probability ``temperature``.

The search follows the approach of decomp-permuter and of the Global Assault project's
tools/decomp_permuter.py (GPL-3.0); only a small, semantics-preserving mutation set is used
here.  Results are candidates for review, not proof of original source.
"""
from __future__ import annotations

import copy
import hashlib
import random
import re
import subprocess
import tempfile
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass, field
from pathlib import Path

from . import symbols as symmod
from .engine import Target, match_source
from .pipeline import BuildSpec
from .toolchain import Config

ASM_LABEL = re.compile(r"\s*__asm__\s*\(\s*\"([^\"]+)\"\s*\)")
DECL_WITH_LABEL = re.compile(
    r"([A-Za-z_]\w*)\s*((?:\[[^\]]*\]\s*)*(?:\((?:[^()]|\([^()]*\))*\)\s*)?)__asm__\s*\(\s*\"([^\"]+)\"\s*\)")
COMMUTATIVE = {"+", "*", "&", "|", "^", "==", "!="}
FLIP = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}


def available() -> bool:
    try:
        import pycparser  # noqa: F401
        return True
    except ImportError:
        return False


@dataclass
class Prepared:
    text: str                       # preprocessed, parseable C
    symbols: dict[str, int]         # C names -> retail addresses
    function: str                   # C name of the function to permute


def prepare(source: Path, symbols: symmod.SymbolMap, includes: list[Path], function: str | None,
            vram: int) -> Prepared:
    raw = Path(source).read_text(errors="replace")
    extra = symmod.from_source(raw, symbols)
    names: dict[str, int] = dict(extra)
    func_name = function
    for m in DECL_WITH_LABEL.finditer(raw):
        cname, label = m.group(1), m.group(3)
        addr = extra.get(label, symbols.resolve(label))
        if addr is not None:
            names[cname] = addr
            if func_name is None and addr == vram:
                func_name = cname
    stripped = ASM_LABEL.sub("", raw)
    defines = ["-D__attribute__(x)=", "-D__extension__=", "-D__inline__=inline", "-D__asm__(x)="]
    proc = subprocess.run(["cpp", "-P", "-nostdinc", "-undef", *defines, *[f"-I{p}" for p in includes],
                           "-I", str(Path(source).parent), "-"], input=stripped, capture_output=True, text=True)
    if proc.returncode:
        raise RuntimeError("preprocessing failed: " + proc.stderr.strip()[-400:])
    if func_name is None:
        raise RuntimeError("cannot tell which function to permute; pass the function name")
    return Prepared(proc.stdout, names, func_name)


# ------------------------------------------------------------------------- mutations
def _nodes(node, cls):
    out = []

    def walk(n):
        if isinstance(n, cls):
            out.append(n)
        for _, child in n.children():
            walk(child)
    walk(node)
    return out


def _ids(node) -> set[str]:
    from pycparser import c_ast
    return {n.name for n in _nodes(node, c_ast.ID)}


def _mutations(fn, rng: random.Random) -> bool:
    from pycparser import c_ast
    choices = []
    binops = _nodes(fn.body, c_ast.BinaryOp)
    if any(b.op in COMMUTATIVE for b in binops):
        choices.append("commute")
    if any(b.op in FLIP for b in binops):
        choices.append("flip")
    compounds = [c for c in _nodes(fn.body, c_ast.Compound) if c.block_items and len(c.block_items) > 1]
    if compounds:
        choices += ["swap-decl", "swap-stmt"]
    ifs = [i for i in _nodes(fn.body, c_ast.If) if i.iffalse is not None]
    if ifs:
        choices.append("invert-if")
    if not choices:
        return False
    kind = rng.choice(choices)
    if kind == "commute":
        b = rng.choice([b for b in binops if b.op in COMMUTATIVE])
        b.left, b.right = b.right, b.left
    elif kind == "flip":
        b = rng.choice([b for b in binops if b.op in FLIP])
        b.left, b.right, b.op = b.right, b.left, FLIP[b.op]
    elif kind == "invert-if":
        i = rng.choice(ifs)
        i.cond = c_ast.UnaryOp("!", i.cond)
        i.iftrue, i.iffalse = i.iffalse, i.iftrue
    else:
        c = rng.choice(compounds)
        pairs = []
        for k in range(len(c.block_items) - 1):
            a, b = c.block_items[k], c.block_items[k + 1]
            if kind == "swap-decl" and isinstance(a, c_ast.Decl) and isinstance(b, c_ast.Decl) \
                    and a.init is None and b.init is None:
                pairs.append(k)
            elif kind == "swap-stmt" and isinstance(a, c_ast.Assignment) and isinstance(b, c_ast.Assignment) \
                    and isinstance(a.lvalue, c_ast.ID) and isinstance(b.lvalue, c_ast.ID) \
                    and a.lvalue.name != b.lvalue.name and not _nodes(a, c_ast.FuncCall) \
                    and not _nodes(b, c_ast.FuncCall) and a.lvalue.name not in _ids(b.rvalue) \
                    and b.lvalue.name not in _ids(a.rvalue):
                pairs.append(k)
        if not pairs:
            return False
        k = rng.choice(pairs)
        c.block_items[k], c.block_items[k + 1] = c.block_items[k + 1], c.block_items[k]
    return True


@dataclass
class PermuteResult:
    base_score: int
    best_score: int
    best_source: str
    iterations: int
    exact: bool
    history: list[tuple[int, int]] = field(default_factory=list)  # (iteration, score)


def permute(source: Path, spec: BuildSpec, cfg: Config, target: Target, vram: int, size: int | None,
            symbols: symmod.SymbolMap, includes: list[Path], function: str | None = None,
            iterations: int = 200, jobs: int = 4, seed: int | None = None, max_mutations: int = 3,
            temperature: float = 0.1, out_dir: Path | None = None) -> PermuteResult:
    from pycparser import c_ast, c_generator, c_parser
    prep = prepare(source, symbols, includes, function, vram)
    syms = symbols.copy()
    syms.update(prep.symbols)
    ast = c_parser.CParser().parse(prep.text, filename=str(source))
    gen = c_generator.CGenerator()
    rng = random.Random(seed)
    seen: dict[str, int] = {}

    def find(tree):
        for ext in tree.ext:
            if isinstance(ext, c_ast.FuncDef) and ext.decl.name == prep.function:
                return ext
        raise RuntimeError(f"function {prep.function} not found")

    with tempfile.TemporaryDirectory(prefix="n64dk-permute-") as tmp:
        work = Path(tmp)

        def score(text: str) -> int:
            key = hashlib.sha1(text.encode()).hexdigest()
            if key in seen:
                return seen[key]
            path = work / f"cand_{key[:16]}.c"
            path.write_text(text)
            o = match_source(path, spec, cfg, target, vram, size, syms, work, includes=[],
                             infer_externals=True)
            if o.exact:
                val = 0
            elif o.result is None:
                val = 1 << 30
            else:
                val = max(1, o.result.diff.score if o.result.diff else 1)
            seen[key] = val
            path.unlink(missing_ok=True)
            return val

        base_text = gen.visit(ast)
        base_score = best_score = current_score = score(base_text)
        best_text, current = base_text, ast
        history = [(0, base_score)]
        it = 0
        with ThreadPoolExecutor(jobs) as pool:
            while it < iterations and best_score > 0:
                cands = []
                for _ in range(min(jobs, iterations - it)):
                    tree = copy.deepcopy(current)
                    fn = find(tree)
                    for _ in range(rng.randint(1, max_mutations)):
                        _mutations(fn, rng)
                    cands.append((tree, gen.visit(tree)))
                it += len(cands)
                unique = list(dict.fromkeys(text for _, text in cands))
                by_text = dict(zip(unique, pool.map(score, unique)))
                scores = [by_text[text] for _, text in cands]
                for (tree, text), sc in zip(cands, scores):
                    if sc < current_score or (sc == current_score and rng.random() < temperature):
                        current, current_score = tree, sc
                    if sc < best_score:
                        best_score, best_text = sc, text
                        history.append((it, sc))
    if out_dir is not None:
        out_dir.mkdir(parents=True, exist_ok=True)
        header = "".join(f"/* LDSYM {k}=0x{v:08X} */\n" for k, v in sorted(prep.symbols.items()))
        (out_dir / "best.c").write_text(header + best_text)
    return PermuteResult(base_score, best_score, best_text, it, best_score == 0, history)
