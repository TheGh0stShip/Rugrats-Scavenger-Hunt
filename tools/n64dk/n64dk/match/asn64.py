"""ASN64-compatible preprocessing of GCC assembly for modern GNU as.

Rewrites raw GCC ``-S`` output so that a modern MIPS GNU assembler (run with
``-march=vr4300 -mabi=32 -EB -G0 --no-pad-sections``) reproduces the instruction
reordering, macro expansion and hazard nops of the SN Systems ASN64 assembler used for
many retail N64 objects.

Adapted from earlier GPL-3.0 N64 decompilation tooling (modern_asn64.py,
GPL-3.0 projects), which in turn derive from RocketRet/modern-asn64 @ 20d21729 (Unlicense).
The "lane" rules (MEM-SYM-BASE, MEM-BIG-CONST, FPCMP-BC1-NOP, REMU-EXPAND,
MULTU-HILO-HAZARD, MUL-HOIST-SPACING) come from those projects' retail validation and can be
switched off individually through ``disabled``.  Upstream quirks that affect output (for
example the concatenated ``"jr" "blt"`` entry of the branch set) are preserved on purpose.
"""
from __future__ import annotations

import re

BRANCH_MNEMONICS = {
    "bc1f", "bc1fl", "bc1t", "bc1tl",
    "beq", "beql",
    "bgez", "bgezal", "bgezall", "bgezl",
    "bgtz", "bgtzl",
    "blez", "blezl",
    "bltz", "bltzal", "bltzall", "bltzl",
    "bne", "bnel",
    "j", "jal", "jalr", "jr" "blt", "ble", "bgt", "bge",  # upstream quirk kept: "jrblt"
}
COP1_BRANCH_MNEMONICS = {"bc1f", "bc1fl", "bc1t", "bc1tl"}
COMPARE_MNEMONICS = {"c.lt.s", "c.le.s", "c.eq.s", "c.lt.d", "c.le.d", "c.eq.d"}
INT_LOADS = {"lb", "lbu", "lh", "lhu", "lw"}
INT_STORES = {"sb", "sh", "sw"}
SYM_BASE_RE = re.compile(r"^([A-Za-z_.$][\w.$]*(?:[+-]\d+)?)\((\$\w+)\)$")
CONST_BASE_RE = re.compile(r"^(-?\d+)\((\$\w+)\)$")

PRELUDE = (
    ".macro glabel label\n    .global \\label\n    \\label:\n.endm\n"
    ".macro dlabel label\n    .global \\label\n    \\label:\n.endm\n\n"
    ".macro move a, b\n    addu \\a, \\b, $0\n.endm\n\n"
    ".macro b target\n    bgez $0, \\target\n.endm\n"
    "\t.set noreorder\n"
)

GNU_AS_FLAGS = ("-march=vr4300", "-mabi=32", "-EB", "-G0", "--no-pad-sections", "-mips3")


def _tokens(line: str) -> list[str]:
    return [s.strip() for s in line.strip().split("#")[0].strip().split()]


def _expand_memory(identifier: str, operand_text: str, disabled) -> str | None:
    parts = operand_text.split(",", 1)
    if len(parts) != 2:
        return None
    target, address = parts[0].strip(), parts[1].strip()
    m = SYM_BASE_RE.match(address)
    if m and identifier in INT_LOADS and "MEM-SYM-BASE" not in disabled:
        sym, base = m.group(1), m.group(2)
        return (f"\t.set noat\n\tlui $at,%hi({sym})\n\taddu $at,$at,{base}\n"
                f"\t{identifier} {target},%lo({sym})($at)\n\t.set at\n")
    m = CONST_BASE_RE.match(address)
    if m and (identifier in INT_LOADS or identifier in INT_STORES) and "MEM-BIG-CONST" not in disabled:
        offset, base = int(m.group(1)), m.group(2)
        if not (0x8000 <= offset <= 0xFFFF):
            return None
        low = ((offset & 0xFFFF) ^ 0x8000) - 0x8000
        high = ((offset - low) >> 16) & 0xFFFF
        return (f"\t.set noat\n\tlui $at,{high:#x}\n\taddu $at,{base},$at\n"
                f"\t{identifier} {target},{low}($at)\n\t.set at\n")
    return None


