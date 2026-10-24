# rlz resource codec

Decoder in the game: `func_800166F0(u8 *src, u8 *dst, u32 *outLen)`.

| control byte `c` | meaning |
|---|---|
| `0x00` | end of stream |
| `c & 0x80` | back-reference; with next byte `b`: offset = `((c & 0x7F) << 3) \| (b >> 5)` (1..1023), length = `b & 0x1F` |
| `c & 0x40` | run: next byte repeated `c & 0x3F` times |
| otherwise | literal: copy the next `c` bytes (1..63) |

## Encoder (reproduces the original packer bit-exactly)

At each position:

1. If the next three or more bytes are identical, emit a run token (the run is capped at 63).
2. Otherwise find the longest back-reference of at least 3 bytes. References are capped at 31 bytes, may not
   overlap the bytes being produced (length ≤ offset), and may reach back at most 1023 bytes. When several
   candidates are equally long, the farthest one wins.
3. Otherwise append the byte to the pending literal run, which is flushed in chunks of at most 63 bytes.

Verified against all 148 compressed records of archive 2 (tests/test_archive.py).
