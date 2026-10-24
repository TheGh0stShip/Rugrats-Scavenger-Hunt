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


def normalize_resource_dispatch(text: str) -> str:
    text = replace_once(text, (
        "\tlui\t$3,%hi(D_80061DC8) # high\n"
        "\tlui\t$2,%hi(D_80053934) # high\n"
    ), (
        "\tlui\t$2,%hi(D_80053934) # high\n"
        "\tlui\t$3,%hi(D_80061DC8) # high\n"
    ), "reviewed resource-dispatch address scheduling")
    return normalize_standard_return(text)


def normalize_lookup_entry(text: str) -> str:
    return replace_once(text, (
        "\tbne\t$3,$2,$L6\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\taddu\t$2,$4,24\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L2:\n\tmove\t$2,$0\n"
        "$L6:\n\tj\t$31\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeql\t$3,$2,$L6\n\taddu\t$2,$4,24\n"
        "\tj\t$L6\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L2:\n\tmove\t$2,$0\n"
        "$L6:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n"
    ), "reviewed lookup shared-return layout")


def normalize_string_width(text: str) -> str:
    text = replace_once(text, (
        "\tlw\t$7,%lo(D_8006D688)($2)\n\t#nop\n"
        "\tlw\t$2,20($7)\n"
    ), (
        "\tlw\t$3,%lo(D_8006D688)($2)\n\t#nop\n"
        "\tlw\t$2,20($3)\n"
    ), "reviewed string-width base register")
    text = replace_once(text, (
        "\tbeq\t$5,$0,$L3\n\tsubu\t$6,$0,$2\n"
    ), (
        "\tbeq\t$5,$0,$L3\n\tsubu\t$7,$0,$2\n"
    ), "reviewed string-width accumulator register")
    text = replace_once(text, (
        "\tli\t$8,10\t\t\t# 0x0000000a\n$L6:\n"
    ), (
        "\tli\t$8,10\t\t\t# 0x0000000a\n\tmove\t$6,$3\n$L6:\n"
    ), "reviewed string-width preserved base")
    text = text.replace("\tlw\t$3,20($7)\n", "\tlw\t$3,20($6)\n", 1)
    text = text.replace("\taddu\t$2,$7,$2\n", "\taddu\t$2,$6,$2\n", 1)
    text = text.replace("\taddu\t$6,$6,$2\n", "\taddu\t$7,$7,$2\n", 1)
    return text.replace("\tmove\t$2,$6\n", "\tmove\t$2,$7\n", 1)


def normalize_allocated_block_tail(text: str) -> str:
    text = replace_once(text, (
        "\tlw\t$31,20($sp)\n\tlw\t$16,16($sp)\n"
        "\tlui\t$3,%hi(D_80061E4C) # high\n"
        "\tsw\t$2,%lo(D_80061E4C)($3)\n"
    ), (
        "\tlui\t$3,%hi(D_80061E4C) # high\n"
        "\tsw\t$2,%lo(D_80061E4C)($3)\n"
        "\tlw\t$31,20($sp)\n\tlw\t$16,16($sp)\n"
    ), "reviewed allocated-block epilogue scheduling")
    return normalize_standard_return(text)


def normalize_resource_start_tail(text: str) -> str:
    text = replace_once(text, (
        "\tlw\t$31,24($sp)\n\tlw\t$17,20($sp)\n\tlw\t$16,16($sp)\n"
        "\tlui\t$2,%hi(D_80061E50) # high\n"
        "\tsw\t$0,%lo(D_80061E50)($2)\n"
    ), (
        "\tlui\t$2,%hi(D_80061E50) # high\n"
        "\tsw\t$0,%lo(D_80061E50)($2)\n"
        "\tlw\t$31,24($sp)\n\tlw\t$17,20($sp)\n\tlw\t$16,16($sp)\n"
    ), "reviewed resource-start epilogue scheduling")
    return normalize_standard_return(text)


def normalize_resource_descriptor_init(text: str) -> str:
    text = replace_once(text, (
        "$L4:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001DEF8\n\tsw\t$0,40($4)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L5:\n"
    ), (
        "$L4:\n\tsw\t$0,40($4)\n"
        "$L5:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001DEF8\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
    ), "reviewed descriptor shared-call layout")
    return normalize_standard_return(text)


def normalize_object_loop_reset(text: str) -> str:
    text = replace_once(text, (
        "\tlw\t$31,20($sp)\n\tlw\t$16,16($sp)\n"
        "\tli\t$2,10\t\t\t# 0x0000000a\n"
        "\tsw\t$2,%lo(D_8006FB90)($3)\n"
    ), (
        "\tli\t$2,10\t\t\t# 0x0000000a\n"
        "\tsw\t$2,%lo(D_8006FB90)($3)\n"
        "\tlw\t$31,20($sp)\n\tlw\t$16,16($sp)\n"
    ), "reviewed object-loop reset scheduling")
    return normalize_standard_return(text)


