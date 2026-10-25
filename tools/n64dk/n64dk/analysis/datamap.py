"""Initialized-image data map: cross references, section split, ownership, ledger and typed units.

Works on the main segment that the IPL3 copies (ROM 0x1000 -> header entry) up to the
BSS start found by :mod:`n64dk.boot`, or on a program image's main segment
(``analyse(rom, program=...)``, :mod:`n64dk.image`):

1. **Code extent.** ``jal`` targets that land on code-looking words are clustered; the
   largest cluster (plus the entry routine) is ``.text``. Its ends are trimmed against
   the nearest addresses that code loads or stores.
2. **Cross references.** Every ``.text`` instruction is decoded with per-function register
   tracking: ``lui`` halves pair with ``addiu``/``ori``/loads/stores, survive ``addu``
   indexing (jump tables, arrays), and an address held in a register is followed through
   later ``offset(reg)`` accesses. Each site records its access type (f32 for
   ``lwc1``/``swc1``, f64 for ``ldc1``/``sdc1``, word, half, byte, dword or ``addr``).
3. **Sections.** Each non-code region is split into ``.rodata`` and ``.data`` at the point
   that best separates written addresses (data) from read-only evidence (jump tables,
   floating-point literals, strings).
4. **Objects and ownership.** Regions are cut at referenced addresses; each object lists
   the functions that reference it, its inferred type (string, Shift-JIS string, jump
   table, f32/f64 pool, pointer words, words) and, with one consumer, its owner.
5. **Ledger and units.** Matched rows (``ROM_START SIZE SOURCE SECTION|FLAGS ...`` TSV)
   are laid into an :class:`~n64dk.ledger.Ledger`; the remainder is classified from the
   analysis. Unmatched data runs can be emitted as explicit-width C structs.

The typed unit generator generalises an earlier GPL data-unit generator
decompilation project (GPL-3.0-or-later).
"""
from __future__ import annotations

import bisect
import csv
import hashlib
import re
import struct
from dataclasses import dataclass, field
from pathlib import Path

from .. import boot as bootmod
from .. import mips
from ..ledger import Interval, Ledger

ACCESS = {
    "lwc1": "f32", "swc1": "f32", "ldc1": "f64", "sdc1": "f64",
    "lw": "word", "sw": "word", "lwl": "word", "lwr": "word", "swl": "word", "swr": "word", "ll": "word", "sc": "word",
    "lh": "half", "lhu": "half", "sh": "half",
    "lb": "byte", "lbu": "byte", "sb": "byte",
    "ld": "dword", "sd": "dword", "ldl": "dword", "ldr": "dword", "sdl": "dword", "sdr": "dword", "lwu": "word",
    "addiu": "addr", "ori": "addr", "addi": "addr", "daddiu": "addr",
}


@dataclass(frozen=True)
class Xref:
    site: int          # vram of the instruction completing the address
    address: int       # data vram
    opcode: str
    access: str        # f32 f64 word half byte dword addr
    kind: str          # load store addr
    func: int          # vram of the containing function
    via: str = "hilo"  # "hilo" or "reg" (offset through a tracked address register)


@dataclass
class DataObject:
    start: int  # vram
    end: int
    section: str
    type: str
    owners: tuple[int, ...]
    accesses: tuple[str, ...]
    written: bool

    @property
    def owner(self) -> int | None:
        return self.owners[0] if len(self.owners) == 1 else None


@dataclass
class DataMap:
    boot: bootmod.BootInfo
    image_start: int          # vram
    image_end: int            # vram (BSS start)
    text: list[tuple[int, int]]
    functions: list[int]
    sections: list[tuple[int, int, str]]
    xrefs: dict[int, list[Xref]]
    objects: list[DataObject]
    jump_tables: list[tuple[int, int]] = field(default_factory=list)
    strings: list[tuple[int, int, str]] = field(default_factory=list)

    def rom(self, vram: int) -> int:
        return self.boot.vram_to_rom(vram)

    def func_of(self, vram: int) -> int | None:
        i = bisect.bisect_right(self.functions, vram) - 1
        return self.functions[i] if i >= 0 else None

    def in_text(self, vram: int) -> bool:
        return any(a <= vram < b for a, b in self.text)

    def section_of(self, vram: int) -> str | None:
        for a, b, s in self.sections:
            if a <= vram < b:
                return s
        return None


# --------------------------------------------------------------------------- code extent

def _word(data: bytes, off: int) -> int:
    return struct.unpack_from(">I", data, off)[0]


