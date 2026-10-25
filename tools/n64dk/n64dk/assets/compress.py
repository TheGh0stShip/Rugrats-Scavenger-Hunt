"""Compression codecs used by N64 games and a ROM-wide compressed-stream scanner.

Codecs (each decodes *and* encodes, and reports the exact number of input bytes a
stream consumes):

* ``yay0`` / ``yaz0`` -- Nintendo LZ formats ("Yay0": separate mask/link/literal
  sections; "Yaz0": interleaved code bytes).
* ``mio0`` -- the older Nintendo split-section LZ format.  The encoder implements the
  common greedy parser with one-byte lazy look-ahead, farthest-offset tie breaking,
  4096-byte window and 3..18 byte matches; it reproduces retail MIO0 streams of games
  built with that parser byte for byte.
* ``lzari`` -- big-endian LZARI: a ``u32`` expanded size followed by Okumura's 1989
  LZARI arithmetic-coded bit stream (N=4096, F=60, THRESHOLD=2).  The encoder is the
  original binary-tree parser, so streams produced by it re-encode byte-identically.
  Adapted from an earlier GPL-3.0 N64 LZARI implementation.
* ``zlib`` / ``gzip`` / ``deflate`` -- standard library wrappers (raw deflate has no
  header; zlib and gzip have the usual magic).

New codecs register with :func:`register`.  :func:`scan` finds candidate streams
anywhere in a ROM image and validates each by a complete in-bounds decode.
"""
from __future__ import annotations

import bisect
import struct
import zlib
from dataclasses import dataclass, field
from typing import Callable, Iterable


class CodecError(ValueError):
    pass


@dataclass(frozen=True)
class Decoded:
    data: bytes
    consumed: int  # input bytes the stream occupies, header included


@dataclass
class Codec:
    name: str
    decode: Callable[..., Decoded]  # (buf, offset=0, max_output=...) -> Decoded
    encode: Callable[[bytes], bytes]
    magic: bytes | None = None
    description: str = ""


REGISTRY: dict[str, Codec] = {}
DEFAULT_MAX_OUTPUT = 16 << 20


def register(codec: Codec) -> Codec:
    REGISTRY[codec.name] = codec
    return codec


def decode(name: str, buf: bytes, offset: int = 0, max_output: int = DEFAULT_MAX_OUTPUT) -> Decoded:
    return REGISTRY[name].decode(buf, offset, max_output)


def encode(name: str, data: bytes) -> bytes:
    return REGISTRY[name].encode(data)


# ---------------------------------------------------------------------------
# Shared LZ parser (window 4096, minimum match 3)


class _Matcher:
    """Position index by first byte; finds the longest match, farthest on ties.

    Candidates are visited farthest first and only replace the best on a strictly
    longer match, so each candidate costs one slice comparison."""

    def __init__(self, data: bytes, window: int = 4096):
        self.data = data
        self.window = window
        self.index: dict[int, list[int]] = {}
        for i, b in enumerate(data):
            self.index.setdefault(b, []).append(i)

    def longest(self, pos: int, max_len: int) -> tuple[int, int]:
        data = self.data
        if max_len <= 0:
            return 0, 0
        cands = self.index[data[pos]]
        lo = bisect.bisect_left(cands, max(pos - self.window, 0))
        hi = bisect.bisect_left(cands, pos, lo)
        best_len = best_dist = 0
        for k in range(lo, hi):
            off = cands[k]
            need = best_len + 1
            if data[off:off + need] != data[pos:pos + need]:
                continue
            n = need
            while n < max_len and data[off + n] == data[pos + n]:
                n += 1
            best_len, best_dist = n, pos - off
            if n == max_len:
                break
        return best_len, best_dist


def lz_parse(data: bytes, max_len: int, lazy: bool = True, window: int = 4096) -> list[tuple[int, int]]:
    """Token list: ``(0, byte)`` literal or ``(length, distance)`` match.

    The first byte is always a literal.  With ``lazy``, a match is deferred by one
    literal when the match starting at the next byte is at least two bytes longer.
    """
    out: list[tuple[int, int]] = []
    n = len(data)
    if not n:
        return out
    m = _Matcher(data, window)
    out.append((0, data[0]))
    pos = 1
    while pos < n:
        length, dist = m.longest(pos, min(n - pos, max_len))
        if length > 2:
            if lazy:
                la_len, la_dist = m.longest(pos + 1, min(n - pos - 1, max_len)) if pos + 1 < n else (0, 0)
                if length + 1 < la_len:
                    out.append((0, data[pos]))
                    pos += 1
                    length, dist = la_len, la_dist
            out.append((length, dist))
            pos += length
        else:
            out.append((0, data[pos]))
            pos += 1
    return out


