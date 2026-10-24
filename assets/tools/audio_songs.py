#!/usr/bin/env python3
"""libmus song files (version 0x215) and the shared libmus command-stream codec.

Command semantics come from the game's own libmus player: the 45-entry handler
table at ROM 0x56630 (VRAM 0x80055A30) for opcodes 0x80..0xAC, the note reader at
0x8003CB40, the volume/pitch-bend stream readers at 0x8003D6E4 / 0x8003D7A8 and
the song start routine around 0x8003E36C (see formats/audio.md).

Song layout: header 0x38 bytes, then offset-addressed tables and streams. The
rebuilder lays items out in the parsed order and recomputes every header/table
offset and every goto (0xA1) displacement from token indices.

CLI: python3 tools/audio_songs.py [--midi] [--inventory]
"""
from __future__ import annotations

import hashlib
import os
import struct
import sys

SONG_VERSION = 0x215
ARCHIVE2_SONGS = range(504, 529)

# opcode -> (name, argument kinds); "u8", "var" (1 or 2 bytes, top bit = 2-byte), "goto"
COMMANDS = {
    0x80: ("stop", []), 0x81: ("wave", ["var"]), 0x82: ("port", ["u8"]), 0x83: ("port_off", []),
    0x84: ("define_envelope", ["u8"] * 7), 0x85: ("tempo", ["u8"]), 0x86: ("cutoff", ["u8", "u8"]),
    0x87: ("endit", ["u8"]), 0x88: ("vib_up", ["u8"] * 3), 0x89: ("vib_down", ["u8"] * 3),
    0x8A: ("vib_off", []), 0x8B: ("fixed_length", ["var"]), 0x8C: ("ignore_length", []),
    0x8D: ("transpose", ["u8"]), 0x8E: ("ignore_transpose", []), 0x8F: ("distort", ["u8"]),
    0x90: ("envelope", ["var"]), 0x91: ("env_off", []), 0x92: ("env_on", []),
    0x93: ("trigger_off", []), 0x94: ("trigger_on", []), 0x95: ("for", ["u8"]), 0x96: ("next", []),
    0x97: ("wobble", ["u8"] * 3), 0x98: ("wobble_off", []), 0x99: ("velocity_on", []),
    0x9A: ("velocity_off", []), 0x9B: ("velocity", ["u8"]), 0x9C: ("pan", ["u8"]),
    0x9D: ("stereo", ["u8", "u8"]), 0x9E: ("drums", ["var"]), 0x9F: ("drums_off", []),
    0xA0: ("print", ["u8"]), 0xA1: ("goto", ["goto"]), 0xA2: ("reverb", ["u8"]),
    0xA3: ("rand_note", ["u8", "u8"]), 0xA4: ("rand_volume", ["u8", "u8"]),
    0xA5: ("rand_pan", ["u8", "u8"]), 0xA6: ("volume", ["u8"]), 0xA7: ("start_fx", ["var"]),
    0xA8: ("bend_range", ["u8"]), 0xA9: ("sweep", ["u8"]), 0xAA: ("change_fx", ["u8"]),
    0xAB: ("marker", ["u8", "var"]), 0xAC: ("fixed_length_off", []),
}


class SongError(ValueError):
    pass


def need(c, msg):
    if not c:
        raise SongError(msg)


def u32(b, o):
    return struct.unpack_from(">I", b, o)[0]


def _var(d, p):
    b = d[p]
    if b & 0x80:
        return ((b & 0x7F) << 8) | d[p + 1], True, p + 2
    return b, False, p + 1


def _pvar(v, wide):
    return bytes([0x80 | v >> 8, v & 0xFF]) if wide else bytes([v])


# ----------------------------------------------------------------------------
# Channel command streams
# ----------------------------------------------------------------------------
# token: ("note", key, vel|None, (len, wide)|None, offset)
#        ("cmd", op, [args], offset)  arg = int | (v, wide) | ("goto", ch, vol, pitch)