def _code_ratio(code: bytes, vram: int) -> float:
    total = ok = 0
    for ins in mips.instructions(code, vram):
        total += 1
        ok += ins.getRaw() == 0 or ins.isValid()
    return ok / total if total else 1.0


def _entry_end(data: bytes, base_rom: int, vram: int, limit: int = 0x400) -> int:
    """End vram of the entry routine: the first ``j``/``jr`` plus its delay slot."""
    for i in range(0, limit, 4):
        ins = mips.decode(_word(data, base_rom + i), vram + i)
        if ins.isJump() and not ins.doesLink():
            return vram + i + 8
        if ins.getOpcodeName() == "break":
            return vram + i + 4
    return vram + limit


def find_text(data: bytes, b: bootmod.BootInfo, image_end: int) -> tuple[list[tuple[int, int]], list[int]]:
    """Return ``.text`` ranges and candidate function starts (vram)."""
    lo, hi = b.main_vram, image_end
    counts: dict[int, int] = {}
    for off in range(b.main_rom, b.vram_to_rom(hi) - 3, 4):
        w = _word(data, off)
        if w >> 26 != 3:
            continue
        t = ((b.rom_to_vram(off) + 4) & 0xF0000000) | ((w & 0x03FFFFFF) << 2)
        if lo <= t < hi:
            counts[t] = counts.get(t, 0) + 1

    def codeish(t: int) -> bool:
        r = b.vram_to_rom(t)
        chunk = data[r:r + 0x20]
        return len(chunk) == 0x20 and _word(chunk, 0) != 0 and mips.looks_like_code(chunk, t, 1.0)

    targets = sorted(t for t in counts if codeish(t))
    # Cluster: neighbours join when the gap between them decodes as code.
    clusters: list[list[int]] = []
    for t in targets:
        if clusters:
            p = clusters[-1][-1]
            gap = data[b.vram_to_rom(p):b.vram_to_rom(t)]
            if _code_ratio(gap, p) >= 0.9:
                clusters[-1].append(t)
                continue
        clusters.append([t])
    # Second pass: join large neighbouring clusters across short or mostly-code gaps
    # (hand-written tables or microcode stubs embedded in .text).
    merged: list[list[int]] = []
    for c in clusters:
        if merged:
            p, t = merged[-1][-1], c[0]
            gap = data[b.vram_to_rom(p):b.vram_to_rom(t)]
            big = len(merged[-1]) >= 8 and len(c) >= 8
            if t - p <= 0x400 or (big and _code_ratio(gap, p) >= 0.6):
                merged[-1].extend(c)
                continue
        merged.append(list(c))
    best = max(merged, key=lambda c: sum(counts[t] for t in c))
    start, last = best[0], best[-1]
    # Bounds from loads/stores the cluster makes into its surroundings.
    seg = data[b.vram_to_rom(start):b.vram_to_rom(last) + 0x4000]
    refs = mips.scan_refs(seg, start)
    data_refs = [r.address for r in refs if r.kind in ("load", "store") and lo <= r.address < hi]
    below = max([a for a in data_refs if a < start], default=lo)
    above = min([a for a in data_refs if a > last], default=hi)
    # Extend backwards over uncalled functions that still decode cleanly.
    p = start
    while p - 4 > below:
        w = _word(data, b.vram_to_rom(p - 4))
        ins = mips.decode(w, p - 4)
        if not (w == 0 or (ins.isValid() and ins.isImplemented() and not ins.notEmittedByCompilers())):
            break
        p -= 4
    # Snap forward to the first stack-frame prologue or post-return boundary.
    cand = p
    while cand < start:
        w = _word(data, b.vram_to_rom(cand))
        if w != 0 and (w >> 16) == 0x27BD and w & 0x8000:
            break
        cand += 4
    if cand < start:
        chunk = data[b.vram_to_rom(cand):b.vram_to_rom(start)]
        if mips.returns(chunk, cand) and mips.looks_like_code(chunk, cand, 1.0):
            start = cand
            counts[start] = counts.get(start, 0)
    # Forward: last jr $ra (+ delay slot) before the first data reference above.
    end_rom = b.vram_to_rom(min(above, hi))
    rets = mips.returns(data[b.vram_to_rom(last):end_rom], last)
    end = (rets[-1] + 8) if rets else last + 4
    while end % 16 and end < above and _word(data, b.vram_to_rom(end)) == 0:
        end += 4
    text = [(start, end)]
    entry_end = _entry_end(data, b.main_rom, b.main_vram)
    if entry_end < start:
        text.insert(0, (b.main_vram, entry_end))
    else:
        text = [(b.main_vram, end)]
    funcs = sorted({t for t in counts if any(a <= t < z for a, z in text)} | {b.main_vram, start})
    return text, _split_functions(data, b, text, funcs)


