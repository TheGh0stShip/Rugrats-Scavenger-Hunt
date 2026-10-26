# Assembly source audit

The project reconstructs every executable byte, but byte reconstruction and C
decompilation are different measurements. A function in
`matching_asm_units.tsv` is byte-exact source, but it is not compiled C.

## Current status

The assembly manifest contains 31 functions and 9,040 bytes:

| Classification | Functions | Bytes |
|---|---:|---:|
| Runtime, compiler helper, exception, codec, or SDK assembly | 26 | 6,264 |
| Debugger exception handler | 1 | 1,332 |
| Ordinary game logic still awaiting matching C | 4 | 1,444 |

The first two categories are retained assembly with low-level behavior or
source provenance consistent with assembly. The third category is not counted
as matching C, even though its current assembly source reproduces the ROM.

## Ordinary game functions awaiting matching C

| Function | ROM | Bytes | Purpose |
|---|---:|---:|---|
| `func_80009194` | `0x9D94` | 304 | Find record state |
| `func_80012FB0` | `0x13BB0` | 396 | Initialize resource tables |
| `func_8001F850` | `0x20450` | 420 | Emit scaled textured rectangle |
| `func_8001FA40` | `0x20640` | 324 | Emit textured rectangle |

`tools/verify_assembly_policy.py` fixes this list as an upper bound. The check
fails if an unexpected game assembly unit appears, if one of these ranges
changes, or if the exact debugger handler is removed from its audited class.
When a fallback becomes matching C, remove it from both the assembly manifest
and the allowlist. The check deliberately permits the fallback list to shrink.

## Matching-C conversions

These functions previously used assembly fallbacks and now rebuild exactly
from C:

| Function | Source pipeline | Notes |
|---|---|---|
| `func_8002D404` | SN Systems GCC 2.8, reviewed normalization | Game logic |
| `func_8003F0A8` | KMC GCC 2.7.2 through GNU `as` | n_audio routine; eight trailing alignment bytes |
| `func_80042B70` | KMC GCC 2.7.2 through GNU `as` | libmus scheduler submission |
| `func_8002B434` | SN Systems GCC 2.8, reviewed normalization | Cursor motion setup |
| `func_8002B530` | SN Systems GCC 2.8, reviewed normalization | Menu transition setup |
| `func_80014084` | SN Systems GCC 2.8, reviewed normalization | Display-command patching |
| `func_8001B5FC` | SN Systems GCC 2.8, reviewed normalization | Player-menu setup |
| `func_8001F1C4` | SN Systems GCC 2.8, reviewed normalization | Rotating-stream startup |
| `func_8002E478` | KMC GCC 2.7.2, reviewed normalization | Object timer and flag update |

Reviewed normalizers are fail-closed and function-specific. They reproduce
assembler scheduling and register choices without replacing a function body
with encoded instructions.
