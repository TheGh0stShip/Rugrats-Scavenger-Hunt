#!/usr/bin/env python3
"""libmus audio data of Rugrats: Scavenger Hunt (US), ROM 0x57FD0..0x21AC00.

Components (all big-endian):
  0x057FD0..0x05B470  "N64 PtrTablesV2" pointer bank 1 (78 waves)
  0x05B470..0x1105F0  "N64 WaveTables " sample data 1 (+ zero fill)
  0x1105F0..0x111310  libmus effects bank (default fx bank, 112 effects; wave map into bank 1)
  0x111310..0x215C80  "N64 WaveTables " sample data 2
  0x215C80..0x21AC00  "N64 PtrTablesV2" pointer bank 2 (100 waves)

Pointer banks are parsed into structures and re-serialized from them: every
pointer written by the serializer is recomputed from the layout, not copied.
Wave tables are reassembled from per-sample payload bytes and preserved fill
runs. Encoded VADPCM bytes are preserved verbatim (see formats/audio.md).

CLI: python3 tools/audio.py ROM [--wav] [--reencode-test N]
"""
from __future__ import annotations

import hashlib
import os
import struct
import sys
from dataclasses import dataclass, field

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import audio_songs  # noqa: E402  (shared libmus command-stream codec)

PTR_MAGIC = b"N64 PtrTablesV2\0"
WAVE_MAGIC = b"N64 WaveTables \0"

PTR1 = (0x57FD0, 0x5B470)
WAV1 = (0x5B470, 0x1105F0)
FX1 = (0x1105F0, 0x111310)
WAV2 = (0x111310, 0x215C80)
PTR2 = (0x215C80, 0x21AC00)
AREA = (0x57FD0, 0x21AC00)

# Runtime evidence (fulltext.s, func_8001EA7C): loader func_8001EDEC copies
# ROM ranges to RAM; then a musConfig-shaped block at 0x80061E08 is filled.
RUNTIME = {
    "ptr1": "8001EA80..8001EAC0: func_8001EDEC(0xB0057FD0, 0x80061E60, 0xB005B470-0xB0057FD0); "
            "RAM 0x80061E60 stored to cfg+0x18 (musConfig.ptr) at 8001EBF0",
    "wav1": "8001EBB0..8001EBC0: cart address 0xB005B470 stored to cfg+0x1C (musConfig.wbk)",
    "fx1": "8001EAC8..8001EAE4: func_8001EDEC(0xB01105F0, 0x800664C8, 0xB0111310-0xB01105F0); "
           "RAM 0x800664C8 stored to cfg+0x20 (musConfig.default_fxbank) at 8001EBF4",
    "ptr2": "8001EB2C..8001EB48: func_8001EDEC(0xB0215C80, 0x800674D0, 0xB021AC00-0xB0215C80); "
            "8001EC18..8001EC28: func_8003C2E4(0x800674D0, 0xB0111310) pairs it with wave table 2",
    "wav2": "8001EC1C..8001EC2C: cart address 0xB0111310 passed with pointer bank 2 to func_8003C2E4",
}


def u32(b, o):
    return struct.unpack_from(">I", b, o)[0]


def sha(b: bytes) -> str:
    return hashlib.sha256(b).hexdigest()


class AudioError(ValueError):
    pass


def need(c, msg):
    if not c:
        raise AudioError(msg)


# ----------------------------------------------------------------------------
# Pointer bank (PtrTablesV2)
# ----------------------------------------------------------------------------

@dataclass
class Wave:
    index: int
    base: int           # offset into wave table
    length: int
    type: int           # 0 = ADPCM, 1 = raw PCM16
    flags: int
    pad: int
    loop_id: int | None
    book_id: int | None


@dataclass
class Loop:
    start: int
    end: int
    count: int
    state: list[int]    # 16 x s16 for ADPCM loops, [] for raw loops
    raw: bool = False

    def pack(self) -> bytes:
        b = struct.pack(">IIi", self.start, self.end, self.count)
        if not self.raw:
            b += struct.pack(">16h", *self.state)
        return b