def tokenize(d: bytes, start: int, end: int, velocity_on: bool, until_stop: bool = False):
    """Decode commands in d[start:end]. With until_stop, stop after the first 0x80 and
    return (tokens, end_position) instead of requiring exact consumption."""
    toks, p = [], start
    vel, fixed, ignore = velocity_on, 0, False
    while p < end:
        at = p
        op = d[p]
        p += 1
        if op < 0x80:
            v = None
            if vel:
                v = d[p]
                p += 1
                if v >= 0x80:
                    vel = False
            ln = None
            if not fixed or ignore:
                val, wide, p = _var(d, p)
                ln = (val, wide)
                if fixed:
                    ignore = False
            toks.append(("note", op, v, ln, at))
            continue
        need(op in COMMANDS, f"bad opcode 0x{op:02X} at 0x{at:X}")
        args = []
        for k in COMMANDS[op][1]:
            if k == "u8":
                args.append(d[p])
                p += 1
            elif k == "var":
                v, wide, p = _var(d, p)
                args.append((v, wide))
            else:
                args.append(("goto",) + struct.unpack_from(">HHH", d, p))
                p += 6
        if op == 0x8B:
            fixed = args[0][0]
        elif op == 0xAC:
            fixed = 0
        elif op == 0x8C:
            ignore = True
        elif op == 0x99:
            vel = True
        elif op in (0x9A, 0x9B):
            vel = False
        toks.append(("cmd", op, args, at))
        if until_stop and op == 0x80:
            return toks, p
    if until_stop:
        raise SongError("no stop command")
    need(p == end, f"stream overruns boundary 0x{end:X} (0x{p:X})")
    return toks


def emit(toks: list[tuple], resolve=None) -> bytes:
    out = bytearray()
    for t in toks:
        if t[0] == "note":
            out.append(t[1])
            if t[2] is not None:
                out.append(t[2])
            if t[3] is not None:
                out += _pvar(*t[3])
        else:
            out.append(t[1])
            for a in t[2]:
                if isinstance(a, int):
                    out.append(a)
                elif a[0] == "goto":
                    g = resolve(a) if resolve else a[1:]
                    out += struct.pack(">HHH", *g)
                else:
                    out += _pvar(*a)
    return bytes(out)


def vol_tokenize(d: bytes, start: int, end: int) -> list[tuple]:
    """Volume / pitch-bend streams: value byte; top bit set -> duration var follows (+2)."""
    toks, p = [], start
    while p < end:
        at, b = p, d[p]
        p += 1
        if b & 0x80:
            v, wide, p = _var(d, p)
            toks.append((b & 0x7F, (v, wide), at))
        else:
            toks.append((b, None, at))
    need(p == end, f"volume stream overruns 0x{end:X}")
    return toks


def vol_emit(toks) -> bytes:
    out = bytearray()
    for v, dur, _ in toks:
        if dur is None:
            out.append(v)
        else:
            out.append(0x80 | v)
            out += _pvar(*dur)
    return bytes(out)


def stream_stats(toks) -> dict:
    notes = [t for t in toks if t[0] == "note"]
    waves = sorted({t[2][0][0] for t in toks if t[0] == "cmd" and t[1] == 0x81})
    return {"events": len(toks), "notes": len(notes), "wave_cmds": waves,
            "drum_cmds": sorted({t[2][0][0] for t in toks if t[0] == "cmd" and t[1] == 0x9E}),
            "env_cmds": sorted({t[2][0][0] for t in toks if t[0] == "cmd" and t[1] == 0x90}),
            "fx_cmds": sorted({t[2][0][0] for t in toks if t[0] == "cmd" and t[1] == 0xA7})}


# ----------------------------------------------------------------------------
# Song file
# ----------------------------------------------------------------------------

HEADER_FIELDS = ("channels_table", "volume_table", "pitch_table", "envelopes", "drums", "wave_table", "master")