def _div_expansion(kind: str, ops: list[str], label: int) -> tuple[str, int]:
    d, a, b = ops[0], ops[1], ops[2]
    if kind in ("div", "rem"):
        l1, l2 = f"BRANCH_LABEL_{label}", f"BRANCH_LABEL_{label + 1}"
        move = "mflo" if kind == "div" else "mfhi"
        return ("\t.set noat\n"
                f"\tdiv $0,{a},{b}\n\tbnez {b},{l1}\n\tnop\n\tbreak 0x7\n{l1}:\n"
                f"\taddiu $1,$0,-1\n\tbne {b},$1,{l2}\n\tlui $1,0x8000\n"
                f"\tbne {a},$1,{l2}\n\tnop\n\tbreak 0x6\n{l2}:\n\t{move} {d}\n\t.set at\n"), 2
    l1 = f"BRANCH_LABEL_{label}"
    move = "mflo" if kind == "divu" else "mfhi"
    return ("\t.set noat\n"
            f"\tdivu $0,{a},{b}\n\tbnez {b},{l1}\n\tnop\n\tbreak 0x7\n{l1}:\n"
            f"\t{move} {d}\n\t.set at\n"), 1


def convert(source: str, disabled: frozenset[str] | set[str] = frozenset()) -> str:
    """Return assembler input for modern GNU as equivalent to ASN64 assembling ``source``."""
    input_lines = source.splitlines(keepends=True)
    out: list[str] = [PRELUDE]
    is_reorder = True
    generated = 0
    mfhilo_delay_count = 0
    mfhilo_delay_location = 0
    prev_mul = prev_prev_mul = False
    comm_symbols: list[tuple[str, str]] = []
    lcomm_symbols: list[tuple[str, str]] = []
    delay_slot = False
    file_count = 0
    prev_instruction_index = 0
    last_file_directive = -1
    local_symbols: set[str] = set()

    for line_index, line in enumerate(input_lines):
        tokens = _tokens(line)
        if line[1:5] == "#nop":
            prev = _tokens(input_lines[prev_instruction_index])
            if prev and prev[0] in COMPARE_MNEMONICS:
                line = "\tnop\n"
        if not tokens:
            out.append(line)
            continue
        identifier = tokens[0]
        new_prev_mul = False
        is_branch = False

        if identifier[0] == ".":
            directive = identifier[1:]
            if directive == "set":
                if tokens[1] == "noreorder":
                    is_reorder = False
                    continue
                if tokens[1] == "reorder":
                    is_reorder = True
                    continue
            elif directive == "local":
                local_symbols.add(tokens[1])
            elif directive == "comm":
                sym, size = [s.strip() for s in tokens[1].split(",")][:2]
                (lcomm_symbols if sym in local_symbols else comm_symbols).append((sym, size))
                line = ""
            elif directive == "lcomm":
                sym, size = [s.strip() for s in tokens[1].split(",")]
                lcomm_symbols.append((sym, size))
                line = ""
            elif directive == "file":
                file_count += 1
                line = f"\t.file\t{file_count + 1} {tokens[2]}\n"
                last_file_directive = len(out)
            elif directive in ("def", "begin", "bend"):
                line = ""
            elif directive == "word":
                if tokens[1][0] == "$":
                    line = f"\t.word\t.{tokens[1][1:]}\n"
        elif identifier[-1] == ":":
            pass
        else:
            if len(tokens) > 1:
                expanded = _expand_memory(identifier, "".join(tokens[1:]), disabled)
                if expanded is not None:
                    line = expanded
            if identifier in COP1_BRANCH_MNEMONICS and "FPCMP-BC1-NOP" not in disabled:
                prev = _tokens(input_lines[prev_instruction_index]) if prev_instruction_index != line_index else []
                if prev and prev[0] in COMPARE_MNEMONICS and not any(
                        between[1:5] == "#nop" for between in input_lines[prev_instruction_index + 1:line_index]):
                    out.append("\tnop\n")
            if identifier in BRANCH_MNEMONICS:
                is_branch = True
                if is_reorder:
                    line += "\tnop\n"
            elif identifier in ("li.s", "li.d"):
                ops = [s.strip() for s in tokens[1].split(",")]
                if ops[0][1] == "f":
                    sym = f"RODATA_SYM_{generated}"
                    single = identifier == "li.s"
                    line = ("\t.section .rodata\n"
                            f"{sym}:\n"
                            f"\t.align {2 if single else 3}\n"
                            f"\t.{'float' if single else 'double'} {ops[1]}\n"
                            "\t.text\n\t.set\tnoat\n"
                            f"\tlui $at, %hi({sym})\n"
                            f"\t{'lwc1' if single else 'ldc1'} {ops[0]}, %lo({sym})($at)\n"
                            "\t.set\tat\n")
                    generated += 1
            elif identifier in "div" or identifier in "divu" or identifier == "rem" or (
                    identifier == "remu" and "REMU-EXPAND" not in disabled):
                kind = "div" if identifier in "div" else "divu" if identifier in "divu" else identifier
                while mfhilo_delay_count > 0:
                    out.append("\tnop\n")
                    mfhilo_delay_count -= 1
                ops = [s.strip() for s in tokens[1].split(",")]
                if ops[0] != "$0":
                    line, used = _div_expansion(kind, ops, generated)
                    generated += used
                    mfhilo_delay_count = 3
                    mfhilo_delay_location = len(out) + 1
            elif identifier in ("mflo", "mfhi"):
                mfhilo_delay_count = 3
                mfhilo_delay_location = len(out) + 1
            elif identifier == "mult":
                if delay_slot and not is_reorder and mfhilo_delay_count <= 0:
                    out.insert(len(out) - 1, line)
                    line = "\tnop\n"
                while mfhilo_delay_count > 0:
                    if delay_slot:
                        out.insert(mfhilo_delay_location, "\tnop\n")
                    else:
                        out.append("\tnop\n")
                    mfhilo_delay_count -= 1
                if prev_mul:
                    line = "\tnop\n" + line
                new_prev_mul = True
            elif identifier == "multu" and "MULTU-HILO-HAZARD" not in disabled:
                while mfhilo_delay_count > 0:
                    if delay_slot:
                        out.insert(mfhilo_delay_location, "\tnop\n")
                    else:
                        out.append("\tnop\n")
                    mfhilo_delay_count -= 1
            elif identifier in ("mul.s", "mul.d"):
                if delay_slot and not is_reorder and mfhilo_delay_count <= 0:
                    if prev_prev_mul and "MUL-HOIST-SPACING" not in disabled:
                        out.insert(len(out) - 1, "\tnop\n")
                    out.insert(len(out) - 1, line)
                    line = "\tnop\n"
                if prev_mul:
                    line = "\tnop\n" + line
                new_prev_mul = True

            if mfhilo_delay_count > 0:
                mfhilo_delay_count -= 1
            prev_prev_mul = prev_mul if is_branch else False
            prev_mul = new_prev_mul
            delay_slot = is_branch
            prev_instruction_index = line_index
        out.append(line)

    if comm_symbols or lcomm_symbols:
        out.append("\t.section\t.bss\n")
    for sym, size in lcomm_symbols + comm_symbols:
        if int(size) > 4:
            out.append("\t.align 3\n")
        out.append(f"\t.globl {sym}\n{sym}:\n\t.space {size}\n")
    if last_file_directive != -1:
        name = _tokens(out[last_file_directive])[2]
        out[last_file_directive] = ""
        out.insert(0, f"\t.file\t1 {name}\n")
    return "".join(out)
