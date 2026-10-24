#!/usr/bin/env python3
"""Platform-owned and small fixed ROM regions: header, IPL3, display-list block, build stamp, padding.

Each region is parsed into explicit fields and rebuilt from those fields; the rebuilt bytes are
compared with the ROM.  The header CRC words are recomputed with the CIC-6102 checksum algorithm
over ROM 0x1000..0x101000 rather than copied.
"""
import argparse
import hashlib
import os
import struct
import sys
import zlib

KIT = os.environ.get("N64DK_ROOT", "../n64-decomp-kit")
if KIT not in sys.path:
    sys.path.append(KIT)
try:
    from n64dk.assets import gbi  # noqa: E402
except Exception:  # pragma: no cover - library is optional for rebuild, required for decode text
    gbi = None

HEADER = (0x0, 0x40)
IPL3 = (0x40, 0x1000)
DLBLOCK = (0x57E80, 0x57FD0)
STAMP = (0xF4D840, 0xF4D850)
PADDING = (0xF4D850, 0x1000000)

# Known IPL3 identities (CRC-32 / MD5 of ROM 0x40..0x1000) for the CIC-NUS-6102 boot code.
IPL3_6102_CRC32 = 0x90BB6CB5
IPL3_6102_MD5 = "e24dd796b2fa16511521139d28c8356b"
CIC_6102_SEED = 0xF8CA4DDC

M32 = 0xFFFFFFFF


def sha(b: bytes) -> str:
    return hashlib.sha256(b).hexdigest()


# ---------------------------------------------------------------------------
# CIC-6102 checksum
# ---------------------------------------------------------------------------

def _rol(v: int, n: int) -> int:
    n &= 31
    return ((v << n) | (v >> (32 - n))) & M32 if n else v


def cic6102_checksum(rom: bytes) -> tuple[int, int]:
    """Return (CRC1, CRC2) as computed by the CIC-6102 boot check over ROM 0x1000..0x101000."""
    t1 = t2 = t3 = t4 = t5 = t6 = CIC_6102_SEED
    words = struct.unpack_from(">262144I", rom, 0x1000)
    for d in words:
        r = (t6 + d) & M32
        if r < t6:
            t4 = (t4 + 1) & M32
        t6 = r
        t3 ^= d
        r = _rol(d, d & 0x1F)
        t5 = (t5 + r) & M32
        if t2 > d:
            t2 ^= r
        else:
            t2 ^= t6 ^ d
        t1 = (t1 + (t5 ^ d)) & M32
    return (t6 ^ t4 ^ t3) & M32, (t5 ^ t2 ^ t1) & M32


# ---------------------------------------------------------------------------
# Header
# ---------------------------------------------------------------------------

def parse_header(rom: bytes) -> dict:
    h = rom[0:0x40]
    pi, clock, entry, release, crc1, crc2 = struct.unpack_from(">6I", h, 0)
    return {
        "pi_bsd_dom1": pi, "clock_rate": clock, "entry_point": entry, "release": release,
        "release_libultra": f"{(release >> 8) & 0xFF:d}.0{chr(release & 0xFF)}" if 0x41 <= (release & 0xFF) <= 0x5A else None,
        "crc1": crc1, "crc2": crc2,
        "reserved_18": h[0x18:0x20].hex(),
        "name": h[0x20:0x34].rstrip(b" \0").decode("ascii"), "name_raw_len": 0x14,
        "name_pad_byte": h[0x20 + len(h[0x20:0x34].rstrip(b" \0")):0x34][:1].hex() or "",
        "reserved_34": h[0x34:0x3B].hex(),
        "media": chr(h[0x3B]), "cart_id": h[0x3C:0x3E].decode("ascii"), "region": chr(h[0x3E]),
        "version": h[0x3F],
    }


def build_header(f: dict, rom_for_crc: bytes | None = None) -> bytes:
    crc1, crc2 = (f["crc1"], f["crc2"]) if rom_for_crc is None else cic6102_checksum(rom_for_crc)
    name = f["name"].encode("ascii")
    pad = bytes.fromhex(f["name_pad_byte"] or "20")
    name = name + pad * (0x14 - len(name))
    out = struct.pack(">6I", f["pi_bsd_dom1"], f["clock_rate"], f["entry_point"], f["release"], crc1, crc2)
    out += bytes.fromhex(f["reserved_18"]) + name + bytes.fromhex(f["reserved_34"])
    out += f["media"].encode() + f["cart_id"].encode() + f["region"].encode() + bytes([f["version"]])
    return out


