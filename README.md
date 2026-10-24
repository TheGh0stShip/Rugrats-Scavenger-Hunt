# Rugrats: Scavenger Hunt (USA) decompilation

[![Progress](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?mode=shield&label=Code&measure=matched_code_percent)](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
[![Functions](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?mode=shield&label=Functions&measure=matched_functions)](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
[![Data](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?mode=shield&label=Data&measure=matched_data_percent)](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
[![Assets](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?mode=shield&label=Assets&measure=matched_data_percent&category=assets_extracted)](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
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
- Executable sections: 323,264 bytes; initialized data: 32,704 bytes
- 1,273 conservative function candidates; 268 SDK functions fingerprinted
- 1,017 initialized-data objects identified (including 65 jump tables and 153 strings)
- Two resource archives: 910 nonempty indexed resources in 911 records
- Audio: 178 indexed samples across two sample and pointer-bank pairs
- Asset signatures: 380 `MORT` records and 376 `mdlh` model-header records
- Compilers confirmed in game code: KMC GCC 2.7.2, SN Systems GCC 2.8, and IDO 5.3
- Matching C: 466 / 1,273 functions; 30,060 / 323,264 code bytes (9.30%)
- Matching initialized data: 240 / 32,704 bytes (0.73%)
- Extracted assets: 0 / 15,686,080 bytes (0%); container inventory: 100%
- Physical ROM inventory: 16,777,216 / 16,777,216 bytes (100%)

The reproducible physical ROM map is in `analysis/rom_map.md`; the combined
function, data, archive, audio, and attack-priority analysis is in
`analysis/deep_rom_audit.md`. The map's companion
JSON includes every resource and audio-sample record, while the hosted tracker
reports byte-matched code, byte-matched initialized data,
asset extraction, asset-container inventory, and physical ROM coverage as
separate measures. Container discovery is not counted as asset extraction.
`analysis/rom_inventory.tsv` is a gap-free 1,907-row ledger covering every ROM address from `0x000000` through
`0xFFFFFF`. The automated code and initialized-data survey is preserved in
`analysis/survey/`. Function boundaries are candidates until reviewed; a
stripped retail ROM cannot prove original source-level function boundaries.

Counts deliberately remain at the strongest level supported by the formats.
For example, the ROM proves 178 indexed audio samples, but not which samples
are music versus effects. Likewise, an archive record can contain multiple
textures, meshes, or animations, so container counts are not presented as
decoded asset counts.

## Verification

Place the ROM at `baseroms/us/baserom.z64` (a symlink is fine), then run:

```sh
make check
```

By default the Makefile reuses `../n64-decomp-kit` and
`../BattleTanx/.toolchain`. Set `N64DK_ROOT` or `N64DK_TOOLCHAIN` to use other
locations. `make verify-matching` compiles every row in
the matching manifests in `config/us/` and compares relocated output directly
with the retail ROM.

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

