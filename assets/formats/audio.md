# Audio (libmus), ROM 0x57FD0..0x21AC00

Tool: `tools/audio.py` (`roundtrip(rom)`, CLI `python3 tools/audio.py ROM [--inventory] [--wav] [--reencode-step N]`).
Tests: `tests/test_audio.py`. Inventories: `inventory/audio_banks.tsv`, `inventory/audio_samples.tsv`,
`inventory/audio_sequences.tsv`.

## Layout

| ROM range | Size | Content |
|---|---|---|
| 0x057FD0..0x05B470 | 13,472 | PtrTablesV2 pointer bank 1 (78 waves) |
| 0x05B470..0x1105F0 | 741,760 | WaveTables 1: 16-byte magic, 78 VADPCM samples, 639 zero fill bytes |
| 0x1105F0..0x111310 | 3,360 | libmus effects bank (112 effects, 74-entry wave map into bank 1) |
| 0x111310..0x215C80 | 1,067,376 | WaveTables 2: 16-byte magic, 100 VADPCM samples, 815 fill bytes (6 nonzero at 0x215C7A) |
| 0x215C80..0x21AC00 | 20,352 | PtrTablesV2 pointer bank 2 (100 waves) |

The project map treated 0x1105F0..0x111310 as part of sample bank 1; it is not sample data
(no wave record points into it) and the game loads it separately (see Runtime).

## PtrTablesV2 (all offsets relative to bank start, big-endian)

Header 0x30: magic `N64 PtrTablesV2\0`; u32 flags (0); 12-byte wave bank name (zero);
u32 count; u32 base_note table offset (u8 x count); u32 detune table offset (f32 x count);
u32 wave list offset (u32 x count, each the offset of a wave record).

Wave record (ALWaveTable layout, 20 bytes + 4 zero pad in file): u32 base (offset in the
wave table), u32 length, u8 type (0 VADPCM, 1 PCM16), u8 flags, u16 pad, u32 loop offset (0 = none),
u32 book offset. ADPCM loop: u32 start, u32 end, s32 count, s16 state[16] (44 bytes + 4 pad).
Book: s32 order, s32 npredictors, s16 coefs[order*npred*8].

File order observed in both banks: header, then per wave {record, [loop], book}, then wave list,
base notes, 2-byte pad (bank 1), detunes. Every byte is accounted for: inter-record pads are zero.
Base notes are 0 for 149 waves, 6 for 18, 244 for 11; the detune words are mostly 0 and otherwise decode to
implausible floats, so their encoding is unconfirmed (kept as raw 32-bit words; TSV shows the f32 reading). Every wave has its own book (no sharing); all books
are order 2, 4 predictors. Loops: bank 1 has 5, bank 2 has 71. No PCM16 waves.

libmus pointer banks contain only waves; there are no instrument/sound/envelope records here
(libmus keeps those in song/effect data).

## Effects bank

u32 count (112); u32 field_04 (0x5B, meaning unknown); u32 wave_map_count (74); u32 0; u32 0;
u32 wave_map offset (0xC82); then count x {u32 stream offset, u32 value (0..0x6E, often 0x64)};
command streams from 0x398 up to the wave map; u16 wave_map[74] (indices into pointer bank 1,
max 77); zero tail. Each stream is decoded with the libmus command codec (below, velocity mode off:
only the song start routine sets it) up to its 0x80 stop; all 112 end exactly at the next stream
except the last, which leaves one byte (0x70) before the wave map, kept as an undecoded gap byte.
The rebuilder emits the decoded commands and recomputes all offsets. Wave commands (0x81) in the
effects use slots 0..73, i.e. the whole wave map.

## libmus command codec (tools/audio_songs.py: tokenize / emit)

Derived from the game's player, not only from libmus documentation: handler table at ROM 0x56630
(VRAM 0x80055A30, 45 entries for 0x80..0xAC); argument sizes read from each handler's pointer
advances. "var" = 1 byte, or 2 bytes big-endian 15-bit when the first byte has bit 7 set.

| op | name | args | op | name | args |
|---|---|---|---|---|---|
| 80 | stop | - | 97 | wobble | 3 |
| 81 | wave | var | 98 | wobble_off | - |
| 82 | port | 1 | 99 | velocity_on | - |
| 83 | port_off | - | 9A | velocity_off | - |
| 84 | define_envelope | 7 | 9B | velocity | 1 |
| 85 | tempo | 1 | 9C | pan | 1 |
| 86 | cutoff | 2 | 9D | stereo | 2 |
| 87 | endit | 1 | 9E | drums | var (drum table index) |
| 88/89 | vib_up/vib_down | 3 | 9F | drums_off | - |
| 8A | vib_off | - | A0 | print | 1 |
| 8B | fixed_length | var | A1 | goto | u16 ch, u16 vol, u16 pitch (stream-relative) |
| 8C | ignore_length | - | A2 | reverb | 1 |
| 8D | transpose | 1 | A3/A4/A5 | rand note/volume/pan | 2 |
| 8E | ignore_transpose | - | A6 | volume | 1 |
| 8F | distort | 1 | A7 | start_fx | var |
| 90 | envelope | var (envelope index) | A8 | bend_range | 1 |
| 91..94 | env off/on, trigger off/on | - | A9 | sweep | 1 |
| 95 | for | 1 (0xFF = forever) | AA | change_fx | 1 |
| 96 | next | - | AB | marker | 1 + var |
| | | | AC | fixed_length_off | - |