def normalize_tile_probe(text: str) -> str:
    replacements = (
        ("\taddu\t$5,$sp,32\n", "\taddu\t$5,$sp,24\n"),
        ("\taddu\t$6,$sp,36\n", "\taddu\t$6,$sp,28\n"),
        ("\tlw\t$2,32($sp)\n", "\tlw\t$2,24($sp)\n"),
        ("\tlw\t$2,36($sp)\n", "\tlw\t$2,28($sp)\n"),
        ("\ttrunc.w.s $f4,$f2,$8\n\tmfc1\t$5,$f4\n",
         "\ttrunc.w.s $f0,$f2,$8\n\tmfc1\t$5,$f0\n"),
        ("\taddu\t$6,$sp,24\n", "\taddu\t$6,$sp,32\n"),
    )
    for old, new in replacements:
        text = replace_once(text, old, new, "reviewed tile-probe stack/register layout")
    return normalize_standard_return(text)


def normalize_randomized_actor_init(text: str) -> str:
    text = replace_once(text, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8000DD88\n\tandi\t$4,$2,0x00ff\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
    ), (
        "\tlbu\t$4,%lo(D_800581E9)($17)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8000DD88\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
    ), "reviewed randomized-actor byte reload")
    return normalize_standard_return(text)


def normalize_menu_draw_tail(text: str) -> str:
    text = replace_once(text, (
        "\taddu\t$4,$18,14\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80016AA8\n\tsw\t$2,4($18)\n"
    ), (
        "\tsw\t$2,4($18)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80016AA8\n\taddu\t$4,$18,14\n"
    ), "reviewed menu-draw call scheduling")
    return normalize_standard_return(text)


def normalize_event_dispatch(text: str) -> str:
    text = replace_once(text, "\tslt\t$2,$3,53\n", "\tsltu\t$2,$3,53\n",
                        "reviewed event selector comparison")
    text = replace_once(text, (
        "\tlui\t$2,%hi(D_80052F04) # high\n"
        "\tlui\t$3,%hi(D_80001918) # high\n"
        "\tlw\t$2,%lo(D_80052F04)($2)\n"
        "\tl.s\t$f0,%lo(D_80001918)($3)\n"
    ), (
        "\tlui\t$2,%hi(D_80052F04) # high\n"
        "\tlw\t$2,%lo(D_80052F04)($2)\n"
        "\t.set\tnoat\n\tlui\t$1,%hi(D_80001918) # high\n"
        "\tl.s\t$f0,%lo(D_80001918)($1)\n\t.set\tat\n"
    ), "reviewed event float-load scheduling")
    return normalize_standard_return(text)


def normalize_random_object_flags(text: str) -> str:
    text = replace_once(text, (
        "\tsra\t$4,$2,31\n\tmfhi\t$9\n\t#nop\n\t#nop\n"
        "\tsubu\t$4,$9,$4\n\tsll\t$3,$4,1\n\taddu\t$3,$3,$4\n"
        "\tsubu\t$3,$2,$3\n"
    ), (
        "\tsra\t$3,$2,31\n\tmfhi\t$9\n\t#nop\n\t#nop\n"
        "\tsubu\t$4,$9,$3\n\tsll\t$3,$4,1\n\taddu\t$3,$3,$4\n"
        "\tsubu\t$4,$2,$3\n"
    ), "reviewed random-object remainder registers")
    text = text.replace("\tbeq\t$3,$2,$L8\n", "\tbeq\t$4,$2,$L8\n", 1)
    text = text.replace("\tslt\t$2,$3,2\n", "\tslt\t$2,$4,2\n", 1)
    text = text.replace("\tbeq\t$3,$0,$L25\n", "\tbeq\t$4,$0,$L25\n", 1)
    text = text.replace("\tbeq\t$3,$2,$L9\n", "\tbeq\t$4,$2,$L9\n", 1)
    text = replace_once(text, "\tbnel\t$2,$3,$L16\n", "\tbnel\t$3,$2,$L16\n",
                        "reviewed random-object operand order")
    return normalize_standard_return(text)


def normalize_state_machine_frame(text: str, limit: int) -> str:
    replacements = (
        ("\t.frame\t$sp,24,$31", "\t.frame\t$sp,40,$31"),
        ("\tsubu\t$sp,$sp,24\n", "\tsubu\t$sp,$sp,40\n"),
        ("\tsw\t$31,20($sp)\n", "\tsw\t$31,36($sp)\n"),
        ("\tsw\t$16,16($sp)\n", "\tsw\t$16,32($sp)\n"),
        (f"\tslt\t$2,$3,{limit}\n", f"\tsltu\t$2,$3,{limit}\n"),
        ("\tlw\t$31,20($sp)\n", "\tlw\t$31,36($sp)\n"),
        ("\tlw\t$16,16($sp)\n", "\tlw\t$16,32($sp)\n"),
        ("\taddu\t$sp,$sp,24\n", "\taddu\t$sp,$sp,40\n"),
    )
    for old, new in replacements:
        text = replace_once(text, old, new, "reviewed state-machine frame layout")
    return normalize_standard_return(text)