def parse_song(d: bytes) -> dict:
    need(len(d) >= 0x38 and u32(d, 0) == SONG_VERSION, "song version")
    nch, nw = u32(d, 4), u32(d, 8)
    offs = dict(zip(HEADER_FIELDS, struct.unpack_from(">7I", d, 0x0C)))
    flags, res = u32(d, 0x28), d[0x2C:0x38]
    need(1 <= nch <= 32 and nw < 1024, "song counts")
    items = {}                                   # offset -> (kind, ident)

    def add(off, kind, ident=None):
        need(0 < off <= len(d), f"{kind} offset 0x{off:X}")
        if off in items:
            if kind in ("envelopes", "drums") and items[off][0] in ("envelopes", "drums"):
                return                   # empty table sharing the next table's offset
            need(items[off][0] == kind, f"shared offset 0x{off:X} {kind}/{items[off][0]}")
            return
        items[off] = (kind, ident)

    items[0] = ("header", None)
    for k in ("channels_table", "volume_table", "pitch_table", "envelopes", "drums", "wave_table", "master"):
        add(offs[k], k)
    tabs = {k: [u32(d, offs[k] + 4 * c) for c in range(nch)]
            for k in ("channels_table", "volume_table", "pitch_table")}
    for kind, k in (("channel", "channels_table"), ("volume", "volume_table"), ("pitch", "pitch_table")):
        for o in tabs[k]:
            if o:
                add(o, kind, o)
    order = sorted(items)
    bounds = dict(zip(order, order[1:] + [len(d)]))
    fixed_len = {"header": 0x38, "channels_table": 4 * nch, "volume_table": 4 * nch,
                 "pitch_table": 4 * nch, "wave_table": 2 * nw}
    body = {}
    for off in order:
        kind, _ = items[off]
        end = bounds[off]
        if kind in fixed_len:
            used = fixed_len[kind]
        elif kind in ("envelopes", "drums"):
            size = 7 if kind == "envelopes" else 6
            used = (end - off) // size * size
            body[off] = [d[off + i:off + i + size] for i in range(0, used, size)]
        elif kind in ("channel", "master"):
            body[off] = tokenize(d, off, end, True)
            used = end - off
        else:
            body[off] = vol_tokenize(d, off, end)
            used = end - off
        need(off + used <= end, f"{kind} at 0x{off:X} overlaps next item")
        body[(off, "pad")] = d[off + used:end]
    waves = list(struct.unpack_from(f">{nw}H", d, offs["wave_table"]))
    song = {"channels": nch, "flags": flags, "reserved": res, "offsets": offs, "tables": tabs,
            "waves": waves, "order": order, "items": items, "body": body, "size": len(d)}
    _link_gotos(song)
    return song


def _link_gotos(song):
    """Replace goto displacements by (channel stream, token index) references."""
    items, body = song["items"], song["body"]
    starts = {k: {o: i for i, (o2) in enumerate([0]) for o in []} for k in ()}  # placeholder
    del starts
    for off, (kind, _) in items.items():
        if kind not in ("channel", "master"):
            continue
        ch = [c for c, o in enumerate(song["tables"]["channels_table"]) if o == off]
        vbase = [song["tables"]["volume_table"][c] for c in ch] or [0]
        pbase = [song["tables"]["pitch_table"][c] for c in ch] or [0]
        idx = {t[-1] - off: i for i, t in enumerate(body[off])}
        new = []
        for t in body[off]:
            if t[0] == "cmd" and t[1] == 0xA1:
                _, cd, vd, pd = t[2][0]
                need(cd in idx, f"goto into mid-token at 0x{off + cd:X}")
                link = ["goto", ("tok", idx[cd]), vd, pd, vbase[0], pbase[0]]
                t = ("cmd", 0xA1, [tuple(link)], t[3])
            new.append(t)
        body[off] = new


