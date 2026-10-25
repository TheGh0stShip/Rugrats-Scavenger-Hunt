"""GBI display lists: command tables, decoding, walking, ucode detection and scanning.

Families (``FAMILIES``):

* ``f3d``    Fast3D (gbi.h without F3DEX_GBI): G_VTX ``(n-1)<<4|v0``, triangle indices x10.
* ``f3dex``  F3DEX/F3DLX/F3DLP 1.x (F3DEX_GBI): G_VTX ``v0*2 | n<<10 | len-1``, G_TRI2,
  G_BRANCH_Z, G_ENDDL 0xB8.
* ``f3dex2`` F3DEX2/F3DZEX (F3DEX_GBI_2): G_VTX 0x01, G_DL 0xDE, G_ENDDL 0xDF.
* ``s2dex`` / ``s2dex2`` sprite microcodes on the 1.x / 2.x numbering.

Addresses in commands are *segmented*: bits 24..27 select one of 16 segment bases and
bits 0..23 are the offset; KSEG0 addresses (0x80xxxxxx) resolve through segment 0.
The walker resolves them against a ``Memory`` (bytes + segment table + physical origin).
"""
from __future__ import annotations

import struct
from dataclasses import dataclass, field

from .texture import SIZ_BITS, SUPPORTED, format_name

FAMILIES = ("f3d", "f3dex", "f3dex2", "s2dex", "s2dex2")

# ---------------------------------------------------------------------------
# Opcode tables
# ---------------------------------------------------------------------------

RDP = {
    0xC0: "G_NOOP", 0xC8: "G_TRI_FILL", 0xC9: "G_TRI_FILL_ZBUFF", 0xCA: "G_TRI_TXTR",
    0xCB: "G_TRI_TXTR_ZBUFF", 0xCC: "G_TRI_SHADE", 0xCD: "G_TRI_SHADE_ZBUFF",
    0xCE: "G_TRI_SHADE_TXTR", 0xCF: "G_TRI_SHADE_TXTR_ZBUFF",
    0xE4: "G_TEXRECT", 0xE5: "G_TEXRECTFLIP", 0xE6: "G_RDPLOADSYNC", 0xE7: "G_RDPPIPESYNC",
    0xE8: "G_RDPTILESYNC", 0xE9: "G_RDPFULLSYNC", 0xEA: "G_SETKEYGB", 0xEB: "G_SETKEYR",
    0xEC: "G_SETCONVERT", 0xED: "G_SETSCISSOR", 0xEE: "G_SETPRIMDEPTH",
    0xEF: "G_RDPSETOTHERMODE", 0xF0: "G_LOADTLUT", 0xF2: "G_SETTILESIZE", 0xF3: "G_LOADBLOCK",
    0xF4: "G_LOADTILE", 0xF5: "G_SETTILE", 0xF6: "G_FILLRECT", 0xF7: "G_SETFILLCOLOR",
    0xF8: "G_SETFOGCOLOR", 0xF9: "G_SETBLENDCOLOR", 0xFA: "G_SETPRIMCOLOR",
    0xFB: "G_SETENVCOLOR", 0xFC: "G_SETCOMBINE", 0xFD: "G_SETTIMG", 0xFE: "G_SETZIMG",
    0xFF: "G_SETCIMG",
}

_IMM1 = {  # immediates shared by Fast3D and F3DEX 1.x (G_IMMFIRST = -65)
    0xBF: "G_TRI1", 0xBE: "G_CULLDL", 0xBD: "G_POPMTX", 0xBC: "G_MOVEWORD", 0xBB: "G_TEXTURE",
    0xBA: "G_SETOTHERMODE_H", 0xB9: "G_SETOTHERMODE_L", 0xB8: "G_ENDDL",
    0xB7: "G_SETGEOMETRYMODE", 0xB6: "G_CLEARGEOMETRYMODE", 0xB5: "G_LINE3D",
    0xB4: "G_RDPHALF_1", 0xB3: "G_RDPHALF_2",
}
_DMA1 = {0x00: "G_SPNOOP", 0x01: "G_MTX", 0x03: "G_MOVEMEM", 0x04: "G_VTX", 0x06: "G_DL"}

_F3DEX2 = {
    0x00: "G_NOOP", 0x01: "G_VTX", 0x02: "G_MODIFYVTX", 0x03: "G_CULLDL", 0x04: "G_BRANCH_Z",
    0x05: "G_TRI1", 0x06: "G_TRI2", 0x07: "G_QUAD", 0x08: "G_LINE3D",
    0xD3: "G_SPECIAL_3", 0xD4: "G_SPECIAL_2", 0xD5: "G_SPECIAL_1", 0xD6: "G_DMA_IO",
    0xD7: "G_TEXTURE", 0xD8: "G_POPMTX", 0xD9: "G_GEOMETRYMODE", 0xDA: "G_MTX",
    0xDB: "G_MOVEWORD", 0xDC: "G_MOVEMEM", 0xDD: "G_LOAD_UCODE", 0xDE: "G_DL", 0xDF: "G_ENDDL",
    0xE0: "G_SPNOOP", 0xE1: "G_RDPHALF_1", 0xE2: "G_SETOTHERMODE_L", 0xE3: "G_SETOTHERMODE_H",
    0xF1: "G_RDPHALF_2",
}

