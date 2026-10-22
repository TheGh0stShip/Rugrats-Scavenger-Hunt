# ROM physical-layout audit

SHA-1: `fb2a62f1625630d6f0beb5fd00a32e12155d50e8`  
ROM size: 16,777,216 bytes (`0x1000000`)

All ranges are half-open. The table covers every physical ROM byte exactly once.

| Start | End | Bytes | Classification |
|---:|---:|---:|---|
| `0x000000` | `0x001000` | 4,096 | rom_header_and_boot |
| `0x001000` | `0x057E80` | 355,968 | initial_program_image |
| `0x057E80` | `0x21AC00` | 1,846,656 | audio_and_display_data |
| `0x21AC00` | `0x4DB6D0` | 2,886,352 | resource_archive_1 |
| `0x4DB6D0` | `0xF4D840` | 10,953,072 | resource_archive_2 |
| `0xF4D840` | `0xF4D850` | 16 | build_stamp |
| `0xF4D850` | `0x1000000` | 731,056 | zero_padding |

The machine-readable physical ledger contains 1,907 non-overlapping rows and accounts for 16,777,216 of 16,777,216 bytes. It subdivides both archive indices, every resource record, and every alignment gap.

| Ledger classification | Intervals | Bytes |
|---|---:|---:|
| archive_resource | 910 | 13,823,389 |
| audio_sample | 178 | 1,807,650 |
| zero_padding | 1 | 731,056 |
| program_text | 2 | 323,264 |
| audio_pointer_bank_2 | 1 | 20,352 |
| program_data | 2 | 18,972 |
| program_rodata | 1 | 13,732 |
| audio_pointer_bank_1 | 1 | 13,472 |
| archive_index | 2 | 10,948 |
| alignment_padding | 638 | 5,087 |
| audio_bank_metadata_or_padding | 167 | 4,846 |
| boot_code | 1 | 4,032 |
| display_lists_and_loader_block | 1 | 336 |
| rom_header | 1 | 64 |
| build_stamp | 1 | 16 |

## Initial program image

| Start | End | Bytes | Section |
|---:|---:|---:|---|
| `0x001000` | `0x00103C` | 60 | text |
| `0x00103C` | `0x0045E0` | 13,732 | rodata |
| `0x0045E0` | `0x0045F0` | 16 | data |
| `0x0045F0` | `0x053474` | 323,204 | text |
| `0x053474` | `0x057E80` | 18,956 | data |

Executable sections total 323,264 bytes; read-only data totals 13,732 bytes; initialized writable data totals 18,972 bytes. These are physical section sizes, not the smaller sum of function-candidate extents.

## Audio and display data

| Start | End | Bytes | Classification |
|---:|---:|---:|---|
| `0x057E80` | `0x057FD0` | 336 | display_lists_and_loader_block |
| `0x057FD0` | `0x05B470` | 13,472 | audio_pointer_bank_1 |
| `0x05B470` | `0x111310` | 745,120 | audio_sample_bank_1 |
| `0x111310` | `0x215C80` | 1,067,376 | audio_sample_bank_2 |
| `0x215C80` | `0x21AC00` | 20,352 | audio_pointer_bank_2 |

The two pointer banks index 178 audio samples containing 1,807,650 bytes of encoded sample payload in total.

| Bank | Samples | ADPCM | PCM16 | Looped | Sample bytes |
|---|---:|---:|---:|---:|---:|
| audio_bank_1 | 78 | 78 | 0 | 5 | 741,105 |
| audio_bank_2 | 100 | 100 | 0 | 71 | 1,066,545 |

## Resource archives

### resource_archive_1

- Range: `0x21AC00..0x4DB6D0` (2,886,352 bytes)
- Index: 380 records, 4,568 bytes
- Resources: 380 nonempty, 0 empty
- Storage paths: 380 direct, 0 transformed
- Indexed storage: 2,879,376 bytes
- Logical/allocation sizes: 2,879,376 bytes
- Inter-record/alignment padding: 2,408 bytes
- Recognized leading signatures: MORT=380, mdlh=0
- Exact prefix families (records / stored bytes): 4d4f5254=380/2,879,376

### resource_archive_2

- Range: `0x4DB6D0..0xF4D840` (10,953,072 bytes)
- Index: 531 records, 6,380 bytes
- Resources: 530 nonempty, 1 empty
- Storage paths: 382 direct, 148 transformed
- Indexed storage: 10,944,013 bytes
- Logical/allocation sizes: 11,913,050 bytes
- Inter-record/alignment padding: 2,679 bytes
- Recognized leading signatures: MORT=0, mdlh=376
- Exact prefix families (records / stored bytes): 6d646c68=376/10,092,456, 04000100=48/39,840, 04000002=26/55,670, 06000400=17/44,720, 06000300=11/8,430, 06004000=10/562,167, 04000200=8/11,750, 06000200=7/6,284, 00000000=6/74,616, 06000600=4/14,578, 43000128=3/1,011, 43000132=3/2,007, 04000001=2/2,091, 04000005=2/6,154, 04000046=2/13,512, 04000003=1/1,215, 04000004=1/1,906, 04000300=1/2,788, 06000700=1/2,450, 430001de=1/368

## Important limits

The archive indices establish exact physical boundaries and counts. Resource semantics (models, textures, animation, and related content) require format-specific parsing; prefixes alone are not used to claim semantic counts.
In particular, an indexed sample is not automatically a music track or sound effect, and an archive record is not automatically one texture, model, or animation. The audit reports those container-level counts separately until their internal formats are decoded.
