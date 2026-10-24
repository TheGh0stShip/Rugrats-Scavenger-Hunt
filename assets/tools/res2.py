#!/usr/bin/env python3
"""Resource archive 2 record formats: parse, export, exact rebuild.

Families (see formats/res2.md for the byte evidence):

* ``mdlh``        model container: 0x18-byte header + rlz model stream (+ optional secondary rlz
                  streams). The model stream holds F3DEX display lists, a vertex array (segment 3)
                  and a texture block (segment 4).
* ``imagebank``   CI8 tile bank: 0x18 header, tile entries (16 or 12 bytes), 256-entry RGBA16 TLUT,
                  8-bit index pixels.
* ``gridmap``     64x64 u16 cell grid + 8 rectangle blocks (records 226..231, consumer 0x8000DACC).
* ``pairtable``   two tables of n x 4 s16 (semantic not proven).
* ``libmus_song`` header word 0x215 (parsed by tools/audio_songs.py when present).
* ``raw``         unparsed, kept verbatim.

Every record is rebuilt from its parsed form (nested streams re-encoded with rlz.encode) and the
stored bytes are recompressed when the archive codec is rlz, then compared with the ROM.
"""
import argparse
import hashlib
import os
import struct
import sys
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rlz  # noqa: E402
import archive  # noqa: E402

try:  # optional: display-list walker (analysis only, not needed for rebuild)
    sys.path.insert(0, os.environ.get("N64DK_ROOT", "../n64-decomp-kit"))
    from n64dk.assets import gbi  # type: ignore
except Exception:  # pragma: no cover
    gbi = None

ROOT = os.path.dirname(HERE)
ARCH = archive.ARCHIVES["resource_archive_2"]
OUT_PRIV = os.path.join(ROOT, "private_extracted", "res2")


def sha(b):
    return hashlib.sha256(b).hexdigest()


def be16(b, o):
    return struct.unpack_from(">H", b, o)[0]


def be32(b, o):
    return struct.unpack_from(">I", b, o)[0]


# --------------------------------------------------------------------------- family detection

