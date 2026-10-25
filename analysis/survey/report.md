# Survey: RUGRATSSCAVENGERHUNT (NRGE)

- **Program**: static image, main segment 0x80000400..0x80057280 (rom); 1270 CPU functions, 268 named from the SDK
- **ROM understood**: 17.5 % by content, plus 2.0 % only referenced by loader tables; 13,512,484 bytes unknown in 3 ranges
- **Next**: ROM 0x749D72..0xF4D840 (0x803ace bytes) is unclassified; 7.0 bits/byte, zlib ratio 0.83: raw media or lightly packed data (audio, textures, animation)
- **Next**: ROM 0x21AC00..0x6FA050 (0x4df450 bytes) is unclassified; 7.5 bits/byte, zlib ratio 0.93: raw media or lightly packed data (audio, textures, animation)
- **Next**: preserve the reviewed text end at 0x800529A0, including its final alignment word

## ROM

- **title**: RUGRATSSCAVENGERHUNT
- **game_code**: NRGE
- **version**: 0
- **sha1**: fb2a62f1625630d6f0beb5fd00a32e12155d50e8
- **size**: 16777216
- **byte_order**: z64
- **cic**: 6102
- **checksum_ok**: True
- **entry_header**: 0x80000400
- **entry**: 0x80000400
- **crc**: 0C02B3C5/9E2511B8

## Program image

- method: static; entry 0x80000400; hand-over 0x800279B0; stack 0x803FFFF0

| vram | end | size | kind | ROM | codec | evidence |
|---|---|---|---|---|---|---|
| 0x80000400 | 0x80057280 | 0x56E80 | rom | 0x1000..0x57E80 | - | IPL3 copy |
| 0x80057280 | 0x800C7E30 | 0x70BB0 | bss | - | - | entry routine clear loop |

Offsets below are ROM offsets.

## Functions

- **text**: 0x80000400..0x8005299C
- **text_size**: 337308
- **data_start**: 0x8005299C
- **image_end**: 0x80057280
- **functions**: 1270
- **kinds**: {'c': 875, 'leaf': 258, 'asm': 32, 'stub': 105}
- **seeds**: {'entry': 1, 'unreferenced': 149, 'call': 1007, 'pointer': 116}
- **jump_tables**: 64
- **jump_table_entries**: 990
- **in_text_data**: ['0x800004F8+0x34E8', '0x800367EC+0x4', '0x800367F0+0x2160', '0x80045888+0x1CC', '0x800485E0+0x14', '0x800488F0+0x10']
- **stray_words**: 52
- **findings**: {'rejected_start': 1, 'in_text_data': 6, 'stray_words': 52, 'cross_branch': 5, 'start_in_delay_slot': 6, 'falls_through': 3, 'branch_to_entry': 2, 'late_start': 2, 'branch_outside': 1}

## Data map

- **image**: 0x80000400..0x80057280 (ROM 0x1000..0x57E80)
- **bss**: 0x80057280..0x800C7E30
- **sections**: ['.text 0x80000400..0x800004F8 (0xF8)', '.rodata 0x800004F8..0x800039E0 (0x34E8)', '.data 0x800039E0..0x800039F0 (0x10)', '.text 0x800039F0..0x800529A0 (0x4EFB0)', '.data 0x800529A0..0x80057280 (0x48E0)']
- **section_bytes**: {'.text': '0x4f0a8', '.rodata': '0x34e8', '.data': '0x48f0'}
- **functions**: 1335
- **xref_sites**: 2640
- **data_addresses**: 1135
- **objects**: 1017
- **object_types**: {'unreferenced': 2, 'jtbl': 65, 'f32': 207, 'f64': 166, 'string': 153, 'words': 409, 'pointers': 15}
- **single_owner_objects**: 833
- **shared_objects**: 182
- **jump_tables**: 65
- **strings**: 153

## SDK

- **functions_named**: 268
- **high_confidence**: 224
- **sdk_version**: ['2.0D/2.0E/2.0F/2.0G/2.0H:libgultra_rom', '2.0I:libgultra_rom', '2.0D/2.0E/2.0F/2.0G/2.0H:libgultra', '2.0I:libgultra']
- **library_compiler**: {'kmc': 217}
- **game_compiler**: kmc
- **game_compiler_share**: {'kmc': 0.83, 'ido': 0.17}
- **microcode**: ['F3DEX 1.23 @0x55A70']
- **audio**: {}
- **libmus**: ['N64 PtrTablesV2', 'N64 WaveTables']

## Audio

- **counts**: {'containers': 3, 'samples': 178, 'midi': 0, 'intervals': 1038}
- **bytes**: {'container': 32842, 'padding': 2453, 'payload': 1807650}

## Graphics

- **family**: f3dex
- **family_from**: microcode F3DEX 1.23 at ROM 0x55A70
- **display_lists**: 3
- **pools**: []
- **blobs**: 0
- **textured_blobs**: 0
- **pool_bytes**: 0
- **standalone_lists**: 3

## Compressed streams and loader ranges

- **streams**: {}
- **streams_with_loader**: 0
- **loader_ranges**: {'records': 55, 'call': 4}
- **dma_wrappers**: ['0x8001EDEC']
- **ledger_bytes**: {'-': 16148880, 'loader_range': 628336}
- **unknown_bytes**: 15413728

## ROM ledger

- complete: **False** (unknown bytes: 13,512,484 in 3 ranges)

| class | bytes |
|---|---|
| unknown | 13,512,484 |
| payload | 1,807,970 |
| padding | 732,312 |
| referenced | 331,544 |
| code | 323,752 |
| container | 32,842 |
| data | 32,216 |
| rom_metadata | 4,096 |

| category | bytes |
|---|---|
| - | 14,244,796 |
| audio | 1,840,492 |
| program | 355,968 |
| loader_range | 331,544 |
| rom_metadata | 4,096 |
| graphics | 320 |

Largest unknown ranges:

- 0x749D72..0xF4D840 (0x803ace)
- 0x21AC00..0x6FA050 (0x4df450)
- 0x215C7A..0x215C80 (0x6)

## Cross-checks

- final function ends at 0x8005299C; text ends after alignment at 0x800529A0

## Next steps

Most bytes (or failures) first.

1. ROM 0x749D72..0xF4D840 (0x803ace bytes) is unclassified; 7.0 bits/byte, zlib ratio 0.83: raw media or lightly packed data (audio, textures, animation)
2. ROM 0x21AC00..0x6FA050 (0x4df450 bytes) is unclassified; 7.5 bits/byte, zlib ratio 0.93: raw media or lightly packed data (audio, textures, animation)
3. preserve the reviewed text end at 0x800529A0
4. review function boundaries flagged in functions.toml: 5 cross_branch, 3 falls_through, 1 branch_outside
5. ROM 0x215C7A..0x215C80 (0x6 bytes) is unclassified; 1.9 bits/byte, zlib ratio 2.33: already compressed (zlib cannot shrink it): a codec n64dk does not know; find its loader

## Timings (s)

program_image 0.0, functions 1.19, datamap 0.54, sdk 4.42, audio 0.34, gfx 0.03, assets 7.33, rsp_in_text 0.03