def normalize_object_allocation_tail(text: str) -> str:
    text = replace_once(text, "\taddu\t$17,$17,$2\n", "\taddu\t$17,$17,1\n",
                        "reviewed allocation count increment")
    text = replace_once(text, (
        "\tlui\t$3,%hi(D_80057D58) # high\n"
        "\tlw\t$2,%lo(D_80057D58)($3)\n"
        "\tlw\t$31,32($sp)\n\tlw\t$19,28($sp)\n\tlw\t$18,24($sp)\n"
        "\tlw\t$17,20($sp)\n\tlw\t$16,16($sp)\n"
        "\taddu\t$2,$2,1\n\tsw\t$2,%lo(D_80057D58)($3)\n"
    ), (
        "\tlui\t$3,%hi(D_80057D58) # high\n"
        "\tlw\t$2,%lo(D_80057D58)($3)\n"
        "\taddu\t$2,$2,1\n\tsw\t$2,%lo(D_80057D58)($3)\n"
        "\tlw\t$31,32($sp)\n\tlw\t$19,28($sp)\n\tlw\t$18,24($sp)\n"
        "\tlw\t$17,20($sp)\n\tlw\t$16,16($sp)\n"
    ), "reviewed allocation counter epilogue scheduling")
    return normalize_standard_return(text)


def normalize_resource_bounds(text: str) -> str:
    text = replace_once(text, (
        "\tlw\t$4,4($18)\n\t#nop\n"
        "\tlw\t$3,0($4)\n\tlw\t$2,4($4)\n\t#nop\n"
        "\tsltu\t$2,$3,$2\n"
    ), (
        "\tlw\t$3,4($18)\n\t#nop\n"
        "\tlw\t$2,4($3)\n\tlw\t$4,0($3)\n\t#nop\n"
        "\tsltu\t$2,$4,$2\n"
    ), "reviewed resource-bound registers")
    text = replace_once(text, "\tsw\t$3,4($4)\n", "\tsw\t$4,4($3)\n",
                        "reviewed resource-bound store operands")
    return normalize_standard_return(text)


def normalize_menu_flag_draw(text: str) -> str:
    text = replace_once(text, (
        "\tlui\t$2,%hi(D_800533E4) # high\n"
        "\taddiu\t$2,$2,%lo(D_800533E4) # low\n"
    ), "", "reviewed menu-table deferred address")
    text = replace_once(text, (
        "\taddu\t$16,$2,6\n$L5:\n"
    ), (
        "\tlui\t$2,%hi(D_800533E4) # high\n"
        "\taddiu\t$16,$2,%lo(D_800533E4) # low\n$L5:\n"
    ), "reviewed menu-table loop base")
    text = replace_once(text, "\tlh\t$5,-2($16)\n", "\tlh\t$5,4($16)\n",
                        "reviewed menu-table first coordinate")
    text = replace_once(text, "\tlh\t$6,0($16)\n", "\tlh\t$6,6($16)\n",
                        "reviewed menu-table second coordinate")
    return normalize_standard_return(text)


def normalize_object_value_dispatch(text: str) -> str:
    return replace_once(text, (
        "\tbeq\t$4,$2,$L2\n\tjal\tfunc_80016530\n\tj\t$L3\n"
        "$L2:\n\tlw\t$2,396($3)\n$L3:\n"
        "\tlw\t$31,16($sp)\n\t#nop\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\taddu\t$sp,$sp,24\n"
        "\t.set\tmacro\n\t.set\treorder\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$4,$2,$L2\n\tnop\n"
        "\tlw\t$2,396($3)\n\tj\t$L3\n\tnop\n"
        "$L2:\n\tjal\tfunc_80016530\n\tnop\n"
        "$L3:\n\tlw\t$31,16($sp)\n\taddu\t$sp,$sp,24\n"
        "\tj\t$31\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n"
    ), "reviewed object-value dispatch control flow")


def normalize_controller_direction_bits(text: str) -> str:
    text = replace_once(text, (
        "\tsll\t$2,$4,2\n\taddu\t$2,$2,$4\n\tsll\t$2,$2,1\n"
        "\tlui\t$3,%hi(D_80058B58) # high\n"
        "\taddiu\t$3,$3,%lo(D_80058B58) # low\n"
        "\taddu\t$2,$2,$3\n"
        "\tlui\t$3,%hi(D_800012D0) # high\n"
        "\tlb\t$7,6($2)\n\tl.s\t$f4,%lo(D_800012D0)($3)\n"
    ), (
        "\tlui\t$3,%hi(D_80058B58) # high\n"
        "\taddiu\t$3,$3,%lo(D_80058B58) # low\n"
        "\tsll\t$2,$4,2\n\taddu\t$2,$2,$4\n\tsll\t$2,$2,1\n"
        "\taddu\t$2,$2,$3\n\tlb\t$7,6($2)\n"
        "\t.word\t0x3c018000\n\t.word\t0xc42412d0\n"
    ), "reviewed controller base and first threshold scheduling")
    return replace_once(text, (
        "\tbeq\t$2,$0,$L2\n\tlui\t$2,%hi(D_800012D4) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L5\n\tori\t$5,$5,0x0002\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L2:\n\tl.s\t$f2,%lo(D_800012D4)($2)\n"
    ), (
        "\tbeq\t$2,$0,$L2\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L5\n\tori\t$5,$5,0x0002\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L2:\n\t.word\t0x3c018000\n\t.word\t0xc42212d4\n"
    ), "reviewed controller second threshold scheduling")


