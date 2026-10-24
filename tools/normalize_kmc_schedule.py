#!/usr/bin/env python3
"""Reproduce reviewed KMC assembler choices for matching C units."""

from __future__ import annotations

import argparse
import re
from pathlib import Path


TRANSFER = re.compile(
    r"^(?:b(?:al|eq|ne|gez|gezal|gtz|lez|ltz|ltzal|c1[ft]|c1fl|c1tl|eql|nel|gezl|gtzl|lezl|ltzl)?|"
    r"j|jal|jr|jalr)\s+"
)
LONG_SHIFT_BRANCH = re.compile(r"^(\s*)b(\s+)3f\b")


def builtin(text: str) -> str:
    out: list[str] = []
    reorder = True
    for line in text.splitlines(keepends=True):
        body = line.split("#", 1)[0].strip()
        if body == ".set\tnoreorder" or re.fullmatch(r"\.set\s+noreorder", body):
            reorder = False
            out.append(line)
            continue
        if re.fullmatch(r"\.set\s+reorder", body):
            reorder = True
            continue
        if reorder and LONG_SHIFT_BRANCH.match(line):
            line = LONG_SHIFT_BRANCH.sub(r"\1bgez\2$0,3f", line, count=1)
            body = line.split("#", 1)[0].strip()
        out.append(line)
        if reorder and TRANSFER.match(body):
            out.append("\tnop\n")
    return "\t.set\tnoreorder\n" + "".join(out)


def replace_once(text: str, old: str, new: str, description: str) -> str:
    if text.count(old) != 1:
        raise ValueError(f"{description} not found uniquely")
    return text.replace(old, new, 1)


def normalize_state_copy(text: str) -> str:
    text = replace_once(
        text,
        "\tla\t$7,D_80052F4C\n",
        "\tlui\t$2,0x8005\n\taddiu\t$7,$2,0x2F4C\n",
        "state-copy destination address",
    )
    return replace_once(
        text,
        "\tla\t$6,D_80052F18\n",
        "\tlui\t$2,0x8005\n\taddiu\t$6,$2,0x2F18\n",
        "state-copy source address",
    )


def normalize_guarded_callback(text: str) -> str:
    return replace_once(
        text,
        "\tlh\t$6,D_800535B2\n\tmove\t$4,$2\n",
        "\tmove\t$4,$2\n\tlui\t$2,0x8005\n\tlh\t$6,0x35B2($2)\n",
        "guarded-callback argument scheduling",
    )


def normalize_divisible_state(text: str) -> str:
    return replace_once(
        text,
        "\tlw\t$4,324($16)\n\tli\t$17,0x55550000\t\t# 1431633920\n"
        "\tori\t$17,$17,0x5556\n",
        "\tlui\t$17,0x5555\n\tlw\t$4,324($16)\n\tori\t$17,$17,0x5556\n",
        "divide-by-three constant scheduling",
    )


def normalize_object_setup(text: str) -> str:
    text = replace_once(
        text,
        "\tsw\t$6,D_8006FBA0\n",
        "\tlui\t$2,0x8007\n\tsw\t$6,-0x460($2)\n",
        "object-setup global store",
    )
    return replace_once(
        text,
        "\tlw\t$3,D_80099B30\n\tli\t$4,0x00000009\t\t# 9\n",
        "\tli\t$4,0x00000009\t\t# 9\n\tlw\t$3,D_80099B30\n",
        "object-setup call argument scheduling",
    )


def normalize_random_flag(text: str) -> str:
    text = replace_once(
        text,
        "\tsra\t$4,$2,31\n\tmfhi\t$5\n\t#nop\n\t#nop\n"
        "\tsubu\t$4,$5,$4\n\tsll\t$3,$4,1\n\taddu\t$3,$3,$4\n"
        "\tsubu\t$3,$2,$3\n",
        "\tsra\t$3,$2,31\n\tmfhi\t$5\n\t#nop\n\t#nop\n"
        "\tsubu\t$4,$5,$3\n\tsll\t$3,$4,1\n\taddu\t$3,$3,$4\n"
        "\tsubu\t$4,$2,$3\n",
        "random-flag remainder registers",
    )
    text = text.replace("\tbeq\t$3,$2,.L4\n", "\tbeq\t$4,$2,.L4\n", 1)
    text = text.replace("\tslt\t$2,$3,2\n", "\tslt\t$2,$4,2\n", 1)
    text = text.replace("\tbeq\t$3,$0,.L3\n", "\tbeq\t$4,$0,.L3\n", 1)
    return text.replace("\tbeq\t$3,$2,.L5\n", "\tbeq\t$4,$2,.L5\n", 1)


