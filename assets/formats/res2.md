# Resource archive 2 record formats

Tool: `tools/res2.py` (`roundtrip(rom)`, CLI `python3 tools/res2.py [rom] [--export]`).
The archive container is handled by `tools/archive.py`. This document describes the payloads.
All values are big-endian. "Proven" means the layout is confirmed by game code or by an
invariant that holds for every record in the family. Every record is rebuilt from its parsed
form and compared byte-for-byte with the ROM.

## Families (531 records)

| family | records | proven type | evidence |
|---|---|---|---|
| mdlh | 376 | f3dex_model (376); 445 secondary streams = vertex keyframe animations | loader func_80013358; F3DEX walk; func_8000C490 -> func_80013570 -> func_8000B380 |
| imagebank | 107 | ci8_tile_bank | func_8000D0AC, func_800328CC/80032914 |
| libmus_song | 25 (504..528) | libmus_song (tools/audio_songs.py rebuild verified for all 25) | header 0x215 |
| pairtable | 14 | camera_path | func_8001E204 / func_8000FAE8 / func_8000F2B8 / func_8001E2B0 |
| gridmap | 6 (226..231) | grid_map | func_8000DACC |
| font | 2 (0, 1) | font | func_8001F588(0 or 1) |
| empty | 1 (530) | 0 bytes | |

## mdlh

    +00 'mdlh'
    +04 u32 decoded model size + 0x20 (holds for all 376)
    +08 u32 allocation size; equals +04 when there are no secondary streams, otherwise
        +04 + the largest decoded secondary stream (holds for all 127)
    +0C u32 byte length of the rlz model stream at +0x18 (exact for all 376)
    +10 u32 offset of the secondary stream area (0 = none), relative to the record
    +14 u32 length of the first secondary stream (matches 99 of 127; kept as stored)

Animation chain (proven):

* func_8000C1A4 tests flag 0x2000 in the model's u16 at inner+0 (set in every model that has
  secondary streams) and calls func_8000C490.
* func_8000C490 reads the animation header A = inner+[inner+0x18], picks the u16 stream id at
  A + 0x28 + 12*anim (anim = obj+8), and calls func_80013570 when it differs from the cached id
  at A+0x1A.
* func_80013570 reads the table entry at A + [A+0x1C] + 16*id, which is
  {u32 offset relative to word[4], u32 stored bytes, u32 decoded bytes, u32 ticks}. It decodes
  that stream from ROM into dst+word[1] and grows the 0x80052ECC maximum by the decoded size.
* The decoded block goes to func_8000B380, the keyframe vertex interpolator.

Every one of the 445 secondary streams has a matching table entry: same offset, same stored
length, same decoded length, and the entry's ticks equal the stream's own tick field. No stream
is left without an entry.

Decoded animation block (fields read by func_8000B380):

    +00 u32 offset (0x14 + 2*keys rounded)   +04 u32 frame data offset (relocated pointer)
    +08 u16 flags (0x8000: <<1, 0x2000: <<2 scale of packed coordinates)
    +0A u16 ticks   +0C u16 keyframe count   +0E u16 flags (0x80 = loop: the last key wraps)
    +10 u32 0       +14 u16 key tick[keyframes] (0 .. ticks)
    frame data: packed u32 per key and vertex: x = bits 0..10, y = bits 11..21, z = bits 22..31
    (signed; sll/sra 21/10/22 in func_8000B380), interpolated (delta / tick span).

Vertex run list at inner+[0x14] (directly after the vertex array):
{u16 A animated vertices; u16 B copies; B x u16 source vertex index}, repeated until A == 0.
func_8000B380 writes A interpolated vertices, then copies B already-written output vertices
(by index) from the list. In all 127 animated models, the sum of A+B equals the model's vertex
count.

Frame data layout: per animated vertex (N = sum A), K consecutive packed words, where
K = keyframes, or keyframes-1 when flag 0x80 is set (func_8000B380 decrements the count and
uses K*4 as the vertex stride). Then zero padding (0 or 4 bytes).

* This holds for 443 of 445 streams. Those 443 are parsed into per-vertex lists of (x,y,z)
  keys and rebuilt by packing them back.