def build_song(song: dict) -> bytes:
    items, body, nch = song["items"], song["body"], song["channels"]
    # compute stream bytes first (sizes do not depend on placement)
    blobs, newoff, pos = {}, {}, 0
    for off in song["order"]:
        kind, _ = items[off]
        newoff[off] = pos
        if kind == "header":
            n = 0x38
        elif kind in ("channels_table", "volume_table", "pitch_table"):
            n = 4 * nch
        elif kind == "wave_table":
            n = 2 * len(song["waves"])
        elif kind in ("envelopes", "drums"):
            blobs[off] = b"".join(body[off])
            n = len(blobs[off])
        elif kind in ("channel", "master"):
            n = len(emit(body[off], lambda a: (0, 0, 0)))
        else:
            blobs[off] = vol_emit(body[off])
            n = len(blobs[off])
        pos += n + len(body[(off, "pad")])

    def remap(o):
        return newoff[o] if o else 0

    out = bytearray()
    for off in song["order"]:
        kind, _ = items[off]
        if kind == "header":
            b = struct.pack(">III", SONG_VERSION, nch, len(song["waves"]))
            b += struct.pack(">7I", *(remap(song["offsets"][k]) for k in HEADER_FIELDS))
            b += struct.pack(">I", song["flags"]) + song["reserved"]
        elif kind in ("channels_table", "volume_table", "pitch_table"):
            b = b"".join(struct.pack(">I", remap(o)) for o in song["tables"][kind])
        elif kind == "wave_table":
            b = struct.pack(f">{len(song['waves'])}H", *song["waves"])
        elif kind in ("channel", "master"):
            toks = body[off]
            rel, p = [], 0
            for t in toks:
                rel.append(p)
                p += len(emit([t], lambda a: (0, 0, 0)))

            def resolve(a, rel=rel):
                _, (_, ti), vd, pd, _vb, _pb = a
                return rel[ti], vd, pd
            b = emit(toks, resolve)
        else:
            b = blobs[off]
        out += b + body[(off, "pad")]
    return bytes(out)


def song_summary(song: dict) -> dict:
    ev = notes = 0
    wave_cmds, drums, envs = set(), set(), set()
    vol_events = 0
    for off, (kind, _) in song["items"].items():
        if kind in ("channel", "master"):
            s = stream_stats(song["body"][off])
            ev += s["events"]
            notes += s["notes"]
            wave_cmds |= set(s["wave_cmds"])
            drums |= set(s["drum_cmds"])
            envs |= set(s["env_cmds"])
        elif kind in ("volume", "pitch"):
            vol_events += len(song["body"][off])
    used = set()
    for w in wave_cmds:
        if w < len(song["waves"]):
            used.add(song["waves"][w])
    for off, (kind, _) in song["items"].items():
        if kind == "drums":
            for rec in song["body"][off]:
                slot = struct.unpack_from(">H", rec, 0)[0]
                if slot < len(song["waves"]):
                    used.add(song["waves"][slot])
    return {"events": ev, "notes": notes, "volume_pitch_events": vol_events,
            "wave_slots_used": sorted(wave_cmds), "bank_waves": sorted(used - {0xFFFF}),
            "wave_table": song["waves"],
            "envelopes": sum(len(song["body"][o]) for o, (k, _) in song["items"].items() if k == "envelopes"),
            "drums": sum(len(song["body"][o]) for o, (k, _) in song["items"].items() if k == "drums")}


# ----------------------------------------------------------------------------
# Diagnostic MIDI export (approximate timing: one tick per length unit)
# ----------------------------------------------------------------------------

def _vlq(n):
    b = [n & 0x7F]
    n >>= 7
    while n:
        b.insert(0, 0x80 | (n & 0x7F))
        n >>= 7
    return bytes(b)


def to_midi(song: dict) -> bytes:
    tracks = []
    for c, off in enumerate(song["tables"]["channels_table"]):
        if not off:
            continue
        ev, vel, fixed, prev = bytearray(), 100, 0, None
        delay = 0
        ch = c % 16
        for t in song["body"][off]:
            if t[0] == "cmd":
                if t[1] == 0x80:
                    break
                if t[1] == 0x8B:
                    fixed = t[2][0][0]
                elif t[1] == 0xAC:
                    fixed = 0
                elif t[1] == 0x81:
                    ev += _vlq(delay) + bytes([0xC0 | ch, t[2][0][0] & 0x7F])
                    delay = 0
                continue
            ln = t[3][0] if t[3] else fixed
            if t[2] is not None:
                vel = t[2] & 0x7F
            key = t[1]
            if key != 0x60:            # 0x60 appears as the rest key in these streams
                ev += _vlq(delay) + bytes([0x90 | ch, key & 0x7F, max(1, vel)])
                ev += _vlq(ln) + bytes([0x80 | ch, key & 0x7F, 0])
                delay = 0
            else:
                delay += ln
        ev += b"\x00\xFF\x2F\x00"
        tracks.append(b"MTrk" + struct.pack(">I", len(ev)) + bytes(ev))
    return b"MThd" + struct.pack(">IHHH", 6, 1, len(tracks), 48) + b"".join(tracks)