def normalize_state_copy_init(text: str) -> str:
    text = replace_once(text, (
        "\tjal\tfunc_800196A4\n\tli\t$2,0x00000008\t\t# 8\n"
        "\tla\t$7,D_80052F4C\n\tla\t$6,D_80052F18\n"
        "\taddu\t$8,$6,48\n\tsw\t$2,D_80059D40\n"
    ), (
        "\tjal\tfunc_800196A4\n\tlui\t$3,0x8006\n"
        "\tli\t$2,0x00000008\t\t# 8\n\tsw\t$2,-0x62c0($3)\n"
        "\tlui\t$2,0x8005\n\taddiu\t$7,$2,0x2f4c\n"
        "\tlui\t$2,0x8005\n\taddiu\t$6,$2,0x2f18\n"
        "\taddu\t$8,$6,48\n"
    ), "state-copy initialization scheduling")
    return text


def normalize_slot_store(text: str) -> str:
    return replace_once(
        text, "\tlw\t$3,D_8006D688\n",
        "\tlui\t$2,0x8007\n\tlw\t$3,-0x2978($2)\n",
        "slot-store global address register",
    )


def normalize_object_timer(text: str) -> str:
    replacements = (
        ("\t.frame\t$sp,32,$31", "\t.frame\t$sp,24,$31"),
        ("\tsubu\t$sp,$sp,32\n", "\tsubu\t$sp,$sp,24\n"),
        ("\tsw\t$16,24($sp)\n", "\tsw\t$16,16($sp)\n"),
        ("\tsw\t$31,28($sp)\n", "\tsw\t$31,20($sp)\n"),
        ("\tlw\t$31,28($sp)\n", "\tlw\t$31,20($sp)\n"),
        ("\tlw\t$16,24($sp)\n", "\tlw\t$16,16($sp)\n"),
        ("\taddu\t$sp,$sp,32\n", "\taddu\t$sp,$sp,24\n"),
    )
    for old, new in replacements:
        text = replace_once(text, old, new, "object-timer frame layout")
    text = replace_once(text, (
        "\tlh\t$2,404($16)\n\t#nop\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$2,$0,.L2\n\tmove\t$3,$2\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\taddu\t$2,$3,-1\n"
    ), (
        "\tlh\t$2,404($16)\n\tlhu\t$3,404($16)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$2,$0,.L2\n\taddu\t$2,$3,-1\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
    ), "object-timer unsigned decrement")
    return replace_once(
        text, "\tlh\t$4,D_800C3E44\n",
        "\t.word\t0x3c02800c\n\t.word\t0x84443e44\n",
        "object-timer comparison address expansion",
    )


def normalize_entry_copy_selection(text: str) -> str:
    return replace_once(text, (
        "\tmove\t$9,$4\n\tla\t$6,D_8005368C\n\tmove\t$8,$0\n.L5:\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$8,$9,.L6\n\taddu\t$7,$6,24\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\taddu\t$7,$6,12\n"
    ), (
        "\tmove\t$9,$4\n\tmove\t$8,$0\n"
        "\tlui\t$2,0x8005\n\taddiu\t$6,$2,0x368c\n.L5:\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$8,$9,.L6\n\taddu\t$7,$6,12\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\taddu\t$7,$6,24\n"
    ), "entry-copy selection layout")