def normalize_object_type_selection(text: str) -> str:
    text = replace_once(text, (
        "\tsw\t$17,20($sp)\n\tmove\t$17,$0\n"
        "\tsw\t$16,16($sp)\n\tmove\t$16,$17\n"
    ), (
        "\tsw\t$16,16($sp)\n\tmove\t$16,$0\n"
        "\tsw\t$17,20($sp)\n\tmove\t$17,$16\n"
    ), "reviewed object-type selection prologue")
    text = replace_once(
        text, "\tjal\tfunc_80024660\n\tmove\t$4,$17\n",
        "\tjal\tfunc_80024660\n\tmove\t$4,$16\n",
        "reviewed object-type initial argument register",
    )
    text = replace_once(
        text, "$L38:\n\tli\t$16,2\t\t\t# 0x00000002\n$L3:\n",
        "$L38:\n$L3:\n\tli\t$16,2\t\t\t# 0x00000002\n",
        "reviewed object-type default label",
    )
    return normalize_standard_return(text)


def normalize_node_pool_init(text: str) -> str:
    text = replace_once(text, (
        "\tmove\t$6,$0\n\tmove\t$7,$2\n"
        "\tlw\t$3,%lo(D_80059DC8)($16)\n"
        "\tlui\t$2,%hi(D_80059DC4) # high\n"
        "\tsw\t$7,%lo(D_80059DC4)($2)\n"
        "\tlui\t$2,%hi(D_80059DC0) # high\n"
    ), (
        "\tmove\t$6,$0\n\tlui\t$3,%hi(D_80059DC4) # high\n"
        "\tmove\t$4,$2\n\tsw\t$4,%lo(D_80059DC4)($3)\n"
        "\tlw\t$3,%lo(D_80059DC8)($16)\n"
        "\tlui\t$2,%hi(D_80059DC0) # high\n"
    ), "reviewed node-pool global scheduling")
    text = replace_once(
        text, "\tbeq\t$3,$0,$L3\n\tsw\t$7,%lo(D_80059DC0)($2)\n",
        "\tbeq\t$3,$0,$L3\n\tsw\t$4,%lo(D_80059DC0)($2)\n",
        "reviewed node-pool head store",
    )
    text = replace_once(
        text, "\tmove\t$5,$7\n\tli\t$4,16\t\t\t# 0x00000010\n",
        "\tmove\t$7,$4\n\tmove\t$5,$7\n\tli\t$4,16\t\t\t# 0x00000010\n",
        "reviewed node-pool loop registers",
    )
    return normalize_standard_return(text)


def normalize_object_update_dispatch(text: str) -> str:
    text = replace_once(
        text, "\tandi\t$2,$3,0x0080\n",
        "\tandi\t$4,$3,0x0080\n",
        "reviewed object-update flag register",
    )
    text = replace_once(
        text, "\tbeq\t$2,$0,$L2\n\tli\t$2,-129",
        "\tbeq\t$4,$0,$L2\n\tli\t$2,-129",
        "reviewed object-update flag branch",
    )
    text = replace_once(text, (
        "\tand\t$2,$3,$2\n\tsw\t$2,280($16)\n"
        "\tandi\t$2,$3,0x0008\n"
    ), (
        "\tand\t$2,$3,$2\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$4,$0,$L2\n\tsw\t$2,280($16)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tandi\t$2,$2,0x0008\n"
    ), "reviewed object-update cleared-flag check")
    text = replace_once(
        text, "\tjal\tfunc_8000DFD8\n\tli\t$4,6",
        "\tjal\tfunc_8000DFD8\n\tnop",
        "reviewed object-update call scheduling",
    )
    return normalize_standard_return(text)


def remap_registers(text: str, mapping: dict[int, int]) -> str:
    for old in mapping:
        text = text.replace(f"${old}", f"$R{old}")
    for old, new in mapping.items():
        text = text.replace(f"$R{old}", f"${new}")
    return text