# ----------------------------------------------------------------------------

def songs_roundtrip(payload_by_index: dict[int, bytes]) -> list[dict]:
    rows = []
    for idx in sorted(payload_by_index):
        d = payload_by_index[idx]
        if len(d) < 4 or u32(d, 0) != SONG_VERSION:
            continue
        try:
            s = parse_song(d)
            built = build_song(s)
            sm = song_summary(s)
            ok = built == d
            ev = (f"channels={s['channels']} events={sm['events']} notes={sm['notes']} "
                  f"bank_waves={len(sm['bank_waves'])}")
        except (SongError, struct.error, IndexError) as e:
            ok, ev = False, f"parse failed: {e}"
        rows.append({"start": 0, "end": len(d), "category": "audio", "subtype": "libmus_song",
                     "owner": "tools/audio_songs.py", "format": "libmus song 0x215",
                     "extraction": f"resource_archive_2 record {idx}",
                     "rebuild": "re-serialized from tokens; offsets and gotos recomputed",
                     "verified": ok, "evidence": ev, "record": idx})
    return rows


def load_archive2(root: str) -> dict[int, bytes]:
    base = os.path.join(root, "private_extracted", "archives", "resource_archive_2")
    out = {}
    for i in ARCHIVE2_SONGS:
        p = os.path.join(base, f"{i:03d}.bin")
        if os.path.exists(p):
            out[i] = open(p, "rb").read()
    return out


def main(argv):
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument("--midi", action="store_true")
    ap.add_argument("--inventory", action="store_true")
    a = ap.parse_args(argv)
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    pay = load_archive2(root)
    rows = songs_roundtrip(pay)
    for r in rows:
        print(r["record"], r["verified"], r["evidence"])
    print(f"songs={len(rows)} exact={sum(r['verified'] for r in rows)} "
          f"bytes={sum(r['end'] for r in rows)} exact_bytes={sum(r['end'] for r in rows if r['verified'])}")
    if a.midi:
        out = os.path.join(root, "private_extracted", "audio", "songs")
        os.makedirs(out, exist_ok=True)
        for i, d in pay.items():
            with open(os.path.join(out, f"song_{i:03d}.mid"), "wb") as f:
                f.write(to_midi(parse_song(d)))
    if a.inventory:
        with open(os.path.join(root, "inventory", "audio_sequences.tsv"), "w") as f:
            f.write("name\tsource\tsize\tformat\tchannels\tevents\tnotes\tvolume_pitch_events\t"
                    "envelopes\tdrums\twave_bank\tbank_waves_used\tsha256\tverified\tevidence\n")
            for r in rows:
                s = parse_song(pay[r["record"]])
                sm = song_summary(s)
                f.write(f"song_{r['record']:03d}\tresource_archive_2/{r['record']:03d}.bin\t{r['end']}\t"
                        f"libmus song 0x215\t{s['channels']}\t{sm['events']}\t{sm['notes']}\t"
                        f"{sm['volume_pitch_events']}\t{sm['envelopes']}\t{sm['drums']}\tbank2\t"
                        f"{','.join(map(str, sm['bank_waves']))}\t"
                        f"{hashlib.sha256(pay[r['record']]).hexdigest()}\t{r['verified']}\t"
                        f"wave ids up to {max(s['waves'] or [0])}, only valid in pointer bank 2 (100 waves)\n")
    return 0 if all(r["verified"] for r in rows) else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