@dataclass
class BookRec:
    order: int
    npred: int
    coefs: list[int]

    def pack(self) -> bytes:
        return struct.pack(">ii", self.order, self.npred) + struct.pack(f">{len(self.coefs)}h", *self.coefs)


@dataclass
class PtrBank:
    flags: int
    wbk_name: bytes                     # 12 bytes
    waves: list[Wave]
    loops: list[Loop]
    books: list[BookRec]
    base_notes: bytes
    detunes: list[float]
    detune_raw: bytes                   # exact float bits
    # file layout: ordered items ("hdr"|"wave"|"loop"|"book"|"wlist"|"notes"|"detune"|"pad", id/bytes)
    layout: list[tuple] = field(default_factory=list)
    size: int = 0


def parse_ptrbank(d: bytes) -> PtrBank:
    need(d[:16] == PTR_MAGIC, "PtrTablesV2 magic")
    flags = u32(d, 0x10)
    wbk = d[0x14:0x20]
    n, notes, det, wl = struct.unpack_from(">4I", d, 0x20)
    need(0 < n < 4096, "wave count")
    items = {0: ("hdr", None, 0x30), wl: ("wlist", None, 4 * n),
             notes: ("notes", None, n), det: ("detune", None, 4 * n)}
    waves, loops, books = [], [], []
    loop_at, book_at = {}, {}
    for i in range(n):
        p = u32(d, wl + 4 * i)
        base, ln, t, fl, pad, lp, bk = struct.unpack_from(">IIBBHII", d, p)
        need(t in (0, 1), f"wave {i} type {t}")
        lid = bid = None
        if lp:
            if lp not in loop_at:
                st, en, ct = struct.unpack_from(">IIi", d, lp)
                raw = t == 1
                state = [] if raw else list(struct.unpack_from(">16h", d, lp + 12))
                loop_at[lp] = len(loops)
                loops.append(Loop(st, en, ct, state, raw))
                items[lp] = ("loop", loop_at[lp], 12 if raw else 44)
            lid = loop_at[lp]
        if t == 0:
            need(bk != 0, f"wave {i} ADPCM without book")
            if bk not in book_at:
                o, npr = struct.unpack_from(">ii", d, bk)
                need(1 <= o <= 8 and 1 <= npr <= 16, f"book at 0x{bk:X}")
                c = list(struct.unpack_from(f">{o * npr * 8}h", d, bk + 8))
                book_at[bk] = len(books)
                books.append(BookRec(o, npr, c))
                items[bk] = ("book", book_at[bk], 8 + 16 * o * npr)
            bid = book_at[bk]
        else:
            need(bk == 0, f"raw wave {i} has book field")
        need(p not in items, f"wave record 0x{p:X} shared")
        items[p] = ("wave", i, 20)
        waves.append(Wave(i, base, ln, t, fl, pad, lid, bid))
    layout, pos = [], 0
    for off in sorted(items):
        kind, ident, ln = items[off]
        need(off >= pos, f"overlap at 0x{off:X}")
        if off > pos:
            layout.append(("pad", d[pos:off]))
        layout.append((kind, ident))
        pos = off + ln
    if pos < len(d):
        layout.append(("pad", d[pos:]))
    bank = PtrBank(flags, wbk, waves, loops, books, d[notes:notes + n],
                   [struct.unpack_from(">f", d, det + 4 * i)[0] for i in range(n)],
                   d[det:det + 4 * n], layout, len(d))
    return bank


def _item_len(bank: PtrBank, kind, ident) -> int:
    n = len(bank.waves)
    if kind == "pad":
        return len(ident)
    return {"hdr": 0x30, "wave": 20, "wlist": 4 * n, "notes": n, "detune": 4 * n}.get(kind) or (
        len(bank.loops[ident].pack()) if kind == "loop" else len(bank.books[ident].pack()))