def normalize_list_range_dispatch(text: str) -> str:
    text = remap_registers(text, {20: 18, 19: 17, 18: 20, 17: 19})
    for opcode, register, old, new in (
        ("sw", 18, 32, 24), ("lw", 18, 32, 24),
        ("sw", 17, 28, 20), ("lw", 17, 28, 20),
        ("sw", 20, 24, 32), ("lw", 20, 24, 32),
        ("sw", 19, 20, 28), ("lw", 19, 20, 28),
    ):
        text = replace_once(
            text, f"\t{opcode}\t${register},{old}($sp)\n",
            f"\t{opcode}\t${register},{new}($sp)\n",
            "reviewed list-range saved-register layout",
        )
    text = replace_once(text, (
        "\tlw\t$31,40($sp)\n\tlw\t$21,36($sp)\n"
        "\tlw\t$18,24($sp)\n\tlw\t$17,20($sp)\n"
        "\tlw\t$20,32($sp)\n\tlw\t$19,28($sp)\n"
        "\tlw\t$16,16($sp)\n"
    ), (
        "\tlw\t$31,40($sp)\n\tlw\t$21,36($sp)\n"
        "\tlw\t$20,32($sp)\n\tlw\t$19,28($sp)\n"
        "\tlw\t$18,24($sp)\n\tlw\t$17,20($sp)\n"
        "\tlw\t$16,16($sp)\n"
    ), "reviewed list-range restore order")
    return normalize_standard_return(text)


def normalize_nearest_actor_search(text: str) -> str:
    text = remap_registers(text, {19: 18, 18: 19})
    for opcode, register, old, new in (
        ("sw", 18, 36, 32), ("lw", 18, 36, 32),
        ("sw", 19, 32, 36), ("lw", 19, 32, 36),
    ):
        text = replace_once(
            text, f"\t{opcode}\t${register},{old}($sp)\n",
            f"\t{opcode}\t${register},{new}($sp)\n",
            "reviewed nearest-actor saved-register layout",
        )
    text = replace_once(text, (
        "\tlbu\t$2,316($16)\n\tmove\t$19,$16\n"
        "\tsb\t$2,16($sp)\n\tlbu\t$2,317($19)\n"
        "\tmov.s\t$f20,$f0\n\tsb\t$2,17($sp)\n"
    ), (
        "\tlbu\t$2,316($16)\n\tmov.s\t$f20,$f0\n"
        "\tsb\t$2,16($sp)\n\tlbu\t$2,317($16)\n"
        "\tmove\t$19,$16\n\tsb\t$2,17($sp)\n"
    ), "reviewed nearest-actor best-result scheduling")
    text = replace_once(text, (
        "\tlw\t$31,44($sp)\n\tlw\t$20,40($sp)\n"
        "\tlw\t$18,32($sp)\n\tlw\t$19,36($sp)\n"
    ), (
        "\tlw\t$31,44($sp)\n\tlw\t$20,40($sp)\n"
        "\tlw\t$19,36($sp)\n\tlw\t$18,32($sp)\n"
    ), "reviewed nearest-actor restore order")
    return normalize_standard_return(text)


def normalize_direction_output(text: str) -> str:
    text = replace_once(
        text, "\tj\t$31\n\tsw\t$0,0($8)\n",
        "\tj\t$L14\n\tsw\t$0,0($8)\n",
        "reviewed direction-zero shared return",
    )
    text = replace_once(text, (
        "\tli\t$2,180\t\t\t# 0x000000b4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tsw\t$2,0($8)\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L13\n\tli\t$2,180\t\t\t# 0x000000b4\n"
    ), "reviewed direction-180 shared store")
    text = replace_once(text, (
        "\tli\t$2,90\t\t\t# 0x0000005a\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tsw\t$2,0($8)\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L13\n\tli\t$2,90\t\t\t# 0x0000005a\n"
    ), "reviewed direction-90 shared store")
    return replace_once(
        text, "\tli\t$2,270\t\t\t# 0x0000010e\n\tsw\t$2,0($8)\n$L14:\n",
        "\tli\t$2,270\t\t\t# 0x0000010e\n$L13:\n\tsw\t$2,0($8)\n$L14:\n",
        "reviewed direction-angle common store",
    )