# ---------------------------------------------------------------------------
# IPL3
# ---------------------------------------------------------------------------

def identify_ipl3(rom: bytes) -> dict:
    b = rom[IPL3[0]:IPL3[1]]
    crc = zlib.crc32(b) & M32
    md5 = hashlib.md5(b).hexdigest()
    return {"crc32": crc, "md5": md5, "sha256": sha(b),
            "cic": "6102" if (crc == IPL3_6102_CRC32 and md5 == IPL3_6102_MD5) else None}


# ---------------------------------------------------------------------------
# Display-list block (DMA'd by func at 0x80013B40 from ROM 0x57E80..0x57FD0 to RAM 0x800C7E30)
# ---------------------------------------------------------------------------
# Layout (offsets inside the block, segment 0x01 base == block start):
#   0x000 Vp viewport (vscale[4], vtrans[4] s16), referenced by G_MOVEMEM 0x80 at 0x01000000
#   0x010 display list "rdp_init"  (31 commands, ends G_ENDDL)
#   0x100 display list "othermode" (5 commands)
#   0x128 display list "rsp_init"  (5 commands)

DL_STARTS = (0x10, 0x100, 0x128)
SEGMENT = 0x01

# Encoders: name -> function(fields) -> (w0, w1).  Field names follow decode() in gbi.py; opcodes
# whose decoded fields do not carry every bit (e.g. G_SETPRIMDEPTH) keep their own field model.

def _decode_dl(block: bytes, start: int) -> list[dict]:
    cmds = []
    pc = start
    while True:
        w0, w1 = struct.unpack_from(">II", block, pc)
        op = w0 >> 24
        c = {"offset": pc, "op": op}
        name = None
        if gbi is not None:
            g = gbi.decode(w0, w1, "f3dex", pc)
            name = g.name
            c["gbi"] = str(g)
            if g.error:
                raise ValueError(f"gbi decode error at {pc:#x}: {g.error}")
        # explicit field model (lossless)
        if op in (0xFB, 0xFA, 0xF9, 0xF8, 0xF7):           # colour setters
            c.update(kind="color", lo=w0 & 0xFFFFFF, color=w1)
        elif op in (0xEE, 0xEC, 0xEB, 0xEA, 0xE7, 0xB8, 0xBB, 0xB6, 0xB7):  # w0 low, w1 word
            c.update(kind="word", lo=w0 & 0xFFFFFF, value=w1)
        elif op in (0xBA, 0xB9):                              # SETOTHERMODE_H/L
            c.update(kind="othermode", shift=(w0 >> 8) & 0xFF, length=w0 & 0xFF, pad=(w0 >> 16) & 0xFF, data=w1)
        elif op == 0xFC:
            c.update(kind="combine", mux0=w0 & 0xFFFFFF, mux1=w1)
        elif op == 0xED:
            c.update(kind="scissor", ulx=(w0 >> 12) & 0xFFF, uly=w0 & 0xFFF, mode=(w1 >> 24) & 0xFF,
                     lrx=(w1 >> 12) & 0xFFF, lry=w1 & 0xFFF)
        elif op == 0xF5:
            c.update(kind="settile", fmt=(w0 >> 21) & 7, siz=(w0 >> 19) & 3, line=(w0 >> 9) & 0x1FF,
                     tmem=w0 & 0x1FF, tile=(w1 >> 24) & 7, palette=(w1 >> 20) & 0xF, cmt=(w1 >> 18) & 3,
                     maskt=(w1 >> 14) & 0xF, shiftt=(w1 >> 10) & 0xF, cms=(w1 >> 8) & 3,
                     masks=(w1 >> 4) & 0xF, shifts=w1 & 0xF, pad0=(w0 >> 18) & 1, pad1=(w1 >> 27) & 0x1F)
        elif op == 0xF2:
            c.update(kind="tilesize", uls=(w0 >> 12) & 0xFFF, ult=w0 & 0xFFF, pad=(w1 >> 27) & 0x1F,
                     tile=(w1 >> 24) & 7, lrs=(w1 >> 12) & 0xFFF, lrt=w1 & 0xFFF)
        elif op == 0x03:
            c.update(kind="movemem", index=(w0 >> 16) & 0xFF, length=w0 & 0xFFFF, addr=w1)
        else:
            raise ValueError(f"unmodelled opcode {op:#x} at {pc:#x}")
        c["name"] = name
        cmds.append(c)
        pc += 8
        if op == 0xB8:
            return cmds