* Records 132 (stream 0) and 243 (stream 0) do not fit: 764 bytes short, and 20 bytes over
  (non-zero). For these two the frame words are kept as a raw field.
* Every stream is rebuilt exactly and re-encoded.

Hierarchy: no model has a skeleton. Across all 376 lists the walk finds no G_MTX, G_POPMTX,
G_MOVEMEM or sub-list calls. Each model has exactly one mesh entry. inner+0x10 and +0x1C are 0
in ROM (runtime slots). The models are single-mesh and vertex-animated.
(inner+0x24 is non-zero exactly in the 127 animated models; its meaning is not resolved.)

Mesh entry at inner+0x48 (one per model; entry 0 always points to the list at 0x60):
{u32 dl offset, u32 mesh end (== inner+0x08 in 342 models), u16 patch count, u16 b,
u32 patch table offset, u32 x, u32 0}. Patch records are 8 bytes:
{u16 command index from the DL start, u16 kind, s32 param}. The kind always matches the
opcode at that index: 2-4 G_SETCOMBINE, 5/6/8 G_SETOTHERMODE_L, 9/10 G_SETGEOMETRYMODE,
11/12 G_CLEARGEOMETRYMODE, 13-16 G_SETTIMG, 17 G_ENDDL, 0 an empty slot. Record 345's slot 3 is
kind 0. The walker treats kind-0 slots as G_SPNOOP, and with that all 376 lists walk cleanly
(64,731 triangles; sub-lists are followed by the walker).

func_80013358 reads the 0x18 header from ROM, allocates word[2] bytes (func_8001A2B0), copies
the header and rlz-decodes (func_800166F0) into dst+0x18. func_80013570 later decodes a
secondary stream from ROM at record+word[4]+entry.off into dst+word[1]. The secondary area is a
run of rlz streams with zero fill between them; the tool keeps each stream's offset and the
fill bytes. The animation table (above) indexes these streams.

Decoded model (offsets relative to the decoded buffer):

    +04 u16 vertex count n
    +0C u32 vertex array offset (RSP segment 3); +14 == +0C + 16*n (all 376)
    +20 u32 texture block offset (RSP segment 4)
    +48 first mesh entry; its first word is 0x60, where the root display list starts
    +60 F3DEX display list (G_VTX 0x04 with v0*2 / n<<10 packing, G_TRI2 0xB1, G_ENDDL 0xB8)

Vertices are standard 16-byte Vtx records (s16 x,y,z; u16 flag; s16 s,t; u8 r,g,b,a).
They are parsed into tuples and packed again on rebuild. All other bytes are kept as two
explicit raw spans (head before the vertex array, tail after it). Walking the list with
segment 3 = vertices and segment 4 = texture block succeeds for 375 of 376 models. Texture
references come from the walk (G_SETTIMG/SETTILE/LOADTLUT). CI4 and CI8 references with a
resolved tile size and palette are exported. References the walk leaves as RGBA32 loads with no
render tile are not exported.

## imagebank (CI8 tile bank)

    +00 u16 tile count   +02 u16 columns   +04 u16 rows   +06 u16 0
    +08 u16 bits per texel (8 in every record; func_8000D0AC branches on 8 and 0x10)
    +0A u16 width  +0C u16 height (12-byte variant: tile size; read as bytes +0B/+0D by func_8000D0AC)
    +0E u16 pixel area offset   +10 u16 TLUT offset (= 0x18 + count*stride)   +12 u16 0
    +14 u32 TLUT + pixel byte count (pixel offset + this - 0x200 == record size, all 107)
    +18 count entries:
        16-byte (97 records): u16 w,h,x,y; u32 pixel offset; u16 bytes (= w*h); u16 0xCDCD
        12-byte (10 records, header 0x00400008): u16 x,y; u32 pixel offset; u16 bytes; u16 0xCDCD
    TLUT: 256 x RGBA16 (loaded by G_SETTIMG 0xFD10, G_LOADTLUT 255, tmem 0x100 in func_8000D0AC)
    pixels: 8-bit indices (G_SETTIMG 0xFD50 = CI 8b)

