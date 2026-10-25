"""N64 audio data: libaudio banks/sequences, VADPCM, MIDI, libmus and seqfile-style sound banks.

Formats recognised purely from ROM structure (no game tables):

* libultra ``ALBankFile`` (.ctl, revision ``'B1'``) with full struct walk (banks, instruments,
  sounds, envelopes, keymaps, wavetables, ADPCM books and loops) and byte-exact coverage;
  the matching sample table (.tbl) is located by scoring candidate bases with VADPCM frame
  headers, then every sample interval and the padding between them is accounted for.
* libultra ``ALSeqFile`` (``'S1'``) whose entries are Standard MIDI or compact MIDI.
* Standard MIDI files (``MThd``) with event-level validation and alignment padding.
* Software Creations libmus stores (``N64 PtrTablesV2`` pointer banks, ``N64 WaveTables``
  sample banks, effect banks and version-0x215 songs), framed by a cartridge file directory
  when one references the detected files.
* Nintendo "seqfile" containers (revision 1 = ctl, 2 = tbl, 3 = sequences) as used by
  Super Mario 64 / Mario Kart 64, with instrument/drum/sample/book/loop/envelope walk and
  the bank-set table that follows the sequence file.

The libaudio bank parser generalises an earlier GPL asset inventory tool
``tools/asset_inventory/audio_banks.py`` and the libmus parser generalises the Global
Assault ``tools/inventory_libmus_assets.py`` / ``decode_libmus_wave.py`` (both GPL-3.0).
"""
from __future__ import annotations

import hashlib
import io
import re
import struct
import wave as _wave
from dataclasses import dataclass, field

from ..ledger import Interval

CART = 0xB0000000
CATEGORY = "audio"


class AudioError(ValueError):
    pass


def _u16(b, o): return struct.unpack_from(">H", b, o)[0]
def _s16(b, o): return struct.unpack_from(">h", b, o)[0]
def _u32(b, o): return struct.unpack_from(">I", b, o)[0]
def _s32(b, o): return struct.unpack_from(">i", b, o)[0]
def _align(x, a): return (x + a - 1) // a * a


def _require(cond, msg):
    if not cond:
        raise AudioError(msg)


# --------------------------------------------------------------------------------------
# VADPCM
# --------------------------------------------------------------------------------------

@dataclass
class Book:
    order: int
    npredictors: int
    coefs: list[int]          # order * npredictors * 8 signed 16-bit values

    @classmethod
    def read(cls, b: bytes, off: int) -> "Book":
        order, npred = struct.unpack_from(">ii", b, off)
        _require(1 <= order <= 8 and 1 <= npred <= 16, f"bad ADPCM book at 0x{off:X}")
        n = order * npred * 8
        _require(off + 8 + 2 * n <= len(b), "ADPCM book truncated")
        return cls(order, npred, list(struct.unpack_from(f">{n}h", b, off + 8)))

    @property
    def size(self) -> int:
        return 8 + 2 * len(self.coefs)


def frame_score(data: bytes, npred: int, max_frames: int | None = None) -> tuple[int, int]:
    """(valid, total) 9-byte frame headers: scale <= 12 and predictor index < npred."""
    n = len(data) // 9
    if max_frames is not None:
        n = min(n, max_frames)
    ok = 0
    for i in range(n):
        h = data[9 * i]
        ok += (h >> 4) <= 12 and (h & 15) < npred
    return ok, n


