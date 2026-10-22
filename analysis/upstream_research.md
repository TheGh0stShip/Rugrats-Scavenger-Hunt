# Upstream research survey

This survey records fifty targeted searches for public projects and primary
technical references that can accelerate the USA decompilation. Results are
ranked by direct applicability; a shared developer or platform alone is not
treated as proof of shared code.

## Search rounds

| Round | Search focus | Result |
|---:|---|---|
| 1 | Exact game title and decompilation | No independent matching project found |
| 2 | Exact USA title and Nintendo 64 | Metadata only |
| 3 | PAL title, Rugrats: Treasure Hunt | Metadata only |
| 4 | Retail SHA-1 | No public source match |
| 5 | Developer attribution | Realtime Associates confirmed |
| 6 | Realtime Associates Nintendo 64 projects | Four related releases identified |
| 7 | Gex 64 decompilation | Active source reconstruction found |
| 8 | Charlie Blast's Territory decompilation | No public source reconstruction found |
| 9 | Elmo's Letter Adventure decompilation | No public source reconstruction found |
| 10 | Elmo's Number Journey decompilation | No public source reconstruction found |
| 11 | Gex 64 compiler configuration | KMC GCC workflow and MIPS source groups found |
| 12 | Gex 64 runtime library | KMC runtime source group found |
| 13 | Gex 64 operating-system library | Extensive libultra source coverage found |
| 14 | Gex 64 audio library | Standard N64 audio synthesis sources found |
| 15 | Gex 64 music library | Pointer-bank music sources and symbols found |
| 16 | Gex 64 asset loader | zlib object/level loader found; layout differs |
| 17 | Gex 64 model tooling | Level viewer found; format is game-specific |
| 18 | Realtime Associates shared signatures | No public signature database found |
| 19 | `MORT` signature | No relevant public format definition found |
| 20 | `mdlh` signature | No relevant public format definition found |
| 21 | Combined `MORT` and `mdlh` | No shared public implementation found |
| 22 | Resource archive parser | No matching container implementation found |
| 23 | Indexed-resource tables | No matching public table layout found |
| 24 | Pointer-bank audio | Nintendo 64 music-library implementations found |
| 25 | Wave-table audio | Standard SDK structures and extractors found |
| 26 | Sample-bank extraction | Mature extraction examples found |
| 27 | MIDI sequence extraction | General Nintendo 64 tooling found; ROM has no identified MIDI records |
| 28 | F3DEX 1.23 | SDK microcode references found |
| 29 | KMC GCC 2.7.2 | Multiple matching projects and compiler workflows found |
| 30 | SN Systems GCC 2.8 | Few directly reusable Nintendo 64 sources found |
| 31 | IDO 5.3 | Mature static compiler and matching workflows found |
| 32 | Libultra function recovery | `libreultra` is the primary source reference |
| 33 | Scheduler source | Multiple libultra scheduler implementations found |
| 34 | Controller checksum source | Standard libultra implementations found |
| 35 | PI/SI queue source | Standard libultra implementations found |
| 36 | VI manager source | Standard libultra implementations found |
| 37 | Matrix utility source | Standard `gu` implementations found |
| 38 | Audio synthesis source | Standard `audio` implementations found |
| 39 | KMC C library source | Small runtime subset found in Gex 64 |
| 40 | Function fingerprinting | Existing local survey remains the strongest result |
| 41 | Assembly comparison tooling | `asm-differ` remains applicable |
| 42 | MIPS instruction decoding | `rabbitizer` remains applicable |
| 43 | C reconstruction tooling | `m2c` remains useful for first-pass control flow |
| 44 | Source permutation tooling | `decomp-permuter` remains useful after semantic recovery |
| 45 | ROM splitting | `splat` remains useful for eventual rebuild layout |
| 46 | Object comparison reports | `objdiff` report format remains appropriate |
| 47 | Audio sound-font extraction | Harvest Moon 64 provides a mature reference pipeline |
| 48 | Asset extraction architecture | Mario Kart 64 and Animal Forest provide build-time patterns |
| 49 | April 1999 prototype metadata | Prototype existence confirmed; no copyrighted image acquired |
| 50 | RAM annotations and behavioral notes | Public code notes exist and can help name gameplay state |

## Highest-value upstreams

### Gex 64

[Gex64Decomp](https://github.com/MatBourgon/Gex64Decomp) is the closest known
developer-relative project. Inspection of its current tree confirms:

- a KMC compiler driver and separate MIPS I, MIPS III, and KMC-library source
  groups;
- reconstructed libultra, graphics utility, standard audio-synthesis, and
  pointer-bank music code;
- compiler-sensitive examples that document scheduling, register allocation,
  and source-shape requirements;
- a zlib-based object and level loader that does not match this ROM's indexed
  resource layout.

The shared developer and compiler family make it a strong source for naming and
compiler idioms. They do not establish that gameplay structures, archive
records, models, or animations are interchangeable.

### Libultra

[libreultra](https://github.com/n64decomp/libreultra) is the authoritative
public reconstruction for the standard operating-system layer. The local ROM
survey already fingerprints 268 SDK functions, including 224 high-confidence
identifications. Its sources should be tested first for every remaining SDK
range before reconstructing equivalent code manually.

### Audio and asset extraction references

- [Harvest Moon 64](https://github.com/harvestwhisperer/hm64-decomp) provides
  a mature wave-table and sound-font extraction pipeline.
- [Mario Kart 64](https://github.com/n64decomp/mk64) demonstrates a clean
  build-time split between source, textures, course data, sample tables, banks,
  and instrument sets.
- [Animal Forest asset extraction notes](https://github.com/zeldaret/af/blob/main/docs/asset_extraction.md)
  document a reproducible approach for loaded asset segments and display-list
  interpretation.
- [Gex 2 level viewer](https://github.com/MatBourgon/Gex2LevelViewer) is useful
  as a viewer architecture reference, but its input format must not be assumed
  to match this ROM.

### General matching tools

- [splat](https://github.com/ethteck/splat) for linker-layout reconstruction
- [asm-differ](https://github.com/simonlindholm/asm-differ) for instruction diffs
- [m2c](https://github.com/matt-kempster/m2c) for control-flow recovery
- [decomp-permuter](https://github.com/simonlindholm/decomp-permuter) for
  compiler-shape searches
- [rabbitizer](https://github.com/Decompollaborate/rabbitizer) for MIPS decoding
- [objdiff](https://github.com/encounter/objdiff) for reproducible progress data

## Quick-win plan

1. Cross-reference every unmatched function in the SDK range against
   `libreultra` before writing new code.
2. Use Gex 64's KMC runtime sources to identify compiler-library leaves and
   source idioms in the final runtime region.
3. Compare pointer-bank initialization and music-player call shapes with the
   public Gex 64 music sources to name the currently anonymous audio routines.
4. Adapt the Harvest Moon 64 extraction architecture to decode the already
   proven 178 indexed samples without committing extracted media.
5. Keep `MORT`, `mdlh`, and the indexed resource archives game-specific until
   their field semantics are established directly from loaders and consumers.
6. Use prototype differences only if a legally supplied image becomes
   available locally; do not fetch or publish prototype data.
7. Reconcile public RAM code notes with independently proven symbols, treating
   annotations as naming evidence rather than binary-layout proof.

## Rejected assumptions

- Software Creations is not the developer of this title.
- Gex 64 does not establish a shared resource or model format.
- A shared SDK routine does not prove the same compiler flags or library
  revision; every unit still requires byte verification against the USA ROM.
- Asset containers are not decoded asset counts. Existing conservative counts
  remain in force until nested formats are parsed.