def _split_functions(data, b, text, funcs):
    """Add starts after ``jr $ra`` + delay slot (skipping nop padding) to the call targets."""
    out = set(funcs)
    for a, z in text:
        for r in mips.returns(data[b.vram_to_rom(a):b.vram_to_rom(z)], a):
            p = r + 8
            while p < z and _word(data, b.vram_to_rom(p)) == 0:
                p += 4
            if p < z:
                out.add(p)
    return sorted(out)


# --------------------------------------------------------------------------- xrefs

def _r(ins, field_name: str) -> int:
    try:
        return mips.reg(getattr(ins, field_name))
    except RuntimeError:
        return -1


def scan_xrefs(data: bytes, b: bootmod.BootInfo, text, functions, lo: int, hi: int) -> list[Xref]:
    """Per-function %hi/%lo pairing with ``addu`` propagation and address-register tracking."""
    out: list[Xref] = []
    fset = set(functions)
    for a, z in text:
        hi_regs: dict[int, int] = {}
        ptr: dict[int, int] = {}
        func = a
        pending_clear = False
        for ins in mips.instructions(data[b.vram_to_rom(a):b.vram_to_rom(z)], a):
            v = ins.vram
            if v in fset:
                func = v
                hi_regs.clear(); ptr.clear()
            name = ins.getOpcodeName()
            rs, rt = _r(ins, "rs"), _r(ins, "rt")
            dest = ins.getDestinationGpr()
            dest = mips.reg(dest) if dest is not None else -1
            produced: dict[int, int] = {}
            if ins.canBeHi():
                produced[rt] = ins.getImmediate() << 16
            elif name in ("addu", "daddu") and (rs in hi_regs) != (rt in hi_regs):
                # base + index keeps the %hi for a later %lo access (indexed tables)
                produced[_r(ins, "rd")] = hi_regs[rs] if rs in hi_regs else hi_regs[rt]
            elif ins.canBeLo() and rs in hi_regs:
                imm = ins.getImmediate()
                addr = (hi_regs[rs] | imm) if name == "ori" else (hi_regs[rs] + (imm - 0x10000 if imm & 0x8000 else imm)) & 0xFFFFFFFF
                kind = "load" if ins.doesLoad() else "store" if ins.doesStore() else "addr"
                if lo <= addr < hi:
                    out.append(Xref(v, addr, name, ACCESS.get(name, "word"), kind, func))
                if kind == "addr":
                    ptr_val = addr
                    produced[rt] = None  # clear hi tracking for rt
                    ptr[rt] = ptr_val
                    hi_regs.pop(rt, None)
                    dest = -1
            elif (ins.doesLoad() or ins.doesStore()) and rs in ptr and name in ACCESS:
                imm = ins.getImmediate()
                addr = (ptr[rs] + (imm - 0x10000 if imm & 0x8000 else imm)) & 0xFFFFFFFF
                if lo <= addr < hi:
                    out.append(Xref(v, addr, name, ACCESS[name], "load" if ins.doesLoad() else "store", func, "reg"))
            # invalidate overwritten registers
            if dest >= 0 and dest not in produced:
                if not (ins.canBeLo() and rs in hi_regs and mips.kind_keeps_hi(ins)):
                    hi_regs.pop(dest, None)
                ptr.pop(dest, None)
            for r, val in produced.items():
                if val is not None:
                    hi_regs[r] = val
                    ptr.pop(r, None)
            if pending_clear:
                hi_regs.clear(); ptr.clear()
                pending_clear = False
            if ins.isJrRa() or ins.isUnconditionalBranch() or ins.isJrNotRa():
                pending_clear = True
            elif ins.isFunctionCall():
                # caller-saved registers die across a call (after the delay slot)
                for r in list(ptr):
                    if not 16 <= r <= 23:
                        ptr.pop(r)
    return out


# --------------------------------------------------------------------------- strings

PRINTABLE = set(range(0x20, 0x7F)) | {0x09, 0x0A, 0x0D}


