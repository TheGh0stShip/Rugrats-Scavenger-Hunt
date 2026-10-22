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

The two pointer banks index 178 audio samples in total.

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

### resource_archive_2

- Range: `0x4DB6D0..0xF4D840` (10,953,072 bytes)
- Index: 531 records, 6,380 bytes
- Resources: 530 nonempty, 1 empty
- Storage paths: 382 direct, 148 transformed
- Indexed storage: 10,944,013 bytes
- Logical/allocation sizes: 11,913,050 bytes
- Inter-record/alignment padding: 2,679 bytes
- Recognized leading signatures: MORT=0, mdlh=376

## Important limits

The archive indices establish exact physical boundaries and counts. Resource semantics (models, textures, animation, and related content) require format-specific parsing; prefixes alone are not used to claim semantic counts.