def normalize_state_restore_with_flags(text: str) -> str:
    text = replace_once(text, (
        "\taddiu\t$5,$2,%lo(D_8005496C) # low\n"
        "\tlui\t$2,%hi(D_8005497C+8) # high\n"
        "\taddiu\t$4,$2,%lo(D_8005497C+8) # low\n"
    ), (
        "\taddiu\t$5,$2,%lo(D_8005496C) # low\n"
        "\tlui\t$2,%hi(D_8005497C) # high\n"
        "\taddiu\t$2,$2,%lo(D_8005497C) # low\n"
        "\taddiu\t$4,$2,8\n"
    ), "reviewed flag-table destination address")
    text = replace_once(
        text, "\tsw\t$2,0($4)\n$L9:\n\taddu\t$4,$4,4\n",
        "\tsw\t$2,0($4)\n\taddu\t$4,$4,4\n$L9:\n",
        "reviewed flag-table skipped-entry label",
    )
    return replace_once(text, (
        "\tlui\t$2,%hi(D_80052F18) # high\n"
        "\taddiu\t$3,$2,%lo(D_80052F18) # low\n"
        "\tlui\t$2,%hi(D_80052F4C) # high\n"
        "\taddiu\t$2,$2,%lo(D_80052F4C) # low\n"
        "\taddu\t$4,$2,48\n$L8:\n"
        "\tlw\t$7,0($2)\n\tlw\t$8,4($2)\n"
        "\tlw\t$9,8($2)\n\tlw\t$10,12($2)\n"
        "\tsw\t$7,0($3)\n\tsw\t$8,4($3)\n"
        "\tsw\t$9,8($3)\n\tsw\t$10,12($3)\n"
        "\taddu\t$2,$2,16\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$4,$L8\n\taddu\t$3,$3,16\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$7,0($2)\n\tsw\t$7,0($3)\n"
        "\tlw\t$31,72($sp)\n"
    ), (
        "\tlui\t$2,%hi(D_80052F18) # high\n"
        "\taddiu\t$7,$2,%lo(D_80052F18) # low\n"
        "\tlui\t$2,%hi(D_80052F4C) # high\n"
        "\taddiu\t$6,$2,%lo(D_80052F4C) # low\n"
        "\taddu\t$8,$6,48\n$L8:\n"
        "\tlw\t$2,0($6)\n\tlw\t$3,4($6)\n"
        "\tlw\t$4,8($6)\n\tlw\t$5,12($6)\n"
        "\tsw\t$2,0($7)\n\tsw\t$3,4($7)\n"
        "\tsw\t$4,8($7)\n\tsw\t$5,12($7)\n"
        "\taddu\t$6,$6,16\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$6,$8,$L8\n\taddu\t$7,$7,16\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$2,0($6)\n\tlw\t$31,72($sp)\n"
        "\tsw\t$2,0($7)\n"
    ), "reviewed state restore copy registers")


def normalize_timed_object_result(text: str) -> str:
    text = replace_once(text, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$3,$17,$L5\n\tslt\t$3,$3,$17\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli\t$2,60\t\t\t# 0x0000003c\n"
        "$L10:\n\tsh\t$2,404($16)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L8\n\tli\t$2,16\t\t\t# 0x00000010\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L5:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$3,$0,$L8\n\tli\t$2,1\t\t\t# 0x00000001\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli\t$2,2\t\t\t# 0x00000002\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$3,$17,$L9\n\tslt\t$3,$3,$17\n"
        "\tbne\t$3,$0,$L8\n\tli\t$2,1\t\t\t# 0x00000001\n"
        "\tj\t$L8\n\tli\t$2,2\t\t\t# 0x00000002\n"
        "$L9:\n\tli\t$2,60\t\t\t# 0x0000003c\n"
        "$L10:\n\tsh\t$2,404($16)\n\tli\t$2,16\t\t\t# 0x00000010\n"
        "\t.set\tmacro\n\t.set\treorder\n"
    ), "reviewed timed-object result flow")
    return normalize_standard_return(text)


def normalize_object_trigger_result(text: str) -> str:
    text = replace_once(text, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L3\n\tmove\t$17,$0\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$3,252($18)\n\tli\t$2,6\t\t\t# 0x00000006\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$3,$2,$L2\n\tli\t$2,2\t\t\t# 0x00000002\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L3:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L12\n\tmove\t$2,$0\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L2:\n\tlw\t$3,324($18)\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L_early_return\n\tmove\t$17,$0\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$3,252($18)\n\tli\t$2,6\t\t\t# 0x00000006\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$3,$2,$L12\n\tmove\t$2,$0\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L2:\n\tlw\t$3,324($18)\n"
        "\tli\t$2,2\t\t\t# 0x00000002\n"
    ), "reviewed object-trigger early return flow")
    text = replace_once(text, (
        "\tsltu\t$2,$2,2\n\tbeq\t$2,$0,$L9\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8002A5B0\n\tli\t$4,44\t\t\t# 0x0000002c\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L13\n\tmove\t$4,$0\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L9:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8002A5B0\n\tli\t$4,43\t\t\t# 0x0000002b\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli\t$4,1\t\t\t# 0x00000001\n"
        "$L13:\n\tjal\tfunc_8002A5EC\n\tlw\t$3,284($18)\n"
    ), (
        "\tsltu\t$2,$2,2\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L9\n\tnop\n"
        "\tjal\tfunc_8002A5B0\n\tli\t$4,43\t\t\t# 0x0000002b\n"
        "\tj\t$L13\n\tli\t$4,1\t\t\t# 0x00000001\n"
        "$L9:\n\tjal\tfunc_8002A5B0\n\tli\t$4,44\t\t\t# 0x0000002c\n"
        "\tmove\t$4,$0\n$L13:\n"
        "\tjal\tfunc_8002A5EC\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$3,284($18)\n"
    ), "reviewed object-trigger random branch layout")
    text = replace_once(text, (
        "\tbne\t$3,$2,$L11\n\tli\t$2,30\t\t\t# 0x0000001e\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tsh\t$2,404($18)\n"
    ), (
        "\tbne\t$3,$2,$L11\n\tli\t$4,30\t\t\t# 0x0000001e\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tsh\t$4,404($18)\n"
    ), "reviewed object-trigger timer register")
    text = replace_once(
        text, "\tli\t$17,1\t\t\t# 0x00000001\n\tmove\t$2,$17\n$L12:\n",
        "\tli\t$17,1\t\t\t# 0x00000001\n$L_early_return:\n\tmove\t$2,$17\n$L12:\n",
        "reviewed object-trigger early return target",
    )
    return normalize_standard_return(text)


