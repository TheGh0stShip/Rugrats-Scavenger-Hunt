#!/usr/bin/env python3
"""Reproduce retail SN assembler scheduling for reviewed matching units."""

from __future__ import annotations

import argparse
import re
from pathlib import Path


def replace_once(text: str, old: str, new: str, description: str) -> str:
    if text.count(old) != 1:
        raise ValueError(f"{description} not found uniquely")
    return text.replace(old, new, 1)


def normalize_scene_renderer(text: str) -> str:
    branch = re.compile(
        r"(?P<prefix>\t\.set\tnoreorder\n\t\.set\tnomacro\n"
        r"\tbeq\t\$17,\$0,(?P<label>\$L\d+)\n)"
        r"\tlui\t\$20,%hi\(D_800C33C0\) # high\n"
        r"(?P<suffix>\t\.set\tmacro\n\t\.set\treorder\n\n)"
    )
    match = branch.search(text)
    if match is None:
        raise ValueError("reviewed empty-list branch pattern not found")
    label = match.group("label")
    text = branch.sub(
        match.group("prefix")
        + "\tli\t$6,-1174405120\t\t\t# 0xba000000\n"
        + match.group("suffix")
        + "\tlui\t$20,%hi(D_800C33C0) # high\n",
        text,
        count=1,
    )

    target = label + ":\n\tli\t$6,-1174405120\t\t\t# 0xba000000\n"
    if text.count(target) != 1:
        raise ValueError("reviewed final-command label pattern not found uniquely")
    text = text.replace(
        target,
        "\tli\t$6,-1174405120\t\t\t# 0xba000000\n" + label + ":\n",
        1,
    )

    return replace_once(text, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\taddu\t$sp,$sp,40\n"
    ), (
        "\taddu\t$sp,$sp,40\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tnop\n"
    ), "reviewed renderer return scheduling pattern")


def normalize_player_selection(text: str) -> str:
    text = replace_once(text, (
        "\tlui\t$3,%hi(D_80054410) # high\n"
        "\taddiu\t$3,$3,%lo(D_80054410) # low\n"
        "\tlui\t$2,%hi(D_800B9280) # high\n"
    ), (
        "\tlui\t$2,%hi(D_800B9280) # high\n"
        "\tlui\t$3,%hi(D_80054410) # high\n"
        "\taddiu\t$3,$3,%lo(D_80054410) # low\n"
    ), "reviewed selection-table scheduling pattern")

    branch = re.compile(
        r"(?P<prefix>\t\.set\tnoreorder\n\t\.set\tnomacro\n"
        r"\tblez\t\$2,(?P<label>\$L\d+)\n)"
        r"\tli\t\$17,-4259840\t\t\t# 0xffbf0000\n"
        r"(?P<suffix>\t\.set\tmacro\n\t\.set\treorder\n\n)"
    )
    match = branch.search(text)
    if match is None:
        raise ValueError("reviewed player-loop entry pattern not found")
    label = match.group("label")
    text = branch.sub(
        match.group("prefix")
        + "\tlui\t$2,%hi(D_80052F04) # high\n"
        + match.group("suffix")
        + "\tli\t$17,-4259840\t\t\t# 0xffbf0000\n",
        text,
        count=1,
    )

    loop_tail = (
        "\tslt\t$2,$16,$2\n\tbne\t$2,$0,$L61\n"
        + label + ":\n\tlui\t$2,%hi(D_80052F04) # high\n"
    )
    text = replace_once(text, loop_tail, (
        "\tslt\t$2,$16,$2\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L61\n\tlui\t$2,%hi(D_80052F04) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n" + label + ":\n"
    ), "reviewed player-loop exit pattern")

    return replace_once(text, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\taddu\t$sp,$sp,96\n"
    ), (
        "\taddu\t$sp,$sp,96\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tnop\n"
    ), "reviewed selection return scheduling pattern")


def normalize_random_selection(text: str) -> str:
    return replace_once(text, (
        "\tlw\t$31,28($sp)\n\tlw\t$18,24($sp)\n"
        "\tlw\t$17,20($sp)\n\tlw\t$16,16($sp)\n"
        "\tsw\t$3,%lo(D_800529F8)($2)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\taddu\t$sp,$sp,32\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tsw\t$3,%lo(D_800529F8)($2)\n"
        "\tlw\t$31,28($sp)\n\tlw\t$18,24($sp)\n"
        "\tlw\t$17,20($sp)\n\tlw\t$16,16($sp)\n"
        "\taddu\t$sp,$sp,32\n\tj\t$31\n\tnop\n"
    ), "reviewed random-selection epilogue pattern")


