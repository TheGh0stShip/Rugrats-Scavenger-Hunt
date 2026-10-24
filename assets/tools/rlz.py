"""Resource codec used by the game's loader (decoder: func_800166F0).

Stream grammar, one control byte ``c`` per token:

    c == 0x00        end of stream
    c & 0x80         back-reference: offset = ((c & 0x7F) << 3) | (next >> 5), length = next & 0x1F
    c & 0x40         run: the next byte repeated (c & 0x3F) times
    otherwise        literal run: copy the next ``c`` bytes (1..63)

The encoder reproduces the original packer exactly (verified on every compressed archive record):

* a run of three or more identical bytes is always emitted as a run token (max 63);
* otherwise the longest back-reference of at least three bytes is used; references never
  overlap the bytes being produced (length <= offset), length <= 31, offset <= 1023, and among
  equally long candidates the farthest offset wins;
* everything else is gathered into literal runs of at most 63 bytes.
"""


def decode(src: bytes, pos: int = 0) -> tuple[bytes, int]:
    """Decode one stream starting at ``src[pos]``; return (output, position after the terminator)."""
    out = bytearray()
    while True:
        c = src[pos]
        pos += 1
        if c == 0:
            return bytes(out), pos
        if c & 0x80:
            b = src[pos]
            pos += 1
            off = ((c & 0x7F) << 3) | (b >> 5)
            start = len(out) - off
            if start < 0:
                raise ValueError(f"back-reference before start of output at {pos - 2:#x}")
            for i in range(b & 0x1F):
                out.append(out[start + i])
        elif c & 0x40:
            out.extend(bytes([src[pos]]) * (c & 0x3F))
            pos += 1
        else:
            out.extend(src[pos:pos + c])
            pos += c


def encode(data: bytes) -> bytes:
    out = bytearray()
    lit = bytearray()
    index: dict[bytes, list[int]] = {}
    n = len(data)
    p = 0
    indexed = 0

    def flush() -> None:
        nonlocal lit
        while lit:
            chunk = lit[:63]
            out.append(len(chunk))
            out.extend(chunk)
            lit = lit[63:]

    while p < n:
        while indexed < p:
            if indexed + 3 <= n:
                index.setdefault(data[indexed:indexed + 3], []).append(indexed)
            indexed += 1
        run = 1
        while p + run < n and data[p + run] == data[p] and run < 63:
            run += 1
        best_len = best_off = 0
        for q in reversed(index.get(data[p:p + 3], ())):
            off = p - q
            if off > 1023:
                break
            length = 0
            limit = min(31, off)
            while length < limit and p + length < n and data[q + length] == data[p + length]:
                length += 1
            if length >= best_len:
                best_len, best_off = length, off
        if run >= 3:
            flush()
            out += bytes((0x40 | run, data[p]))
            p += run
        elif best_len >= 3:
            flush()
            out += bytes((0x80 | (best_off >> 3), ((best_off & 7) << 5) | best_len))
            p += best_len
        else:
            lit.append(data[p])
            p += 1
            if len(lit) == 63:
                flush()
    flush()
    out.append(0)
    return bytes(out)
