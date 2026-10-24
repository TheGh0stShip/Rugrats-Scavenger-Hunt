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


def normalize(text: str) -> str:
    if "\t.ent\tfunc_800149FC\n" in text:
        return normalize_scene_renderer(text)
    if "\t.ent\tfunc_8002A9D4\n" in text:
        return normalize_player_selection(text)
    raise ValueError("no reviewed SN scheduling normalization for this unit")


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