def build_ptrbank(bank: PtrBank) -> bytes:
    offs, pos = {}, 0
    for kind, ident in bank.layout:
        if kind != "pad":
            offs[(kind, ident)] = pos
        pos += _item_len(bank, kind, ident)
    n = len(bank.waves)
    out = bytearray()
    for kind, ident in bank.layout:
        if kind == "pad":
            out += ident
        elif kind == "hdr":
            out += PTR_MAGIC + struct.pack(">I", bank.flags) + bank.wbk_name
            out += struct.pack(">4I", n, offs[("notes", None)], offs[("detune", None)], offs[("wlist", None)])
        elif kind == "wave":
            w = bank.waves[ident]
            lp = offs[("loop", w.loop_id)] if w.loop_id is not None else 0
            bk = offs[("book", w.book_id)] if w.book_id is not None else 0
            out += struct.pack(">IIBBHII", w.base, w.length, w.type, w.flags, w.pad, lp, bk)
        elif kind == "loop":
            out += bank.loops[ident].pack()
        elif kind == "book":
            out += bank.books[ident].pack()
        elif kind == "wlist":
            out += b"".join(struct.pack(">I", offs[("wave", i)]) for i in range(n))
        elif kind == "notes":
            out += bank.base_notes
        elif kind == "detune":
            out += bank.detune_raw
    return bytes(out)


# ----------------------------------------------------------------------------
# Wave table
# ----------------------------------------------------------------------------

def split_wavetable(d: bytes, bank: PtrBank) -> list[tuple]:
    """Ordered parts: ("header", bytes) ("sample", wave index, bytes) ("fill", bytes)."""
    need(d[:16] == WAVE_MAGIC, "WaveTables magic")
    parts, pos = [("header", d[:16])], 16
    for w in sorted(bank.waves, key=lambda w: w.base):
        need(w.base >= pos and w.base + w.length <= len(d), f"wave {w.index} range")
        if w.base > pos:
            parts.append(("fill", d[pos:w.base]))
        parts.append(("sample", w.index, d[w.base:w.base + w.length]))
        pos = w.base + w.length
    if pos < len(d):
        parts.append(("fill", d[pos:]))
    return parts


def build_wavetable(parts: list[tuple]) -> bytes:
    return b"".join(p[-1] for p in parts)


# ----------------------------------------------------------------------------
# Effects bank
# ----------------------------------------------------------------------------

@dataclass
class FxBank:
    count: int
    field_04: int
    wave_count: int
    field_0c: int
    field_10: int
    entries: list[tuple[int, int]]      # (stream offset, second word)
    streams: list[list[tuple]]          # decoded command tokens per effect (audio_songs.tokenize)
    wave_map: list[int]                 # u16 indices into pointer bank 1
    tail: bytes
    gaps: list[bytes]                   # undecoded bytes between a stream's stop and the next item


def parse_fx(d: bytes) -> FxBank:
    n, f4, nw, fc, f10, wmap = struct.unpack_from(">6I", d, 0)
    need(0 < n < 1024 and fc == 0 and f10 == 0, "fx header")
    ent = [struct.unpack_from(">II", d, 0x18 + 8 * i) for i in range(n)]
    need(ent[0][0] == 0x18 + 8 * n, "fx stream base follows table")
    offs = [e[0] for e in ent]
    need(offs == sorted(offs) and offs[-1] < wmap <= len(d) - 2 * nw, "fx offsets")
    bounds = offs + [wmap]
    # effect channels start with velocity mode off (only song start sets it, 0x8003E36C)
    streams, gaps = [], []
    for a, b in zip(bounds, bounds[1:]):
        toks, q = audio_songs.tokenize(d, a, b, False, until_stop=True)
        need(q <= b, f"fx stream at 0x{a:X} overruns")
        streams.append(toks)
        gaps.append(d[q:b])          # bytes after the stop command (only the last stream: 1 byte)
    wm = list(struct.unpack_from(f">{nw}H", d, wmap))
    return FxBank(n, f4, nw, fc, f10, ent, streams, wm, d[wmap + 2 * nw:], gaps)


