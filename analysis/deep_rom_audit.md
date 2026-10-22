# Deep whole-ROM audit and attack map

Retail SHA-1: `fb2a62f1625630d6f0beb5fd00a32e12155d50e8`  
ROM bytes: 16,777,216  
Build stamp: `05.04.99 11.13`

This report separates physical coverage, structural decoding, semantic decoding, and byte-matched reconstruction. A bounded archive payload is physically inventoried but is not counted as an extracted or semantically decoded asset.

## Whole-ROM physical coverage

The gap-free ledger has 1,907 non-overlapping intervals covering 16,777,216 / 16,777,216 bytes (100%).

| Physical class | Intervals | Bytes | ROM share |
|---|---:|---:|---:|
| archive_resource | 910 | 13,823,389 | 82.393819% |
| audio_sample | 178 | 1,807,650 | 10.774434% |
| zero_padding | 1 | 731,056 | 4.357433% |
| program_text | 2 | 323,264 | 1.926804% |
| audio_pointer_bank_2 | 1 | 20,352 | 0.121307% |
| program_data | 2 | 18,972 | 0.113082% |
| program_rodata | 1 | 13,732 | 0.081849% |
| audio_pointer_bank_1 | 1 | 13,472 | 0.080299% |
| archive_index | 2 | 10,948 | 0.065255% |
| alignment_padding | 638 | 5,087 | 0.030321% |
| audio_bank_metadata_or_padding | 167 | 4,846 | 0.028884% |
| boot_code | 1 | 4,032 | 0.024033% |
| display_lists_and_loader_block | 1 | 336 | 0.002003% |
| rom_header | 1 | 64 | 0.000381% |
| build_stamp | 1 | 16 | 0.000095% |

## Executable and function coverage

The two executable sections total 323,264 bytes. The function catalogue contains 1,273 functions spanning 314,356 instruction bytes; the remaining 8,908 executable-section bytes are padding, embedded tables, or boundary-sensitive regions and are not invented as functions.

- Exact manifest coverage: 25,268 / 323,264 bytes (7.816521%)
- Fully covered functions: 394 / 1,273 (30.950511%)
- Partially covered catalogue functions: 7
- Catalogue classifications: {'asm': 35, 'c': 873, 'leaf': 259, 'stub': 106}
- Not-fully-covered classifications: {'asm': 35, 'c': 708, 'leaf': 133, 'stub': 3}
- Jump tables: 64 with 990 entries
- Boundary findings requiring review: {'rejected_start': 1, 'in_text_data': 6, 'stray_words': 52, 'cross_branch': 5, 'start_in_delay_slot': 6, 'falls_through': 3, 'branch_to_entry': 2, 'late_start': 2, 'branch_outside': 1}

The survey and physical audit disagree only in terminology: the older survey labels archive payloads unknown because it does not parse the game-specific archive indices. The physical audit proves their byte boundaries; their internal semantics remain undecoded.

## Program data

Initialized writable data totals 32,704 bytes; 240 bytes are presently byte-matched. Read-only data totals 13,732 bytes and includes strings, floating-point constants, jump tables, and embedded control data owned by code units.

## Audio structures

Two pointer banks index 178 non-overlapping samples occupying 1,807,650 encoded bytes. All 178 records identify the standard ADPCM codec; none identify PCM16. This proves sample boundaries and codec class, not whether an individual sample is music, speech, ambience, or an effect.

| Bank | Samples | Looped | Payload bytes |
|---|---:|---:|---:|
| audio_bank_1 | 78 | 5 | 741,105 |
| audio_bank_2 | 100 | 71 | 1,066,545 |

## Resource archives

### resource_archive_1

- 380 index records; 380 nonempty
- 2,879,376 stored payload bytes and 2,408 alignment bytes
- record sizes 472..63,012 bytes; median 6,752.0; p90 11,492
- 1 exact duplicate groups containing 2 records
- signatures: MORT=380, mdlh=0

### resource_archive_2

- 531 index records; 530 nonempty
- 10,944,013 stored payload bytes and 2,679 alignment bytes
- record sizes 116..342,144 bytes; median 3,421.5; p90 58,200
- 19 exact duplicate groups containing 48 records
- signatures: MORT=0, mdlh=376
- 148 transformed records: 776,941 stored -> 1,745,978 logical bytes (ratio 0.445)

## Highest-yield attack order

The archive-2 transform path is now localized. `func_80013238` multiplies the resource index by 12 and interprets each record as ROM offset, logical/allocation size, and stored transform size. A zero transform size takes the direct DMA path. A nonzero value DMA-loads that many bytes into the scratch buffer at `D_80052EC8`, then calls `func_800166F0` with the scratch buffer, allocated output, and decoded-size pointer.

`func_800166F0` is a control-byte copy/back-reference transform. Some archive streams reference output history before bytes emitted by the isolated record, so it must not yet be described as a standalone decompressor. Its caller setup or history semantics must be recovered before extraction results can be accepted.

1. Reconstruct `func_800166F0` and determine its required output history. This transform path covers 148 records and 1,745,978 logical bytes.
2. Trace `mdlh` consumers from archive-2 load sites. The 376 uniform headers cover 10,092,456 stored bytes, making this the largest single format family.
3. Trace the archive-1 `MORT` consumer. All 380 records use that signature and occupy 2,879,376 bytes; do not label them as speech until the consumer proves it.
4. Continue exact matching from known SDK/runtime call-graph anchors and repeated wrapper clusters. This has a better expected byte yield than isolated large gameplay functions.
5. Review the five cross-branch, three fall-through, one outside-branch, and remaining boundary anomalies before treating all 1,273 catalogue entries as final.
6. Decode nested resource formats only after their loader invariants are known. Count textures, models, animations, music, and speech from validated structures rather than prefix or entropy guesses.

## Completion limits

Physical inventory is complete. Byte-matched code, byte-matched initialized data, and semantic asset extraction are not complete and remain separately reported. The audit contains no extracted media and does not turn container coverage into decompilation progress.
