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