def normalize_actor_state_dispatch(text: str) -> str:
    text = replace_once(text, (
        "\tbeq\t$2,$0,$L1\n\tlw\t$3,252($16)\n\t#nop\n"
        "\tsltu\t$2,$3,13\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$2,$0,$L1\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$3,252($16)\n\tsltu\t$2,$3,13\n"
    ), "reviewed actor-dispatch early return scheduling")
    for value in (523, 514, 518):
        text = replace_once(
            text,
            f"\tli\t$4,{value}\t\t\t# 0x{value:08x}\n",
            f"\tli\t$3,{value}\t\t\t# 0x{value:08x}\n",
            "reviewed actor-dispatch resource identifier register",
        )
    text = replace_once(text, (
        "$L7:\n\tbeq\t$2,$4,$L10\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001ECE4\n\tli\t$5,56\t\t\t# 0x00000038\n"
    ), (
        "$L7:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$2,$3,$L10\n\tmove\t$4,$3\n"
        "\tjal\tfunc_8001ECE4\n\tli\t$5,56\t\t\t# 0x00000038\n"
    ), "reviewed actor-dispatch resource call registers")
    text = replace_once(text, (
        "$L11:\n\tlw\t$2,248($16)\n"
        "\tli\t$5,4\t\t\t# 0x00000004\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$5,$L12\n\tlui\t$2,%hi(D_80052F18) # high\n"
    ), (
        "$L11:\n\tli\t$5,4\t\t\t# 0x00000004\n"
        "\tlw\t$2,248($16)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$5,$L12\n\taddu\t$4,$16,304\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$2,%hi(D_80052F18) # high\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
    ), "reviewed actor-dispatch selection scheduling")
    text = replace_once(text, (
        "$L12:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80021580\n\taddu\t$4,$16,304\n"
    ), (
        "$L12:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80021580\n\tnop\n"
    ), "reviewed actor-dispatch selection call scheduling")
    return normalize_standard_return(text)


def normalize_selection_menu(text: str) -> str:
    text = replace_once(text, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$3,$2,$L2\n\tlui\t$2,%hi(D_8000193C) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$2,%hi(D_80001930) # high\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L3\n\taddiu\t$4,$2,%lo(D_80001930) # low\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L2:\n\taddiu\t$4,$2,%lo(D_8000193C) # low\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbnel\t$3,$2,$L2\n\tlui\t$4,%hi(D_8000193C) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$4,%hi(D_80001930) # high\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L3\n\taddiu\t$4,$4,%lo(D_80001930) # low\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L2:\n\taddiu\t$4,$4,%lo(D_8000193C) # low\n"
    ), "reviewed selection-menu string branch")
    text = replace_once(text, (
        "\tlw\t$2,%lo(D_800529F0)($16)\n\t#nop\n"
        "\tlw\t$3,284($2)\n"
    ), (
        "\tlw\t$4,%lo(D_800529F0)($16)\n\t#nop\n"
        "\tlw\t$3,284($4)\n"
    ), "reviewed selection-menu player register")
    text = replace_once(
        text, "\tbne\t$3,$2,$L9\n",
        "\tbne\t$3,$2,$L_player_ready\n",
        "reviewed selection-menu player reuse branch",
    )
    text = replace_once(text, (
        "$L9:\n\tlw\t$4,%lo(D_800529F0)($16)\n"
        "\tjal\tfunc_800166B0\n"
    ), (
        "$L9:\n\tlw\t$4,%lo(D_800529F0)($16)\n"
        "$L_player_ready:\n\tjal\tfunc_800166B0\n"
    ), "reviewed selection-menu shared player call")
    return normalize_standard_return(text)


