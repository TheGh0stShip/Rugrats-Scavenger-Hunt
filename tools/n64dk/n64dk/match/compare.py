"""Relocated comparison of compiled objects against the retail ROM.

The object is relocated at its retail address with :meth:`n64dk.elf.Elf.relocate`; section
bases and external symbols that no symbol map provides can be recovered from the ROM itself:
every ``%hi/%lo`` or ``jal`` relocation in the compiled ``.text`` is paired with the
reference decoded at the same offset of the retail function, and a majority vote gives the
base of ``.rodata``/``.data``/``.bss`` or the address of the symbol.  ``.rodata`` candidates
are also located by searching the ROM for the emitted constant pool.
"""
from __future__ import annotations

import struct
from collections import Counter
from dataclasses import dataclass, field

from .. import mips
from ..elf import R_MIPS_26, R_MIPS_32, R_MIPS_HI16, R_MIPS_LO16, Elf
from . import diff as diffmod
from .symbols import SymbolMap

ALIASES = {".rdata": ".rodata", ".rodata": ".rdata", ".sdata": ".data"}
COMPARED = (".rodata", ".data")


def find_section(elf: Elf, name: str):
    sec = elf.section(name)
    if (sec is None or sec.size == 0) and name in ALIASES:
        alt = elf.section(ALIASES[name])
        if alt is not None and alt.size:
            return alt
    return sec


def _section_bases(elf: Elf, bases: dict[str, int]) -> dict[str, int]:
    out = dict(bases)
    for k, v in bases.items():
        if k in (".rdata", ".rodata"):
            out.setdefault(ALIASES[k], v)
    return out


@dataclass
class Relocation:
    offset: int
    type: int
    symbol: str
    section: str | None   # defining section of the symbol (None if external)
    value: int            # symbol value inside its section
    addend: int           # REL addend from the instruction (low half for HI/LO)


def relocations(elf: Elf, section: str) -> list[Relocation]:
    sec = find_section(elf, section)
    if sec is None:
        return []
    out = []
    for off, rtype, sidx, addend in elf.relocations(sec):
        sym = elf.symbols[sidx]
        defined = sym.section not in (0, 0xFFF1, 0xFFF2) and sym.section < len(elf.sections)
        sname = elf.sections[sym.section].name if defined else None
        word, = struct.unpack_from(">I", sec.data, off)
        if addend is None:
            if rtype in (R_MIPS_HI16, R_MIPS_LO16):
                addend = (word & 0xFFFF) - (0x10000 if word & 0x8000 and rtype == R_MIPS_LO16 else 0)
            elif rtype == R_MIPS_26:
                addend = (word & 0x3FFFFFF) << 2
            elif rtype == R_MIPS_32:
                addend = word
            else:
                addend = 0
        name = sym.name or (sname or "")
        out.append(Relocation(off, rtype, name, sname, sym.value, addend))
    return out


def infer_from_rom(elf: Elf, target: bytes, vram: int, bases: dict[str, int],
                   symbols: SymbolMap, externals: bool = True) -> tuple[dict[str, int], dict[str, int]]:
    """Vote section bases and unresolved external addresses from the retail code ``target``.

    HI16/LO16 pairs are read straight from the retail words at the relocated offsets (this
    also covers indexed accesses such as jump-table loads that a register tracker loses);
    lone LO16s fall back to decoded references and R_MIPS_26 to the retail jump target."""
    def rom_word(off: int) -> int | None:
        return struct.unpack_from(">I", target, off)[0] if 0 <= off <= len(target) - 4 else None

    refs = {r.site: r for r in mips.scan_refs(target, vram) if r.kind not in ("call", "jump")}
    sec_votes: dict[str, Counter] = {}
    sym_votes: dict[str, Counter] = {}
    have = _section_bases(elf, bases)
    pending: dict[tuple, list[Relocation]] = {}

    def wanted(rel: Relocation) -> str | None:
        if rel.section is not None and rel.section != ".text" and rel.section not in have:
            return "section"
        if rel.section is None and externals and rel.symbol and rel.symbol not in symbols:
            return "symbol"
        return None

    def vote(rel: Relocation, address: int, addend: int) -> None:
        kind = wanted(rel)
        if kind == "section":
            sec_votes.setdefault(rel.section, Counter())[(address - rel.value - addend) & 0xFFFFFFFF] += 1
        elif kind == "symbol":
            sym_votes.setdefault(rel.symbol, Counter())[(address - addend) & 0xFFFFFFFF] += 1

    for rel in relocations(elf, ".text"):
        key = (rel.symbol, rel.section, rel.value)
        if rel.type == R_MIPS_HI16:
            pending.setdefault(key, []).append(rel)
        elif rel.type == R_MIPS_LO16:
            his = pending.pop(key, [])
            lo_word = rom_word(rel.offset)
            if lo_word is None or wanted(rel) is None:
                continue
            lo = (lo_word & 0xFFFF) - (0x10000 if lo_word & 0x8000 else 0)
            for hi in his:
                hi_word = rom_word(hi.offset)
                if hi_word is not None and hi_word >> 26 == 0x0F:  # lui
                    vote(rel, ((hi_word & 0xFFFF) << 16) + lo, (hi.addend << 16) + rel.addend)
            if not his and vram + rel.offset in refs:
                vote(rel, refs[vram + rel.offset].address, rel.addend)
        elif rel.type == R_MIPS_26 and wanted(rel) == "symbol":
            word = rom_word(rel.offset)
            if word is not None:
                vote(rel, ((word & 0x3FFFFFF) << 2) | (vram & 0xF0000000), rel.addend)
    new_bases = {s: c.most_common(1)[0][0] for s, c in sec_votes.items()}
    new_syms = {s: c.most_common(1)[0][0] for s, c in sym_votes.items()}
    return new_bases, new_syms