def string_at(data: bytes, p: int, end: int, min_len: int = 4) -> tuple[int, str] | None:
    """If ``data[p:]`` holds a NUL-terminated ASCII or Shift-JIS string, return (end incl. NUL, kind)."""
    q, sjis = p, False
    while q < end:
        c = data[q]
        if c in PRINTABLE:
            q += 1
        elif (0x81 <= c <= 0x9F or 0xE0 <= c <= 0xEF) and q + 1 < end and 0x40 <= data[q + 1] <= 0xFC and data[q + 1] != 0x7F:
            q += 2
            sjis = True
        else:
            break
    if q - p < min_len or q >= end or data[q] != 0:
        return None
    return q + 1, "sjis" if sjis else "string"


# --------------------------------------------------------------------------- analysis

def _split_point(addrs: list[int], data_ev: set[int], ro_ev: set[int], a: int, z: int) -> tuple[list[tuple[int, int, str]], int]:
    """Best single cut of [a, z) into .data/.rodata (either order) by evidence agreement."""
    cuts = sorted({a, z} | {x for x in addrs if a <= x < z})
    d = sorted(data_ev)
    r = sorted(ro_ev)

    def count(s, x, y):
        return bisect.bisect_left(s, y) - bisect.bisect_left(s, x)

    best = None
    for c in cuts:
        for first, second in ((".data", ".rodata"), (".rodata", ".data")):
            score = 0
            for (x, y, sec) in ((a, c, first), (c, z, second)):
                score += count(d if sec == ".data" else r, x, y)
            key = (score, first == ".rodata")
            if best is None or key > best[0]:
                best = (key, c, first, second)
    (_, c, first, second) = best
    parts = [(x, y, s) for x, y, s in ((a, c, first), (c, z, second)) if y > x]
    return parts, best[0][0]


def analyse(rom, bss_start: int | None = None, program=None) -> DataMap:
    """``program`` (an :class:`n64dk.image.ProgramImage`) selects the main segment; with a
    packed segment the data offsets are offsets into the unpacked segment."""
    if program is not None:
        data, b = program.main_data(rom), program.boot_info()
    else:
        data, b = rom.data, bootmod.analyse(rom)
    image_end = bss_start or b.bss_start or (b.main_vram + b.copy_size)
    image_end = min(image_end, b.rom_to_vram(len(data)))
    lo = b.main_vram
    text, funcs = find_text(data, b, image_end)
    xs = scan_xrefs(data, b, text, funcs, lo, image_end)
    xrefs: dict[int, list[Xref]] = {}
    for x in xs:
        xrefs.setdefault(x.address, []).append(x)

    # non-code regions of the initialized image
    regions, cur = [], lo
    for a, z in text:
        if a > cur:
            regions.append((cur, a))
        cur = z
    if cur < image_end:
        regions.append((cur, image_end))

    text_lo, text_hi = text[0][0], text[-1][1]

    def into_text(w: int) -> bool:
        return any(a <= w < z for a, z in text)

    # Jump tables: referenced runs of words pointing into the referencing function.
    jtbls: list[tuple[int, int]] = []
    addrs = sorted(xrefs)
    for i, adr in enumerate(addrs):
        if adr % 4 or into_text(adr):
            continue
        nxt = addrs[i + 1] if i + 1 < len(addrs) else image_end
        p = adr
        while p < nxt and p + 4 <= image_end and into_text(_word(data, b.vram_to_rom(p))):
            p += 4
        if p - adr >= 8 and all(x.access in ("word", "addr") for x in xrefs[adr]):
            jtbls.append((adr, p))
    jt_set = {a for a, _ in jtbls}

    data_ev = {x.address for x in xs if x.kind == "store"}
    ro_ev = set(jt_set) | {x.address for x in xs if x.kind == "load" and x.access in ("f32", "f64") and x.via == "hilo"}
    ro_ev -= data_ev

    sections: list[tuple[int, int, str]] = []
    for a, z in regions:
        # the alignment pad straight after the entry routine belongs with it
        parts, _ = _split_point(addrs, data_ev, ro_ev, a, z)
        sections.extend(parts)
    for a, z in text:
        sections.append((a, z, ".text"))
    sections.sort()

    # Objects
    objects: list[DataObject] = []
    strings: list[tuple[int, int, str]] = []
    jt_end = dict(jtbls)
    for a, z, sec in sections:
        if sec == ".text":
            continue
        starts = sorted({a} | {x for x in addrs if a <= x < z})
        for i, s in enumerate(starts):
            e = starts[i + 1] if i + 1 < len(starts) else z
            refs = xrefs.get(s, [])
            accesses = tuple(sorted({x.access for x in refs}))
            owners = tuple(sorted({x.func for x in refs}))
            off = b.vram_to_rom(s)
            st = string_at(data, off, b.vram_to_rom(e))
            if s in jt_end:
                typ = "jtbl"
            elif st and (not accesses or accesses == ("addr",)):
                typ = st[1]
                strings.append((s, b.rom_to_vram(st[0]), st[1]))
            elif accesses and set(accesses) <= {"f32"}:
                typ = "f32"
            elif accesses and set(accesses) <= {"f64"}:
                typ = "f64"
            elif not refs:
                typ = "unreferenced"
            elif s % 4 == 0 and e - s >= 4 and all(into_text(_word(data, b.vram_to_rom(p))) or
                                                   lo <= _word(data, b.vram_to_rom(p)) < 0x80800000 and _word(data, b.vram_to_rom(p)) >= lo
                                                   for p in range(s, e - 3, 4)):
                typ = "pointers"
            else:
                typ = "words" if accesses else "unreferenced"
            objects.append(DataObject(s, e, sec, typ, owners, accesses, any(x.kind == "store" for x in refs)))
    return DataMap(b, lo, image_end, text, funcs, sections, xrefs, objects, jtbls, strings)