def encode_cmd(c: dict) -> bytes:
    op, k = c["op"], c["kind"]
    if k == "color":
        w0, w1 = (op << 24) | c["lo"], c["color"]
    elif k == "word":
        w0, w1 = (op << 24) | c["lo"], c["value"]
    elif k == "othermode":
        w0, w1 = (op << 24) | (c["pad"] << 16) | (c["shift"] << 8) | c["length"], c["data"]
    elif k == "combine":
        w0, w1 = (op << 24) | c["mux0"], c["mux1"]
    elif k == "scissor":
        w0 = (op << 24) | (c["ulx"] << 12) | c["uly"]
        w1 = (c["mode"] << 24) | (c["lrx"] << 12) | c["lry"]
    elif k == "settile":
        w0 = (op << 24) | (c["fmt"] << 21) | (c["siz"] << 19) | (c["pad0"] << 18) | (c["line"] << 9) | c["tmem"]
        w1 = ((c["pad1"] << 27) | (c["tile"] << 24) | (c["palette"] << 20) | (c["cmt"] << 18) | (c["maskt"] << 14)
              | (c["shiftt"] << 10) | (c["cms"] << 8) | (c["masks"] << 4) | c["shifts"])
    elif k == "tilesize":
        w0 = (op << 24) | (c["uls"] << 12) | c["ult"]
        w1 = (c["pad"] << 27) | (c["tile"] << 24) | (c["lrs"] << 12) | c["lrt"]
    elif k == "movemem":
        w0, w1 = (op << 24) | (c["index"] << 16) | c["length"], c["addr"]
    else:
        raise ValueError(k)
    return struct.pack(">II", w0, w1)


def parse_dlblock(rom: bytes) -> dict:
    blk = rom[DLBLOCK[0]:DLBLOCK[1]]
    vp = struct.unpack_from(">8h", blk, 0)
    lists = [{"start": s, "commands": _decode_dl(blk, s)} for s in DL_STARTS]
    return {"size": len(blk), "viewport": {"vscale": list(vp[:4]), "vtrans": list(vp[4:])}, "lists": lists}


def build_dlblock(p: dict) -> bytes:
    out = bytearray(p["size"])
    struct.pack_into(">8h", out, 0, *p["viewport"]["vscale"], *p["viewport"]["vtrans"])
    covered = 0x10
    for dl in p["lists"]:
        pc = dl["start"]
        for c in dl["commands"]:
            out[pc:pc + 8] = encode_cmd(c)
            pc += 8
        covered += pc - dl["start"]
    if covered != p["size"]:
        raise ValueError(f"block coverage {covered:#x} != {p['size']:#x}")
    return bytes(out)


# Loader record: the code at 0x80013B40..0x80013B6C builds the DMA request inline:
#   a0 = 0xB0057E80 (PI address of ROM 0x57E80), a1 = 0x800C7E30 (RAM dest, also stored to
#   0x80052EFC), a2 = 0xB0057FD0 - a0 (= 0x150), then jal func_80028B58 at 0x80013B68.
LOADER_CODE = {"0x80013B44": (0x3C04B005, 0x24847E80), "0x80013B50": (0x3C05800C, 0x24A57E30),
               "0x80013B58": (0x3C06B005, 0x24C67FD0)}


def loader_record(rom: bytes) -> dict:
    def w(va):
        return struct.unpack_from(">I", rom, va - 0x7FFFF400)[0]
    pairs = [(0x80013B44, 0x80013B48), (0x80013B50, 0x80013B54), (0x80013B58, 0x80013B5C)]
    vals = []
    for hi, lo in pairs:
        h, l_ = w(hi), w(lo)
        vals.append(((h & 0xFFFF) << 16) + ((l_ & 0xFFFF) ^ 0x8000) - 0x8000 & M32)
    jal = w(0x80013B68)
    return {"pi_src": vals[0], "ram_dst": vals[1], "pi_end": vals[2], "size": vals[2] - vals[0],
            "jal_target": 0x80000000 | ((jal & 0x03FFFFFF) << 2),
            "rom_start": vals[0] - 0xB0000000, "rom_end": vals[2] - 0xB0000000}


# ---------------------------------------------------------------------------
# Build stamp and padding
# ---------------------------------------------------------------------------

def parse_stamp(rom: bytes) -> dict:
    b = rom[STAMP[0]:STAMP[1]]
    nul = b.index(0)
    return {"text": b[:nul].decode("ascii"), "terminator": b[nul], "trailer": b[nul + 1:].hex()}


