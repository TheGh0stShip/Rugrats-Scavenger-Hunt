"""N64 texture formats: decode to RGBA8888, PNG export and dimension heuristics.

The pixel codecs and the PNG writer are adapted from ``tools/n64_texture.py`` of the
Earlier GPL-3.0 N64 decompilation tooling. Changes: palettes are passed separately
from the index data, IA16 TLUTs, IA4 and odd-width 4-bit rows are supported, and a
dimension guesser for headerless data was added.

Formats use the G_IM_FMT / G_IM_SIZ numbers: fmt 0 RGBA, 1 YUV, 2 CI, 3 IA, 4 I;
siz 0..3 = 4/8/16/32 bits per texel.
"""
from __future__ import annotations

import struct
import zlib
from dataclasses import dataclass

FMT_NAMES = {0: "RGBA", 1: "YUV", 2: "CI", 3: "IA", 4: "I"}
SIZ_BITS = {0: 4, 1: 8, 2: 16, 3: 32}
# (fmt, siz) pairs the RDP can sample from TMEM.
SUPPORTED = {(0, 2), (0, 3), (2, 0), (2, 1), (3, 0), (3, 1), (3, 2), (4, 0), (4, 1)}


class TextureError(ValueError):
    """Raised when a texture descriptor and its payload disagree."""


def format_name(fmt: int, siz: int) -> str:
    return f"{FMT_NAMES.get(fmt, f'FMT{fmt}')}{SIZ_BITS.get(siz, '?')}"


def parse_format(name: str) -> tuple[int, int]:
    """``"CI4"`` -> ``(2, 0)``."""
    name = name.upper()
    for fmt, label in FMT_NAMES.items():
        if name.startswith(label) and name[len(label):].isdigit():
            bits = int(name[len(label):])
            for siz, b in SIZ_BITS.items():
                if b == bits and (fmt, siz) in SUPPORTED:
                    return fmt, siz
    raise TextureError(f"unknown texture format {name!r}")


def texture_bytes(fmt: int, siz: int, width: int, height: int) -> int:
    return (width * SIZ_BITS[siz] + 7) // 8 * height


def _five_to_eight(v: int) -> int:
    return (v << 3) | (v >> 2)


def rgba16(pixel: int) -> bytes:
    return bytes((_five_to_eight((pixel >> 11) & 0x1F), _five_to_eight((pixel >> 6) & 0x1F),
                  _five_to_eight((pixel >> 1) & 0x1F), 0xFF if pixel & 1 else 0))


def ia16(pixel: int) -> bytes:
    i = pixel >> 8
    return bytes((i, i, i, pixel & 0xFF))


def decode_palette(data: bytes, count: int, tlut: str = "rgba16") -> list[bytes]:
    """Decode ``count`` big-endian 16-bit TLUT entries (``rgba16`` or ``ia16``)."""
    conv = ia16 if tlut == "ia16" else rgba16
    count = min(count, len(data) // 2)
    return [conv(v) for v in struct.unpack_from(f">{count}H", data)]


def _nibbles(data: bytes, width: int, height: int):
    """Yield 4-bit values row by row; each row starts on a byte boundary."""
    row = (width + 1) // 2
    for y in range(height):
        base = y * row
        for x in range(width):
            b = data[base + (x >> 1)]
            yield (b >> 4) if not x & 1 else (b & 0xF)


def decode(data: bytes, fmt: int, siz: int, width: int, height: int,
           palette: bytes | list[bytes] | None = None, tlut: str = "rgba16") -> bytes:
    """Return row-major RGBA8888 pixels.

    ``palette`` is raw TLUT bytes (or pre-decoded entries) for CI formats; a missing
    palette renders indices as greyscale.
    """
    if width <= 0 or height <= 0:
        raise TextureError("texture dimensions must be positive")
    if (fmt, siz) not in SUPPORTED:
        raise TextureError(f"unsupported N64 texture format/size: {fmt}/{siz}")
    need = texture_bytes(fmt, siz, width, height)
    if len(data) < need:
        raise TextureError(f"{format_name(fmt, siz)} {width}x{height} needs {need} bytes, got {len(data)}")
    pixels = width * height
    out = bytearray()
    if fmt == 0 and siz == 2:
        for v in struct.unpack_from(f">{pixels}H", data):
            out += rgba16(v)
    elif fmt == 0 and siz == 3:
        out += data[:pixels * 4]
    elif fmt == 2:
        n = 16 if siz == 0 else 256
        if palette is None:
            step = 17 if siz == 0 else 1
            pal = [bytes((i * step, i * step, i * step, 255)) for i in range(n)]
        elif isinstance(palette, (bytes, bytearray)):
            pal = decode_palette(bytes(palette), n, tlut)
        else:
            pal = list(palette)
        pal += [b"\0\0\0\0"] * (n - len(pal))
        src = _nibbles(data, width, height) if siz == 0 else data[:pixels]
        for i in src:
            out += pal[i]
    elif fmt == 3 and siz == 0:
        for v in _nibbles(data, width, height):
            i = (v >> 1) * 255 // 7
            out += bytes((i, i, i, 255 if v & 1 else 0))
    elif fmt == 3 and siz == 1:
        for v in data[:pixels]:
            i = (v >> 4) * 17
            out += bytes((i, i, i, (v & 0xF) * 17))
    elif fmt == 3 and siz == 2:
        for i, a in zip(data[0:pixels * 2:2], data[1:pixels * 2:2]):
            out += bytes((i, i, i, a))
    elif fmt == 4 and siz == 0:
        for v in _nibbles(data, width, height):
            i = v * 17
            out += bytes((i, i, i, 255))
    elif fmt == 4 and siz == 1:
        for i in data[:pixels]:
            out += bytes((i, i, i, 255))
    return bytes(out)


def encode_png(width: int, height: int, pixels: bytes) -> bytes:
    """Encode RGBA8888 pixels as a deterministic, non-interlaced PNG."""
    if len(pixels) != width * height * 4:
        raise TextureError("RGBA pixel count does not match the image dimensions")

    def chunk(kind: bytes, payload: bytes) -> bytes:
        body = kind + payload
        return struct.pack(">I", len(payload)) + body + struct.pack(">I", zlib.crc32(body))

    stride = width * 4
    scan = b"".join(b"\0" + pixels[o:o + stride] for o in range(0, len(pixels), stride))
    return (b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0))
            + chunk(b"IDAT", zlib.compress(scan, 9)) + chunk(b"IEND", b""))


