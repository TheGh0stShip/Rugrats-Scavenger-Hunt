# Rugrats: Scavenger Hunt (USA) decompilation

An in-progress, byte-matching C decompilation of the Nintendo 64 game. You must
provide your own legally obtained USA retail ROM.

## Current baseline

- ROM identity: `fb2a62f1625630d6f0beb5fd00a32e12155d50e8`
- CIC 6102; header entry point `0x80000400`
- Initial program: ROM `0x1000..0x57E80`, VRAM `0x80000400..0x80057280`
- BSS: `0x80057280..0x800C7E30`
- 1,273 initial function candidates; 268 SDK functions fingerprinted
- Compilers confirmed in game code: KMC GCC 2.7.2, SN Systems GCC 2.8, and IDO 5.3
- Matching C: 212 functions / 11,524 code bytes
- Matching initialized data: 144 bytes

The automated survey is preserved in `analysis/survey/`. Function boundaries
are candidates until reviewed; its report explicitly records known boundary
ambiguities.

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