def build_stamp(p: dict) -> bytes:
    return p["text"].encode("ascii") + bytes([p["terminator"]]) + bytes.fromhex(p["trailer"])


def parse_padding(rom: bytes, start: int, end: int) -> dict:
    b = rom[start:end]
    vals = set(b)
    return {"fill": b[0] if len(vals) == 1 else None, "distinct": sorted(vals)[:8], "size": len(b)}


# ---------------------------------------------------------------------------

def _row(start, end, cat, sub, fmt, extraction, rebuild, ok, evidence):
    return {"start": start, "end": end, "category": cat, "subtype": sub, "owner": "tools/platform.py",
            "format": fmt, "extraction": extraction, "rebuild": rebuild, "verified": bool(ok),
            "evidence": evidence}


def roundtrip(rom: bytes) -> list[dict]:
    rows = []
    h = parse_header(rom)
    crc = cic6102_checksum(rom)
    hb = build_header(h, rom)
    ok = hb == rom[0:0x40] and crc == (h["crc1"], h["crc2"])
    rows.append(_row(0, 0x40, "header", "n64_cart_header", "formats/platform.md#header", "parse_header",
                     "build_header + cic6102_checksum", ok,
                     f"name={h['name']!r} id=N{h['cart_id']}{h['region']} v{h['version']} "
                     f"crc1={h['crc1']:08X} crc2={h['crc2']:08X} recomputed={crc[0]:08X}/{crc[1]:08X}; rebuilt bytes equal={hb == rom[0:0x40]}"))
    ip = identify_ipl3(rom)
    rows.append(_row(0x40, 0x1000, "boot", "ipl3_cic6102", "platform boot code (preserved bytes)",
                     "byte copy", "byte copy (platform-owned)", ip["cic"] == "6102",
                     f"crc32={ip['crc32']:08X} md5={ip['md5']} identified={ip['cic']}"))
    p = parse_dlblock(rom)
    b = build_dlblock(p)
    lr = loader_record(rom)
    ok = b == rom[DLBLOCK[0]:DLBLOCK[1]] and (lr["rom_start"], lr["rom_end"]) == DLBLOCK
    n = sum(len(d["commands"]) for d in p["lists"])
    rows.append(_row(DLBLOCK[0], DLBLOCK[1], "display_list", "display_lists_and_loader_block",
                     "formats/platform.md#display-list-block", "parse_dlblock (gbi f3dex decode)",
                     "build_dlblock (encode from fields)", ok,
                     f"Vp+3 DLs {n} cmds; DMA {lr['pi_src']:08X}..{lr['pi_end']:08X} -> {lr['ram_dst']:08X} "
                     f"via jal {lr['jal_target']:08X}; rebuilt equal={b == rom[DLBLOCK[0]:DLBLOCK[1]]}"))
    s = parse_stamp(rom)
    sb = build_stamp(s)
    rows.append(_row(STAMP[0], STAMP[1], "metadata", "build_stamp", "ASCII 'DD.MM.YY HH.MM' NUL + trailer",
                     "parse_stamp", "build_stamp", sb == rom[STAMP[0]:STAMP[1]],
                     f"text={s['text']!r} trailer={s['trailer']}"))
    pad = parse_padding(rom, *PADDING)
    regen = bytes([pad["fill"] if pad["fill"] is not None else 0]) * pad["size"]
    rows.append(_row(PADDING[0], PADDING[1], "padding", "rom_tail_fill", "constant fill", "parse_padding",
                     "regenerate fill", regen == rom[PADDING[0]:PADDING[1]],
                     f"fill={pad['fill']} size={pad['size']:#x} distinct={pad['distinct']}"))
    return rows


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("rom", nargs="?", default=os.environ.get("ROM", "baseroms/us/baserom.z64"))
    a = ap.parse_args()
    rom = open(a.rom, "rb").read()
    rows = roundtrip(rom)
    for r in rows:
        print(f"{r['start']:08X} {r['end']:08X} {r['subtype']:32} {'EXACT' if r['verified'] else 'FAIL'}  {r['evidence']}")
    good = sum(r["end"] - r["start"] for r in rows if r["verified"])
    print(f"verified bytes {good:#x} of {sum(r['end'] - r['start'] for r in rows):#x}")
    return 0 if all(r["verified"] for r in rows) else 1


if __name__ == "__main__":
    sys.exit(main())