def to_png(data: bytes, fmt: int, siz: int, width: int, height: int,
           palette=None, tlut: str = "rgba16") -> bytes:
    return encode_png(width, height, decode(data, fmt, siz, width, height, palette, tlut))


@dataclass(frozen=True)
class DimensionGuess:
    width: int
    height: int
    score: float  # mean absolute difference between vertically adjacent texels (lower = better)


def _texel_values(data: bytes, siz: int) -> list[int]:
    if siz == 0:
        return [v for b in data for v in (b >> 4, b & 0xF)]
    if siz == 1:
        return list(data)
    if siz == 2:
        # Compare 16-bit texels by summed 5-bit channels so RGBA16 noise in the low bits
        # does not dominate.
        return [((v >> 11) & 31) + ((v >> 6) & 31) + ((v >> 1) & 31)
                for v in struct.unpack_from(f">{len(data) // 2}H", data)]
    return [sum(data[i:i + 3]) for i in range(0, len(data) - 3, 4)]


def guess_dimensions(nbytes_or_data, siz: int, *, min_side: int = 4, max_side: int = 1024,
                     data: bytes | None = None, limit: int = 5) -> list[DimensionGuess]:
    """Rank power-of-two (width, height) pairs for a headerless texture of ``n`` bytes.

    With pixel data the ranking uses vertical coherence: at the true width each row
    resembles the row above it. Without data the squarest shapes are preferred.
    """
    if isinstance(nbytes_or_data, (bytes, bytearray)):
        data = bytes(nbytes_or_data)
        nbytes = len(data)
    else:
        nbytes = nbytes_or_data
    texels = nbytes * 8 // SIZ_BITS[siz]
    cands = []
    w = min_side
    while w <= max_side:
        if texels % w == 0:
            h = texels // w
            if min_side <= h <= max_side:
                cands.append((w, h))
        w *= 2
    if not cands:
        return []
    out = []
    values = _texel_values(data, siz) if data is not None else None
    for w, h in cands:
        if values is None:
            score = abs((w.bit_length()) - (h.bit_length())) + (0.5 if w < h else 0)
        else:
            diffs = [abs(values[i] - values[i - w]) for i in range(w, len(values))]
            horiz = [abs(values[i] - values[i - 1]) for i in range(1, len(values)) if i % w]
            # Normalise by horizontal variation so flat images do not tie on zero.
            score = (sum(diffs) / max(len(diffs), 1)) / (1 + sum(horiz) / max(len(horiz), 1))
        out.append(DimensionGuess(w, h, round(score, 4)))
    out.sort(key=lambda g: (g.score, abs(g.width.bit_length() - g.height.bit_length()), -g.width))
    return out[:limit]