OPCODES: dict[str, dict[int, str]] = {
    "f3d": {**RDP, **_DMA1, 0x09: "G_SPRITE2D_BASE", **_IMM1, 0xB2: "G_RDPHALF_CONT"},
    "f3dex": {**RDP, **_DMA1, **_IMM1, 0xB2: "G_MODIFYVTX", 0xB1: "G_TRI2", 0xB0: "G_BRANCH_Z",
              0xAF: "G_LOAD_UCODE"},
    "f3dex2": {**RDP, **_F3DEX2},
    "s2dex": {**RDP, 0x00: "G_SPNOOP", 0x01: "G_BG_1CYC", 0x02: "G_BG_COPY",
              0x03: "G_OBJ_RECTANGLE", 0x04: "G_OBJ_SPRITE", 0x05: "G_OBJ_MOVEMEM", 0x06: "G_DL",
              0xB0: "G_SELECT_DL", 0xB1: "G_OBJ_RENDERMODE", 0xB2: "G_OBJ_RECTANGLE_R",
              0xC1: "G_OBJ_LOADTXTR", 0xC2: "G_OBJ_LDTX_SPRITE", 0xC3: "G_OBJ_LDTX_RECT",
              0xC4: "G_OBJ_LDTX_RECT_R", 0xE4: "G_RDPHALF_0", 0xAF: "G_LOAD_UCODE",
              **{k: v for k, v in _IMM1.items() if v not in ("G_TRI1", "G_LINE3D", "G_CULLDL")}},
    "s2dex2": {**RDP, **{k: v for k, v in _F3DEX2.items() if k >= 0xD3 or k == 0},
               0x01: "G_OBJ_RECTANGLE", 0x02: "G_OBJ_SPRITE", 0x04: "G_SELECT_DL",
               0x05: "G_OBJ_LOADTXTR", 0x06: "G_OBJ_LDTX_SPRITE", 0x07: "G_OBJ_LDTX_RECT",
               0x08: "G_OBJ_LDTX_RECT_R", 0x09: "G_BG_1CYC", 0x0A: "G_BG_COPY",
               0x0B: "G_OBJ_RENDERMODE", 0xDA: "G_OBJ_RECTANGLE_R", 0xDC: "G_OBJ_MOVEMEM",
               0xE4: "G_RDPHALF_0"},
}
# Vertex cache size (indices beyond this are invalid).
VTX_CACHE = {"f3d": 16, "f3dex": 64, "f3dex2": 64}
# Size in bytes of the structure an S2DEX object command points at.
_OBJ_SIZES = {"G_BG_1CYC": 40, "G_BG_COPY": 40, "G_OBJ_RECTANGLE": 24, "G_OBJ_SPRITE": 24,
              "G_OBJ_RECTANGLE_R": 24, "G_OBJ_LOADTXTR": 24, "G_OBJ_LDTX_SPRITE": 48,
              "G_OBJ_LDTX_RECT": 48, "G_OBJ_LDTX_RECT_R": 48}

ENDDL_WORDS = {"f3d": 0xB8000000, "f3dex": 0xB8000000, "s2dex": 0xB8000000,
               "f3dex2": 0xDF000000, "s2dex2": 0xDF000000}


def opcode_name(op: int, family: str) -> str | None:
    return OPCODES[family].get(op)


# ---------------------------------------------------------------------------
# Single-command decoding
# ---------------------------------------------------------------------------

@dataclass
class Command:
    offset: int          # byte offset of the command in its buffer (-1 if not applicable)
    w0: int
    w1: int
    op: int
    name: str
    fields: dict = field(default_factory=dict)
    ref: tuple | None = None   # (kind, segmented address, byte length or None)
    error: str | None = None   # structural problem; None when plausible

    def __str__(self):
        args = ", ".join(f"{k}={('0x%X' % v) if isinstance(v, int) and k in _HEX else v}"
                         for k, v in self.fields.items())
        return f"{self.w0:08X} {self.w1:08X}  {self.name}({args})"


_HEX = {"addr", "value", "set", "clear", "data", "mux0", "mux1", "color", "colour"}


def _b(v, s, n):
    return (v >> s) & ((1 << n) - 1)