def build_fx(fx: FxBank) -> bytes:
    base = 0x18 + 8 * fx.count
    blobs = [audio_songs.emit(t) + g for t, g in zip(fx.streams, fx.gaps)]
    offs, p = [], base
    for s in blobs:
        offs.append(p)
        p += len(s)
    out = struct.pack(">6I", fx.count, fx.field_04, len(fx.wave_map), fx.field_0c, fx.field_10, p)
    out += b"".join(struct.pack(">II", o, e[1]) for o, e in zip(offs, fx.entries))
    out += b"".join(blobs) + struct.pack(f">{len(fx.wave_map)}H", *fx.wave_map) + fx.tail
    return out


# ----------------------------------------------------------------------------
# VADPCM
# ----------------------------------------------------------------------------

def _wrap32(x):
    return ((x + 0x80000000) & 0xFFFFFFFF) - 0x80000000


def _half(coefs, order, base, prev, nibs, scale):
    res = [n << scale for n in nibs]
    last = base + (order - 1) * 8
    cur = []
    for i in range(8):
        acc = res[i] << 11
        for k in range(order):
            acc += coefs[base + k * 8 + i] * prev[8 - order + k]
        for j in range(i):
            acc += coefs[last + j] * res[i - 1 - j]
        cur.append(max(-32768, min(32767, _wrap32(acc) >> 11)))
    return cur