# --------------------------------------------------------------------------- matched rows / ledger

@dataclass(frozen=True)
class MatchedRow:
    rom: int
    size: int
    source: str
    section: str  # ".text" for code rows


def read_matched(path: str | Path) -> list[MatchedRow]:
    """Read ``ROM_START SIZE SOURCE <SECTION|FLAGS> ...`` rows; zero-size rows are skipped."""
    out = []
    for line in Path(path).read_text().splitlines():
        if not line.strip() or line.startswith("#"):
            continue
        cols = line.split("\t")
        try:
            start, size = int(cols[0], 16), int(cols[1], 16)
        except (ValueError, IndexError):
            continue
        if size <= 0:
            continue
        sec = cols[3] if len(cols) > 3 and cols[3].startswith(".") else ".text"
        if sec == ".rdata":
            sec = ".rodata"
        out.append(MatchedRow(start, size, cols[2] if len(cols) > 2 else "-", sec))
    return out


def build_ledger(dm: DataMap, data: bytes, matched: list[MatchedRow] = ()) -> tuple[list[Interval], list[MatchedRow]]:
    """Initialized-image ledger (ROM offsets). Returns (intervals, rejected overlapping rows)."""
    b = dm.boot
    led = Ledger(b.vram_to_rom(dm.image_start), b.vram_to_rom(dm.image_end))
    taken: list[tuple[int, int]] = []
    rejected = []
    for r in sorted(matched, key=lambda r: r.rom):
        a, z = r.rom, r.rom + r.size
        if not led.start <= a < z <= led.end or (taken and a < taken[-1][1]):
            rejected.append(r)
            continue
        cls = "code" if r.section == ".text" else "data"
        led.add(a, z, cls, category=r.section, kind="matched", name=r.source, evidence="matched_row")
        taken.append((a, z))
    starts = [t[0] for t in taken]

    def free_runs(a: int, z: int):
        i = max(bisect.bisect_right(starts, a) - 1, 0)
        cur = a
        for s, e in taken[i:]:
            if s >= z:
                break
            if e <= cur:
                continue
            if s > cur:
                yield cur, s
            cur = max(cur, e)
        if cur < z:
            yield cur, z

    for a, z, sec in dm.sections:
        ra, rz = b.vram_to_rom(a), b.vram_to_rom(z)
        if sec == ".text":
            for x, y in free_runs(ra, rz):
                led.add(x, y, "code", category=".text", kind="unmatched", name=f"text_{b.rom_to_vram(x):08X}", evidence="jal_cluster")
            continue
    for o in dm.objects:
        ra, rz = b.vram_to_rom(o.start), b.vram_to_rom(o.end)
        for x, y in free_runs(ra, rz):
            fill = set(data[x:y])
            if fill <= {0} and o.type == "unreferenced":
                continue  # left for padding fill
            owner = f"func_{o.owner:08X}" if o.owner is not None else ("shared" if o.owners else "-")
            led.add(x, y, "data", category=o.section, kind=o.type, name=f"D_{b.rom_to_vram(x):08X}",
                    evidence=f"owner={owner};access={','.join(o.accesses) or '-'}")
    return led.complete(data), rejected