def normalize_resource_release(text: str) -> str:
    return replace_once(text, (
        "\tlw\t$2,%lo(D_8005392C)($16)\n"
        "\tlw\t$31,20($sp)\n"
        "\tsw\t$0,%lo(D_8005392C)($16)\n"
        "\tlw\t$16,16($sp)\n\t#nop\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\taddu\t$sp,$sp,24\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tlw\t$2,%lo(D_8005392C)($16)\n"
        "\tsw\t$0,%lo(D_8005392C)($16)\n"
        "\tlw\t$31,20($sp)\n\tlw\t$16,16($sp)\n"
        "\taddu\t$sp,$sp,24\n\tj\t$31\n\tnop\n"
    ), "reviewed resource-release epilogue pattern")


def normalize_state_reset(text: str) -> str:
    return replace_once(text, (
        "\tlw\t$31,16($sp)\n"
        "\tlui\t$2,%hi(D_80061E00) # high\n"
    ), (
        "\tlui\t$2,%hi(D_80061E00) # high\n"
        "\tlw\t$31,16($sp)\n"
    ), "reviewed state-reset scheduling pattern")


def normalize_byte_clear_loop(text: str) -> str:
    return replace_once(
        text, "\tbne\t$2,$0,$L7\n", "\tbnel\t$2,$0,$L7\n",
        "reviewed byte-clear branch-likely pattern"
    )


def normalize_shared_return(text: str) -> str:
    first = (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tsltu\t$2,$2,1\n"
    )
    text = replace_once(text, first, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L_return\n\tsltu\t$2,$2,1\n"
    ), "reviewed shared-return first path")
    return replace_once(text, (
        "$L2:\n\tsw\t$2,%lo(D_80053828)($3)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tmove\t$2,$0\n"
    ), (
        "$L2:\n\tsw\t$2,%lo(D_80053828)($3)\n"
        "\tmove\t$2,$0\n$L_return:\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tnop\n"
    ), "reviewed shared-return second path")


def normalize_standard_return(text: str) -> str:
    epilogue = re.compile(
        r"\t\.set\tnoreorder\n\t\.set\tnomacro\n"
        r"\tj\t\$31\n\taddu\t\$sp,\$sp,(?P<frame>\d+)\n"
    )
    match = epilogue.search(text)
    if match is None:
        raise ValueError("reviewed SN return scheduling pattern not found")
    frame = match.group("frame")
    return epilogue.sub(
        f"\taddu\t$sp,$sp,{frame}\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tj\t$31\n\tnop\n",
        text,
        count=1,
    )


def normalize_room_search(text: str) -> str:
    old = "\taddu\t$2,$2,$18\n"
    if old not in text:
        raise ValueError("reviewed room-count decrement pattern not found")
    text = text.replace(old, "\taddu\t$2,$2,-1\n", 1)
    return normalize_standard_return(text)


def normalize_current_object_filter(text: str) -> str:
    text = replace_once(text, (
        "\tbne\t$3,$2,$L2\n"
        "\tlui\t$2,%hi(D_8006FB8C) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$4,%lo(D_8006FB8C)($2)\n"
    ), (
        "\tbne\t$3,$2,$L2\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$2,%hi(D_8006FB8C) # high\n"
        "\tlw\t$4,%lo(D_8006FB8C)($2)\n"
    ), "reviewed current-object branch scheduling pattern")
    return text


def normalize(text: str) -> str:
    if "\t.ent\tfunc_800149FC\n" in text:
        return normalize_scene_renderer(text)
    if "\t.ent\tfunc_8002A9D4\n" in text:
        return normalize_player_selection(text)
    if "\t.ent\tfunc_80021580\n" in text:
        return normalize_random_selection(text)
    if "\t.ent\tfunc_8001EED0\n" in text:
        return normalize_resource_release(text)
    if "\t.ent\tfunc_8001E0B0\n" in text:
        return normalize_state_reset(text)
    if "\t.ent\tfunc_80020D34\n" in text:
        return normalize_byte_clear_loop(text)
    if "\t.ent\tfunc_8001B7B0\n" in text:
        return normalize_shared_return(text)
    if "\t.ent\tfunc_800219EC\n" in text:
        return normalize_room_search(text)
    if "\t.ent\tfunc_8001C560\n" in text:
        return normalize_current_object_filter(text)
    return normalize_standard_return(text)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("input", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    args.output.write_bytes(
        normalize(args.input.read_text()).replace("\n", "\r\n").encode()
    )


if __name__ == "__main__":
    main()
