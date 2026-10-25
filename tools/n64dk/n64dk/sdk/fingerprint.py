"""Fingerprints: RSP microcode, audio library data and per-function compiler idioms."""
from __future__ import annotations

import math
import re
import struct
from collections import Counter
from dataclasses import dataclass, field

# --------------------------------------------------------------------------- microcode

_UCODE_RE = re.compile(rb"RSP (?:Gfx|SW|Audio)[ -~]{4,120}")
_FAMILY_RE = re.compile(r"\b(F3DEX2|F3DEX|F3DLX2|F3DLX|F3DLP|F3DZEX2|F3DZEX|F3D|L3DEX2|L3DEX|S2DEX2|S2DEX|L3D|Turbo3D|Fast3D)\b",
                        re.IGNORECASE)
_VERSION_RE = re.compile(r"(?:fifo|xbus|dram|NoN)?\s*(\d+\.\d+[a-z]?)")
_UCODE_NAMES = (b"aspMain", b"gspF3DEX", b"gspF3DEX2", b"gspS2DEX", b"gspL3DEX", b"gspTurbo3D", b"gspFast3D",
                b"n_aspMain", b"rspboot")


@dataclass
class Microcode:
    rom: int
    text: str
    family: str | None
    version: str | None
    variant: str | None
    data_rom: int | None = None   # start of the enclosing RSP data block (estimate)
    text_range: tuple[int, int] | None = None


_RSP_OPS = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x12,
            0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2B, 0x32, 0x3A}


def _rsp_block(words: tuple[int, ...]) -> bool:
    vec = sum(1 for w in words if (w >> 26) in (0x12, 0x32, 0x3A))  # COP2, LWC2, SWC2
    valid = sum(1 for w in words if (w >> 26) in _RSP_OPS)
    zeros = sum(1 for w in words if w == 0)
    return vec >= len(words) * 0.15 and valid >= len(words) * 0.95 and zeros < len(words) // 3


def rsp_text_ranges(data: bytes, start: int = 0, end: int | None = None, block: int = 0x40,
                    min_size: int = 0x200, gap: int = 0x200) -> list[tuple[int, int]]:
    """Regions of RSP code: only RSP-legal opcodes and a high share of vector (COP2/LWC2/SWC2) ops.

    CPU code never contains COP2/LWC2/SWC2, so this separates RSP text from VR4300 code.
    """
    end = min(len(data), end or len(data))
    out: list[list[int]] = []
    for off in range(start - start % block, end - block + 1, block):
        if _rsp_block(struct.unpack_from(f">{block // 4}I", data, off)):
            if out and off - out[-1][1] <= gap:
                out[-1][1] = off + block
            else:
                out.append([off, off + block])
    return [(a, b) for a, b in out if b - a >= min_size]


def find_microcode(data: bytes) -> list[Microcode]:
    texts = rsp_text_ranges(data)
    found = []
    for m in _UCODE_RE.finditer(data):
        text = m.group().decode("ascii", "replace").strip()
        fam = _FAMILY_RE.search(text)
        ver = _VERSION_RE.search(text[fam.end():] if fam else text)
        variant = None
        for v in ("fifo", "xbus", "dram", "NoN", "Rej"):
            if re.search(rf"\b{v}\b", text, re.IGNORECASE):
                variant = v.lower()
        # The ucode data block is 16-byte aligned and its text immediately
        # precedes or follows; take the nearest RSP text range.
        near = min(texts, key=lambda r: min(abs(r[0] - m.start()), abs(r[1] - m.start())), default=None)
        if near and min(abs(near[0] - m.start()), abs(near[1] - m.start())) > 0x200000:
            near = None
        found.append(Microcode(m.start(), text, fam.group(1).upper() if fam else None,
                               ver.group(1) if ver else None, variant,
                               data_rom=_data_block_start(data, m.start()), text_range=near))
    return found


