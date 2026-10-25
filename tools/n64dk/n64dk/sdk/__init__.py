"""SDK identification: libultra/libaudio/libgcc signature matching, RSP microcode, audio
library and compiler fingerprints.

``analyse(rom)`` runs everything over the main code segment and returns one report.
"""
from __future__ import annotations

import struct
from collections import Counter
from dataclasses import dataclass, field

from .. import boot, mips
from .db import Database
from .fingerprint import compiler_verdict, find_audio, find_libmus, find_microcode, microcode_symbols
from .matcher import Match, scan, version_candidates


@dataclass
class SdkReport:
    region: tuple[int, int, int]                 # rom start, rom end, vram start
    matches: list[Match]
    versions: list[dict]
    microcode: list
    audio: list
    libmus: list
    compiler: dict
    library_compiler: dict = field(default_factory=dict)

    def summary(self) -> dict:
        return {
            "functions_named": len(self.matches),
            "high_confidence": sum(m.confidence >= 0.8 for m in self.matches),
            "sdk_version": version_summary(self.versions),
            "library_compiler": self.library_compiler,
            "game_compiler": self.compiler.get("verdict"),
            "game_compiler_share": self.compiler.get("share"),
            "microcode": [" ".join(filter(None, (u.family or "?", u.version, u.variant))) + f" @0x{u.rom:X}"
                          for u in self.microcode],
            "audio": dict(Counter(a.kind for a in self.audio)),
            "libmus": sorted({s for _o, s in self.libmus}),
        }


def version_summary(versions: list[dict]) -> list[str]:
    """Builds within the candidate margin of the best score (``2.0I:libultra_rom`` style)."""
    return [v["build"].split(":", 1)[1] for v in versions if v.get("candidate")]


def code_region(rom) -> tuple[int, int, int]:
    """The IPL3-loaded main segment: (rom start, rom end, vram start)."""
    b = boot.analyse(rom)
    return b.main_rom, min(len(rom.data), b.main_rom + b.copy_size), b.main_vram


def function_spans(data: bytes, start: int, end: int, vram: int, extra_starts=()) -> list[tuple[int, list[int]]]:
    """Rough function bodies: call targets (plus ``extra_starts``) up to the first ``jr $ra`` and its delay slot."""
    code = data[start:end]
    starts = {t for t in mips.call_targets(code, vram) if vram <= t < vram + len(code)}
    starts |= {s for s in extra_starts if vram <= s < vram + len(code)}
    ordered = sorted(starts)
    out = []
    for i, s in enumerate(ordered):
        limit = min((ordered[i + 1] if i + 1 < len(ordered) else vram + len(code)) - vram, len(code))
        off, words = s - vram, []
        while off + 4 <= limit:
            w, = struct.unpack_from(">I", code, off)
            words.append(w)
            off += 4
            if w == mips.JR_RA:
                if off + 4 <= len(code):
                    words.append(struct.unpack_from(">I", code, off)[0])
                out.append((s, words))
                break
    return out


def analyse(rom, db: Database | None = None, region: tuple[int, int, int] | None = None,
            min_confidence: float = 0.5, program=None) -> SdkReport:
    """``program`` (an :class:`n64dk.image.ProgramImage`) selects the code segment; with a
    packed segment, match offsets are offsets into the unpacked segment.  Microcode and
    audio fingerprints always scan the whole ROM."""
    db = db if db is not None else Database.load()
    code = rom.data
    if program is not None and region is None:
        b, code = program.boot_info(), program.main_data(rom)
        region = (b.main_rom, min(len(code), b.main_rom + b.copy_size), b.main_vram)
    start, end, vram = region or code_region(rom)
    matches = [m for m in scan(code, start, end, vram, db) if m.confidence >= min_confidence]
    versions = version_candidates(matches, db)
    lib = Counter(c for m in matches if m.kind == "exact" and m.confidence >= 0.8 and len(m.compilers) == 1
                  for c in m.compilers)
    library_vrams = {m.vram for m in matches}
    spans = function_spans(code, start, end, vram, library_vrams)
    game = [(v, w) for v, w in spans if v not in library_vrams]
    verdict = compiler_verdict(db.model, game)
    return SdkReport((start, end, vram), matches, versions, find_microcode(rom.data), find_audio(rom.data),
                     find_libmus(rom.data), verdict, dict(lib.most_common()))


__all__ = ["Database", "SdkReport", "analyse", "scan", "version_candidates", "version_summary", "code_region",
           "function_spans", "find_microcode", "find_audio", "find_libmus", "microcode_symbols",
           "compiler_verdict"]