# --------------------------------------------------------------------------- symbols

_HEX = re.compile(r"\b(?:0x)?([0-9A-Fa-f]{8})\b")
_IDENT = re.compile(r"\b([A-Za-z_][A-Za-z0-9_.$]*)\b")


def read_symbols(path: str | Path) -> dict[int, str]:
    """Generic symbol map: lines holding one identifier and one 8-digit hex address
    (``name = 0x80001234;``, ``80001234 T name``, ``{ name = "x", vram = 0x... }``)."""
    out: dict[int, str] = {}
    for line in Path(path).read_text(errors="replace").splitlines():
        line = line.split("//")[0]
        m = re.search(r'name\s*=\s*"([^"]+)".*?vram\s*=\s*(0x[0-9A-Fa-f]+)', line)
        if m:
            out.setdefault(int(m.group(2), 16), m.group(1))
            continue
        h = _HEX.search(line)
        if not h:
            continue
        rest = line[:h.start()] + " " + line[h.end():]
        names = [n for n in _IDENT.findall(rest) if len(n) > 1 and n not in ("size", "vram", "rom", "name") and not re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9A-Fa-f]+", n)]
        if names:
            out.setdefault(int(h.group(1), 16), names[-1] if len(names[-1]) > 1 and line.lstrip().startswith(h.group(0)) else names[0])
    return out


# --------------------------------------------------------------------------- typed units

def c_string(raw: bytes) -> str:
    out = []
    for ch in raw:
        if ch in (0x22, 0x5C):
            out.append("\\" + chr(ch))
        elif 0x20 <= ch < 0x7F:
            out.append(chr(ch))
        else:
            out.append("\\%03o" % ch)
    return '"' + "".join(out) + '"'


def float_literal(word: int) -> str | None:
    if word & 0x7F800000 in (0, 0x7F800000) and word & 0x7FFFFFFF:
        return None  # denormal, inf or nan: keep the raw word
    value = struct.unpack(">f", struct.pack(">I", word))[0]
    text = "%.9g" % value
    if struct.unpack(">I", struct.pack(">f", float(text)))[0] != word:
        return None
    if "e" not in text and "." not in text and "n" not in text:
        text += ".0"
    return text + "f"


def double_literal(raw: bytes) -> str | None:
    bits = struct.unpack(">Q", raw)[0]
    if (bits >> 52) & 0x7FF in (0, 0x7FF) and bits & 0x7FFFFFFFFFFFFFFF:
        return None
    text = repr(struct.unpack(">d", raw)[0])
    if struct.pack(">d", float(text)) != raw:
        return None
    return text


def type_evidence(dm: DataMap) -> dict[int, str]:
    """vram -> 'f32'/'f64' from floating-point accesses (hi/lo and register-offset)."""
    ev: dict[int, str] = {}
    for adr, refs in dm.xrefs.items():
        kinds = {x.access for x in refs}
        if kinds == {"f64"}:
            ev[adr] = "f64"
        elif kinds == {"f32"}:
            ev[adr] = "f32"
    return ev