def _data_block_start(data: bytes, pos: int, limit: int = 0x400) -> int | None:
    """Walk back from a ucode string to the run of zero padding preceding its data block."""
    p = pos - pos % 16
    lo = max(0, pos - limit)
    while p - 16 >= lo:
        if p >= 32 and data[p - 32:p] == bytes(32):
            return p
        p -= 16
    return None


def microcode_symbols(data: bytes) -> list[tuple[int, str]]:
    out = []
    for name in _UCODE_NAMES:
        for m in re.finditer(re.escape(name) + rb"[A-Za-z0-9_]*\0", data):
            out.append((m.start(), m.group()[:-1].decode()))
    return sorted(set(out))


# --------------------------------------------------------------------------- audio

@dataclass
class AudioFile:
    rom: int
    kind: str      # "ALBankFile" | "ALSeqFile"
    count: int
    size_hint: int | None = None


def _plausible_bank(data: bytes, base: int, off: int) -> bool:
    p = base + off
    if off <= 0 or off % 4 or p + 12 > len(data):
        return False
    inst_count, flags, _pad, rate = struct.unpack_from(">HBBi", data, p)
    return 0 < inst_count <= 128 and flags in (0, 1) and 4000 <= rate <= 48000 or (
        0 < inst_count <= 128 and flags in (0, 1) and rate == 0)


def find_audio(data: bytes, limit: int = 256) -> list[AudioFile]:
    out = []
    for m in re.finditer(rb"B1", data):
        p = m.start()
        if p % 4 or p + 8 > len(data):
            continue
        count, = struct.unpack_from(">H", data, p + 2)
        if not 0 < count <= 64:
            continue
        offs = struct.unpack_from(f">{count}I", data, p + 4) if p + 4 + 4 * count <= len(data) else ()
        if offs and all(_plausible_bank(data, p, o) for o in offs):
            out.append(AudioFile(p, "ALBankFile", count))
            if len(out) >= limit:
                break
    for m in re.finditer(rb"S1", data):
        p = m.start()
        if p % 4 or p + 4 > len(data):
            continue
        count, = struct.unpack_from(">H", data, p + 2)
        if not 0 < count <= 1024 or p + 4 + 8 * count > len(data):
            continue
        pairs = struct.unpack_from(f">{2 * count}I", data, p + 4)
        offs, lens = pairs[0::2], pairs[1::2]
        header = 4 + 8 * count
        if (all(header <= o < 0x1000000 and 0 <= l < 0x200000 for o, l in zip(offs, lens))
                and list(offs) == sorted(offs) and any(lens)):
            out.append(AudioFile(p, "ALSeqFile", count, max(o + l for o, l in zip(offs, lens))))
            if len(out) >= 2 * limit:
                break
    return out


def find_libmus(data: bytes) -> list[tuple[int, str]]:
    return [(m.start(), m.group().decode().strip()) for m in re.finditer(rb"N64 (?:PtrTablesV2|WaveTables|SoundTables)", data)]


# --------------------------------------------------------------------------- compiler idioms

FEATURES = ("move_or", "move_addu", "move_daddu", "branch_likely", "lui_at", "at_other", "ra_top", "ra_mid",
            "sd_save", "jr_nop", "teq", "brk7", "fcmp_nop", "sp_delay", "leaf", "sw_args")


