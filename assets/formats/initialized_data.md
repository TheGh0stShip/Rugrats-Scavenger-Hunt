# Initialized program data (src/data, src/asm)

The assignment covers 32,704 ROM bytes (VRAM = ROM + 0x7FFFF400). Checking them showed that 488 of those bytes are
code, so the split is 32,216 data bytes + 488 code bytes:

| ROM | VRAM | bytes | content |
|---|---|---|---|
| 0x103C..0x10F8 | 0x8000043C..0x800004F8 | 188 | code: func_8000043C, func_80000498 (+4-byte nop pad) -> src/asm/ctors_8000043C.s |
| 0x10F8..0x45E0 | 0x800004F8..0x800039E0 | 13,544 | .rodata |
| 0x45E0..0x45F0 | 0x800039E0..0x800039F0 | 16 | .data |
| 0x53474..0x535A0 | 0x80052874..0x800529A0 | 300 | code: tail of func_80052680 (bcopy, size 0x31C per symbol_addrs) + nop pad -> src/asm/bcopy_tail_80052874.s |
| 0x535A0..0x57E80 | 0x800529A0..0x80057280 | 18,656 | .data |

The survey's section list puts .rodata at 0x8000043C and .data at 0x80052874, which is why its total is 32,704.
The two walker functions save registers at 4/8/12($sp) in a 16-byte frame and call through $t0. KMC GCC never
lays out a frame like that, so they are hand-written assembly and are kept as .s files. Assembling them
reproduces the bytes exactly.

## Files
* `src/data/<section>_<VRAM>.c`: one file per section slice. Line 2 gives the placement. Compile with
  `kmc gcc -G0 -O2 -mips2`; `-mips3` widens char-array alignment to 8. Before linking at VRAM, lower the
  section alignment to 4 (`objcopy --set-section-alignment`).
* `src/data/<same>.json`: machine-readable mirror that the materializer reads.
* `src/data/initialized_data.h`: struct typedefs (`Struct_<VRAM>`, libultra `OSViMode`) and an extern for every object.
* `src/data/data_types.h`: typedefs, plus `extern char X[]` for code, BSS and foreign symbols.
* `src/data/symbols.json`: maps names to VRAM, for resolving relocations.

## Object record
Common keys: `vram`, `rom`, `size`, `section`, `name`, `rep`, `ctype`, plus `note` (evidence) and `reason`
(typed-raw only). Values by `rep`:
* `string`: `literal` is a C literal (octal escapes, `\?`); the rest of `size` is zero.
* `f32` / `f64`: decimal literals, used only when they are bit-exact.
* `jtbl`: `{reloc, sym, off}` entries, each meaning function + offset.
* `pointer`: an array of pointers, used only when a traced load of the word is dereferenced or passed in $a0-$a3 to a jal.
* `typed-array`: the element type (u32/s16/u16/s8/u8) comes from the widths and signedness of traced loads and stores.
* `struct`: `stype`, `stride`, `count`, `fields[{off, ctype, name, n, unk?}]`, `values[element][field]`.
  Field offsets and widths come from traced accesses. Strides come from loop pointer increments
  (`addiu r,r,S` on a formed address) and from scaled index registers (sll/addu chains feeding `addu base,idx`).
  A field is `ptr` (`void *`) only when its use is confirmed. `unk_XX` fields hold bytes that were never accessed.
  OSViMode uses the libultra field names (type, comRegs.*, fldRegs[2].*) and is recognised by its layout.
* `ucode-data`: an RSP microcode DMEM image, identified from the OSTask setup code that stores it as `ucode_data`.
* `typed-raw`: hex words that have not been decompiled. `reason` says why.
* `padding`: an unreferenced zero gap.
* Relocations. A u32 word is emitted as a relocation (`{reloc, sym, off}`) only when its pointer use is confirmed
  by `tools/ptrflow.py`: the loaded value was dereferenced, called through, or passed in a0-a3 to a callee that
  dereferences it. Every other word whose value falls in pointer range is emitted as an integer. Counts are in
  `src/data/pointer_stats.json`.
* `tools/ptrflow.py` traces values through registers, $sp slots, branch-join snapshots, jumps into join labels,
  and calls (a0-a3 seeded into callees, up to 4 rounds). Accesses made through a pointer loaded from a data word
  or table field type the object the pointer targets. Indexed accesses through such a pointer make the target an
  element array.
* Reverb parameter lists are recognised by layout: s32 {sectionCount, length, 8 words per section} with
  size == (2 + 8n) words.
* Integration: `src/asm/*.s` is code, not data. It is owned by the code-decompilation effort, kept only as local
  evidence for the 488 bytes, and must not go into the integration patch, which contains C only (`src/data/*.c|h`).

No display lists were found in the initialized data: there is no 0xB8/0xDF end command at any 8-byte position.
The display lists live in the separate 0x57E80 block.

## Verification
`python3 tools/datasrc.py verify|compile|inventory|all`; `roundtrip(rom)` returns rows for both the code and the data objects.
Access tracing lives in `tools/data_access.py`.