def normalize_grid_direction_probe(text: str) -> str:
    text = replace_once(
        text, "\tsw\t$21,36($sp)\n\tla\t$21,.L11\n",
        "\tlui\t$2,0x8000\n\tsw\t$21,36($sp)\n"
        "\taddiu\t$21,$2,0x1f10\n",
        "grid-direction jump-table address",
    )
    return replace_once(text, (
        "\tlw\t$2,D_800581F0\n\tandi\t$3,$18,0x00ff\n"
    ), (
        "\tlui\t$2,0x8006\n\tandi\t$3,$18,0x00ff\n"
        "\tlw\t$2,-0x7e10($2)\n"
    ), "grid-direction table-load scheduling")


def normalize_known_identifier_test(text: str) -> str:
    text = replace_once(
        text, "\tlw\t$3,D_80052F18\n",
        "\t.word\t0x3c028005\n\t.word\t0x8c432f18\n",
        "known-identifier state address expansion",
    )
    text = replace_once(text, (
        ".L2:\n\tlw\t$2,D_80053414\n\t#nop\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeql\t$4,$2,.L3\n\tli\t$5,0x00000001\t\t# 1\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$2,D_80053414+4\n\t#nop\n"
    ), (
        ".L2:\n\tlui\t$2,0x8005\n\tlw\t$3,0x3414($2)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$4,$3,.L3\n\taddiu\t$2,$2,0x3414\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$2,4($2)\n"
    ), "known-identifier array access")
    return replace_once(
        text, "\tli\t$5,0x00000001\t\t# 1\n.L3:\n",
        ".L3:\n\tli\t$5,0x00000001\t\t# 1\n",
        "known-identifier shared true label",
    )


def normalize_counted_byte_update(text: str) -> str:
    text = replace_once(text, (
        "\tlbu\t$2,0($5)\n\tlbu\t$3,2($5)\n\tlbu\t$4,3($5)\n"
        "\taddu\t$3,$3,-1\n\taddu\t$2,$2,$4\n"
    ), (
        "\tlbu\t$2,0($5)\n\tlbu\t$4,3($5)\n\tlbu\t$3,2($5)\n"
        "\taddu\t$2,$2,$4\n\taddu\t$3,$3,-1\n"
    ), "counted-byte load and arithmetic scheduling")
    return replace_once(text, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$3,$0,.L3\n\tsb\t$2,0($5)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlbu\t$3,1($5)\n\tli\t$2,0x00000002\t\t# 2\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t.L4\n\tsb\t$3,0($5)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        ".L3:\n\tli\t$2,0x00000001\t\t# 1\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$3,$0,.L3\n\tsb\t$2,0($5)\n"
        "\tj\t.L4\n\tli\t$2,0x00000001\t\t# 1\n"
        ".L3:\n\tlbu\t$3,1($5)\n\tli\t$2,0x00000002\t\t# 2\n"
        "\tsb\t$3,0($5)\n"
        "\t.set\tmacro\n\t.set\treorder\n"
    ), "counted-byte shared return flow")


def normalize(text: str, unit: str) -> str:
    if unit == "func_800196E8":
        text = normalize_state_copy(text)
    elif unit == "func_8001D0F0":
        text = normalize_guarded_callback(text)
    elif unit == "func_80022760":
        text = normalize_divisible_state(text)
    elif unit == "func_80024D14":
        text = normalize_object_setup(text)
    elif unit == "func_80023544":
        text = normalize_random_flag(text)
    elif unit == "func_80018FFC":
        text = normalize_state_copy_init(text)
    elif unit == "func_8001F9F4":
        text = normalize_slot_store(text)
    elif unit == "func_800231B4":
        text = normalize_object_timer(text)
    elif unit == "func_8001AB7C":
        text = normalize_entry_copy_selection(text)
    elif unit == "func_80024328":
        text = normalize_grid_direction_probe(text)
    elif unit == "func_80019740":
        text = normalize_known_identifier_test(text)
    elif unit == "func_80016AA8":
        text = normalize_counted_byte_update(text)
    return builtin(text)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--unit", required=True)
    parser.add_argument("input", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    args.output.write_text(normalize(args.input.read_text(), args.unit))


if __name__ == "__main__":
    main()