def family(p: bytes) -> str:
    if not p:
        return "empty"
    if p[:4] == b"mdlh":
        return "mdlh"
    if len(p) >= 0x18:
        n, _c, _r, z, bpp = struct.unpack_from(">5H", p)
        po, pa, z2 = struct.unpack_from(">3H", p, 0xE)
        sz = be32(p, 0x14)
        if n and z == 0 and z2 == 0 and bpp == 8 and pa == 0x18 + n * ((pa - 0x18) // n) \
                and (pa - 0x18) // n in (12, 16) and po == pa + 0x200 and po + sz - 0x200 == len(p):
            return "imagebank"
    if len(p) > 0x850 and be32(p, 0) + 0x200 == len(p) and be32(p, 4) == 0:
        return "font"
    if len(p) >= 0x10 and be32(p, 0) == 0x215:
        return "libmus_song"
    if len(p) == 0x3094 and be32(p, 0) == 0:
        return "gridmap"
    if len(p) >= 0x10:
        n, n2, a, b = struct.unpack_from(">4I", p)
        if n and n == n2 and a == 0x10 and b == 0x10 + n * 8 and len(p) == 0x10 + n * 16:
            return "pairtable"
    return "raw"


# --------------------------------------------------------------------------- imagebank

def parse_imagebank(p):
    n, cols, rows, z, bpp, w, h, po, pa, z2 = struct.unpack_from(">10H", p)
    size = be32(p, 0x14)
    stride = (pa - 0x18) // n
    tiles = []
    for i in range(n):
        e = 0x18 + i * stride
        if stride == 16:
            tw, th, x, y = struct.unpack_from(">4H", p, e)
            off, tsz, pad = struct.unpack_from(">IHH", p, e + 8)
        else:
            tw, th = w, h
            x, y = struct.unpack_from(">2H", p, e)
            off, tsz, pad = struct.unpack_from(">IHH", p, e + 4)
        tiles.append({"w": tw, "h": th, "x": x, "y": y, "off": off, "size": tsz, "pad": pad,
                      "pixels": p[po + off:po + off + tsz]})
    covered = bytearray(len(p) - po)
    for t in tiles:
        covered[t["off"]:t["off"] + t["size"]] = b"\1" * t["size"]
    return {"count": n, "cols": cols, "rows": rows, "z": z, "bpp": bpp, "w": w, "h": h,
            "pix_off": po, "pal_off": pa, "z2": z2, "size": size, "stride": stride,
            "tiles": tiles, "palette": list(struct.unpack_from(">256H", p, pa)),
            # bytes of the pixel area not owned by any tile (kept explicitly; empty in retail)
            "fill": [(i, p[po + i]) for i in range(len(covered)) if not covered[i]]}


def build_imagebank(r):
    b = bytearray(r["pix_off"] + r["size"] - 0x200)
    struct.pack_into(">10H", b, 0, r["count"], r["cols"], r["rows"], r["z"], r["bpp"], r["w"], r["h"],
                     r["pix_off"], r["pal_off"], r["z2"])
    struct.pack_into(">I", b, 0x14, r["size"])
    for i, t in enumerate(r["tiles"]):
        e = 0x18 + i * r["stride"]
        if r["stride"] == 16:
            struct.pack_into(">4HIHH", b, e, t["w"], t["h"], t["x"], t["y"], t["off"], t["size"], t["pad"])
        else:
            struct.pack_into(">2HIHH", b, e, t["x"], t["y"], t["off"], t["size"], t["pad"])
        b[r["pix_off"] + t["off"]:r["pix_off"] + t["off"] + t["size"]] = t["pixels"]
    struct.pack_into(">256H", b, r["pal_off"], *r["palette"])
    for i, v in r["fill"]:
        b[r["pix_off"] + i] = v
    return bytes(b)


# --------------------------------------------------------------------------- indexed PNG codec

def _chunk(kind, data):
    return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data) & 0xFFFFFFFF)


def rgba16_to_rgba8(v):
    f = lambda c: (c << 3) | (c >> 2)  # noqa: E731
    return f(v >> 11), f((v >> 6) & 31), f((v >> 1) & 31), 255 if v & 1 else 0


def png_indexed(w, h, indices: bytes, pal16: list) -> bytes:
    """Palette PNG (bit depth 8); PLTE/tRNS carry the RGBA16 TLUT expanded to 8 bits."""
    raw = b"".join(b"\0" + indices[y * w:(y + 1) * w] for y in range(h))
    plte = b"".join(bytes(rgba16_to_rgba8(v)[:3]) for v in pal16)
    trns = bytes(rgba16_to_rgba8(v)[3] for v in pal16)
    return (b"\x89PNG\r\n\x1a\n" + _chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 3, 0, 0, 0))
            + _chunk(b"PLTE", plte) + _chunk(b"tRNS", trns) + _chunk(b"IDAT", zlib.compress(raw, 9))
            + _chunk(b"IEND", b""))