def members(data: bytes, b: bootmod.BootInfo, a: int, z: int, evidence: dict[int, str],
            symbols: dict[int, str] | None = None, string_starts: set[int] | None = None):
    """Typed members for vram [a, z): (ctype, name, initializer, size, extern-or-None)."""
    out = []
    p = a
    align8 = a % 8 == 0
    rom = lambda v: b.vram_to_rom(v)
    while p < z:
        rel = p - a
        if p % 4:
            n = min(4 - p % 4, z - p)
            s = string_at(data, rom(p), rom(z))
            if s and (string_starts is None or p in string_starts or (out and out[-1][0] == "char")):
                n2 = b.rom_to_vram(s[0]) - p
                out.append(("char", f"text{rel:X}[{n2}]", c_string(data[rom(p):rom(p) + n2]), n2, None))
                p += n2
                continue
            out.append(("u8", f"pad{rel:X}[{n}]", "{ " + ", ".join("0x%02X" % x for x in data[rom(p):rom(p) + n]) + " }", n, None))
            p += n
            continue
        prev_text = bool(out) and out[-1][0] == "char"
        s = string_at(data, rom(p), rom(z)) if (string_starts is None or p in string_starts or prev_text) else None
        if s:
            n = b.rom_to_vram(s[0]) - p
            out.append(("char", f"text{rel:X}[{n}]", c_string(data[rom(p):rom(p) + n]), n, None))
            p += n
            continue
        if z - p >= 8 and evidence.get(p) == "f64" and p % 8 == 0 and align8:
            lit = double_literal(data[rom(p):rom(p) + 8])
            if lit:
                out.append(("f64", f"value{rel:X}", lit, 8, None))
                p += 8
                continue
        if z - p >= 4:
            w = _word(data, rom(p))
            if evidence.get(p) == "f32":
                lit = float_literal(w)
                if lit:
                    out.append(("f32", f"value{rel:X}", lit, 4, None))
                    p += 4
                    continue
            if symbols and w in symbols and w != 0:
                sym = symbols[w]
                out.append(("void *", f"ptr{rel:X}", sym, 4, sym))
                p += 4
                continue
            out.append(("u32", f"word{rel:X}", "0x%08X" % w, 4, None))
            p += 4
            continue
        n = z - p
        if n >= 2 and p % 2 == 0:
            for q in range(p, z - 1, 2):
                out.append(("u16", f"half{q - a:X}", "0x%04X" % struct.unpack_from(">H", data, rom(q))[0], 2, None))
            if (z - p) % 2:
                out.append(("u8", f"tail{z - 1 - a:X}", "0x%02X" % data[rom(z - 1)], 1, None))
        else:
            out.append(("u8", f"tail{rel:X}[{n}]", "{ " + ", ".join("0x%02X" % x for x in data[rom(p):rom(z)]) + " }", n, None))
        p = z
    return out


def render_unit(data: bytes, b: bootmod.BootInfo, a: int, z: int, section: str, evidence, symbols=None,
                header: str = '#include "types.h"', owner: str = "-", string_starts: set[int] | None = None) -> str:
    ms = members(data, b, a, z, evidence, symbols, string_starts)
    tname = ("Rodata" if section == ".rodata" else "Data") + f"{a:08X}"
    externs = sorted({m[4] for m in ms if m[4]})
    lines = [header, ""]
    lines += [f"extern u8 {e}[];" for e in externs]
    if externs:
        lines.append("")
    lines += [f"/* Initialized {section[1:]} at 0x{a:08X}..0x{z:08X} (ROM 0x{b.vram_to_rom(a):X}..0x{b.vram_to_rom(z):X}); consumer: {owner}. */",
              f"typedef struct {tname} {{"]
    lines += [f"    {t} {n};" if not t.endswith("*") else f"    {t}{n};" for t, n, _, _, _ in ms]
    lines += [f"}} {tname};", "", f"{'const ' if section == '.rodata' else ''}{tname} D_{a:08X} = {{"]
    lines += [f"    {init}," for _, _, init, _, _ in ms]
    lines += ["};", ""]
    return "\n".join(lines)