def normalize_actor_scale_update(text: str) -> str:
    text = replace_once(text, (
        "\tlui\t$3,%hi(D_80057D40) # high\n"
        "\tlw\t$2,244($16)\n"
        "\tlw\t$3,%lo(D_80057D40)($3)\n\t#nop\n"
        "\txor\t$2,$2,$3\n\tsltu\t$17,$2,1\n"
    ), (
        "\tlui\t$2,%hi(D_80057D40) # high\n"
        "\tlw\t$3,244($16)\n"
        "\tlw\t$2,%lo(D_80057D40)($2)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeql\t$3,$2,$L_scale_flag_set\n"
        "\tli\t$17,1\t\t\t# 0x00000001\n"
        "\t.set\tmacro\n\t.set\treorder\n"
        "$L_scale_flag_set:\n"
    ), "reviewed actor-scale equality test")
    text = replace_once(
        text, "\tbne\t$16,$2,$L2\n",
        "\tbne\t$16,$2,$L_scale_ready\n",
        "reviewed actor-scale pointer mismatch target",
    )
    text = replace_once(text, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbnel\t$2,$0,$L2\n\tli\t$17,1\t\t\t# 0x00000001\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L2:\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$2,$0,$L_scale_ready\n\tnop\n"
        "$L2:\n\tli\t$17,1\t\t\t# 0x00000001\n"
        "$L_scale_ready:\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
    ), "reviewed actor-scale selected-object flag test")
    text = replace_once(
        text, "\tli.s\t$f0,1.00000000000000000000e0\n",
        "\tl.s\t$f0,$LC_scale_full\n",
        "reviewed actor-scale full-scale constant",
    )
    text = replace_once(
        text, "\tli.d\t$f2,1.00000000000000005551e-1\n",
        "\tl.d\t$f2,$LC_scale_up\n",
        "reviewed actor-scale increase constant",
    )
    text = replace_once(
        text, "\tli.d\t$f2,2.00000000000000011102e-1\n",
        "\tl.d\t$f2,$LC_scale_down\n",
        "reviewed actor-scale decrease constant",
    )
    text = replace_once(text, "\t.end\tfunc_80020E8C\n", (
        "\t.end\tfunc_80020E8C\n"
        "\t.rdata\n\t.align\t2\n"
        "$LC_scale_full:\n\t.word\t0x3f800000\n"
        "$LC_scale_up:\n\t.word\t0x3fb99999\n\t.word\t0x9999999a\n"
        "$LC_scale_down:\n\t.word\t0x3fc99999\n\t.word\t0x9999999a\n"
    ), "reviewed actor-scale literal pool")
    return normalize_standard_return(text)


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
    if "\t.ent\tfunc_8001EF08\n" in text:
        return normalize_resource_dispatch(text)
    if "\t.ent\tfunc_80019788\n" in text:
        return normalize_lookup_entry(text)
    if "\t.ent\tfunc_8001FF88\n" in text:
        return normalize_string_width(text)
    if "\t.ent\tfunc_8001EC78\n" in text:
        return normalize_allocated_block_tail(text)
    if "\t.ent\tfunc_8001ECE4\n" in text:
        return normalize_resource_start_tail(text)
    if "\t.ent\tfunc_8001BDD0\n" in text:
        return normalize_resource_descriptor_init(text)
    if "\t.ent\tfunc_800227F0\n" in text:
        return normalize_object_loop_reset(text)
    if "\t.ent\tfunc_80020FB0\n" in text:
        return normalize_tile_probe(text)
    if "\t.ent\tfunc_800162B0\n" in text:
        return normalize_randomized_actor_init(text)
    if "\t.ent\tfunc_80017BE4\n" in text or "\t.ent\tfunc_800176FC\n" in text:
        return normalize_menu_draw_tail(text)
    if "\t.ent\tfunc_8001C77C\n" in text:
        return normalize_event_dispatch(text)
    if "\t.ent\tfunc_80023360\n" in text:
        return normalize_random_object_flags(text)
    if "\t.ent\tfunc_8001ABC8\n" in text:
        return normalize_state_machine_frame(text, 33)
    if "\t.ent\tfunc_8001ADC8\n" in text:
        return normalize_state_machine_frame(text, 29)
    if "\t.ent\tfunc_800206F8\n" in text:
        return normalize_object_allocation_tail(text)
    if "\t.ent\tfunc_8001E204\n" in text:
        return normalize_resource_bounds(text)
    if "\t.ent\tfunc_80016AF8\n" in text:
        return normalize_menu_flag_draw(text)
    if "\t.ent\tfunc_800166B0\n" in text:
        return normalize_object_value_dispatch(text)
    if "\t.ent\tfunc_80016530\n" in text:
        return normalize_controller_direction_bits(text)
    if "\t.ent\tfunc_800226A8\n" in text:
        return normalize_object_type_selection(text)
    if "\t.ent\tfunc_80016830\n" in text:
        return normalize_node_pool_init(text)
    if "\t.ent\tfunc_80023C40\n" in text:
        return normalize_object_update_dispatch(text)
    if "\t.ent\tfunc_80016958\n" in text:
        return normalize_list_range_dispatch(text)
    if "\t.ent\tfunc_800218BC\n" in text:
        return normalize_nearest_actor_search(text)
    if "\t.ent\tfunc_80021604\n" in text:
        return normalize_direction_output(text)
    if "\t.ent\tfunc_8001906C\n" in text:
        return normalize_state_restore_with_flags(text)
    if "\t.ent\tfunc_80023110\n" in text:
        return normalize_timed_object_result(text)
    if "\t.ent\tfunc_80023D64\n" in text:
        return normalize_object_trigger_result(text)
    if "\t.ent\tfunc_800212D0\n" in text:
        return normalize_actor_state_dispatch(text)
    if "\t.ent\tfunc_8001CC40\n" in text:
        return normalize_selection_menu(text)
    if "\t.ent\tfunc_80020E8C\n" in text:
        return normalize_actor_scale_update(text)
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
