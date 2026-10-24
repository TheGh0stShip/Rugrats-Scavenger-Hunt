# Rugrats: Scavenger Hunt (USA) decompilation

[![Progress](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?mode=shield&label=Code&measure=matched_code_percent)](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
[![Functions](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?mode=shield&label=Functions&measure=matched_functions)](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
[![Data](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?mode=shield&label=Data&measure=matched_data_percent)](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
[![Assets](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?mode=shield&label=Assets&measure=matched_data_percent&category=asset_roundtrip)](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
[![Inventory](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?mode=shield&label=Inventory&measure=matched_data_percent&category=rom_inventory)](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
[![decmp.dev](https://api.decmp.dev/badge/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?measure=matched_code_percent&label=decmp.dev)](https://decmp.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
[![Report](https://github.com/TheGh0stShip/Rugrats-Scavenger-Hunt/actions/workflows/progress.yml/badge.svg)](https://github.com/TheGh0stShip/Rugrats-Scavenger-Hunt/actions/workflows/progress.yml)

An in-progress, byte-matching C decompilation of the Nintendo 64 game. You must
provide your own legally obtained USA retail ROM.

## Current baseline

- ROM identity: `fb2a62f1625630d6f0beb5fd00a32e12155d50e8`
- CIC 6102; header entry point `0x80000400`
- Initial program: ROM `0x1000..0x57E80`, VRAM `0x80000400..0x80057280`
- BSS: `0x80057280..0x800C7E30`
- Executable sections: 323,752 bytes; initialized data: 32,216 bytes
- 1,273 conservative function candidates; 268 SDK functions fingerprinted
- 1,017 initialized-data objects identified (including 65 jump tables and 153 strings)
- Two resource archives: 910 nonempty indexed resources in 911 records
- Audio: 178 indexed samples across two sample and pointer-bank pairs
- Asset signatures: 380 `MORT` records and 376 `mdlh` model-header records
- Compilers confirmed in game code: KMC GCC 2.7.2, SN Systems GCC 2.8, and IDO 5.3
- Byte-matched source: 1,229 / 1,273 functions; 286,104 / 323,752 code bytes (88.37%)
- Matching initialized data: 32,216 / 32,216 bytes (100%)
- Asset/resource round-trip: 15,686,080 / 15,686,080 bytes (100%)
- Published extracted asset payloads: 0 bytes; no copyrighted payloads are committed
- All non-code ROM bytes: 16,453,464 / 16,453,464 bytes (100%)
- Physical ROM inventory: 16,777,216 / 16,777,216 bytes (100%)

The reproducible physical ROM map is in `analysis/rom_map.md`; the combined
function, data, archive, audio, and attack-priority analysis is in
`analysis/deep_rom_audit.md`. The map's companion
JSON includes every resource and audio-sample record, while the hosted tracker
reports byte-matched code, byte-matched initialized data, byte-exact resource
round trips, published asset extraction, asset-container inventory, and
physical ROM coverage as separate measures. Parsed formats and verified
rebuilds are not mislabeled as published copyrighted payloads.
`analysis/rom_inventory.tsv` is a gap-free 1,907-row ledger covering every ROM address from `0x000000` through
`0xFFFFFF`. The automated code and initialized-data survey is preserved in
`analysis/survey/`. Function boundaries are candidates until reviewed; a
stripped retail ROM cannot prove original source-level function boundaries.

The decoded inventory includes 2,207 textures, 429 palettes, 376 models, 445
vertex-animation streams, 178 audio samples, 25 songs, 112 sound effects, 380
speech clips, 14 camera paths, 6 grid maps, 2 fonts, and 107 image tile banks.
The source metadata and round-trip tools are under `assets/`; extracted ROM
payloads remain local and untracked.

## Verification

Place the ROM at `baseroms/us/baserom.z64` (a symlink is fine), then run:

```sh
make check
```

By default the Makefile reuses `../n64-decomp-kit` and
`../BattleTanx/.toolchain`. Set `N64DK_ROOT` or `N64DK_TOOLCHAIN` to use other
locations. `make verify-matching` compiles every row in
the matching manifests in `config/us/` and compares relocated output directly
with the retail ROM. Reviewed SN assembler scheduling differences are reproduced
by `tools/normalize_sn_schedule.py`; generated assembly remains temporary and is
never stored in the repository.

`make report` generates the ROM-independent objdiff v2 progress report used by
[ByteMatch](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt). The
root [`.recomp.json`](.recomp.json) supplies the project metadata used by
[recomp.fyi](https://recomp.fyi/). The default-branch workflow publishes the
same report as `NRGE_report` after every push and retains `us_report` for the
established ByteMatch entry. It also sends the verified report directly to
[decmp.dev](https://decmp.dev/TheGh0stShip/Rugrats-Scavenger-Hunt). Its
aggregate measures report program code and initialized program data.
The ROM-wide audit identifies audio banks and both indexed resource archives.
Post-program resources remain excluded from the code/data progress percentage.
Run `make deep-rom-audit` to regenerate the physical map, progress report, and
combined attack map from the verified retail image.