def emit_units(dm: DataMap, data: bytes, ledger: list[Interval], out_dir: str | Path,
               symbols: dict[int, str] | None = None, prefix: str = "src/data", per_owner: bool = True) -> list[dict]:
    """Write one C unit per contiguous unmatched .rodata/.data run (split into owner pools:
    consecutive objects with the same single consumer; unreferenced bytes join their
    neighbour), plus ``data_units.tsv``
    in the ``ROM_START SIZE SOURCE SECTION SHA256_DATA`` layout. Returns the unit rows."""
    b = dm.boot
    runs: list[list] = []
    for it in ledger:
        if it.cls != "data" or it.kind == "matched" or it.category not in (".rodata", ".data"):
            continue
        owner = it.evidence.split(";")[0].partition("=")[2] or "-"
        if runs and runs[-1][1] == it.start and runs[-1][2] == it.category and (
                not per_owner or owner in ("-", runs[-1][3]) or runs[-1][3] == "-"):
            runs[-1][1] = it.end
            if runs[-1][3] == "-":
                runs[-1][3] = owner
        else:
            runs.append([it.start, it.end, it.category, owner])
    ev = type_evidence(dm)
    starts = set(dm.xrefs) | {o.start for o in dm.objects}
    out_dir = Path(out_dir)
    rows = []
    for a, z, sec, owner in runs:
        va, vz = b.rom_to_vram(a), b.rom_to_vram(z)
        stem = ("rodata" if sec == ".rodata" else "data") + f"_{va:08X}"
        rel = f"{prefix}/{stem}.c"
        path = out_dir / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(render_unit(data, b, va, vz, sec, ev, symbols, owner=owner, string_starts=starts))
        rows.append({"owner": owner, "rom": a, "size": z - a, "source": rel, "section": sec,
                     "sha256": hashlib.sha256(data[a:z]).hexdigest()})
    with open(out_dir / "data_units.tsv", "w", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow(["# ROM_START", "SIZE", "SOURCE", "SECTION", "SHA256_DATA"])
        for r in rows:
            w.writerow([f"0x{r['rom']:X}", f"0x{r['size']:X}", r["source"], r["section"], r["sha256"]])
    return rows


# --------------------------------------------------------------------------- validation

def code_ranges(dm: DataMap, code_rows: list[MatchedRow]) -> dict[str, list[tuple[int, int]]]:
    """Source -> code vram ranges from matched code rows."""
    out: dict[str, list[tuple[int, int]]] = {}
    b = dm.boot
    for r in code_rows:
        if r.section == ".text":
            out.setdefault(r.source, []).append((b.rom_to_vram(r.rom), b.rom_to_vram(r.rom + r.size)))
    return out


def ownership_agreement(dm: DataMap, data_rows: list[tuple[int, int, str]],
                        ranges: dict[str, list[tuple[int, int]]]) -> dict:
    """Compare predicted consumers of ground-truth data rows ``(vram_start, vram_end, source)``
    with the code ranges of that source.

    A row is evaluable when its source has code. ``any`` counts rows where at least one
    predicted consumer lies in that source; ``exact`` where every consumer does.
    """
    starts = [o.start for o in dm.objects]
    res = {"rows": 0, "evaluable": 0, "no_prediction": 0, "any": 0, "exact": 0, "mismatches": []}
    for va, vz, source in data_rows:
        res["rows"] += 1
        rng = ranges.get(source)
        if not rng:
            continue
        res["evaluable"] += 1
        i = max(bisect.bisect_right(starts, va) - 1, 0)
        owners = set()
        while i < len(dm.objects) and dm.objects[i].start < vz:
            o = dm.objects[i]
            if o.end > va:
                owners |= set(o.owners)
            i += 1
        if not owners:
            res["no_prediction"] += 1
            res["mismatches"].append(f"0x{va:08X} {source}: no reference found")
            continue
        inside = {f for f in owners if any(x <= f < y for x, y in rng)}
        if inside:
            res["any"] += 1
        if inside == owners:
            res["exact"] += 1
        else:
            res["mismatches"].append(f"0x{va:08X} {source}: also/only {sorted(hex(f) for f in owners - inside)[:4]}")
    return res


def summary(dm: DataMap, ledger: list[Interval] | None = None) -> dict:
    b = dm.boot
    sec_tot: dict[str, int] = {}
    for a, z, s in dm.sections:
        sec_tot[s] = sec_tot.get(s, 0) + z - a
    types: dict[str, int] = {}
    for o in dm.objects:
        types[o.type] = types.get(o.type, 0) + 1
    owned = [o for o in dm.objects if o.owners]
    out = {
        "image": f"0x{dm.image_start:08X}..0x{dm.image_end:08X} (ROM 0x{b.vram_to_rom(dm.image_start):X}..0x{b.vram_to_rom(dm.image_end):X})",
        "bss": f"0x{b.bss_start:08X}..0x{b.bss_end:08X}" if b.bss_start else None,
        "sections": [f"{s} 0x{a:08X}..0x{z:08X} (0x{z - a:X})" for a, z, s in dm.sections],
        "section_bytes": {k: hex(v) for k, v in sec_tot.items()},
        "functions": len(dm.functions),
        "xref_sites": sum(len(v) for v in dm.xrefs.values()),
        "data_addresses": len(dm.xrefs),
        "objects": len(dm.objects),
        "object_types": types,
        "single_owner_objects": sum(1 for o in owned if len(o.owners) == 1),
        "shared_objects": sum(1 for o in owned if len(o.owners) > 1),
        "jump_tables": len(dm.jump_tables),
        "strings": len(dm.strings),
    }
    if ledger is not None:
        out["ledger_class"] = {k: hex(v) for k, v in Ledger.totals(ledger).items()}
        kinds: dict[str, int] = {}
        for it in ledger:
            if it.cls in ("code", "data"):
                k = "matched" if it.kind == "matched" else "unmatched"
                kinds[f"{it.cls}_{k}"] = kinds.get(f"{it.cls}_{k}", 0) + it.size
        out["ledger_matched"] = {k: hex(v) for k, v in sorted(kinds.items())}
    return out