func_800328CC/80032914 iterate count entries at +0x18 with stride 0x10 and compute
max(x+w) and max(y+h). Export is one 8-bit palette PNG per tile (PLTE+tRNS from the TLUT);
`png_read_indexed` converts back to indices plus RGBA16 TLUT, and the bytes were compared.

## gridmap (records 226..231)

func_8000DACC loads record 0xE2 + 3*a + b, iterates s16 count at +4 over blocks of 0x210
bytes starting at +0x2014 (s16 x,y,w,h), and stores max(x+w)/max(y+h) into +0/+2.
Layout: 0x14 header (10 u16), 64x64 u16 cells, 8 blocks of 0x210 bytes
(0x14 + 0x2000 + 8*0x210 = 0x3094 = record size).

## pairtable = camera path (138, 145, 162, 175, 185, 195, 207, 218, 421..426)

    +00 u32 eye key count  +04 u32 target key count  +08 u32 eye table offset (0x10)
    +0C u32 target table offset (0x10 + 8n); entries s16 x, y, z, pad

func_8001E204(record) loads the record and calls func_8000FAE8, which turns +08/+0C into
pointers. For record 0xB9 it subtracts 0x1C from +00 and clamps +04 to +00. Per frame,
func_8001E2B0 compares its counter with +00 and calls func_8000F2B8(rec, frame, &eye, &at).
That function clamps the frame to count-1, reads the +08 table (eye) and the +0C table (target),
and passes both to func_800461E0 as eye and look-at points with up = (0, k, 0). Callers include
0x80030A14 (record 0x8A).

## libmus_song (504..528)

Header u32 0x215, u32 n, u32 m, then offsets 0x38 / 0x38+4n / 0x38+8n (holds for all 25).
The format is parsed by tools/audio_songs.py (audio area). res2 imports it when present.
Otherwise it rebuilds these payloads verbatim and marks them unproven.

## font (0, 1)

func_8001F588(n) is called with n = 0 or 1. It loads record n through func_80013238, caches it
per font, and writes n into +04 (0 in ROM). The draw path reads +0x0C (glyph height:
15 / 13) and +0x22 (row width, used as texels*2), and loads the image at +0x850
(G_SETTIMG 16b, LOADBLOCK). Layout:

    +00 u32 record size - 0x200   +04 u32 runtime slot   +08..+2C u32 metrics
    +30 16 x RGBA16 TLUT          +50 256 x 8-byte glyph entries {4 x s8, u16 offset, u16}
    +850 4-bit glyph pixels to the end

Glyph image (strip loop at 0x8001F640..0x8001F808, glyph draw at 0x8001F8A0):

* 8 strips at +0x850 + k*0x800. Each strip is (+0x22) bytes per row, i.e. 2*(+0x22) 4-bit
  texels wide (272 / 304), and +0x0C rows high (15 / 13). Fill to 0x800 is kept explicitly.
  The 8 strips end exactly at the record end.
* Glyph entry +0x50+8*ch: u8 advance (plus font +0x14 spacing), u8, u8 y offset (subtracted
  from +0x12), u8, u16 S = texel column inside the strip (G_RDPHALF_1 s<<21), s16 strip index
  (-1 = no glyph).
* Both fonts have 94 glyphs (printable ASCII); an ASCII dump of strip 2 shows 'A', 'B', 'C'.

Export: 16 strip PNGs and 188 glyph crops (width = advance, full strip height), as palette
PNGs with the 16 x RGBA16 entries at +0x30. That this is the TLUT is inferred from the grey
ramp; the TLUT load itself was not traced. Strip PNGs decode back to the exact 4-bit bytes,
and each glyph crop matches its strip region.

## Inventory

inventory/assets.tsv lists all records. textures.tsv has rows for tile-bank tiles and model CI
textures, 16 font strips and 188 font glyphs (2207 rows, all exact). models.tsv has one row per mdlh record: walk, triangles, animation
table, patch slots. animations.tsv has 445 rows, one per proven keyframe animation stream (443 split per vertex).
scripts.tsv contains only a header (no scripts proven; the libmus songs are in the audio
inventory).
