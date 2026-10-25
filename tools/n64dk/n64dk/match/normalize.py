"""Assembly normalizers placed between GCC ``-S`` and an old GNU assembler.

``builtin`` makes KMC GCC 2.7.2 output deterministic for GNU as 2.x in the way the
An earlier GPL-3.0 normalizer establishes the initial transformation: GCC
already wraps the transfers whose delay slots it filled in ``.set noreorder`` regions, so
assembler reordering is disabled everywhere else and an explicit ``nop`` is added after each
transfer emitted in reorder mode.  GCC's internal ``b 3f`` of the 64-bit shift helper is
spelled ``bgez $zero,3f`` as the retail assembler expanded it.

Project-specific normalizers run as external hooks (see :func:`run_hook`).
"""
from __future__ import annotations

import re
import shlex
import subprocess
from pathlib import Path

TRANSFER = re.compile(
    r"^(?:b(?:al|eq|ne|gez|gezal|gtz|lez|ltz|ltzal|c1[ft]|c1fl|c1tl|eql|nel|gezl|gtzl|lezl|ltzl)?|"
    r"j|jal|jr|jalr)\s+")
LONG_SHIFT_BRANCH = re.compile(r"^(\s*)b(\s+)3f\b")


def builtin(text: str) -> str:
    out: list[str] = []
    reorder = True
    for line in text.splitlines(keepends=True):
        body = line.split("#", 1)[0].strip()
        if body == ".set\tnoreorder" or re.fullmatch(r"\.set\s+noreorder", body):
            reorder = False
            out.append(line)
            continue
        if re.fullmatch(r"\.set\s+reorder", body):
            reorder = True
            continue  # keep the assembler in noreorder mode
        if reorder and LONG_SHIFT_BRANCH.match(line):
            line = LONG_SHIFT_BRANCH.sub(r"\1bgez\2$0,3f", line, count=1)
            body = line.split("#", 1)[0].strip()
        out.append(line)
        if reorder and TRANSFER.match(body):
            out.append("\tnop\n")
    return "\t.set\tnoreorder\n" + "".join(out)


def run_hook(command: str, src: Path, dst: Path, cwd: Path, extra: list[str] | None = None) -> None:
    """Run an external normalizer.  ``command`` is a shell-style template with ``{input}``,
    ``{output}`` and optional ``{extra}`` placeholders; without placeholders the input and
    output paths are appended."""
    argv = shlex.split(command)
    extra = extra or []
    if not any("{input}" in a or "{output}" in a for a in argv):
        argv = argv + ["{extra}", "{input}", "{output}"]
    final: list[str] = []
    for a in argv:
        if a == "{extra}":
            final.extend(extra)
        else:
            final.append(a.replace("{input}", str(src)).replace("{output}", str(dst)))
    proc = subprocess.run(final, cwd=cwd, capture_output=True, text=True)
    if proc.returncode:
        raise RuntimeError(f"normalizer failed: {proc.stderr.strip()[-400:]}")