def complete_bases(elf: Elf, section: str, bases: dict[str, int], symbols: SymbolMap) -> dict[str, int]:
    """Give sections referenced by ``section``'s relocations a base derived from a named,
    resolvable symbol they define (the address the project's linker gave it)."""
    out = _section_bases(elf, bases)
    for rel in relocations(elf, section):
        if rel.section and rel.section not in out and rel.symbol and rel.symbol != rel.section:
            addr = symbols.resolve(rel.symbol)
            if addr is not None:
                out[rel.section] = addr - rel.value
    return out


def relocate(elf: Elf, section: str, bases: dict[str, int], symbols: SymbolMap) -> bytes:
    """Section bytes with relocations applied at the given bases (see :meth:`Elf.relocate`)."""
    sec = find_section(elf, section)
    if sec is None:
        return b""
    return elf.relocate(sec.name, complete_bases(elf, sec.name, bases, symbols), symbols)


def section_candidates(elf: Elf, name: str, rom: bytes, rom_lo: int, rom_hi: int, to_vram,
                       near: int = 0, limit: int = 32) -> list[int]:
    """VRAM addresses where section ``name`` of the object occurs in the ROM (words carrying
    relocations masked, trailing zero padding ignored), nearest to ``near`` first."""
    sec = find_section(elf, name)
    if sec is None or not sec.size:
        return []
    data = bytearray(sec.data)
    # relocated words keep the bits the relocation does not touch (opcode and registers)
    keep = {R_MIPS_HI16: 0xFFFF0000, R_MIPS_LO16: 0xFFFF0000, R_MIPS_26: 0xFC000000}
    masked = {r.offset: keep.get(r.type, 0) for r in relocations(elf, sec.name)}
    end = len(data)
    while end >= 4 and data[end - 4:end] == b"\0\0\0\0" and end - 4 not in masked:
        end -= 4
    if end == 0:
        return []
    words = [struct.unpack_from(">I", data, off)[0] for off in range(0, end, 4)]
    runs, start = [], 0
    for off in range(0, end, 4):
        if off in masked:
            runs.append((start, off))
            start = off + 4
    runs.append((start, end))
    key_lo, key_hi = max(runs, key=lambda r: r[1] - r[0])
    if key_hi - key_lo < 4:
        return []
    key = bytes(data[key_lo:key_hi])
    hits = []
    pos = rom.find(key, rom_lo)
    while pos != -1 and pos < rom_hi:
        cand = pos - key_lo
        if cand >= rom_lo and cand % 4 == 0 and cand + end <= len(rom):
            ok = True
            for i, word in enumerate(words):
                mask = masked.get(4 * i, 0xFFFFFFFF)
                if mask and (struct.unpack_from(">I", rom, cand + 4 * i)[0] ^ word) & mask:
                    ok = False
                    break
            if ok:
                hits.append(to_vram(cand))
        pos = rom.find(key, pos + 1)
    hits.sort(key=lambda v: abs(v - near))
    return hits[:limit]


def rodata_candidates(elf: Elf, rom: bytes, rom_lo: int, rom_hi: int, to_vram, text_vram: int,
                      limit: int = 32) -> list[int]:
    return section_candidates(elf, ".rodata", rom, rom_lo, rom_hi, to_vram, text_vram, limit)


def locate_text(elf: Elf, rom: bytes, rom_lo: int, rom_hi: int, to_vram) -> list[int]:
    """Candidate VRAM addresses of the object's ``.text`` in the ROM (tiny functions whose
    unrelocated words are common can have many)."""
    return section_candidates(elf, ".text", rom, rom_lo, rom_hi, to_vram, limit=1024)


@dataclass
class SectionCompare:
    name: str
    vram: int
    size: int
    match: bool
    first_diff: int | None = None
    note: str = ""


