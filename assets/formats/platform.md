# Platform and small fixed regions

Tool: `tools/platform.py` (`roundtrip(rom)`; CLI prints one row per region).
All values big-endian. Every "exact" below is a byte comparison executed by the tool.

## Header (ROM 0x000..0x040)

| off | size | field | value |
|-----|------|-------|-------|
| 0x00 | u32 | PI BSD DOM1 config | 0x80371240 |
| 0x04 | u32 | clock rate | 0x0000000F |
| 0x08 | u32 | entry point | 0x80000400 |
| 0x0C | u32 | release (libultra 0x14 'D') | 0x00001444 |
| 0x10 | u32 | CRC1 | 0x0C02B3C5 |
| 0x14 | u32 | CRC2 | 0x9E2511B8 |
| 0x18 | 8 | reserved | zero |
| 0x20 | 20 | image name | `RUGRATSSCAVENGERHUNT` (fills all 20 bytes) |
| 0x34 | 7 | reserved | zero |
| 0x3B | 1 | media | `N` |
| 0x3C | 2 | cartridge id | `RG` |
| 0x3E | 1 | region | `E` |
| 0x3F | 1 | version | 0 |

Rebuild: fields are packed back and CRC1/CRC2 are **recomputed** with the CIC-6102 algorithm:
seed 0xF8CA4DDC for t1..t6; for each big-endian word d of ROM 0x1000..0x101000:
t6+=d (t4++ on carry); t3^=d; r=rol(d, d&31); t5+=r; t2 ^= (t2>d) ? r : (t6^d); t1 += t5^d.
CRC1 = t6^t4^t3, CRC2 = t5^t2^t1. The result equals the stored words (also cross-checked
against `n64dk.rom.compute_checksum(rom, "6102")`).

## IPL3 (ROM 0x040..0x1000)

Platform boot code, preserved as bytes. Identified as CIC-NUS-6102 IPL3: CRC-32 0x90BB6CB5,
MD5 e24dd796b2fa16511521139d28c8356b.

## Display-list block (ROM 0x57E80..0x57FD0, 0x150 bytes)

Loaded by the code at 0x80013B40: `a0 = 0xB0057E80`, `a1 = 0x800C7E30` (also stored to
0x80052EFC), `a2 = 0xB0057FD0 - a0 = 0x150`, `jal func_80028B58` at 0x80013B68 (PI DMA).
The destination is the end of BSS (0x800C7E30). Inside the block (segment 0x01 base):

| off | content |
|-----|---------|
| 0x000 | Vp: vscale (0x600,0x1E0,0x1FF,0), vtrans (0x400,0x1E0,0x1FF,0); referenced by `G_MOVEMEM 0x80, 0x01000000` |
| 0x010 | DL, 31 cmds: clear env/prim/blend/fog/fill colours, primdepth/convert/key, othermode_h, combine, scissor 0,0..512,240, 8x SETTILE, 8x SETTILESIZE, pipesync, ENDDL |
| 0x100 | DL, 5 cmds: SETOTHERMODE_L/H x4, ENDDL |
| 0x128 | DL, 5 cmds: MOVEMEM viewport, CLEAR/SETGEOMETRYMODE, TEXTURE off, ENDDL |

Commands are decoded with `n64dk/assets/gbi.py` (F3DEX family) and also parsed into a lossless
field model (including padding bits); the block is rebuilt by encoding those fields. Exact.

## Build stamp (ROM 0xF4D840..0xF4D850)

ASCII `05.04.99 11.13`, NUL, trailer byte 0x2A (`*`). Rebuilt from parsed fields. Exact.

## Tail padding (ROM 0xF4D850..0x1000000)

0xB27B0 bytes, all 0x00; regenerated from the rule. Exact.

## Ledger

`tools/ledger.py` composes `inventory/full_rom_ledger.tsv` from all `roundtrip()` sources
(`tools/archive_rt.py` wraps `tools/archive.py`), program text proven by `make verify-matching` run in a scratch clone of the pinned
commit (private_extracted/verify_clone; log verification/verify_matching.log, parsed units cached in
verification/code_units.json; manifest SHA-256 columns are source-file digests, not ROM hashes),
the assembled text at 0x103C..0x10F8 and 0x53474..0x535A0 classified as code,
and writes `verification/{gaps,overlaps,unknowns}.tsv`. Missing modules produce `unresolved` rows.
Content-level rows (mort/res2) go to `inventory/nested_ledger.tsv` with a containment check.