def _fields(w):
    return w >> 26, (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31, w & 0x3F


def function_features(words) -> dict[str, bool]:
    """Binary idiom features of one function's instruction words."""
    f = dict.fromkeys(FEATURES, False)
    n = len(words)
    frame = None
    for i, w in enumerate(words):
        op, rs, rt, rd, fn = _fields(w)
        if op == 0:
            if rt == 0 and rd and fn == 0x25:
                f["move_or"] = True
            elif rt == 0 and rd and fn == 0x21:
                f["move_addu"] = True
            elif rt == 0 and rd and fn == 0x2D:
                f["move_daddu"] = True
            elif fn == 0x34:
                f["teq"] = True
            elif fn == 0x0D and (w >> 6) & 0xFFFFF in (7, 7 << 10):
                f["brk7"] = True
            elif w == 0x03E00008 and i + 1 < n:
                nxt = words[i + 1]
                if nxt == 0:
                    f["jr_nop"] = True
                elif nxt >> 16 == 0x27BD:
                    f["sp_delay"] = True
        if op in (0x14, 0x15, 0x16, 0x17) or (op == 1 and rt in (2, 3, 0x12, 0x13)) or (
                op == 0x11 and rs == 8 and rt & 2):
            f["branch_likely"] = True
        if op == 0x0F and rt == 1:
            f["lui_at"] = True
        elif op != 0x0F and (rs == 1 or rt == 1) and op not in (2, 3) and not (op == 0 and fn in (0, 2, 3)):
            if op in (0, 8, 9, 0x0A, 0x0B, 0x0C, 0x0D, 4, 5) or op >= 0x20:
                f["at_other"] = True
        if w >> 16 == 0x27BD and frame is None and i < 3:
            imm = w & 0xFFFF
            if imm & 0x8000:
                frame = 0x10000 - imm
        if frame and i < 12 and op in (0x2B, 0x3F) and rs == 29 and rt == 31:
            off = w & 0xFFFF
            if op == 0x3F:
                f["sd_save"] = True
            if off == frame - 4 or (op == 0x3F and off == frame - 8):
                f["ra_top"] = True
            else:
                f["ra_mid"] = True
        if frame and i < 12 and op == 0x2B and rs == 29 and 4 <= rt <= 7 and (w & 0xFFFF) >= frame:
            f["sw_args"] = True
        if op == 0x11 and rs in (16, 17) and (fn & 0x30) == 0x30 and i + 2 < n:
            if words[i + 1] == 0 and (words[i + 2] >> 21) == (0x11 << 5 | 8):
                f["fcmp_nop"] = True
    f["leaf"] = frame is None
    return f


@dataclass
class CompilerModel:
    """Naive-Bayes rates per compiler class, trained on compiled library C."""
    counts: dict[str, dict[str, int]] = field(default_factory=dict)
    totals: dict[str, int] = field(default_factory=dict)

    def add(self, compiler: str, feats: dict[str, bool]) -> None:
        c = self.counts.setdefault(compiler, dict.fromkeys(FEATURES, 0))
        self.totals[compiler] = self.totals.get(compiler, 0) + 1
        for k, v in feats.items():
            if v:
                c[k] = c.get(k, 0) + 1

    def classes(self) -> list[str]:
        return [c for c, n in self.totals.items() if n >= 5]

    def score(self, feats: dict[str, bool]) -> dict[str, float]:
        """Posterior per class (uniform prior)."""
        logp = {}
        for c in self.classes():
            n = self.totals[c]
            lp = 0.0
            for k in FEATURES:
                p = (self.counts[c].get(k, 0) + 1) / (n + 2)
                lp += math.log(p if feats.get(k) else 1 - p)
            logp[c] = lp
        if not logp:
            return {}
        top = max(logp.values())
        z = sum(math.exp(v - top) for v in logp.values())
        return {c: math.exp(v - top) / z for c, v in logp.items()}

    def to_json(self) -> dict:
        return {"counts": self.counts, "totals": self.totals}

    @classmethod
    def from_json(cls, d: dict | None) -> "CompilerModel":
        d = d or {}
        return cls(d.get("counts", {}), d.get("totals", {}))


def compiler_verdict(model: CompilerModel, functions: list[tuple[int, list[int]]],
                     informative_min: int = 6) -> dict:
    """Classify each function (``(vram, words)``) and aggregate an overall verdict."""
    per = []
    tally: Counter = Counter()
    for vram, words in functions:
        if len(words) < informative_min:
            continue
        post = model.score(function_features(words))
        if not post:
            continue
        best = max(post, key=post.get)
        per.append({"vram": vram, "compiler": best, "p": round(post[best], 3)})
        if post[best] >= 0.8:
            tally[best] += 1
    total = sum(tally.values())
    verdict = tally.most_common(1)[0][0] if tally else None
    return {"verdict": verdict, "share": {k: round(v / total, 3) for k, v in tally.most_common()} if total else {},
            "classified": total, "functions": per}