def _check_size(size: int, max_output: int) -> None:
    if size > max_output:
        raise CodecError(f"declared size 0x{size:X} exceeds limit 0x{max_output:X}")


def _need(buf: bytes, end: int) -> None:
    if end > len(buf):
        raise CodecError("stream runs past the end of the input")


# ---------------------------------------------------------------------------
# MIO0 and Yay0 (split mask / link / literal sections)


def _split_decode(buf: bytes, offset: int, max_output: int, magic: bytes, yay0: bool) -> Decoded:
    _need(buf, offset + 16)
    if buf[offset:offset + 4] != magic:
        raise CodecError(f"missing {magic.decode()} magic")
    size, link, chunk = struct.unpack_from(">III", buf, offset + 4)
    _check_size(size, max_output)
    if link < 16 or chunk < 16:
        raise CodecError("section offsets out of order")
    _need(buf, offset + max(link, chunk))
    out = bytearray()
    mask_pos = offset + 16
    lp, cp = offset + link, offset + chunk
    bits = mask = 0
    while len(out) < size:
        if bits == 0:
            _need(buf, mask_pos + 4)
            mask = struct.unpack_from(">I", buf, mask_pos)[0]
            mask_pos += 4
            bits = 32
        bits -= 1
        if mask & (1 << bits):
            _need(buf, cp + 1)
            out.append(buf[cp])
            cp += 1
        else:
            _need(buf, lp + 2)
            word = (buf[lp] << 8) | buf[lp + 1]
            lp += 2
            dist = (word & 0xFFF) + 1
            n = word >> 12
            if yay0:
                if n == 0:
                    _need(buf, cp + 1)
                    n = buf[cp] + 18
                    cp += 1
                else:
                    n += 2
            else:
                n += 3
            src = len(out) - dist
            if src < 0:
                raise CodecError("back-reference before the start of the output")
            if len(out) + n > size:
                raise CodecError("match runs past the declared size")
            for i in range(n):
                out.append(out[src + i])
    # Mask words precede the link/literal sections, so the stream ends with them.
    return Decoded(bytes(out), max(lp, cp) - offset)