Notes (byte < 0x80, reader at 0x8003CB40): key; if velocity mode on, a velocity byte (bit 7 set:
sticky value, velocity mode off); then a var length unless fixed length is set (an
ignore_length makes the next note read one anyway). Key 0x60 is a rest (the reader skips it), and a
wave table entry 0xFFFF also forces a rest.

Volume and pitch-bend streams (0x8003D6E4 / 0x8003D7A8): value byte; bit 7 set -> value&0x7F
followed by a var duration (+2 ticks); otherwise duration 1.

## libmus songs (resource archive 2 records 504..528; tools/audio_songs.py)

Header 0x38: u32 0x215, u32 channels, u32 wave count, then offsets: channel table, volume table,
pitch table (u32 x channels each, 0 = none), envelope table (7-byte records), drum table (6-byte
records: u16 wave slot, u16 envelope, u8 pan*2, u8 key; indexed by note key), wave table (u16 x
wave count -> pointer bank 2 wave index), master track; u32 flags; 12 reserved bytes.
Song start (0x8003E36C..0x8003E434) sets velocity mode on for the master track and each channel and
records per-channel stream bases used by goto. An empty envelope or drum table shares the next table's
offset (21 songs have no drums).

All channel and master streams decode exactly to their boundary and end with stop (0x80) or a
forever loop's next (0x96); every envelope / wave slot index used is inside its table. The rebuilder
re-emits all tokens and recomputes header, table and goto offsets (no gotos occur). Diagnostic
MIDI files (approximate timing) are under private_extracted/audio/songs/.
Exact: 25 songs, 99,010 bytes. Songs reference 84 of the 100 bank-2 waves (`used_by` column).
`songs_roundtrip(payload_by_index)` returns rows with payload-relative start/end plus `record`.

## Runtime evidence (fulltext.s, function starting 0x8001EA7C)

- func_8001EDEC(0xB0057FD0 -> RAM 0x80061E60, len 0x34A0); that RAM address is stored to config+0x18.
- func_8001EDEC(0xB01105F0 -> 0x800664C8, len 0xD20); stored to config+0x20.
- config+0x1C = 0xB005B470 (cart address of wave table 1).
- func_8001EDEC(0xB0215C80 -> 0x800674D0, len 0x4F80); func_8003C2E4(0x800674D0, 0xB0111310).
- config at 0x80061E08 also gets 24 (+0x04), 0x3D090 (+0x14), 0x80 (+0x28), 0x55F0 = 22000 (+0x2C),
  0x1000, 1, 0x40, 0x500. This matches the libmus musConfig field order
  (channels, ..., ptr, wbk, default_fxbank, fifo, syn_updates, syn_output_rate, ...).
  The field names are an identification by layout, not from symbols.

## Classification (evidence only)

- Bank 1 = effects bank: it is the bank paired with the default fx bank, and 74 of 78 waves are in
  the fx wave map (column `fx_bank_ref`). Waves 0x17, 0x19, 0x1C, 0x46 are not mapped.
- Bank 2 = music bank: 25 records in resource archive 2 begin with libmus song version 0x00000215
  (records 504..528); their wave tables reference ids up to 98, which exist only in bank 2
  (column `archive2_song_refs`, 84 of 100 waves referenced). Those songs lie outside this ROM range and
  are listed in `audio_sequences.tsv` (they live outside 0x57FD0..0x21AC00, in archive 2).
- Speech: none identified here. Archive 1's 380 `MORT` records all carry header bytes 4..5 = record-specific,
  6..7 = 0x3E80 (16000); none has the libmus song header, so they are not libmus songs.
- Sample rate: not stored per wave in libmus; pitch is set by song/effect data. Synth output rate is 22000 Hz.
  `rate` is left `unknown`; diagnostic WAVs are written at 22000 Hz nominal.

## Rebuild and verification (executed byte comparisons)

- Pointer banks re-serialized from parsed structures with pointers recomputed: 13,472 + 20,352 bytes exact.
- Effects bank re-serialized from decoded commands (827 tokens): 3,360 bytes exact.
- Wave tables reassembled from 178 per-sample payloads + preserved fill runs + magic: 1,808,624 bytes exact
  (samples 1,807,650; fill 1,454; headers 32).
- Whole range 0x57FD0..0x21AC00 (1,846,320 bytes) rebuilt and identical; 348 roundtrip rows, contiguous.

## VADPCM re-encoding

Decoding is deterministic (libultra recurrence, order 2). Original PCM masters are not available,
so encoding from masters cannot be reproduced. Test from decoded PCM: every 50th frame of every sample
(4,103 frames) searched over all predictor x scale(0..12) encodings that decode to identical PCM.
The original frame was always among the candidates (0 missing), but 291 frames (7.1%) had more than one
valid encoding, and no simple tie-break (lowest scale/predictor first matched only 192 of 291) picks the
original. Therefore re-encoding is not bit-exact; the rebuilder keeps the original encoded bytes.
Next experiment: compare candidates against the SDK `vadpcm_enc` selection rule (minimum error over
the original, not decoded, signal), which cannot be evaluated without masters.