def decode_vadpcm(data: bytes, book: BookRec) -> list[int]:
    st, out = [0] * 8, []
    for f in range(len(data) // 9):
        p = 9 * f
        h = data[p]
        scale, pred = h >> 4, h & 15
        need(pred < book.npred, f"frame {f}: predictor {pred} >= {book.npred}")
        for half in range(2):
            nibs = []
            for i in range(8):
                b = data[p + 1 + half * 4 + (i >> 1)]
                v = b >> 4 if not i & 1 else b & 15
                nibs.append(v - 16 if v >= 8 else v)
            st = _half(book.coefs, book.order, pred * book.order * 8, st, nibs, scale)
            out.extend(st)
    return out


def _encode_half(coefs, order, base, prev, target, scale):
    """Find nibbles reproducing target exactly (sequential solve), or None."""
    nibs = []
    last = base + (order - 1) * 8
    for i in range(8):
        part = 0
        for k in range(order):
            part += coefs[base + k * 8 + i] * prev[8 - order + k]
        for j in range(i):
            part += coefs[last + j] * (nibs[i - 1 - j] << scale)
        want = (target[i] << 11) - part
        guess = round(want / (2048 << scale))
        hit = None
        for n in (guess, guess - 1, guess + 1):
            if -8 <= n <= 7 and max(-32768, min(32767, _wrap32(part + ((n << scale) << 11)) >> 11)) == target[i]:
                hit = n
                break
        if hit is None:
            return None
        nibs.append(hit)
    return nibs


def reencode_frame(book: BookRec, state: list[int], pcm16: list[int]) -> list[bytes]:
    """All 9-byte frames (any predictor, scale 0..12) that decode to pcm16 from state."""
    found = []
    for pred in range(book.npred):
        base = pred * book.order * 8
        for scale in range(13):
            a = _encode_half(book.coefs, book.order, base, state, pcm16[:8], scale)
            if a is None:
                continue
            b = _encode_half(book.coefs, book.order, base, pcm16[:8], pcm16[8:], scale)
            if b is None:
                continue
            nb = [(x & 15) for x in a + b]
            found.append(bytes([scale << 4 | pred] + [nb[2 * i] << 4 | nb[2 * i + 1] for i in range(8)]))
    return found


def reencode_test(data: bytes, book: BookRec, step: int) -> dict:
    """Decode, then for every `step`-th frame search all encodings of the decoded PCM."""
    pcm = decode_vadpcm(data, book)
    tested = unique_exact = ambiguous_exact = missing = 0
    for f in range(0, len(data) // 9, step):
        st = pcm[16 * f - 8:16 * f] if f else [0] * 8
        cands = reencode_frame(book, st, pcm[16 * f:16 * f + 16])
        orig = data[9 * f:9 * f + 9]
        tested += 1
        if orig not in cands:
            missing += 1
        elif len(cands) == 1:
            unique_exact += 1
        else:
            ambiguous_exact += 1
    return {"tested": tested, "unique": unique_exact, "ambiguous": ambiguous_exact, "missing": missing}


def wav_bytes(samples: list[int], rate: int) -> bytes:
    body = struct.pack(f"<{len(samples)}h", *samples)
    fmt = struct.pack("<HHIIHH", 1, 1, rate, rate * 2, 2, 16)
    return (b"RIFF" + struct.pack("<I", 36 + len(body)) + b"WAVEfmt " + struct.pack("<I", 16) + fmt
            + b"data" + struct.pack("<I", len(body)) + body)


# ----------------------------------------------------------------------------
# Whole area
# ----------------------------------------------------------------------------

def parse_all(rom: bytes) -> dict:
    p1 = parse_ptrbank(rom[PTR1[0]:PTR1[1]])
    p2 = parse_ptrbank(rom[PTR2[0]:PTR2[1]])
    w1 = split_wavetable(rom[WAV1[0]:WAV1[1]], p1)
    w2 = split_wavetable(rom[WAV2[0]:WAV2[1]], p2)
    fx = parse_fx(rom[FX1[0]:FX1[1]])
    need(max(fx.wave_map) < len(p1.waves), "fx wave map exceeds pointer bank 1")
    return {"ptr1": p1, "ptr2": p2, "wav1": w1, "wav2": w2, "fx": fx}


def rebuild_area(rom: bytes) -> bytes:
    a = parse_all(rom)
    return (build_ptrbank(a["ptr1"]) + build_wavetable(a["wav1"]) + build_fx(a["fx"])
            + build_wavetable(a["wav2"]) + build_ptrbank(a["ptr2"]))


def samples(rom: bytes) -> list[dict]:
    a = parse_all(rom)
    fxset = set(a["fx"].wave_map)
    rows = []
    for name, pk, wk, (ws, _we) in (("bank1", "ptr1", "wav1", WAV1), ("bank2", "ptr2", "wav2", WAV2)):
        bank = a[pk]
        payload = {p[1]: p[2] for p in a[wk] if p[0] == "sample"}
        for w in bank.waves:
            data = rom[ws + w.base:ws + w.base + w.length]
            lp = bank.loops[w.loop_id] if w.loop_id is not None else None
            bk = bank.books[w.book_id] if w.book_id is not None else None
            rows.append({
                "bank": name, "index": w.index, "rom_start": ws + w.base, "rom_end": ws + w.base + w.length,
                "codec": "vadpcm" if w.type == 0 else "pcm16", "frames": w.length // 9 if w.type == 0 else "",
                "loop": f"{lp.start}-{lp.end}x{lp.count}" if lp else "",
                "book": f"book{w.book_id}:o{bk.order}n{bk.npred}" if bk else "",
                "base_note": bank.base_notes[w.index], "detune": bank.detunes[w.index],
                "fx_bank_ref": (w.index in fxset) if name == "bank1" else "",
                "sha256": sha(data), "verified": payload[w.index] == data,
                "_data": data, "_book": bk, "_loop": lp,
            })
    return rows


def roundtrip(rom: bytes) -> list[dict]:
    a = parse_all(rom)
    rows = []

    def row(s, e, cat, sub, owner, fmt, ext, reb, ok, ev):
        rows.append({"start": s, "end": e, "category": cat, "subtype": sub, "owner": owner, "format": fmt,
                     "extraction": ext, "rebuild": reb, "verified": bool(ok), "evidence": ev})

    for key, (s, e), sub in (("ptr1", PTR1, "pointer_bank_1"), ("ptr2", PTR2, "pointer_bank_2")):
        b = a[key]
        built = build_ptrbank(b)
        row(s, e, "audio", sub, "tools/audio.py", "libmus PtrTablesV2",
            f"{len(b.waves)} waves, {len(b.loops)} loops, {len(b.books)} books",
            "re-serialized from parsed structures, pointers recomputed", built == rom[s:e],
            RUNTIME[key])
    for key, (s, e), sub in (("wav1", WAV1, "wave_table_1"), ("wav2", WAV2, "wave_table_2")):
        pos = s
        bank = a["ptr1" if key == "wav1" else "ptr2"]
        for p in a[key]:
            data = p[-1]
            if p[0] == "header":
                row(pos, pos + 16, "audio", sub + "_header", "tools/audio.py", "WaveTables magic", "header",
                    "literal magic", data == rom[pos:pos + 16] == WAVE_MAGIC, RUNTIME[key])
            elif p[0] == "sample":
                w = bank.waves[p[1]]
                row(pos, pos + len(data), "audio", "sample_vadpcm" if w.type == 0 else "sample_pcm16",
                    "tools/audio.py", "VADPCM 9-byte frames" if w.type == 0 else "PCM16BE",
                    f"{sub} wave {p[1]}", "original encoded bytes preserved",
                    data == rom[pos:pos + len(data)], f"pointer bank wave record {p[1]}")
            else:
                z = data.count(0) == len(data)
                row(pos, pos + len(data), "audio", "fill_zero" if z else "fill_nonzero", "tools/audio.py",
                    "fill", "preserved fill bytes", "literal", data == rom[pos:pos + len(data)],
                    "between/after referenced samples" + ("" if z else "; nonzero bytes, not referenced"))
            pos += len(data)
        assert pos == e
    s, e = FX1
    row(s, e, "audio", "effects_bank", "tools/audio.py", "libmus fx bank",
        f"{a['fx'].count} effects, {len(a['fx'].wave_map)} wave map entries",
        "re-serialized from decoded commands; offsets recomputed", build_fx(a["fx"]) == rom[s:e], RUNTIME["fx1"])
    rows.sort(key=lambda r: r["start"])
    return rows


def used_by(rom: bytes, songs: dict[int, bytes]) -> dict[tuple[str, int], list[str]]:
    """(bank, wave) -> users. Effects: 0x81 wave slot -> fx wave map -> bank 1 wave.
    Songs: 0x81 wave slot or drum-record slot -> song wave table -> bank 2 wave."""
    a = parse_all(rom)
    out: dict[tuple[str, int], list[str]] = {}
    fx = a["fx"]
    for i, toks in enumerate(fx.streams):
        for slot in sorted({t[2][0][0] for t in toks if t[0] == "cmd" and t[1] == 0x81}):
            out.setdefault(("bank1", fx.wave_map[slot]), []).append(f"fx{i}")
    for idx, d in sorted(songs.items()):
        sm = audio_songs.song_summary(audio_songs.parse_song(d))
        for w in sm["bank_waves"]:
            out.setdefault(("bank2", w), []).append(f"song{idx}")
    return out


def song_wave_refs(paths: list[str]) -> dict[int, int]:
    """Count libmus songs (version 0x215) whose wave table names each wave id."""
    refs: dict[int, int] = {}
    for p in paths:
        d = open(p, "rb").read()
        if len(d) < 0x38 or u32(d, 0) != 0x215:
            continue
        nw, wt = u32(d, 8), u32(d, 0x20)
        if not wt or wt + 2 * nw > len(d):
            continue
        for i in {struct.unpack_from(">H", d, wt + 2 * k)[0] for k in range(nw)} - {0xFFFF}:
            refs[i] = refs.get(i, 0) + 1
    return refs


def main(argv):
    import argparse
    import glob
    ap = argparse.ArgumentParser()
    ap.add_argument("rom")
    ap.add_argument("--wav", action="store_true", help="write diagnostic WAVs to private_extracted/audio/")
    ap.add_argument("--reencode-step", type=int, default=0, help="test every Nth frame re-encoding")
    ap.add_argument("--inventory", action="store_true", help="write inventory/audio_*.tsv")
    args = ap.parse_args(argv)
    rom = open(args.rom, "rb").read()
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    rows = roundtrip(rom)
    cov = sum(r["end"] - r["start"] for r in rows)
    ok = sum(r["end"] - r["start"] for r in rows if r["verified"])
    contiguous = all(x["end"] == y["start"] for x, y in zip(rows, rows[1:]))
    area_ok = rebuild_area(rom) == rom[AREA[0]:AREA[1]]
    print(f"rows={len(rows)} covered=0x{rows[0]['start']:X}..0x{rows[-1]['end']:X} bytes={cov} "
          f"verified_bytes={ok} contiguous={contiguous} full_area_rebuild_exact={area_ok}")
    sm = samples(rom)
    print(f"samples={len(sm)} verified={sum(s['verified'] for s in sm)} "
          f"bytes={sum(s['rom_end'] - s['rom_start'] for s in sm)}")
    if args.inventory:
        a = parse_all(rom)
        ub = used_by(rom, audio_songs.load_archive2(root))
        refs = song_wave_refs(sorted(glob.glob(os.path.join(root, "private_extracted/archives/resource_archive_2/*.bin"))))
        inv = os.path.join(root, "inventory")
        os.makedirs(inv, exist_ok=True)
        cols = ["bank", "index", "rom_start", "rom_end", "size", "codec", "rate", "frames", "loop", "book",
                "base_note", "detune", "fx_bank_ref", "archive2_song_refs", "used_by", "sha256", "verified"]
        with open(os.path.join(inv, "audio_samples.tsv"), "w") as f:
            f.write("\t".join(cols) + "\n")
            for s in sm:
                s = dict(s, size=s["rom_end"] - s["rom_start"], rate="unknown",
                         archive2_song_refs=refs.get(s["index"], 0) if s["bank"] == "bank2" else "",
                         used_by=",".join(ub.get((s["bank"], s["index"]), [])) or "none_found")
                s["rom_start"], s["rom_end"] = f"0x{s['rom_start']:06X}", f"0x{s['rom_end']:06X}"
                f.write("\t".join(str(s[c]) for c in cols) + "\n")
        with open(os.path.join(inv, "audio_banks.tsv"), "w") as f:
            f.write("name\trom_start\trom_end\tsize\tformat\tcontents\tsha256\trebuild_exact\truntime_evidence\n")
            for r in rows:
                if r["subtype"].startswith("sample") or r["subtype"].startswith("fill"):
                    continue
                f.write(f"{r['subtype']}\t0x{r['start']:06X}\t0x{r['end']:06X}\t{r['end'] - r['start']}\t"
                        f"{r['format']}\t{r['extraction']}\t{sha(rom[r['start']:r['end']])}\t{r['verified']}\t{r['evidence']}\n")
            for name, (s, e) in (("wave_table_1", WAV1), ("wave_table_2", WAV2)):
                parts = a["wav1" if name.endswith("1") else "wav2"]
                nf = sum(len(p[-1]) for p in parts if p[0] == "fill")
                f.write(f"{name}\t0x{s:06X}\t0x{e:06X}\t{e - s}\tN64 WaveTables\t"
                        f"{sum(p[0] == 'sample' for p in parts)} samples, {nf} fill bytes\t{sha(rom[s:e])}\t"
                        f"{build_wavetable(parts) == rom[s:e]}\t{RUNTIME['wav1' if name.endswith('1') else 'wav2']}\n")
        with open(os.path.join(inv, "audio_sequences.tsv"), "w") as f:
            f.write("name\trom_start\trom_end\tformat\tchannels\twave_bank\tsha256\tverified\tevidence\n")
    if args.wav:
        out = os.path.join(root, "private_extracted", "audio")
        os.makedirs(out, exist_ok=True)
        for s in sm:
            pcm = decode_vadpcm(s["_data"], s["_book"]) if s["codec"] == "vadpcm" else \
                list(struct.unpack(f">{len(s['_data']) // 2}h", s["_data"][:len(s["_data"]) // 2 * 2]))
            with open(os.path.join(out, f"{s['bank']}_{s['index']:03d}.wav"), "wb") as f:
                f.write(wav_bytes(pcm, 22000))
        print(f"wrote {len(sm)} diagnostic WAVs (22000 Hz nominal) to {out}")
    if args.reencode_step:
        tot = {"tested": 0, "unique": 0, "ambiguous": 0, "missing": 0}
        for s in sm:
            if s["codec"] == "vadpcm":
                for k, v in reencode_test(s["_data"], s["_book"], args.reencode_step).items():
                    tot[k] += v
        print("reencode", tot)
    return 0 if ok == cov and area_ok and contiguous else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