def _split_encode(data: bytes, magic: bytes, yay0: bool) -> bytes:
    tokens = lz_parse(data, 273 if yay0 else 18)
    bits: list[int] = []
    links = bytearray()
    chunks = bytearray()
    for length, val in tokens:
        if length == 0:
            bits.append(1)
            chunks.append(val)
        else:
            bits.append(0)
            dist = val - 1
            if yay0:
                if length >= 18:
                    links += struct.pack(">H", dist)
                    chunks.append(length - 18)
                else:
                    links += struct.pack(">H", ((length - 2) << 12) | dist)
            else:
                links += struct.pack(">H", ((length - 3) << 12) | dist)
    if yay0:
        nwords = (len(bits) + 31) // 32
        mask = bytearray(4 * nwords)
    else:
        mask = bytearray((len(bits) + 7) // 8)
    for i, b in enumerate(bits):
        if b:
            mask[i >> 3] |= 0x80 >> (i & 7)
    link_off = (16 + len(mask) + 3) & ~3
    chunk_off = link_off + len(links)
    body = bytearray(magic + struct.pack(">III", len(data), link_off, chunk_off))
    body += mask
    body += bytes(link_off - len(body))
    body += links + chunks
    return bytes(body)


def mio0_decode(buf: bytes, offset: int = 0, max_output: int = DEFAULT_MAX_OUTPUT) -> Decoded:
    return _split_decode(buf, offset, max_output, b"MIO0", False)


def mio0_encode(data: bytes) -> bytes:
    return _split_encode(data, b"MIO0", False)


def yay0_decode(buf: bytes, offset: int = 0, max_output: int = DEFAULT_MAX_OUTPUT) -> Decoded:
    return _split_decode(buf, offset, max_output, b"Yay0", True)


def yay0_encode(data: bytes) -> bytes:
    return _split_encode(data, b"Yay0", True)


# ---------------------------------------------------------------------------
# Yaz0


def yaz0_decode(buf: bytes, offset: int = 0, max_output: int = DEFAULT_MAX_OUTPUT) -> Decoded:
    _need(buf, offset + 16)
    if buf[offset:offset + 4] != b"Yaz0":
        raise CodecError("missing Yaz0 magic")
    size = struct.unpack_from(">I", buf, offset + 4)[0]
    _check_size(size, max_output)
    out = bytearray()
    p = offset + 16
    code = bits = 0
    while len(out) < size:
        if bits == 0:
            _need(buf, p + 1)
            code, bits = buf[p], 8
            p += 1
        bits -= 1
        if code & (1 << bits):
            _need(buf, p + 1)
            out.append(buf[p])
            p += 1
        else:
            _need(buf, p + 2)
            b1, b2 = buf[p], buf[p + 1]
            p += 2
            dist = ((b1 & 0xF) << 8 | b2) + 1
            n = b1 >> 4
            if n == 0:
                _need(buf, p + 1)
                n = buf[p] + 0x12
                p += 1
            else:
                n += 2
            src = len(out) - dist
            if src < 0:
                raise CodecError("back-reference before the start of the output")
            if len(out) + n > size:
                raise CodecError("match runs past the declared size")
            for i in range(n):
                out.append(out[src + i])
    return Decoded(bytes(out), p - offset)


def yaz0_encode(data: bytes) -> bytes:
    out = bytearray(b"Yaz0" + struct.pack(">I", len(data)) + bytes(8))
    tokens = lz_parse(data, 0x111)
    for g in range(0, len(tokens), 8):
        code_at = len(out)
        out.append(0)
        code = 0
        for i, (length, val) in enumerate(tokens[g:g + 8]):
            if length == 0:
                code |= 0x80 >> i
                out.append(val)
            else:
                dist = val - 1
                if length >= 0x12:
                    out += bytes((dist >> 8, dist & 0xFF, length - 0x12))
                else:
                    out += bytes((((length - 2) << 4) | (dist >> 8), dist & 0xFF))
        out[code_at] = code
    return bytes(out)


# ---------------------------------------------------------------------------
# LZARI (big-endian size header). Adapted from earlier GPL-3.0 tooling.
# tools/lzari.py (GPL-3.0).

LZ_N = 4096
LZ_F = 60
LZ_THRESHOLD = 2
LZ_NCHAR = 256 - LZ_THRESHOLD + LZ_F
Q1 = 1 << 15
Q2, Q3, Q4 = 2 * Q1, 3 * Q1, 4 * Q1
MAX_CUM = Q1 - 1


def _position_cum() -> list[int]:
    cum = [0] * (LZ_N + 1)
    for i in range(LZ_N, 0, -1):
        cum[i - 1] = cum[i] + 10000 // (i + 200)
    return cum


_POS_CUM = _position_cum()


class _Model:
    __slots__ = ("char_to_sym", "sym_to_char", "freq", "cum")

    def __init__(self):
        self.char_to_sym = [0] * LZ_NCHAR
        self.sym_to_char = [0] * (LZ_NCHAR + 1)
        self.freq = [0] * (LZ_NCHAR + 1)
        self.cum = [0] * (LZ_NCHAR + 1)
        for sym in range(LZ_NCHAR, 0, -1):
            self.char_to_sym[sym - 1] = sym
            self.sym_to_char[sym] = sym - 1
            self.freq[sym] = 1
            self.cum[sym - 1] = self.cum[sym] + 1

    def update(self, sym: int) -> None:
        freq, cum = self.freq, self.cum
        if cum[0] >= MAX_CUM:
            total = 0
            for i in range(LZ_NCHAR, 0, -1):
                cum[i] = total
                freq[i] = (freq[i] + 1) >> 1
                total += freq[i]
            cum[0] = total
        i = sym
        f = freq[sym]
        while freq[i - 1] == f:
            i -= 1
        if i < sym:
            a, b = self.sym_to_char[i], self.sym_to_char[sym]
            self.sym_to_char[i], self.sym_to_char[sym] = b, a
            self.char_to_sym[a] = sym
            self.char_to_sym[b] = i
        freq[i] += 1
        cum[:i] = [c + 1 for c in cum[:i]]


class _LzariDecoder:
    def __init__(self, buf: bytes, start: int, end: int):
        self.buf, self.pos, self.end = buf, start, end
        self.mask = 0
        self.byte = 0
        self.padding_bits = 0
        self.low, self.high, self.value = 0, Q4, 0
        self.model = _Model()

    def bit(self) -> int:
        self.mask >>= 1
        if self.mask == 0:
            if self.pos >= self.end:
                # getc() EOF behaves as an all-ones byte; retail tails rely on it.
                self.padding_bits += 8
                if self.padding_bits > 32:
                    raise CodecError("LZARI stream ended before the output was complete")
                self.byte = 0xFF
            else:
                self.byte = self.buf[self.pos]
                self.pos += 1
            self.mask = 0x80
        return 1 if self.byte & self.mask else 0

    def renormalize(self) -> None:
        low, high, value = self.low, self.high, self.value
        while True:
            if low >= Q2:
                value -= Q2; low -= Q2; high -= Q2
            elif low >= Q1 and high <= Q3:
                value -= Q1; low -= Q1; high -= Q1
            elif high > Q2:
                break
            low <<= 1
            high <<= 1
            value = (value << 1) | self.bit()
        self.low, self.high, self.value = low, high, value

    @staticmethod
    def _search(cum: list[int], x: int, hi: int) -> int:
        lo = 1
        while lo < hi:
            mid = (lo + hi) >> 1
            if cum[mid] > x:
                lo = mid + 1
            else:
                hi = mid
        return lo

    def character(self) -> int:
        m = self.model
        cum = m.cum
        span = self.high - self.low
        x = ((self.value - self.low + 1) * cum[0] - 1) // span
        sym = self._search(cum, x, LZ_NCHAR)
        self.high = self.low + span * cum[sym - 1] // cum[0]
        self.low += span * cum[sym] // cum[0]
        self.renormalize()
        c = m.sym_to_char[sym]
        m.update(sym)
        return c

    def position(self) -> int:
        cum = _POS_CUM
        span = self.high - self.low
        x = ((self.value - self.low + 1) * cum[0] - 1) // span
        p = self._search(cum, x, LZ_N) - 1
        self.high = self.low + span * cum[p] // cum[0]
        self.low += span * cum[p + 1] // cum[0]
        self.renormalize()
        return p


@dataclass(frozen=True)
class LzariResult(Decoded):
    padding_bits: int = 0


def lzari_decode(buf: bytes, offset: int = 0, max_output: int = DEFAULT_MAX_OUTPUT,
                 limit: int | None = None, end: int | None = None, strict_ring: bool = False) -> LzariResult:
    """Decode a stream at ``offset``; ``limit`` stops after that many output bytes
    (a prefix decode for cheap validation); ``end`` bounds the input.  With
    ``strict_ring`` a match reaching back into the initial space-filled window is an
    error: encoders only do that for data that itself repeats spaces, while arbitrary
    bytes decode to such matches almost at once."""
    end = len(buf) if end is None else end
    if offset + 4 > end:
        raise CodecError("LZARI stream is too short")
    size = struct.unpack_from(">I", buf, offset)[0]
    if size == 0:
        return LzariResult(b"", 4, 0)
    _check_size(size, max_output)
    want = size if limit is None else min(size, limit)
    d = _LzariDecoder(buf, offset + 4, end)
    for _ in range(17):
        d.value = (d.value << 1) | d.bit()
    ring = bytearray(b" " * LZ_N)
    r = LZ_N - LZ_F
    out = bytearray()
    while len(out) < want:
        c = d.character()
        if c < 256:
            out.append(c)
            ring[r] = c
            r = (r + 1) & (LZ_N - 1)
            continue
        back = d.position() + 1
        if strict_ring and back > len(out):
            raise CodecError("match reaches into the initial window")
        src = (r - back) & (LZ_N - 1)
        n = c - 255 + LZ_THRESHOLD
        if len(out) + n > size:
            raise CodecError("LZARI match runs past the declared size")
        for i in range(n):
            c = ring[(src + i) & (LZ_N - 1)]
            out.append(c)
            ring[r] = c
            r = (r + 1) & (LZ_N - 1)
    # Bits read = coder shifts + the 17-bit initial fill; the encoder's flush emits the
    # shifts plus two terminating bits, zero-padded to a byte.
    unread = (d.mask.bit_length() - 1) if d.mask else 0
    bits = (d.pos - offset - 4) * 8 + d.padding_bits - unread
    exact = 4 + (bits - 15 + 7) // 8 if limit is None or want == size else d.pos - offset
    return LzariResult(bytes(out), min(exact, d.pos - offset), d.padding_bits)


class _LzariEncoder:
    """Okumura's LZARI encoder with the binary-tree match finder (exact parser)."""

    def __init__(self, source: bytes, expect: bytes | None = None, prefix_only: bool = False):
        N, F = LZ_N, LZ_F
        self.source = source
        self.expect = expect  # stream bytes every pre-flush output byte must equal
        self.verified = 0
        self.flushing = False
        self.prefix_only = prefix_only  # source is a prefix: stop once the look-ahead runs short
        self.low, self.high, self.pending = 0, Q4, 0
        self.out = bytearray()
        self.obyte, self.omask = 0, 0x80
        self.model = _Model()
        self.buffer = bytearray(N + F - 1)
        self.left = [N] * (N + 1)
        self.right = [N] * (N + 257)
        self.parent = [N] * (N + 1)
        self.match_position = self.match_length = 0

    def put(self, bit: int) -> None:
        if bit:
            self.obyte |= self.omask
        self.omask >>= 1
        if self.omask == 0:
            if self.expect is not None and not self.flushing:
                i = len(self.out)
                if i >= len(self.expect) or self.expect[i] != self.obyte:
                    raise _Mismatch
                self.verified = i + 1
            self.out.append(self.obyte)
            self.obyte, self.omask = 0, 0x80

    def output(self, bit: int) -> None:
        self.put(bit)
        while self.pending:
            self.put(not bit)
            self.pending -= 1

    def renormalize(self) -> None:
        while True:
            if self.high <= Q2:
                self.output(0)
            elif self.low >= Q2:
                self.output(1)
                self.low -= Q2
                self.high -= Q2
            elif self.low >= Q1 and self.high <= Q3:
                self.pending += 1
                self.low -= Q1
                self.high -= Q1
            else:
                return
            self.low <<= 1
            self.high <<= 1

    def character(self, c: int) -> None:
        m = self.model
        sym = m.char_to_sym[c]
        span = self.high - self.low
        base = self.low
        self.high = base + span * m.cum[sym - 1] // m.cum[0]
        self.low = base + span * m.cum[sym] // m.cum[0]
        self.renormalize()
        m.update(sym)

    def position(self, p: int) -> None:
        span = self.high - self.low
        base = self.low
        self.high = base + span * _POS_CUM[p] // _POS_CUM[0]
        self.low = base + span * _POS_CUM[p + 1] // _POS_CUM[0]
        self.renormalize()

    def insert(self, node: int) -> None:
        N, F = LZ_N, LZ_F
        buf, left, right, parent = self.buffer, self.left, self.right, self.parent
        p = N + 1 + buf[node]
        cmp = 1
        right[node] = left[node] = N
        self.match_length = 0
        while True:
            if cmp >= 0:
                if right[p] != N:
                    p = right[p]
                else:
                    right[p] = node
                    parent[node] = p
                    return
            elif left[p] != N:
                p = left[p]
            else:
                left[p] = node
                parent[node] = p
                return
            cmp = buf[node + 1] - buf[p + 1]
            if cmp:
                i = 1
            elif buf[node:node + F] == buf[p:p + F]:
                i, cmp = F, 0
            else:  # binary search for the first differing byte
                lo, hi = 2, F - 1
                while lo < hi:
                    mid = (lo + hi) >> 1
                    if buf[node + 2:node + mid + 1] == buf[p + 2:p + mid + 1]:
                        lo = mid + 1
                    else:
                        hi = mid
                i = lo
                cmp = buf[node + i] - buf[p + i]
            if i > LZ_THRESHOLD:
                dist = (node - p) & (N - 1)
                if i > self.match_length:
                    self.match_position, self.match_length = dist, i
                    if i >= F:
                        break
                elif i == self.match_length and dist < self.match_position:
                    self.match_position = dist
        parent[node] = parent[p]
        left[node] = left[p]
        right[node] = right[p]
        parent[left[p]] = node
        parent[right[p]] = node
        if right[parent[p]] == p:
            right[parent[p]] = node
        else:
            left[parent[p]] = node
        parent[p] = N

    def delete(self, node: int) -> None:
        N = LZ_N
        left, right, parent = self.left, self.right, self.parent
        if parent[node] == N:
            return
        if right[node] == N:
            q = left[node]
        elif left[node] == N:
            q = right[node]
        else:
            q = left[node]
            if right[q] != N:
                while right[q] != N:
                    q = right[q]
                right[parent[q]] = left[q]
                parent[left[q]] = parent[q]
                left[q] = left[node]
                parent[left[node]] = q
            right[q] = right[node]
            parent[right[node]] = q
        parent[q] = parent[node]
        if right[parent[node]] == node:
            right[parent[node]] = q
        else:
            left[parent[node]] = q
        parent[node] = N

    def encode(self) -> bytes:
        N, F = LZ_N, LZ_F
        src = self.source
        if not src:
            return struct.pack(">I", 0)
        read_at = min(F, len(src))
        remaining = read_at
        s = N - F
        r_exp = 0
        self.buffer[:s] = b" " * s
        self.buffer[s:s + read_at] = src[:read_at]
        for i in range(1, F + 1):
            self.insert(s - i)
        self.insert(s)
        while remaining:
            if self.prefix_only and read_at >= len(src):
                return b""
            self.match_length = min(self.match_length, remaining)
            if self.match_length <= LZ_THRESHOLD:
                self.match_length = 1
                self.character(self.buffer[s])
            else:
                self.character(255 - LZ_THRESHOLD + self.match_length)
                self.position(self.match_position - 1)
            consumed = self.match_length
            done = 0
            while done < consumed and read_at < len(src):
                self.delete(r_exp)
                c = src[read_at]
                read_at += 1
                self.buffer[r_exp] = c
                if r_exp < F - 1:
                    self.buffer[r_exp + N] = c
                r_exp = (r_exp + 1) & (N - 1)
                s = (s + 1) & (N - 1)
                self.insert(s)
                done += 1
            while done < consumed:
                self.delete(r_exp)
                r_exp = (r_exp + 1) & (N - 1)
                s = (s + 1) & (N - 1)
                remaining -= 1
                if remaining:
                    self.insert(s)
                done += 1
        self.flushing = True
        self.pending += 1
        self.output(0 if self.low < Q1 else 1)
        for _ in range(7):
            self.put(0)
        return struct.pack(">I", len(src)) + bytes(self.out)


def lzari_encode(data: bytes) -> bytes:
    return _LzariEncoder(data).encode()


def lzari_probe(buf: bytes, offset: int, end: int | None = None, prefix: int = 1024,
                min_size: int = 8, max_output: int = DEFAULT_MAX_OUTPUT) -> bool:
    """Cheap test for a header-less LZARI stream at ``offset``.

    Decodes a short output prefix and re-encodes it with the exact encoder, checking
    every byte it emits against the stream.  The parser only looks ``F`` bytes ahead
    and arithmetic-coder bytes are final once emitted, so a genuine stream agrees up
    to the encoder's final flush, while arbitrary data diverges within a few bytes.
    A short prefix is tried before the full one.
    """
    end = len(buf) if end is None else end
    if offset + 8 > end:
        return False
    size = struct.unpack_from(">I", buf, offset)[0]
    if not min_size <= size <= max_output:
        return False
    for limit in sorted({min(prefix, 256), prefix}):
        if not _probe_once(buf, offset, end, size, limit, max_output):
            return False
    return True


def _probe_once(buf: bytes, offset: int, end: int, size: int, limit: int, max_output: int) -> bool:
    try:
        head = lzari_decode(buf, offset, max_output, limit=min(size, limit), end=end)
    except CodecError:
        return False
    sample = head.data
    if len(sample) < 16 and size > len(sample):
        return False
    enc = _LzariEncoder(sample, expect=buf[offset + 4:min(end, offset + head.consumed)],
                        prefix_only=size > len(sample))
    try:
        full = enc.encode()
    except _Mismatch:
        return False
    if size == len(head.data):  # whole stream decoded: require an exact re-encode
        return len(full) >= 6 and buf[offset:offset + len(full)] == full
    return enc.verified >= 4


class _Mismatch(Exception):
    pass


# ---------------------------------------------------------------------------
# Deflate family


def _zlib_decode(wbits: int, label: str):
    def run(buf: bytes, offset: int = 0, max_output: int = DEFAULT_MAX_OUTPUT) -> Decoded:
        d = zlib.decompressobj(wbits)
        try:
            out = d.decompress(buf[offset:], max_output + 1)
        except zlib.error as exc:
            raise CodecError(f"{label}: {exc}") from None
        if len(out) > max_output:
            raise CodecError(f"{label}: output exceeds limit")
        if not d.eof:
            raise CodecError(f"{label}: stream did not terminate")
        return Decoded(out, len(buf) - offset - len(d.unused_data))
    return run


def _zlib_encode(wbits: int, level: int = 9):
    def run(data: bytes) -> bytes:
        c = zlib.compressobj(level, zlib.DEFLATED, wbits)
        return c.compress(data) + c.flush()
    return run


def deflate_reencode_identical(raw: bytes, original: bytes, wbits: int = -15) -> tuple[int, int] | None:
    """Find a zlib (level, memLevel) that reproduces ``original`` exactly, if any."""
    for level in range(9, -1, -1):
        for mem in (8, 9, 7, 6, 5, 4, 3, 2, 1):
            c = zlib.compressobj(level, zlib.DEFLATED, wbits, mem)
            if c.compress(raw) + c.flush() == original:
                return level, mem
    return None


for _codec in (
    Codec("yay0", yay0_decode, yay0_encode, b"Yay0", "Nintendo Yay0 LZ"),
    Codec("yaz0", yaz0_decode, yaz0_encode, b"Yaz0", "Nintendo Yaz0 LZ"),
    Codec("mio0", mio0_decode, mio0_encode, b"MIO0", "Nintendo MIO0 LZ"),
    Codec("lzari", lzari_decode, lzari_encode, None, "big-endian size + Okumura LZARI"),
    Codec("zlib", _zlib_decode(15, "zlib"), _zlib_encode(15), None, "zlib (RFC 1950)"),
    Codec("gzip", _zlib_decode(31, "gzip"), _zlib_encode(31), b"\x1f\x8b\x08", "gzip (RFC 1952)"),
    Codec("deflate", _zlib_decode(-15, "deflate"), _zlib_encode(-15), None, "raw deflate (RFC 1951)"),
):
    register(_codec)


# ---------------------------------------------------------------------------
# Scanner


@dataclass
class Stream:
    rom: int
    codec: str
    consumed: int
    size: int
    method: str  # magic | probe | seed | chain | trial
    reencode: str = "-"  # identical | roundtrip | differs | -
    data: bytes | None = field(default=None, repr=False)

    @property
    def end(self) -> int:
        return self.rom + self.consumed

    def as_dict(self) -> dict:
        return {"rom": f"0x{self.rom:X}", "end": f"0x{self.end:X}", "codec": self.codec,
                "consumed": self.consumed, "size": self.size, "method": self.method,
                "reencode": self.reencode}


def _magic_streams(data: bytes, max_output: int) -> list[Stream]:
    out = []
    for name, codec in REGISTRY.items():
        if not codec.magic or name == "gzip":
            continue
        pos = data.find(codec.magic)
        while pos >= 0:
            try:
                d = codec.decode(data, pos, max_output)
                if d.data:
                    out.append(Stream(pos, name, d.consumed, len(d.data), "magic", data=d.data))
            except CodecError:
                pass
            pos = data.find(codec.magic, pos + 1)
    return out


def _zlib_streams(data: bytes, max_output: int, min_output: int) -> list[Stream]:
    out = []
    for magic, name in ((b"\x1f\x8b\x08", "gzip"),):
        pos = data.find(magic)
        while pos >= 0:
            try:
                d = REGISTRY[name].decode(data, pos, max_output)
                if len(d.data) >= min_output:
                    out.append(Stream(pos, name, d.consumed, len(d.data), "magic", data=d.data))
            except CodecError:
                pass
            pos = data.find(magic, pos + 1)
    pos = data.find(b"\x78")
    while 0 <= pos < len(data) - 2:
        flg = data[pos + 1]
        if (0x78 * 256 + flg) % 31 == 0 and not flg & 0x20:
            try:
                d = REGISTRY["zlib"].decode(data, pos, max_output)
                if len(d.data) >= min_output:
                    out.append(Stream(pos, "zlib", d.consumed, len(d.data), "magic", data=d.data))
            except CodecError:
                pass
        pos = data.find(b"\x78", pos + 1)
    return out


def _deflate_trial(data: bytes, align: int, covered, max_output: int, min_output: int,
                   start: int = 0, min_trial: int = 256) -> list[Stream]:
    out = []
    pos = (start + align - 1) // align * align
    n = len(data)
    view = memoryview(data)
    while pos < n - 8:
        skip = covered(pos)
        if skip:
            pos = (skip + align - 1) // align * align
            continue
        b0 = data[pos]
        # BTYPE 3 is invalid; an all-zero/all-FF word is padding, not a stream.
        if (b0 >> 1) & 3 == 3 or data[pos:pos + 4] in (b"\0\0\0\0", b"\xff\xff\xff\xff"):
            pos += align
            continue
        d = zlib.decompressobj(-15)
        try:
            res = d.decompress(view[pos:pos + 0x40000], max_output)
            if d.eof and len(res) >= min_output:
                consumed = min(0x40000, n - pos) - len(d.unused_data)
                # Short or incompressible "streams" are what arbitrary bytes decode to.
                if consumed >= min_trial and len(res) >= 1.5 * consumed:
                    out.append(Stream(pos, "deflate", consumed, len(res), "trial", data=res))
                    pos = (pos + consumed + align - 1) // align * align
                    continue
        except zlib.error:
            pass
        pos += align
    return out


class _Cover:
    def __init__(self):
        self.starts: list[int] = []
        self.ends: list[int] = []

    def add(self, a: int, b: int) -> None:
        i = bisect.bisect_left(self.starts, a)
        self.starts.insert(i, a)
        self.ends.insert(i, b)

    def __call__(self, pos: int) -> int:
        """End of the interval covering ``pos`` or 0."""
        i = bisect.bisect_right(self.starts, pos) - 1
        if i >= 0 and self.ends[i] > pos:
            return self.ends[i]
        return 0


def _lzari_full(data: bytes, pos: int, max_output: int, method: str) -> Stream | None:
    try:
        d = lzari_decode(data, pos, max_output)
    except CodecError:
        return None
    if d.padding_bits > 8 or len(d.data) < 8 or d.consumed < 6:
        return None
    return Stream(pos, "lzari", d.consumed, len(d.data), method, data=d.data)


def _lzari_quick(data: bytes, pos: int) -> bool:
    try:
        lzari_decode(data, pos, limit=48, strict_ring=True)
    except CodecError:
        return False
    return True


def scan_lzari(data: bytes, seeds: Iterable[int] = (), *, brute: bool = True, align: int = 4,
               max_output: int = 4 << 20, covered=None, chain_gap: int = 16) -> list[Stream]:
    """Find header-less LZARI streams.

    ``seeds`` (e.g. loader-evidenced range starts) are probed first.  Every accepted
    stream starts a chain: the next aligned word after its end (skipping zero padding)
    is probed too.  With ``brute`` every ``align``-aligned word that follows padding
    (or a non-matching region boundary) and holds a plausible size is probed.
    """
    covered = covered or _Cover()
    found: dict[int, Stream] = {}
    tried: set[int] = set()

    def chain(pos: int, method: str) -> None:
        while pos not in tried and pos + 8 <= len(data) and not covered(pos):
            tried.add(pos)
            if not lzari_probe(data, pos, max_output=max_output):
                return
            s = _lzari_full(data, pos, max_output, method)
            if s is None:
                return
            found[pos] = s
            covered.add(s.rom, s.end)
            nxt = s.end
            stop = nxt + chain_gap
            while nxt < stop and nxt + 8 <= len(data):
                if nxt % align == 0 and data[nxt:nxt + 4] != b"\0\0\0\0" and lzari_probe(data, nxt, prefix=256, max_output=max_output):
                    break
                nxt += 1
            else:
                return
            pos, method = nxt, "chain"

    for s in sorted(set(seeds)):
        if 0 <= s < len(data):
            chain(s, "seed")
    if brute:
        prev_zero = True
        for pos in range(0, len(data) - 8, align):
            word = data[pos:pos + 4]
            if word == b"\0\0\0\0":
                prev_zero = True
                continue
            if prev_zero:
                size = int.from_bytes(word, "big")
                if 8 <= size <= max_output and data[pos + 4:pos + 6] != b"\0\0" and _lzari_quick(data, pos):
                    chain(pos, "probe")
            prev_zero = False
    # A short all-literal stream re-encodes exactly from almost any bytes, so a group
    # of adjacent streams made only of tiny probe/chain finds is discarded.
    out = sorted(found.values(), key=lambda s: s.rom)
    keep: list[Stream] = []
    group: list[Stream] = []
    for s in out + [None]:
        if s is not None and group and 0 <= s.rom - group[-1].end < chain_gap:
            group.append(s)
            continue
        if group and any(g.size >= TINY_LZARI or g.method == "seed" for g in group):
            keep += group
        group = [s] if s is not None else []
    return keep


TINY_LZARI = 0x40


def scan(data: bytes, *, lzari_seeds: Iterable[int] = (), lzari_brute: bool = True,
         deflate_align: int = 0, min_output: int = 64, max_output: int = DEFAULT_MAX_OUTPUT,
         verify_reencode: bool = False, skip: list[tuple[int, int]] = ()) -> list[Stream]:
    """Find, validate and size compressed streams anywhere in ``data``.

    Magic-headed formats are decoded wherever their magic appears; LZARI uses seeds,
    chains and probe validation; raw deflate is tried every ``deflate_align`` bytes
    (0 disables).  Overlapping hits are resolved in favour of the earlier/longer one.
    ``skip`` ranges (e.g. known code) are excluded from header-less trials.
    """
    streams = _magic_streams(data, max_output) + _zlib_streams(data, max_output, min_output)
    cover = _Cover()
    for a, b in skip:
        cover.add(a, b)
    accepted: list[Stream] = []
    for s in sorted(streams, key=lambda s: (s.rom, -s.consumed)):
        if not cover(s.rom):
            accepted.append(s)
            cover.add(s.rom, s.end)
    accepted += scan_lzari(data, lzari_seeds, brute=lzari_brute, covered=cover)
    if deflate_align:
        accepted += _deflate_trial(data, deflate_align, cover, max_output, min_output)
    accepted.sort(key=lambda s: s.rom)
    if verify_reencode:
        for s in accepted:
            s.reencode = reencode_status(data, s)
    return accepted


def reencode_status(data: bytes, s: Stream) -> str:
    original = data[s.rom:s.end]
    raw = s.data if s.data is not None else decode(s.codec, data, s.rom).data
    if s.codec in ("deflate", "zlib", "gzip"):
        wbits = {"deflate": -15, "zlib": 15, "gzip": 31}[s.codec]
        return "identical" if deflate_reencode_identical(raw, original, wbits) else _roundtrip(s.codec, raw)
    enc = encode(s.codec, raw)
    if enc == original:
        return "identical"
    if s.codec == "lzari" and enc == original[:len(enc)] and len(original) - len(enc) <= 1:
        return "identical"
    return _roundtrip(s.codec, raw)


def _roundtrip(name: str, raw: bytes) -> str:
    enc = encode(name, raw)
    return "roundtrip" if decode(name, enc, 0).data == raw else "differs"