def png_read_indexed(data: bytes):
    """Inverse of png_indexed: returns (w, h, indices, rgba16 palette)."""
    pos, chunks = 8, {}
    idat = b""
    while pos < len(data):
        n = be32(data, pos)
        kind = data[pos + 4:pos + 8]
        body = data[pos + 8:pos + 8 + n]
        if kind == b"IDAT":
            idat += body
        else:
            chunks[kind] = body
        pos += 12 + n
    w, h, depth, ctype = struct.unpack_from(">IIBB", chunks[b"IHDR"])
    if depth != 8 or ctype != 3:
        raise ValueError("expected 8-bit palette PNG")
    raw = zlib.decompress(idat)
    rows = []
    for y in range(h):
        line = raw[y * (w + 1):(y + 1) * (w + 1)]
        if line[0] != 0:
            raise ValueError("unsupported PNG filter")
        rows.append(line[1:])
    plte, trns = chunks[b"PLTE"], chunks.get(b"tRNS", b"")
    pal = []
    for i in range(len(plte) // 3):
        r, g, b = plte[i * 3:i * 3 + 3]
        a = trns[i] if i < len(trns) else 255
        pal.append(((r >> 3) << 11) | ((g >> 3) << 6) | ((b >> 3) << 1) | (1 if a else 0))
    return w, h, b"".join(rows), pal


def ci4_unpack(data, w, h):
    row = (w + 1) // 2
    return bytes((data[y * row + (x >> 1)] >> (0 if x & 1 else 4)) & 15 for y in range(h) for x in range(w))


def ci4_pack(idx, w, h):
    out = bytearray()
    for y in range(h):
        r = idx[y * w:(y + 1) * w]
        for x in range(0, w, 2):
            out.append((r[x] << 4) | (r[x + 1] if x + 1 < w else 0))
    return bytes(out)


# --------------------------------------------------------------------------- mdlh

def parse_model_inner(m):
    nv = be16(m, 4)
    vo, ve, to = be32(m, 0xC), be32(m, 0x14), be32(m, 0x20)
    rep = {"vtx_count": nv, "vtx_off": vo, "vtx_end": ve, "tex_off": to}
    if nv and ve == vo + nv * 16 and vo <= ve <= len(m):
        rep["head"] = m[:vo]
        rep["vertices"] = [struct.unpack_from(">3hH2h4B", m, vo + i * 16) for i in range(nv)]
        rep["tail"] = m[ve:]
    else:
        rep["head"] = m
        rep["vertices"] = []
        rep["tail"] = b""
    return rep


def build_model_inner(r):
    return r["head"] + b"".join(struct.pack(">3hH2h4B", *v) for v in r["vertices"]) + r["tail"]


def walk_model(m, mesh=None):
    """Walk the mesh display list. Patch-table slots of kind 0 are runtime-written command
    slots; they are walked as G_SPNOOP (zero words) on a copy."""
    if gbi is None:
        return None
    start = 0x60
    if mesh is not None:
        start = mesh["dl"]
        b = bytearray(m)
        for ix, kind, _p in mesh["patches"]:
            if kind == 0:
                b[start + 8 * ix:start + 8 * ix + 8] = bytes(8)
        m = bytes(b)
    return gbi.walk(gbi.Memory(m, {0: 0, 3: be32(m, 0xC), 4: be32(m, 0x20)}), start, "f3dex")



def _sx(v, bits):
    return v - (1 << bits) if v & (1 << (bits - 1)) else v


def unpack_xyz(w):
    """Packed key vertex (func_8000B380): x bits 0..10, y bits 11..21, z bits 22..31, signed."""
    return _sx(w & 0x7FF, 11), _sx((w >> 11) & 0x7FF, 11), _sx(w >> 22, 10)


def pack_xyz(x, y, z):
    return (x & 0x7FF) | ((y & 0x7FF) << 11) | ((z & 0x3FF) << 22)


def vertex_runs(m):
    """Run list at inner+[0x14] (right after the vertex array): repeated
    {u16 animated count A; u16 copy count B; B x u16 source vertex index}, ended by A == 0.
    func_8000B380 writes A interpolated vertices, then copies B earlier output vertices."""
    q = be32(m, 0x14)
    runs = []
    while q + 2 <= len(m):
        a = be16(m, q)
        q += 2
        if a == 0:
            break
        b = be16(m, q)
        runs.append((a, [be16(m, q + 2 + 2 * k) for k in range(b)]))
        q += 2 + 2 * b
    return runs


def parse_anim(d, nanim=None):
    """Decoded secondary stream: vertex keyframe animation (consumer func_8000B380)."""
    if len(d) < 0x14:
        return {"raw": d}
    o0, o1, f8, ticks, nkeys, fe, z = struct.unpack_from(">IIHHHHI", d)
    if 0x14 + 2 * nkeys > len(d) or not (0x14 + 2 * nkeys <= o0 <= o1 <= len(d)):
        return {"raw": d}
    a = {"o0": o0, "o1": o1, "flags8": f8, "ticks": ticks, "nkeys": nkeys, "flagsE": fe, "z": z,
         "keys": list(struct.unpack_from(">%dH" % nkeys, d, 0x14)),
         "mid": d[0x14 + 2 * nkeys:o1], "frames": d[o1:]}
    # per-vertex split: nanim vertices x K packed words, K = keys (keys - 1 when looping, 0x80)
    k = nkeys - (1 if fe & 0x80 else 0)
    if nanim and k > 0 and nanim * k * 4 <= len(d) - o1 and not any(d[o1 + nanim * k * 4:]):
        body = d[o1:o1 + nanim * k * 4]
        words = struct.unpack(">%dI" % (nanim * k), body)
        a["vertices"] = [[unpack_xyz(w) for w in words[v * k:(v + 1) * k]] for v in range(nanim)]
        a["pad"] = len(d) - o1 - nanim * k * 4
        del a["frames"]
    return a


def build_anim(a):
    if "raw" in a:
        return a["raw"]
    return (struct.pack(">IIHHHHI", a["o0"], a["o1"], a["flags8"], a["ticks"], a["nkeys"], a["flagsE"], a["z"])
            + struct.pack(">%dH" % a["nkeys"], *a["keys"]) + a["mid"]
            + (a["frames"] if "frames" in a else
               b"".join(struct.pack(">I", pack_xyz(*t)) for v in a["vertices"] for t in v) + bytes(a["pad"])))


def anim_table(m, h, streams):
    """Match the animation table (anim header at inner+0x18, table at hdr+[hdr+0x1C], 16-byte
    entries {stream offset rel. word[4], stored bytes, decoded bytes, ticks}) to the streams."""
    s = be32(m, 0x18)
    if not s or s + 0x20 > len(m):
        return s, []
    t = s + be32(m, s + 0x1C)
    by_off = {off - h[4]: (data, ln) for off, data, ln in streams}
    rows = []
    k = 0
    while t + 16 * (k + 1) <= len(m):
        off, sl, ds, ticks = struct.unpack_from(">4I", m, t + 16 * k)
        if sl == 0 and off == 0 and ds == 0 and ticks == 0:
            rows.append((k, None))
        elif off in by_off and by_off[off][1] == sl and len(by_off[off][0]) == ds:
            rows.append((k, (off, sl, ds, ticks)))
        else:
            break
        k += 1
    while rows and rows[-1][1] is None:
        rows.pop()
    return t, rows


def parse_mesh(m):
    dl, mend, npatch, kind, pt, x, z = struct.unpack_from(">IIHHIII", m, 0x48)
    patches = [struct.unpack_from(">HHi", m, pt + 8 * k) for k in range(npatch)] if pt + 8 * npatch <= len(m) else []
    return {"dl": dl, "end": mend, "npatch": npatch, "u16b": kind, "patch_off": pt, "x": x, "patches": patches}


def parse_mdlh(p):
    h = list(struct.unpack_from(">6I", p))
    inner, end = rlz.decode(p, 0x18)
    rep = {"hdr": h, "model": parse_model_inner(inner), "streams": [], "fills": []}
    pos = end
    if h[4]:
        rep["fills"].append((pos, p[pos:h[4]]))
        pos = h[4]
        while pos < len(p):
            q = pos
            while q < len(p) and p[q] == 0:
                q += 1
            if q >= len(p):
                break
            if q > pos:
                rep["fills"].append((pos, p[pos:q]))
            data, e = rlz.decode(p, q)
            rep["streams"].append((q, data, e - q))
            pos = e
    if pos < len(p):
        rep["fills"].append((pos, p[pos:]))
    rep["size"] = len(p)
    rep["inner_len"] = len(inner)
    rep["runs"] = vertex_runs(inner) if h[4] else []
    rep["anim_vertices"] = sum(a for a, _b in rep["runs"])
    rep["anims"] = [parse_anim(d, rep["anim_vertices"]) for _q, d, _n in rep["streams"]]
    rep["mesh"] = parse_mesh(inner)
    rep["anim_hdr"], rep["anim_table"] = anim_table(inner, h, rep["streams"]) if h[4] else (0, [])
    return rep


def build_mdlh(r):
    b = bytearray(r["size"])
    struct.pack_into(">6I", b, 0, *r["hdr"])
    s = rlz.encode(build_model_inner(r["model"]))
    b[0x18:0x18 + len(s)] = s
    for (off, _data, _n), a in zip(r["streams"], r["anims"]):
        e = rlz.encode(build_anim(a))
        b[off:off + len(e)] = e
    for off, data in r["fills"]:
        b[off:off + len(data)] = data
    return bytes(b)


# --------------------------------------------------------------------------- gridmap / pairtable

def parse_gridmap(p):
    return {"head": list(struct.unpack_from(">10H", p)),
            "cells": list(struct.unpack_from(">4096H", p, 0x14)),
            "blocks": [(struct.unpack_from(">4h", p, 0x2014 + i * 0x210), p[0x2014 + i * 0x210 + 8:0x2014 + (i + 1) * 0x210])
                       for i in range(8)]}


def build_gridmap(r):
    b = struct.pack(">10H", *r["head"]) + struct.pack(">4096H", *r["cells"])
    return b + b"".join(struct.pack(">4h", *xy) + rest for xy, rest in r["blocks"])


def parse_pairtable(p):
    n = be32(p, 0)
    return {"n": n, "a": [struct.unpack_from(">4h", p, 0x10 + i * 8) for i in range(n)],
            "b": [struct.unpack_from(">4h", p, 0x10 + n * 8 + i * 8) for i in range(n)]}


def build_pairtable(r):
    n = r["n"]
    return (struct.pack(">4I", n, n, 0x10, 0x10 + n * 8) + b"".join(struct.pack(">4h", *v) for v in r["a"])
            + b"".join(struct.pack(">4h", *v) for v in r["b"]))


def parse_font(p):
    """Font (func_8001F588 loads record 0/1). Image: 8 strips at +0x850 + k*0x800 (strip loop
    at 0x8001F640..0x8001F808), each (+0x22) bytes per row = 2*(+0x22) 4-bit texels wide and
    (+0x0C) rows high. Glyph entry +0x50+8*ch: u8 advance, u8, u8 y offset (from +0x12),
    u8, u16 S (texel column), s16 strip index (-1 = no glyph); see 0x8001F8A0."""
    head = list(struct.unpack_from(">12I", p))
    rowb, h = be16(p, 0x22), head[3]
    strips, pads = [], []
    for k in range(8):
        o = 0x850 + k * 0x800
        strips.append(ci4_unpack(p[o:o + rowb * h], rowb * 2, h))
        pads.append(p[o + rowb * h:o + 0x800])
    return {"head": head, "palette": list(struct.unpack_from(">16H", p, 0x30)),
            "glyphs": [struct.unpack_from(">4BHh", p, 0x50 + 8 * k) for k in range(256)],
            "row_bytes": rowb, "height": h, "strips": strips, "pads": pads}


def build_font(r):
    out = bytearray(struct.pack(">12I", *r["head"]) + struct.pack(">16H", *r["palette"])
                    + b"".join(struct.pack(">4BHh", *g) for g in r["glyphs"]))
    for idx, pad in zip(r["strips"], r["pads"]):
        out += ci4_pack(idx, r["row_bytes"] * 2, r["height"]) + pad
    return bytes(out)


PARSERS = {"font": (parse_font, build_font), "imagebank": (parse_imagebank, build_imagebank), "mdlh": (parse_mdlh, build_mdlh),
           "gridmap": (parse_gridmap, build_gridmap), "pairtable": (parse_pairtable, build_pairtable)}

PROVEN = {"imagebank": "ci8_tile_bank", "mdlh": "f3dex_model", "gridmap": "grid_map",
          "pairtable": "camera_path", "font": "font"}


def _songs():
    try:
        import audio_songs  # type: ignore
        return audio_songs
    except Exception:
        return None


# --------------------------------------------------------------------------- driver

def records(rom):
    meta = archive.parse(rom, *ARCH)
    out = []
    for r in meta["records"]:
        stored = rom[r["rom_start"]:r["rom_end"]]
        payload = rlz.decode(rom, r["rom_start"])[0] if r["codec"] == "rlz" else stored
        out.append((r, stored, payload))
    return out


def analyse(rom, export=False, only=None):
    recs = records(rom)
    songs = _songs()
    song_rows = {}
    if songs is not None:
        try:
            got = songs.songs_roundtrip({r["index"]: p for r, _s, p in recs if family(p) == "libmus_song"})
            for row in got or []:
                k = row.get("index", row.get("record"))
                if k is not None:
                    song_rows[int(k)] = row
        except Exception as ex:  # keep going; rows fall back to verbatim rebuild
            song_rows = {"error": str(ex)}
    res = []
    for r, stored, payload in recs:
        i = r["index"]
        if only is not None and i not in only:
            continue
        fam = family(payload)
        info = {"index": i, "rom_start": r["rom_start"], "rom_end": r["rom_end"], "codec": r["codec"],
                "family": fam, "stored_sha256": sha(stored), "payload_sha256": sha(payload),
                "nested": fam == "mdlh", "proven": PROVEN.get(fam, "unproven"), "notes": ""}
        rep = None
        if fam in PARSERS:
            rep = PARSERS[fam][0](payload)
            rebuilt = PARSERS[fam][1](rep)
        else:
            rebuilt = payload
        if fam == "libmus_song":
            sr = song_rows.get(i) if isinstance(song_rows, dict) else None
            if sr is not None:
                info["proven"] = "libmus_song"
                info["notes"] = "audio_songs verified=%s" % sr.get("verified")
            else:
                info["notes"] = "audio_songs parser unavailable; payload kept verbatim"
        info["payload_match"] = rebuilt == payload
        if r["codec"] == "rlz":
            st = rlz.encode(rebuilt) + bytes.fromhex(r.get("stream_tail_hex", ""))
        else:
            st = rebuilt
        info["stored_match"] = st == stored
        info["verified"] = info["payload_match"] and info["stored_match"]
        if fam == "libmus_song" and isinstance(song_rows, dict) and i in song_rows:
            info["verified"] = info["verified"] and bool(song_rows[i].get("verified"))
        info["rep"] = rep
        if fam == "mdlh":
            info["walk"] = walk_model(build_model_inner(rep["model"]), rep["mesh"])
            if info["walk"] is not None and not info["walk"].ok:
                info["proven"] = "model_container"
                info["notes"] = "dl walk: " + "; ".join(info["walk"].errors[:2])
        res.append(info)
    if export:
        export_all(res)
    return res


def export_all(res):
    tdir = os.path.join(OUT_PRIV, "textures")
    os.makedirs(tdir, exist_ok=True)
    tex_rows, model_rows, anim_rows = [], [], []
    for info in res:
        rep, i = info["rep"], info["index"]
        if info["family"] == "imagebank":
            for k, t in enumerate(rep["tiles"]):
                name = f"r{i:03d}_t{k:02d}.png"
                png = png_indexed(t["w"], t["h"], t["pixels"], rep["palette"])
                with open(os.path.join(tdir, name), "wb") as fh:
                    fh.write(png)
                w, h, idx, pal = png_read_indexed(png)
                ok = idx == t["pixels"] and pal == rep["palette"] and (w, h) == (t["w"], t["h"])
                tex_rows.append([i, "bank", k, "CI8", t["w"], t["h"], f"0x{rep['pix_off'] + t['off']:X}",
                                 f"0x{rep['pal_off']:X}", 256, name, ok])
        if info["family"] == "font":
            w, h = rep["row_bytes"] * 2, rep["height"]
            for k, idx in enumerate(rep["strips"]):
                name = f"r{i:03d}_strip{k}.png"
                png = png_indexed(w, h, idx, rep["palette"])
                with open(os.path.join(tdir, name), "wb") as fh:
                    fh.write(png)
                w2, h2, idx2, pal2 = png_read_indexed(png)
                ok = ci4_pack(idx2, w2, h2) == ci4_pack(idx, w, h) and pal2 == rep["palette"]
                tex_rows.append([i, "font_strip", k, "4bit+TLUT16", w, h, f"0x{0x850 + k * 0x800:X}", "0x30", 16, name, ok])
            gdir = os.path.join(tdir, f"r{i:03d}_glyphs")
            os.makedirs(gdir, exist_ok=True)
            for ch, (adv, _b1, _yo, _b3, s0, strip) in enumerate(rep["glyphs"]):
                if strip < 0 or strip >= 8 or adv == 0 or s0 + adv > w:
                    continue
                crop = b"".join(rep["strips"][strip][y * w + s0:y * w + s0 + adv] for y in range(h))
                name = f"r{i:03d}_glyphs/c{ch:03d}.png"
                png = png_indexed(adv, h, crop, rep["palette"])
                with open(os.path.join(tdir, name), "wb") as fh:
                    fh.write(png)
                _w, _h, idx2, _p = png_read_indexed(png)
                back = all(rep["strips"][strip][y * w + s0:y * w + s0 + adv] == idx2[y * adv:(y + 1) * adv] for y in range(h))
                tex_rows.append([i, "font_glyph", ch, "4bit+TLUT16", adv, h, f"strip{strip}+S{s0}", "0x30", 16, name, back])
        if info["family"] == "mdlh":
            m = rep["model"]
            inner = build_model_inner(m)
            wk = info.get("walk")
            ntex = 0
            if wk is not None and wk.ok:
                for k, t in enumerate(wk.textures):
                    if t.fmt != 2 or t.width is None or t.palette_offset is None or t.offset is None:
                        continue
                    ntex += 1
                    w, h = t.width, t.height
                    if t.siz == 0:
                        nb = ((w + 1) // 2) * h
                        idx = ci4_unpack(inner[t.offset:t.offset + nb], w, h)
                        pal = list(struct.unpack_from(">16H", inner, t.palette_offset))
                    else:
                        nb = w * h
                        idx = inner[t.offset:t.offset + nb]
                        pal = list(struct.unpack_from(">256H", inner, t.palette_offset))
                    if len(idx) != w * h:
                        continue
                    name = f"r{i:03d}_m{k:02d}.png"
                    png = png_indexed(w, h, idx, pal)
                    with open(os.path.join(tdir, name), "wb") as fh:
                        fh.write(png)
                    w2, h2, idx2, pal2 = png_read_indexed(png)
                    back = ci4_pack(idx2, w2, h2) if t.siz == 0 else idx2
                    ok = back == inner[t.offset:t.offset + nb] and pal2 == pal
                    tex_rows.append([i, "model", k, t.format, w, h, f"0x{t.offset:X}", f"0x{t.palette_offset:X}",
                                     len(pal), name, ok])
            h4 = rep["hdr"][4]
            matched = {e[0]: (k, e) for k, e in rep["anim_table"] if e is not None}
            for q, (soff, data, ln) in enumerate(rep["streams"]):
                a = rep["anims"][q]
                ent = matched.get(soff - h4)
                if ent is None or "raw" in a:
                    continue
                k, (_o, sl, ds, ticks) = ent
                anim_rows.append([i, q, k, f"0x{soff:X}", sl, ds, a["ticks"], a["nkeys"], f"0x{a['flags8']:04X}",
                                  f"0x{a['flagsE']:04X}", ticks == a["ticks"], rep["anim_vertices"],
                                  "vertices" in a, len(a["vertices"][0]) if "vertices" in a else "",
                                  "vertex_keyframes", info["verified"]])
            ms = rep["mesh"]
            model_rows.append([i, m["vtx_count"], f"0x{m['vtx_off']:X}", f"0x{m['tex_off']:X}", info["inner_len"]
                               if "inner_len" in info else len(inner),
                               "" if wk is None else wk.ok, "" if wk is None else wk.triangles,
                               "" if wk is None else len(wk.dls), ntex, len(rep["streams"]),
                               sum(1 for _k, e in rep["anim_table"] if e is not None), f"0x{ms['dl']:X}",
                               ms["npatch"], ";".join(f"{ix}:{kd}" for ix, kd, _p in ms["patches"]),
                               info["verified"]])
    inv = os.path.join(ROOT, "inventory")
    os.makedirs(inv, exist_ok=True)

    def tsv(name, head, rows):
        with open(os.path.join(inv, name), "w") as fh:
            fh.write("\t".join(head) + "\n")
            for r in rows:
                fh.write("\t".join(str(x) for x in r) + "\n")

    tsv("textures.tsv", ["record", "source", "item", "format", "width", "height", "data_off", "palette_off",
                         "palette_entries", "png", "png_roundtrip_exact"], tex_rows)
    tsv("models.tsv", ["record", "vertices", "vtx_off", "tex_off", "inner_size", "dl_walk_ok", "triangles",
                       "display_lists", "ci_textures", "secondary_streams", "anim_table_entries", "dl_off", "patches",
                       "patch_slots(cmd_index:kind)", "verified"], model_rows)
    tsv("animations.tsv", ["record", "stream", "table_entry", "rec_off", "stored_bytes", "decoded_bytes", "ticks",
                           "keyframes", "flags08", "flags0E", "table_ticks_match", "animated_vertices",
                           "split_per_vertex", "words_per_vertex", "kind",
                           "verified"], anim_rows)
    tsv("scripts.tsv", ["record", "item", "kind", "evidence"], [])
    tsv("assets.tsv", ["index", "rom_start", "rom_end", "codec", "family", "proven_type", "nested",
                       "stored_sha256", "payload_sha256", "verified", "notes"],
        [[r["index"], f"0x{r['rom_start']:X}", f"0x{r['rom_end']:X}", r["codec"], r["family"], r["proven"],
          r["nested"], r["stored_sha256"], r["payload_sha256"], r["verified"], r["notes"]] for r in res])


def roundtrip(rom: bytes) -> list:
    rows = []
    for r in analyse(rom):
        if r["rom_end"] == r["rom_start"]:
            continue
        rows.append({"start": r["rom_start"], "end": r["rom_end"], "category": "resource",
                     "subtype": r["proven"], "owner": "res2", "format": r["family"],
                     "extraction": f"archive2 record {r['index']} ({r['codec']})",
                     "rebuild": "parsed -> build -> " + ("rlz.encode" if r["codec"] == "rlz" else "direct"),
                     "verified": bool(r["verified"]),
                     "evidence": f"payload_match={r['payload_match']} stored_match={r['stored_match']} {r['notes']}"})
    return rows


def main():
    ap = argparse.ArgumentParser(description="archive-2 record formats")
    ap.add_argument("rom", nargs="?", default=os.environ.get("ROM", "baseroms/us/baserom.z64"))
    ap.add_argument("--export", action="store_true", help="write PNGs and inventory TSVs")
    a = ap.parse_args()
    rom = open(a.rom, "rb").read()
    res = analyse(rom, export=a.export)
    from collections import Counter
    fam = Counter(r["family"] for r in res)
    prov = Counter(r["proven"] for r in res)
    ver = [r for r in res if r["verified"]]
    print("records", len(res), "verified", len(ver), "bytes", sum(r["rom_end"] - r["rom_start"] for r in ver),
          "of", sum(r["rom_end"] - r["rom_start"] for r in res))
    print("families", dict(fam))
    print("proven", dict(prov))
    bad = [r["index"] for r in res if not r["verified"]]
    if bad:
        print("unverified", bad)
    return 0 if not bad else 1


if __name__ == "__main__":
    sys.exit(main())
