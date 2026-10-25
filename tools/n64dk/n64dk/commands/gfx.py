"""``gfx``: find and walk GBI display lists in a ROM range, export textures as PNG."""
from __future__ import annotations

import os
from collections import Counter

from ..assets import gbi, texture
from ..cli import command
from ..rom import Rom


def _int(s: str) -> int:
    return int(s, 16)


def _range(s: str, size: int) -> tuple[int, int]:
    a, _, b = s.partition(":")
    return (_int(a) if a else 0, _int(b) if b else size)


def _segments(items) -> dict[int, int]:
    out = {}
    for item in items or ():
        k, _, v = item.partition("=")
        out[int(k, 0)] = _int(v)
    return out


def _list_row(r: gbi.WalkResult, base: int) -> dict:
    lo, hi = r.extent()
    return {"rom": f"0x{base + r.start:X}", "end": f"0x{base + hi:X}", "dl_end": f"0x{base + r.end:X}",
            "commands": len(r.commands), "vertex_loads": len(r.vertices),
            "vertices": sum(n for *_x, n in gbi._merge_vtx(r.vertices)) if any(o is not None for _a, o, _n in r.vertices)
            else sum(n for *_x, n in r.vertices),
            "triangles": r.triangles, "matrices": len(r.matrices), "sub_dls": len(r.sub_dls),
            "textures": [dict(t.as_dict(), rom=None if t.offset is None else f"0x{base + t.offset:X}")
                         for t in r.textures],
            "unresolved": len(r.unresolved)}


def _export_png(data: bytes, t: gbi.TextureRef) -> bytes | None:
    if t.offset is None or not t.width or not t.height or (t.fmt, t.siz) not in texture.SUPPORTED:
        return None
    pal = None
    if t.fmt == 2 and t.palette_offset is not None:
        pal = data[t.palette_offset:t.palette_offset + t.palette_count * 2]
    try:
        return texture.to_png(data[t.offset:t.offset + t.nbytes], t.fmt, t.siz, t.width, t.height,
                              pal, t.tlut)
    except texture.TextureError:
        return None


@command("gfx", "Find/walk GBI display lists in a ROM range; vertices, textures, PNG export")
def gfx(args):
    rom = Rom.load(args.rom)
    start, end = _range(args.range or ":", len(rom.data))
    data = rom.data[start:end]
    segs = _segments(args.segment)
    family = args.ucode
    if family == "auto":
        family = gbi.detect_family(data, [_int(w) - start for w in args.walk] if args.walk else None)
        if family is None:
            return {"range": f"0x{start:X}:0x{end:X}", "error": "no display lists decode in any ucode family"}

    mode = args.mode
    if mode == "auto":
        if args.walk:
            mode = "walk"
        else:
            probe = gbi.walk(gbi.Memory(data), 0, family)
            mode = "blobs" if probe.ok and len(probe.commands) > 1 else "scan"

    lists: list[tuple[int, gbi.WalkResult, bytes]] = []  # (buffer base ROM offset, result, buffer)
    gaps = []
    if mode == "blobs":
        blobs, gaps = gbi.partition_blobs(data, family)
        lists = [(start, b.walk, data) for b in blobs]
    elif mode == "walk":
        mem = gbi.Memory(rom.data, segs or {0: 0}, 0, False) if segs else gbi.Memory.unmapped(rom.data)
        for w in args.walk:
            lists.append((0, gbi.walk(mem, _int(w), family), rom.data))
    else:
        mem = gbi.Memory(rom.data, segs, 0, False) if segs else gbi.Memory.unmapped(rom.data)
        lists = [(0, r, rom.data) for r in gbi.find_display_lists(rom.data, family, memory=mem,
                                                                  start=start, end=end)]
    streams = []
    if args.decompress:
        from ..assets import compress
        for s in compress.scan(data, lzari_brute=False):
            buf = s.data or compress.decode(s.codec, data, s.rom).data
            found = gbi.find_display_lists(buf, family)
            streams.append({"rom": f"0x{start + s.rom:X}", "codec": s.codec, "size": len(buf),
                            "display_lists": len(found)})
            for r in found:
                lists.append((None, r, buf, start + s.rom))

    rows = []
    png_count = 0
    if args.out:
        os.makedirs(args.out, exist_ok=True)
    for item in lists:
        base, r, buf = item[0], item[1], item[2]
        row = _list_row(r, base or 0)
        if base is None:
            row["stream"] = f"0x{item[3]:X}"
            row["rom"] = f"stream+0x{r.start:X}"
        rows.append(row)
        if args.out and args.png:
            for t in r.textures:
                png = _export_png(buf, t)
                if png is None:
                    continue
                tag = f"{item[3]:X}_{t.offset:X}" if base is None else f"{base + t.offset:X}"
                with open(os.path.join(args.out, f"tex_{tag}_{t.format}_{t.width}x{t.height}.png"), "wb") as fh:
                    fh.write(png)
                png_count += 1
    formats = Counter(t["format"] + (f" {t['width']}x{t['height']}" if t["width"] else "")
                      for row in rows for t in row["textures"])
    result = {
        "range": f"0x{start:X}:0x{end:X}", "ucode": family, "mode": mode,
        "display_lists": len(rows),
        "commands": sum(r["commands"] for r in rows),
        "vertex_loads": sum(r["vertex_loads"] for r in rows),
        "vertices": sum(r["vertices"] for r in rows),
        "triangles": sum(r["triangles"] for r in rows),
        "textures": sum(len(r["textures"]) for r in rows),
        "texture_formats": dict(formats.most_common()),
        "gaps": [f"0x{start + a:X}:0x{start + b:X}" for a, b in gaps],
        "compressed_streams": len(streams) if args.decompress else None,
        "pngs": png_count if args.png else None,
    }
    if args.out:
        with open(os.path.join(args.out, "display_lists.tsv"), "w") as fh:
            fh.write("rom\tend\tdl_end\tcommands\tvertex_loads\tvertices\ttriangles\ttextures\n")
            for r in rows:
                fh.write(f"{r['rom']}\t{r['end']}\t{r['dl_end']}\t{r['commands']}\t{r['vertex_loads']}\t"
                         f"{r['vertices']}\t{r['triangles']}\t"
                         + (",".join(f"{t['format']}:{t['width']}x{t['height']}@{t['rom'] or t['addr']}"
                                     for t in r["textures"]) or "-") + "\n")
    if args.format == "json":
        result["lists"] = rows
        result["streams"] = streams
    return result


def _args(p):
    p.add_argument("rom", help="ROM image")
    p.add_argument("--range", help="ROM range START:END (hex); default the whole ROM")
    p.add_argument("--ucode", default="auto", choices=("auto",) + gbi.FAMILIES)
    p.add_argument("--mode", default="auto", choices=("auto", "blobs", "scan", "walk"),
                   help="blobs: consecutive self-relative blobs (list at offset 0, segment-0 "
                        "addresses relative to the blob); scan: find lists anywhere; walk: --walk offsets")
    p.add_argument("--walk", action="append", help="ROM offset of a list to walk (repeatable)")
    p.add_argument("--segment", action="append", help="N=ROMOFFSET segment base for scan/walk (repeatable)")
    p.add_argument("--decompress", action="store_true", help="also scan decoded compressed streams in the range")
    p.add_argument("--out", help="output directory (display_lists.tsv, PNGs)")
    p.add_argument("--png", action="store_true", help="export textures with known format/size as PNG")


gfx.arguments = _args