def decode(w0: int, w1: int, family: str, offset: int = -1) -> Command:
    """Decode one 64-bit GBI command."""
    op = w0 >> 24
    name = OPCODES[family].get(op)
    if name is None:
        return Command(offset, w0, w1, op, f"0x{op:02X}", error=f"unknown opcode 0x{op:02X}")
    c = Command(offset, w0, w1, op, name)
    f = c.fields
    ex2 = family in ("f3dex2", "s2dex2")

    def addr_ref(kind, length=None, *, align=8):
        c.ref = (kind, w1, length)
        f["addr"] = w1
        if w1 % align:
            c.error = f"{name} address 0x{w1:08X} not {align}-byte aligned"
        elif not valid_segmented(w1):
            c.error = f"{name} address 0x{w1:08X} is not a segmented or KSEG0 address"

    if name in ("G_SPNOOP", "G_NOOP", "G_RDPLOADSYNC", "G_RDPPIPESYNC", "G_RDPTILESYNC",
                "G_RDPFULLSYNC"):
        if w0 & 0x00FFFFFF or (w1 and name != "G_NOOP"):
            c.error = f"{name} with non-zero operands"
    elif name == "G_ENDDL":
        if w0 & 0x00FFFFFF or w1:
            c.error = "G_ENDDL with non-zero operands"
    elif name == "G_VTX":
        if family == "f3d":
            n, v0, length = _b(w0, 20, 4) + 1, _b(w0, 16, 4), _b(w0, 0, 16)
            if length != n * 16:
                c.error = "G_VTX length does not match the vertex count"
        elif family == "f3dex":
            n, length, v0x2 = _b(w0, 10, 6), _b(w0, 0, 10) + 1, _b(w0, 16, 8)
            v0 = v0x2 // 2
            if length != n * 16 or v0x2 & 1 or n == 0:
                c.error = "G_VTX length/count/v0 inconsistent"
        else:
            n, end2 = _b(w0, 12, 8), _b(w0, 0, 8)
            v0, length = (end2 >> 1) - n, n * 16
            if end2 & 1 or v0 < 0 or n == 0 or _b(w0, 20, 4):
                c.error = "G_VTX count/v0 inconsistent"
        f.update(n=n, v0=v0)
        if not c.error and v0 + n > VTX_CACHE[family]:
            c.error = f"G_VTX loads past the {VTX_CACHE[family]}-entry vertex cache"
        err = c.error
        addr_ref("vtx", length)
        c.error = err or c.error
    elif name == "G_DL":
        push = _b(w0, 16, 8)
        f["branch"] = push == 1
        err = None if push in (0, 1) and not w0 & 0xFFFF else "G_DL with bad push flag"
        addr_ref("dl")
        c.error = err or c.error
    elif name in ("G_BRANCH_Z", "G_SELECT_DL"):
        pass  # target comes from the preceding G_RDPHALF_1 / G_RDPHALF_0
    elif name == "G_MTX":
        if ex2:
            f.update(params=_b(w0, 0, 8) ^ 1, length=(_b(w0, 19, 5) + 1) * 8)
        else:
            f.update(params=_b(w0, 16, 8), length=_b(w0, 0, 16))
        err = None if f["length"] == 64 and f["params"] < 8 else "G_MTX with bad length/params"
        addr_ref("mtx", 64)
        c.error = err or c.error
    elif name == "G_MOVEMEM":
        if ex2:
            f.update(index=_b(w0, 0, 8), offset=_b(w0, 8, 8) * 8, length=(_b(w0, 19, 5) + 1) * 8)
        else:
            f.update(index=_b(w0, 16, 8), length=_b(w0, 0, 16))
        err = None if 0 < f["length"] <= 256 else "G_MOVEMEM with bad length"
        addr_ref("movemem", f["length"])
        c.error = err or c.error
    elif name == "G_OBJ_MOVEMEM":
        addr_ref("obj", 24)
    elif name == "G_MOVEWORD":
        if ex2:
            f.update(index=_b(w0, 16, 8), offset=_b(w0, 0, 16))
        else:
            f.update(index=_b(w0, 0, 8), offset=_b(w0, 8, 16))
        f["value"] = w1
        if f["index"] > 0x0E or f["index"] & 1:
            c.error = "G_MOVEWORD with bad index"
    elif name in ("G_TRI1", "G_TRI2", "G_QUAD", "G_LINE3D"):
        mul = 10 if family == "f3d" else 2
        if ex2:
            idx = [_b(w0, 16, 8), _b(w0, 8, 8), _b(w0, 0, 8)]
            if name != "G_TRI1":
                idx += [_b(w1, 16, 8), _b(w1, 8, 8), _b(w1, 0, 8)]
            elif w1:
                c.error = "G_TRI1 with non-zero second word"
        elif name == "G_TRI2":
            idx = [_b(w0, 16, 8), _b(w0, 8, 8), _b(w0, 0, 8),
                   _b(w1, 16, 8), _b(w1, 8, 8), _b(w1, 0, 8)]
        else:
            idx = [_b(w1, 16, 8), _b(w1, 8, 8)] + ([_b(w1, 0, 8)] if name != "G_LINE3D" else [])
            if w0 & 0x00FFFFFF:
                c.error = f"{name} with non-zero first word operands"
            if _b(w1, 24, 8) > 2:
                c.error = f"{name} with bad flag"
        if name == "G_LINE3D" and ex2:
            idx = idx[:2]
        if any(i % mul for i in idx):
            c.error = f"{name} vertex index not a multiple of {mul}"
        f["v"] = [i // mul for i in idx]
    elif name == "G_CULLDL":
        f.update(first=_b(w0, 0, 16), last=_b(w1, 0, 16))
    elif name in ("G_SETGEOMETRYMODE", "G_CLEARGEOMETRYMODE"):
        f["mode"] = w1
        if w0 & 0x00FFFFFF:
            c.error = f"{name} with non-zero first word operands"
    elif name == "G_GEOMETRYMODE":
        f.update(clear=(~w0) & 0xFFFFFF, set=w1)
    elif name == "G_TEXTURE":
        f.update(level=_b(w0, 11, 3), tile=_b(w0, 8, 3), on=_b(w0, 1 if ex2 else 0, 7),
                 s=_b(w1, 16, 16), t=_b(w1, 0, 16))
    elif name in ("G_SETOTHERMODE_H", "G_SETOTHERMODE_L"):
        if ex2:
            length = _b(w0, 0, 8) + 1
            shift = 32 - _b(w0, 8, 8) - length
        else:
            shift, length = _b(w0, 8, 8), _b(w0, 0, 8)
        f.update(shift=shift, length=length, data=w1)
        if shift < 0 or length == 0 or shift + length > 32 or w1 & ~(((1 << length) - 1) << shift) & 0xFFFFFFFF:
            c.error = f"{name} with inconsistent shift/length/data"
    elif name in ("G_RDPHALF_1", "G_RDPHALF_2", "G_RDPHALF_0", "G_RDPHALF_CONT"):
        f["value"] = w1
    elif name in ("G_SETTIMG", "G_SETCIMG", "G_SETZIMG"):
        fmt, siz, width = _b(w0, 21, 3), _b(w0, 19, 2), _b(w0, 0, 12) + 1
        f.update(fmt=fmt, siz=siz, width=width)
        if fmt > 4 or _b(w0, 12, 7):
            c.error = f"{name} with bad format/padding bits"
        err = c.error
        addr_ref({"G_SETTIMG": "timg", "G_SETCIMG": "cimg", "G_SETZIMG": "zimg"}[name])
        c.error = err or c.error
    elif name == "G_SETTILE":
        f.update(fmt=_b(w0, 21, 3), siz=_b(w0, 19, 2), line=_b(w0, 9, 9), tmem=_b(w0, 0, 9),
                 tile=_b(w1, 24, 3), palette=_b(w1, 20, 4), cmt=_b(w1, 18, 2), maskt=_b(w1, 14, 4),
                 shiftt=_b(w1, 10, 4), cms=_b(w1, 8, 2), masks=_b(w1, 4, 4), shifts=_b(w1, 0, 4))
        if f["fmt"] > 4 or _b(w0, 18, 1) or _b(w1, 27, 5):
            c.error = "G_SETTILE with bad format/padding bits"
    elif name in ("G_SETTILESIZE", "G_LOADTILE", "G_LOADBLOCK", "G_LOADTLUT"):
        f.update(uls=_b(w0, 12, 12), ult=_b(w0, 0, 12), tile=_b(w1, 24, 3),
                 lrs=_b(w1, 12, 12), lrt=_b(w1, 0, 12))
        if name == "G_LOADBLOCK":
            f["texels"] = f["lrs"] + 1
            f["dxt"] = f.pop("lrt")
        elif name == "G_LOADTLUT":
            f["count"] = (f["lrs"] >> 2) + 1
        if _b(w1, 27, 5):
            c.error = f"{name} with bad padding bits"
    elif name in ("G_SETFILLCOLOR", "G_SETFOGCOLOR", "G_SETBLENDCOLOR", "G_SETENVCOLOR"):
        f["color"] = w1
    elif name == "G_SETPRIMCOLOR":
        f.update(minlevel=_b(w0, 8, 8), lodfrac=_b(w0, 0, 8), color=w1)
    elif name == "G_SETCOMBINE":
        f.update(mux0=w0 & 0xFFFFFF, mux1=w1)
    elif name in ("G_FILLRECT", "G_SETSCISSOR"):
        f.update(ulx=_b(w1, 12, 12), uly=_b(w1, 0, 12), lrx=_b(w0, 12, 12), lry=_b(w0, 0, 12))
        if name == "G_SETSCISSOR":
            f = c.fields
            f.update(ulx=_b(w0, 12, 12), uly=_b(w0, 0, 12), lrx=_b(w1, 12, 12), lry=_b(w1, 0, 12))
    elif name in ("G_TEXRECT", "G_TEXRECTFLIP"):
        f.update(lrx=_b(w0, 12, 12), lry=_b(w0, 0, 12), tile=_b(w1, 24, 3),
                 ulx=_b(w1, 12, 12), uly=_b(w1, 0, 12))
    elif name == "G_LOAD_UCODE":
        addr_ref("ucode", align=8)
    elif name in _OBJ_SIZES:
        addr_ref("obj", _OBJ_SIZES[name])
    elif name == "G_OBJ_RENDERMODE":
        f["mode"] = w1
    elif name == "G_RDPSETOTHERMODE":
        f.update(hi=w0 & 0xFFFFFF, lo=w1)
    elif name.startswith("G_TRI_"):
        c.error = "raw RDP triangle commands are not walked"
    return c


def valid_segmented(addr: int) -> bool:
    top = addr >> 24
    return top < 0x10 or 0x80 <= top < 0x88


# ---------------------------------------------------------------------------
# Memory model
# ---------------------------------------------------------------------------

@dataclass
class Memory:
    """A byte buffer seen through the RSP segment table.

    ``origin`` is the physical (RDRAM) address of ``data[0]``; ``segments`` maps segment
    numbers to physical bases. Segment 0 defaults to physical 0, so with ``origin=0`` and
    no table, segment-0 addresses are plain buffer offsets. ``Memory.unmapped(data)``
    resolves nothing (raw ROM scans, where no runtime mapping is known).
    """
    data: bytes
    segments: dict[int, int] = field(default_factory=lambda: {0: 0})
    origin: int = 0
    kseg0: bool = True   # resolve 0x80xxxxxx addresses (only meaningful with a real origin)
    bounded: bool = True  # resolvable addresses must land inside ``data``

    def physical(self, addr: int) -> int | None:
        if addr >> 24 >= 0x80:
            return addr & 0x1FFFFFFF if self.kseg0 else None
        seg = (addr >> 24) & 0x0F
        base = self.segments.get(seg)
        if base is None:
            return None
        return (base + (addr & 0x00FFFFFF)) & 0x1FFFFFFF

    def offset(self, addr: int) -> int | None:
        """Buffer offset of a segmented address, or None when not resolvable/in range."""
        phys = self.physical(addr)
        if phys is None:
            return None
        off = phys - self.origin
        return off if 0 <= off < len(self.data) else None

    @classmethod
    def unmapped(cls, data: bytes) -> "Memory":
        return cls(data, {}, 0, False, False)

    def words(self, off: int) -> tuple[int, int]:
        return struct.unpack_from(">II", self.data, off)


# ---------------------------------------------------------------------------
# Walker
# ---------------------------------------------------------------------------

@dataclass
class TextureRef:
    timg: int                 # segmented address of the image
    offset: int | None        # buffer offset (None if unresolved)
    nbytes: int               # bytes loaded into TMEM
    load: str                 # LOADBLOCK / LOADTILE
    fmt: int | None = None    # render tile format/size (falls back to the load)
    siz: int | None = None
    width: int | None = None
    height: int | None = None
    palette: int | None = None          # segmented address of the TLUT
    palette_offset: int | None = None
    palette_count: int = 0
    tlut: str = "rgba16"
    tile: int = 0

    @property
    def format(self) -> str:
        return format_name(self.fmt, self.siz) if self.fmt is not None else "?"

    def as_dict(self):
        return {"addr": f"0x{self.timg:08X}", "offset": self.offset, "bytes": self.nbytes,
                "load": self.load, "format": self.format, "width": self.width, "height": self.height,
                "palette": None if self.palette is None else f"0x{self.palette:08X}",
                "palette_offset": self.palette_offset, "palette_count": self.palette_count,
                "tlut": self.tlut}


@dataclass
class WalkResult:
    start: int
    family: str
    commands: list[Command] = field(default_factory=list)
    dls: list[tuple[int, int]] = field(default_factory=list)      # buffer [start, end) of each list walked
    vertices: list[tuple[int, int, int]] = field(default_factory=list)  # (addr, offset|None, count)
    textures: list[TextureRef] = field(default_factory=list)
    palettes: list[tuple[int, int | None, int]] = field(default_factory=list)  # (addr, offset, count)
    matrices: list[tuple[int, int | None]] = field(default_factory=list)
    sub_dls: list[tuple[int, int | None]] = field(default_factory=list)
    other_refs: list[tuple[str, int, int | None, int | None]] = field(default_factory=list)
    unresolved: list[tuple[str, int]] = field(default_factory=list)
    errors: list[str] = field(default_factory=list)
    end: int | None = None    # offset just past the root list's terminating command
    triangles: int = 0

    @property
    def ok(self) -> bool:
        return not self.errors and self.end is not None

    def extent(self) -> tuple[int, int]:
        """Lowest and highest buffer offsets touched by the lists and resolved data."""
        lo, hi = self.start, self.end or self.start
        for s, e, _k in self.data_spans():
            lo, hi = min(lo, s), max(hi, e)
        for s, e in self.dls:
            lo, hi = min(lo, s), max(hi, e)
        return lo, hi

    def data_spans(self) -> list[tuple[int, int, str]]:
        spans = []
        for _a, off, n in self.vertices:
            if off is not None:
                spans.append((off, off + n * 16, "vertices"))
        for t in self.textures:
            if t.offset is not None:
                spans.append((t.offset, t.offset + t.nbytes, "texture"))
        for _a, off, n in self.palettes:
            if off is not None:
                spans.append((off, off + n * 2, "palette"))
        for _a, off in self.matrices:
            if off is not None:
                spans.append((off, off + 64, "matrix"))
        for kind, _a, off, n in self.other_refs:
            if off is not None and n:
                spans.append((off, off + n, kind))
        return sorted(spans)

    def summary(self) -> dict:
        return {"start": self.start, "end": self.end, "family": self.family, "ok": self.ok,
                "commands": len(self.commands), "dls": len(self.dls),
                "vertex_loads": len(self.vertices),
                "vertices": sum(n for _a, _o, n in _merge_vtx(self.vertices)),
                "triangles": self.triangles, "textures": [t.as_dict() for t in self.textures],
                "matrices": len(self.matrices), "sub_dls": len(self.sub_dls),
                "unresolved": [(k, f"0x{a:08X}") for k, a in self.unresolved],
                "errors": self.errors}


def _merge_vtx(vtx):
    spans = sorted((o, o + n * 16) for _a, o, n in vtx if o is not None)
    out = []
    for s, e in spans:
        if out and s <= out[-1][1]:
            out[-1] = (out[-1][0], max(out[-1][1], e))
        else:
            out.append((s, e))
    return [(None, s, (e - s) // 16) for s, e in out]


_TERMINATORS = {"G_ENDDL"}
_DRAW = {"G_TRI1", "G_TRI2", "G_QUAD", "G_TEXRECT", "G_TEXRECTFLIP", "G_OBJ_SPRITE",
         "G_OBJ_RECTANGLE", "G_OBJ_RECTANGLE_R", "G_BG_1CYC", "G_BG_COPY"}


class _TexState:
    def __init__(self):
        self.timg = None   # (fmt, siz, width, addr)
        self.tiles: dict[int, dict] = {}
        self.pending: list[dict] = []
        self.tluts: dict[int, tuple[int, int | None, int]] = {}  # tmem -> (addr, off, count)
        self.textlut = 2
        self.emitted: set = set()


def walk(mem: Memory | bytes, start: int, family: str, *, follow: bool = True,
         max_commands: int = 200000, max_depth: int = 18, strict: bool = True) -> WalkResult:
    """Walk the display list at buffer offset ``start``.

    Follows G_DL (call/branch) and G_BRANCH_Z/G_SELECT_DL targets that resolve into the
    buffer when ``follow`` is set, applies G_MOVEWORD segment updates, and records vertex
    loads, texture images with their derived render-tile format and size, palettes,
    matrices and sub-lists. Errors (unknown opcode, misalignment, out-of-range data,
    indices beyond loaded vertices) stop the walk when ``strict``.
    """
    if not isinstance(mem, Memory):
        mem = Memory(mem)
    mem = Memory(mem.data, dict(mem.segments), mem.origin, mem.kseg0, mem.bounded)
    res = WalkResult(start, family)
    tex = _TexState()
    loaded = 0  # bitmask of vertex cache entries ever loaded
    cache = VTX_CACHE.get(family, 64)
    visited: set[int] = set()
    pending_lists = [start]
    half1 = None
    root = True
    data = mem.data
    while pending_lists:
        pc = pending_lists.pop(0)
        if pc in visited:
            continue
        stack: list[int] = []
        list_start = pc
        while True:
            if len(res.commands) >= max_commands:
                res.errors.append(f"command limit reached at 0x{pc:X}")
                return res
            if pc % 8 or pc + 8 > len(data) or pc < 0:
                res.errors.append(f"display list runs outside the buffer at 0x{pc:X}")
                break
            if pc in visited:
                break  # merged into an already-walked list
            visited.add(pc)
            w0, w1 = struct.unpack_from(">II", data, pc)
            c = decode(w0, w1, family, pc)
            res.commands.append(c)
            if c.error:
                res.errors.append(f"0x{pc:X}: {c.error}")
                if strict:
                    break
            n = c.name
            nxt = pc + 8
            if c.ref is not None:
                kind, addr, length = c.ref
                off = mem.offset(addr)
                if off is None and mem.bounded and mem.physical(addr) is not None:
                    res.errors.append(f"0x{pc:X}: {kind} address 0x{addr:08X} outside the buffer")
                    if strict:
                        break
                if kind == "vtx":
                    if off is None:
                        res.unresolved.append((kind, addr))
                    elif off + length > len(data):
                        res.errors.append(f"0x{pc:X}: vertices 0x{addr:08X}+0x{length:X} outside the buffer")
                    res.vertices.append((addr, off, c.fields["n"]))
                    for i in range(c.fields["v0"], c.fields["v0"] + c.fields["n"]):
                        loaded |= 1 << i
                elif kind == "dl":
                    if off is None:
                        res.unresolved.append((kind, addr))
                        res.sub_dls.append((addr, None))
                        if c.fields["branch"]:
                            break
                    else:
                        res.sub_dls.append((addr, off))
                        if follow:
                            if c.fields["branch"]:
                                res.dls.append((list_start, nxt))
                                pc = list_start = off
                                continue
                            if len(stack) >= max_depth:
                                res.errors.append(f"0x{pc:X}: display list stack overflow")
                                break
                            res.dls.append((list_start, nxt))
                            stack.append(nxt)
                            pc = list_start = off
                            continue
                        if c.fields["branch"]:
                            break
                elif kind == "mtx":
                    if off is None:
                        res.unresolved.append((kind, addr))
                    res.matrices.append((addr, off))
                elif kind == "timg":
                    tex.timg = (c.fields["fmt"], c.fields["siz"], c.fields["width"], addr, off)
                    if off is None:
                        res.unresolved.append((kind, addr))
                else:
                    if off is None:
                        res.unresolved.append((kind, addr))
                    res.other_refs.append((kind, addr, off, length))
            if n in ("G_RDPHALF_1", "G_RDPHALF_0"):
                half1 = w1
            elif n in ("G_BRANCH_Z", "G_SELECT_DL") and half1 is not None:
                off = mem.offset(half1)
                res.sub_dls.append((half1, off))
                if off is None:
                    res.unresolved.append(("dl", half1))
                elif follow:
                    pending_lists.append(off)
            elif n == "G_MOVEWORD" and c.fields["index"] == 6:
                phys = mem.physical(w1) if w1 >> 24 < 0x10 or w1 >> 24 >= 0x80 else None
                if phys is not None:
                    mem.segments[c.fields["offset"] // 4] = phys
            elif n in ("G_TRI1", "G_TRI2", "G_QUAD", "G_LINE3D"):
                res.triangles += {"G_TRI1": 1, "G_TRI2": 2, "G_QUAD": 2}.get(n, 0)
                for v in c.fields["v"]:
                    if v >= cache or not loaded >> v & 1:
                        res.errors.append(f"0x{pc:X}: {n} uses vertex {v} that was never loaded")
                        if strict:
                            break
                if strict and res.errors and res.errors[-1].startswith(f"0x{pc:X}"):
                    break
            elif n in ("G_SETOTHERMODE_H",):
                sh, ln = c.fields["shift"], c.fields["length"]
                if sh <= 14 < sh + ln:
                    tex.textlut = (w1 >> 14) & 3
            elif n == "G_RDPSETOTHERMODE":
                tex.textlut = (w0 >> 14) & 3
            elif n == "G_SETTILE":
                tex.tiles[c.fields["tile"]] = dict(c.fields)
            elif n == "G_SETTILESIZE":
                t = tex.tiles.setdefault(c.fields["tile"], {})
                t["w"] = ((c.fields["lrs"] - c.fields["uls"]) >> 2) + 1
                t["h"] = ((c.fields["lrt"] - c.fields["ult"]) >> 2) + 1
            elif n in ("G_LOADBLOCK", "G_LOADTILE", "G_LOADTLUT"):
                err = _load(res, tex, c, mem)
                if err:
                    res.errors.append(f"0x{pc:X}: {err}")
                    if strict:
                        break
            elif n in _DRAW:
                _finalize(res, tex)
            if n in _TERMINATORS:
                res.dls.append((list_start, nxt))
                if stack:
                    pc = list_start = stack.pop()
                    continue
                if root:
                    res.end = nxt
                break
            pc = nxt
        root = False
        if strict and res.errors:
            break
    _finalize(res, tex)
    if strict:
        _check_bounds(res, len(data))
    return res


def _load(res, tex, c, mem):
    f = c.fields
    if tex.timg is None:
        return f"{c.name} without G_SETTIMG"
    fmt, siz, width, addr, off = tex.timg
    tile = tex.tiles.get(f["tile"], {})
    if c.name == "G_LOADTLUT":
        count = f["count"]
        res.palettes.append((addr, off, count))
        tex.tluts[tile.get("tmem", 256)] = (addr, off, count)
        return None
    if c.name == "G_LOADBLOCK":
        nbytes = (f["texels"] * SIZ_BITS[siz] + 7) // 8
    else:
        w = ((f["lrs"] - f["uls"]) >> 2) + 1
        h = ((f["lrt"] - f["ult"]) >> 2) + 1
        row = (width * SIZ_BITS[siz] + 7) // 8
        first = row * (f["ult"] >> 2) + ((f["uls"] >> 2) * SIZ_BITS[siz]) // 8
        nbytes = row * (h - 1) + (w * SIZ_BITS[siz] + 7) // 8
        addr, off = addr + first, None if off is None else off + first
        if w <= 0 or h <= 0:
            return "G_LOADTILE with empty rectangle"
    tex.pending.append({"addr": addr, "off": off, "bytes": nbytes, "load": c.name[2:],
                        "tmem": tile.get("tmem", 0), "fmt": fmt, "siz": siz,
                        "ltile": f["tile"], "width": width})
    return None


def _finalize(res, tex):
    """Attach render-tile format, size and palette to each pending image load."""
    for p in tex.pending:
        key = (p["addr"], p["bytes"])
        rt = None
        for t in sorted(tex.tiles):
            ti = tex.tiles[t]
            if t != p["ltile"] and ti.get("tmem") == p["tmem"] and "w" in ti:
                rt = ti
                break
        if rt is None:
            rt = tex.tiles.get(0) if 0 != p["ltile"] else None
        fmt = siz = w = h = None
        if rt is not None and "fmt" in rt:
            fmt, siz = rt["fmt"], rt["siz"]
            w, h = rt.get("w"), rt.get("h")
            if w and h and (fmt, siz) in SUPPORTED:
                full = (w * SIZ_BITS[siz] + 7) // 8 * h
                # Clamped or mirrored tiles can cover fewer texels than were loaded.
                if full > p["bytes"] and p["load"] == "LOADBLOCK":
                    w = h = None
        if fmt is None:
            fmt, siz = p["fmt"], p["siz"]
        ref = TextureRef(p["addr"], p["off"], p["bytes"], p["load"], fmt, siz, w, h,
                         tile=rt and rt.get("tile", 0) or 0)
        if fmt == 2:
            tm = 256 + (rt.get("palette", 0) * 16 if siz == 0 and rt else 0)
            pal = tex.tluts.get(tm) or (next(reversed(tex.tluts.values())) if tex.tluts else None)
            if pal:
                ref.palette, ref.palette_offset, ref.palette_count = pal
            ref.tlut = "ia16" if tex.textlut == 3 else "rgba16"
        if (key, ref.fmt, ref.siz, ref.width) in tex.emitted:
            continue
        tex.emitted.add((key, ref.fmt, ref.siz, ref.width))
        res.textures.append(ref)
    tex.pending.clear()


def _check_bounds(res, size):
    for s, e, kind in res.data_spans():
        if s < 0 or e > size:
            res.errors.append(f"{kind} at 0x{s:X}..0x{e:X} outside the buffer")
    for t in res.textures:
        if t.offset is not None and t.offset % 8:
            res.errors.append(f"texture at 0x{t.offset:X} not 8-byte aligned")
    for _a, off, _n in res.palettes:
        if off is not None and off % 8:
            res.errors.append(f"palette at 0x{off:X} not 8-byte aligned")


def disassemble(data: bytes, start: int, family: str, count: int | None = None) -> list[Command]:
    """Linear decode until G_ENDDL (or ``count`` commands)."""
    out = []
    pc = start
    while pc + 8 <= len(data):
        w0, w1 = struct.unpack_from(">II", data, pc)
        c = decode(w0, w1, family, pc)
        out.append(c)
        pc += 8
        if c.name == "G_ENDDL" or (count and len(out) >= count):
            break
    return out


# ---------------------------------------------------------------------------
# Ucode family detection
# ---------------------------------------------------------------------------

_EVIDENCE = {"G_VTX", "G_TRI1", "G_TRI2", "G_QUAD", "G_LINE3D", "G_DL", "G_MTX",
             "G_MOVEMEM", "G_MOVEWORD", "G_BRANCH_Z", "G_CULLDL", "G_TEXTURE"}


def _score(r: WalkResult) -> int:
    if not r.ok:
        return -1
    # RDP, sync and othermode commands decode identically everywhere and carry no
    # evidence; validated geometry/control commands do.
    return 1 + sum(1 for c in r.commands if c.name in _EVIDENCE or c.name.startswith(("G_OBJ", "G_BG")))


_PREFERENCE = ("f3dex", "f3dex2", "f3d", "s2dex", "s2dex2")
_SPRITE_BASE = {"s2dex": "f3dex", "s2dex2": "f3dex2"}


def detect_family(data: bytes, starts: list[int] | int | None = None) -> str | None:
    """Pick the ucode family that decodes the given list(s) best.

    ``starts`` defaults to the lists found by scanning ``data`` for each family's G_ENDDL.
    Returns None when nothing decodes.
    """
    if isinstance(starts, int):
        starts = [starts]
    totals = {f: 0 for f in FAMILIES}
    mem = Memory.unmapped(data)
    if starts is None:
        lists = {fam: [r for r in find_display_lists(data, fam)] for fam in FAMILIES}
    else:
        lists = {fam: [walk(mem, s, fam, follow=False) for s in starts] for fam in FAMILIES}
    for fam in FAMILIES:
        base3d = _SPRITE_BASE.get(fam)
        for r in lists[fam]:
            # A sprite ucode only earns lists its 3D sibling cannot decode.
            if base3d and walk(mem, r.start, base3d, follow=False).ok:
                continue
            totals[fam] += max(_score(r), 0)
    # Ties (e.g. lists made only of RDP commands) carry no evidence; prefer the most
    # common family, and 3D over sprite ucodes, which share most immediates.
    best = max(FAMILIES, key=lambda f: (totals[f], -_PREFERENCE.index(f)))
    return best if totals[best] > 0 else None


# ---------------------------------------------------------------------------
# Scanning
# ---------------------------------------------------------------------------

_MIN_USEFUL = {"G_VTX", "G_TRI1", "G_TRI2", "G_QUAD", "G_DL", "G_SETTIMG", "G_LOADBLOCK",
               "G_LOADTILE", "G_SETCOMBINE", "G_TEXRECT", "G_FILLRECT", "G_SETTILE",
               "G_SETOTHERMODE_H", "G_SETOTHERMODE_L", "G_SETGEOMETRYMODE", "G_GEOMETRYMODE",
               "G_MTX", "G_OBJ_SPRITE", "G_BG_1CYC", "G_BG_COPY", "G_OBJ_RECTANGLE",
               "G_SETPRIMCOLOR", "G_SETENVCOLOR", "G_TEXTURE", "G_SETFILLCOLOR"}


def find_display_lists(data: bytes, family: str, *, memory: Memory | None = None, min_commands: int = 2,
                       limit: int | None = None, start: int = 0,
                       end: int | None = None) -> list[WalkResult]:
    """Find display lists in raw bytes.

    Every 8-byte-aligned G_ENDDL word pair is a candidate terminator; the list start is
    found by stepping backwards while commands decode without error, then the list is
    re-walked forwards (without following external lists) to validate it. ``memory``
    supplies a segment table/origin for resolving references (default: none). Overlapping
    candidates are resolved in favour of the earliest start.
    """
    end = len(data) if end is None else end
    needle = struct.pack(">II", ENDDL_WORDS[family], 0)
    mem = memory if memory is not None else Memory.unmapped(data)
    out: list[WalkResult] = []
    prev_end = start
    pos = start
    while True:
        i = data.find(needle, pos, end)
        if i < 0:
            break
        pos = i + 8
        if i % 8:
            continue
        s = i
        while s - 8 >= prev_end:
            w0, w1 = struct.unpack_from(">II", data, s - 8)
            c = decode(w0, w1, family)
            if c.error or c.name == "G_ENDDL":
                break
            s -= 8
        # Trim leading commands until a forward walk succeeds.
        while s < i:
            r = walk(mem, s, family, follow=False)
            if r.ok and r.end == i + 8:
                break
            s += 8
        else:
            r = walk(mem, s, family, follow=False)
        if not r.ok or r.end != i + 8 or len(r.commands) < min_commands:
            continue
        if not any(c.name in _MIN_USEFUL for c in r.commands):
            continue
        out.append(r)
        prev_end = i + 8
        if limit and len(out) >= limit:
            break
    return out


@dataclass
class Blob:
    start: int
    end: int
    walk: WalkResult


def partition_blobs(data: bytes, family: str, start: int = 0, end: int | None = None,
                    align: int = 8) -> tuple[list[Blob], list[tuple[int, int]]]:
    """Split a pool of self-relative blobs (display list at offset 0, segment-0 addresses
    relative to the blob start, data following the list) into consecutive blobs.

    Each blob extends to the end of the furthest data it references (rounded up to
    ``align``); bytes that do not start a valid list are reported as gaps.
    """
    end = len(data) if end is None else end
    blobs, gaps = [], []
    cur = start
    gap_start = None
    view = memoryview(data)
    while cur < end:
        r = walk(Memory(bytes(view[cur:end])), 0, family)
        if r.ok and r.commands[0].name != "G_SPNOOP":
            lo, hi = r.extent()
            hi = (hi + align - 1) // align * align
            if gap_start is not None:
                gaps.append((gap_start, cur))
                gap_start = None
            _shift(r, cur)
            blobs.append(Blob(cur, cur + hi, r))
            cur += hi
        else:
            if gap_start is None:
                gap_start = cur
            cur += align
    if gap_start is not None:
        gaps.append((gap_start, end))
    return blobs, gaps


_ZERO8 = bytes(8)


@dataclass
class Pool:
    start: int
    end: int
    blobs: list[Blob]


def find_blob_pools(data: bytes, family: str, start: int = 0, end: int | None = None, *,
                    lists: list[WalkResult] | None = None, min_blobs: int = 4,
                    window: int = 0x40000, back: int = 0x2000, align: int = 8) -> list[Pool]:
    """Find pools of consecutive self-relative blobs (see :func:`partition_blobs`) without
    knowing their bounds.

    Display lists found by :func:`find_display_lists` (or ``lists``) are the candidates; a
    candidate whose list walks with segment-0 addresses relative to its own start opens a
    pool, which is partitioned forward until the next offset does not start a blob.  Each
    walk sees at most ``window`` bytes, so a blob's data must lie within that distance.
    Runs of fewer than ``min_blobs`` blobs are not reported.  A pool is extended backwards
    (up to ``back`` bytes per blob) over blobs that end exactly at its start.
    """
    end = len(data) if end is None else end
    if lists is None:
        lists = find_display_lists(data, family, start=start, end=end)
    view = memoryview(data)
    pools: list[Pool] = []
    covered_to = start

    def blob_at(cur: int) -> Blob | None:
        if view[cur:cur + 8] == _ZERO8:  # G_SPNOOP: no blob starts with one
            return None
        r = walk(Memory(view[cur:min(end, cur + window)]), 0, family)
        if not r.ok or r.commands[0].name == "G_SPNOOP":
            return None
        # a blob's data follows its lists; data inside the commands means a misaligned start
        if any(s < e2 and s2 < e for s, e, _k in r.data_spans() for s2, e2 in r.dls):
            return None
        _lo, hi = r.extent()
        hi = (hi + align - 1) // align * align
        _shift(r, cur)
        return Blob(cur, cur + hi, r)

    for r0 in sorted((r for r in lists if start <= r.start < end), key=lambda r: r.start):
        if r0.end is not None and r0.end <= covered_to:
            continue
        # The scan's list start can be misaligned with the blob (it may begin with the
        # previous blob's trailing commands, or run back through zero padding): when it does
        # not start a blob, try the command offsets up to the list's first self-relative
        # reference (a segment-0 address within the window), at most ``back`` bytes before it.
        first = blob_at(r0.start) if r0.start >= covered_to else None
        if first is None:
            sites = [c.offset for c in r0.commands
                     if c.ref and c.ref[1] >> 24 == 0 and c.ref[1] < window and c.offset >= 0]
            if sites:
                lo = max(r0.start, covered_to, sites[0] - back)
                lo += -lo % align
                for cand in range(lo, sites[0] + align, align):
                    if (first := blob_at(cand)) is not None:
                        break
        if first is None:
            continue
        blobs, cur = [first], first.end
        while cur < end:
            b = blob_at(cur)
            if b is None:
                break
            blobs.append(b)
            cur = b.end
        covered_to = cur
        if len(blobs) < min_blobs:
            continue
        # The scan can miss a pool's first blobs (a list it trimmed or merged): extend
        # backwards over blobs that end exactly where the pool starts.
        floor = max(start, pools[-1].end if pools else start)
        while True:
            head = blobs[0].start
            prev = next((b for off in range(head - align, max(floor, head - back) - 1, -align)
                         if (b := blob_at(off)) is not None and b.end == head), None)
            if prev is None:
                break
            blobs.insert(0, prev)
        pools.append(Pool(blobs[0].start, min(cur, end), blobs))
    return pools


def _shift(r: WalkResult, base: int) -> None:
    """Rebase a blob-relative walk result onto the enclosing buffer."""
    r.start += base
    r.end = None if r.end is None else r.end + base
    r.dls = [(s + base, e + base) for s, e in r.dls]
    r.vertices = [(a, None if o is None else o + base, n) for a, o, n in r.vertices]
    for t in r.textures:
        t.offset = None if t.offset is None else t.offset + base
        t.palette_offset = None if t.palette_offset is None else t.palette_offset + base
    r.palettes = [(a, None if o is None else o + base, n) for a, o, n in r.palettes]
    r.matrices = [(a, None if o is None else o + base) for a, o in r.matrices]
    r.sub_dls = [(a, None if o is None else o + base) for a, o in r.sub_dls]
    r.other_refs = [(k, a, None if o is None else o + base, n) for k, a, o, n in r.other_refs]
    for c in r.commands:
        c.offset += base