@dataclass
class Result:
    exact: bool
    text: SectionCompare
    sections: list[SectionCompare] = field(default_factory=list)
    bases: dict[str, int] = field(default_factory=dict)
    inferred_symbols: dict[str, int] = field(default_factory=dict)
    diff: diffmod.Report | None = None
    compiled_size: int = 0
    error: str | None = None

    def as_dict(self) -> dict:
        out = {"exact": self.exact, "text": vars(self.text), "compiled_text_size": self.compiled_size,
               "sections": [vars(s) for s in self.sections],
               "bases": {k: f"0x{v:08X}" for k, v in self.bases.items()}}
        if self.inferred_symbols:
            out["inferred_symbols"] = {k: f"0x{v:08X}" for k, v in self.inferred_symbols.items()}
        if self.diff is not None:
            out["diff"] = self.diff.as_dict()
        if self.error:
            out["error"] = self.error
        return out


def _first_diff(a: bytes, b: bytes) -> int | None:
    for i, (x, y) in enumerate(zip(a, b)):
        if x != y:
            return i
    return None if len(a) == len(b) else min(len(a), len(b))


def compare_object(elf: Elf, rom: bytes, vram_to_rom, text_vram: int, size: int | None,
            symbols: SymbolMap, bases: dict[str, int] | None = None, infer: bool = True,
            infer_externals: bool = True, rodata_search: bool = True, with_diff: bool = True,
            rom_window: tuple[int, int] | None = None, to_vram=None,
            compare_sections: bool = True, section_sizes: dict[str, int] | None = None) -> Result:
    """Relocate ``elf`` at ``text_vram`` and compare its ``.text`` (``size`` bytes, compiled
    tail must be zero padding) and, where a base is known, its ``.rodata``/``.data``."""
    bases = dict(bases or {})
    bases.setdefault(".text", text_vram)
    text_sec = find_section(elf, ".text")
    compiled_size = text_sec.size if text_sec else 0
    if size is None:
        size = compiled_size
    rom_off = vram_to_rom(text_vram)
    target = rom[rom_off:rom_off + size]
    syms = symbols.copy()
    inferred: dict[str, int] = {}
    if infer:
        new_bases, new_syms = infer_from_rom(elf, target, text_vram, bases, syms, infer_externals)
        for k, v in new_bases.items():
            bases.setdefault(k, v)
        inferred = new_syms
        syms.update(new_syms)
    candidates: list[int] = []
    rod = find_section(elf, ".rodata")
    rod_key = rod.name if rod is not None else ".rodata"
    if rodata_search and rod is not None and rod.size and rod_key not in bases and ".rodata" not in bases \
            and to_vram is not None and rom_window is not None:
        candidates = rodata_candidates(elf, rom, rom_window[0], rom_window[1], to_vram, text_vram)
    attempts = [None] + candidates if not candidates else candidates
    best: Result | None = None
    for cand in attempts:
        trial = dict(bases)
        if cand is not None:
            trial[rod_key] = cand
        try:
            text = relocate(elf, ".text", trial, syms)
            error = None
        except (KeyError, ValueError) as exc:
            text, error = (text_sec.data if text_sec else b""), f"relocation: {exc}"
        body = text[:size]
        tail_ok = not any(text[size:])
        exact = error is None and body == target and len(body) == size and tail_ok
        note = "" if tail_ok else "nonzero bytes after expected size"
        res = Result(exact, SectionCompare(".text", text_vram, size, exact, _first_diff(body, target), note),
                     bases=trial, inferred_symbols=inferred, compiled_size=len(text), error=error)
        if best is None or (res.exact and not best.exact) or (
                not best.exact and res.text.first_diff is not None and best.text.first_diff is not None
                and res.text.first_diff > best.text.first_diff):
            best = res
        if exact:
            break
    assert best is not None
    if compare_sections:
        for name in COMPARED:
            sec = find_section(elf, name)
            if sec is None or not sec.size:
                continue
            base = best.bases.get(sec.name, best.bases.get(name))
            if base is None:
                continue
            try:
                data = relocate(elf, sec.name, best.bases, syms)
            except (KeyError, ValueError) as exc:
                best.sections.append(SectionCompare(name, base, sec.size, False, None, f"relocation: {exc}"))
                best.exact = False
                continue
            limit = (section_sizes or {}).get(name)
            if limit is not None:
                data = data[:limit]
            off = vram_to_rom(base)
            want = rom[off:off + len(data)]
            used = len(data)
            ok = data == want
            if not ok:  # tolerate trailing alignment padding that the retail image does not keep
                stripped = data.rstrip(b"\0")
                used = (len(stripped) + 3) & ~3
                ok = data[:used] == want[:used]
            sc = SectionCompare(name, base, used, ok, None if ok else _first_diff(data, want))
            best.sections.append(sc)
            best.exact = best.exact and ok
    if with_diff and not best.exact:
        trial_text = None
        try:
            trial_text = relocate(elf, ".text", best.bases, syms)
        except (KeyError, ValueError):
            trial_text = text_sec.data if text_sec else b""
        used = max(size, (len(trial_text.rstrip(b"\0")) + 3) & ~3)
        best.diff = diffmod.diff(trial_text[:used], target, text_vram)
    return best