def decode_vadpcm(data: bytes, book: Book, state: list[int] | None = None) -> list[int]:
    """Decode 9-byte VADPCM frames (16 samples each) to signed 16-bit samples.

    Implements the libultra ADPCM recurrence for any book order: the previous ``order``
    outputs weight row ``k`` and earlier residuals of the half-frame use the last row."""
    order = book.order
    st = list(state or [0] * 8)
    out: list[int] = []
    c = book.coefs
    for f in range(len(data) // 9):
        p = 9 * f
        hdr = data[p]
        scale, base = hdr >> 4, (hdr & 15) * order * 8
        if (hdr & 15) >= book.npredictors:
            raise AudioError(f"frame {f}: predictor {hdr & 15} outside book")
        for half in range(2):
            res = []
            for i in range(8):
                byte = data[p + 1 + half * 4 + (i >> 1)]
                nib = byte >> 4 if not i & 1 else byte & 15
                if nib >= 8:
                    nib -= 16
                res.append(nib << scale)
            prev = st[8 - order:]
            cur = []
            last = base + (order - 1) * 8
            for i in range(8):
                acc = res[i] << 11
                for k in range(order):
                    acc += c[base + k * 8 + i] * prev[k]
                for j in range(i):
                    acc += c[last + j] * res[i - 1 - j]
                acc = ((acc + 0x80000000) & 0xFFFFFFFF) - 0x80000000
                v = max(-32768, min(32767, acc >> 11))
                cur.append(v)
            st = cur
            out.extend(cur)
    return out


def pcm16be_samples(data: bytes) -> list[int]:
    n = len(data) // 2
    return list(struct.unpack_from(f">{n}h", data))


def wav_bytes(samples: list[int], rate: int, loop: tuple[int, int, int] | None = None) -> bytes:
    """Mono PCM16 WAV; ``loop=(start, end, count)`` with count -1 adds a RIFF smpl loop."""
    buf = io.BytesIO()
    with _wave.open(buf, "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(max(1, int(rate)))
        w.writeframes(struct.pack(f"<{len(samples)}h", *samples))
    data = buf.getvalue()
    if loop and loop[2] in (-1, 0xFFFFFFFF) and 0 <= loop[0] < loop[1] <= len(samples):
        period = 1_000_000_000 // max(1, int(rate))
        body = struct.pack("<15I", 0, 0, period, 60, 0, 0, 0, 1, 0, 0, 0, loop[0], loop[1] - 1, 0, 0)
        data += b"smpl" + struct.pack("<I", len(body)) + body
        data = data[:4] + struct.pack("<I", len(data) - 8) + data[8:]
    return data


# --------------------------------------------------------------------------------------
# Shared result types
# --------------------------------------------------------------------------------------

@dataclass
class Sample:
    """One stored sample interval (ROM offsets) and how to decode it."""
    start: int
    end: int
    codec: str                      # "adpcm" | "pcm16"
    book: Book | None = None
    loop: tuple[int, int, int] | None = None   # start, end, count (count -1 = infinite)
    rate: int = 0
    refs: list[str] = field(default_factory=list)

    def decode(self, rom: bytes) -> list[int]:
        raw = rom[self.start:self.end]
        if self.codec == "adpcm":
            return decode_vadpcm(raw, self.book)
        return pcm16be_samples(raw)


@dataclass
class Container:
    """A detected audio file (or file group) with its byte-exact interval list."""
    format: str
    kind: str
    name: str
    start: int
    end: int
    summary: dict
    intervals: list[Interval] = field(default_factory=list)
    samples: list[Sample] = field(default_factory=list)
    sequences: list[tuple[str, int, int, str]] = field(default_factory=list)  # name, start, end, ext

    def as_dict(self) -> dict:
        return {"format": self.format, "kind": self.kind, "name": self.name,
                "start": self.start, "end": self.end, "size": self.end - self.start, **self.summary}


class _Cover:
    """Struct map of one file: offset -> (kind, size, detail, refs); emits gap-checked intervals."""

    def __init__(self, data: bytes, base: int, limit: int, name: str):
        self.d, self.base, self.limit, self.name = data, base, limit, name
        self.items: dict[int, list] = {}

    def add(self, off: int, kind: str, size: int, detail: str = "") -> bool:
        if off in self.items:
            k, s = self.items[off][:2]
            _require((k, s) == (kind, size), f"{self.name}: conflicting {k}/{kind} at 0x{off:X}")
            self.items[off][3] += 1
            return False
        _require(0 <= off and off + size <= self.limit, f"{self.name}: {kind} 0x{off:X} out of range")
        self.items[off] = [kind, size, detail, 1]
        return True

    def end(self) -> int:
        return max(o + v[1] for o, v in self.items.items())

    def intervals(self, file_end: int, fmt: str, garbage_ok: int = 0) -> list[Interval]:
        out, pos = [], 0
        for off in sorted(self.items):
            kind, size, detail, refs = self.items[off]
            _require(off >= pos, f"{self.name}: {kind} overlaps at 0x{off:X}")
            if off > pos:
                out.append(self._gap(pos, off, fmt, garbage_ok))
            ev = f"{detail} refs={refs}".strip()
            out.append(Interval(self.base + off, self.base + off + size, "container", CATEGORY, f"{fmt}.{kind}",
                                f"{self.name}.{kind}_{off:X}", ev))
            pos = off + size
        if pos < file_end:
            out.append(self._gap(pos, file_end, fmt, garbage_ok))
        return out

    def _gap(self, a, b, fmt, garbage_ok):
        gap = self.d[a:b]
        if not any(gap):
            return Interval(self.base + a, self.base + b, "padding", CATEGORY, "fill_00", f"{self.name}.pad_{a:X}", "align")
        if b - a < garbage_ok:
            return Interval(self.base + a, self.base + b, "padding", CATEGORY, "align_nonzero", f"{self.name}.pad_{a:X}",
                            "alignment bytes with nonzero fill")
        raise AudioError(f"{self.name}: unexplained bytes 0x{a:X}..0x{b:X}")


def _zero_tail(rom: bytes, pos: int, align: int, limit: int | None = None) -> int:
    """End of the all-zero run from ``pos`` up to the next ``align`` boundary (or pos)."""
    end = _align(pos, align)
    if limit is not None:
        end = min(end, limit)
    return end if not any(rom[pos:end]) else pos


def _sample_intervals(rom: bytes, samples: list[Sample], start: int, end: int, name: str,
                      fmt: str, garbage_ok: int = 0) -> list[Interval]:
    """Cover [start, end) with the (merged) sample intervals and classified gaps."""
    out, pos = [], start
    for s in sorted(samples, key=lambda s: (s.start, s.end)):
        if s.start < pos:
            _require(s.end <= pos, f"{name}: sample 0x{s.start:X} partially overlaps")
            continue
        if s.start > pos:
            out.append(_gap_interval(rom, pos, s.start, name, garbage_ok))
        out.append(Interval(s.start, s.end, "payload", CATEGORY, f"{fmt}.{s.codec}_sample", f"{name}.sample_{s.start - start:X}",
                            f"frames9={(s.end - s.start) // 9} refs={','.join(s.refs)}" if s.codec == "adpcm" else
                            f"samples={(s.end - s.start) // 2} refs={','.join(s.refs)}"))
        pos = s.end
    if pos < end:
        out.append(_gap_interval(rom, pos, end, name, garbage_ok))
    return out


def _gap_interval(rom, a, b, name, garbage_ok=0):
    gap = rom[a:b]
    if not any(gap):
        return Interval(a, b, "padding", CATEGORY, "fill_00", f"{name}.pad_{a:X}", "align")
    if b - a < garbage_ok:
        return Interval(a, b, "padding", CATEGORY, "align_nonzero", f"{name}.pad_{a:X}", "alignment bytes with nonzero fill")
    return Interval(a, b, "unknown", CATEGORY, "unreferenced", f"{name}.gap_{a:X}", f"{sum(1 for x in gap if x)} nonzero bytes")


# --------------------------------------------------------------------------------------
# libultra ALBankFile / sample table
# --------------------------------------------------------------------------------------

@dataclass
class CtlParse:
    start: int
    end: int
    cover: _Cover
    banks: list[dict]
    waves: dict          # (base, length) -> dict(type, book, loop, wavetables)
    rate: int


def parse_ctl(rom: bytes, start: int, limit: int | None = None) -> CtlParse:
    """Walk an ALBankFile at ROM ``start``; offsets are file-relative (unrelocated)."""
    limit = min(len(rom), limit or start + 0x100000) - start
    ctl = rom[start:start + limit]
    _require(_u16(ctl, 0) == 0x4231, "ALBankFile revision is not B1")
    nb = _s16(ctl, 2)
    _require(1 <= nb <= 64, "ALBankFile bank count out of range")
    name = f"ctl_{start:X}"
    cov = _Cover(ctl, start, limit, name)
    cov.add(0, "ALBankFile", 4 + 4 * nb, f"rev=B1 banks={nb}")
    hdr_end = 4 + 4 * nb

    def ptr(off, align=4):
        _require(off >= hdr_end and off % align == 0 and off < limit, f"{name}: bad pointer 0x{off:X}")
        return off

    banks, waves, rate = [], {}, 0
    for bi in range(nb):
        b = ptr(_u32(ctl, 4 + 4 * bi))
        ni = _s16(ctl, b)
        _require(0 <= ni <= 256, f"{name}: bank {bi} instrument count {ni}")
        flags, brate, perc = ctl[b + 2], _u32(ctl, b + 4), _u32(ctl, b + 8)
        _require(flags == 0, f"{name}: bank {bi} already relocated")
        _require(0 < brate <= 96000 or brate == 0, f"{name}: bank {bi} sample rate {brate}")
        rate = rate or brate
        cov.add(b, "ALBank", 12 + 4 * ni, f"bank={bi} insts={ni} rate={brate} perc=0x{perc:X}")
        insts = [_u32(ctl, b + 12 + 4 * i) for i in range(ni)] + ([perc] if perc else [])
        info = {"index": bi, "instruments": ni, "percussion": bool(perc), "sample_rate": brate}
        banks.append(info)
        for ii in insts:
            if not ii:
                continue
            ii = ptr(ii)
            ns = _s16(ctl, ii + 14)
            _require(0 <= ns <= 512, f"{name}: instrument sound count {ns}")
            if not cov.add(ii, "ALInstrument", 16 + 4 * ns, f"vol={ctl[ii]} pan={ctl[ii + 1]} prio={ctl[ii + 2]} sounds={ns}"):
                continue
            for si in range(ns):
                s = ptr(_u32(ctl, ii + 16 + 4 * si))
                if not cov.add(s, "ALSound", 16, f"pan={ctl[s + 12]} vol={ctl[s + 13]} flags={ctl[s + 14]}"):
                    continue
                env, km, wt = ptr(_u32(ctl, s)), ptr(_u32(ctl, s + 4), 2), ptr(_u32(ctl, s + 8))
                at, dt, rt = struct.unpack_from(">iii", ctl, env)
                cov.add(env, "ALEnvelope", 16, f"atk={at} dec={dt} rel={rt} av={ctl[env + 12]} dv={ctl[env + 13]}")
                cov.add(km, "ALKeyMap", 6, "vel={}-{} key={}-{} base={} detune={}".format(*struct.unpack_from(">5Bb", ctl, km)))
                base, length = _u32(ctl, wt), _u32(ctl, wt + 4)
                wtype, wflags = ctl[wt + 8], ctl[wt + 9]
                _require(wtype in (0, 1) and wflags == 0, f"{name}: wavetable 0x{wt:X} type {wtype} flags {wflags}")
                _require(0 < length < 0x1000000 and base < 0x4000000, f"{name}: wavetable 0x{wt:X} range")
                loop, book = _u32(ctl, wt + 12), _u32(ctl, wt + 16)
                codec = "adpcm" if wtype == 0 else "pcm16"
                if cov.add(wt, "ALWaveTable", 20, f"base=0x{base:X} len=0x{length:X} type={codec} loop=0x{loop:X} book=0x{book:X}"):
                    w = waves.setdefault((base, length), {"codec": codec, "book": None, "loop": None, "wavetables": []})
                    w["wavetables"].append(wt)
                    lp = None
                    if loop:
                        loop = ptr(loop)
                        st, en, cnt = _u32(ctl, loop), _u32(ctl, loop + 4), _s32(ctl, loop + 8)
                        _require(st <= en, f"{name}: loop 0x{loop:X} reversed")
                        cov.add(loop, "ALADPCMloop" if wtype == 0 else "ALRawLoop", 44 if wtype == 0 else 12,
                                f"start={st} end={en} count={cnt}")
                        lp = (st, en, cnt)
                    w["loop"] = w["loop"] or lp
                    if wtype == 0:
                        bk = Book.read(ctl, ptr(book))
                        cov.add(book, "ALADPCMBook", bk.size, f"order={bk.order} npredictors={bk.npredictors}")
                        w["book"] = w["book"] or bk
    last = cov.end()
    end = _zero_tail(ctl, last, 16)
    return CtlParse(start, start + end, cov, banks, waves, rate)


def _tbl_score(rom: bytes, waves: dict, t: int, frames: int = 4) -> float:
    ok = tot = 0
    for (base, length), w in waves.items():
        if w["codec"] != "adpcm":
            continue
        a = t + base
        if a < 0 or a + length > len(rom):
            return 0.0
        o, n = frame_score(rom[a:a + length], w["book"].npredictors, frames)
        ok, tot = ok + o, tot + n
    return ok / tot if tot else 0.0


def locate_tbl(rom: bytes, ctl: CtlParse, hints: list[int]) -> tuple[int | None, float, list[int]]:
    """Choose the sample-table base by VADPCM frame-header plausibility.

    Candidates: the table ending at (or up to 64 bytes before) the .ctl start, and the table
    starting at (or shortly after) each hint position (ends of other detected objects)."""
    extent = max(b + l for b, l in ctl.waves)
    first = min(b for b, _ in ctl.waves)
    cands = set()
    for slack in range(0, 65):
        cands.add(ctl.start - extent - slack)
    for h in hints:
        for slack in range(0, 65):
            cands.add(h + slack - first if first < 16 else h + slack)
    cands = sorted(c for c in cands if 0 <= c and c + extent <= len(rom))
    scored = sorted(((_tbl_score(rom, ctl.waves, c), c) for c in cands), reverse=True)
    if not scored or scored[0][0] < 0.95:
        return None, scored[0][0] if scored else 0.0, []
    top = [c for s, c in scored if s >= scored[0][0] - 1e-9]
    full = [(_tbl_score(rom, ctl.waves, c, None), c) for c in top]
    best = max(full)[0]
    ties = sorted(c for s, c in full if s == best)

    def touch(c):
        # prefer the base leaving the fewest unexplained bytes after the preceding hint
        # (or before the ctl) and a zero-filled tail reaching the ctl start
        prev = max((h for h in hints if h <= c), default=c)
        lead = sum(1 for x in rom[prev:c] if x) if c - prev < 0x10000 else 0x10000
        tail = rom[c + extent:ctl.start] if c + extent <= ctl.start else b"x"
        return (lead, 0 if not any(tail) and len(tail) < 64 else 1, -c)
    ties.sort(key=touch)
    return ties[0], best, ties


def scan_albankfiles(rom: bytes) -> list[CtlParse]:
    out = []
    for m in re.finditer(rb"B1\x00[\x01-\x40]", rom):
        if m.start() % 2:
            continue
        try:
            out.append(parse_ctl(rom, m.start()))
        except (AudioError, struct.error, IndexError):
            continue
    return out


def libaudio_bank(rom: bytes, ctl: CtlParse, hints: list[int]) -> Container:
    name = f"ctl_{ctl.start:X}"
    ivs = ctl.cover.intervals(ctl.end - ctl.start, "libaudio")
    kinds: dict[str, int] = {}
    for v in ctl.cover.items.values():
        kinds[v[0]] = kinds.get(v[0], 0) + 1
    t, score, ties = locate_tbl(rom, ctl, hints)
    summary = {"banks": ctl.banks, "structs": kinds, "unique_waves": len(ctl.waves),
               "sample_rate": ctl.rate, "tbl_start": t, "tbl_score": round(score, 4),
               "tbl_candidates": len(ties)}
    c = Container("libaudio", "ALBankFile", name, ctl.start, ctl.end, summary, ivs)
    if t is not None:
        extent = max(b + l for b, l in ctl.waves)
        tbl_end = t + extent
        if tbl_end <= ctl.start and not any(rom[tbl_end:ctl.start]):
            tbl_end = ctl.start      # sample table padded up to its bank file
        else:
            tbl_end = _zero_tail(rom, tbl_end, 16)
        for (base, length), w in sorted(ctl.waves.items()):
            c.samples.append(Sample(t + base, t + base + length, w["codec"], w["book"], w["loop"], ctl.rate or 22050,
                                    [f"0x{x:X}" for x in w["wavetables"]]))
        tname = f"tbl_{t:X}"
        c.intervals += _sample_intervals(rom, c.samples, t, tbl_end, tname, "libaudio")
        summary.update(tbl_start=t, tbl_end=tbl_end)
    return c


# --------------------------------------------------------------------------------------
# MIDI
# --------------------------------------------------------------------------------------

def _vlq(d, o):
    v = 0
    for _ in range(4):
        c = d[o]
        o += 1
        v = (v << 7) | (c & 0x7F)
        if not c & 0x80:
            return v, o
    raise AudioError("bad VLQ")


def _midi_track(d, o, end):
    running, events = None, 0
    while o < end:
        _, o = _vlq(d, o)
        st = d[o]
        if st == 0xFF:
            typ = d[o + 1]
            ln, o = _vlq(d, o + 2)
            o += ln
            events += 1
            if typ == 0x2F:
                return o, events
            continue
        if st in (0xF0, 0xF7):
            ln, o = _vlq(d, o + 1)
            o += ln
            events += 1
            continue
        if st & 0x80:
            running = st
            o += 1
        elif running is None:
            raise AudioError("data byte without running status")
        n = 1 if (running & 0xF0) in (0xC0, 0xD0) else 2
        _require(not any(x & 0x80 for x in d[o:o + n]), "bad MIDI data byte")
        o += n
        events += 1
    raise AudioError("no end-of-track")


def parse_midi(rom: bytes, start: int) -> dict:
    """Validate a Standard MIDI file at ``start``; returns its exact end and zero padding."""
    d = rom
    _require(d[start:start + 4] == b"MThd" and _u32(d, start + 4) == 6, "MThd header")
    fmt, ntr, div = struct.unpack_from(">HHH", d, start + 8)
    _require(fmt <= 2 and 1 <= ntr <= 256, "MThd fields")
    o, events = start + 14, 0
    for _ in range(ntr):
        _require(d[o:o + 4] == b"MTrk", "MTrk")
        ln = _u32(d, o + 4)
        _require(o + 8 + ln <= len(d), "MTrk length")
        end, n = _midi_track(d, o + 8, o + 8 + ln)
        _require(end == o + 8 + ln, "end-of-track before chunk end")
        events += n
        o = end
    pad_end = _zero_tail(d, o, 16)
    return {"start": start, "end": o, "pad_end": pad_end, "format": fmt, "tracks": ntr, "division": div,
            "events": events, "sha1": hashlib.sha1(d[start:o]).hexdigest()}


def scan_midi(rom: bytes) -> list[dict]:
    out = []
    for m in re.finditer(rb"MThd\x00\x00\x00\x06", rom):
        try:
            out.append(parse_midi(rom, m.start()))
        except (AudioError, struct.error, IndexError):
            continue
    return out


def parse_compact_midi(d: bytes) -> dict:
    """libultra compact MIDI (ALCMidiHdr): 16 track offsets then division."""
    _require(len(d) >= 0x44, "compact MIDI too short")
    offs = struct.unpack_from(">16I", d, 0)
    div = _u32(d, 0x40)
    _require(0 < div < 0x10000, "compact MIDI division")
    used = [o for o in offs if o]
    _require(used and all(0x44 <= o < len(d) for o in used), "compact MIDI track offsets")
    return {"tracks": len(used), "division": div}


def scan_alseqfiles(rom: bytes) -> list[dict]:
    out = []
    for m in re.finditer(rb"S1", rom):
        s = m.start()
        if s % 4:
            continue
        try:
            n = _s16(rom, s + 2)
            _require(1 <= n <= 1024, "count")
            hdr = 4 + 8 * n
            ents, kinds = [], []
            for i in range(n):
                off, ln = struct.unpack_from(">Ii", rom, s + 4 + 8 * i)
                _require(hdr <= off < 0x1000000 and 0 < ln < 0x400000 and s + off + ln <= len(rom), "entry")
                d = rom[s + off:s + off + ln]
                if d[:4] == b"MThd":
                    parse_midi(rom, s + off)
                    kinds.append("midi")
                else:
                    parse_compact_midi(d)
                    kinds.append("compact_midi")
                ents.append((off, ln))
            out.append({"start": s, "count": n, "entries": ents, "kinds": kinds,
                        "end": max(o + l for o, l in ents) + s})
        except (AudioError, struct.error, IndexError):
            continue
    return out


# --------------------------------------------------------------------------------------
# Cartridge file directories (address tables that frame detected files)
# --------------------------------------------------------------------------------------

def find_directories(rom: bytes, starts: list[int]) -> list[dict]:
    """Tables of cartridge addresses (0xB0000000 | rom) that reference detected file starts.

    Recognises ``(address, size)`` pair tables and ascending boundary lists."""
    found, seen = [], set()
    for s in starts:
        key = struct.pack(">I", CART | s)
        for m in re.finditer(re.escape(key), rom):
            p = m.start()
            if p % 4 or p in seen:
                continue
            t = _pair_table(rom, p) or _boundary_table(rom, p)
            if t and t["entries"] and t["at"] not in {f["at"] for f in found}:
                found.append(t)
                seen.update(range(t["at"], t["at_end"], 4))
    return found


def _is_cart(w, rom):
    return w & 0xF0000000 == CART and (w & 0x0FFFFFFF) < len(rom)


def _pair_table(rom, p):
    def ok(q):
        if q < 0 or q + 8 > len(rom):
            return False
        a, n = struct.unpack_from(">II", rom, q)
        return _is_cart(a, rom) and 0 < n < 0x1000000 and (a & 0x0FFFFFFF) + n <= len(rom)
    if not ok(p) or not ok(p + 8):
        return None
    a0, n0, a1 = struct.unpack_from(">III", rom, p)
    if not 0 <= (a1 & 0x0FFFFFFF) - ((a0 & 0x0FFFFFFF) + n0) < 64:
        return None
    q = p
    while ok(q - 8):
        a, n = struct.unpack_from(">II", rom, q - 8)
        if not 0 <= (a0 & 0x0FFFFFFF) - ((a & 0x0FFFFFFF) + n) < 64:
            break
        q -= 8
        a0 = a
    ents, r, prev_end = [], q, None
    while ok(r):
        a, n = struct.unpack_from(">II", rom, r)
        a &= 0x0FFFFFFF
        if prev_end is not None and not 0 <= a - prev_end < 64:
            break
        ents.append((a, a + n))
        prev_end = a + n
        r += 8
    return {"at": q, "at_end": r, "shape": "address_size", "entries": ents} if len(ents) >= 2 else None


def _boundary_table(rom, p):
    def word(q):
        return _u32(rom, q) if 0 <= q <= len(rom) - 4 else 0
    q = p
    while _is_cart(word(q - 4), rom) and 0 < word(q) - word(q - 4) < 0x400000:
        q -= 4
    r = q
    while _is_cart(word(r + 4), rom) and 0 < word(r + 4) - word(r) < 0x400000:
        r += 4
    bounds = [word(x) & 0x0FFFFFFF for x in range(q, r + 4, 4)]
    if len(bounds) < 3:
        return None
    return {"at": q, "at_end": r + 4, "shape": "boundaries", "entries": list(zip(bounds, bounds[1:]))}


# --------------------------------------------------------------------------------------
# libmus (Software Creations)
# --------------------------------------------------------------------------------------

PTR_MAGIC = b"N64 PtrTablesV2\0"
WAVE_MAGIC = b"N64 WaveTables \0"


def parse_libmus_ptr(rom: bytes, start: int, wave_start: int, limit: int | None = None) -> tuple[_Cover, list[dict]]:
    limit = (limit or len(rom)) - start
    d = rom[start:start + limit]
    _require(d[:16] == PTR_MAGIC, "PtrTablesV2 magic")
    name = f"ptr_{start:X}"
    cov = _Cover(d, start, limit, name)
    count, notes, detunes, wlist = (_u32(d, 0x20 + 4 * i) for i in range(4))
    _require(0 < count < 4096, f"{name}: wave count")
    cov.add(0, "PtrTablesHeader", 0x30, f"waves={count}")
    cov.add(notes, "BaseNotes", count, "")
    cov.add(detunes, "Detunes", 4 * count, "")
    cov.add(wlist, "WaveList", 4 * count, "")
    wd = rom[wave_start:]
    _require(wd[:16] == WAVE_MAGIC, "WaveTables magic")
    waves = []
    for i in range(count):
        w = _u32(d, wlist + 4 * i)
        base, size, typ, flags = _u32(d, w), _u32(d, w + 4), d[w + 8], d[w + 9]
        loop, book = _u32(d, w + 12), _u32(d, w + 16)
        _require(typ in (0, 1) and flags == 0, f"{name}: wave {i} type/flags")
        _require(16 <= base and base + size <= len(wd), f"{name}: wave {i} sample range")
        cov.add(w, "WaveTable", 20, f"base=0x{base:X} len=0x{size:X}")
        lp = bk = None
        if loop:
            st, en, cnt = _u32(d, loop), _u32(d, loop + 4), _s32(d, loop + 8)
            cov.add(loop, "ADPCMloop" if typ == 0 else "RawLoop", 44 if typ == 0 and cnt else 12 if typ else 44,
                    f"start={st} end={en} count={cnt}")
            lp = (st, en, cnt)
        if typ == 0:
            bk = Book.read(d, book)
            cov.add(book, "ADPCMBook", bk.size, f"order={bk.order} npredictors={bk.npredictors}")
        waves.append({"index": i, "start": wave_start + base, "end": wave_start + base + size,
                      "codec": "adpcm" if typ == 0 else "pcm16", "loop": lp, "book": bk,
                      "base_note": d[notes + i], "detune": struct.unpack_from(">b", d, detunes + 4 * i)[0]})
    return cov, waves


def parse_libmus_song(d: bytes) -> dict:
    _require(len(d) >= 0x38 and _u32(d, 0) == 0x215, "libmus song version")
    nch, nw = _u32(d, 4), _u32(d, 8)
    _require(1 <= nch <= 32 and nw < 4096, "libmus song counts")
    offs = [_u32(d, o) for o in range(0x0C, 0x28, 4)]
    _require(_u32(d, 0x28) == 0 and not any(d[0x2C:0x38]), "libmus song reserved fields")
    _require(all(o < len(d) for o in offs), "libmus song section offsets")
    for t in range(3):
        _require(offs[t] and offs[t] + 4 * nch <= len(d), "libmus stream table")
        _require(all(_u32(d, offs[t] + 4 * c) < len(d) for c in range(nch)), "libmus stream offsets")
    _require(offs[5] and offs[5] + 2 * nw <= len(d) and offs[6], "libmus wave table / master track")
    ids = {_u16(d, offs[5] + 2 * i) for i in range(nw)} - {0xFFFF}
    return {"channels": nch, "wave_slots": nw, "waves": sorted(ids)}


def parse_libmus_effects(d: bytes) -> dict:
    n = _u32(d, 0)
    _require(0 < n < 4096 and _u32(d, 0x10) == 0 and 0x18 + 8 * n <= len(d), "libmus effect bank header")
    _require(_u32(d, 0x14) < len(d), "libmus effect bytecode base")
    _require(all(0 < _u32(d, 0x18 + 8 * i) < len(d) for i in range(n)), "libmus effect offsets")
    return {"effects": n}


def scan_libmus(rom: bytes) -> list[Container]:
    ptrs = [m.start() for m in re.finditer(re.escape(PTR_MAGIC), rom)]
    wavs = [m.start() for m in re.finditer(re.escape(WAVE_MAGIC), rom)]
    if not ptrs:
        return []
    dirs = find_directories(rom, ptrs)
    files = sorted({e for t in dirs for e in t["entries"]})
    out = []
    if files:
        d0 = dirs[0]
        f0, f1 = files[0][0], files[-1][1]
        c = Container("libmus", "store", f"libmus_{f0:X}", f0, f1,
                      {"directory": d0["at"], "files": len(files)})
        out.append(c)
    else:
        c = None
    # pair each pointer bank with the nearest wave bank whose samples score as ADPCM
    music_waves = {}
    songs = []
    for p in ptrs:
        limit = next((e for s, e in files if s == p), None)
        best = None
        for w in sorted(wavs, key=lambda w: abs(w - p)):
            try:
                cov, waves = parse_libmus_ptr(rom, p, w, limit)
            except (AudioError, struct.error, IndexError):
                continue
            ok = tot = 0
            for x in waves:
                if x["codec"] == "adpcm":
                    a, b = frame_score(rom[x["start"]:x["end"]], x["book"].npredictors, 8)
                    ok, tot = ok + a, tot + b
            if tot and ok / tot > 0.95:
                best = (w, cov, waves)
                break
        if not best:
            continue
        w, cov, waves = best
        pend = limit or p + cov.end()
        wend = next((e for s, e in files if s == w), None) or max(x["end"] for x in waves)
        bank = Container("libmus", "PtrTablesV2", f"ptr_{p:X}", p, pend,
                         {"waves": len(waves), "wave_bank": w, "wave_bank_end": wend,
                          "looped": sum(1 for x in waves if x["loop"] and x["loop"][2])},
                         cov.intervals(pend - p, "libmus", garbage_ok=8))
        for x in waves:
            bank.samples.append(Sample(x["start"], x["end"], x["codec"], x["book"], x["loop"], 22050,
                                       [f"wave{x['index']}"]))
        bank.intervals.append(Interval(w, w + 16, "container", CATEGORY, "libmus.WaveTablesHeader", f"wbk_{w:X}.header", "magic"))
        bank.intervals += _sample_intervals(rom, bank.samples, w + 16, wend, f"wbk_{w:X}", "libmus", garbage_ok=16)
        music_waves[p] = len(waves)
        out.append(bank)
    claimed = {(b.start, b.end) for b in out if b.kind == "PtrTablesV2"}
    claimed |= {(b.summary["wave_bank"], b.summary["wave_bank_end"]) for b in out if b.kind == "PtrTablesV2"}
    for s, e in files:
        if (s, e) in claimed:
            continue
        d = rom[s:e]
        try:
            info = parse_libmus_song(d)
            k = Container("libmus", "song", f"song_{s:X}", s, e, info,
                          [Interval(s, e, "payload", CATEGORY, "libmus.song", f"song_{s:X}", f"channels={info['channels']}")])
            k.sequences.append((f"song_{s:X}", s, e, "bin"))
            songs.append(k)
            continue
        except (AudioError, struct.error, IndexError):
            pass
        try:
            info = parse_libmus_effects(d)
            out.append(Container("libmus", "effects", f"fx_{s:X}", s, e, info,
                                 [Interval(s, e, "payload", CATEGORY, "libmus.effects", f"fx_{s:X}", f"effects={info['effects']}")]))
        except (AudioError, struct.error, IndexError):
            out.append(Container("libmus", "file", f"file_{s:X}", s, e, {},
                                 [Interval(s, e, "unknown", CATEGORY, "libmus.file", f"file_{s:X}", "directory entry")]))
    out += songs
    if c is not None:
        # inter-file alignment bytes
        for (s0, e0), (s1, _e1) in zip(files, files[1:]):
            if s1 > e0:
                c.intervals.append(_gap_interval(rom, e0, s1, c.name, garbage_ok=16))
        c.summary["songs"] = len(songs)
        c.summary["song_waves_used"] = len({w for s in songs for w in s.summary["waves"]})
    return out


# --------------------------------------------------------------------------------------
# Nintendo seqfile containers (SM64 / MK64 sound banks)
# --------------------------------------------------------------------------------------

def _seqfile(rom: bytes, s: int, rev: int) -> list[tuple[int, int]] | None:
    if s + 16 > len(rom) or _u16(rom, s) != rev:
        return None
    n = _u16(rom, s + 2)
    if not 1 <= n <= 255:
        return None
    hs = _align(4 + 8 * n, 16)
    ents = [struct.unpack_from(">II", rom, s + 4 + 8 * i) for i in range(n)]
    if not any(o == hs for o, l in ents if l):
        return None
    for o, l in ents:
        if l and not (hs <= o and s + o + l <= len(rom) and l < 0x800000):
            return None
        if not l and o >= n:
            return None
    if any(rom[s + 4 + 8 * n:s + hs]):
        return None
    return ents


def _sm64_ctl_entry(rom: bytes, s: int, length: int, name: str, tbl_base: int) -> tuple[_Cover, dict, list[Sample]]:
    d = rom[s:s + length]
    ninst, ndrum, shared, date = struct.unpack_from(">IIII", d, 0)
    _require(ninst <= 128 and ndrum <= 128 and shared <= 1, f"{name}: header")
    body = d[16:]
    cov = _Cover(body, s + 16, len(body), name)
    hdr = _Cover(d, s, 16, name)
    cov.add(0, "AudioBank", 4 + 4 * ninst, f"instruments={ninst} drums={ndrum}")
    samples: dict[int, Sample] = {}
    envs: set[int] = set()

    def ptr(o, a=4):
        _require(o % a == 0 and 0 < o < len(body), f"{name}: pointer 0x{o:X}")
        return o

    def env(o):
        o = ptr(o)
        if o in envs:
            cov.items[o][3] += 1
            return
        envs.add(o)
        p = o
        while True:
            _require(p + 4 <= len(body), f"{name}: envelope runs off")
            delay = _s16(body, p)
            p += 4
            if delay <= 0:
                break
        cov.add(o, "Envelope", p - o, f"points={(p - o) // 4}")

    def sound(o):
        smp = _u32(body, o)
        if not smp:
            return
        smp = ptr(smp)
        addr, loop, book, size = struct.unpack_from(">IIII", body, smp + 4)
        if not cov.add(smp, "AudioBankSample", 20, f"addr=0x{addr:X} size=0x{size:X}"):
            samples[smp].refs.append(f"0x{o:X}")
            return
        bk = Book.read(body, ptr(book))
        cov.add(book, "AdpcmBook", bk.size, f"order={bk.order} npredictors={bk.npredictors}")
        loop = ptr(loop)
        st, en, cnt = struct.unpack_from(">IIi", body, loop)
        cov.add(loop, "AdpcmLoop", 16 + (32 if cnt else 0), f"start={st} end={en} count={cnt}")
        _require(0 < size < 0x400000, f"{name}: sample size")
        samples[smp] = Sample(tbl_base + addr, tbl_base + addr + size, "adpcm", bk, (st, en, cnt) if cnt else None,
                              32000, [f"{name}:0x{smp:X}"])

    for i in range(ninst):
        o = _u32(body, 4 + 4 * i)
        if not o:
            continue
        o = ptr(o)
        if cov.add(o, "Instrument", 0x20, f"range={body[o + 1]}-{body[o + 2]} release={body[o + 3]}"):
            env(_u32(body, o + 4))
            for k in range(3):
                sound(o + 8 + 8 * k)
    dlist = _u32(body, 0)
    if ndrum and dlist:
        dlist = ptr(dlist)
        cov.add(dlist, "DrumList", 4 * ndrum, f"drums={ndrum}")
        for i in range(ndrum):
            o = _u32(body, dlist + 4 * i)
            if not o:
                continue
            o = ptr(o)
            if cov.add(o, "Drum", 0x10, f"release={body[o]} pan={body[o + 1]}"):
                sound(o + 4)
                env(_u32(body, o + 12))
    hdr.add(0, "CtlHeader", 16, f"instruments={ninst} drums={ndrum} shared={shared} date={date:08X}")
    info = {"instruments": ninst, "drums": ndrum, "samples": len(samples), "date": f"{date:08X}"}
    return (hdr, cov), info, list(samples.values())


def scan_seqfile_banks(rom: bytes) -> list[Container]:
    """Find ctl(1)/tbl(2)/seq(3) seqfile containers; parse the banks against the sample table."""
    found = {1: [], 2: [], 3: []}
    for s in range(0, len(rom) - 16, 16):
        r = _u16(rom, s)
        if r in found and rom[s + 2] == 0 and rom[s + 3]:
            ents = _seqfile(rom, s, r)
            if ents:
                found[r].append((s, ents))
    out = []
    for cs, cents in found[1]:
        # pair with a table file whose entry count matches
        tbls = [(ts, te) for ts, te in found[2] if len(te) == len(cents)]
        if not tbls:
            continue
        ts, tents = min(tbls, key=lambda x: abs(x[0] - cs))
        name = f"seqctl_{cs:X}"
        try:
            ivs, all_samples, infos, kinds = [], {}, [], {}
            hs = _align(4 + 8 * len(cents), 16)
            ivs.append(Interval(cs, cs + hs, "container", CATEGORY, "seqfile.header", f"{name}.header", f"rev=1 entries={len(cents)}"))
            for i, (o, ln) in enumerate(cents):
                if not ln:
                    continue
                to, tl = tents[i]
                if not tl:
                    to, tl = tents[to]
                (hdr, cov), info, smps = _sm64_ctl_entry(rom, cs + o, ln, f"{name}.bank{i}", ts + to)
                infos.append({"index": i, **info, "tbl_entry": to})
                ivs += hdr.intervals(16, "seqfile")
                ivs += cov.intervals(ln - 16, "seqfile")
                for v in cov.items.values():
                    kinds[v[0]] = kinds.get(v[0], 0) + 1
                for sm in smps:
                    key = (sm.start, sm.end)
                    if key in all_samples:
                        all_samples[key].refs += sm.refs
                    else:
                        all_samples[key] = sm
        except (AudioError, struct.error, IndexError):
            continue
        cend = cs + max(o + l for o, l in cents)
        a64 = cs + _align(cend - cs, 64)
        ce = a64 if not any(rom[cend:a64]) else cend
        prev = cs + hs
        for o, ln in sorted(set(cents)):
            if ln and cs + o > prev:
                ivs.append(_gap_interval(rom, prev, cs + o, name, garbage_ok=16))
            prev = max(prev, cs + o + ln)
        if ce > prev:
            ivs.append(_gap_interval(rom, prev, ce, name))
        c = Container("seqfile", "ctl", name, cs, ce,
                      {"entries": len(cents), "banks": infos, "structs": kinds, "tbl": ts,
                       "unique_samples": len(all_samples)}, ivs)
        c.samples = sorted(all_samples.values(), key=lambda x: x.start)
        # table file
        ths = _align(4 + 8 * len(tents), 16)
        tend = ts + max(o + l for o, l in tents if l)
        tname = f"seqtbl_{ts:X}"
        tc = Container("seqfile", "tbl", tname, ts, _align(tend, 16) if not any(rom[tend:_align(tend, 16)]) else tend,
                       {"entries": len(tents), "distinct_banks": len({e for e in tents if e[1]})},
                       [Interval(ts, ts + ths, "container", CATEGORY, "seqfile.header", f"{tname}.header", "rev=2")])
        tc.intervals += _sample_intervals(rom, c.samples, ts + ths, tc.end, tname, "seqfile", garbage_ok=16)
        out += [c, tc]
    for ss, sents in found[3]:
        name = f"seq_{ss:X}"
        hs = _align(4 + 8 * len(sents), 16)
        ivs = [Interval(ss, ss + hs, "container", CATEGORY, "seqfile.header", f"{name}.header", f"rev=3 entries={len(sents)}")]
        c = Container("seqfile", "sequences", name, ss, ss, {"entries": len(sents)}, ivs)
        prev = ss + hs
        for i, (o, ln) in sorted(enumerate(sents), key=lambda x: x[1]):
            if not ln or ss + o < prev:
                continue
            if ss + o > prev:
                ivs.append(_gap_interval(rom, prev, ss + o, name, garbage_ok=16))
            ivs.append(Interval(ss + o, ss + o + ln, "payload", CATEGORY, "seqfile.m64", f"{name}.seq{i}", f"len={ln}"))
            c.sequences.append((f"{name}_seq{i:02d}", ss + o, ss + o + ln, "m64"))
            prev = ss + o + ln
        a64 = ss + _align(prev - ss, 64)
        end = a64 if not any(rom[prev:a64]) else prev
        if end > prev:
            ivs.append(_gap_interval(rom, prev, end, name))
        c.end = end
        # bank-set table: u16 offsets[nseq], then (count, bank ids...) records
        nb = max((len(x[1]) for x in found[1]), default=0)
        try:
            n = len(sents)
            offs = struct.unpack_from(f">{n}H", rom, end)
            _require(min(offs) >= 2 * n, "bank set offsets")
            last = 0
            for o in offs:
                cnt = rom[end + o]
                _require(1 <= cnt <= 16 and all(b < (nb or 256) for b in rom[end + o + 1:end + o + 1 + cnt]), "bank set")
                last = max(last, o + 1 + cnt)
            _require(not any(rom[end + 2 * n:end + min(offs)]), "bank set gap")
            send = _zero_tail(rom, end + last, 16)
            ivs.append(Interval(end, end + last, "container", CATEGORY, "seqfile.bank_sets", f"{name}.bank_sets", f"sequences={n}"))
            if send > end + last:
                ivs.append(_gap_interval(rom, end + last, send, name))
            c.end = send
            c.summary["bank_sets"] = n
        except (AudioError, struct.error, IndexError, ValueError):
            pass
        out.append(c)
    return out


# --------------------------------------------------------------------------------------
# Whole-ROM scan
# --------------------------------------------------------------------------------------

@dataclass
class AudioScan:
    containers: list[Container]
    midi: list[dict]
    seqfiles: list[dict]
    rom: bytes = b""

    @property
    def intervals(self) -> list[Interval]:
        items = [iv for c in self.containers for iv in c.intervals]
        for m in self.midi:
            items.append(Interval(m["start"], m["end"], "payload", CATEGORY, "midi.smf", f"midi_{m['start']:X}",
                                  f"format={m['format']} tracks={m['tracks']} events={m['events']}"))
            if m["pad_end"] > m["end"]:
                items.append(Interval(m["end"], m["pad_end"], "padding", CATEGORY, "fill_00", f"midi_{m['start']:X}.pad", "align16"))
        items = sorted(set(items))
        # zero-filled alignment between adjacent detected audio objects
        fill = []
        for a, b in zip(items, items[1:]):
            if 0 < b.start - a.end < 64 and not any(self.rom[a.end:b.start]):
                fill.append(Interval(a.end, b.start, "padding", CATEGORY, "fill_00", f"{a.name}.tail", "align"))
        return sorted(items + fill)

    def samples(self) -> list[tuple[str, Sample]]:
        return [(c.name, s) for c in self.containers for s in c.samples]

    def inventory(self) -> dict:
        ivs = self.intervals
        tot: dict[str, int] = {}
        for iv in ivs:
            tot[iv.cls] = tot.get(iv.cls, 0) + iv.size
        return {"containers": [c.as_dict() for c in self.containers],
                "midi": self.midi, "alseqfiles": self.seqfiles,
                "counts": {"containers": len(self.containers), "samples": len(self.samples()),
                           "midi": len(self.midi), "intervals": len(ivs)},
                "bytes": tot}


def scan(rom: bytes) -> AudioScan:
    """Detect and fully parse every supported audio structure in a big-endian ROM image."""
    midi = scan_midi(rom)
    seqs = scan_alseqfiles(rom)
    containers: list[Container] = []
    ctls = scan_albankfiles(rom)
    hints = sorted({c.end for c in ctls} | {m["pad_end"] for m in midi} | {0x1000})
    for ctl in ctls:
        containers.append(libaudio_bank(rom, ctl, hints))
    for s in seqs:
        name = f"alseq_{s['start']:X}"
        c = Container("libaudio", "ALSeqFile", name, s["start"], s["end"], {"entries": s["count"]},
                      [Interval(s["start"], s["start"] + 4 + 8 * s["count"], "container", CATEGORY, "libaudio.ALSeqFile",
                                name, f"entries={s['count']}")])
        midi_starts = {m["start"] for m in midi}
        for i, ((o, ln), k) in enumerate(zip(s["entries"], s["kinds"])):
            if s["start"] + o not in midi_starts:
                c.intervals.append(Interval(s["start"] + o, s["start"] + o + ln, "payload", CATEGORY,
                                            f"libaudio.{k}", f"{name}.seq{i}", ""))
            c.sequences.append((f"{name}_seq{i:02d}", s["start"] + o, s["start"] + o + ln, "cmf" if k == "compact_midi" else "mid"))
        containers.append(c)
    containers += scan_libmus(rom)
    containers += scan_seqfile_banks(rom)
    return AudioScan(containers, midi, seqs, rom)


def check_intervals(items: list[Interval]) -> list[str]:
    """Overlap problems in an interval list (empty when consistent)."""
    bad, prev = [], None
    for it in sorted(items):
        if prev and it.start < prev.end:
            bad.append(f"{prev.name} 0x{prev.start:X}..0x{prev.end:X} / {it.name} 0x{it.start:X}..0x{it.end:X}")
        prev = it if not prev or it.end > prev.end else prev
    return bad
