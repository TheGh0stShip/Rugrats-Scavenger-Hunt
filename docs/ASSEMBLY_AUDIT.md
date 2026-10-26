# Assembly source audit

The project reconstructs every executable byte, but byte reconstruction and C
decompilation are different measurements. A function in
`matching_asm_units.tsv` is byte-exact source, but it is not compiled C.

## Current status

The assembly manifest contains 29 functions and 8,340 bytes:

| Classification | Functions | Bytes |
|---|---:|---:|
| Runtime, compiler helper, exception, codec, or SDK assembly | 26 | 6,264 |
| Debugger exception handler | 1 | 1,332 |
| Authentic low-level display-list emitters | 2 | 744 |
| Ordinary game logic still awaiting matching C | 0 | 0 |

Every ordinary game routine now has matching C. The retained assembly has
low-level behavior or source provenance consistent with assembly. It remains
separate from matching C even though its source reproduces the ROM exactly.

## Authentic low-level game assembly

| Function | ROM | Bytes | Purpose |
|---|---:|---:|---|
| `func_8001F850` | `0x20450` | 420 | Emit scaled textured rectangle |
| `func_8001FA40` | `0x20640` | 324 | Emit textured rectangle |

These are paired, leaf-level RDP display-list emitters with parallel control
flow and command-writing sequences. `func_8001F850` reads live inputs from
`$v1` and `$t4` on entry in addition to the o32 argument locations, which
cannot be represented by a standard C function signature. Its unscaled sibling
uses the same hand-allocated register roles and emits the same three raw RDP
commands. This custom ABI and shared construction identify the pair as
hand-authored low-level assembly rather than unresolved ordinary C.

`tools/verify_assembly_policy.py` requires exactly this pair and the audited
debugger exception handler under `src/game/`. It fails if another game assembly
unit appears, if a range changes, or if an audited low-level unit disappears
without a deliberate policy update.

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
| `func_80012FB0` | SN Systems GCC 2.8, reviewed normalization | Resource-table initialization |
| `func_80009194` | SN Systems GCC 2.8, reviewed normalization | Record-state search |

Reviewed normalizers are fail-closed and function-specific. They reproduce
assembler scheduling and register choices without replacing a function body
with encoded instructions.
