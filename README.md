# Rugrats: Scavenger Hunt (Nintendo 64, USA)

[![Progress](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?mode=shield&label=Code&measure=matched_code_percent)](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
[![Functions](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?mode=shield&label=Functions&measure=matched_functions)](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
[![Data](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?mode=shield&label=Data&measure=matched_data_percent)](https://bytematch.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
[![decmp.dev](https://api.decmp.dev/badge/TheGh0stShip/Rugrats-Scavenger-Hunt.svg?measure=matched_code_percent&label=decmp.dev)](https://decmp.dev/TheGh0stShip/Rugrats-Scavenger-Hunt)
[![Report](https://github.com/TheGh0stShip/Rugrats-Scavenger-Hunt/actions/workflows/progress.yml/badge.svg)](https://github.com/TheGh0stShip/Rugrats-Scavenger-Hunt/actions/workflows/progress.yml)

This repository contains a complete, byte-exact source reconstruction of the
USA retail release of *Rugrats: Scavenger Hunt*. It includes matching C,
reviewed assembly, symbolic RSP source, initialized-data descriptions, asset
parsers, and whole-ROM reconstruction checks.

Executable-byte coverage is 100%, but matching-C conversion is not: three
ordinary game functions (1,048 bytes) still use reviewed assembly fallbacks.
They are listed in [`docs/ASSEMBLY_AUDIT.md`](docs/ASSEMBLY_AUDIT.md). The
remaining assembly consists of low-level runtime, compiler-helper, exception,
codec, SDK, and debugger code. This distinction is intentional so byte-exact
reconstruction is never presented as 100% C decompilation.

No ROM, extracted media, or proprietary compiler is included. You must provide
your own legally obtained copy of the game and the historical compiler files
needed for code verification.

## Completion status

| Area | Verified coverage |
|---|---:|
| CPU functions | 1,270 / 1,270 |
| CPU-function bytes | 314,304 / 314,304 |
| Other executable material | 9,448 / 9,448 |
| All executable bytes | 323,752 / 323,752 |
| Initialized data | 32,216 / 32,216 |
| Non-code ROM round trip | 16,453,464 / 16,453,464 |
| Physical ROM inventory | 16,777,216 / 16,777,216 |

The assembly manifest currently contains 30 functions (8,644 bytes). Of
those, 3 functions (1,048 bytes) are ordinary game logic still awaiting
matching C. The other 27 functions are audited low-level or source-assembly
units.

The non-function executable material consists of RSP microcode, embedded
read-only tables, and alignment. It is deliberately tracked separately instead
of being presented as CPU functions.

## Requirements

- Linux or WSL
- Python 3.11 or newer
- GNU Make
- `rabbitizer` (installed from `requirements.txt`)
- The historical N64 compiler toolchains described below
- `armips` for rebuilding the symbolic RSP sources

All project-specific analysis, matching, normalization, asset, and verification
tools are stored in this repository. The matching package is vendored under
`tools/n64dk/`; no sibling source checkout is required.

## Setup

Clone the repository and install its Python dependency:

```sh
git clone https://github.com/TheGh0stShip/Rugrats-Scavenger-Hunt.git
cd Rugrats-Scavenger-Hunt
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements.txt
```

Place the USA ROM at `baseroms/us/baserom.z64`. The required SHA-1 is:

```text
fb2a62f1625630d6f0beb5fd00a32e12155d50e8
```

Verify it before doing anything else:

```sh
make verify-rom PYTHON=.venv/bin/python
```

## Historical compiler layout

The original compilers cannot be redistributed. Put your legally obtained
toolchain files in the ignored `.toolchain/` directory:

```text
.toolchain/
├── kmc-gcc-2.7.2/
│   ├── gcc
│   ├── cc1
│   └── as
├── sn-gcc-2.8/
│   ├── wibo
│   ├── cc1n64.exe
│   ├── asn64.exe
│   └── psyq-obj-parser
├── ido5.3/
│   └── cc
├── ido7.1/
│   └── cc
└── mips-binutils/
    └── usr/bin/mips-linux-gnu-as
```

IDO 7.1 is supported by the tooling but is not currently required by a
matching manifest. A compatible MIPS assembler on `PATH` may be used instead of
the `mips-binutils` directory. To keep toolchains elsewhere, pass
`N64DK_TOOLCHAIN=/absolute/path` to Make.

## Verify the decompilation

Run the primary verification suite:

```sh
make check PYTHON=.venv/bin/python
```

This verifies the ROM, rebuilds every matching CPU unit with its recorded
compiler pipeline, checks executable tables and alignment, regenerates the
progress report, and runs the repository tests.

Verify the symbolic RSP programs separately by providing `armips`:

```sh
make verify-rsp-microcode \
  PYTHON=.venv/bin/python \
  ARMIPS=/absolute/path/to/armips
```

Run the complete data and asset validation:

```sh
make data-tests PYTHON=.venv/bin/python
make data-roundtrip PYTHON=.venv/bin/python
```

`data-roundtrip` rebuilds all non-code ROM bytes from parsed representations and
compares them with the retail image. It does not write a distributable ROM.

## Useful targets

| Command | Purpose |
|---|---|
| `make verify-rom` | Check ROM size and SHA-1 |
| `make verify-matching` | Rebuild and compare all CPU-function manifests |
| `make verify-assembly-policy` | Reject unreviewed additions to game assembly |
| `make verify-executable-inventory` | Check every non-function executable interval |
| `make verify-executable-data` | Rebuild executable tables and alignment from C |
| `make verify-rsp-microcode` | Assemble and compare symbolic RSP programs |
| `make data-tests` | Run format, data, audio, BSS, and asset tests |
| `make data-roundtrip` | Rebuild and compare every non-code ROM byte |
| `make report` | Generate `build/report.json` for progress trackers |
| `make deep-rom-audit` | Regenerate the ROM map and consolidated audit |

Variables such as `ROM`, `PYTHON`, `N64DK_TOOLCHAIN`, and `ARMIPS` can be
overridden on the Make command line.

## Repository layout

| Path | Contents |
|---|---|
| `src/game/` | Game and runtime C source |
| `src/runtime/`, `src/libultra/`, `src/libmus/` | Runtime and library source |
| `src/data/` | Executable-section data represented in C |
| `rsp/` | Symbolic RSP boot and audio microcode source |
| `config/us/` | Matching manifests and progress metadata |
| `analysis/` | Function catalogue, address map, ROM ledger, and audit reports |
| `assets/` | Asset/data parsers, source metadata, and round-trip tests |
| `tools/` | All project verification and matching tools |

The decoded inventory includes 2,207 textures, 429 palettes, 376 models, 445
vertex-animation streams, 178 audio samples, 25 songs, 112 sound effects, 380
speech clips, 14 camera paths, 6 grid maps, 2 fonts, and 107 image tile banks.
Extracted copyrighted payloads remain local and are ignored by Git.

## Technical details

- Target: big-endian MIPS III / VR4300 using the 32-bit o32 ABI
- CIC: 6102
- Entry point: `0x80000400`
- Program ROM range: `0x1000..0x57E80`
- Program VRAM range: `0x80000400..0x80057280`
- BSS: `0x80057280..0x800C7E30`
- Confirmed compilers: KMC GCC 2.7.2, SN Systems GCC 2.8, and IDO 5.3

See [`analysis/deep_rom_audit.md`](analysis/deep_rom_audit.md) for the complete
coverage audit and [`analysis/rom_map.md`](analysis/rom_map.md) for the physical
ROM map.

## Legal

This project does not provide the game ROM, extracted copyrighted media, or
proprietary compiler binaries. The source and tooling are licensed under the
terms in [`COPYING`](COPYING).
