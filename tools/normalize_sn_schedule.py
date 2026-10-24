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


def normalize_debris_spawn(text: str) -> str:
    text = replace_once(text, (
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$2,%hi(func_8000A9E0) # high\n"
    ), (
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli\t$3,1\t\t\t# 0x00000001\n"
        "\tlui\t$2,%hi(func_8000A9E0) # high\n"
    ), "reviewed debris constant allocation")
    text = replace_once(text, (
        "\tlui\t$2,%hi(D_800581A8) # high\n"
        "\tlw\t$3,%lo(D_800581A8)($2)\n"
        "\tli\t$2,1\t\t\t# 0x00000001\n"
        "\tsw\t$2,224($16)\n\tsh\t$2,10($16)\n"
        "\tsw\t$17,228($16)\n\tsw\t$0,232($16)\n"
        "\tlw\t$8,44($3)\n\tlw\t$9,48($3)\n\tlw\t$10,52($3)\n"
        "\tsw\t$8,44($16)\n\tsw\t$9,48($16)\n\tsw\t$10,52($16)\n"
        "\tlw\t$8,236($3)\n\tlw\t$9,240($3)\n\tlw\t$10,244($3)\n"
        "\tsw\t$8,236($16)\n\tsw\t$9,240($16)\n\tsw\t$10,244($16)\n"
    ), (
        "\tlui\t$2,%hi(D_800581A8) # high\n"
        "\tsw\t$3,224($16)\n\tsh\t$3,10($16)\n"
        "\tlw\t$3,%lo(D_800581A8)($2)\n"
        "\tsw\t$17,228($16)\n\tsw\t$0,232($16)\n"
        "\tmove\t$2,$3\n"
        "\tlw\t$4,44($3)\n\tlw\t$5,48($3)\n\tlw\t$6,52($3)\n"
        "\tsw\t$4,44($16)\n\tsw\t$5,48($16)\n\tsw\t$6,52($16)\n"
        "\tlw\t$3,236($2)\n\tlw\t$4,240($2)\n\tlw\t$5,244($2)\n"
        "\tsw\t$3,236($16)\n\tsw\t$4,240($16)\n\tsw\t$5,244($16)\n"
    ), "reviewed debris vector-copy allocation")
    text = replace_once(
        text, "\tsra\t$7,$8,$6\n", "\tsra\t$7,$8,4\n",
        "reviewed debris final divide shift"
    )
    return normalize_standard_return(text)


def normalize_board_space_label(text: str) -> str:
    """Reproduce reviewed SN switch lowering and float-store scheduling."""
    text = replace_once(
        text, "\tsll\t$3,$3,$19\n", "\tsll\t$3,$3,2\n",
        "reviewed label jump-table scale",
    )
    text = replace_once(
        text,
        "\tbeq\t$4,$2,$L21\n\tsltu\t$2,$4,$2\n",
        "\tbeq\t$4,$2,$L21\n\tsltu\t$2,$4,7\n",
        "reviewed label first range immediate",
    )
    text = replace_once(
        text,
        "\tbeq\t$4,$2,$L17\n\tsltu\t$2,$4,$2\n",
        "\tbeq\t$4,$2,$L17\n\tsltu\t$2,$4,12\n",
        "reviewed label second range immediate",
    )
    text = replace_once(text, (
        "$L39:\n\tlui\t$2,%hi($LC22) # high\n$L71:\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L14\n\taddiu\t$17,$2,%lo($LC22) # low\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L45:\n\tlw\t$2,%lo(D_80052F18)($17)\n\t#nop\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L46\n\tlui\t$2,%hi($LC24) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$2,%hi($LC23) # high\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L14\n\taddiu\t$17,$2,%lo($LC23) # low\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L46:\n\taddiu\t$17,$2,%lo($LC24) # low\n$L14:\n"
    ), (
        "$L45:\n\tlw\t$2,%lo(D_80052F18)($17)\n\t#nop\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L46\n\tlui\t$2,%hi($LC24) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$2,%hi($LC23) # high\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L14\n\taddiu\t$17,$2,%lo($LC23) # low\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L46:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L14\n\taddiu\t$17,$2,%lo($LC24) # low\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L39:\n\tlui\t$2,%hi($LC22) # high\n"
        "$L71:\n\taddiu\t$17,$2,%lo($LC22) # low\n$L14:\n"
    ), "reviewed label default and door tail layout")
    text = replace_once(text, (
        "\tjal\tfunc_8000E108\n\tbeq\t$16,$2,$L58\n"
        "\tli.s\t$f4,1.00000000000000000000e4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$20,$0,$L72\n\tlui\t$2,%hi(D_8005368C) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli.s\t$f4,1.00000000000000000000e3\n$L72:\n"
        "\tli.s\t$f0,-8.00000000000000000000e2\n"
        "\tli.s\t$f2,-6.00000000000000000000e3\n"
        "\taddiu\t$2,$2,%lo(D_8005368C) # low\n"
        "\ts.s\t$f4,60($2)\n\ts.s\t$f0,64($2)\n\ts.s\t$f2,68($2)\n"
    ), (
        "\tjal\tfunc_8000E108\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$16,$2,$L58\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli.s\t$f0,1.00000000000000000000e4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$20,$0,$L72\n\tlui\t$2,%hi(D_8005368C) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli.s\t$f0,1.00000000000000000000e3\n$L72:\n"
        "\ts.s\t$f0,%lo(D_8005368C+60)($2)\n"
        "\tlui\t$2,%hi(D_8005368C) # high\n"
        "\tli.s\t$f0,-8.00000000000000000000e2\n"
        "\tli.s\t$f2,-6.00000000000000000000e3\n"
        "\taddiu\t$2,$2,%lo(D_8005368C) # low\n"
        "\ts.s\t$f0,64($2)\n\ts.s\t$f2,68($2)\n"
    ), "reviewed label camera float stores")
    tail_start = text.find("\tjal\tfunc_8000DF14\n", text.find("D_8005B214"))
    tail_end = text.find("$L1:\n", tail_start)
    if tail_start < 0 or tail_end < 0:
        raise ValueError("reviewed label final switch span not found")
    tail = text[tail_start:tail_end]
    tail = tail.replace("\tsra\t$3,$2,16\n", "\tsra\t$4,$2,16\n", 1)
    tail = tail.replace("\tbeq\t$3,$2,$L1\n\tsltu\t$2,$3,$2\n", "\tbeq\t$4,$2,$L1\n\tsltu\t$2,$4,11\n", 1)
    tail = tail.replace("\tbne\t$2,$0,$L73\n", "\tbnel\t$2,$0,$L73_after\n", 1)
    tail = tail.replace("\tsltu\t$2,$3,249\n", "\tsltu\t$2,$4,249\n", 1)
    tail = tail.replace("\tsltu\t$2,$3,245\n", "\tsltu\t$2,$4,245\n", 1)
    tail = tail.replace("$L73:\n\tmove\t$5,$4\n", "$L73:\n\tli\t$4,255\t\t\t# 0x000000ff\n$L73_after:\n\tmove\t$5,$4\n", 1)
    text = text[:tail_start] + tail + text[tail_end:]
    return normalize_standard_return(text)


def normalize_actor_event_completion(text: str) -> str:
    """Preserve the reviewed result copy and late global-load order."""
    text = replace_once(text, (
        "\tli\t$2,-1\t\t\t# 0xffffffff\n"
        "\tsw\t$2,528($17)\n\tsw\t$16,544($17)\n"
    ), (
        "\tli\t$2,-1\t\t\t# 0xffffffff\n"
        "\tsw\t$2,528($17)\n\tmove\t$2,$16\n\tsw\t$2,544($17)\n"
    ), "reviewed actor completion result copy")
    text = replace_once(text, (
        "\tlui\t$3,%hi(D_80054994) # high\n"
        "\tlui\t$2,%hi(D_80058190) # high\n"
    ), (
        "\tlui\t$2,%hi(D_80058190) # high\n"
        "\tlui\t$3,%hi(D_80054994) # high\n"
    ), "reviewed actor completion global load order")
    return normalize_standard_return(text)


def normalize_frame_display_lists(text: str) -> str:
    """Reproduce the reviewed SN constant allocation and display-list loop."""
    text = replace_once(
        text, "\tsubu\t$sp,$sp,64\n",
        "\tsubu\t$sp,$sp,64\n\tsw\t$19,36($sp)\n\tmove\t$19,$0\n",
        "reviewed frame-list early loop state",
    )
    text = replace_once(
        text, "\tsw\t$19,36($sp)\n\tmove\t$19,$0\n\tlui\t$2,%hi(D_8007784C) # high\n",
        "\tlui\t$2,%hi(D_8007784C) # high\n",
        "reviewed frame-list removed late loop state",
    )
    text = replace_once(text, (
        "\tsw\t$23,52($sp)\n"
        "\tli\t$23,-184549376\t\t\t# 0xf5000000\n"
        "\tori\t$23,$23,0x0100\n"
        "\tsw\t$22,48($sp)\n"
        "\tli\t$22,117637120\t\t\t# 0x07030000\n"
        "\tori\t$22,$22,0xc000\n"
        "\tsw\t$fp,56($sp)\n"
        "\tli\t$fp,-419430400\t\t\t# 0xe7000000\n"
    ), (
        "\tsw\t$fp,56($sp)\n"
        "\tli\t$fp,-402653184\t\t\t# 0xe8000000\n"
        "\tsw\t$22,48($sp)\n"
        "\tli\t$22,-184549376\t\t\t# 0xf5000000\n"
        "\tori\t$22,$22,0x0100\n"
        "\tsw\t$23,52($sp)\n"
        "\tli\t$23,117440512\t\t\t# 0x07000000\n"
    ), "reviewed frame-list persistent constants")
    text = replace_once(
        text,
        "\tlw\t$13,%lo(D_8006D680)($3)\n",
        "\tlw\t$14,%lo(D_8006D680)($3)\n",
        "reviewed frame-list base allocation",
    )
    text = replace_once(
        text, "\tmove\t$7,$fp\n", "\tli\t$7,-419430400\t\t\t# 0xe7000000\n",
        "reviewed frame-list pipe-sync constant",
    )
    start = text.find("$L5:\n")
    end = text.find("\tjal\tfunc_80047E80\n", start)
    if start < 0 or end < 0:
        raise ValueError("reviewed frame-list loop span not found")
    loop = """$L5:
\tlw\t$2,0($25)
\t.set\tnoreorder
\t.set\tnomacro
\tbeq\t$2,$0,$L13
\tli\t$10,117637120\t\t\t# 0x07030000
\t.set\tmacro
\t.set\treorder

\tori\t$10,$10,0xc000
\tmove\t$13,$0
\tmove\t$15,$14
\tmove\t$11,$14
\tlw\t$4,%lo(D_8007784C)($24)
\tli\t$12,32\t\t\t# 0x00000020
\tmove\t$5,$4
\taddu\t$4,$4,8
\tli\t$16,-49283072\t\t\t# 0xfd100000
\taddu\t$6,$4,8
\taddu\t$7,$4,16
\taddu\t$8,$4,24
\taddu\t$9,$4,32
\tsw\t$16,0($5)
\tlw\t$3,0($25)
\taddu\t$2,$4,40
\tsw\t$4,%lo(D_8007784C)($24)
\tsw\t$6,%lo(D_8007784C)($24)
\tsw\t$7,%lo(D_8007784C)($24)
\tsw\t$8,%lo(D_8007784C)($24)
\tsw\t$9,%lo(D_8007784C)($24)
\tsw\t$2,%lo(D_8007784C)($24)
\tli\t$2,-436207616\t\t\t# 0xe6000000
\taddu\t$3,$3,48
\tsw\t$3,4($5)
\tsw\t$fp,0($4)
\tsw\t$0,4($4)
\tsw\t$22,8($4)
\tsw\t$23,4($6)
\tsw\t$2,16($4)
\tli\t$2,-268435456\t\t\t# 0xf0000000
\tsw\t$0,4($7)
\tsw\t$2,24($4)
\tli\t$2,-419430400\t\t\t# 0xe7000000
\tsw\t$10,4($8)
\tsw\t$2,32($4)
\tsw\t$0,4($9)
$L10:
\taddu\t$4,$15,$12
\taddu\t$12,$12,3600
\tlw\t$3,%lo(D_8007784C)($24)
\taddu\t$13,$13,1
\tmove\t$2,$3
\tsw\t$21,0($2)
\tsw\t$4,4($2)
\tlw\t$2,0($11)
\taddu\t$3,$3,8
\tsw\t$3,%lo(D_8007784C)($24)
\tmove\t$3,$2
\taddu\t$2,$2,8
\tsw\t$2,0($11)
\taddu\t$11,$11,4
\tslt\t$2,$13,8
\tsw\t$20,0($3)
\t.set\tnoreorder
\t.set\tnomacro
\tbne\t$2,$0,$L10
\tsw\t$0,4($3)
\t.set\tmacro
\t.set\treorder

$L13:
\taddu\t$14,$14,28832
\taddu\t$19,$19,1
\tslt\t$2,$19,2
\t.set\tnoreorder
\t.set\tnomacro
\tbne\t$2,$0,$L5
\taddu\t$25,$25,4
\t.set\tmacro
\t.set\treorder

"""
    return normalize_standard_return(text[:start] + loop + text[end:])


def normalize_spin_object_setup(text: str) -> str:
    """Reproduce the reviewed SN allocation and scheduling for the spin-object setup loop."""
    start = text.find("\tlui\t$2,%hi(D_800C3E50) # high\n")
    end_marker = "\tbne\t$2,$0,$L5\n\taddu\t$18,$18,60\n"
    end = text.find(end_marker, start)
    if start < 0 or end < 0:
        raise ValueError("reviewed spin-object loop allocation span not found")
    end += len(end_marker)
    marker = "$@spin_object_swap"
    span = text[start:end]
    span = span.replace("$17", marker).replace("$18", "$17").replace(marker, "$18")
    text = text[:start] + span + text[end:]

    text = replace_once(text, (
        "\tlh\t$3,%lo(D_800C3E44)($22)\n\t#nop\n"
        "\taddu\t$3,$3,-3\n #APP\n #NO_APP\n"
        "\tsubu\t$3,$16,$3\n\tmult\t$3,$20\n"
        "\tlw\t$2,456($19)\n\taddu\t$19,$19,4\n\tsw\t$2,0($18)\n"
    ), (
        "\tlh\t$3,%lo(D_800C3E44)($22)\n\tlw\t$2,456($19)\n"
        "\taddu\t$3,$3,-3\n #APP\n #NO_APP\n"
        "\tsubu\t$3,$16,$3\n\tmult\t$3,$20\n"
        "\taddu\t$19,$19,4\n\taddu\t$16,$16,1\n\tsw\t$2,0($18)\n"
    ), "reviewed spin-object loop-head schedule")
    text = replace_once(text, "\tlw\t$6,0($2)\n\tlw\t$7,4($2)\n\tlw\t$8,8($2)\n\tsw\t$6,0($17)\n\tsw\t$7,4($17)\n\tsw\t$8,8($17)\n", "\tlw\t$3,0($2)\n\tlw\t$4,4($2)\n\tlw\t$5,8($2)\n\tsw\t$3,0($17)\n\tsw\t$4,4($17)\n\tsw\t$5,8($17)\n", "reviewed spin-object vector-copy allocation")
    text = replace_once(text, (
        "\tlw\t$2,0($18)\n\taddu\t$16,$16,1\n\tsw\t$2,52($17)\n"
        "\tlw\t$4,0($18)\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_800197C8\n\taddu\t$18,$18,4\n"
    ), (
        "\tlw\t$4,0($18)\n\taddu\t$18,$18,4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_800197C8\n\tsw\t$4,52($17)\n"
    ), "reviewed spin-object item call schedule")
    text = replace_once(text, (
        "\tbeq\t$2,$0,$L11\n\tsll\t$2,$16,4\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli\t$4,-1\t\t\t# 0xffffffff\n"
        "\tlui\t$3,%hi(D_80054B08) # high\n"
        "\taddiu\t$3,$3,%lo(D_80054B08) # low\n"
    ), (
        "\tbeq\t$2,$0,$L11\n\tli\t$4,-1\t\t\t# 0xffffffff\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$3,%hi(D_80054B08) # high\n"
        "\taddiu\t$3,$3,%lo(D_80054B08) # low\n"
        "\tsll\t$2,$16,4\n"
    ), "reviewed spin-object clear-loop entry schedule")
    return normalize_standard_return(text)


def normalize_spin_object_motion(text: str) -> str:
    """Reproduce the reviewed retail allocation for the contiguous spin-motion loop."""
    text = replace_once(text, "\tmove\t$9,$0\n", "", "reviewed redundant offset initialization")
    mapping = {
        "8": "7", "9": "8", "10": "9", "24": "15", "15": "14",
        "14": "13", "13": "12", "12": "11", "11": "10", "7": "6",
    }
    marker = re.compile(r"\$(24|15|14|13|12|11|10|9|8|7)(?!\d)")
    text = marker.sub(lambda match: "$" + mapping[match.group(1)], text)
    text = replace_once(text, "\tmove\t$7,$8\n", "\tmove\t$7,$0\n", "reviewed motion-loop index initialization")
    text = replace_once(text, "\tmove\t$9,$2\n\tlui\t$15,%hi(D_800C3E40) # high\n", "\tlui\t$15,%hi(D_800C3E40) # high\n\tmove\t$9,$2\n", "reviewed motion-loop global-base schedule")
    text = replace_once(text, "\tmove\t$6,$11\n", "\tmove\t$6,$11\n\tmove\t$8,$7\n", "reviewed motion-loop offset initialization")
    text = replace_once(text, "\tlui\t$2,%hi(D_80002FE8) # high\n\tl.s\t$f4,%lo(D_80002FE8)($2)\n", "\t.set\tnoat\n\tlui\t$1,%hi(D_80002FE8) # high\n\tl.s\t$f4,%lo(D_80002FE8)($1)\n\t.set\tat\n", "reviewed motion constant address allocation")
    text = replace_once(text, (
        "\tmove\t$6,$4\n\taddu\t$2,$7,$9\n\tsubu\t$2,$2,$10\n",
    )[0], (
        "\taddu\t$2,$7,$9\n\tsubu\t$2,$2,$10\n\tmove\t$3,$4\n",
    )[0], "reviewed motion-loop side-value schedule")
    text = text.replace("\taddu\t$3,$3,$11\n\taddu\t$3,$8,$3\n", "\taddu\t$3,$3,$8\n\taddu\t$3,$3,$11\n")
    text = replace_once(text, "\tsll\t$3,$6,1\n\taddu\t$3,$3,$6\n", "\tmove\t$4,$3\n\tsll\t$3,$4,1\n\taddu\t$3,$3,$4\n", "reviewed reverse-motion side allocation")
    text = replace_once(text, "\taddu\t$8,$8,60\n", "\taddu\t$6,$6,60\n", "reviewed object-pointer increment")
    text = replace_once(text, "\tbne\t$2,$0,$L5\n\taddu\t$6,$6,60\n", "\tbne\t$2,$0,$L5\n\taddu\t$8,$8,60\n", "reviewed motion-offset delay slot")
    return normalize_standard_return(text)


def normalize_spin_object_targets(text: str) -> str:
    """Reproduce the reviewed retail allocation for the adjacent target-motion loop."""
    text = replace_once(text, "\tlw\t$12,0($5)\n\tlw\t$13,4($5)\n\tlw\t$14,8($5)\n\tsw\t$12,16($sp)\n\tsw\t$13,20($sp)\n\tsw\t$14,24($sp)\n", "\tlw\t$2,0($5)\n\tlw\t$3,4($5)\n\tlw\t$4,8($5)\n\tsw\t$2,16($sp)\n\tsw\t$3,20($sp)\n\tsw\t$4,24($sp)\n", "reviewed current-position copy allocation")
    text = replace_once(text, "\tlw\t$12,0($2)\n\tlw\t$13,4($2)\n\tlw\t$14,8($2)\n\tsw\t$12,32($sp)\n\tsw\t$13,36($sp)\n\tsw\t$14,40($sp)\n", "\tlw\t$3,0($2)\n\tlw\t$4,4($2)\n\tlw\t$5,8($2)\n\tsw\t$3,32($sp)\n\tsw\t$4,36($sp)\n\tsw\t$5,40($sp)\n", "reviewed target-position copy allocation")

    start = text.find("\tlui\t$4,%hi(D_80054AB0) # high\n")
    end_marker = "\tbne\t$2,$0,$L5\n\ts.s\t$f0,44($3)\n"
    end = text.find(end_marker, start)
    if start < 0 or end < 0:
        raise ValueError("reviewed target-motion allocation span not found")
    end += len(end_marker)
    mapping = {"4": "3", "6": "8", "11": "13", "8": "9", "10": "12", "7": "11", "9": "10", "5": "7"}
    pattern = re.compile(r"\$(11|10|9|8|7|6|5|4)(?!\d)")
    span = pattern.sub(lambda match: "$" + mapping[match.group(1)], text[start:end])
    text = text[:start] + span + text[end:]
    text = replace_once(text, "\tlw\t$2,0($7)\n\tlw\t$3,4($7)\n\tlw\t$3,8($7)\n\tsw\t$2,16($sp)\n\tsw\t$3,20($sp)\n\tsw\t$3,24($sp)\n", "\tlw\t$2,0($7)\n\tlw\t$3,4($7)\n\tlw\t$4,8($7)\n\tsw\t$2,16($sp)\n\tsw\t$3,20($sp)\n\tsw\t$4,24($sp)\n", "reviewed current-position post-map allocation")
    text = replace_once(text, "\tlw\t$3,0($2)\n\tlw\t$3,4($2)\n\tlw\t$7,8($2)\n\tsw\t$3,32($sp)\n\tsw\t$3,36($sp)\n\tsw\t$7,40($sp)\n", "\tlw\t$3,0($2)\n\tlw\t$4,4($2)\n\tlw\t$5,8($2)\n\tsw\t$3,32($sp)\n\tsw\t$4,36($sp)\n\tsw\t$5,40($sp)\n", "reviewed target-position post-map allocation")

    text = replace_once(text, "\tli\t$13,6\t\t\t# 0x00000006\n\tlui\t$9,%hi(D_800C3E40) # high\n", "\tlui\t$9,%hi(D_800C3E40) # high\n\tli\t$13,6\t\t\t# 0x00000006\n", "reviewed target-motion setup schedule")
    text = replace_once(text, "\tlui\t$2,%hi(D_80002FEC) # high\n\tl.s\t$f4,%lo(D_80002FEC)($2)\n", "\t.set\tnoat\n\tlui\t$1,%hi(D_80002FEC) # high\n\tl.s\t$f4,%lo(D_80002FEC)($1)\n\t.set\tat\n", "reviewed target-motion constant allocation")
    text = replace_once(text, "\tsh\t$13,48($7)\n\tsw\t$7,%lo(D_800C3E40)($9)\n", "\tsh\t$13,48($7)\n", "reviewed target pointer publication schedule source")
    text = replace_once(text, "\tbne\t$16,$12,$L6\n\taddu\t$3,$8,1\n", "\tbne\t$16,$12,$L6\n\tsw\t$7,%lo(D_800C3E40)($9)\n", "reviewed target pointer publication delay slot")
    text = replace_once(text, "$L6:\n\tlw\t$2,%lo(D_80054AB0)($11)\n\t#nop\n", "$L6:\n\tlw\t$2,%lo(D_80054AB0)($11)\n\taddu\t$3,$8,1\n", "reviewed next-index schedule")
    text = replace_once(text, (
        "\tl.s\t$f2,32($sp)\n\tl.s\t$f0,16($sp)\n\t#nop\n"
        "\tsub.s\t$f2,$f2,$f0\n\tlw\t$3,%lo(D_800C3E40)($9)\n"
        "\tdiv.s\t$f2,$f2,$f4\n\ts.s\t$f2,36($3)\n"
        "\tl.s\t$f2,36($sp)\n\tl.s\t$f0,20($sp)\n\t#nop\n"
        "\tsub.s\t$f2,$f2,$f0\n\taddu\t$7,$7,60\n"
        "\tlui\t$3,%hi(D_80054AB0) # high\n\tdiv.s\t$f2,$f2,$f4\n"
        "\ts.s\t$f2,40($3)\n\tl.s\t$f0,40($sp)\n\tl.s\t$f2,24($sp)\n"
        "\tlw\t$2,%lo(D_80054AB0)($3)\n\tsub.s\t$f0,$f0,$f2\n"
        "\taddu\t$8,$8,1\n\tslt\t$2,$8,$2\n\tdiv.s\t$f0,$f0,$f4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tbne\t$2,$0,$L5\n\ts.s\t$f0,44($3)\n"
    ), (
        "\tl.s\t$f2,32($sp)\n\tl.s\t$f0,16($sp)\n\t#nop\n"
        "\tsub.s\t$f2,$f2,$f0\n\tlw\t$2,%lo(D_800C3E40)($9)\n"
        "\tdiv.s\t$f2,$f2,$f4\n\ts.s\t$f2,36($2)\n"
        "\tl.s\t$f0,36($sp)\n\tl.s\t$f2,20($sp)\n\t#nop\n"
        "\tsub.s\t$f0,$f0,$f2\n\tdiv.s\t$f0,$f0,$f4\n"
        "\ts.s\t$f0,40($2)\n\tl.s\t$f0,40($sp)\n\tl.s\t$f2,24($sp)\n"
        "\tsub.s\t$f0,$f0,$f2\n\tdiv.s\t$f0,$f0,$f4\n\ts.s\t$f0,44($2)\n"
        "\tlui\t$3,%hi(D_80054AB0) # high\n\tlw\t$2,%lo(D_80054AB0)($3)\n"
        "\taddu\t$8,$8,1\n\tslt\t$2,$8,$2\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tbne\t$2,$0,$L5\n\taddu\t$7,$7,60\n"
    ), "reviewed target-motion float and loop-tail schedule")
    return normalize_standard_return(text)


def normalize_random_spin_objects(text: str) -> str:
    """Reproduce the reviewed retail scheduling for the random spin-object initializer."""
    setup_start = text.find("\tmove\t$21,$0\n")
    random_start = text.find("$L6:\n", setup_start)
    if setup_start < 0 or random_start < 0:
        raise ValueError("reviewed random-spin setup span not found")
    setup = (
        "\tmove\t$17,$0\n"
        "\tli\t$22,1717960704\t\t\t# 0x66660000\n"
        "\tori\t$22,$22,0x6667\n"
        "\tmove\t$fp,$16\n"
        "\tlui\t$23,%hi(D_80057D5C) # high\n"
        "\tmove\t$21,$17\n"
        "\tlui\t$3,%hi(D_80054AB0) # high\n"
        "\tli\t$2,3\t\t\t# 0x00000003\n"
        "\tsw\t$2,%lo(D_80054AB0)($3)\n"
        "\tli\t$16,1\t\t\t# 0x00000001\n"
        "$L19:\n"
        "\tlui\t$20,%hi(D_80052F18) # high\n"
        "\tlui\t$2,%hi(D_80054ABC) # high\n"
        "\taddiu\t$19,$2,%lo(D_80054ABC) # low\n"
        "\tli\t$18,413\t\t\t# 0x0000019d\n"
    )
    text = text[:setup_start] + setup + text[random_start:]

    random_start = text.find("$L6:\n") + len("$L6:\n")
    choice_start = text.find("$L17:\n", random_start)
    if choice_start < 0:
        raise ValueError("reviewed random-spin choice span not found")
    random_loop = (
        "\tjal\tfunc_8003A840\n\t#nop\n"
        "\tmult\t$2,$22\n\tsra\t$3,$2,31\n\tmfhi\t$8\n\t#nop\n\t#nop\n"
        "\tsra\t$4,$8,1\n\tsubu\t$7,$4,$3\n"
        "\tsll\t$3,$7,2\n\taddu\t$3,$3,$7\n\tsubu\t$7,$2,$3\n"
        "\taddu\t$4,$7,$fp\n\tlbu\t$2,0($4)\n\t#nop\n"
        "\tbne\t$2,$0,$L6\n\tlw\t$2,%lo(D_80057D5C)($23)\n\t#nop\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbnel\t$2,$16,$L18\n\tli\t$18,1431633920\t\t\t# 0x55550000\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$3,%lo(D_80052F18)($20)\n"
        "\tsll\t$2,$3,2\n\taddu\t$2,$2,$3\n\tsll\t$2,$2,2\n"
        "\taddu\t$2,$2,$19\n\tsll\t$3,$7,2\n\taddu\t$3,$3,$2\n"
        "\tlw\t$2,0($3)\n\t#nop\n"
        "\tbeq\t$2,$18,$L6\n\t#nop\n"
    )
    text = text[:random_start] + random_loop + text[choice_start:]
    text = replace_once(text, "$L17:\n", "\tli\t$18,1431633920\t\t\t# 0x55550000\n$L18:\n", "reviewed random-spin branch-likely constant path")

    choice_start = text.find("$L18:\n") + len("$L18:\n")
    loop_end = text.find("\tlui\t$16,%hi(D_800529F0) # high\n", choice_start)
    if loop_end < 0:
        raise ValueError("reviewed random-spin object loop end not found")
    choice_loop = (
        "\tori\t$18,$18,0x5556\n"
        "\tlui\t$5,%hi(D_800C3E50) # high\n\taddiu\t$5,$5,%lo(D_800C3E50) # low\n"
        "\tsll\t$6,$17,2\n\tlui\t$2,%hi(D_80052F18) # high\n"
        "\tlui\t$3,%hi(D_80054ABC) # high\n\taddiu\t$3,$3,%lo(D_80054ABC) # low\n"
        "\tsb\t$16,0($4)\n\tlw\t$4,%lo(D_80052F18)($2)\n\taddu\t$6,$6,$5\n"
        "\tsll\t$2,$4,2\n\taddu\t$2,$2,$4\n\tsll\t$2,$2,2\n\taddu\t$2,$2,$3\n"
        "\tsll\t$4,$7,2\n\tlui\t$3,%hi(D_800C3E44) # high\n\taddu\t$4,$4,$2\n"
        "\tlh\t$3,%lo(D_800C3E44)($3)\n\tlw\t$2,0($4)\n"
        "\tlui\t$4,%hi(D_80054B08) # high\n\taddu\t$3,$3,-3\n #APP\n #NO_APP\n"
        "\tsubu\t$3,$17,$3\n\tmult\t$3,$18\n"
        "\taddiu\t$4,$4,%lo(D_80054B08) # low\n\taddu\t$4,$21,$4\n\tsw\t$2,0($6)\n"
        "\tsra\t$2,$3,31\n\tmfhi\t$8\n\t#nop\n\t#nop\n"
        "\tsubu\t$5,$8,$2\n\tsll\t$2,$5,1\n\taddu\t$2,$2,$5\n\tsubu\t$5,$3,$2\n"
        "\tlui\t$3,%hi(D_80054AE4) # high\n\taddiu\t$3,$3,%lo(D_80054AE4) # low\n"
        "\tsll\t$2,$5,1\n\taddu\t$2,$2,$5\n\tsll\t$2,$2,2\n\taddu\t$2,$2,$3\n"
        "\tlw\t$3,0($2)\n\tlw\t$5,4($2)\n\tlw\t$7,8($2)\n"
        "\tsw\t$3,0($4)\n\tsw\t$5,4($4)\n\tsw\t$7,8($4)\n"
        "\tlw\t$2,0($6)\n\taddu\t$21,$21,60\n\tsw\t$2,52($4)\n"
        "\tlw\t$4,0($6)\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_800197C8\n\taddu\t$17,$17,1\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tslt\t$2,$17,3\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L19\n\tli\t$16,1\t\t\t# 0x00000001\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
    )
    text = text[:choice_start] + choice_loop + text[loop_end:]

    player_start = text.find("\tlui\t$16,%hi(D_800529F0) # high\n")
    epilogue = text.find("$L1:\n", player_start)
    if player_start < 0 or epilogue < 0:
        raise ValueError("reviewed random-spin player span not found")
    player = (
        "\tlui\t$16,%hi(D_800529F0) # high\n\tlw\t$2,%lo(D_800529F0)($16)\n"
        "\tlw\t$3,284($2)\n\tli\t$2,-1\t\t\t# 0xffffffff\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tbne\t$3,$2,$L12\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tjal\tfunc_8003A840\n\t#nop\n\tmult\t$2,$18\n"
        "\tli\t$3,30\t\t\t# 0x0000001e\n\tlw\t$5,%lo(D_800529F0)($16)\n"
        "\tsra\t$4,$2,31\n\tsh\t$3,404($5)\n\tmfhi\t$8\n\t#nop\n\t#nop\n"
        "\tsubu\t$4,$8,$4\n\tsll\t$3,$4,1\n\taddu\t$3,$3,$4\n"
        "\tsubu\t$2,$2,$3\n\tsw\t$2,412($5)\n"
        "$L12:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001BF00\n\tli\t$17,26\t\t\t# 0x0000001a\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tbne\t$2,$17,$L13\n\t#nop\n\tlw\t$4,%lo(D_800529F0)($16)\n"
        "\tjal\tfunc_80026010\n\t#nop\n\tsll\t$2,$2,16\n\tsra\t$2,$2,16\n"
        "\tli\t$3,3\t\t\t# 0x00000003\n\tbeq\t$2,$3,$L1\n\t#nop\n"
        "$L13:\n\tjal\tfunc_8001BF00\n\t#nop\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$17,$L15\n\tlui\t$2,%hi(D_800529F0) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n"
        "\tlw\t$3,%lo(D_800529F0)($16)\n\tlb\t$2,423($3)\n"
        "\tslt\t$2,$2,6\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$2,$0,$L15\n\tlui\t$2,%hi(D_800529F0) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n"
        "\tlw\t$4,248($3)\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L17\n\taddu\t$4,$4,375\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L15:\n\tlw\t$2,%lo(D_800529F0)($2)\n\tlw\t$4,248($2)\n\taddu\t$4,$4,371\n"
        "$L17:\n\tjal\tfunc_8001F1C4\n\t#nop\n"
    )
    text = text[:player_start] + player + text[epilogue:]
    return normalize_standard_return(text)


def normalize_board_player_input(text: str) -> str:
    """Reproduce the reviewed retail allocation for the board-input unit."""
    text = replace_once(
        text,
        "\taddu\t$5,$2,-525\n\tsubu\t$4,$4,$5\n",
        "\taddu\t$2,$2,-525\n\tsubu\t$4,$4,$2\n",
        "reviewed camera-angle temporary allocation",
    )
    text = replace_once(text, "\tbeq\t$2,$0,$L34\n\tli\t$3,-1240793088\t\t\t# 0xb60b0000\n", "\tbeq\t$2,$0,$L34\n\tli\t$2,-1240793088\t\t\t# 0xb60b0000\n", "reviewed clockwise rotation constant allocation")
    text = replace_once(
        text,
        "\tlw\t$2,296($17)\n\tori\t$3,$3,0x60b7\n"
        "\taddu\t$6,$2,270\n\tmult\t$6,$3\n",
        "\tlw\t$6,296($17)\n\tori\t$2,$2,0x60b7\n"
        "\taddu\t$6,$6,270\n\tmult\t$6,$2\n",
        "reviewed clockwise rotation allocation",
    )
    text = replace_once(
        text,
        "\tli\t$3,-1240793088\t\t\t# 0xb60b0000\n"
        "\tlw\t$2,296($17)\n\tori\t$3,$3,0x60b7\n"
        "\taddu\t$6,$2,90\n\tmult\t$6,$3\n",
        "\tlw\t$6,296($17)\n\tori\t$2,$2,0x60b7\n"
        "\taddu\t$6,$6,90\n\tmult\t$6,$2\n",
        "reviewed counterclockwise rotation allocation",
    )
    text = replace_once(
        text,
        "\tli\t$4,3\t\t\t# 0x00000003\n$L56:\n"
        "\tsra\t$3,$6,31\n",
        "\tli\t$4,3\t\t\t# 0x00000003\n$L56:\n"
        "\tmove\t$5,$17\n\tsra\t$3,$6,31\n",
        "reviewed shared rotation call-argument schedule",
    )
    text = replace_once(text, "\tsubu\t$3,$2,$3\n\tmove\t$5,$17\n", "\tsubu\t$2,$2,$3\n", "reviewed quotient allocation")
    text = replace_once(
        text,
        "\tsll\t$2,$3,1\n\taddu\t$2,$2,$3\n\tsll\t$3,$2,4\n"
        "\tsubu\t$3,$3,$2\n\tsll\t$3,$3,3\n\tsubu\t$3,$6,$3\n",
        "\tsll\t$3,$2,1\n\taddu\t$3,$3,$2\n\tsll\t$2,$3,4\n"
        "\tsubu\t$2,$2,$3\n\tsll\t$2,$2,3\n\tsubu\t$6,$6,$2\n",
        "reviewed modulo remainder allocation",
    )
    text = replace_once(text, "\tsw\t$3,296($17)\n", "\tsw\t$6,296($17)\n", "reviewed rotation result store")
    text = replace_once(
        text,
        "\tbeq\t$2,$0,$L37\n\tandi\t$2,$16,0x0008\n",
        "\tbeq\t$2,$0,$L37\n\tli\t$2,-1240793088\t\t\t# 0xb60b0000\n",
        "reviewed reverse-input branch delay slot",
    )
    text = replace_once(
        text,
        "$L37:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$2,$0,$L39\n\tli\t$2,-1240793088\t\t\t# 0xb60b0000\n",
        "$L37:\n\tandi\t$2,$16,0x0008\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$2,$0,$L39\n\tli\t$2,-1240793088\t\t\t# 0xb60b0000\n",
        "reviewed reverse-input test schedule",
    )
    text = replace_once(
        text,
        "\tlui\t$4,%hi(D_80052F80) # high\n\taddiu\t$4,$4,%lo(D_80052F80) # low\n",
        "\tlui\t$3,%hi(D_80052F80) # high\n\taddiu\t$3,$3,%lo(D_80052F80) # low\n",
        "reviewed direction-table address allocation",
    )
    text = replace_once(
        text,
        "\tmult\t$3,$2\n\tlui\t$3,%hi(D_80052F80) # high\n"
        "\taddiu\t$3,$3,%lo(D_80052F80) # low\n\tmfhi\t$7\n",
        "\tmult\t$3,$2\n\tmfhi\t$7\n",
        "reviewed direction-table address schedule source",
    )
    text = replace_once(
        text,
        "\tandi\t$2,$2,0x0003\n\tsll\t$2,$2,2\n",
        "\tandi\t$2,$2,0x0003\n\tlui\t$3,%hi(D_80052F80) # high\n"
        "\taddiu\t$3,$3,%lo(D_80052F80) # low\n\tsll\t$2,$2,2\n",
        "reviewed direction-table address schedule destination",
    )
    text = replace_once(text, "\taddu\t$2,$2,$4\n\tlw\t$2,0($2)\n", "\taddu\t$2,$2,$3\n\tlw\t$5,0($2)\n", "reviewed direction-table value allocation")
    text = replace_once(
        text,
        "\tbnel\t$18,$2,$L62\n\taddu\t$18,$18,-249\n",
        "\tbne\t$18,$5,$L62\n\taddu\t$2,$18,-249\n",
        "reviewed direction comparison branch",
    )
    text = replace_once(text, "$L46:\n\taddu\t$18,$18,-249\n$L62:\n\tsltu\t$18,$18,4\n", "$L46:\n\taddu\t$2,$18,-249\n$L62:\n\tsltu\t$2,$2,4\n", "reviewed tile-range temporary allocation")
    text = replace_once(text, "\tbeq\t$18,$0,$L60\n", "\tbeq\t$2,$0,$L60\n", "reviewed tile-range branch allocation")
    return normalize_standard_return(text)


def normalize_indexed_resource_pointer(text: str) -> str:
    text = replace_once(
        text,
        "\tlw\t$3,D_80052EC0\n\tlw\t$2,0($2)\n",
        "\tlui\t$3,%hi(D_80052EC0) # high\n\tlw\t$4,0($2)\n"
        "\tlw\t$2,%lo(D_80052EC0)($3)\n",
        "reviewed resource pointer load allocation",
    )
    text = replace_once(
        text,
        "\tj\t$31\n\taddu\t$2,$3,$2\n",
        "\tj\t$L_resource_return\n\taddu\t$2,$2,$4\n",
        "reviewed resource pointer shared return",
    )
    return replace_once(
        text,
        "$L2:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tmove\t$2,$0\n",
        "$L2:\n\tmove\t$2,$0\n$L_resource_return:\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tj\t$31\n\tnop\n",
        "reviewed resource pointer zero return",
    )


def normalize_other_player_vectors(text: str) -> str:
    text = replace_once(
        text,
        "\tmove\t$9,$4\n\tlui\t$6,%hi(D_800544E0) # high\n"
        "\taddiu\t$6,$6,%lo(D_800544E0) # low\n\tmove\t$8,$0\n",
        "\tmove\t$9,$4\n\tmove\t$8,$0\n\tlui\t$2,%hi(D_800544E0) # high\n"
        "\taddiu\t$6,$2,%lo(D_800544E0) # low\n",
        "reviewed player-vector loop setup",
    )
    text = replace_once(
        text,
        "\tbne\t$8,$9,$L6\n\taddu\t$7,$6,24\n",
        "\tbeq\t$8,$9,$L6\n\taddu\t$7,$6,12\n",
        "reviewed player-vector source branch",
    )
    text = replace_once(text, "\taddu\t$7,$6,12\n$L6:\n", "\taddu\t$7,$6,24\n$L6:\n", "reviewed alternate player-vector source")
    return replace_once(
        text,
        "\tlw\t$3,0($7)\n\tlw\t$5,4($7)\n\tlw\t$10,8($7)\n"
        "\tsw\t$3,0($6)\n\tsw\t$5,4($6)\n\tsw\t$10,8($6)\n",
        "\tlw\t$2,0($7)\n\tlw\t$3,4($7)\n\tlw\t$4,8($7)\n"
        "\tsw\t$2,0($6)\n\tsw\t$3,4($6)\n\tsw\t$4,8($6)\n",
        "reviewed player-vector copy allocation",
    )


def normalize_room_grid_tile(text: str) -> str:
    text = replace_once(
        text,
        "\tsubu\t$sp,$sp,24\n\tlui\t$3,%hi(D_800581F0) # high\n"
        "\tandi\t$4,$4,0x00ff\n\tsll\t$2,$4,5\n\taddu\t$2,$2,$4\n"
        "\tsll\t$2,$2,4\n\tsll\t$7,$5,16\n\tsll\t$5,$6,16\n"
        "\tlw\t$3,%lo(D_800581F0)($3)\n\tsra\t$5,$5,16\n"
        "\tsw\t$31,16($sp)\n\taddu\t$3,$3,$2\n\tlhu\t$2,8214($3)\n"
        "\tsra\t$7,$7,16\n\taddu\t$5,$5,$2\n\tsll\t$5,$5,16\n"
        "\tlhu\t$2,8212($3)\n\tsra\t$5,$5,16\n\taddu\t$4,$7,$2\n"
        "\tsll\t$4,$4,16\n",
        "\tsubu\t$sp,$sp,24\n\tsll\t$7,$5,16\n"
        "\tlui\t$3,%hi(D_800581F0) # high\n\tandi\t$4,$4,0x00ff\n"
        "\tsll\t$2,$4,5\n\taddu\t$2,$2,$4\n\tsll\t$2,$2,4\n"
        "\tsra\t$7,$7,16\n\tlw\t$3,%lo(D_800581F0)($3)\n"
        "\tsll\t$5,$6,16\n\tsw\t$31,16($sp)\n\taddu\t$3,$3,$2\n"
        "\tlh\t$2,8212($3)\n\tsra\t$5,$5,16\n\taddu\t$7,$7,$2\n"
        "\tsll\t$7,$7,16\n\tlh\t$2,8214($3)\n\tsra\t$4,$7,16\n"
        "\taddu\t$5,$5,$2\n\tsll\t$5,$5,16\n",
        "reviewed room-grid coordinate allocation and schedule",
    )
    return replace_once(
        text,
        "\tjal\tfunc_8000DF14\n\tsra\t$4,$4,16\n",
        "\tjal\tfunc_8000DF14\n\tsra\t$5,$5,16\n",
        "reviewed room-grid call delay slot",
    )


def normalize_zone_choice_input(text: str) -> str:
    text = text.replace("$17", "$sn_swap").replace("$18", "$17").replace("$sn_swap", "$18")
    text = replace_once(
        text,
        "\tsw\t$17,24($sp)\n\tmove\t$17,$4\n"
        "\tsw\t$18,20($sp)\n\tmove\t$18,$5\n",
        "\tsw\t$17,20($sp)\n\tmove\t$17,$4\n"
        "\tsw\t$18,24($sp)\n\tmove\t$18,$5\n",
        "reviewed zone-choice saved-register setup",
    )
    text = replace_once(
        text,
        "\tli\t$16,1\t\t\t# 0x00000001\n\tlw\t$2,280($4)\n"
        "\tlhu\t$3,D_800B9296\n\tori\t$2,$2,0x0800\n"
        "\taddu\t$3,$3,$16\n\tsw\t$2,280($4)\n",
        "\tlw\t$2,280($17)\n\tori\t$2,$2,0x0800\n\tsw\t$2,280($4)\n"
        "\tlhu\t$2,D_800B9296\n\tli\t$16,1\t\t\t# 0x00000001\n"
        "\taddu\t$2,$2,1\n",
        "reviewed zone-choice flag and timer schedule",
    )
    text = replace_once(text, "\tsh\t$3,340($4)\n", "\tsh\t$2,340($4)\n", "reviewed zone-choice timer allocation")
    text = replace_once(
        text,
        "\tlw\t$17,24($sp)\n\tlw\t$18,20($sp)\n",
        "\tlw\t$18,24($sp)\n\tlw\t$17,20($sp)\n",
        "reviewed zone-choice saved-register restore",
    )
    return normalize_standard_return(text)


def normalize_round_state_reset(text: str) -> str:
    text = replace_once(
        text,
        "\taddiu\t$5,$2,%lo(D_80052F4C) # low\n"
        "\tlui\t$2,%hi(D_80052F18) # high\n"
        "\taddiu\t$4,$2,%lo(D_80052F18) # low\n\taddu\t$6,$4,48\n",
        "\taddiu\t$7,$2,%lo(D_80052F4C) # low\n"
        "\tlui\t$2,%hi(D_80052F18) # high\n"
        "\taddiu\t$6,$2,%lo(D_80052F18) # low\n\taddu\t$8,$6,48\n",
        "reviewed round-state copy pointer allocation",
    )
    text = replace_once(
        text,
        "$L2:\n\tlw\t$7,0($4)\n\tlw\t$8,4($4)\n"
        "\tlw\t$9,8($4)\n\tlw\t$10,12($4)\n"
        "\tsw\t$7,0($5)\n\tsw\t$8,4($5)\n"
        "\tsw\t$9,8($5)\n\tsw\t$10,12($5)\n\taddu\t$4,$4,16\n",
        "$L2:\n\tlw\t$2,0($6)\n\tlw\t$3,4($6)\n"
        "\tlw\t$4,8($6)\n\tlw\t$5,12($6)\n"
        "\tsw\t$2,0($7)\n\tsw\t$3,4($7)\n"
        "\tsw\t$4,8($7)\n\tsw\t$5,12($7)\n\taddu\t$6,$6,16\n",
        "reviewed round-state copy word allocation",
    )
    text = replace_once(text, "\tbne\t$4,$6,$L2\n\taddu\t$5,$5,16\n", "\tbne\t$6,$8,$L2\n\taddu\t$7,$7,16\n", "reviewed round-state copy loop allocation")
    text = replace_once(text, "\tlw\t$7,0($4)\n\tsw\t$7,0($5)\n", "\tlw\t$2,0($6)\n\tsw\t$2,0($7)\n", "reviewed round-state copy tail allocation")
    text = replace_once(
        text,
        "\tlui\t$16,%hi(D_80052F18) # high\n\tli\t$17,1\t\t\t# 0x00000001\n"
        "\tlw\t$2,0($6)\n\tsw\t$2,0($7)\n",
        "\tlw\t$2,0($6)\n\tsw\t$2,0($7)\n"
        "\tlui\t$16,%hi(D_80052F18) # high\n\tli\t$17,1\t\t\t# 0x00000001\n",
        "reviewed round-state copy tail schedule",
    )
    text = replace_once(text, "\tmfhi\t$7\n\t#nop\n\t#nop\n\tsubu\t$4,$7,$4\n\tsll\t$3,$4,$17\n", "\tmfhi\t$5\n\t#nop\n\t#nop\n\tsubu\t$4,$5,$4\n\tsll\t$3,$4,1\n", "reviewed modulo-three allocation")
    text = replace_once(text, "\tsra\t$3,$3,$17\n\tsll\t$3,$3,$17\n", "\tsra\t$3,$3,1\n\tsll\t$3,$3,1\n", "reviewed modulo-two shifts")
    text = replace_once(
        text,
        "\tlui\t$3,%hi(D_80058BF0) # high\n\tlw\t$31,24($sp)\n"
        "\tli\t$2,10\t\t\t# 0x0000000a\n\tsw\t$2,%lo(D_80058BF0)($3)\n"
        "\tlui\t$2,%hi(D_80052E98) # high\n\tsw\t$17,%lo(D_80052E98)($2)\n",
        "\tlui\t$3,%hi(D_80058BF0) # high\n"
        "\tli\t$2,10\t\t\t# 0x0000000a\n\tsw\t$2,%lo(D_80058BF0)($3)\n"
        "\tlui\t$2,%hi(D_80052E98) # high\n\tsw\t$17,%lo(D_80052E98)($2)\n"
        "\tlw\t$31,24($sp)\n",
        "reviewed round-state epilogue schedule",
    )
    return normalize_standard_return(text)


def normalize_round_controller_create(text: str) -> str:
    text = replace_once(
        text,
        "\tli\t$4,131\t\t\t# 0x00000083\n"
        "\tlui\t$2,%hi(D_80052F18) # high\n",
        "\tlui\t$2,%hi(D_80052F18) # high\n",
        "reviewed round-controller default resource schedule source",
    )
    text = replace_once(
        text,
        "\tbne\t$3,$2,$L2\n\tli\t$17,526\t\t\t# 0x0000020e\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli\t$4,133\t\t\t# 0x00000085\n$L2:\n",
        "\tbne\t$3,$2,$L2\n\tli\t$4,131\t\t\t# 0x00000083\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli\t$4,133\t\t\t# 0x00000085\n$L2:\n"
        "\tli\t$17,526\t\t\t# 0x0000020e\n",
        "reviewed round-controller resource selection schedule",
    )
    text = replace_once(text, "\tjal\tfunc_8001EEBC\n\tli\t$4,526\t\t\t# 0x0000020e\n", "\tjal\tfunc_8001EEBC\n\tmove\t$4,$17\n", "reviewed shared resource argument allocation")
    text = replace_once(text, "\tmove\t$4,$16\n$L4:\n", "\tmove\t$4,$16\n$L4:\n\tmove\t$5,$18\n", "reviewed controller callback arguments")
    text = replace_once(
        text,
        "\tlw\t$3,292($4)\n\tli\t$2,5\t\t\t# 0x00000005\n"
        "\tsw\t$2,252($4)\n\tlui\t$2,%hi(D_80058C00) # high\n"
        "\tsw\t$4,%lo(D_80058C00)($2)\n\tmove\t$7,$3\n"
        "\tsw\t$7,300($4)\n\tmove\t$7,$0\n",
        "\tli\t$2,5\t\t\t# 0x00000005\n\tsw\t$2,252($4)\n"
        "\tlui\t$2,%hi(D_80058C00) # high\n\tlw\t$3,292($4)\n"
        "\tmove\t$7,$0\n\tsw\t$3,300($4)\n",
        "reviewed round-controller position copy schedule",
    )
    text = replace_once(text, "\tjal\tfunc_80007EFC\n\tsw\t$3,296($4)\n", "\tsw\t$3,296($4)\n\t.set\tnoreorder\n\t.set\tnomacro\n\tjal\tfunc_80007EFC\n\tsw\t$4,%lo(D_80058C00)($2)\n", "reviewed round-controller publish delay slot")
    return normalize_standard_return(text)


def normalize_zero_copy_with_padding(
    text: str, old: str, new: str, description: str
) -> str:
    """Preserve a reviewed zero-copy source and the retail trailing pad word."""
    text = replace_once(text, old, new, description)
    text = normalize_standard_return(text)
    end = re.compile(
        r"(?P<prefix>\t\.set\tmacro\n\t\.set\treorder\n\n)"
        r"(?P<end>\t\.end\tfunc_[0-9A-F]+\n)"
    )
    match = end.search(text)
    if match is None:
        raise ValueError("reviewed trailing padding location not found")
    return end.sub(
        match.group("prefix") + "\tnop\n" + match.group("end"), text, count=1
    )


def normalize_available_position_search(text: str) -> str:
    return normalize_zero_copy_with_padding(
        text,
        "\tmove\t$18,$0\n",
        "\tmove\t$18,$16\n",
        "reviewed available-position zero-copy pattern",
    )


def normalize_completion_state_scan(text: str) -> str:
    return normalize_zero_copy_with_padding(
        text,
        "\tmove\t$18,$17\n",
        "\tmove\t$18,$16\n",
        "reviewed completion-state zero-copy pattern",
    )


def normalize_tile_collision_search(text: str) -> str:
    text = replace_once(
        text,
        "\tlw\t$3,316($2)\n\tlw\t$2,316($18)\n",
        "\tlw\t$3,316($18)\n\tlw\t$2,316($2)\n",
        "reviewed commutative tile comparison load order",
    )
    return normalize_zero_copy_with_padding(
        text,
        "\tmove\t$22,$19\n",
        "\tmove\t$22,$17\n",
        "reviewed tile-search zero-copy pattern",
    )


def normalize_packed_state_dispatch(text: str) -> str:
    return normalize_zero_copy_with_padding(
        text,
        "\tlhu\t$4,%lo(D_800544D4)($2)\n"
        "\tlhu\t$2,2($3)\n"
        "\tsll\t$4,$4,16\n"
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "\tjal\tfunc_8002C714\n"
        "\tor\t$4,$4,$2\n",
        "\tlhu\t$2,%lo(D_800544D4)($2)\n"
        "\tlhu\t$4,2($3)\n"
        "\tsll\t$2,$2,16\n"
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "\tjal\tfunc_8002C714\n"
        "\tor\t$4,$2,$4\n",
        "reviewed packed-state operand allocation pattern",
    )


def normalize_menu_entry_reset(text: str) -> str:
    text = replace_once(
        text,
        "\tbeq\t$5,$0,$L2\n"
        "\tmove\t$17,$0\n"
        "\t.set\tmacro\n"
        "\t.set\treorder\n\n"
        "\tli\t$2,2\t\t\t# 0x00000002\n",
        "\tbeq\t$5,$0,$L2\n"
        "\tli\t$2,2\t\t\t# 0x00000002\n"
        "\t.set\tmacro\n"
        "\t.set\treorder\n\n",
        "reviewed menu-state branch scheduling pattern",
    )
    text = replace_once(
        text,
        "\tslt\t$2,$17,$2\n"
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "\tbeq\t$2,$0,$L11\n"
        "\tli\t$20,-1\t\t\t# 0xffffffff\n"
        "\t.set\tmacro\n"
        "\t.set\treorder\n",
        "\tslt\t$2,$0,$2\n"
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "\tbeq\t$2,$0,$L11\n"
        "\tmove\t$17,$0\n"
        "\t.set\tmacro\n"
        "\t.set\treorder\n\n"
        "\tli\t$20,-1\t\t\t# 0xffffffff\n",
        "reviewed menu-loop entry scheduling pattern",
    )
    text = normalize_standard_return(text)
    end = re.compile(r"(?P<prefix>\t\.set\tmacro\n\t\.set\treorder\n\n)(?P<end>\t\.end\tfunc_80016DB0\n)")
    match = end.search(text)
    if match is None:
        raise ValueError("reviewed menu-entry trailing padding location not found")
    return end.sub(
        match.group("prefix") + "\tnop\n" + match.group("end"), text, count=1
    )


def normalize_wrapped_actor_index(text: str) -> str:
    text = replace_once(
        text,
        "\taddu\t$4,$4,$fp\n",
        "\taddiu\t$4,$4,1\n",
        "reviewed actor-index increment constant",
    )
    text = replace_once(
        text,
        "\tsll\t$2,$16,$fp\n",
        "\tsll\t$2,$16,1\n",
        "reviewed actor-index scale-by-two constant",
    )
    return normalize_zero_copy_with_padding(
        text,
        "\tsll\t$2,$2,$23\n",
        "\tsll\t$2,$2,3\n",
        "reviewed actor-index scale-by-eight constant",
    )


def normalize_sprite_display_list(text: str) -> str:
    def swap_registers(span: str, left: str, right: str) -> str:
        marker = "$@sprite_swap"
        return span.replace(left, marker).replace(right, left).replace(marker, right)

    case8 = text.find("$L5:\n")
    case16 = text.find("$L15:\n", case8)
    end = text.rfind("\tlw\t$22,24($sp)\n")
    if case8 < 0 or case16 < 0 or end <= case16:
        raise ValueError("reviewed sprite case spans not found")

    ci8 = text[case8:case16]
    ci8 = replace_once(ci8,
        "\tbeq\t$2,$0,$L4\n\taddu\t$11,$25,24\n",
        "\tbeq\t$2,$0,$L4\n\tmove\t$11,$15\n",
        "reviewed CI8 loop-entry scheduling")
    ci8 = replace_once(ci8, "\tmove\t$10,$15\n", "",
                       "reviewed CI8 global-address move")
    ci8 = replace_once(ci8,
        "\taddu\t$3,$13,-1\n\tsll\t$3,$3,2\n\tandi\t$3,$3,0x0fff\n"
        "\tsll\t$3,$3,12\n\tmult\t$13,$14\n\taddu\t$2,$14,-1\n"
        "\tsll\t$2,$2,2\n\tandi\t$2,$2,0x0fff\n\tor\t$16,$3,$2\n",
        "\taddu\t$2,$13,-1\n\tsll\t$2,$2,2\n\tandi\t$2,$2,0x0fff\n"
        "\tsll\t$2,$2,12\n\taddu\t$3,$14,-1\n\tmult\t$13,$14\n"
        "\tsll\t$3,$3,2\n\tandi\t$3,$3,0x0fff\n\tor\t$16,$2,$3\n"
        "\taddu\t$10,$25,24\n",
        "reviewed CI8 tile-word allocation and part scheduling")
    loop = ci8.find("$L7:\n")
    if loop < 0:
        raise ValueError("reviewed CI8 loop not found")
    ci8 = ci8[:loop] + swap_registers(ci8[loop:], "$10", "$11")
    ci8 = replace_once(ci8, "\taddu\t$24,$24,1\n", "", "reviewed CI8 counter schedule")
    ci8 = replace_once(ci8,
        "\tori\t$9,$9,0x0400\n\tlw\t$5,%lo(D_8007784C)($11)\n",
        "\tori\t$9,$9,0x0400\n\taddu\t$24,$24,1\n"
        "\tlw\t$5,%lo(D_8007784C)($11)\n",
        "reviewed CI8 counter schedule destination")

    rgba = text[case16:end]
    rgba = replace_once(rgba, "\tmove\t$12,$15\n", "", "reviewed RGBA16 global move")
    rgba = replace_once(rgba,
        "\tbeq\t$2,$0,$L4\n\taddu\t$11,$25,24\n",
        "\tbeq\t$2,$0,$L4\n\tmove\t$12,$15\n",
        "reviewed RGBA16 loop-entry scheduling")
    rgba = replace_once(rgba,
        "\tmult\t$13,$14\n\taddu\t$2,$14,-1\n",
        "\taddu\t$2,$14,-1\n\tmult\t$13,$14\n",
        "reviewed RGBA16 multiply scheduling")
    rgba = replace_once(rgba, "\tor\t$18,$3,$2\n\tmflo\t$16\n",
        "\tor\t$18,$3,$2\n\taddu\t$11,$25,24\n\tmflo\t$16\n",
        "reviewed RGBA16 part-pointer scheduling")
    rgba = replace_once(rgba,
        "\tandi\t$13,$17,0x00ff\n\tli\t$15,-183500800\t\t\t# 0xf5100000\n",
        "\tli\t$15,-183500800\t\t\t# 0xf5100000\n\tandi\t$13,$17,0x00ff\n",
        "reviewed RGBA16 format-command scheduling")
    rgba = replace_once(rgba,
        "\tsra\t$10,$10,2\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tblez\t$10,$L21\n\tli\t$5,2048\t\t\t# 0x00000800\n",
        "\tsra\t$2,$10,2\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tblez\t$2,$L21\n\tli\t$4,2048\t\t\t# 0x00000800\n",
        "reviewed RGBA16 quarter allocation")
    rgba = replace_once(rgba, "\taddu\t$5,$10,2047\n", "\taddu\t$4,$2,2047\n",
                        "reviewed RGBA16 dxt allocation")
    rgba = replace_once(rgba, "\tmove\t$4,$13\n", "\tmove\t$5,$13\n",
                        "reviewed RGBA16 divisor copy")
    rgba = replace_once(rgba, "\taddu\t$4,$13,3\n$L23:\n\tsra\t$4,$4,2\n",
        "\taddu\t$5,$13,3\n$L23:\n\tsra\t$2,$5,2\n",
        "reviewed RGBA16 signed divisor")
    rgba = replace_once(rgba,
        "\tblez\t$4,$L25\n\tandi\t$2,$5,0x0fff\n",
        "\tblezl\t$2,$L25\n\tandi\t$2,$4,0x0fff\n",
        "reviewed RGBA16 divide branch")
    rgba = replace_once(rgba, "\tdiv\t$2,$5,$4\n", "\tdiv\t$2,$4,$2\n",
                        "reviewed RGBA16 division allocation")
    rgba = replace_once(rgba, "$L25:\n\taddu\t$24,$24,1\n", "$L25:\n"
        "\tor\t$2,$6,$2\n\tsw\t$2,4($7)\n",
        "reviewed RGBA16 result-store schedule")
    rgba = replace_once(rgba,
        "\tlw\t$5,%lo(D_8007784C)($12)\n\tor\t$2,$6,$2\n\tsw\t$2,4($7)\n",
        "\taddu\t$24,$24,1\n\tlw\t$5,%lo(D_8007784C)($12)\n",
        "reviewed RGBA16 result-store schedule destination")
    return normalize_standard_return(text[:case8] + ci8 + rgba + text[end:])

    # Legacy reconstruction shapes below remain documented for comparison.
    text = replace_once(
        text,
        "\tsw\t$17,4($sp)\n"
        "\tli\t$17,8\t\t\t# 0x00000008\n"
        "\tsw\t$22,24($sp)\n"
        "\tsw\t$21,20($sp)\n"
        "\tsw\t$20,16($sp)\n"
        "\tsw\t$19,12($sp)\n"
        "\tsw\t$18,8($sp)\n",
        "\tsw\t$18,8($sp)\n"
        "\tli\t$18,8\t\t\t# 0x00000008\n"
        "\tsw\t$22,24($sp)\n"
        "\tsw\t$21,20($sp)\n"
        "\tsw\t$20,16($sp)\n"
        "\tsw\t$19,12($sp)\n"
        "\tsw\t$17,4($sp)\n",
        "reviewed sprite prologue register allocation",
    )
    start = text.find("\tlhu\t$2,14($25)\n")
    case8 = text.find("$L5:\n", start)
    case16 = text.find("$L15:\n", case8)
    end = text.rfind("\tlw\t$22,24($sp)\n")
    if start < 0 or case8 < 0 or case16 < 0 or end <= case16:
        raise ValueError("reviewed sprite register-allocation span not found")

    def rename(span: str, pairs: tuple[tuple[str, str], ...]) -> str:
        for number, (old, _) in enumerate(pairs):
            span = span.replace(old, f"$@sprite_{number}")
        for number, (_, new) in enumerate(pairs):
            span = span.replace(f"$@sprite_{number}", new)
        return span

    setup = rename(text[start:case8], (("$17", "$18"), ("$18", "$17")))
    ci8 = text[case8:case16]
    ci8 = replace_once(
        ci8,
        "\tbeq\t$2,$0,$L4\n\taddu\t$11,$25,24\n",
        "\tbeq\t$2,$0,$L4\n\tmove\t$11,$15\n",
        "reviewed CI8 loop-entry scheduling",
    )
    ci8 = replace_once(
        ci8,
        "\tmove\t$10,$15\n",
        "",
        "reviewed CI8 global-address move",
    )
    ci8 = replace_once(
        ci8,
        "\taddu\t$3,$13,-1\n"
        "\tsll\t$3,$3,2\n"
        "\tandi\t$3,$3,0x0fff\n"
        "\tsll\t$3,$3,12\n"
        "\tmult\t$13,$14\n"
        "\taddu\t$2,$14,-1\n"
        "\tsll\t$2,$2,2\n"
        "\tandi\t$2,$2,0x0fff\n"
        "\tor\t$16,$3,$2\n",
        "\taddu\t$2,$13,-1\n"
        "\tsll\t$2,$2,2\n"
        "\tandi\t$2,$2,0x0fff\n"
        "\tsll\t$2,$2,12\n"
        "\taddu\t$3,$14,-1\n"
        "\tmult\t$13,$14\n"
        "\tsll\t$3,$3,2\n"
        "\tandi\t$3,$3,0x0fff\n"
        "\tor\t$16,$2,$3\n",
        "reviewed CI8 tile-word register allocation",
    )
    ci8 = replace_once(
        ci8,
        "\tor\t$16,$2,$3\n",
        "\tor\t$16,$2,$3\n\taddu\t$10,$25,24\n",
        "reviewed CI8 part-pointer scheduling",
    )
    loop = ci8.find("$L7:\n")
    if loop < 0:
        raise ValueError("reviewed CI8 loop label not found")
    ci8 = rename(ci8[:loop], (("$18", "$17"),)) + ci8[loop:]
    load_end_marker = "$L13:\n"
    load_end = ci8.find(load_end_marker, loop)
    if load_end < 0:
        raise ValueError("reviewed CI8 load-block end not found")
    load_end += len(load_end_marker)
    ci8 = ci8[:loop] + rename(
        ci8[loop:load_end],
        (("$10", "$11"), ("$11", "$10")),
    ) + rename(
        ci8[load_end:],
        (("$10", "$11"), ("$11", "$10")),
    )
    body = setup + ci8 + text[case16:end]
    return normalize_standard_return(text[:start] + body + text[end:])


def normalize_sprite_glyph_renderer(text: str) -> str:
    def rename(span: str, mapping: dict[str, str]) -> str:
        for index, old in enumerate(mapping):
            span = span.replace(old, f"@@glyph_{index}@@")
        for index, new in enumerate(mapping.values()):
            span = span.replace(f"@@glyph_{index}@@", new)
        return span

    start = text.find("\tlhu\t$2,14($25)\n")
    case8 = text.find("$L7:\n", start)
    case16 = text.find("$L3:\n", case8)
    end = text.rfind("$L13:\n")
    if min(start, case8, case16, end) < 0:
        raise ValueError("reviewed glyph-renderer spans not found")

    setup = rename(text[start:case8], {
        "$22": "$21", "$17": "$24", "$19": "$18",
    })
    setup = replace_once(setup, "\tmove\t$13,$14\n", "\tmove\t$10,$14\n",
                         "reviewed glyph CI8 global base")
    setup = replace_once(setup, "\tli\t$14,24\t\t\t# 0x00000018\n", "",
                         "reviewed glyph CI8 pointer constant")
    ci8 = rename(text[case8:case16], {
        "$22": "$21", "$17": "$24", "$19": "$18", "$18": "$17",
        "$20": "$19", "$13": "$10", "$15": "$14", "$24": "$15",
    })
    ci8 = replace_once(ci8,
        "$L7:\n\taddu\t$11,$25,$14\n",
        "\taddu\t$11,$25,24\n$L7:\n",
        "reviewed glyph CI8 pointer setup")
    ci8 = replace_once(ci8, "\tli\t$8,2048\t\t\t# 0x00000800\n",
        "\tli\t$9,2048\t\t\t# 0x00000800\n", "reviewed glyph CI8 dxt register")
    ci8 = replace_once(ci8, "\taddu\t$9,$4,16\n", "\taddu\t$8,$4,16\n",
                       "reviewed glyph CI8 load pointer")
    ci8 = replace_once(ci8, "\tsw\t$9,%lo(D_80077830)($10)\n",
        "\tsw\t$8,%lo(D_80077830)($10)\n", "reviewed glyph CI8 load pointer store")
    ci8 = replace_once(ci8, "\taddu\t$14,$14,16\n", "\taddu\t$11,$11,16\n",
                       "reviewed glyph CI8 pointer advance")
    ci8 = replace_once(ci8,
        "\tandi\t$10,$16,0x00ff\n\tsrl\t$12,$10,3\n"
        "\tli\t$2,-49283072\t\t\t# 0xfd100000\n",
        "\tli\t$2,-49283072\t\t\t# 0xfd100000\n",
        "reviewed glyph CI8 width calculation schedule")
    ci8 = replace_once(ci8,
        "\tbeq\t$12,$0,$L8\n\tsw\t$2,16($4)\n",
        "\tsw\t$2,16($4)\n\tandi\t$2,$16,0x00ff\n\tsrl\t$5,$2,3\n"
        "\tbeq\t$5,$0,$L8\n\tmove\t$6,$8\n",
        "reviewed glyph CI8 load-block setup")
    ci8 = replace_once(ci8, "\taddu\t$8,$12,2047\n$L8:\n\tandi\t$2,$17,0x00ff\n"
        "\tmult\t$2,$10\n", "\taddu\t$9,$5,2047\n$L8:\n\tmult\t$17,$2\n",
        "reviewed glyph CI8 load-block arithmetic")
    ci8 = replace_once(ci8,
        "\tbeq\t$12,$0,$L10\n\tor\t$4,$2,$3\n",
        "\tbeq\t$5,$0,$L10\n\tor\t$4,$2,$3\n",
        "reviewed glyph CI8 divide guard")
    ci8 = replace_once(ci8, "\tdiv\t$2,$8,$12\n", "\tdiv\t$2,$9,$5\n",
                       "reviewed glyph CI8 division registers")
    ci8 = replace_once(ci8,
        "\tj\t$L26\n\tor\t$2,$4,$2\n",
        "\tor\t$2,$4,$2\n\tj\t$L26\n\tsw\t$2,4($6)\n",
        "reviewed glyph CI8 quotient store")
    ci8 = replace_once(ci8,
        "$L10:\n\tor\t$2,$4,$8\n$L26:\n\tsw\t$2,4($9)\n",
        "$L10:\n\tor\t$2,$4,$9\n\tsw\t$2,4($8)\n$L26:\n",
        "reviewed glyph CI8 fallback store")

    rgba_source = text[case16:end]
    loop_start = rgba_source.find("\tmove\t$13,$14\n")
    if loop_start < 0:
        raise ValueError("reviewed glyph RGBA loop setup not found")
    rgba_setup = rename(rgba_source[:loop_start], {
        "$22": "$21", "$17": "$24", "$19": "$18", "$18": "$17",
        "$20": "$19", "$21": "$20",
    })
    rgba_loop = rename(rgba_source[loop_start:], {
        "$22": "$21", "$17": "$24", "$19": "$18", "$18": "$17",
        "$20": "$19", "$21": "$20", "$14": "$12", "$13": "$11",
        "$11": "$13", "$15": "$14", "$24": "$15",
    })
    rgba = rgba_setup + rgba_loop
    rgba = replace_once(rgba, "\tmove\t$11,$12\n", "\tmove\t$11,$14\n",
                        "reviewed glyph RGBA global base")
    rgba = replace_once(rgba, "\tli\t$16,24\t\t\t# 0x00000018\n", "",
                        "reviewed glyph RGBA pointer constant")
    rgba = replace_once(rgba, "\taddu\t$13,$25,$16\n", "\taddu\t$13,$25,24\n",
                        "reviewed glyph RGBA pointer setup")
    rgba = replace_once(rgba, "\taddu\t$13,$25,24\n", "",
                        "reviewed glyph RGBA pointer schedule source")
    rgba = replace_once(rgba, "\tor\t$20,$3,$2\n\tmult\t$12,$17\n",
        "\tor\t$20,$3,$2\n\taddu\t$13,$25,24\n\tmult\t$12,$17\n",
        "reviewed glyph RGBA pointer schedule destination")
    rgba = replace_once(rgba, "\tmove\t$9,$12\n\tmove\t$10,$9\n",
        "\tandi\t$9,$16,0x00ff\n\tmove\t$10,$9\n",
        "reviewed glyph RGBA quarter input")
    rgba = replace_once(rgba,
        "\tsra\t$10,$10,2\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tblez\t$10,$L20\n\tli\t$8,2048\t\t\t# 0x00000800\n",
        "\tsra\t$2,$10,2\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tblez\t$2,$L20\n\tli\t$4,2048\t\t\t# 0x00000800\n",
        "reviewed glyph RGBA quarter allocation")
    rgba = replace_once(rgba, "\taddu\t$8,$10,2047\n", "\taddu\t$4,$2,2047\n",
                        "reviewed glyph RGBA dxt allocation")
    rgba = replace_once(rgba,
        "\tbgez\t$12,$L22\n\tor\t$12,$2,$3\n",
        "\tbgez\t$12,$L22\n\tor\t$6,$2,$3\n",
        "reviewed glyph RGBA packed word")
    rgba = replace_once(rgba, "$L21:\n", "$L21:\n\tmove\t$5,$12\n",
                        "reviewed glyph RGBA divisor schedule")
    rgba = replace_once(rgba, "\taddu\t$9,$12,3\n$L22:\n\tsra\t$9,$9,2\n",
        "\taddu\t$5,$12,3\n$L22:\n\tsra\t$2,$5,2\n",
        "reviewed glyph RGBA signed divisor")
    rgba = replace_once(rgba,
        "\tblez\t$9,$L24\n\tandi\t$2,$8,0x0fff\n",
        "\tblezl\t$2,$L24\n\tandi\t$2,$4,0x0fff\n",
        "reviewed glyph RGBA divide branch")
    rgba = replace_once(rgba, "\tdiv\t$2,$8,$9\n", "\tdiv\t$2,$4,$2\n",
                        "reviewed glyph RGBA division registers")
    rgba = replace_once(rgba, "$L24:\n", "$L24:\n\tor\t$2,$6,$2\n\tsw\t$2,4($7)\n",
                        "reviewed glyph RGBA result store")
    rgba = replace_once(rgba,
        "\tlw\t$5,%lo(D_80077830)($11)\n\tor\t$2,$12,$2\n\tsw\t$2,4($7)\n",
        "\tlw\t$5,%lo(D_80077830)($11)\n",
        "reviewed glyph RGBA late result store removal")
    rgba = replace_once(rgba, "\taddu\t$16,$16,16\n", "\taddu\t$13,$13,16\n",
                        "reviewed glyph RGBA pointer advance")
    rgba = rgba.replace("\tor\t$fp,$2,$18\n", "\tor\t$22,$2,$18\n")
    rgba = rgba.replace("\tsw\t$fp,0($5)\n", "\tsw\t$22,0($5)\n")

    return normalize_standard_return(text[:start] + setup + ci8 + rgba + text[end:])


def normalize_resource_range(text: str) -> str:
    text = replace_once(text, (
        "\tlui\t$8,%hi(D_80052EB0) # high\n"
        "\tlui\t$3,%hi(D_80058C40) # high\n"
    ), (
        "\tlui\t$3,%hi(D_80058C40) # high\n"
        "\tlui\t$8,%hi(D_80052EB0) # high\n"
    ), "reviewed resource-range address scheduling")
    return normalize_standard_return(text)


def normalize_runtime_callback_setup(text: str) -> str:
    return replace_once(text, (
        "\tlw\t$31,16($sp)\n"
        "\tlui\t$2,%hi(D_800545E4) # high\n"
        "\tsw\t$0,%lo(D_800545E4)($2)\n"
    ), (
        "\tlui\t$2,%hi(D_800545E4) # high\n"
        "\tlw\t$31,16($sp)\n"
        "\tsw\t$0,%lo(D_800545E4)($2)\n"
    ), "reviewed callback-setup epilogue scheduling")


def normalize_state_finish_branch(text: str) -> str:
    text = replace_once(text, "\tbeq\t$3,$2,$L2\n", "\tbne\t$3,$2,$L2\n",
                        "reviewed state-finish branch sense")
    return normalize_standard_return(text)


def normalize_effect_loop_branch(text: str) -> str:
    text = replace_once(text, "\tbnel\t$2,$0,$L8\n", "\tbne\t$2,$0,$L8\n",
                        "reviewed effect-loop branch form")
    return normalize_standard_return(text)


def normalize_transition_state(text: str) -> str:
    text = replace_once(text, "\tslt\t$2,$3,4\n", "\tsltu\t$2,$3,4\n",
                        "reviewed transition-state unsigned comparison")
    return normalize_standard_return(text)


def normalize_object_cleanup(text: str) -> str:
    text = replace_once(text, (
        "\tlui\t$5,%hi(D_80057D58) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$4,%hi(D_80057D5C) # high\n"
    ), (
        "\tlui\t$4,%hi(D_80057D5C) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$5,%hi(D_80057D58) # high\n"
    ), "reviewed object-cleanup address scheduling")
    return normalize_standard_return(text)


def normalize_text_width_accumulation(text: str) -> str:
    text = replace_once(text, (
        "\taddu\t$2,$2,4\n\taddu\t$18,$18,$2\n"
    ), (
        "\taddu\t$3,$18,4\n\taddu\t$18,$3,$2\n"
    ), "reviewed text-width accumulator registers")
    return normalize_standard_return(text)


def normalize_unique_actor_test(text: str) -> str:
    text = replace_once(text, (
        "\tbne\t$2,$0,$L11\n\tmove\t$2,$8\n"
    ), (
        "\tbne\t$2,$0,$L11\n\tmove\t$2,$4\n"
    ), "reviewed unique-actor zero register")
    return normalize_standard_return(text)


def normalize_owner_search(text: str) -> str:
    text = replace_once(text, (
        "\tsll\t$3,$16,2\n\taddu\t$2,$2,$3\n\tlw\t$2,428($2)\n"
    ), (
        "\tsll\t$3,$16,2\n\taddu\t$3,$3,$2\n\tlw\t$2,428($3)\n"
    ), "reviewed owner-search address register")
    return normalize_standard_return(text)


def normalize_node_result(text: str) -> str:
    old = "\taddu\t$3,$3,$16\n"
    if text.count(old) != 2:
        raise ValueError("reviewed node-result operand patterns not found")
    text = text.replace(old, "\taddu\t$3,$16,$3\n")
    return normalize_standard_return(text)


def normalize_player_meter_update(text: str) -> str:
    text = replace_once(text, "\taddu\t$4,$4,$2\n", "\taddu\t$4,$2,$4\n",
                        "reviewed meter primary operand order")
    text = replace_once(text, "\taddu\t$5,$16,$2\n", "\taddu\t$5,$2,$16\n",
                        "reviewed meter secondary operand order")
    old = "\taddu\t$3,$16,$3\n"
    if text.count(old) != 2:
        raise ValueError("reviewed meter index operands not found")
    text = text.replace(old, "\taddu\t$3,$3,$16\n")
    return normalize_standard_return(text)


def normalize_large_leaf_frame(text: str) -> str:
    replacements = (
        ("\t.frame\t$sp,24,$31", "\t.frame\t$sp,40,$31"),
        ("\tsubu\t$sp,$sp,24\n", "\tsubu\t$sp,$sp,40\n"),
        ("\tsw\t$31,16($sp)\n", "\tsw\t$31,32($sp)\n"),
        ("\tlw\t$31,16($sp)\n", "\tlw\t$31,32($sp)\n"),
        ("\taddu\t$sp,$sp,24\n", "\taddu\t$sp,$sp,40\n"),
    )
    for old, new in replacements:
        text = replace_once(text, old, new, "reviewed large leaf frame")
    return normalize_standard_return(text)


def normalize_object_array_init(text: str) -> str:
    text = replace_once(text, (
        "\tlui\t$2,%hi(D_80052EE4) # high\n"
        "\tlui\t$4,%hi(D_800C33D8) # high\n"
        "\taddiu\t$4,$4,%lo(D_800C33D8) # low\n"
        "\tmove\t$5,$0\n\tli\t$6,60\t\t\t# 0x0000003c\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80038E90\n\tsw\t$0,%lo(D_80052EE4)($2)\n"
    ), (
        "\tlui\t$2,%hi(D_80052EE4) # high\n"
        "\tsw\t$0,%lo(D_80052EE4)($2)\n"
        "\tlui\t$4,%hi(D_800C33D8) # high\n"
        "\taddiu\t$4,$4,%lo(D_800C33D8) # low\n"
        "\tmove\t$5,$0\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80038E90\n\tli\t$6,60\t\t\t# 0x0000003c\n"
    ), "reviewed object-array clear scheduling")
    return normalize_standard_return(text)


def normalize_actor_facing(text: str) -> str:
    text = replace_once(text, (
        "\tsb\t$16,316($17)\n\tsb\t$18,317($17)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8000DD88\n\tsw\t$0,48($17)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlh\t$2,2($2)\n\tmove\t$4,$19\n"
    ), (
        "\tsb\t$16,316($17)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8000DD88\n\tsb\t$18,317($17)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlh\t$2,2($2)\n\tmove\t$4,$19\n\tsw\t$0,48($17)\n"
    ), "reviewed actor-facing call scheduling")
    return normalize_standard_return(text)


def normalize_owner_count(text: str, loop_label: str) -> str:
    text = replace_once(text, (
        "\tsll\t$3,$16,2\n\taddu\t$2,$2,$3\n\tlw\t$2,428($2)\n"
    ), (
        "\tsll\t$3,$16,2\n\taddu\t$3,$3,$2\n\tlw\t$2,428($3)\n"
    ), "reviewed owner-count address register")
    text = replace_once(text, (
        f"\tslt\t$2,$17,$2\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        f"\tbne\t$2,$0,{loop_label}\n"
    ), (
        f"\tslt\t$2,$17,$2\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        f"\tbnel\t$2,$0,{loop_label}\n"
    ),
                        "reviewed owner-count loop branch")
    return normalize_standard_return(text)


def normalize_room_index_store(text: str) -> str:
    text = replace_once(text, (
        "\taddu\t$2,$2,$4\n"
        "\tlw\t$3,%lo(D_800581F0)($16)\n"
        "\tsll\t$2,$2,4\n"
    ), (
        "\taddu\t$2,$2,$4\n"
        "\tsll\t$2,$2,4\n"
        "\tlw\t$3,%lo(D_800581F0)($16)\n"
    ), "reviewed room-index shift scheduling")
    text = replace_once(text, (
        "\tsll\t$2,$2,4\n"
        "\tlw\t$3,%lo(D_800581F0)($16)\n"
        "\tsw\t$31,24($sp)\n"
        "\taddu\t$2,$2,$3\n"
        "\taddu\t$2,$2,8212\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_800278F4\n\tsw\t$2,%lo(D_800581EC)($5)\n"
    ), (
        "\tsll\t$2,$2,4\n"
        "\tlw\t$3,%lo(D_800581F0)($16)\n"
        "\taddu\t$2,$2,8212\n"
        "\tsw\t$31,24($sp)\n"
        "\taddu\t$3,$3,$2\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_800278F4\n\tsw\t$3,%lo(D_800581EC)($5)\n"
    ), "reviewed room-index address scheduling")
    return normalize_standard_return(text)


def normalize_scene_start(text: str) -> str:
    pattern = re.compile(
        r"\tli\t\$4,15[^\n]*\n"
        r"\t\.set\tnoreorder\n\t\.set\tnomacro\n"
        r"\tjal\tfunc_80012D9C\n"
        r"\tsw\t\$0,%lo\(D_8005435C\)\(\$2\)\n"
    )
    if len(pattern.findall(text)) != 1:
        raise ValueError("reviewed scene-start store pattern not found uniquely")
    text = pattern.sub(
        "\tsw\t$0,%lo(D_8005435C)($2)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80012D9C\n\tli\t$4,15\n",
        text,
        count=1,
    )
    return normalize_standard_return(text)


def normalize_random_slot_assignment(text: str) -> str:
    text = replace_once(text, (
        "\taddu\t$2,$2,$3\n"
        "\tlw\t$3,0($2)\n\t#nop\n"
        "\tsh\t$18,8($3)\n"
        "\tlw\t$2,4($2)\n"
        "\tlui\t$4,%hi(D_800581F0) # high\n"
        "\tsll\t$3,$2,5\n"
        "\taddu\t$3,$3,$2\n"
        "\tlw\t$2,%lo(D_800581F0)($4)\n"
        "\tsll\t$3,$3,4\n"
        "\taddu\t$3,$3,$2\n"
    ), (
        "\taddu\t$2,$3,$2\n"
        "\tlw\t$3,0($2)\n\t#nop\n"
        "\tsh\t$18,8($3)\n"
        "\tlw\t$3,4($2)\n"
        "\tlui\t$4,%hi(D_800581F0) # high\n"
        "\tsll\t$2,$3,5\n"
        "\taddu\t$2,$2,$3\n"
        "\tlw\t$3,%lo(D_800581F0)($4)\n"
        "\tsll\t$2,$2,4\n"
        "\taddu\t$3,$3,$2\n"
    ), "reviewed random-slot register allocation pattern")
    return normalize_standard_return(text)


def normalize_player_marker_draw(text: str) -> str:
    text = replace_once(text, (
        "\tlui\t$2,%hi(D_800549FC) # high\n"
        "\taddiu\t$17,$2,%lo(D_800549FC) # low\n"
        "\tmove\t$16,$0\n"
        "\tlui\t$20,%hi(D_800C3E24) # high\n"
        "\tli\t$18,-2147483648\t\t\t# 0x80000000\n"
        "\tlui\t$19,%hi(D_800C3E30) # high\n"
    ), (
        "\tmove\t$16,$0\n"
        "\tlui\t$20,%hi(D_800C3E24) # high\n"
        "\tli\t$18,-2147483648\t\t\t# 0x80000000\n"
        "\tlui\t$19,%hi(D_800C3E30) # high\n"
        "\tlui\t$2,%hi(D_800549FC) # high\n"
        "\taddiu\t$17,$2,%lo(D_800549FC) # low\n"
    ), "reviewed player-marker setup scheduling pattern")
    old = "\taddu\t$2,$2,$3\n"
    if text.count(old) != 2:
        raise ValueError("reviewed player-marker operand patterns not found")
    text = text.replace(old, "\taddu\t$2,$3,$2\n")
    return normalize_standard_return(text)


def normalize_label_menu_draw(text: str) -> str:
    for register, symbol in (
        ("$20", "D_80052F18"),
        ("$22", "D_80054964"),
        ("$19", "D_800532A4"),
    ):
        text = replace_once(text, (
            f"\tlui\t{register},%hi({symbol}) # high\n"
            f"\taddiu\t{register},{register},%lo({symbol}) # low\n"
        ), (
            f"\tlui\t$2,%hi({symbol}) # high\n"
            f"\taddiu\t{register},$2,%lo({symbol}) # low\n"
        ), f"reviewed {symbol} address scheduling pattern")

    text = replace_once(text, (
        "\taddu\t$4,$18,14\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80016AA8\n\tsw\t$2,4($18)\n"
    ), (
        "\tsw\t$2,4($18)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80016AA8\n\taddu\t$4,$18,14\n"
    ), "reviewed label-menu result scheduling pattern")
    return normalize_standard_return(text)


def normalize_player_screen_positions(text: str) -> str:
    text = replace_once(text, (
        "\tmove\t$16,$2\n\tmove\t$5,$16\n"
        "\tlhu\t$4,0($17)\n\tlhu\t$2,2($17)\n"
        "\taddu\t$17,$17,4\n\taddu\t$19,$19,1\n"
        "\tsll\t$4,$4,16\n\tand\t$4,$4,$20\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001CF20\n\tor\t$4,$4,$2\n"
    ), (
        "\tandi\t$4,$4,65535\n\tmove\t$16,$2\n\tmove\t$5,$16\n"
        "\tlhu\t$2,0($17)\n\tlhu\t$3,2($17)\n"
        "\taddu\t$17,$17,4\n\taddu\t$19,$19,1\n"
        "\tsll\t$2,$2,16\n\tor\t$4,$4,$2\n\tand\t$4,$4,$20\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001CF20\n\tor\t$4,$4,$3\n"
    ), "reviewed first packed-screen-position pattern")
    text = replace_once(text, (
        "\tmove\t$5,$16\n\tlhu\t$4,0($18)\n\tlhu\t$2,2($18)\n"
        "\tsll\t$4,$4,16\n\tand\t$4,$4,$20\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001D034\n\tor\t$4,$4,$2\n"
    ), (
        "\tandi\t$4,$4,65535\n\tlhu\t$2,0($18)\n\tmove\t$5,$16\n"
        "\tsll\t$2,$2,16\n\tor\t$4,$4,$2\n\tlhu\t$2,2($18)\n"
        "\tand\t$4,$4,$20\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001D034\n\tor\t$4,$4,$2\n"
    ), "reviewed second packed-screen-position pattern")
    return normalize_standard_return(text)


def normalize_selected_screen_positions(text: str) -> str:
    text = replace_once(text, (
        "\tmove\t$16,$2\n\tmove\t$5,$16\n"
        "\tlhu\t$4,0($17)\n\tlhu\t$2,2($17)\n"
        "\tsll\t$4,$4,16\n\tand\t$4,$4,$21\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001CF20\n\tor\t$4,$4,$2\n"
    ), (
        "\tandi\t$4,$4,65535\n\tmove\t$16,$2\n"
        "\tlhu\t$2,0($17)\n\tmove\t$5,$16\n"
        "\tsll\t$2,$2,16\n\tor\t$4,$4,$2\n"
        "\tlhu\t$2,2($17)\n\tand\t$4,$4,$21\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001CF20\n\tor\t$4,$4,$2\n"
    ), "reviewed selected first screen-position pattern")
    text = replace_once(text, (
        "\tmove\t$5,$16\n\tlhu\t$4,0($18)\n\tlhu\t$2,2($18)\n"
        "\tsll\t$4,$4,16\n\tand\t$4,$4,$21\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001D034\n\tor\t$4,$4,$2\n"
    ), (
        "\tandi\t$4,$4,65535\n\tlhu\t$2,0($18)\n\tmove\t$5,$16\n"
        "\tsll\t$2,$2,16\n\tor\t$4,$4,$2\n"
        "\tlhu\t$2,2($18)\n\tand\t$4,$4,$21\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001D034\n\tor\t$4,$4,$2\n"
    ), "reviewed selected second screen-position pattern")
    return normalize_standard_return(text)


def normalize_single_player_screen_positions(text: str) -> str:
    text = replace_once(text, (
        "\taddiu\t$3,$2,%lo(D_800535B0) # low\n"
        "\tlhu\t$4,%lo(D_800535B0)($2)\n\tlhu\t$2,2($3)\n"
        "\tsll\t$4,$4,16\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001CF20\n\tor\t$4,$4,$2\n"
    ), (
        "\taddiu\t$3,$2,%lo(D_800535B0) # low\n"
        "\tlhu\t$2,%lo(D_800535B0)($2)\n\tlhu\t$4,2($3)\n"
        "\tsll\t$2,$2,16\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001CF20\n\tor\t$4,$2,$4\n"
    ), "reviewed primary player screen-position pattern")
    text = replace_once(text, (
        "\tlui\t$2,%hi(D_800535B4) # high\n"
        "\taddiu\t$3,$2,%lo(D_800535B4) # low\n"
        "\tlw\t$5,%lo(D_8005B214)($16)\n"
        "\tlhu\t$4,%lo(D_800535B4)($2)\n\tlhu\t$2,2($3)\n"
        "\tsll\t$4,$4,16\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001D034\n\tor\t$4,$4,$2\n"
    ), (
        "\tandi\t$4,$4,65535\n"
        "\tlui\t$3,%hi(D_800535B4) # high\n"
        "\tlhu\t$2,%lo(D_800535B4)($3)\n"
        "\taddiu\t$3,$3,%lo(D_800535B4) # low\n"
        "\tlw\t$5,%lo(D_8005B214)($16)\n\tlhu\t$3,2($3)\n"
        "\tsll\t$2,$2,16\n\tor\t$4,$4,$2\n"
        "\tlui\t$2,65535\n\tand\t$4,$4,$2\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001D034\n\tor\t$4,$4,$3\n"
    ), "reviewed secondary player screen-position pattern")
    return normalize_standard_return(text)


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


def normalize_display_list_relocation(text: str) -> str:
    return replace_once(text, (
        "$L8:\n\tlw\t$2,4($8)\n\tsw\t$3,4($8)\n"
        "\tsll\t$3,$2,1\n\tandi\t$3,$3,0x00fe\n"
        "\tsll\t$3,$3,16\n\tsrl\t$2,$2,16\n"
        "\tsll\t$4,$2,10\n\tsll\t$2,$2,4\n"
        "\taddu\t$2,$2,-1\n\tor\t$4,$4,$2\n"
        "\tandi\t$4,$4,0xffff\n\tor\t$3,$3,$4\n"
        "\tor\t$3,$3,$10\n\tsw\t$3,0($8)\n"
    ), (
        "$L8:\n\tlw\t$2,4($8)\n\tsw\t$3,4($8)\n"
        "\tsrl\t$4,$2,16\n\tsll\t$2,$2,1\n"
        "\tandi\t$2,$2,0x00fe\n\tsll\t$2,$2,16\n"
        "\tsll\t$3,$4,10\n\tsll\t$4,$4,4\n"
        "\taddu\t$4,$4,-1\n\tor\t$3,$3,$4\n"
        "\tandi\t$3,$3,0xffff\n\tor\t$3,$3,$10\n"
        "\tor\t$2,$2,$3\n\tsw\t$2,0($8)\n"
    ), "reviewed display-list relocation registers")


def normalize_object_tile_event(text: str) -> str:
    text = remap_registers(text, {16: 17})
    text = replace_once(text, (
        "\t.frame\t$sp,24,$31\t\t# vars= 0, regs= 2/0, args= 16, extra= 0\n"
        "\t.mask\t0x80010000,-4\n"
        "\t.fmask\t0x00000000,0\n"
        "\tsubu\t$sp,$sp,24\n"
        "\tsw\t$17,16($sp)\n\tsw\t$31,20($sp)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8000E108\n\tmove\t$17,$4\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tsll\t$2,$2,16\n\tlh\t$3,336($17)\n"
        "\tsra\t$2,$2,16\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$3,$2,$L2\n\tmove\t$5,$17\n"
    ), (
        "\t.frame\t$sp,32,$31\t\t# vars= 0, regs= 3/0, args= 16, extra= 0\n"
        "\t.mask\t0x80030000,-4\n"
        "\t.fmask\t0x00000000,0\n"
        "\tsubu\t$sp,$sp,32\n"
        "\tsw\t$17,20($sp)\n\tmove\t$17,$4\n"
        "\tsw\t$31,24($sp)\n\tsw\t$16,16($sp)\n"
        "\tlh\t$16,336($17)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8000E108\n\tnop\n"
        "\tbne\t$16,$2,$L2\n\tmove\t$5,$17\n"
    ), "reviewed object-tile-event frame and cached team")
    return replace_once(text, (
        "$L1:\n\tlw\t$31,20($sp)\n\tlw\t$17,16($sp)\n\t#nop\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\taddu\t$sp,$sp,24\n"
    ), (
        "$L1:\n\tlw\t$31,24($sp)\n\tlw\t$17,20($sp)\n"
        "\tlw\t$16,16($sp)\n\taddu\t$sp,$sp,32\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tnop\n"
    ), "reviewed object-tile-event epilogue")


def normalize_object_activation_filter(text: str) -> str:
    text = replace_once(text, (
        "\tsubu\t$sp,$sp,48\n"
        "\tsw\t$17,20($sp)\n\tmove\t$17,$4\n"
        "\tsw\t$18,24($sp)\n\tmove\t$18,$0\n"
        "\tsw\t$19,28($sp)\n\tmove\t$19,$18\n"
        "\tsw\t$21,36($sp)\n\tmove\t$21,$18\n"
        "\tlui\t$4,%hi(D_80052F18) # high\n"
        "\tlw\t$3,%lo(D_80052F18)($4)\n"
        "\tli\t$2,2\t\t\t# 0x00000002\n"
        "\tsw\t$31,40($sp)\n\tsw\t$20,32($sp)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$3,$2,$L18\n\tsw\t$16,16($sp)\n"
    ), (
        "\tsubu\t$sp,$sp,48\n"
        "\tsw\t$17,20($sp)\n\tmove\t$17,$4\n"
        "\tsw\t$21,36($sp)\n\tmove\t$21,$0\n"
        "\tsw\t$18,24($sp)\n\tmove\t$18,$21\n"
        "\tsw\t$19,28($sp)\n\tmove\t$19,$18\n"
        "\tlui\t$4,%hi(D_80052F18) # high\n"
        "\tlw\t$3,%lo(D_80052F18)($4)\n"
        "\tli\t$2,2\t\t\t# 0x00000002\n"
        "\tsw\t$31,40($sp)\n\tsw\t$20,32($sp)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$3,$2,$L_mode_return\n\tsw\t$16,16($sp)\n"
    ), "reviewed activation-filter prologue and early mode return")
    text = replace_once(text, (
        "\tlw\t$3,280($17)\n\t#nop\n"
        "\tandi\t$2,$3,0x0020\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L17\n\tmove\t$2,$0\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tandi\t$2,$3,0x0008\n"
    ), (
        "\tlw\t$3,280($17)\n\tandi\t$2,$3,0x0020\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$2,$0,$L_check_flag8\n\tandi\t$2,$3,0x0008\n"
        "$L_mode_return:\n\tj\t$L17\n\tmove\t$2,$0\n"
        "\t.set\tmacro\n\t.set\treorder\n"
        "$L_check_flag8:\n"
    ), "reviewed activation-filter flag checks")
    text = replace_once(text, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$3,$2,$L20\n\tlui\t$2,%hi(D_800529F8) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tbne\t$19,$0,$L20\n"
        "$L18:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L17\n\tmove\t$2,$0\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L20:\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$3,$2,$L20\n\tlui\t$2,%hi(D_800529F8) # high\n"
        "\tbeql\t$19,$0,$L17\n\tmove\t$2,$0\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L20:\n"
    ), "reviewed activation-filter mode-one return")
    text = replace_once(text, (
        "\tbne\t$3,$2,$L6\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$19,$0,$L17\n\tmove\t$2,$21\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$3,$2,$L6\n\tnop\n"
        "\tbeq\t$19,$0,$L17\n\tmove\t$2,$21\n"
    ), "reviewed activation-filter selected-value return")
    text = replace_once(text, (
        "$L16:\n\tjal\tfunc_80009140\n\tmove\t$4,$2\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80009554\n\tmove\t$5,$17\n"
    ), (
        "$L16:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80009140\n\tli\t$21,1\t\t\t# 0x00000001\n"
        "\tmove\t$4,$2\n\tjal\tfunc_80009554\n\tmove\t$5,$17\n"
    ), "reviewed activation-filter success call")
    text = replace_once(text, (
        "\tlbu\t$3,1($2)\n\tli\t$2,1\t\t\t# 0x00000001\n"
        "\tsb\t$3,319($17)\n$L17:\n"
    ), (
        "\tlbu\t$2,1($2)\n\tsb\t$2,319($17)\n"
        "\tmove\t$2,$21\n$L17:\n"
    ), "reviewed activation-filter return value")
    return normalize_standard_return(text)


def normalize_main_state_driver(text: str) -> str:
    text = replace_once(
        text, "\tbeq\t$3,$2,$L97\n",
        "\tbne\t$3,$2,$L97\n",
        "reviewed main-state initial condition",
    )
    text = replace_once(text, (
        "\tlui\t$2,%hi(D_80052F18) # high\n"
        "\tlw\t$4,%lo(D_80052F18)($2)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80008E20\n\tsltu\t$4,$0,$4\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tjal\tfunc_80008FB4\n"
        "\tlui\t$2,%hi(D_80053638) # high\n"
        "$L98:\n"
    ), (
        "\tlui\t$2,%hi(D_80052F18) # high\n"
        "\tlw\t$2,%lo(D_80052F18)($2)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L_mode_bool_ready\n"
        "\tli\t$4,1\t\t\t# 0x00000001\n"
        "\tmove\t$4,$0\n$L_mode_bool_ready:\n"
        "\tjal\tfunc_80008E20\n\tnop\n"
        "\tjal\tfunc_80008FB4\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$2,%hi(D_80053638) # high\n"
        "$L98:\n"
    ), "reviewed main-state boolean call sequence")
    text = replace_once(
        text, "\t.rdata\n\t.align\t3\n$L13:\n",
        "\t.rdata\n\t.align\t2\n$L13:\n",
        "reviewed first main-state table alignment",
    )
    text = replace_once(
        text, "\t.rdata\n\t.align\t3\n$L49:\n",
        "\t.rdata\n\t.align\t2\n\t.word\t0\n$L49:\n",
        "reviewed second main-state table alignment",
    )
    text = replace_once(
        text, "\t.rdata\n\t.align\t3\n$L91:\n",
        "\t.rdata\n\t.align\t2\n$L91:\n",
        "reviewed third main-state table alignment",
    )
    return normalize_standard_return(text)


def normalize_actor_creation(text: str) -> str:
    text = replace_once(text, (
        "\tli\t$2,3\t\t\t# 0x00000003\n"
        "\tsh\t$2,8($18)\n"
        "\tli\t$2,31232\t\t\t# 0x00007a00\n"
        "\tli\t$3,2\t\t\t# 0x00000002\n"
        "\tsw\t$2,40($18)\n"
    ), (
        "\tli\t$2,3\t\t\t# 0x00000003\n"
        "\tli\t$3,2\t\t\t# 0x00000002\n"
        "\tsh\t$2,8($18)\n"
        "\tli\t$2,31232\t\t\t# 0x00007a00\n"
        "\tsw\t$2,40($18)\n"
    ), "reviewed actor-creation constants scheduling")
    text = replace_once(text, (
        "$L12:\n\taddiu\t$5,$2,%lo(D_8005301E+2) # low\n"
        "\tlw\t$2,%lo(D_8005301E+2)($2)\n"
        "\tli\t$19,-1\t\t\t# 0xffffffff\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$2,$19,$L7\n\tlui\t$16,%hi(D_800581F0) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$3,%lo(D_800581F0)($16)\n"
        "\tlhu\t$2,-2($5)\n\taddu\t$5,$5,-2\n"
        "\tsh\t$2,6($3)\n\tlhu\t$2,4($5)\n"
        "\tlh\t$4,6($3)\n\tsh\t$2,8($3)\n"
        "\tlh\t$5,4($5)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8000E078\n\tlui\t$17,%hi(D_800581E9) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tsb\t$2,%lo(D_800581E9)($17)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8000DD88\n\tandi\t$4,$2,0x00ff\n"
    ), (
        "$L12:\n\taddiu\t$4,$2,%lo(D_8005301E+2) # low\n"
        "\tlw\t$2,%lo(D_8005301E+2)($2)\n"
        "\tli\t$19,-1\t\t\t# 0xffffffff\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$2,$19,$L7\n\tlui\t$16,%hi(D_800581F0) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$2,%hi(D_8005301E) # high\n"
        "\tlw\t$3,%lo(D_800581F0)($16)\n"
        "\tlhu\t$2,%lo(D_8005301E)($2)\n"
        "\tsh\t$2,6($3)\n\tlhu\t$2,2($4)\n"
        "\tlh\t$4,6($3)\n"
        "\tsll\t$5,$2,16\n\tsra\t$5,$5,16\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8000E078\n\tsh\t$2,8($3)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$17,%hi(D_800581E9) # high\n"
        "\tsb\t$2,%lo(D_800581E9)($17)\n"
        "\tlbu\t$4,%lo(D_800581E9)($17)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8000DD88\n\tnop\n"
    ), "reviewed actor-creation room coordinates")
    text = replace_once(text, (
        "$L14:\n\tlw\t$2,%lo(D_80057D5C)($3)\n"
        "\tlw\t$31,40($sp)\n\tlw\t$21,36($sp)\n"
        "\tlw\t$20,32($sp)\n\tlw\t$19,28($sp)\n"
        "\tlw\t$18,24($sp)\n\tlw\t$17,20($sp)\n"
        "\tlw\t$16,16($sp)\n"
        "\taddu\t$2,$2,1\n\tsw\t$2,%lo(D_80057D5C)($3)\n"
    ), (
        "$L14:\n\tlw\t$2,%lo(D_80057D5C)($3)\n"
        "\taddu\t$2,$2,1\n\tsw\t$2,%lo(D_80057D5C)($3)\n"
        "\tlw\t$31,40($sp)\n\tlw\t$21,36($sp)\n"
        "\tlw\t$20,32($sp)\n\tlw\t$19,28($sp)\n"
        "\tlw\t$18,24($sp)\n\tlw\t$17,20($sp)\n"
        "\tlw\t$16,16($sp)\n"
    ), "reviewed actor-creation counter epilogue")
    return normalize_standard_return(text)


def normalize_prize_wheel(text: str) -> str:
    text = replace_once(text, (
        "\tsw\t$2,%lo(D_800537DC)($16)\n"
        "\tlui\t$2,%hi(D_800535E4) # high\n"
        "\tsw\t$0,%lo(D_800535E4)($2)\n"
        "\tlui\t$2,%hi(D_800537BC) # high\n"
        "\tlbu\t$3,%lo(D_800537BC)($2)\n"
        "\tlui\t$2,%hi(D_800537D8) # high\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80047C20\n\tsw\t$3,%lo(D_800537D8)($2)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli\t$3,1717960704\t\t\t# 0x66660000\n"
        "\tori\t$3,$3,0x6667\n\tmult\t$2,$3\n"
        "\tlui\t$5,%hi(D_800537E0) # high\n"
    ), (
        "\tsw\t$2,%lo(D_800537DC)($16)\n"
        "\tlui\t$2,%hi(D_800537BC) # high\n"
        "\tlbu\t$3,%lo(D_800537BC)($2)\n"
        "\tlui\t$2,%hi(D_800537D8) # high\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80047C20\n\tsw\t$3,%lo(D_800537D8)($2)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli\t$3,1717960704\t\t\t# 0x66660000\n"
        "\tori\t$3,$3,0x6667\n\tmult\t$2,$3\n"
        "\tlui\t$3,%hi(D_800535E4) # high\n"
        "\tlui\t$5,%hi(D_800537E0) # high\n"
        "\tsw\t$0,%lo(D_800535E4)($3)\n"
    ), "reviewed prize-wheel start scheduling")

    start = text.index("$L10:\n")
    end = text.index("\n\tlw\t$5,%lo(D_8005B214)($18)\n", start)
    old = text[start:end]
    new = """$L10:
\tli\t$6,-2004353024\t\t\t# 0x88880000
\tlui\t$7,%hi(D_800537D4) # high
\tlw\t$3,%lo(D_800537D4)($7)
\tori\t$6,$6,0x8889
\taddu\t$3,$3,39
\tmult\t$3,$6
\tli\t$5,-1240793088\t\t\t# 0xb60b0000
\tori\t$5,$5,0x60b7
\tmfhi\t$8
\t#nop
\t#nop
\taddu\t$2,$8,$3
\tsra\t$2,$2,5
\tsra\t$3,$3,31
\tsubu\t$2,$2,$3
\tsll\t$4,$2,4
\tsubu\t$4,$4,$2
\tsll\t$4,$4,2
\tmult\t$4,$5
\tsra\t$3,$4,31
\tmfhi\t$8
\t#nop
\t#nop
\taddu\t$2,$8,$4
\tsra\t$2,$2,8
\tsubu\t$2,$2,$3
\tsll\t$3,$2,1
\taddu\t$3,$3,$2
\tsll\t$2,$3,4
\tsubu\t$2,$2,$3
\tsll\t$2,$2,3
\tsubu\t$4,$4,$2
\tlui\t$2,%hi(D_800537D8) # high
\tmult\t$4,$6
\tlui\t$3,%hi(D_80053604) # high
\taddiu\t$3,$3,%lo(D_80053604) # low
\tsw\t$0,%lo(D_800537D8)($2)
\tsw\t$4,%lo(D_800537D4)($7)
\tmfhi\t$8
\t#nop
\t#nop
\taddu\t$2,$8,$4
\tsra\t$2,$2,5
\tsra\t$4,$4,31
\tsubu\t$2,$2,$4
\tsll\t$2,$2,2
\taddu\t$2,$2,$3
\tlw\t$3,%lo(D_8005B214)($18)
\tlbu\t$2,3($2)
\tli\t$4,14\t\t\t# 0x0000000e
\t.set\tnoreorder
\t.set\tnomacro
\tjal\tfunc_8001BF24
\tsb\t$2,424($3)
\t.set\tmacro
\t.set\treorder
"""
    text = text[:start] + new + text[end:]

    text = replace_once(text, (
        "\taddu\t$4,$4,$2\n\tmult\t$4,$3\n"
        "\tlw\t$31,28($sp)\n\tlw\t$18,24($sp)\n"
        "\tlw\t$17,20($sp)\n\tsra\t$3,$4,31\n"
    ), (
        "\taddu\t$4,$4,$2\n\tmult\t$4,$3\n"
        "\tsra\t$3,$4,31\n"
    ), "reviewed prize-wheel modulo scheduling")
    text = replace_once(text, (
        "\tsw\t$4,%lo(D_800537D4)($16)\n"
        "\tlw\t$16,16($sp)\n\t#nop\n"
    ), (
        "\tsw\t$4,%lo(D_800537D4)($16)\n"
        "\tlw\t$31,28($sp)\n\tlw\t$18,24($sp)\n"
        "\tlw\t$17,20($sp)\n\tlw\t$16,16($sp)\n\t#nop\n"
    ), "reviewed prize-wheel restore scheduling")
    return normalize_standard_return(text)


def normalize_scene_camera_pass(text: str) -> str:
    text = replace_once(text, (
        "\tlui\t$2,%hi(D_8007782C) # high\n"
        "\tlw\t$2,%lo(D_8007782C)($2)\n"
    ), (
        "\tlui\t$2,%hi(D_8007782C) # high\n"
        "\tlw\t$4,%lo(D_8007782C)($2)\n"
    ), "reviewed camera-pass active flag register")
    text = replace_once(text, (
        "\tbeq\t$2,$0,$L10\n\tlui\t$2,%hi(D_8005B830) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tjal\tfunc_8001E28C\n\tlui\t$2,%hi(D_8005B830) # high\n"
        "$L10:\n"
    ), (
        "\tbeq\t$4,$0,$L10\n\tlui\t$2,%hi(D_8005B830) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8001E28C\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$2,%hi(D_8005B830) # high\n"
        "$L10:\n"
    ), "reviewed camera-pass optional update scheduling")
    text = replace_once(text, (
        "\tlh\t$2,40($sp)\n\tlh\t$3,42($sp)\n"
        "\tlh\t$7,44($sp)\n\tmtc1\t$2,$f2\n"
    ), (
        "\tlh\t$2,40($sp)\n\tlh\t$3,42($sp)\n"
        "\tlh\t$7,44($sp)\n"
        "\t.set\tnoat\n\tlui\t$1,%hi(D_80001D60) # high\n"
        "\tl.s\t$f22,%lo(D_80001D60)($1)\n\t.set\tat\n"
        "\tsll\t$4,$4,6\n"
        "\tmtc1\t$2,$f2\n"
    ), "reviewed camera-pass unit scale load")
    text = replace_once(text, (
        "\tlui\t$2,%hi(D_80001D60) # high\n"
        "\tl.s\t$f22,%lo(D_80001D60)($2)\n"
        "\tsll\t$4,$4,6\n"
    ), "", "reviewed camera-pass late unit scale load")
    text = replace_once(text, (
        "\tsw\t$0,28($sp)\n\tsw\t$0,36($sp)\n"
        "\tmtc1\t$2,$f0\n\tcvt.s.w\t$f0,$f0\n"
        "\ts.s\t$f22,32($sp)\n"
    ), (
        "\tsw\t$0,28($sp)\n\ts.s\t$f22,32($sp)\n"
        "\tsw\t$0,36($sp)\n\tmtc1\t$2,$f0\n"
        "\tcvt.s.w\t$f0,$f0\n"
    ), "reviewed camera-pass unit scale argument scheduling")
    text = replace_once(text, (
        "\tlui\t$16,%hi(D_800587F8) # high\n"
        "\tlui\t$2,%hi(D_80001D64) # high\n"
        "\tlui\t$3,%hi(D_80052BB0) # high\n"
        "\tli.s\t$6,4.00000000000000000000e1\n"
        "\tli.s\t$7,2.13333344459533691406e0\n"
        "\tlw\t$4,%lo(D_80054358)($18)\n"
        "\tl.s\t$f0,%lo(D_80001D64)($2)\n"
        "\tlh\t$2,%lo(D_80052BB0)($3)\n"
    ), (
        "\tlui\t$16,%hi(D_800587F8) # high\n"
        "\tlui\t$2,%hi(D_80052BB0) # high\n"
        "\tli.s\t$6,4.00000000000000000000e1\n"
        "\tli.s\t$7,2.13333344459533691406e0\n"
        "\tlw\t$4,%lo(D_80054358)($18)\n"
        "\t.set\tnoat\n\tlui\t$1,%hi(D_80001D64) # high\n"
        "\tl.s\t$f0,%lo(D_80001D64)($1)\n\t.set\tat\n"
        "\tlh\t$2,%lo(D_80052BB0)($2)\n"
    ), "reviewed camera-pass perspective constants")
    text = replace_once(text, (
        "\tblez\t$3,$L5\n\tsw\t$2,172($20)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tmove\t$18,$20\n"
        "$L7:\n"
    ), (
        "\tblez\t$3,$L5\n\tsw\t$2,172($20)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tl.s\t$f20,72($sp)\n"
        "\tmove\t$18,$20\n"
        "$L7:\n"
    ), "reviewed camera-pass loop invariant")
    text = replace_once(text, (
        "\tl.s\t$f20,72($sp)\n"
        "\tmove\t$16,$2\n"
        "\ts.s\t$f22,16($sp)\n"
        "\tl.s\t$f0,88($sp)\n"
        "\tmfc1\t$5,$f20\n"
        "\tmove\t$4,$16\n"
        "\ts.s\t$f0,20($sp)\n"
        "\tl.s\t$f0,92($sp)\n"
        "\tmove\t$6,$5\n"
        "\ts.s\t$f0,24($sp)\n"
        "\tl.s\t$f0,96($sp)\n"
        "\tmove\t$7,$5\n"
    ), (
        "\tmfc1\t$5,$f20\n"
        "\tmfc1\t$6,$f20\n"
        "\tmfc1\t$7,$f20\n"
        "\ts.s\t$f22,16($sp)\n"
        "\tl.s\t$f0,88($sp)\n"
        "\ts.s\t$f0,20($sp)\n"
        "\tl.s\t$f0,92($sp)\n"
        "\tmove\t$16,$2\n"
        "\ts.s\t$f0,24($sp)\n"
        "\tl.s\t$f0,96($sp)\n"
        "\tmove\t$4,$16\n"
    ), "reviewed camera-pass transform arguments")
    return normalize_standard_return(text)


def normalize_large_render_pass(text: str) -> str:
    text = replace_once(text, (
        "$L4:\n"
        "\tlui\t$2,%hi(D_8000126C) # high\n"
        "\tlui\t$3,%hi(D_80001270) # high\n"
        "\tmtc1\t$0,$f0\n"
        "\tl.s\t$f20,%lo(D_8000126C)($2)\n"
        "\tmfc1\t$5,$f0\n"
        "\tl.s\t$f0,%lo(D_80001270)($3)\n"
    ), (
        "$L4:\n"
        "\tmtc1\t$0,$f0\n"
        "\t.set\tnoat\n\tlui\t$1,%hi(D_8000126C) # high\n"
        "\tl.s\t$f20,%lo(D_8000126C)($1)\n\t.set\tat\n"
        "\tmfc1\t$5,$f0\n"
        "\t.set\tnoat\n\tlui\t$1,%hi(D_80001270) # high\n"
        "\tl.s\t$f0,%lo(D_80001270)($1)\n\t.set\tat\n"
    ), "reviewed large-render view constants")
    text = replace_once(text, (
        "\tmove\t$4,$16\n"
        "\tlui\t$2,%hi(D_80001274) # high\n"
        "\tlui\t$3,%hi(D_80001278) # high\n"
        "\tli.s\t$6,2.00000000000000000000e1\n"
        "\tli.s\t$7,2.13333344459533691406e0\n"
        "\tl.s\t$f0,%lo(D_80001274)($2)\n"
        "\tl.s\t$f2,%lo(D_80001278)($3)\n"
    ), (
        "\tmove\t$4,$16\n"
        "\tli.s\t$6,2.00000000000000000000e1\n"
        "\tli.s\t$7,2.13333344459533691406e0\n"
        "\t.set\tnoat\n\tlui\t$1,%hi(D_80001274) # high\n"
        "\tl.s\t$f0,%lo(D_80001274)($1)\n\t.set\tat\n"
        "\t.set\tnoat\n\tlui\t$1,%hi(D_80001278) # high\n"
        "\tl.s\t$f2,%lo(D_80001278)($1)\n\t.set\tat\n"
    ), "reviewed large-render perspective constants")
    return normalize_standard_return(text)


def normalize_warp_tile_scan(text: str) -> str:
    text = replace_once(text, (
        "\tbeq\t$2,$0,$L2\n\tli\t$22,2\t\t\t# 0x00000002\n"
        "\t.set\tmacro\n\t.set\treorder\n\n$L17:\n"
    ), (
        "\tbeq\t$2,$0,$L2\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tli\t$22,2\t\t\t# 0x00000002\n"
        "\t.set\tmacro\n\t.set\treorder\n\n$L17:\n"
    ), "reviewed warp-tile scan loop constant placement")
    return normalize_standard_return(text)





def normalize_turn_step_update(text: str) -> str:
    text = replace_once(text, (
        "\ttrunc.w.s $f6,$f0,$8\n\ts.s\t$f6,292($16)\n"
    ), (
        "\ttrunc.w.s $f2,$f0,$8\n\ts.s\t$f2,292($16)\n"
    ), "reviewed turn-step truncation register")
    return normalize_standard_return(text)


def normalize_random_handle_refresh(text: str) -> str:
    text = replace_once(text, (
        "\tjal\tfunc_8003A840\n"
        "\tsll\t$4,$16,1\n"
        "\tlui\t$3,%hi(D_80052A20) # high\n"
        "\taddiu\t$3,$3,%lo(D_80052A20) # low\n"
    ), (
        "\tjal\tfunc_8003A840\n"
        "\tlui\t$3,%hi(D_80052A20) # high\n"
        "\taddiu\t$3,$3,%lo(D_80052A20) # low\n"
        "\tsll\t$4,$16,1\n"
    ), "reviewed random handle refresh table address schedule")
    return normalize_standard_return(text)




def normalize_result_message_states(text: str) -> str:
    text = replace_once(text, (
        "\tsubu\t$sp,$sp,144\n"
        "\tsw\t$31,136($sp)\n\tsw\t$19,132($sp)\n"
        "\tsw\t$18,128($sp)\n\tsw\t$17,124($sp)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80009140\n\tsw\t$16,120($sp)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tmove\t$16,$0\n"
    ), (
        "\tsubu\t$sp,$sp,144\n"
        "\tsw\t$16,120($sp)\n\tmove\t$16,$0\n"
        "\tsw\t$31,136($sp)\n\tsw\t$19,132($sp)\n"
        "\tsw\t$18,128($sp)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80009140\n\tsw\t$17,124($sp)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
    ), "reviewed result-message state flag prologue")
    return normalize_standard_return(text)


def normalize_depth_node_insert(text: str) -> str:
    # func_8000B910: retail allocates $2/$3 as the scratch pair of the
    # by-value Vec3f stack copy; the compiler picks $8/$9.
    text = replace_once(text, (
        "\tlw\t$8,4($17)\n\tlw\t$9,8($17)\n"
        "\tsw\t$8,16($sp)\n\tsw\t$9,20($sp)\n"
    ), (
        "\tlw\t$2,4($17)\n\tlw\t$3,8($17)\n"
        "\tsw\t$2,16($sp)\n\tsw\t$3,20($sp)\n"
    ), "reviewed depth-node struct copy scratch registers")
    return normalize_standard_return(text)



def normalize_camera_depth_node_insert(text: str) -> str:
    # func_8000BB80: retail copies the by-value Vec3f stack words through
    # $3/$4 ahead of the camera pointer load; the compiler picks $8/$9.
    text = replace_once(text, (
        "\tlw\t$2,%lo(D_80052F04)($2)\n"
        "\tlui\t$16,%hi(D_800581B0) # high\n"
        "\tlw\t$8,64($sp)\n\tlw\t$9,68($sp)\n"
        "\tsw\t$8,16($sp)\n\tsw\t$9,20($sp)\n"
    ), (
        "\tlw\t$3,64($sp)\n\tlw\t$4,68($sp)\n"
        "\tsw\t$3,16($sp)\n\tsw\t$4,20($sp)\n"
        "\tlui\t$16,%hi(D_800581B0) # high\n"
        "\tlw\t$2,%lo(D_80052F04)($2)\n"
    ), "reviewed camera depth-node struct copy scratch registers")
    return normalize_standard_return(text)



def normalize_far_node_append(text: str) -> str:
    # func_8000C034: retail fills the loop-guard delay slot with the index
    # copy and shares one empty-slot return between both exits.
    text = replace_once(text, (
        "\tbne\t$2,$0,$L4\n\tlui\t$10,%hi(D_80052A90) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n\tmove\t$3,$6\n"
    ), (
        "\tbne\t$2,$0,$L4\n\tmove\t$3,$6\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$10,%hi(D_80052A90) # high\n"
    ), "reviewed far-node loop guard delay slot")
    return replace_once(text, (
        "\tj\t$31\n\tsw\t$4,52($2)\n\t.set\tmacro\n\t.set\treorder\n\n"
        "$L13:\n\tj\t$31\n"
    ), (
        "\tsw\t$4,52($2)\n$L13:\n\tj\t$31\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
    ), "reviewed far-node shared return")



def normalize_animation_frame_step(text: str) -> str:
    # func_8000C674: retail fills the scan-loop range guard delay slot with
    # the counter copy (pure delay-slot move).
    text = replace_once(text, (
        "\tmove\t$16,$6\n\tslt\t$2,$19,$6\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L26\n\tmove\t$21,$0\n"
        "\t.set\tmacro\n\t.set\treorder\n\n$L31:\n"
    ), (
        "\tslt\t$2,$19,$6\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L26\n\tmove\t$16,$6\n"
        "\t.set\tmacro\n\t.set\treorder\n\n\tmove\t$21,$0\n$L31:\n"
    ), "reviewed animation scan-loop guard delay slot")
    # Retail keeps the step reset at the head of the shared overflow block
    # and places the callee argument copy in the scan loop's branch-likely
    # slot; the compiler has the two the other way round (delay-slot swap,
    # only the scan loop enters at $L16).
    text = replace_once(text, (
        "\tbeql\t$21,$0,$L16\n\tli\t$20,1\t\t\t# 0x00000001\n"
    ), (
        "\tbeql\t$21,$0,$L16\n\tmove\t$4,$18\n"
    ), "reviewed animation scan-loop overflow slot")
    text = replace_once(text, (
        "$L16:\n\tmove\t$4,$18\n$L58:\n"
    ), (
        "$L16:\n\tli\t$20,1\t\t\t# 0x00000001\n$L58:\n"
    ), "reviewed animation overflow block head")
    return normalize_standard_return(text)



def normalize_pair_value_copy(text: str) -> str:
    # func_8000CAB0: retail holds the destination index in $4 and the pair
    # table base in $3; the compiler swaps the two temporaries.
    text = replace_once(text, (
        "\tlhu\t$3,0($2)\n\tlw\t$2,24($17)\n\t#nop\n\tlw\t$4,16($2)\n"
        "\tsll\t$2,$5,3\n\taddu\t$2,$2,$4\n\tlw\t$2,0($2)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tj\t$L23\n\tsll\t$3,$3,3\n"
    ), (
        "\tlhu\t$4,0($2)\n\tlw\t$2,24($17)\n\t#nop\n\tlw\t$3,16($2)\n"
        "\tsll\t$2,$5,3\n\taddu\t$2,$2,$3\n\tlw\t$2,0($2)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tj\t$L23\n\tsll\t$4,$4,3\n"
    ), "reviewed pair-copy first store registers")
    text = replace_once(text, (
        "\tlhu\t$3,0($2)\n\tlw\t$2,24($17)\n\tsll\t$3,$3,3\n\tlw\t$4,16($2)\n"
        "\tsll\t$2,$5,3\n\taddu\t$2,$2,$4\n\tlw\t$2,4($2)\n"
        "$L23:\n\taddu\t$3,$3,$22\n\tsw\t$2,4($3)\n"
    ), (
        "\tlhu\t$4,0($2)\n\tlw\t$2,24($17)\n\tsll\t$4,$4,3\n\tlw\t$3,16($2)\n"
        "\tsll\t$2,$5,3\n\taddu\t$2,$2,$3\n\tlw\t$2,4($2)\n"
        "$L23:\n\taddu\t$3,$22,$4\n\tsw\t$2,4($3)\n"
    ), "reviewed pair-copy shared store registers")
    return normalize_standard_return(text)



def normalize_fade_state_step(text: str) -> str:
    # func_8000D838: retail leaf routes every exit through one final
    # `jr $ra; nop`; the compiler emits a `j $31` at each exit.
    text = replace_once(text, (
        "\tbeq\t$3,$2,$L5\n$L20:\n\tj\t$31\n"
    ), (
        "\tbeq\t$3,$2,$L5\n\tj\t$L20\n"
    ), "reviewed fade-state default exit")
    count = text.count("\tj\t$31\n\tsw\t$2,")
    if count != 3:
        raise ValueError("reviewed fade-state exits not found")
    text = text.replace("\tj\t$31\n\tsw\t$2,", "\tj\t$L20\n\tsw\t$2,")
    return replace_once(text, (
        "$L18:\n\tsw\t$2,40($4)\n\tj\t$31\n"
    ), (
        "$L18:\n\tsw\t$2,40($4)\n$L20:\n\tj\t$31\n"
    ), "reviewed fade-state shared return")



def normalize_camera_matrix_commands(text: str) -> str:
    # func_8000F800: retail commits each display-list pointer advance before
    # the remaining command words; the compiler sinks the three pointer
    # stores below the register restores.  Same instructions, retail order.
    text = replace_once(text, (
        "\tlw\t$3,%lo(D_8007784C)($5)\n\tlhu\t$4,40($sp)\n\tmove\t$2,$3\n"
        "\tsw\t$4,4($2)\n\tlw\t$4,%lo(D_80054358)($17)\n\taddu\t$3,$3,8\n"
        "\tsw\t$6,0($2)\n\taddu\t$6,$3,8\n\tsw\t$7,0($3)\n\tsw\t$8,8($3)\n"
        "\tsll\t$4,$4,6\n\taddu\t$16,$4,$16\n\taddu\t$4,$4,$18\n"
        "\tsw\t$16,4($3)\n\tsw\t$4,4($6)\n"
        "\tlw\t$31,60($sp)\n\tlw\t$18,56($sp)\n\tlw\t$17,52($sp)\n"
        "\tlw\t$16,48($sp)\n\taddu\t$2,$3,16\n"
        "\tsw\t$3,%lo(D_8007784C)($5)\n\tsw\t$6,%lo(D_8007784C)($5)\n"
        "\tsw\t$2,%lo(D_8007784C)($5)\n"
    ), (
        "\tlw\t$3,%lo(D_8007784C)($5)\n\tlhu\t$4,40($sp)\n\tmove\t$2,$3\n"
        "\taddu\t$3,$3,8\n\tsw\t$6,0($2)\n\taddu\t$6,$3,8\n"
        "\tsw\t$4,4($2)\n\tlw\t$4,%lo(D_80054358)($17)\n\taddu\t$2,$3,16\n"
        "\tsw\t$3,%lo(D_8007784C)($5)\n\tsw\t$6,%lo(D_8007784C)($5)\n"
        "\tsw\t$7,0($3)\n\tsw\t$2,%lo(D_8007784C)($5)\n\tsw\t$8,8($3)\n"
        "\tsll\t$4,$4,6\n\taddu\t$16,$4,$16\n\taddu\t$4,$4,$18\n"
        "\tsw\t$16,4($3)\n\tsw\t$4,4($6)\n"
        "\tlw\t$31,60($sp)\n\tlw\t$18,56($sp)\n\tlw\t$17,52($sp)\n"
        "\tlw\t$16,48($sp)\n"
    ), "reviewed camera matrix command store order")
    return normalize_standard_return(text)



def normalize_scene_grid_extent(text: str) -> str:
    # func_8000D970: retail gives the loop counter $7 and the entry offset
    # induction variable $8; the compiler swaps the two.
    start = text.index("\t.ent\tfunc_8000D970\n")
    end = text.index("\t.end\tfunc_8000D970\n")
    body = text[start:end]
    pairs = [
        ("\tmove\t$8,$0\n", "\tmove\t$7,$0\n"),
        ("\tli\t$7,8212\t\t\t# 0x00002014\n", "\tli\t$8,8212\t\t\t# 0x00002014\n"),
        ("\taddu\t$5,$6,$7\n", "\taddu\t$5,$6,$8\n"),
        ("\taddu\t$8,$8,1\n\tslt\t$2,$8,$2\n", "\taddu\t$7,$7,1\n\tslt\t$2,$7,$2\n"),
        ("\taddu\t$7,$7,528\n", "\taddu\t$8,$8,528\n"),
    ]
    for old, new in pairs:
        if body.count(old) != 1:
            raise ValueError("reviewed scene-grid loop registers not found")
        body = body.replace(old, new, 1)
    if body.count("$7") != 4 or body.count("$8") != 4:
        raise ValueError("unexpected scene-grid register use")
    return normalize_standard_return(text[:start] + body + text[end:])


def normalize_byte_from_float_trunc(text: str) -> str:
    text = replace_once(
        text,
        "\ttrunc.w.s $f4,$f0,$12\n\tmfc1\t$2,$f4\n",
        "\ttrunc.w.s $f2,$f0,$12\n\tmfc1\t$2,$f2\n",
        "reviewed float-to-byte truncation register",
    )
    return normalize_standard_return(text)








def normalize_camera_orbit_copy(text: str) -> str:
    text = replace_once(text, (
        "\tlw\t$8,276($18)\n\tlw\t$9,280($18)\n\tlw\t$10,284($18)\n"
        "\tsw\t$8,88($18)\n\tsw\t$9,92($18)\n\tsw\t$10,96($18)\n"
    ), (
        "\tlw\t$2,276($18)\n\tlw\t$3,280($18)\n\tlw\t$4,284($18)\n"
        "\tsw\t$2,88($18)\n\tsw\t$3,92($18)\n\tsw\t$4,96($18)\n"
    ), "reviewed camera position copy registers")
    text = replace_once(text, (
        "\tsubu\t$4,$4,$2\n"
        "\tlui\t$2,%hi(D_80052F18) # high\n"
        "\tlw\t$3,%lo(D_80052F18)($2)\n"
        "\tli\t$2,1\t\t\t# 0x00000001\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$3,$2,$L5\n\tsw\t$4,256($18)\n"
    ), (
        "\tsubu\t$4,$4,$2\n"
        "\tlui\t$2,%hi(D_80052F18) # high\n"
        "\tsw\t$4,256($18)\n"
        "\tlw\t$3,%lo(D_80052F18)($2)\n"
        "\tli\t$2,1\t\t\t# 0x00000001\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$3,$2,$L5\n\tnop\n"
    ), "reviewed camera angle store scheduling")
    return normalize_standard_return(text)





def normalize_resource_slot_load(text: str) -> str:
    text = replace_once(text, (
        "\tbne\t$17,$0,$L10\n\taddu\t$5,$17,24\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$4,24($sp)\n\tjal\tfunc_8001A2B0\n\tmove\t$17,$2\n"
        "\taddu\t$5,$17,24\n$L10:\n"
        "\tlw\t$4,%lo(D_80052EC8)($16)\n"
    ), (
        "\tbne\t$17,$0,$L10\n\tnop\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$4,24($sp)\n\tjal\tfunc_8001A2B0\n\tmove\t$17,$2\n"
        "$L10:\n"
    ), "reviewed resource header destination scheduling")
    copy_start = text.index("$L10:\n") + len("$L10:\n")
    copy_end = text.index("\t.set\tnoreorder\n\t.set\tnomacro\n\tjal\tfunc_800166F0\n")
    copy = text[copy_start:copy_end]
    expected = (
        "\tlwl\t$8,16($sp)\n\tlwr\t$8,19($sp)\n\tlwl\t$9,20($sp)\n\tlwr\t$9,23($sp)\n"
        "\tlwl\t$10,24($sp)\n\tlwr\t$10,27($sp)\n\tlwl\t$11,28($sp)\n\tlwr\t$11,31($sp)\n"
        "\tswl\t$8,0($17)\n\tswr\t$8,3($17)\n\tswl\t$9,4($17)\n\tswr\t$9,7($17)\n"
        "\tswl\t$10,8($17)\n\tswr\t$10,11($17)\n\tswl\t$11,12($17)\n\tswr\t$11,15($17)\n"
        "\tlwl\t$8,32($sp)\n\tlwr\t$8,35($sp)\n\tlwl\t$9,36($sp)\n\tlwr\t$9,39($sp)\n"
        "\tswl\t$8,16($17)\n\tswr\t$8,19($17)\n\tswl\t$9,20($17)\n\tswr\t$9,23($17)\n"
    )
    if copy != expected:
        raise ValueError("reviewed resource header copy pattern not found")
    copy = remap_registers(copy, {8: 2, 9: 3, 10: 4, 11: 5})
    copy += "\tlw\t$4,%lo(D_80052EC8)($16)\n\taddu\t$5,$17,24\n"
    text = text[:copy_start] + copy + text[copy_end:]
    return normalize_standard_return(text)





def normalize_controller_stick_deadzone(text: str) -> str:
    text = replace_once(
        text, "\tlb\t$2,2($5)\n\tlbu\t$3,2($5)\n", "\tlb\t$2,2($5)\n",
        "reviewed stick-x load scheduling",
    )
    text = replace_once(
        text, "\tc.lt.s\t$f4,$f0\n\t#nop\n", "\tc.lt.s\t$f4,$f0\n\tlbu\t$2,2($5)\n",
        "reviewed stick-x compare delay",
    )
    text = replace_once(
        text, "\tsb\t$3,6($4)\n$L10:\n", "\tsb\t$2,6($4)\n$L10:\n",
        "reviewed stick-x store register",
    )
    return normalize_standard_return(text)





def normalize_preview_model_loop(text: str) -> str:
    text = replace_once(
        text, "\tbnel\t$2,$0,$L12\n\tandi\t$2,$18,0x00ff\n",
        "\tbne\t$2,$0,$L12\n\tandi\t$2,$18,0x00ff\n",
        "reviewed preview loop branch delay",
    )
    return normalize_standard_return(text)


# Additions for tools/normalize_sn_schedule.py (batch b04).
# Insert the function definitions before `def normalize(` and the DISPATCH
# lines at the top of normalize().


def sink_epilogue_restores(text: str, restores: str, description: str) -> str:
    """Retail SN units keep callee-saved restores after the final body stores.

    Moves the exact consecutive restore block to sit directly before the
    final stack adjustment emitted by normalize_standard_return.
    """
    text = normalize_standard_return(text)
    text = replace_once(text, restores, "", description)
    tail = re.compile(r"\taddu\t\$sp,\$sp,\d+\n\t\.set\tnoreorder\n\t\.set\tnomacro\n\tj\t\$31\n\tnop\n")
    matches = list(tail.finditer(text))
    if len(matches) != 1:
        raise ValueError(f"{description}: final return not found uniquely")
    at = matches[0].start()
    return text[:at] + restores + text[at:]



def normalize_rsp_task_submit(text: str) -> str:
    return sink_epilogue_restores(text, (
        "\tlw\t$31,24($sp)\n\tlw\t$17,20($sp)\n\tlw\t$16,16($sp)\n"
    ), "reviewed RSP task submit epilogue scheduling")



def normalize_frame_display_list_start(text: str) -> str:
    text = replace_once(text, (
        "\taddu\t$3,$21,104\n"
        "\tlui\t$2,%hi(D_80052F00) # high\n"
    ), (
        "\tlui\t$2,%hi(D_80052F00) # high\n"
        "\taddu\t$3,$21,104\n"
    ), "reviewed frame display-list head scheduling")
    text = replace_once(text, (
        "\tsw\t$5,0($3)\n"
        "\taddu\t$5,$2,8\n"
        "\tsw\t$0,4($3)\n"
        "\tli\t$3,-419430400\t\t\t# 0xe7000000\n"
        "\tsw\t$3,0($2)\n"
        "\tsw\t$0,4($2)\n"
        "\tsw\t$6,8($2)\n"
        "\tsw\t$0,4($5)\n"
        "\tlw\t$31,40($sp)\n\tlw\t$21,36($sp)\n\tlw\t$20,32($sp)\n"
        "\tlw\t$19,28($sp)\n\tlw\t$18,24($sp)\n\tlw\t$17,20($sp)\n"
        "\tlw\t$16,16($sp)\n"
        "\taddu\t$3,$2,16\n"
        "\tsw\t$2,%lo(D_8007784C)($4)\n"
        "\tsw\t$5,%lo(D_8007784C)($4)\n"
        "\tsw\t$3,%lo(D_8007784C)($4)\n"
    ), (
        "\tsw\t$5,0($3)\n"
        "\taddu\t$5,$2,8\n"
        "\tsw\t$0,4($3)\n"
        "\tli\t$3,-419430400\t\t\t# 0xe7000000\n"
        "\tsw\t$3,0($2)\n"
        "\taddu\t$3,$2,16\n"
        "\tsw\t$2,%lo(D_8007784C)($4)\n"
        "\tsw\t$5,%lo(D_8007784C)($4)\n"
        "\tsw\t$0,4($2)\n"
        "\tsw\t$3,%lo(D_8007784C)($4)\n"
        "\tsw\t$6,8($2)\n"
        "\tsw\t$0,4($5)\n"
        "\tlw\t$31,40($sp)\n\tlw\t$21,36($sp)\n\tlw\t$20,32($sp)\n"
        "\tlw\t$19,28($sp)\n\tlw\t$18,24($sp)\n\tlw\t$17,20($sp)\n"
        "\tlw\t$16,16($sp)\n"
    ), "reviewed frame display-list tail scheduling")
    return normalize_standard_return(text)



def normalize_menu_icon_draw_tail(text: str) -> str:
    text = replace_once(text, (
        "\taddu\t$4,$18,14\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80016AA8\n\tsw\t$2,4($18)\n"
    ), (
        "\tsw\t$2,4($18)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80016AA8\n\taddu\t$4,$18,14\n"
    ), "reviewed menu-icon draw call scheduling")
    return normalize_standard_return(text)





def remap_body_registers(
    text: str, start: str, mapping: dict[int, int], description: str
) -> str:
    """Rename loop-invariant registers chosen by the retail allocator.

    Applies a one-to-one register permutation to the function body from the
    unique start line onwards, leaving stack save/restore lines untouched.
    """
    if sorted(mapping) != sorted(mapping.values()):
        raise ValueError(f"{description}: mapping is not a permutation")
    if text.count(start) != 1:
        raise ValueError(f"{description}: start line not found uniquely")
    head, tail = text.split(start, 1)
    tail = start + tail
    reg = re.compile(r"\$(\d+)\b")
    lines = []
    for line in tail.split("\n"):
        if "($sp)" not in line and not line.startswith("\t.") and not line.startswith("$L"):
            line = reg.sub(lambda m: "$" + str(mapping.get(int(m.group(1)), int(m.group(1)))), line)
        lines.append(line)
    return head + "\n".join(lines)



def normalize_material_fog_off_commands(text: str) -> str:
    text = remap_body_registers(
        text, "\tli\t$6,-50397184\t\t\t# 0xfcff0000\n",
        {4: 6, 6: 16, 10: 13, 11: 12, 12: 15, 13: 24, 14: 25, 15: 4,
         16: 17, 17: 14, 24: 10, 25: 11},
        "reviewed material command register allocation",
    )
    return normalize_standard_return(text)



def normalize_material_lighting_commands(text: str) -> str:
    load = (
        "\tlui\t$2,%hi(D_80052EE8) # high\n"
        "\tlw\t$9,%lo(D_80052EE8)($2)\n"
    )
    text = replace_once(text, load, "", "reviewed material fog-flag load")
    text = replace_once(text, (
        "\tori\t$25,$25,0xf3ff\n"
    ), (
        "\tori\t$25,$25,0xf3ff\n" + load
    ), "reviewed material fog-flag load placement")
    text = remap_body_registers(
        text, "\tli\t$16,-50397184\t\t\t# 0xfcff0000\n",
        {4: 24, 6: 16, 15: 4, 16: 17, 17: 6, 24: 25, 25: 15},
        "reviewed material command register allocation",
    )
    return normalize_standard_return(text)



def normalize_scene_material_renderer(text: str) -> str:
    # Empty-list branch: retail fills the delay slot with the first command
    # word of the shared exit path (as in normalize_scene_renderer).
    text = replace_once(text, (
        "\tbeq\t$17,$0,$L8\n"
        "\tlui\t$23,%hi(D_800C33C0) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
    ), (
        "\tbeq\t$17,$0,$L8\n"
        "\tli\t$6,-1174405120\t\t\t# 0xba000000\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$23,%hi(D_800C33C0) # high\n"
    ), "reviewed scene-material empty-list branch")
    text = replace_once(text, (
        "$L8:\n\tli\t$6,-1174405120\t\t\t# 0xba000000\n"
    ), (
        "\tli\t$6,-1174405120\t\t\t# 0xba000000\n$L8:\n"
    ), "reviewed scene-material exit label")
    # Retail rematerialises the 0x02000000 flag bit in $2 instead of
    # keeping it in $12.
    li12 = "\tli\t$12,33554432\t\t\t# 0x02000000\n"
    if text.count(li12) != 3:
        raise ValueError("reviewed scene-material flag constant not found")
    text = text.replace(li12, "\tli\t$2,33554432\t\t\t# 0x02000000\n")
    if text.count("\tor\t$20,$20,$12\n") != 2:
        raise ValueError("reviewed scene-material flag set not found")
    text = text.replace("\tor\t$20,$20,$12\n", "\tor\t$20,$20,$2\n")
    text = replace_once(text, "\tand\t$2,$20,$12\n", "\tand\t$2,$20,$2\n",
                        "reviewed scene-material flag test")
    # Material colour command: retail swaps the packet and colour pointers.
    start = "\tmove\t$7,$5\n\taddu\t$5,$5,8\n\taddu\t$6,$2,20\n"
    end = "\tsw\t$2,4($7)\n"
    if text.count(start) != 1:
        raise ValueError("reviewed scene-material colour block not found")
    head, rest = text.split(start, 1)
    body, tail = rest.split(end, 1)
    block = start + body + end
    block = block.replace("$7", "$R7").replace("$6", "$7").replace("$R7", "$6")
    text = head + block + tail
    return normalize_standard_return(text)


# SN scheduling normalizer additions for batch b05 (reorder/rename only).
# Function definitions go before normalize(); the DISPATCH line goes into normalize().


def normalize_bonus_spawn_table(text: str) -> str:
    text = replace_once(text, (
        "\tsll\t$3,$19,1\n"
        "\taddu\t$3,$3,$19\n"
        "\tsll\t$3,$3,3\n"
        "\tlui\t$2,%hi(D_80053848) # high\n"
        "\taddiu\t$2,$2,%lo(D_80053848) # low\n"
        "\taddu\t$18,$3,$2\n"
    ), (
        "\tlui\t$3,%hi(D_80053848) # high\n"
        "\taddiu\t$3,$3,%lo(D_80053848) # low\n"
        "\tsll\t$2,$19,1\n"
        "\taddu\t$2,$2,$19\n"
        "\tsll\t$2,$2,3\n"
        "\taddu\t$18,$2,$3\n"
    ), "reviewed spawn-table row address scheduling")
    return normalize_standard_return(text)


def normalize_hunt_list_reset(text: str) -> str:
    # Retail assigns the second byte argument to $18 and the flag to $20 and
    # orders the argument copies obj, count, flag, a, b.
    old = (
        "\tsw\t$17,44($sp)\n\tmove\t$17,$4\n\tsw\t$19,52($sp)\n\tmove\t$19,$5\n"
        "\tsw\t$20,56($sp)\n\tmove\t$20,$6\n\tsw\t$16,40($sp)\n\tmove\t$16,$7\n"
        "\tsw\t$18,48($sp)\n\tmove\t$18,$0\n"
        "\tlui\t$4,%hi(D_8006D6A0) # high\n\taddiu\t$4,$4,%lo(D_8006D6A0) # low\n"
        "\tmove\t$5,$18\n\tsw\t$31,60($sp)\n"
    )
    new = (
        "\tsw\t$17,44($sp)\n\tmove\t$17,$4\n\tsw\t$16,40($sp)\n\tmove\t$16,$7\n"
        "\tsw\t$20,56($sp)\n\tmove\t$20,$0\n"
        "\tlui\t$4,%hi(D_8006D6A0) # high\n\taddiu\t$4,$4,%lo(D_8006D6A0) # low\n"
        "\tsw\t$19,52($sp)\n\tmove\t$19,$5\n\tmove\t$5,$20\n"
        "\tsw\t$18,48($sp)\n\tmove\t$18,$6\n\tsw\t$31,60($sp)\n"
    )
    if text.count(old) != 1:
        raise ValueError("reviewed hunt-list prologue not found")
    head, tail = text.split(old)
    lines = []
    for line in tail.split("\n"):
        if not re.match(r"\t(sw|lw)\t\$(18|20),(48|56)\(\$sp\)", line):
            line = re.sub(r"\$(18|20)\b",
                          lambda m: "$20" if m.group(1) == "18" else "$18", line)
        lines.append(line)
    return normalize_standard_return(head + new + "\n".join(lines))


def normalize_boot_timer_tail(text: str) -> str:
    text = replace_once(text, (
        "\tlw\t$31,28($sp)\n\tlw\t$16,24($sp)\n"
        "\tsw\t$2,D_80097870\n\tsw\t$3,D_80097870+4\n"
    ), (
        "\tsw\t$2,D_80097870\n\tsw\t$3,D_80097870+4\n"
        "\tlw\t$31,28($sp)\n\tlw\t$16,24($sp)\n"
    ), "reviewed boot-timer epilogue scheduling")
    return normalize_standard_return(text)



def normalize_state_flag_clear_tail(text: str) -> str:
    text = replace_once(text, (
        "\tli\t$3,-2\t\t\t# 0xfffffffe\n"
        "\tsw\t$0,%lo(D_80052EE4)($2)\n"
        "\tlw\t$2,%lo(D_80052EE0)($4)\n"
        "\tlw\t$31,16($sp)\n"
        "\tand\t$2,$2,$3\n"
    ), (
        "\tsw\t$0,%lo(D_80052EE4)($2)\n"
        "\tlw\t$2,%lo(D_80052EE0)($4)\n"
        "\tli\t$3,-2\t\t\t# 0xfffffffe\n"
        "\tand\t$2,$2,$3\n"
        "\tlw\t$31,16($sp)\n"
    ), "reviewed state-flag clear epilogue scheduling")
    return text



def normalize_actor_angle_step(text: str) -> str:
    text = replace_once(text, (
        "\tmov.s\t$f20,$f0\n\tlh\t$4,336($17)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80006D7C\n\tmove\t$5,$0\n"
    ), (
        "\tmove\t$5,$0\n\tlh\t$4,336($17)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80006D7C\n\tmov.s\t$f20,$f0\n"
    ), "reviewed actor-angle call delay slot")
    text = replace_once(text, (
        "\ttrunc.w.s $f6,$f0,$8\n\ts.s\t$f6,292($17)\n"
    ), (
        "\ttrunc.w.s $f2,$f0,$8\n\ts.s\t$f2,292($17)\n"
    ), "reviewed actor-angle position conversion register")
    text = replace_once(text, (
        "\ttrunc.w.s $f6,$f0,$8\n\tmfc1\t$5,$f6\n"
    ), (
        "\ttrunc.w.s $f2,$f0,$8\n\tmfc1\t$5,$f2\n"
    ), "reviewed actor-angle step conversion register")
    return normalize_standard_return(text)



def normalize_baby_banish_sequence(text: str) -> str:
    text = replace_once(text, (
        "\tlw\t$7,252($3)\n\tlw\t$8,256($3)\n\tlw\t$9,260($3)\n"
        "\tsw\t$7,264($3)\n\tsw\t$8,268($3)\n\tsw\t$9,272($3)\n"
    ), (
        "\tlw\t$2,252($3)\n\tlw\t$6,256($3)\n\tlw\t$7,260($3)\n"
        "\tsw\t$2,264($3)\n\tsw\t$6,268($3)\n\tsw\t$7,272($3)\n"
    ), "reviewed camera save block-copy registers")
    text = replace_once(text, (
        "\tlw\t$7,264($3)\n\tlw\t$8,268($3)\n\tlw\t$9,272($3)\n"
        "\tsw\t$7,252($3)\n\tsw\t$8,256($3)\n\tsw\t$9,260($3)\n"
    ), (
        "\tlw\t$5,264($3)\n\tlw\t$7,268($3)\n\tlw\t$8,272($3)\n"
        "\tsw\t$5,252($3)\n\tsw\t$7,256($3)\n\tsw\t$8,260($3)\n"
    ), "reviewed camera restore block-copy registers")
    return normalize_standard_return(text)



def normalize_level_exit_frames(text: str) -> str:
    text = replace_once(text, (
        "\tbeq\t$3,$2,$L17\n\tlui\t$4,%hi(D_80097868) # high\n"
    ), (
        "\tbeq\t$3,$2,$L17\n\tlui\t$12,%hi(D_80097868) # high\n"
    ), "reviewed frame-counter base register")
    text = replace_once(text, (
        "\tlw\t$3,%lo(D_80097868)($4)\n"
    ), (
        "\tlw\t$3,%lo(D_80097868)($12)\n"
    ), "reviewed frame-counter load register")
    text = replace_once(text, (
        "\tsw\t$3,%lo(D_80097868)($4)\n"
    ), (
        "\tsw\t$3,%lo(D_80097868)($12)\n"
    ), "reviewed frame-counter store register")
    text = replace_once(text, (
        "\tblez\t$2,$L37\n\tlui\t$21,%hi(D_80077850) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli\t$20,4\t\t\t# 0x00000004\n\tli\t$19,2\t\t\t# 0x00000002\n"
        "\tlui\t$2,%hi(D_8009C358) # high\n"
    ), (
        "\tblez\t$2,$L37\n\tlui\t$2,%hi(D_8009C358) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$21,%hi(D_80077850) # high\n"
        "\tli\t$20,4\t\t\t# 0x00000004\n\tli\t$19,2\t\t\t# 0x00000002\n"
    ), "reviewed drain-loop setup scheduling")
    return normalize_standard_return(text)


# Normalizer additions for tools/normalize_sn_schedule.py (batch b08).
# Each function is anchored to one unit and raises via replace_once on mismatch.


def normalize_nearest_target_message(text: str) -> str:
    text = replace_once(text, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L2\n\tlui\t$2,%hi($LC1) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$2,%hi($LC0) # high\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L3\n\taddiu\t$4,$2,%lo($LC0) # low\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L2:\n\taddiu\t$4,$2,%lo($LC1) # low\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbnel\t$2,$0,$L2\n\tlui\t$4,%hi($LC1) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$4,%hi($LC0) # high\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L3\n\taddiu\t$4,$4,%lo($LC0) # low\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L2:\n\taddiu\t$4,$4,%lo($LC1) # low\n"
    ), "reviewed nearest-target message string branch")
    return normalize_standard_return(text)




def normalize_angle_dial_update(text: str) -> str:
    text = replace_once(text, (
        "\tmult\t$4,$3\n"
        "\tlw\t$31,28($sp)\n\tlw\t$18,24($sp)\n\tlw\t$17,20($sp)\n"
        "\tsra\t$3,$4,31\n"
    ), (
        "\tmult\t$4,$3\n"
        "\tsra\t$3,$4,31\n"
    ), "reviewed angle-dial epilogue restore placement")
    text = replace_once(text, (
        "\tsw\t$4,%lo(D_800537D4)($16)\n\tlw\t$16,16($sp)\n"
    ), (
        "\tsw\t$4,%lo(D_800537D4)($16)\n"
        "\tlw\t$31,28($sp)\n\tlw\t$18,24($sp)\n\tlw\t$17,20($sp)\n"
        "\tlw\t$16,16($sp)\n"
    ), "reviewed angle-dial epilogue restore order")
    return normalize_standard_return(text)





def normalize_trade_capacity_score(text: str) -> str:
    text = replace_once(text, (
        "\tlh\t$6,488($18)\n\tlh\t$5,488($19)\n\tlh\t$4,496($18)\n"
        "\taddu\t$3,$6,$5\n"
    ), (
        "\tlh\t$5,488($18)\n\tlh\t$6,488($19)\n\tlh\t$4,496($18)\n"
        "\taddu\t$3,$5,$6\n"
    ), "reviewed trade-capacity score operand registers")
    text = replace_once(text, (
        "\tj\t$L14\n\tsubu\t$3,$3,$6\n"
    ), (
        "\tj\t$L14\n\tsubu\t$3,$3,$5\n"
    ), "reviewed trade-capacity overflow subtraction register")
    text = replace_once(text, (
        "$L13:\n\tmove\t$3,$5\n"
    ), (
        "$L13:\n\tmove\t$3,$6\n"
    ), "reviewed trade-capacity fallback score register")
    return normalize_standard_return(text)


def normalize_trade_state_machine(text: str) -> str:
    text = replace_once(text, (
        "\tsll\t$3,$17,1\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\taddu\t$2,$3,$16\n"
        "\taddu\t$7,$3,$18\n"
        "\tlh\t$4,496($2)\n"
        "\tlh\t$5,488($2)\n"
        "\tlh\t$6,488($7)\n"
        "\tsubu\t$4,$4,$5\n"
        "\tslt\t$2,$6,$4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L76\n"
        "\tlui\t$2,%hi(D_800B9304) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$3,%hi(D_800B9304) # high\n"
        "\taddu\t$2,$5,$4\n"
        "\tsw\t$2,%lo(D_800B9304)($3)\n"
        "\tmove\t$2,$6\n"
        "\tlui\t$3,%hi(D_800B9300) # high\n"
        "\tsubu\t$2,$2,$4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L28\n"
        "\tsw\t$2,%lo(D_800B9300)($3)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L23:\n"
        "\tli\t$2,99\t\t\t# 0x00000063\n"
        "\tlh\t$5,504($16)\n"
        "\tlh\t$6,504($18)\n"
        "\tsubu\t$4,$2,$5\n"
        "\tslt\t$2,$6,$4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L29\n"
        "\tlui\t$2,%hi(D_800B9304) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$3,%hi(D_800B9304) # high\n"
        "\taddu\t$2,$5,$4\n"
        "\tsw\t$2,%lo(D_800B9304)($3)\n"
        "\tlui\t$3,%hi(D_800B9300) # high\n"
        "\tsubu\t$2,$6,$4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L28\n"
        "\tsw\t$2,%lo(D_800B9300)($3)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L29:\n$L76:\n"
        "\taddu\t$3,$5,$6\n"
        "\tsw\t$3,%lo(D_800B9304)($2)\n"
        "\tlui\t$2,%hi(D_800B9300) # high\n"
        "\tsw\t$0,%lo(D_800B9300)($2)\n"
    ), (
        "\tsll\t$2,$17,1\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\taddu\t$3,$2,$16\n"
        "\taddu\t$6,$2,$18\n"
        "\tlh\t$2,496($3)\n"
        "\tlh\t$5,488($3)\n"
        "\tlh\t$3,488($6)\n"
        "\tsubu\t$4,$2,$5\n"
        "\tslt\t$2,$3,$4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L76\n"
        "\tlui\t$2,%hi(D_800B9304) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$3,%hi(D_800B9304) # high\n"
        "\taddu\t$2,$5,$4\n"
        "\tsw\t$2,%lo(D_800B9304)($3)\n"
        "\tlh\t$2,488($6)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L75\n"
        "\tlui\t$3,%hi(D_800B9300) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L23:\n"
        "\tli\t$2,99\t\t\t# 0x00000063\n"
        "\tlh\t$5,504($16)\n"
        "\tlh\t$3,504($18)\n"
        "\tsubu\t$4,$2,$5\n"
        "\tslt\t$2,$3,$4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L76\n"
        "\tlui\t$2,%hi(D_800B9304) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$3,%hi(D_800B9304) # high\n"
        "\taddu\t$2,$5,$4\n"
        "\tsw\t$2,%lo(D_800B9304)($3)\n"
        "\tlh\t$2,504($18)\n"
        "\tlui\t$3,%hi(D_800B9300) # high\n"
        "$L75:\n"
        "\tsubu\t$2,$2,$4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L28\n"
        "\tsw\t$2,%lo(D_800B9300)($3)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L76:\n"
        "\taddu\t$3,$5,$3\n"
        "\tsw\t$3,%lo(D_800B9304)($2)\n"
        "\tlui\t$2,%hi(D_800B9300) # high\n"
        "\tsw\t$0,%lo(D_800B9300)($2)\n"
    ), "reviewed trade-state transfer block")
    text = replace_once(text, (
        "\tlh\t$6,496($2)\n\t#nop\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$6,$19,$L9\n"
    ), (
        "\tlh\t$5,496($2)\n\t#nop\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbeq\t$5,$19,$L9\n"
    ), "reviewed trade-state selected-capacity register")
    text = replace_once(text,
        "\tsw\t$6,%lo(D_800B9304)($2)\n",
        "\tsw\t$5,%lo(D_800B9304)($2)\n",
        "reviewed trade-state selected-capacity store")
    text = replace_once(text,
        "\taddu\t$2,$16,$2\n\tlh\t$3,488($2)\n",
        "\taddu\t$2,$2,$16\n\tlh\t$3,488($2)\n",
        "reviewed trade-state item pointer operand order")
    return normalize_standard_return(text)


def normalize_camera_state_machine(text: str) -> str:
    text = replace_once(text,
        "\ttrunc.w.s $f14,$f0,$8\n\ts.s\t$f14,52($4)\n",
        "\ttrunc.w.s $f2,$f0,$8\n\ts.s\t$f2,52($4)\n",
        "reviewed camera destination conversion register")
    start = text.find("$L14:\n")
    end = text.find("$L19:\n", start)
    if start < 0 or end < 0 or text.count("$L14:\n") != 1 or text.count("$L19:\n") != 1:
        raise ValueError("reviewed camera angle block not found uniquely")
    angle = """$L14:
	.set	noreorder
	.set	nomacro
	lw	$4,284($18)
	addiu	$2,$0,-1
	bne	$4,$2,$L75
	lui	$6,0xb60b
	lui	$2,%hi(D_800588F8) # high
	lw	$3,292($18)
	addiu	$5,$2,%lo(D_800588F8) # low
	sw	$3,52($5)
	lw	$2,284($18)
	bne	$2,$4,$L18
	lui	$2,%hi(D_800588F8) # high
	addiu	$2,$0,2400
	j	$L19
	sw	$2,56($5)
$L75:
	lui	$2,%hi(D_800588F8) # high
$L18:
	addiu	$7,$2,%lo(D_800588F8) # low
	lw	$4,52($7)
	ori	$6,$6,0x60b7
	mult	$4,$6
	mtc1	$0,$f2
	sra	$3,$4,31
	mfhi	$8
	addu	$2,$8,$4
	sra	$2,$2,8
	subu	$2,$2,$3
	sll	$3,$2,1
	addu	$3,$3,$2
	sll	$2,$3,4
	subu	$2,$2,$3
	sll	$2,$2,3
	subu	$3,$4,$2
	addiu	$2,$0,180
	subu	$5,$2,$3
	mtc1	$5,$f0
	cvt.s.w	$f0,$f0
	c.le.s	$f2,$f0
	nop
	bc1f	$L20
	lui	$2,%hi(D_80052BE8) # high
	lw	$2,%lo(D_80052BE8)($2)
	mult	$2,$5
	j	$L73
	nop
$L20:
	lw	$2,%lo(D_80052BE8)($2)
	addiu	$3,$3,-180
	mult	$2,$3
$L73:
	mflo	$2
	nop
	nop
	mult	$2,$6
	mfhi	$8
	addu	$3,$8,$2
	sra	$3,$3,7
	sra	$2,$2,31
	subu	$3,$3,$2
	addiu	$3,$3,2400
	sw	$3,56($7)
	.set	macro
	.set	reorder
"""
    text = text[:start] + angle + text[end:]
    start = text.find("$L50:\n")
    end = text.find("$L53:\n", start)
    if start < 0 or end < 0 or text.count("$L50:\n") != 1 or text.count("$L53:\n") != 1:
        raise ValueError("reviewed camera transition setup block not found uniquely")
    transition = """$L50:
	.set	noreorder
	.set	nomacro
	lui	$18,%hi(D_800C33D8) # high
	addiu	$17,$18,%lo(D_800C33D8) # low
	lw	$7,%lo(D_800C33D8)($18)
	lw	$4,12($17)
	lw	$5,16($17)
	lw	$6,20($17)
	lw	$2,4($17)
	lw	$3,8($17)
	sw	$2,16($sp)
	sw	$3,20($sp)
	jal	func_80008B80
	nop
	lui	$3,0x51eb
	ori	$3,$3,0x851f
	mult	$2,$3
	sw	$2,32($17)
	sra	$2,$2,31
	mtc1	$0,$f2
	mfhi	$8
	sra	$3,$8,6
	subu	$2,$3,$2
	mtc1	$2,$f0
	cvt.s.w	$f0,$f0
	c.le.s	$f2,$f0
	nop
	bc1fl	$LCAM_COUNT_SIGN
	subu	$2,$0,$2
$LCAM_COUNT_SIGN:
	sw	$2,40($17)
	lw	$4,252($16)
	sw	$4,24($17)
	lw	$6,260($16)
	slt	$2,$2,5
	beq	$2,$0,$LCAM_COUNT_CLAMPED
	sw	$6,28($17)
	addiu	$2,$0,5
	sw	$2,40($17)
$LCAM_COUNT_CLAMPED:
	lui	$2,%hi(D_800547C0) # high
	lw	$5,%lo(D_800547C0)($2)
	lw	$3,40($17)
	subu	$5,$5,$4
	.set	macro
	.set	reorder
	div	$5,$5,$3
	.set	noreorder
	.set	nomacro
	lui	$2,%hi(D_800547C4) # high
	lw	$4,%lo(D_800547C4)($2)
	subu	$4,$4,$6
	.set	macro
	.set	reorder
	div	$4,$4,$3
	.set	noreorder
	.set	nomacro
	lwc1	$f0,%lo(D_800C33D8)($18)
	lwc1	$f2,88($16)
	sub.s	$f0,$f0,$f2
	mtc1	$3,$f4
	cvt.s.w	$f4,$f4
	div.s	$f0,$f0,$f4
	swc1	$f0,48($17)
	lwc1	$f0,8($17)
	lwc1	$f2,96($16)
	sub.s	$f0,$f0,$f2
	div.s	$f0,$f0,$f4
	swc1	$f0,56($17)
	lwc1	$f0,88($16)
	lwc1	$f2,92($16)
	swc1	$f0,12($17)
	lwc1	$f0,96($16)
	lui	$2,%hi(D_800547C8) # high
	swc1	$f0,20($17)
	lwc1	$f0,%lo(D_800547C8)($2)
	cvt.s.w	$f0,$f0
	addiu	$2,$0,15
	sub.s	$f0,$f0,$f2
	sw	$2,240($16)
	lui	$2,%hi(D_800587F0) # high
	div.s	$f0,$f0,$f4
	trunc.w.s	$f2,$f0
	swc1	$f2,%lo(D_800587F0)($2)
	lui	$2,%hi(D_80058674) # high
	sw	$5,%lo(D_80058674)($2)
	lui	$2,%hi(D_80058670) # high
	sw	$4,%lo(D_80058670)($2)
	.set	macro
	.set	reorder
"""
    text = text[:start] + transition + text[end:]
    text = text.replace("$f14", "$f2")
    text = replace_once(text, (
        "\tlw\t$9,44($18)\n\tlw\t$10,48($18)\n\tlw\t$11,52($18)\n"
        "\tsw\t$9,60($17)\n\tsw\t$10,64($17)\n\tsw\t$11,68($17)\n"
    ), (
        "\tlw\t$2,44($18)\n\tlw\t$3,48($18)\n\tlw\t$4,52($18)\n"
        "\tsw\t$2,60($17)\n\tsw\t$3,64($17)\n\tsw\t$4,68($17)\n"
    ), "reviewed camera target-position copy registers")
    text = replace_once(text, (
        "\tlui\t$2,%hi(D_80052F18) # high\n"
        "\tlw\t$2,44($18)\n\tlw\t$3,48($18)\n\tlw\t$4,52($18)\n"
        "\tsw\t$2,60($17)\n\tsw\t$3,64($17)\n\tsw\t$4,68($17)\n"
    ), (
        "\tlw\t$2,44($18)\n\tlw\t$3,48($18)\n\tlw\t$4,52($18)\n"
        "\tsw\t$2,60($17)\n\tsw\t$3,64($17)\n\tsw\t$4,68($17)\n"
        "\tlui\t$2,%hi(D_80052F18) # high\n"
    ), "reviewed camera mode-address scheduling")
    control_copy_old = (
        "\tlw\t$9,4($17)\n\tlw\t$10,8($17)\n"
        "\tsw\t$9,16($sp)\n\tsw\t$10,20($sp)\n"
    )
    control_copy_new = (
        "\tlw\t$2,4($17)\n\tlw\t$3,8($17)\n"
        "\tsw\t$2,16($sp)\n\tsw\t$3,20($sp)\n"
    )
    if text.count(control_copy_old) != 1:
        raise ValueError("reviewed camera control-vector copy not found uniquely")
    text = text.replace(control_copy_old, control_copy_new)
    text = replace_once(text, (
        "$L53:\n\tlui\t$18,%hi(D_800C33D8) # high\n"
        "\tlw\t$7,%lo(D_800C33D8)($18)\n"
        "\taddiu\t$17,$18,%lo(D_800C33D8) # low\n"
        "\tlw\t$2,4($17)\n\tlw\t$3,8($17)\n"
        "\tsw\t$2,16($sp)\n\tsw\t$3,20($sp)\n"
    ), (
        "$L53:\n\tlui\t$18,%hi(D_800C33D8) # high\n"
        "\taddiu\t$17,$18,%lo(D_800C33D8) # low\n"
        "\tlw\t$2,4($17)\n\tlw\t$3,8($17)\n"
        "\tsw\t$2,16($sp)\n\tsw\t$3,20($sp)\n"
        "\tlw\t$7,%lo(D_800C33D8)($18)\n"
    ), "reviewed camera follow-state scheduling")
    text = replace_once(text, (
        "\tsw\t$0,16($17)\n"
        "\tl.s\t$f0,%lo(D_800C33D8)($18)\n"
        "\tl.s\t$f2,8($17)\n"
        "\tli\t$2,16\t\t\t# 0x00000010\n"
    ), (
        "\tl.s\t$f0,%lo(D_800C33D8)($18)\n"
        "\tl.s\t$f2,8($17)\n"
        "\tli\t$2,16\t\t\t# 0x00000010\n"
        "\tsw\t$0,16($17)\n"
    ), "reviewed camera follow-state reset scheduling")
    return normalize_standard_return(text)




def normalize_player_item_row(text: str) -> str:
    start = text.find("\tmove\t$17,$20\n")
    end = text.find("\taddu\t$17,$17,2\n")
    if start < 0 or end < start or text.count("\tmove\t$17,$20\n") != 1:
        raise ValueError("reviewed item-row loop register block not found")
    end += len("\taddu\t$17,$17,2\n")
    text = text[:start] + remap_registers(text[start:end], {16: 17, 17: 16}) + text[end:]
    text = replace_once(text, (
        "\tsra\t$2,$2,31\n\tmfhi\t$8\n\t#nop\n\t#nop\n"
        "\tsra\t$3,$8,1\n\tsubu\t$3,$3,$2\n"
        "\tli\t$2,2\t\t\t# 0x00000002\n\tsubu\t$2,$2,$3\n"
        "\tsll\t$17,$2,1\n\taddu\t$17,$17,$2\n"
    ), (
        "\tsra\t$2,$2,31\n\tli\t$3,2\t\t\t# 0x00000002\n\tmfhi\t$8\n\t#nop\n\t#nop\n"
        "\tsra\t$5,$8,1\n\tsubu\t$5,$5,$2\n"
        "\tsubu\t$3,$3,$5\n"
        "\tsll\t$2,$3,1\n\taddu\t$2,$2,$3\n"
    ), "reviewed item-row column offset registers")
    text = replace_once(text, "\tsll\t$17,$17,4\n", "\tsll\t$17,$2,4\n",
                        "reviewed item-row column offset scale")
    text = replace_once(text, (
        "\taddu\t$5,$17,$5\n\taddu\t$5,$5,42\n"
    ), (
        "\taddu\t$5,$5,42\n\taddu\t$5,$17,$5\n"
    ), "reviewed item-row label column sum")
    return normalize_standard_return(text)


def normalize_sound_select_store(text: str) -> str:
    # Retail allocates the store address high part to $2 and the loaded
    # selector base to $3 (register choice only; same values stored/loaded).
    text = replace_once(text, (
        "\tlui\t$2,%hi(D_80052F18) # high\n"
        "\tlui\t$3,%hi(D_800B9344) # high\n"
        "\tlw\t$4,%lo(D_80052F18)($2)\n"
        "\tli\t$2,7\t\t\t# 0x00000007\n"
    ), (
        "\tlui\t$2,%hi(D_800B9344) # high\n"
        "\tlui\t$3,%hi(D_80052F18) # high\n"
        "\tlw\t$4,%lo(D_80052F18)($3)\n"
        "\tli\t$3,7\t\t\t# 0x00000007\n"
    ), "reviewed sound-select store registers")
    text = replace_once(text, "\tsw\t$2,%lo(D_800B9344)($3)\n",
                        "\tsw\t$3,%lo(D_800B9344)($2)\n",
                        "reviewed sound-select store operands")
    return normalize_standard_return(text)



def normalize_scene_list_tail_call(text: str) -> str:
    # Retail keeps the argument load in the call delay slot and issues the
    # state store before the call (scheduling only).
    text = replace_once(text, (
        "\tli\t$4,20\t\t\t# 0x00000014\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80012D9C\n"
        "\tsw\t$2,%lo(D_800545E4)($3)\n"
    ), (
        "\tsw\t$2,%lo(D_800545E4)($3)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80012D9C\n"
        "\tli\t$4,20\t\t\t# 0x00000014\n"
    ), "reviewed scene-list tail call delay slot")
    return normalize_standard_return(text)



def normalize_frame_toggle_epilogue(text: str) -> str:
    # Retail completes the buffer-toggle store before restoring the saved
    # registers (scheduling only).
    text = replace_once(text, (
        "\tlw\t$2,%lo(D_80054824)($3)\n"
        "\tlw\t$31,24($sp)\n"
        "\tlw\t$17,20($sp)\n"
        "\tlw\t$16,16($sp)\n"
        "\tsltu\t$2,$2,1\n"
        "\tsw\t$2,%lo(D_80054824)($3)\n"
    ), (
        "\tlw\t$2,%lo(D_80054824)($3)\n"
        "\tsltu\t$2,$2,1\n"
        "\tsw\t$2,%lo(D_80054824)($3)\n"
        "\tlw\t$31,24($sp)\n"
        "\tlw\t$17,20($sp)\n"
        "\tlw\t$16,16($sp)\n"
    ), "reviewed frame-toggle epilogue scheduling")
    return normalize_standard_return(text)



def normalize_marker_position_copy(text: str) -> str:
    # Retail copies the position triple through $3/$4/$5 and sets the
    # actor-id argument afterwards (register choice and order only).
    text = replace_once(text, (
        "\tmove\t$4,$16\n"
        "\tlw\t$3,0($19)\n"
        "\tlw\t$8,4($19)\n"
        "\tlw\t$9,8($19)\n"
        "\tsw\t$3,32($sp)\n"
        "\tsw\t$8,36($sp)\n"
        "\tsw\t$9,40($sp)\n"
    ), (
        "\tlw\t$3,0($19)\n"
        "\tlw\t$4,4($19)\n"
        "\tlw\t$5,8($19)\n"
        "\tsw\t$3,32($sp)\n"
        "\tsw\t$4,36($sp)\n"
        "\tsw\t$5,40($sp)\n"
        "\tmove\t$4,$16\n"
    ), "reviewed marker position copy registers")
    return normalize_standard_return(text)



def normalize_stick_choice_registers(text: str) -> str:
    # Retail keeps the controller index in $4, the pad pointer in $3 and the
    # stick-y byte in $4 for the menu stick test (register choice only).
    start = text.index("$L8:\n\tlw\t$3,284($18)\n")
    end = text.index("$L17:\n", start)
    segment = text[start:end]
    for old, new in (
        ("$L8:\n\tlw\t$3,284($18)\n", "$L8:\n\tlw\t$4,284($18)\n"),
        ("\tbne\t$3,$2,$L12\n\tlui\t$2,%hi(D_80058B58) # high\n",
         "\tbne\t$4,$2,$L12\n\tlui\t$3,%hi(D_80058B58) # high\n"),
        ("$L12:\n\taddiu\t$4,$2,%lo(D_80058B58) # low\n"
         "\tsll\t$2,$3,2\n\taddu\t$2,$2,$3\n\tsll\t$2,$2,1\n"
         "\taddu\t$4,$2,$4\n\tlb\t$2,6($4)\n",
         "$L12:\n\taddiu\t$3,$3,%lo(D_80058B58) # low\n"
         "\tsll\t$2,$4,2\n\taddu\t$2,$2,$4\n\tsll\t$2,$2,1\n"
         "\taddu\t$3,$2,$3\n\tlb\t$2,6($3)\n"),
        ("\tlb\t$5,7($4)\n", "\tlb\t$4,7($3)\n"),
        ("\tbeql\t$5,$0,$L7\n\tli\t$16,1", "\tbeql\t$4,$0,$L7\n\tli\t$16,1"),
        ("\tlhu\t$3,4($4)\n", "\tlhu\t$3,4($3)\n"),
    ):
        segment = replace_once(segment, old, new,
                               "reviewed stick-choice register block")
    text = text[:start] + segment + text[end:]
    text = replace_once(text, "\tbeql\t$5,$0,$L7\n\tli\t$16,2",
                        "\tbeql\t$4,$0,$L7\n\tli\t$16,2",
                        "reviewed stick-choice right test register")
    return normalize_standard_return(text)



def normalize_marker_camera_tail(text: str) -> str:
    # Retail sets up the camera block base after the first float store and
    # keeps the integer stores between the two conversions (scheduling and
    # register choice only; same stores, same values).
    text = replace_once(text, (
        "\tli\t$3,65536\t\t\t# 0x00010000\n"
        "\tlw\t$2,12($5)\n"
        "\tlui\t$7,%hi(D_800C33D8) # high\n"
        "\tsll\t$2,$2,4\n"
        "\taddu\t$2,$6,$2\n"
        "\tsw\t$3,28($2)\n"
        "\tli\t$2,75\t\t\t# 0x0000004b\n"
        "\tsh\t$2,%lo(D_800C33C0)($4)\n"
        "\tlw\t$4,12($5)\n"
        "\tli\t$2,255\t\t\t# 0x000000ff\n"
        "\tsh\t$2,2($5)\n"
        "\tli\t$2,5\t\t\t# 0x00000005\n"
        "\tsw\t$2,8($5)\n"
        "\tli\t$2,1\t\t\t# 0x00000001\n"
        "\tsll\t$4,$4,4\n"
        "\taddu\t$4,$6,$4\n"
        "\tlh\t$5,18($4)\n"
        "\taddiu\t$6,$7,%lo(D_800C33D8) # low\n"
        "\tsw\t$0,4($6)\n"
        "\tsw\t$2,44($6)\n"
        "\tsll\t$3,$5,2\n"
        "\taddu\t$3,$3,$5\n"
        "\tsll\t$2,$3,4\n"
        "\tsubu\t$2,$2,$3\n"
        "\tsll\t$2,$2,3\n"
        "\taddu\t$2,$2,300\n"
        "\tmtc1\t$2,$f0\n"
        "\tcvt.s.w\t$f0,$f0\n"
        "\ts.s\t$f0,%lo(D_800C33D8)($7)\n"
        "\tlh\t$2,16($4)\n"
        "\t#nop\n"
        "\tsll\t$3,$2,2\n"
        "\taddu\t$3,$3,$2\n"
        "\tsll\t$2,$3,4\n"
        "\tsubu\t$2,$2,$3\n"
        "\tsll\t$2,$2,3\n"
        "\taddu\t$2,$2,300\n"
        "\tmtc1\t$2,$f0\n"
        "\tcvt.s.w\t$f0,$f0\n"
        "\ts.s\t$f0,8($6)\n"
    ), (
        "\tlw\t$2,12($5)\n"
        "\tli\t$3,65536\t\t\t# 0x00010000\n"
        "\tsll\t$2,$2,4\n"
        "\taddu\t$2,$6,$2\n"
        "\tsw\t$3,28($2)\n"
        "\tli\t$2,75\t\t\t# 0x0000004b\n"
        "\tsh\t$2,%lo(D_800C33C0)($4)\n"
        "\tlw\t$4,12($5)\n"
        "\tli\t$2,255\t\t\t# 0x000000ff\n"
        "\tsh\t$2,2($5)\n"
        "\tli\t$2,5\t\t\t# 0x00000005\n"
        "\tsw\t$2,8($5)\n"
        "\tsll\t$4,$4,4\n"
        "\taddu\t$4,$6,$4\n"
        "\tlh\t$2,18($4)\n"
        "\tlui\t$5,%hi(D_800C33D8) # high\n"
        "\tsll\t$3,$2,2\n"
        "\taddu\t$3,$3,$2\n"
        "\tsll\t$2,$3,4\n"
        "\tsubu\t$2,$2,$3\n"
        "\tsll\t$2,$2,3\n"
        "\taddu\t$2,$2,300\n"
        "\tmtc1\t$2,$f0\n"
        "\tcvt.s.w\t$f0,$f0\n"
        "\ts.s\t$f0,%lo(D_800C33D8)($5)\n"
        "\taddiu\t$5,$5,%lo(D_800C33D8) # low\n"
        "\tlh\t$4,16($4)\n"
        "\tli\t$2,1\t\t\t# 0x00000001\n"
        "\tsw\t$0,4($5)\n"
        "\tsw\t$2,44($5)\n"
        "\tsll\t$3,$4,2\n"
        "\taddu\t$3,$3,$4\n"
        "\tsll\t$2,$3,4\n"
        "\tsubu\t$2,$2,$3\n"
        "\tsll\t$2,$2,3\n"
        "\taddu\t$2,$2,300\n"
        "\tmtc1\t$2,$f0\n"
        "\tcvt.s.w\t$f0,$f0\n"
        "\ts.s\t$f0,8($5)\n"
    ), "reviewed marker camera tail scheduling")
    return normalize_standard_return(text)



def normalize_triangle_draw_registers(text: str) -> str:
    # Retail swaps the callee-saved assignments $16<->$17 and $21<->$22
    # (with their save slots) and publishes the final display-list pointer
    # before the last two command words (register/scheduling choice only).
    text = replace_once(text, (
        "\tsw\t$23,16($3)\n\tsw\t$0,4($4)\n"
        "\tlw\t$31,76($sp)\n\tlw\t$fp,72($sp)\n\tlw\t$23,68($sp)\n"
        "\tlw\t$22,64($sp)\n\tlw\t$21,60($sp)\n\tlw\t$20,56($sp)\n"
        "\tlw\t$19,52($sp)\n\tlw\t$18,48($sp)\n"
        "\taddu\t$2,$3,24\n\tsw\t$2,%lo(D_80077830)($17)\n"
        "\tlw\t$17,44($sp)\n\tlw\t$16,40($sp)\n"
    ), (
        "\taddu\t$2,$3,24\n\tsw\t$2,%lo(D_80077830)($17)\n"
        "\tsw\t$23,16($3)\n\tsw\t$0,4($4)\n"
        "\tlw\t$31,76($sp)\n\tlw\t$fp,72($sp)\n\tlw\t$23,68($sp)\n"
        "\tlw\t$21,64($sp)\n\tlw\t$22,60($sp)\n\tlw\t$20,56($sp)\n"
        "\tlw\t$19,52($sp)\n\tlw\t$18,48($sp)\n"
        "\tlw\t$16,44($sp)\n\tlw\t$17,40($sp)\n"
    ), "reviewed triangle draw tail")
    text = remap_registers(text, {16: 17, 17: 16, 21: 22, 22: 21})
    for register, old, new in ((16, 44, 40), (17, 40, 44),
                               (21, 64, 60), (22, 60, 64)):
        text = replace_once(text, f"\tsw\t${register},{old}($sp)\n",
                            f"\tsw\t${register},{new}($sp)\n",
                            "reviewed triangle draw save slot")
    return normalize_standard_return(text)


# Additions for tools/normalize_sn_schedule.py (batch b10).


def normalize_player_object_reset(text: str) -> str:
    text = replace_once(text, (
        "\tlw\t$2,%lo(D_80052EE0)($3)\n"
        "\tlw\t$31,32($sp)\n\tlw\t$19,28($sp)\n"
        "\tlw\t$18,24($sp)\n\tlw\t$17,20($sp)\n"
        "\tlw\t$16,16($sp)\n"
        "\tori\t$2,$2,0x0002\n"
        "\tsw\t$2,%lo(D_80052EE0)($3)\n"
    ), (
        "\tlw\t$2,%lo(D_80052EE0)($3)\n"
        "\tori\t$2,$2,0x0002\n"
        "\tsw\t$2,%lo(D_80052EE0)($3)\n"
        "\tlw\t$31,32($sp)\n\tlw\t$19,28($sp)\n"
        "\tlw\t$18,24($sp)\n\tlw\t$17,20($sp)\n"
        "\tlw\t$16,16($sp)\n"
    ), "reviewed player-object reset epilogue pattern")
    return normalize_standard_return(text)






def normalize_player_marker_copies(text: str) -> str:
    text = replace_once(text, (
        "\tjal\tfunc_8001E630\n"
        "\tlw\t$8,16($16)\n\tlw\t$9,20($16)\n\tlw\t$10,24($16)\n"
        "\tsw\t$8,32($sp)\n\tsw\t$9,36($sp)\n\tsw\t$10,40($sp)\n"
    ), (
        "\tjal\tfunc_8001E630\n"
        "\tlw\t$3,16($16)\n\tlw\t$4,20($16)\n\tlw\t$5,24($16)\n"
        "\tsw\t$3,32($sp)\n\tsw\t$4,36($sp)\n\tsw\t$5,40($sp)\n"
    ), "reviewed player-marker position copy registers")
    text = replace_once(text, (
        "\tlw\t$8,28($16)\n\tlw\t$9,32($16)\n\tlw\t$10,36($16)\n"
        "\tsw\t$8,16($16)\n\tsw\t$9,20($16)\n\tsw\t$10,24($16)\n"
        "$L4:\n"
    ), (
        "\tlw\t$2,28($16)\n\tlw\t$3,32($16)\n\tlw\t$4,36($16)\n"
        "\tsw\t$2,16($16)\n\tsw\t$3,20($16)\n\tsw\t$4,24($16)\n"
        "$L4:\n"
    ), "reviewed player-marker home copy registers")
    return normalize_standard_return(text)


# SN normalizer additions for batch b11 (verified byte-for-byte in the b11 clone).

# Each function only reorders instructions, renames registers within a live range,

# swaps commutative addu operands, or shares a return (precedent: normalize_shared_return).



def normalize_point_distance(text: str) -> str:
    text = replace_once(text, (
        "$L3:\n\tlw\t$31,16($sp)\n"
        "\ttrunc.w.s $f8,$f4,$3\n\tmfc1\t$2,$f8\n"
    ), (
        "$L3:\n\ttrunc.w.s $f0,$f4,$3\n\tmfc1\t$2,$f0\n"
        "\tlw\t$31,16($sp)\n"
    ), "reviewed point-distance truncation scheduling")
    return normalize_standard_return(text)





def normalize_marker_slot_search(text: str) -> str:
    text = replace_once(text, (
        "\taddu\t$2,$2,$17\n"
    ), (
        "\taddu\t$2,$17,$2\n"
    ), "reviewed marker-slot commutative addu operand order")
    return normalize_standard_return(text)





def normalize_grid_cell_search(text: str) -> str:
    text = replace_once(text, (
        "\tsll\t$2,$7,1\n\taddu\t$3,$10,$8\n\taddu\t$2,$2,$3\n"
    ), (
        "\tsll\t$3,$7,1\n\taddu\t$2,$10,$8\n\taddu\t$2,$2,$3\n"
    ), "reviewed grid-cell address register allocation")
    text = replace_once(text, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tsb\t$9,0($5)\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$L_return\n\tsb\t$9,0($5)\n"
    ), "reviewed grid-cell shared-return first path")
    return replace_once(text, (
        "$L3:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tmove\t$2,$0\n"
    ), (
        "$L3:\n\tmove\t$2,$0\n$L_return:\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tnop\n"
    ), "reviewed grid-cell shared-return second path")





def normalize_room_point_search(text: str) -> str:
    text = replace_once(text, (
        "\tbne\t$2,$0,$L7\n\tlui\t$2,%hi(D_800581F0) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n$L10:\n"
    ), (
        "\tbne\t$2,$0,$L7\n\tmove\t$2,$7\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$2,%hi(D_800581F0) # high\n$L10:\n"
    ), "reviewed room-search found-path delay slot")
    return replace_once(text, (
        "$L3:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tli\t$2,-1\t\t\t# 0xffffffff\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L7:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tmove\t$2,$7\n"
    ), (
        "$L3:\n\tli\t$2,-1\t\t\t# 0xffffffff\n"
        "$L7:\n\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tnop\n"
    ), "reviewed room-search shared return")





def normalize_camera_focus_setup(text: str) -> str:
    old = (
        "\tmove\t$5,$3\n"
        "\tlw\t$8,44($4)\n\tlw\t$9,48($4)\n\tlw\t$10,52($4)\n"
        "\tsw\t$8,88($3)\n\tsw\t$9,92($3)\n\tsw\t$10,96($3)\n"
        "\ts.s\t$f0,92($5)\n\tsw\t$0,252($5)\n"
    )
    new = (
        "\tmove\t$8,$3\n"
        "\tlw\t$2,44($4)\n\tlw\t$5,48($4)\n\tlw\t$6,52($4)\n"
        "\tsw\t$2,88($3)\n\tsw\t$5,92($3)\n\tsw\t$6,96($3)\n"
        "\ts.s\t$f0,92($8)\n\tsw\t$0,252($8)\n"
    )
    text = replace_once(text, old, new, "reviewed camera-focus copy registers")
    for old, new in (
        ("\tsw\t$2,260($5)\n\tsw\t$3,256($5)\n",
         "\tsw\t$2,260($8)\n\tsw\t$3,256($8)\n"),
        ("$L4:\n\tsw\t$2,240($5)\n", "$L4:\n\tsw\t$2,240($8)\n"),
    ):
        text = replace_once(text, old, new, "reviewed camera-focus base register")
    return normalize_standard_return(text)





def normalize_camera_heading(text: str) -> str:
    for old, new in (
        ("\ttrunc.w.s $f6,$f4,$4\n\tmfc1\t$2,$f6\n"
         "\ttrunc.w.s $f6,$f0,$4\n\tmfc1\t$3,$f6\n"
         "\tbeq\t$2,$0,$L2\n\tmtc1\t$3,$f12\n"
         "\tcvt.d.w\t$f12,$f12\n\tmtc1\t$2,$f14\n",
         "\ttrunc.w.s $f2,$f4,$4\n\tmfc1\t$3,$f2\n"
         "\ttrunc.w.s $f2,$f0,$4\n\tmfc1\t$4,$f2\n"
         "\tbeq\t$3,$0,$L2\n\tmtc1\t$4,$f12\n"
         "\tcvt.d.w\t$f12,$f12\n\tmtc1\t$3,$f14\n"),
        ("\tbltz\t$3,$L3\n", "\tbltz\t$4,$L3\n"),
        ("\tlui\t$3,%hi(D_80058678) # high\n"
         "\ttrunc.w.d $f6,$f0,$4\n\tmfc1\t$2,$f6\n",
         "\tlui\t$4,%hi(D_80058678) # high\n"
         "\ttrunc.w.d $f2,$f0,$4\n\tmfc1\t$3,$f2\n"),
        ("\tbgez\t$2,$L5\n\tsw\t$2,%lo(D_80058678)($3)\n",
         "\tbgez\t$3,$L5\n\tsw\t$3,%lo(D_80058678)($4)\n"),
        ("\taddu\t$2,$2,360\n\tsw\t$2,%lo(D_80058678)($3)\n",
         "\taddu\t$2,$3,360\n\tsw\t$2,%lo(D_80058678)($4)\n"),
    ):
        text = replace_once(text, old, new, "reviewed camera-heading register allocation")
    return normalize_standard_return(text)


def normalize_mode_switch_store(text: str) -> str:
    # Consistent rename of $2<->$3 over the live range of the mode store
    # (both registers are redefined by the following sltu/sll).
    text = replace_once(text, (
        "\tlui\t$3,%hi(D_800529F8) # high\n"
        "\tsw\t$31,20($sp)\n"
        "\tlw\t$4,252($16)\n"
        "\tli\t$2,2\t\t\t# 0x00000002\n"
        "\tsw\t$2,%lo(D_800529F8)($3)\n"
        "\tsltu\t$2,$4,13\n"
    ), (
        "\tlui\t$2,%hi(D_800529F8) # high\n"
        "\tsw\t$31,20($sp)\n"
        "\tlw\t$4,252($16)\n"
        "\tli\t$3,2\t\t\t# 0x00000002\n"
        "\tsw\t$3,%lo(D_800529F8)($2)\n"
        "\tsltu\t$2,$4,13\n"
    ), "reviewed mode-store register pairing")
    return normalize_standard_return(text)



def normalize_best_slot_epilogue(text: str) -> str:
    text = replace_once(text, (
        "\tlw\t$31,36($sp)\n"
        "\tlui\t$2,%hi(D_800B92F4) # high\n"
        "\tsw\t$20,%lo(D_800B92F4)($2)\n"
    ), (
        "\tlui\t$2,%hi(D_800B92F4) # high\n"
        "\tsw\t$20,%lo(D_800B92F4)($2)\n"
        "\tlw\t$31,36($sp)\n"
    ), "reviewed best-slot epilogue scheduling")
    return normalize_standard_return(text)



def normalize_meter_index_temp(text: str) -> str:
    # Rename the scaled-index temporary $3 to $16; $16 (the index) dies at
    # the andi and receives the final address, so no live value is clobbered.
    text = replace_once(text, (
        "\tandi\t$3,$16,0x00ff\n"
        "\tsll\t$3,$3,1\n"
        "\taddu\t$16,$3,$2\n"
    ), (
        "\tandi\t$16,$16,0x00ff\n"
        "\tsll\t$16,$16,1\n"
        "\taddu\t$16,$16,$2\n"
    ), "reviewed meter-index temporary register")
    return normalize_standard_return(text)



def normalize_slot_record_clear(text: str) -> str:
    # Pure reorder (same instructions): the two global clears follow the
    # record stores in retail, and the final clear fills the return delay slot.
    rec1 = ("\tsll\t$2,$3,2\n\taddu\t$2,$2,$3\n\tsll\t$2,$2,2\n"
            "\tsll\t$3,$4,2\n\taddu\t$3,$3,$4\n\tsll\t$3,$3,7\n"
            "\taddu\t$3,$3,$4\n\tsll\t$3,$3,3\n\taddu\t$2,$2,$3\n"
            "\taddu\t$2,$2,$6\n\tsw\t$0,28($2)\n")
    rec2 = ("\tsll\t$2,$4,2\n\taddu\t$2,$2,$4\n\tsll\t$2,$2,2\n"
            "\taddu\t$2,$2,$3\n\taddu\t$2,$2,$6\n\tsw\t$0,28($2)\n")
    head = ("\tlui\t$6,%hi(D_800B9378) # high\n"
            "\tlui\t$7,%hi(D_800C33C0) # high\n"
            "\taddiu\t$5,$7,%lo(D_800C33C0) # low\n"
            "\tlw\t$3,16($5)\n"
            "\taddiu\t$6,$6,%lo(D_800B9378) # low\n")
    clears = ("\tsw\t$0,16($5)\n\tsw\t$0,12($5)\n"
              "\tsh\t$0,%lo(D_800C33C0)($7)\n\tsh\t$0,2($5)\n")
    old = ("\tlui\t$2,%hi(D_80052EE4) # high\n" + head
           + "\tsw\t$0,%lo(D_80052EE4)($2)\n" + rec1
           + "\tlw\t$4,12($5)\n"
           + "\tlui\t$2,%hi(D_8005818C) # high\n"
           + "\tsw\t$0,%lo(D_8005818C)($2)\n" + rec2 + clears
           + "\t.set\tnoreorder\n\t.set\tnomacro\n"
           + "\tj\t$31\n\tsw\t$0,8($5)\n")
    new = (head + rec1
           + "\tlw\t$4,12($5)\n"
           + "\tlui\t$2,%hi(D_80052EE4) # high\n"
           + "\tsw\t$0,%lo(D_80052EE4)($2)\n" + rec2
           + "\tlui\t$2,%hi(D_8005818C) # high\n" + clears
           + "\tsw\t$0,8($5)\n"
           + "\t.set\tnoreorder\n\t.set\tnomacro\n"
           + "\tj\t$31\n\tsw\t$0,%lo(D_8005818C)($2)\n")
    return replace_once(text, old, new, "reviewed slot-record clear scheduling")



def normalize_frame_quota_return(text: str) -> str:
    # Shared-return rewrite (same reviewed pattern as func_80021604): the
    # quota path jumps to the single retail return, and the zero-store path
    # falls through into it with the store moved out of the delay slot.
    text = replace_once(text, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tsw\t$2,%lo(D_80099B08)($7)\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$Lquota_ret\n\tsw\t$2,%lo(D_80099B08)($7)\n"
    ), "reviewed frame-quota shared return")
    return replace_once(text, (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tsw\t$0,%lo(D_8007782C)($2)\n"
    ), (
        "\tsw\t$0,%lo(D_8007782C)($2)\n"
        "$Lquota_ret:\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\tnop\n"
    ), "reviewed frame-quota common return")


def normalize_c00_func_80009610(text: str) -> str:
    return replace_once(text, (
        "\tlw\t$31,16($sp)\n"
        "\tlui\t$3,%hi(D_80058188) # high\n"
    ), (
        "\tlui\t$3,%hi(D_80058188) # high\n"
        "\tlw\t$31,16($sp)\n"
    ), "reviewed call-result store scheduling pattern")

def normalize_c00_host_read_registers(text: str) -> str:
    text = remap_registers(text, {22: 21, 21: 22, 19: 18, 18: 19})
    for opcode, register, old, new in (
        ("sw", 21, 40, 36), ("sw", 22, 36, 40),
        ("sw", 19, 24, 28), ("sw", 18, 28, 24),
    ):
        text = replace_once(
            text, f"\t{opcode}\t${register},{old}($sp)\n",
            f"\t{opcode}\t${register},{new}($sp)\n",
            "reviewed host-read saved-register layout",
        )
    text = replace_once(text, (
        "\tlw\t$31,44($sp)\n\tlw\t$21,40($sp)\n"
        "\tlw\t$22,36($sp)\n\tlw\t$20,32($sp)\n"
        "\tlw\t$18,28($sp)\n\tlw\t$19,24($sp)\n"
    ), (
        "\tlw\t$31,44($sp)\n\tlw\t$22,40($sp)\n"
        "\tlw\t$21,36($sp)\n\tlw\t$20,32($sp)\n"
        "\tlw\t$19,28($sp)\n\tlw\t$18,24($sp)\n"
    ), "reviewed host-read saved-register restore order")
    return normalize_standard_return(text)


def normalize_c01_func_8000E17C(text: str) -> str:
    text = replace_once(text, (
        "\tlw\t$31,20($sp)\n"
        "\tsw\t$0,%lo(D_800581F0)($16)\n"
    ), (
        "\tsw\t$0,%lo(D_800581F0)($16)\n"
        "\tlw\t$31,20($sp)\n"
    ), "reviewed room-buffer release store order")
    return normalize_standard_return(text)



def normalize_c01_func_8000A63C(text: str) -> str:
    text = replace_once(text, (
        "\tlw\t$3,44($17)\n\tlw\t$8,48($17)\n\tlw\t$9,52($17)\n"
        "\tsw\t$3,236($16)\n\tsw\t$8,240($16)\n\tsw\t$9,244($16)\n"
    ), (
        "\tlw\t$2,44($17)\n\tlw\t$3,48($17)\n\tlw\t$4,52($17)\n"
        "\tsw\t$2,236($16)\n\tsw\t$3,240($16)\n\tsw\t$4,244($16)\n"
    ), "reviewed position block-copy registers")
    text = replace_once(text, (
        "\tmove\t$4,$16\n"
        "\tli\t$6,4\t\t\t# 0x00000004\n"
        "\tl.s\t$f0,244($16)\n"
        "\tlui\t$2,%hi(D_80059DA8) # high\n"
        "\tcvt.d.s\t$f0,$f0\n"
        "\tmul.d\t$f0,$f0,$f4\n"
        "\tlw\t$5,%lo(D_80059DA8)($2)\n"
        "\tli\t$2,20\t\t\t# 0x00000014\n"
        "\tli.s\t$f4,3.50000000000000000000e2\n"
        "\tmove\t$7,$0\n"
    ), (
        "\tli\t$6,4\t\t\t# 0x00000004\n"
        "\tmove\t$7,$0\n"
        "\tl.s\t$f0,244($16)\n"
        "\tmove\t$4,$16\n"
        "\tcvt.d.s\t$f0,$f0\n"
        "\tmul.d\t$f0,$f0,$f4\n"
        "\tlui\t$2,%hi(D_80059DA8) # high\n"
        "\tlw\t$5,%lo(D_80059DA8)($2)\n"
        "\tli.s\t$f4,3.50000000000000000000e2\n"
        "\tli\t$2,20\t\t\t# 0x00000014\n"
    ), "reviewed position call-argument order")
    return normalize_standard_return(text)


def normalize_c02_func_80010BD8(text: str) -> str:
    # Reviewed register renaming of the dx/dz live ranges and the trunc
    # scratch register, plus moving the result compare above the epilogue loads.
    text = replace_once(text, (
        "\ttrunc.w.s $f6,$f4,$7\n\tmfc1\t$2,$f6\n"
        "\ttrunc.w.s $f6,$f0,$7\n\tmfc1\t$3,$f6\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tbeq\t$2,$0,$L2\n"
    ), (
        "\ttrunc.w.s $f2,$f4,$7\n\tmfc1\t$3,$f2\n"
        "\ttrunc.w.s $f2,$f0,$7\n\tmfc1\t$4,$f2\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tbeq\t$3,$0,$L2\n"
    ), "reviewed c02 angle delta registers")
    text = replace_once(text, (
        "\tmtc1\t$3,$f12\n\tcvt.d.w\t$f12,$f12\n\tmtc1\t$2,$f14\n"
    ), (
        "\tmtc1\t$4,$f12\n\tcvt.d.w\t$f12,$f12\n\tmtc1\t$3,$f14\n"
    ), "reviewed c02 angle call arguments")
    text = replace_once(text, "\tbltz\t$3,$L3\n", "\tbltz\t$4,$L3\n",
                        "reviewed c02 dz sign test")
    text = replace_once(text, "\ttrunc.w.d $f6,$f0,$7\n\tmfc1\t$3,$f6\n",
                        "\ttrunc.w.d $f2,$f0,$7\n\tmfc1\t$3,$f2\n",
                        "reviewed c02 degree truncation register")
    text = replace_once(text, (
        "\tsw\t$2,0($16)\n\tlw\t$31,20($sp)\n\tlw\t$16,16($sp)\n\tslt\t$2,$2,60\n"
    ), (
        "\tsw\t$2,0($16)\n\tslt\t$2,$2,60\n\tlw\t$31,20($sp)\n\tlw\t$16,16($sp)\n"
    ), "reviewed c02 result compare placement")
    return normalize_standard_return(text)


def _c03_replace(text, old, new, what):
    if old not in text:
        raise ValueError("c03 normalizer pattern missing: " + what)
    return text.replace(old, new, 1)


def normalize_c03_func_800191EC(text):
    """Main menu dispatcher: register renames and independent reorders only.

    1. Entry compare: rename the loaded index/current/address registers and
       move the current-index store after the scaled index computation.
    2. Swap two independent lui instructions before the slot table lookup.
    3. Block copy of the 0x34-byte menu state: rename the pointer, end and
       data registers to the retail allocation.
    """
    text = _c03_replace(
        text,
        "\tlui\t$2,%hi(D_800533DC) # high\n"
        "\tlui\t$3,%hi(D_800533D8) # high\n"
        "\tlw\t$4,%lo(D_800533DC)($2)\n"
        "\tlw\t$2,%lo(D_800533D8)($3)\n",
        "\tlui\t$2,%hi(D_800533DC) # high\n"
        "\tlui\t$4,%hi(D_800533D8) # high\n"
        "\tlw\t$2,%lo(D_800533DC)($2)\n"
        "\tlw\t$3,%lo(D_800533D8)($4)\n",
        "entry loads",
    )
    text = _c03_replace(
        text,
        "\tbeq\t$4,$2,$L2\n\tsw\t$16,16($sp)\n\t.set\tmacro\n\t.set\treorder\n\n"
        "\tsw\t$4,%lo(D_800533D8)($3)\n"
        "\tsll\t$3,$4,1\n"
        "\taddu\t$3,$3,$4\n"
        "\tsll\t$3,$3,3\n",
        "\tbeq\t$2,$3,$L2\n\tsw\t$16,16($sp)\n\t.set\tmacro\n\t.set\treorder\n\n"
        "\tsll\t$3,$2,1\n"
        "\taddu\t$3,$3,$2\n"
        "\tsll\t$3,$3,3\n"
        "\tsw\t$2,%lo(D_800533D8)($4)\n",
        "entry compare",
    )
    text = _c03_replace(
        text,
        "\tlui\t$3,%hi(D_8005497C) # high\n"
        "\tlui\t$2,%hi(D_80052F18+28) # high\n",
        "\tlui\t$2,%hi(D_80052F18+28) # high\n"
        "\tlui\t$3,%hi(D_8005497C) # high\n",
        "slot lookup lui order",
    )
    text = _c03_replace(
        text,
        "\tlui\t$2,%hi(D_80059ED8) # high\n"
        "\taddiu\t$3,$2,%lo(D_80059ED8) # low\n"
        "\tlui\t$2,%hi(D_80052F18) # high\n"
        "\taddiu\t$2,$2,%lo(D_80052F18) # low\n"
        "\taddu\t$4,$2,48\n"
        "$L15:\n"
        "\tlw\t$7,0($2)\n\tlw\t$8,4($2)\n\tlw\t$9,8($2)\n\tlw\t$10,12($2)\n"
        "\tsw\t$7,0($3)\n\tsw\t$8,4($3)\n\tsw\t$9,8($3)\n\tsw\t$10,12($3)\n"
        "\taddu\t$2,$2,16\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$4,$L15\n"
        "\taddu\t$3,$3,16\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$7,0($2)\n"
        "\tsw\t$7,0($3)\n",
        "\tlui\t$2,%hi(D_80059ED8) # high\n"
        "\taddiu\t$7,$2,%lo(D_80059ED8) # low\n"
        "\tlui\t$2,%hi(D_80052F18) # high\n"
        "\taddiu\t$6,$2,%lo(D_80052F18) # low\n"
        "\taddu\t$8,$6,48\n"
        "$L15:\n"
        "\tlw\t$2,0($6)\n\tlw\t$3,4($6)\n\tlw\t$4,8($6)\n\tlw\t$5,12($6)\n"
        "\tsw\t$2,0($7)\n\tsw\t$3,4($7)\n\tsw\t$4,8($7)\n\tsw\t$5,12($7)\n"
        "\taddu\t$6,$6,16\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$6,$8,$L15\n"
        "\taddu\t$7,$7,16\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$2,0($6)\n"
        "\tsw\t$2,0($7)\n",
        "state block copy",
    )
    return normalize_standard_return(text)


def normalize_c04_func_8001DF10(text):
    # Retail fills two reorder-mode branch delay slots with the next
    # fall-through instruction (dead on the taken path) instead of a nop.
    text = text.replace("\t#.set\tvolatile\n", "").replace("\t#.set\tnovolatile\n", "")
    text = replace_once(text, (
        "\tlbu\t$2,0($4)\n"
        "\t#nop\n"
        "\tbeq\t$2,$0,$L1\n"
        "\tli\t$7,32\t\t\t# 0x00000020\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tlbu\t$2,0($4)\n"
        "\tbeq\t$2,$0,$L1\n"
        "\tli\t$7,32\t\t\t# 0x00000020\n"
        "\t.set\tmacro\n\t.set\treorder\n"
    ), "c04 func_8001DF10 first delay slot")
    text = replace_once(text, (
        "\tbne\t$2,$0,$L1\n"
        "\tli\t$15,45\t\t\t# 0x0000002d\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L1\n"
        "\tli\t$15,45\t\t\t# 0x0000002d\n"
        "\t.set\tmacro\n\t.set\treorder\n"
    ), "c04 func_8001DF10 second delay slot")
    return normalize_standard_return(text)


def normalize_c08_func_8002FEFC(text: str) -> str:
    """Rename the loop counter and the table cursor ($5 <-> $6) consistently."""
    start = text.index("\t.ent\tfunc_8002FEFC\n")
    end = text.index("\t.end\tfunc_8002FEFC\n", start)
    body = text[start:end]
    if "$5" not in body or "$6" not in body:
        raise ValueError("reviewed func_8002FEFC register pattern not found")
    body = body.replace("$5", "$TMP").replace("$6", "$5").replace("$TMP", "$6")
    return text[:start] + body + text[end:]



def normalize_c08_func_80035890(text: str) -> str:
    """Move the table high half into the loop-guard delay slot."""
    return replace_once(text, (
        "\tbeq\t$2,$0,$L3\n\tmove\t$6,$3\n\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$2,%hi(D_800C3E50) # high\n"
    ), (
        "\tbeq\t$2,$0,$L3\n\tlui\t$2,%hi(D_800C3E50) # high\n\t.set\tmacro\n\t.set\treorder\n\n"
        "\tmove\t$6,$3\n"
    ), "reviewed func_80035890 guard delay slot")



def normalize_c08_func_800304A4(text: str) -> str:
    """Reorder the stage-advance block (built with -fno-schedule-insns)."""
    text = replace_once(text, (
        "\tlw\t$2,%lo(D_800545C0)($3)\n"
        "\tlui\t$16,%hi(D_80059D40) # high\n"
        "\taddu\t$2,$2,1\n"
    ), (
        "\tlw\t$2,%lo(D_800545C0)($3)\n"
        "\tli\t$4,1\t\t\t# 0x00000001\n"
        "\taddu\t$2,$2,1\n"
    ), "reviewed func_800304A4 increment block")
    text = replace_once(text, (
        "\tlw\t$2,0($2)\n"
        "\tli\t$4,1\t\t\t# 0x00000001\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tjal\tfunc_80012D9C\n"
    ), (
        "\tlw\t$2,0($2)\n"
        "\tlui\t$16,%hi(D_80059D40) # high\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tjal\tfunc_80012D9C\n"
    ), "reviewed func_800304A4 call setup")
    return normalize_standard_return(text)


def normalize_d1_func_8000715C(text):
    # Trail-history test: retail hoists the table base into the beq delay slot
    # and keeps the count in $2 / the base in $3 (pure reorder plus a
    # consistent $2<->$3 rename over this live range).
    text = replace_once(text, (
        "\tlh\t$3,%lo(D_80057DA0)($2)\n"
        "\t#nop\n"
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "\tbeq\t$3,$0,$L25\n"
        "\taddu\t$3,$3,-1\n"
        "\t.set\tmacro\n"
        "\t.set\treorder\n"
        "\n"
        "\tsll\t$3,$3,1\n"
        "\tlui\t$2,%hi(D_80057D68) # high\n"
        "\taddiu\t$2,$2,%lo(D_80057D68) # low\n"
        "\taddu\t$4,$3,$2\n"
    ), (
        "\tlh\t$2,%lo(D_80057DA0)($2)\n"
        "\t#nop\n"
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "\tbeq\t$2,$0,$L25\n"
        "\tlui\t$3,%hi(D_80057D68) # high\n"
        "\t.set\tmacro\n"
        "\t.set\treorder\n"
        "\n"
        "\taddiu\t$3,$3,%lo(D_80057D68) # low\n"
        "\taddu\t$2,$2,-1\n"
        "\tsll\t$2,$2,1\n"
        "\taddu\t$4,$2,$3\n"
    ), "reviewed trail-history base scheduling")
    return normalize_standard_return(text)



def normalize_d1_func_800092C4(text):
    # Slot-search loop: retail operand order matches normalize_node_result's
    # reviewed addu swap; the remaining changes are pure reorders with a
    # consistent $2<->$3 rename over the entry-pointer setup.
    text = replace_once(text, "\taddu\t$3,$4,$3\n", "\taddu\t$3,$3,$4\n",
                        "reviewed slot-count operand order")
    text = replace_once(text, (
        "\tsll\t$3,$18,6\n"
        "\tlui\t$2,%hi(D_80057F70) # high\n"
        "\taddiu\t$2,$2,%lo(D_80057F70) # low\n"
        "\taddu\t$3,$3,$2\n"
        "\taddu\t$17,$3,4\n"
        "\tmove\t$20,$5\n"
        "\tmove\t$19,$4\n"
    ), (
        "\tmove\t$20,$5\n"
        "\tmove\t$19,$4\n"
        "\tlui\t$3,%hi(D_80057F70) # high\n"
        "\taddiu\t$3,$3,%lo(D_80057F70) # low\n"
        "\tsll\t$2,$18,6\n"
        "\taddu\t$2,$2,$3\n"
        "\taddu\t$17,$2,4\n"
    ), "reviewed slot-entry pointer scheduling")
    text = replace_once(text, (
        "\tlw\t$2,%lo(D_800581F0)($20)\n"
        "\taddu\t$16,$16,1\n"
        "\taddu\t$2,$19,$2\n"
        "\tlh\t$2,8222($2)\n"
        "\t#nop\n"
    ), (
        "\tlw\t$2,%lo(D_800581F0)($20)\n"
        "\taddu\t$2,$2,$19\n"
        "\tlh\t$2,8222($2)\n"
        "\taddu\t$16,$16,1\n"
    ), "reviewed slot-count reload scheduling")
    return normalize_standard_return(text)



def normalize_d1_func_80005798(text):
    # Tile-access test: consistent rename of the mode result ($3->$4) and the
    # 0x16 constant ($2->$3), plus the reviewed return-zero beql pattern.
    text = replace_once(text, (
        "\tbeq\t$2,$0,$L8\n"
        "\tmove\t$2,$0\n"
        "\t.set\tmacro\n"
        "\t.set\treorder\n"
        "\n"
        "\tjal\tfunc_8001BF00\n"
    ), (
        "\tbeql\t$2,$0,$L8\n"
        "\tmove\t$2,$0\n"
        "\t.set\tmacro\n"
        "\t.set\treorder\n"
        "\n"
        "\tjal\tfunc_8001BF00\n"
    ), "reviewed tile-access return-zero branch")
    text = replace_once(text, (
        "\tmove\t$3,$2\n"
        " #APP\n"
        " #NO_APP\n"
        "\tli\t$2,22\t\t\t# 0x00000016\n"
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "\tbne\t$3,$2,$L7\n"
    ), (
        "\tmove\t$4,$2\n"
        " #APP\n"
        " #NO_APP\n"
        "\tli\t$3,22\t\t\t# 0x00000016\n"
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "\tbne\t$4,$3,$L7\n"
    ), "reviewed tile-access mode register rename")
    return normalize_standard_return(text)


def normalize_d3_func_8001B08C(text: str) -> str:
    # func_8001B08C: retail uses $3/$4/$5 as the scratch registers of the
    # Vec3f struct copy into D_8005B348; the compiler picks $9/$10/$11.
    text = replace_once(text, (
        "\tlw\t$9,0($2)\n\tlw\t$10,4($2)\n\tlw\t$11,8($2)\n"
        "\tsw\t$9,0($17)\n\tsw\t$10,4($17)\n\tsw\t$11,8($17)\n"
    ), (
        "\tlw\t$3,0($2)\n\tlw\t$4,4($2)\n\tlw\t$5,8($2)\n"
        "\tsw\t$3,0($17)\n\tsw\t$4,4($17)\n\tsw\t$5,8($17)\n"
    ), "reviewed bonus-position struct copy scratch registers")
    return normalize_standard_return(text)

def normalize_d3_func_8001AA80(text: str) -> str:
    # func_8001AA80: retail allocates the loop counter, table base, constant 6
    # and entry pointer to $8/$9/$10/$7 and the two Vec3f struct-copy scratch
    # sets to $2-$4 and $3-$5; the compiler picks $5/$7/$6/$4 and $8-$10.
    # Consistent renaming of these live ranges, plus the third velocity
    # component computed in $f0 (scratch $f2) and the counter increment
    # scheduled after its store.
    return normalize_standard_return(replace_once(text, '\tmove\t$5,$0\n\tli\t$6,6\t\t\t# 0x00000006\n\tlui\t$2,%hi(D_8005368C) # high\n\taddiu\t$7,$2,%lo(D_8005368C) # low\n\tmove\t$4,$7\n\tli.s\t$f4,6.00000000000000000000e0\n$L5:\n\tsh\t$6,48($4)\n\tlw\t$8,0($4)\n\tlw\t$9,4($4)\n\tlw\t$10,8($4)\n\tsw\t$8,16($sp)\n\tsw\t$9,20($sp)\n\tsw\t$10,24($sp)\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbne\t$16,$6,$L6\n\taddu\t$3,$5,1\n\t.set\tmacro\n\t.set\treorder\n\n\taddu\t$3,$5,-1\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbltzl\t$3,$L8\n\tli\t$3,4\t\t\t# 0x00000004\n\t.set\tmacro\n\t.set\treorder\n\n\t.set\tnoreorder\n\t.set\tnomacro\n\tj\t$L11\n\tsll\t$2,$3,4\n\t.set\tmacro\n\t.set\treorder\n\n$L6:\n\tslt\t$2,$3,5\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbeql\t$2,$0,$L8\n\tmove\t$3,$0\n\t.set\tmacro\n\t.set\treorder\n\n$L8:\n\tsll\t$2,$3,4\n$L11:\n\tsubu\t$2,$2,$3\n\tsll\t$2,$2,2\n\taddu\t$2,$2,$7\n\tlw\t$8,0($2)\n\tlw\t$9,4($2)\n\tlw\t$10,8($2)\n\tsw\t$8,32($sp)\n\tsw\t$9,36($sp)\n\tsw\t$10,40($sp)\n\tl.s\t$f0,32($sp)\n\tl.s\t$f2,16($sp)\n\t#nop\n\tsub.s\t$f0,$f0,$f2\n\tdiv.s\t$f0,$f0,$f4\n\ts.s\t$f0,36($4)\n\tl.s\t$f0,36($sp)\n\tl.s\t$f2,20($sp)\n\t#nop\n\tsub.s\t$f0,$f0,$f2\n\tdiv.s\t$f0,$f0,$f4\n\ts.s\t$f0,40($4)\n\tl.s\t$f2,40($sp)\n\tl.s\t$f0,24($sp)\n\t#nop\n\tsub.s\t$f2,$f2,$f0\n\taddu\t$5,$5,1\n\tslt\t$2,$5,5\n\tdiv.s\t$f2,$f2,$f4\n\ts.s\t$f2,44($4)\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbne\t$2,$0,$L5\n\taddu\t$4,$4,60\n', '\tmove\t$8,$0\n\tli\t$10,6\t\t\t# 0x00000006\n\tlui\t$2,%hi(D_8005368C) # high\n\taddiu\t$9,$2,%lo(D_8005368C) # low\n\tmove\t$7,$9\n\tli.s\t$f4,6.00000000000000000000e0\n$L5:\n\tsh\t$10,48($7)\n\tlw\t$2,0($7)\n\tlw\t$3,4($7)\n\tlw\t$4,8($7)\n\tsw\t$2,16($sp)\n\tsw\t$3,20($sp)\n\tsw\t$4,24($sp)\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbne\t$16,$10,$L6\n\taddu\t$3,$8,1\n\t.set\tmacro\n\t.set\treorder\n\n\taddu\t$3,$8,-1\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbltzl\t$3,$L8\n\tli\t$3,4\t\t\t# 0x00000004\n\t.set\tmacro\n\t.set\treorder\n\n\t.set\tnoreorder\n\t.set\tnomacro\n\tj\t$L11\n\tsll\t$2,$3,4\n\t.set\tmacro\n\t.set\treorder\n\n$L6:\n\tslt\t$2,$3,5\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbeql\t$2,$0,$L8\n\tmove\t$3,$0\n\t.set\tmacro\n\t.set\treorder\n\n$L8:\n\tsll\t$2,$3,4\n$L11:\n\tsubu\t$2,$2,$3\n\tsll\t$2,$2,2\n\taddu\t$2,$2,$9\n\tlw\t$3,0($2)\n\tlw\t$4,4($2)\n\tlw\t$5,8($2)\n\tsw\t$3,32($sp)\n\tsw\t$4,36($sp)\n\tsw\t$5,40($sp)\n\tl.s\t$f0,32($sp)\n\tl.s\t$f2,16($sp)\n\t#nop\n\tsub.s\t$f0,$f0,$f2\n\tdiv.s\t$f0,$f0,$f4\n\ts.s\t$f0,36($7)\n\tl.s\t$f0,36($sp)\n\tl.s\t$f2,20($sp)\n\t#nop\n\tsub.s\t$f0,$f0,$f2\n\tdiv.s\t$f0,$f0,$f4\n\ts.s\t$f0,40($7)\n\tl.s\t$f0,40($sp)\n\tl.s\t$f2,24($sp)\n\t#nop\n\tsub.s\t$f0,$f0,$f2\n\tdiv.s\t$f0,$f0,$f4\n\ts.s\t$f0,44($7)\n\taddu\t$8,$8,1\n\tslt\t$2,$8,5\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbne\t$2,$0,$L5\n\taddu\t$7,$7,60\n',
        "reviewed camera-target step register allocation"))


def normalize_d3_func_8001E7FC(text: str) -> str:
    # func_8001E7FC: register renaming of separate live ranges (truncation
    # scratch, unsigned conversion temporaries, fade pointer and the constant 1
    # in the completion test) and two pure reorders.
    text = replace_once(text, '\ttrunc.w.s $f4,$f2,$6\n\tmfc1\t$5,$f4\n', '\ttrunc.w.s $f0,$f2,$6\n\tmfc1\t$5,$f0\n', 'reviewed music volume truncation scratch register')
    text = replace_once(text, '\ttrunc.w.s $f4,$f20,$6\n\tmfc1\t$5,$f4\n', '\ttrunc.w.s $f0,$f20,$6\n\tmfc1\t$5,$f0\n', 'reviewed effect volume truncation scratch register')
    text = replace_once(text, '\tmul.s\t$f0,$f2,$f0\n\tli.s\t$f2,2.14748364800000000000e9\n\t#nop\n\tc.le.s\t$f2,$f0\n\t#nop\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbc1tl\t$L4\n\tsub.s\t$f0,$f0,$f2\n\t.set\tmacro\n\t.set\treorder\n\n\ttrunc.w.s $f4,$f0,$6\n\tmfc1\t$4,$f4\n\tj\t$L5\n$L4:\n\tli\t$2,-2147483648\t\t\t# 0x80000000\n\ttrunc.w.s $f4,$f0,$6\n\tmfc1\t$4,$f4\n\t#nop\n', '\tmul.s\t$f2,$f2,$f0\n\tli.s\t$f0,2.14748364800000000000e9\n\t#nop\n\tc.le.s\t$f0,$f2\n\t#nop\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbc1tl\t$L4\n\tsub.s\t$f0,$f2,$f0\n\t.set\tmacro\n\t.set\treorder\n\n\ttrunc.w.s $f0,$f2,$6\n\tmfc1\t$4,$f0\n\tj\t$L5\n$L4:\n\ttrunc.w.s $f2,$f0,$6\n\tmfc1\t$4,$f2\n\tli\t$2,-2147483648\t\t\t# 0x80000000\n', 'reviewed unsigned volume conversion registers')
    text = replace_once(text, '\tli.s\t$f20,2.55000000000000000000e2\n\tl.s\t$f0,4($16)\n', '\tl.s\t$f0,4($16)\n\tli.s\t$f20,2.55000000000000000000e2\n', 'reviewed fade level load order')
    text = replace_once(text, '\taddiu\t$3,$2,%lo(D_80061D90) # low\n\tl.s\t$f2,4($3)\n\tli.s\t$f0,2.55000000000000000000e2\n\t#nop\n\tc.eq.s\t$f2,$f0\n\t#nop\n\tbc1t\t$L12\n\tmtc1\t$0,$f0\n\t#nop\n\tc.eq.s\t$f2,$f0\n\t#nop\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbc1fl\t$L11\n\tsw\t$0,16($3)\n\t.set\tmacro\n\t.set\treorder\n\n$L12:\n\tlw\t$2,12($3)\n\tli\t$4,1\t\t\t# 0x00000001\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbeql\t$2,$4,$L18\n\tlui\t$2,%hi(D_80053980) # high\n\t.set\tmacro\n\t.set\treorder\n\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbnel\t$2,$0,$L16\n\tli\t$2,1\t\t\t# 0x00000001\n\t.set\tmacro\n\t.set\treorder\n\n\tli\t$2,-1\t\t\t# 0xffffffff\n\tsw\t$2,12($3)\n\tlui\t$2,%hi(D_80053980) # high\n$L18:\n\tsw\t$4,16($3)\n\tsw\t$0,%lo(D_80053980)($2)\n\t.set\tnoreorder\n\t.set\tnomacro\n', '\taddiu\t$4,$2,%lo(D_80061D90) # low\n\tl.s\t$f2,4($4)\n\tli.s\t$f0,2.55000000000000000000e2\n\t#nop\n\tc.eq.s\t$f2,$f0\n\t#nop\n\tbc1t\t$L12\n\tmtc1\t$0,$f0\n\t#nop\n\tc.eq.s\t$f2,$f0\n\t#nop\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbc1fl\t$L11\n\tsw\t$0,16($4)\n\t.set\tmacro\n\t.set\treorder\n\n$L12:\n\tlw\t$2,12($4)\n\tli\t$3,1\t\t\t# 0x00000001\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbeql\t$2,$3,$L18\n\tlui\t$2,%hi(D_80053980) # high\n\t.set\tmacro\n\t.set\treorder\n\n\t.set\tnoreorder\n\t.set\tnomacro\n\tbnel\t$2,$0,$L16\n\tli\t$2,1\t\t\t# 0x00000001\n\t.set\tmacro\n\t.set\treorder\n\n\tli\t$2,-1\t\t\t# 0xffffffff\n\tsw\t$2,12($4)\n\tlui\t$2,%hi(D_80053980) # high\n$L18:\n\tsw\t$3,16($4)\n\tsw\t$0,%lo(D_80053980)($2)\n\t.set\tnoreorder\n\t.set\tnomacro\n', 'reviewed fade completion pointer and constant registers')
    return normalize_standard_return(text)


def normalize_d5_func_80029DC4(text: str) -> str:
    """Rename the reload temporary for the label high half ($9 -> $4).

    The temporary lives only between these two instructions; retail
    builds the address in the argument register itself.
    """
    text = normalize_standard_return(text)
    return replace_once(text, (
        "\tlui\t$9,%hi($LC0) # high\n\taddiu\t$4,$9,%lo($LC0) # low\n"
    ), (
        "\tlui\t$4,%hi($LC0) # high\n\taddiu\t$4,$4,%lo($LC0) # low\n"
    ), "reviewed func_80029DC4 label temporary")


def normalize_d6_func_80010358(text):
    """func_80010358: move the f20 zero materialisation after the first
    argument load, and rename the two button-difference results from $3 to
    $2 (live range: subu -> mtc1). Reorder and rename only."""
    old_zero = "\tmtc1\t$0,$f20\n\tlw\t$5,276($17)\n"
    new_zero = "\tlw\t$5,276($17)\n\tmtc1\t$0,$f20\n"
    if text.count(old_zero) != 2:
        raise ValueError("func_80010358: zero setup pattern missing")
    text = text.replace(old_zero, new_zero)
    old_a = ("\tsubu\t$3,$3,$2\n\tli.s\t$f2,5.00000000000000000000e1\n"
             "\tmtc1\t$3,$f0\n")
    new_a = ("\tsubu\t$2,$3,$2\n\tli.s\t$f2,5.00000000000000000000e1\n"
             "\tmtc1\t$2,$f0\n")
    old_b = "\tsubu\t$3,$3,$2\n\tmfc1\t$5,$f0\n\tmtc1\t$3,$f0\n"
    new_b = "\tsubu\t$2,$3,$2\n\tmfc1\t$5,$f0\n\tmtc1\t$2,$f0\n"
    if old_a not in text or old_b not in text:
        raise ValueError("func_80010358: button difference pattern missing")
    return normalize_standard_return(
        text.replace(old_a, new_a).replace(old_b, new_b))


def normalize_d8_magic_first_arms(text, branch, fall_label, taken_label, name):
    # Both switch arms: retail schedules the modulo-by-9 magic constant before
    # the table address, and fills the branch delay slot with the magic lui.
    hi = "\tlui\t$18,%hi(D_80052F18) # high\n"
    lo = "\taddiu\t$18,$18,%lo(D_80052F18) # low\n"
    app = " #APP\n #NO_APP\n"
    magic_hi = "\tli\t$3,954400768\t\t\t# 0x38e30000\n"
    magic = magic_hi + "\tori\t$3,$3,0x8e39\n"
    pairs = [
        (branch + hi, branch + magic_hi),
        (fall_label + hi + lo + app + magic, fall_label + magic + hi + lo + app),
        (taken_label + lo + app + magic,
         taken_label + "\tori\t$3,$3,0x8e39\n" + hi + lo + app),
    ]
    for old, new in pairs:
        if text.count(old) != 1:
            raise ValueError(name + " arm schedule pattern missing")
        text = text.replace(old, new)
    return text


def normalize_d8_func_80017144(text):
    return normalize_d8_magic_first_arms(
        text, "\tbeq\t$3,$2,$L31\n", "$L30:\n", "$L31:\n", "func_80017144")


def normalize_d8_func_80018A34(text):
    return normalize_d8_magic_first_arms(
        text, "\tbeq\t$3,$0,$L31\n", "$L33:\n", "$L31:\n", "func_80018A34")


def normalize_e4_func_8000E630(text: str) -> str:
    text = replace_once(text, (
        "\taddiu\t$2,$2,%lo(D_800586A8) # low\n\tmove\t$3,$4\n"
    ), (
        "\taddiu\t$7,$2,%lo(D_800586A8) # low\n\tmove\t$6,$4\n"
    ), "reviewed block-copy pointer registers")
    return remap_body_registers(
        text, "\taddu\t$4,$3,320\n",
        {2: 7, 3: 6, 4: 8, 5: 2, 6: 3, 7: 4, 8: 5},
        "reviewed block-copy register allocation",
    )



def normalize_e4_func_8000E680(text: str) -> str:
    text = replace_once(text, (
        "\tmove\t$3,$4\n #APP\n #NO_APP\n"
        "\tlui\t$2,%hi(D_800586A8) # high\n"
        "\taddiu\t$2,$2,%lo(D_800586A8) # low\n"
    ), (
        "\tmove\t$7,$4\n #APP\n #NO_APP\n"
        "\tlui\t$2,%hi(D_800586A8) # high\n"
        "\taddiu\t$6,$2,%lo(D_800586A8) # low\n"
    ), "reviewed block-copy pointer registers")
    text = replace_once(text, (
        "\tlui\t$4,%hi(D_800586A8) # high\n"
        "\taddiu\t$4,$4,%lo(D_800586A8) # low\n"
        "\tli\t$5,-1\t\t\t# 0xffffffff\n"
        "\tli\t$6,328\t\t\t# 0x00000148\n"
        "\tlw\t$7,0($2)\n"
        "\tlw\t$8,4($2)\n"
        "\tsw\t$7,0($3)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80038E90\n"
        "\tsw\t$8,4($3)\n"
    ), (
        "\tlw\t$2,0($6)\n"
        "\tlw\t$3,4($6)\n"
        "\tsw\t$2,0($7)\n"
        "\tsw\t$3,4($7)\n"
        "\tlui\t$4,%hi(D_800586A8) # high\n"
        "\taddiu\t$4,$4,%lo(D_800586A8) # low\n"
        "\tli\t$5,-1\t\t\t# 0xffffffff\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80038E90\n"
        "\tli\t$6,328\t\t\t# 0x00000148\n"
    ), "reviewed block-copy tail and call-argument order")
    start = "\taddu\t$4,$2,320\n"
    head, tail = text.split(start, 1)
    loop, rest = tail.split("\tlw\t$2,0($6)\n", 1)
    loop = remap_body_registers(
        start + loop, start,
        {2: 6, 3: 7, 4: 8, 7: 2, 8: 3, 9: 4, 10: 5, 5: 9, 6: 10},
        "reviewed block-copy register allocation",
    )
    return normalize_standard_return(head + loop + "\tlw\t$2,0($6)\n" + rest)



def normalize_e4_func_8000DB84(text: str) -> str:
    text = replace_once(text, (
        "\tli\t$9,1\t\t\t# 0x00000001\n"
        "\tlw\t$4,%lo(D_800581F0)($3)\n"
        " #APP\n #NO_APP\n\t#nop\n"
    ), (
        "\tlw\t$4,%lo(D_800581F0)($3)\n"
        "\tli\t$9,1\t\t\t# 0x00000001\n"
        " #APP\n #NO_APP\n"
    ), "reviewed room-fill constant load-delay fill")
    text = replace_once(text, "\taddu\t$4,$4,$3\n", "\taddu\t$4,$3,$4\n",
                        "reviewed room-fill cell pointer operand order")
    return text



def normalize_e4_func_80004A18(text: str) -> str:
    text = remap_registers(text, {22: 21, 21: 22})
    for opcode, register, old, new in (
        ("sw", 21, 40, 36), ("sw", 22, 36, 40),
        ("lw", 21, 40, 36), ("lw", 22, 36, 40),
    ):
        text = replace_once(
            text, f"\t{opcode}\t${register},{old}($sp)\n",
            f"\t{opcode}\t${register},{new}($sp)\n",
            "reviewed host-write saved-register layout",
        )
    text = replace_once(text, (
        "\tlw\t$31,44($sp)\n\tlw\t$21,36($sp)\n\tlw\t$22,40($sp)\n"
    ), (
        "\tlw\t$31,44($sp)\n\tlw\t$22,40($sp)\n\tlw\t$21,36($sp)\n"
    ), "reviewed host-write saved-register restore order")
    return normalize_standard_return(text)



def normalize_e4_func_8000E1D0(text: str) -> str:
    text = replace_once(text, (
        "\taddu\t$16,$17,$20\n"
        "\tl.s\t$f0,44($19)\n"
        "\tli.s\t$f2,3.00000000000000000000e2\n"
        "\ttrunc.w.s $f4,$f0,$5\n"
        "\ts.s\t$f4,0($16)\n"
        "\tl.s\t$f0,48($19)\n"
        "\t#nop\n"
        "\tadd.s\t$f0,$f0,$f2\n"
        "\ttrunc.w.s $f4,$f0,$5\n"
        "\ts.s\t$f4,4($16)\n"
        "\tl.s\t$f0,52($19)\n"
        "\t#nop\n"
        "\ttrunc.w.s $f4,$f0,$5\n"
        "\ts.s\t$f4,8($16)\n"
    ), (
        "\tl.s\t$f0,44($19)\n"
        "\taddu\t$16,$17,$20\n"
        "\ttrunc.w.s $f2,$f0,$5\n"
        "\ts.s\t$f2,0($16)\n"
        "\tl.s\t$f0,48($19)\n"
        "\tli.s\t$f2,3.00000000000000000000e2\n"
        "\tadd.s\t$f0,$f0,$f2\n"
        "\ttrunc.w.s $f2,$f0,$5\n"
        "\ts.s\t$f2,4($16)\n"
        "\tl.s\t$f0,52($19)\n"
        "\t#nop\n"
        "\ttrunc.w.s $f2,$f0,$5\n"
        "\ts.s\t$f2,8($16)\n"
    ), "reviewed particle position conversion order and registers")
    return normalize_standard_return(text)



def normalize_e4_func_8000B71C(text: str) -> str:
    # Retail gives the conversion scratch the low register, so every loop
    # variable sits one register higher; the first loop's pointers move too.
    loop_start = "\tsll\t$2,$10,4\n\taddu\t$4,$2,$5\n\taddu\t$3,$2,$6\n"
    loop_end = "\tbne\t$2,$0,$L8\n\taddu\t$4,$4,16\n"
    if text.count(loop_start) != 1 or text.count(loop_end) != 1:
        raise ValueError("reviewed vertex-offset loop pattern not found")
    head, rest = text.split(loop_start, 1)
    loop, tail = rest.split(loop_end, 1)
    loop = loop_start + loop + loop_end
    reg = re.compile(r"\$(f?)(\d+)\b")
    outer = {8: 9, 9: 10, 10: 11, 11: 8}
    inner = {8: 9, 9: 10, 10: 11, 11: 3, 4: 8, 3: 4}

    def rename(part: str, mapping: dict[int, int]) -> str:
        def sub(m: re.Match) -> str:
            if m.group(1):
                return "$f2" if m.group(2) == "4" else m.group(0)
            n = int(m.group(2))
            return "$" + str(mapping.get(n, n))
        return "\n".join(
            line if line.startswith("\t.") or line.startswith("$L")
            else reg.sub(sub, line)
            for line in part.split("\n")
        )
    return rename(head, outer) + rename(loop, inner) + rename(tail, outer)



def normalize_e4_func_80006C18(text: str) -> str:
    # Shared return (precedent: normalize_shared_return); the beq delay slot
    # is filled from the fall-through path instead of the target.
    sn = "\t.set\tnoreorder\n\t.set\tnomacro\n"
    sr = "\t.set\tmacro\n\t.set\treorder\n"
    return replace_once(text, (
        sn + "\tbeq\t$5,$0,$L3\n\tlui\t$2,%hi(D_80052A18) # high\n" + sr + "\n"
        "\tlui\t$2,%hi(D_80052A14) # high\n"
        "\tl.s\t$f0,%lo(D_80052A14)($2)\n"
        "\tlui\t$2,%hi(D_80052A10) # high\n"
        "\ts.s\t$f0,0($6)\n"
        "\tl.s\t$f0,%lo(D_80052A10)($2)\n"
        + sn + "\tj\t$31\n\ts.s\t$f0,0($7)\n" + sr + "\n"
        "$L3:\n"
        "\tl.s\t$f0,%lo(D_80052A18)($2)\n"
        + sn + "\tj\t$31\n\ts.s\t$f0,0($6)\n" + sr + "\n"
        "$L2:\n"
        "\tlui\t$2,%hi(D_80052A1C) # high\n"
        "\tl.s\t$f0,%lo(D_80052A1C)($2)\n"
        + sn + "\tj\t$31\n\ts.s\t$f0,0($6)\n" + sr
    ), (
        sn + "\tbeq\t$5,$0,$L3\n\tlui\t$2,%hi(D_80052A14) # high\n" + sr + "\n"
        "\tl.s\t$f0,%lo(D_80052A14)($2)\n"
        "\tlui\t$2,%hi(D_80052A10) # high\n"
        "\ts.s\t$f0,0($6)\n"
        "\tl.s\t$f0,%lo(D_80052A10)($2)\n"
        + sn + "\tj\t$L_return\n\ts.s\t$f0,0($7)\n" + sr + "\n"
        "$L3:\n"
        "\tlui\t$2,%hi(D_80052A18) # high\n"
        "\tl.s\t$f0,%lo(D_80052A18)($2)\n"
        + sn + "\tj\t$L_return\n\ts.s\t$f0,0($6)\n" + sr + "\n"
        "$L2:\n"
        "\tlui\t$2,%hi(D_80052A1C) # high\n"
        "\tl.s\t$f0,%lo(D_80052A1C)($2)\n"
        "\ts.s\t$f0,0($6)\n"
        "$L_return:\n"
        + sn + "\tj\t$31\n\tnop\n" + sr
    ), "reviewed spawn-offset shared return")


def normalize_two_player_minigame(text: str) -> str:
    """Use retail's scratch registers for four movstrsi Vec3 copies.

    SN emits the same straight-line loads and stores through its generic
    structure-copy expander, but chooses t1-t4 where the retail invocation
    chose v0-a1.  These substitutions do not alter the instruction stream or
    any live value outside each copy.
    """
    # The retail schedule materializes the state value 2 only on the unequal
    # actor path.  SN 2.8 available here hoists it into the earlier beq delay
    # slot.  Keep the empty retail slot and restore the constant at its first
    # use; the intervening path returns, so this is instruction-equivalent.
    text = replace_once(
        text,
        "\tbeq\t$5,$0,$L62\n\tli\t$6,2\t\t\t# 0x00000002\n",
        "\tbeq\t$5,$0,$L62\n\tnop\n",
        "two-player deferred state constant",
    )
    text = replace_once(
        text,
        ("$L79:\n\tlui\t$3,%hi(D_80054A9C) # high\n"
         "\tsw\t$6,%lo(D_800C3E28)($2)\n"),
        ("$L79:\n\tli\t$4,2\t\t\t# 0x00000002\n"
         "\tlui\t$3,%hi(D_80054A9C) # high\n"
         "\tsw\t$4,%lo(D_800C3E28)($2)\n"),
        "two-player unequal actor state",
    )
    text = replace_once(
        text,
        "\tbeq\t$3,$6,$L89\n\tmove\t$18,$0\n",
        "\tbeq\t$3,$4,$L89\n\tmove\t$18,$0\n",
        "two-player pairing case 2",
    )
    text = replace_once(
        text,
        ("\tlw\t$5,4($16)\n\t#nop\n"
         "\t.set\tnoreorder\n\t.set\tnomacro\n"
         "\tbeq\t$5,$0,$L62\n\tnop\n"
         "\t.set\tmacro\n\t.set\treorder\n\n"
         "\tlw\t$4,60($16)\n\t#nop\n"
         "\tbeq\t$4,$0,$L62\n"
         "\t.set\tnoreorder\n\t.set\tnomacro\n"
         "\tbne\t$5,$4,$L79\n"),
        ("\tlw\t$6,4($16)\n\t#nop\n"
         "\t.set\tnoreorder\n\t.set\tnomacro\n"
         "\tbeq\t$6,$0,$L62\n\tnop\n"
         "\t.set\tmacro\n\t.set\treorder\n\n"
         "\tlw\t$5,60($16)\n\t#nop\n"
         "\tbeq\t$5,$0,$L62\n"
         "\t.set\tnoreorder\n\t.set\tnomacro\n"
         "\tbne\t$6,$5,$L79\n"),
        "two-player actor selection registers",
    )
    text = replace_once(
        text,
        "\taddu\t$2,$5,$4\n\taddu\t$3,$2,-836\n",
        "\taddu\t$2,$6,$5\n\taddu\t$3,$2,-836\n",
        "two-player pairing operands",
    )
    # Case 2 has already been removed by the equality above, making <2 and
    # <3 equivalent here.  Retail used the latter bound.
    text = replace_once(
        text,
        "\tsltu\t$2,$3,2\n",
        "\tsltu\t$2,$3,3\n",
        "two-player pairing range",
    )
    # Independent address materializations; retail scheduled the indexed
    # scalar before the table base in both state-display paths.
    text = replace_once(
        text,
        ("\tlui\t$3,%hi(D_80059DB8) # high\n"
         "\tlui\t$2,%hi(D_800C3E20) # high\n"),
        ("\tlui\t$2,%hi(D_800C3E20) # high\n"
         "\tlui\t$3,%hi(D_80059DB8) # high\n"),
        "two-player prize table address order",
    )
    text = replace_once(
        text,
        ("\tlui\t$3,%hi(D_800C3E38) # high\n"
         "\tlui\t$2,%hi(D_800C3E20) # high\n"),
        ("\tlui\t$2,%hi(D_800C3E20) # high\n"
         "\tlui\t$3,%hi(D_800C3E38) # high\n"),
        "two-player result table address order",
    )
    # The switch guarantees a zero/one winner index.  Retail encodes the
    # inversion with xori; this compiler retains the already-loaded literal 1
    # in a1 and emits the equivalent register xor.
    text = replace_once(
        text,
        "\txor\t$3,$3,$5\n",
        "\txori\t$3,$3,1\n",
        "two-player winner inversion",
    )
    text = replace_once(
        text,
        ("\tbeq\t$2,$0,$L121\n"
         "\tlui\t$3,%hi(D_80059DB8) # high\n"
         "\t.set\tmacro\n\t.set\treorder\n\n"
         "\tlui\t$2,%hi(D_800C3E2C) # high\n"),
        ("\tbeq\t$2,$0,$L121\n"
         "\tlui\t$2,%hi(D_800C3E2C) # high\n"
         "\t.set\tmacro\n\t.set\treorder\n\n"
         "\tlui\t$3,%hi(D_80059DB8) # high\n"),
        "two-player prize lookup address schedule",
    )
    text = replace_once(
        text,
        ("\tand\t$2,$2,$3\n"
         "\tlui\t$3,%hi(D_80059DB8) # high\n"
         "\t#.set\tvolatile\n"
         "\tsw\t$2,%lo(D_80052EE0)($4)\n"
         "\t#.set\tnovolatile\n"
         "\tlui\t$2,%hi(D_800C3E20) # high\n"
         "\tlw\t$2,%lo(D_800C3E20)($2)\n"
         "\taddiu\t$3,$3,%lo(D_80059DB8) # low\n"
         "\tsll\t$2,$2,1\n"
         "\taddu\t$2,$2,$3\n"
         "\tlui\t$3,%hi(D_80052F04) # high\n"
         "\tlh\t$5,0($2)\n"
         "\t#.set\tvolatile\n"
         "\tlw\t$4,%lo(D_80052F04)($3)\n"
         "\t#.set\tnovolatile\n"),
        ("\tand\t$2,$2,$3\n"
         "\t#.set\tvolatile\n"
         "\tsw\t$2,%lo(D_80052EE0)($4)\n"
         "\t#.set\tnovolatile\n"
         "\tlui\t$4,%hi(D_80052F04) # high\n"
         "\tlui\t$2,%hi(D_800C3E20) # high\n"
         "\tlui\t$3,%hi(D_80059DB8) # high\n"
         "\taddiu\t$3,$3,%lo(D_80059DB8) # low\n"
         "\tlw\t$2,%lo(D_800C3E20)($2)\n"
         "\t#.set\tvolatile\n"
         "\tlw\t$4,%lo(D_80052F04)($4)\n"
         "\t#.set\tnovolatile\n"
         "\tsll\t$2,$2,1\n"
         "\taddu\t$2,$2,$3\n"
         "\tlh\t$5,0($2)\n"),
        "two-player prize object schedule",
    )
    text = replace_once(
        text,
        ("\tli\t$2,1\t\t\t# 0x00000001\n"
         "\tsll\t$3,$5,$2\n"
         "\taddu\t$3,$3,$5\n"
         "\tsw\t$2,244($4)\n"),
        ("\tli\t$2,1\t\t\t# 0x00000001\n"
         "\tsw\t$2,244($4)\n"
         "\tsll\t$3,$5,1\n"
         "\taddu\t$3,$3,$5\n"),
        "two-player prize angle multiply",
    )
    text = replace_once(
        text,
        ("\tmove\t$19,$0\n"
         "\tlui\t$2,%hi(D_80059DB8) # high\n"
         "\taddiu\t$2,$2,%lo(D_80059DB8) # low\n"
         "\tmove\t$21,$2\n"
         "\tlui\t$2,%hi(D_800C3E2C) # high\n"
         "\tmove\t$20,$2\n"
         "\tmove\t$18,$19\n"),
        ("\tmove\t$19,$0\n"
         "\tlui\t$2,%hi(D_800C3E2C) # high\n"
         "\tmove\t$21,$2\n"
         "\tlui\t$2,%hi(D_80059DB8) # high\n"
         "\taddiu\t$2,$2,%lo(D_80059DB8) # low\n"
         "\tmove\t$20,$2\n"
         "\tmove\t$18,$19\n"),
        "two-player spawn table setup",
    )
    spawn_start = "\tmove\t$18,$19\n"
    spawn_end = "\n\tlui\t$3,%hi(D_80054AA8) # high\n"
    if text.count(spawn_start) != 1:
        raise ValueError("reviewed two-player spawn loop not found")
    before_spawn, spawn_rest = text.split(spawn_start, 1)
    if spawn_end not in spawn_rest:
        raise ValueError("reviewed two-player spawn loop end not found")
    spawn, after_spawn = spawn_rest.split(spawn_end, 1)
    spawn = spawn_start + spawn
    saved = re.compile(r"\$(20|21)\b")
    spawn = saved.sub(lambda m: "$21" if m.group(1) == "20" else "$20", spawn)
    text = before_spawn + spawn + spawn_end + after_spawn
    text = replace_once(
        text,
        ("\t.set\tnoreorder\n\t.set\tnomacro\n"
         "\tbltz\t$19,$L143\n"
         "\tli\t$8,-2147483648\t\t\t# 0x80000000\n"
         "\t.set\tmacro\n\t.set\treorder\n\n"
         "\tlui\t$2,%hi(D_800C3E24) # high\n"
         "\tlw\t$4,%lo(D_800C3E24)($2)\n"
         "\tsll\t$3,$19,2\n"
         "\taddu\t$7,$4,$3\n"
         "\tlw\t$5,428($7)\n\t#nop\n"
         "\t.set\tnoreorder\n\t.set\tnomacro\n"
         "\tbltz\t$5,$L143\n"
         "\tlui\t$2,%hi(D_800C3E30) # high\n"
         "\t.set\tmacro\n\t.set\treorder\n"),
        ("\t.set\tnoreorder\n\t.set\tnomacro\n"
         "\tbltz\t$19,$L143\n"
         "\tlui\t$2,%hi(D_800C3E24) # high\n"
         "\t.set\tmacro\n\t.set\treorder\n\n"
         "\tlw\t$4,%lo(D_800C3E24)($2)\n"
         "\tsll\t$3,$19,2\n"
         "\taddu\t$9,$4,$3\n"
         "\tlw\t$8,428($9)\n\t#nop\n"
         "\t.set\tnoreorder\n\t.set\tnomacro\n"
         "\tbltz\t$8,$L143\n"
         "\tli\t$6,-2147483648\t\t\t# 0x80000000\n"
         "\t.set\tmacro\n\t.set\treorder\n\n"
         "\tlui\t$2,%hi(D_800C3E30) # high\n"),
        "two-player item source schedule",
    )
    text = replace_once(
        text,
        ("\taddu\t$6,$2,$3\n"
         "\tlw\t$2,428($6)\n\t#nop\n"
         "\tand\t$2,$2,$8\n"),
        ("\taddu\t$7,$2,$3\n"
         "\tlw\t$2,428($7)\n\t#nop\n"
         "\tand\t$2,$2,$6\n"),
        "two-player item destination registers",
    )
    text = replace_once(
        text,
        "\taddu\t$9,$4,$3\n",
        "\taddu\t$9,$3,$4\n",
        "two-player source item address operands",
    )
    text = replace_once(
        text,
        "\taddu\t$7,$2,$3\n",
        "\taddu\t$7,$3,$2\n",
        "two-player destination item address operands",
    )
    text = replace_once(
        text,
        ("\tli\t$21,1\t\t\t# 0x00000001\n"
         "\tori\t$3,$3,0xffff\n"
         "\tor\t$2,$5,$8\n"
         "\tsw\t$2,428($7)\n"
         "\tlw\t$2,428($6)\n"
         "\tli\t$5,6\t\t\t# 0x00000006\n"
         "\tand\t$2,$2,$3\n"
         "\t.set\tnoreorder\n\t.set\tnomacro\n"
         "\tjal\tfunc_8002621C\n"
         "\tsw\t$2,428($6)\n"),
        ("\tori\t$3,$3,0xffff\n"
         "\tli\t$5,6\t\t\t# 0x00000006\n"
         "\tor\t$2,$8,$6\n"
         "\tsw\t$2,428($9)\n"
         "\tlw\t$2,428($7)\n"
         "\tli\t$21,1\t\t\t# 0x00000001\n"
         "\tand\t$2,$2,$3\n"
         "\t.set\tnoreorder\n\t.set\tnomacro\n"
         "\tjal\tfunc_8002621C\n"
         "\tsw\t$2,428($7)\n"),
        "two-player item transfer schedule",
    )
    # The index arithmetic and the player-array address load are independent.
    # Retail starts the address load first, hiding it behind the three ALU
    # instructions; the available scheduler emits the arithmetic first.
    text = replace_once(
        text,
        ("\tj\t$L156\n"
         "\tsll\t$16,$17,3\n"),
        ("\tj\t$L156\n"
         "\tlui\t$2,%hi(D_800C3DB0) # high\n"),
        "two-player selected player jump schedule",
    )
    text = replace_once(
        text,
        ("\tsltu\t$17,$0,$2\n"
         "\tsll\t$16,$17,3\n"
         "$L156:\n"
         "\tsubu\t$16,$16,$17\n"
         "\tsll\t$16,$16,3\n"
         "\tlui\t$2,%hi(D_800C3DB0) # high\n"
         "\taddiu\t$2,$2,%lo(D_800C3DB0) # low\n"),
        ("\tsltu\t$17,$0,$2\n"
         "\tlui\t$2,%hi(D_800C3DB0) # high\n"
         "$L156:\n"
         "\taddiu\t$2,$2,%lo(D_800C3DB0) # low\n"
         "\tsll\t$16,$17,3\n"
         "\tsubu\t$16,$16,$17\n"
         "\tsll\t$16,$16,3\n"),
        "two-player active player address schedule",
    )

    copies = (
        ("\taddiu\t$12,$20,%lo(D_80054A0C)\n\tlw\t$9,0($12)\n\tlw\t$10,4($12)\n\tlw\t$11,8($12)\n\tsw\t$9,28($16)\n\tsw\t$10,32($16)\n\tsw\t$11,36($16)\n",
         "\taddiu\t$5,$20,%lo(D_80054A0C)\n\tlw\t$2,0($5)\n\tlw\t$3,4($5)\n\tlw\t$4,8($5)\n\tsw\t$2,28($16)\n\tsw\t$3,32($16)\n\tsw\t$4,36($16)\n"),
        ("\tlw\t$9,12($19)\n\tlw\t$10,16($19)\n\tlw\t$11,20($19)\n\tsw\t$9,28($16)\n\tsw\t$10,32($16)\n\tsw\t$11,36($16)\n",
         "\tlw\t$2,12($19)\n\tlw\t$3,16($19)\n\tlw\t$4,20($19)\n\tsw\t$2,28($16)\n\tsw\t$3,32($16)\n\tsw\t$4,36($16)\n"),
        ("\tlw\t$9,24($19)\n\tlw\t$10,28($19)\n\tlw\t$11,32($19)\n\tsw\t$9,28($16)\n\tsw\t$10,32($16)\n\tsw\t$11,36($16)\n",
         "\tlw\t$2,24($19)\n\tlw\t$3,28($19)\n\tlw\t$4,32($19)\n\tsw\t$2,28($16)\n\tsw\t$3,32($16)\n\tsw\t$4,36($16)\n"),
        ("\tlw\t$9,28($16)\n\tlw\t$10,32($16)\n\tlw\t$11,36($16)\n\tsw\t$9,16($16)\n\tsw\t$10,20($16)\n\tsw\t$11,24($16)\n",
         "\tlw\t$2,28($16)\n\tlw\t$3,32($16)\n\tlw\t$4,36($16)\n\tsw\t$2,16($16)\n\tsw\t$3,20($16)\n\tsw\t$4,24($16)\n"),
    )
    for index, (old, new) in enumerate(copies):
        text = replace_once(text, old, new, f"two-player Vec3 copy {index}")

    text = replace_once(
        text,
        ("\tlw\t$10,0($2)\n\tlw\t$11,4($2)\n\tlw\t$12,8($2)\n"
         "\tsw\t$10,28($16)\n\tsw\t$11,32($16)\n\tsw\t$12,36($16)\n"),
        ("\tlw\t$3,0($2)\n\tlw\t$4,4($2)\n\tlw\t$5,8($2)\n"
         "\tsw\t$3,28($16)\n\tsw\t$4,32($16)\n\tsw\t$5,36($16)\n"),
        "two-player separated destination copy",
    )
    separated_start = "$L169:\n"
    separated_end = "\n\tj\t$L62\n$L98:"
    if text.count(separated_start) != 1 or text.count(separated_end) != 1:
        raise ValueError("reviewed separated interpolation loop not found")
    before_separated, separated_rest = text.split(separated_start, 1)
    separated, after_separated = separated_rest.split(separated_end, 1)
    separated = separated_start + separated
    separated = replace_once(
        separated,
        "\ttrunc.w.s $f12,$f4,$9\n\tmfc1\t$2,$f12\n",
        "\ttrunc.w.s $f10,$f4,$9\n\tmfc1\t$2,$f10\n",
        "two-player separated first conversion",
    )
    separated = replace_once(
        separated,
        "\ttrunc.w.s $f12,$f6,$9\n\tmfc1\t$2,$f12\n",
        "\ttrunc.w.s $f0,$f6,$9\n\tmfc1\t$2,$f0\n",
        "two-player separated second conversion",
    )
    separated = replace_once(
        separated,
        "\ttrunc.w.s $f12,$f2,$9\n\tmfc1\t$2,$f12\n",
        "\ttrunc.w.s $f0,$f2,$9\n\tmfc1\t$2,$f0\n",
        "two-player separated third conversion",
    )
    text = before_separated + separated + separated_end + after_separated

    # Alpha-rename the complete shared-destination interpolation loop.  This
    # is one live range: retail uses a2/a3/t0 for the destination, duration
    # base, and source, while this compiler build uses a0/a1/a2.  Temporary
    # structure-copy registers differ in the same region.  Rename them as a
    # group so pointer and copied-value lifetimes cannot collide.
    loop_start = ("\tmove\t$17,$0\n"
                  "\tlui\t$2,%hi(D_80054A30) # high\n")
    loop_end = "\n\tj\t$L62\n$L79:"
    if text.count(loop_start) != 1 or text.count(loop_end) != 1:
        raise ValueError("reviewed two-player interpolation loop not found")
    head, rest = text.split(loop_start, 1)
    loop, tail = rest.split(loop_end, 1)
    loop = loop_start + loop
    gpr_map = {4: 6, 5: 7, 6: 8, 9: 2, 10: 3, 11: 4}
    gpr = re.compile(r"\$(\d+)\b")

    def rename_interpolation_line(line: str) -> str:
        if line.startswith("\ttrunc.w.s"):
            return line
        return gpr.sub(lambda m: "$" + str(gpr_map.get(int(m.group(1)), int(m.group(1)))), line)

    loop = "\n".join(rename_interpolation_line(line) for line in loop.split("\n"))
    loop = loop.replace("$f10", "$f12")
    loop = replace_once(
        loop,
        "\ttrunc.w.s $f12,$f4,$9\n\tmfc1\t$2,$f12\n",
        "\ttrunc.w.s $f10,$f4,$9\n\tmfc1\t$2,$f10\n",
        "two-player first interpolation conversion",
    )
    loop = replace_once(
        loop,
        "\ttrunc.w.s $f12,$f6,$9\n\tmfc1\t$2,$f12\n",
        "\ttrunc.w.s $f0,$f6,$9\n\tmfc1\t$2,$f0\n",
        "two-player second interpolation conversion",
    )
    loop = replace_once(
        loop,
        "\ttrunc.w.s $f12,$f2,$9\n\tmfc1\t$2,$f12\n",
        "\ttrunc.w.s $f0,$f2,$9\n\tmfc1\t$2,$f0\n",
        "two-player third interpolation conversion",
    )
    text = head + loop + loop_end + tail
    return normalize_standard_return(text)


def normalize_board_scene_renderer(text: str) -> str:
    """Use retail's free t1 temporary outside the paired modulo calculation."""
    text = normalize_standard_return(text)
    # The preferred source shape already assigns both remainder calculations to
    # t1, so there is no conflicting t2 live range left to protect.
    if "$10" not in text:
        return text
    modulo_start = "\ttrunc.w.s $f8,$f2,$10\n"
    modulo_end = (
        "\tmtc1\t$2,$f8\n"
        "\tcvt.s.w\t$f8,$f8\n"
        "\tmfc1\t$7,$f8\n"
    )
    if modulo_start not in text:
        raise ValueError("board renderer modulo block not found")
    head, rest = text.split(modulo_start, 1)
    if modulo_end not in rest:
        raise ValueError("board renderer modulo block end not found")
    modulo, tail = rest.split(modulo_end, 1)
    head = re.sub(r"\$10\b", "$9", head)
    tail = re.sub(r"\$10\b", "$9", tail)
    return head + modulo_start + modulo + modulo_end + tail


def normalize_camera_follow_math(text: str) -> str:
    """Use the retail allocator's registers for the modulo/divide tail."""
    old = (
        "\tsubu\t$5,$2,$3\n\tsll\t$3,$5,1\n\taddu\t$3,$3,$5\n"
        "\tsll\t$2,$3,4\n\tsubu\t$2,$2,$3\n\tsll\t$2,$2,3\n"
        "\tsubu\t$5,$4,$2\n\tli\t$2,180\t\t\t# 0x000000b4\n"
        "\tsubu\t$4,$2,$5\n\tmtc1\t$4,$f0\n"
    )
    new = (
        "\tsubu\t$2,$2,$3\n\tsll\t$3,$2,1\n\taddu\t$3,$3,$2\n"
        "\tsll\t$2,$3,4\n\tsubu\t$2,$2,$3\n\tsll\t$2,$2,3\n"
        "\tsubu\t$3,$4,$2\n\tli\t$2,180\t\t\t# 0x000000b4\n"
        "\tsubu\t$5,$2,$3\n\tmtc1\t$5,$f0\n"
    )
    text = replace_once(text, old, new, "reviewed camera modulo registers")
    old = (
        "\tlw\t$3,%lo(D_80052BE8)($2)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tj\t$L20\n\tmult\t$3,$4\n"
        "\t.set\tmacro\n\t.set\treorder\n\n$L15:\n"
        "\tlw\t$3,%lo(D_80052BE8)($2)\n\taddu\t$2,$5,-180\n"
        "\tmult\t$3,$2\n$L20:\n\tmflo\t$3\n\t#nop\n\t#nop\n"
        "\tmult\t$3,$6\n\tmfhi\t$6\n\t#nop\n\t#nop\n"
        "\taddu\t$2,$6,$3\n\tsra\t$2,$2,7\n\tsra\t$3,$3,31\n"
        "\tsubu\t$2,$2,$3\n\taddu\t$2,$2,2400\n"
    )
    new = (
        "\tlw\t$2,%lo(D_80052BE8)($2)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tj\t$L20\n\tmult\t$2,$5\n"
        "\t.set\tmacro\n\t.set\treorder\n\n$L15:\n"
        "\tlw\t$2,%lo(D_80052BE8)($2)\n\taddu\t$3,$3,-180\n"
        "\tmult\t$2,$3\n$L20:\n\tmflo\t$2\n\t#nop\n\t#nop\n"
        "\tmult\t$2,$6\n\tmfhi\t$7\n\t#nop\n\t#nop\n"
        "\taddu\t$3,$7,$2\n\tsra\t$3,$3,7\n\tsra\t$2,$2,31\n"
        "\tsubu\t$3,$3,$2\n\taddu\t$3,$3,2400\n"
    )
    text = replace_once(text, old, new, "reviewed camera distance registers")
    text = replace_once(
        text,
        "\tj\t$L17\n\tsw\t$2,260($17)\n",
        "\tj\t$L17\n\tsw\t$3,260($17)\n",
        "reviewed camera distance store",
    )
    return normalize_standard_return(text)


def normalize_camera_mode_setup(text: str) -> str:
    """Apply the retail register rotation and Vec3 copy expansion."""
    marker = "$L33:\n"
    if text.count(marker) != 1:
        raise ValueError("camera-mode case-10 marker not found uniquely")
    head, tail = text.split(marker, 1)
    mapping = (("$8", "$9"), ("$9", "$10"), ("$10", "$11"),
               ("$11", "$12"), ("$7", "$8"))
    for index, (old, _) in enumerate(mapping):
        head = re.sub(rf"{re.escape(old)}\b", f"$CAMREG{index}", head)
    for index, (_, new) in enumerate(mapping):
        head = head.replace(f"$CAMREG{index}", new)
    initial_copy = "\tmove\t$9,$5\n"
    first_case = "$L9:\n"
    if head.count(initial_copy) != 1 or head.count(first_case) != 1:
        raise ValueError("camera-mode initial register span not found")
    prefix, body = head.split(initial_copy, 1)
    setup, cases = body.split(first_case, 1)
    setup = re.sub(r"\$5\b", "$7", setup)
    head = prefix + initial_copy + setup + first_case + cases
    head = replace_once(
        head, "\tmove\t$8,$11\n", "\tmove\t$8,$10\n",
        "reviewed camera-mode zero-copy chain",
    )
    old = (
        "\tlw\t$4,%lo(D_80052F04)($3)\n"
        "\tli.s\t$f0,4.00000000000000000000e2\n"
        "\tli\t$2,1200\t\t\t# 0x000004b0\n"
        "\tlw\t$12,44($8)\n\tlw\t$13,48($8)\n\tlw\t$14,52($8)\n"
        "\tsw\t$12,88($5)\n\tsw\t$13,92($5)\n\tsw\t$14,96($5)\n"
        "\ts.s\t$f0,92($4)\n\tsw\t$0,252($4)\n\tsw\t$2,260($4)\n"
        "\tlw\t$2,292($8)\n\tlw\t$5,88($4)\n\tlw\t$6,92($4)\n"
        "\tlw\t$7,96($4)\n"
    )
    new = (
        "\tlw\t$4,%lo(D_80052F04)($3)\n"
        "\tli.s\t$f0,4.00000000000000000000e2\n"
        "\tlw\t$2,44($9)\n\tlw\t$3,48($9)\n\tlw\t$5,52($9)\n"
        "\tsw\t$2,88($7)\n\tsw\t$3,92($7)\n\tsw\t$5,96($7)\n"
        "\ts.s\t$f0,92($4)\n\tsw\t$0,252($4)\n"
        "\tlw\t$3,292($9)\n\tlw\t$5,88($4)\n\tlw\t$6,92($4)\n"
        "\tlw\t$7,96($4)\n\tli\t$2,1200\t\t\t# 0x000004b0\n"
        "\tsw\t$2,260($4)\n"
    )
    tail = replace_once(tail, old, new, "reviewed camera-mode Vec3 copy")
    tail = replace_once(
        tail, "\tsw\t$2,256($4)\n", "\tsw\t$3,256($4)\n",
        "reviewed camera-mode angle store",
    )
    return normalize_standard_return(head + marker + tail)


def normalize_particle_spray(text: str) -> str:
    """Reproduce retail allocation and scheduling for the five-particle loop."""
    text = replace_once(
        text,
        "\tbne\t$3,$2,$L1\n\tlui\t$20,%hi(D_80058208) # high\n",
        "\tbne\t$3,$2,$L1\n\tmove\t$19,$0\n",
        "reviewed particle-loop guard delay slot",
    )
    text = replace_once(text, (
        "\taddiu\t$20,$20,%lo(D_80058208) # low\n"
        "\tlui\t$21,%hi(D_80052F90) # high\n"
        "\taddiu\t$21,$21,%lo(D_80052F90) # low\n"
        "\tmove\t$19,$0\n\tsll\t$18,$16,3\n\taddu\t$18,$18,$16\n"
        "\tsll\t$18,$18,2\n\tsubu\t$18,$18,$16\n\tsll\t$18,$18,2\n"
        "\tmtc1\t$0,$f22\n\taddu\t$16,$20,$18\n"
    ), (
        "\tlui\t$2,%hi(D_80058208) # high\n"
        "\taddiu\t$20,$2,%lo(D_80058208) # low\n"
        "\tlui\t$2,%hi(D_80052F90) # high\n"
        "\taddiu\t$21,$2,%lo(D_80052F90) # low\n"
        "\tmtc1\t$0,$f22\n\tsll\t$2,$16,3\n\taddu\t$2,$2,$16\n"
        "\tsll\t$2,$2,2\n\tsubu\t$2,$2,$16\n\tsll\t$18,$2,2\n"
        "\taddu\t$16,$18,$20\n"
    ), "reviewed particle-loop setup")
    text = replace_once(text, (
        "\tli.s\t$f0,1.00000000000000000000e3\n"
        "\ttrunc.w.s $f6,$f20,$8\n\ts.s\t$f6,0($16)\n"
        "\tl.s\t$f2,48($17)\n\t#nop\n\tadd.s\t$f2,$f2,$f0\n"
        "\tl.s\t$f0,4($16)\n\tcvt.s.w\t$f0,$f0\n\tc.lt.s\t$f2,$f0\n"
    ), (
        "\ttrunc.w.s $f0,$f20,$8\n\ts.s\t$f0,0($16)\n"
        "\tl.s\t$f2,48($17)\n\tli.s\t$f0,1.00000000000000000000e3\n"
        "\tadd.s\t$f2,$f2,$f0\n\tl.s\t$f0,4($16)\n"
        "\tcvt.s.w\t$f0,$f0\n\tc.lt.s\t$f2,$f0\n"
    ), "reviewed particle height comparison")
    text = replace_once(text, (
        "\tl.s\t$f0,44($17)\n\t#nop\n\ttrunc.w.s $f6,$f0,$8\n"
        "\ts.s\t$f6,0($16)\n\tl.s\t$f2,48($17)\n"
        "\tli.s\t$f0,3.00000000000000000000e2\n\t#nop\n"
        "\tadd.s\t$f2,$f2,$f0\n\tsw\t$0,16($16)\n"
        "\ttrunc.w.s $f6,$f2,$8\n\ts.s\t$f6,4($16)\n"
        "\tl.s\t$f0,52($17)\n\t#nop\n\ttrunc.w.s $f6,$f0,$8\n"
        "\ts.s\t$f6,8($16)\n"
    ), (
        "\tl.s\t$f0,44($17)\n\t#nop\n\ttrunc.w.s $f2,$f0,$8\n"
        "\ts.s\t$f2,0($16)\n\tl.s\t$f0,48($17)\n"
        "\tli.s\t$f2,3.00000000000000000000e2\n\t#nop\n"
        "\tadd.s\t$f0,$f0,$f2\n\ttrunc.w.s $f2,$f0,$8\n"
        "\ts.s\t$f2,4($16)\n\tl.s\t$f0,52($17)\n\tsw\t$0,16($16)\n"
        "\ttrunc.w.s $f2,$f0,$8\n\ts.s\t$f2,8($16)\n"
    ), "reviewed particle reset stores")
    text = text.replace("\taddu\t$2,$20,$18\n", "\taddu\t$2,$18,$20\n")
    text = text.replace("\taddu\t$3,$20,$18\n", "\taddu\t$3,$18,$20\n")
    text = replace_once(text, (
        "\taddu\t$3,$18,$20\n\tl.s\t$f0,24($3)\n"
        "\tmfc1\t$5,$f22\n\tmfc1\t$6,$f22\n\tmfc1\t$7,$f22\n"
        "\ts.s\t$f0,16($sp)\n\tl.s\t$f0,32($sp)\n"
        "\taddu\t$18,$18,28\n\ts.s\t$f0,20($sp)\n"
        "\tl.s\t$f0,36($sp)\n\tmove\t$16,$2\n\ts.s\t$f0,24($sp)\n"
        "\tl.s\t$f0,40($sp)\n\tmove\t$4,$16\n"
    ), (
        "\taddu\t$3,$18,$20\n\tmfc1\t$5,$f22\n\tmfc1\t$6,$f22\n"
        "\tmfc1\t$7,$f22\n\tl.s\t$f0,24($3)\n\tmove\t$16,$2\n"
        "\ts.s\t$f0,16($sp)\n\tl.s\t$f0,32($sp)\n\tmove\t$4,$16\n"
        "\ts.s\t$f0,20($sp)\n\tl.s\t$f0,36($sp)\n\taddu\t$18,$18,28\n"
        "\ts.s\t$f0,24($sp)\n\tl.s\t$f0,40($sp)\n"
    ), "reviewed particle draw-call scheduling")
    text = replace_once(
        text, "\taddu\t$16,$20,$18\n", "\taddu\t$16,$18,$20\n",
        "reviewed particle next-pointer operands",
    )
    return normalize_standard_return(text)


def normalize_apply_board_pattern(
    text: str, selection_end: str = "$L17", rewrite_source: bool = True
) -> str:
    """Match retail's pattern-selection schedule and inner-loop allocation."""
    start = "\tmove\t$7,$2\n"
    end = selection_end + ":\n"
    if text.count(start) != 1 or text.count(end) != 1:
        raise ValueError("board-pattern selection span not found")
    head, rest = text.split(start, 1)
    _, tail = rest.split(end, 1)
    selection = (
        "\tmove\t$7,$2\n\tlh\t$3,8($7)\n"
        "\tli\t$2,83\t\t\t# 0x00000053\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$3,$2,$L3\n\tlui\t$5,%hi(D_8005409C) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$3,%hi(D_8005409C) # high\n\tlw\t$2,4($16)\n"
        "\taddiu\t$3,$3,%lo(D_8005409C) # low\n\tsll\t$2,$2,2\n"
        "\taddu\t$3,$3,$2\n\tlw\t$11,248($3)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        f"\tj\t{selection_end}\n\tlui\t$4,%hi(D_8005409C) # high\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "$L3:\n\taddiu\t$5,$5,%lo(D_8005409C) # low\n"
        "\taddu\t$2,$3,-71\n\tsll\t$4,$2,2\n\taddu\t$4,$4,$2\n"
        "\tsll\t$4,$4,2\n\tlui\t$3,%hi(D_80054240) # high\n"
        "\taddiu\t$3,$3,%lo(D_80054240) # low\n\tsll\t$2,$17,2\n"
        "\taddu\t$2,$2,$3\n\tlw\t$2,0($2)\n\taddu\t$4,$4,$5\n"
        "\tsll\t$2,$2,2\n\taddu\t$2,$2,$4\n\tlw\t$11,8($2)\n"
        "\tlui\t$4,%hi(D_8005409C) # high\n"
    )
    text = head + selection + end + tail
    loop_start = "$L8:\n"
    loop_end = "$L6:\n"
    if text.count(loop_start) != 1 or text.count(loop_end) != 1:
        raise ValueError("board-pattern inner loop span not found")
    head, rest = text.split(loop_start, 1)
    loop, tail = rest.split(loop_end, 1)
    loop = re.sub(r"\$4\b", "$PATREG", loop)
    loop = re.sub(r"\$5\b", "$4", loop)
    loop = loop.replace("$PATREG", "$5")
    if rewrite_source:
        loop = replace_once(
            loop, "\taddu\t$2,$11,$2\n\tlbu\t$2,0($2)\n",
            "\taddu\t$3,$11,$2\n\tlbu\t$2,0($3)\n",
            "reviewed board-pattern source pointer",
        )
    return normalize_standard_return(head + loop_start + loop + loop_end + tail)


def normalize_board_event_dispatch(text: str) -> str:
    """Use retail's notify/forced saved-register allocation."""
    start = "\tsubu\t$sp,$sp,48\n"
    body_start = "\tsll\t$2,$2,16\n"
    if text.count(start) != 1 or text.count(body_start) != 1:
        raise ValueError("board-event prologue span not found")
    head, rest = text.split(start, 1)
    _, body = rest.split(body_start, 1)
    prologue = (
        "\tsubu\t$sp,$sp,48\n\tsw\t$17,20($sp)\n\tmove\t$17,$4\n"
        "\tsw\t$21,36($sp)\n\tmove\t$21,$0\n\tsw\t$20,32($sp)\n"
        "\tli\t$20,1\t\t\t# 0x00000001\n\tsw\t$16,16($sp)\n"
        "\tmove\t$16,$20\n\tsw\t$31,40($sp)\n\tsw\t$19,28($sp)\n"
        "\tsw\t$18,24($sp)\n\tlh\t$4,336($17)\n\tlw\t$5,324($17)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8000E154\n\tmove\t$18,$21\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
    )
    epilogue = "$L1:\n"
    if body.count(epilogue) != 1:
        raise ValueError("board-event epilogue marker not found")
    middle, tail = body.split(epilogue, 1)
    middle = re.sub(r"\$18\b", "$EVENTREG", middle)
    middle = re.sub(r"\$20\b", "$18", middle)
    middle = middle.replace("$EVENTREG", "$20")
    return normalize_standard_return(
        head + prologue + body_start + middle + epilogue + tail
    )


def normalize_texture_block_loads(text: str) -> str:
    """Reproduce the reviewed SN allocation and scheduling of the 8-tile load loop."""
    start = text.find("\t.ent\tfunc_8001F588\n")
    end = text.find("\t.end\tfunc_8001F588\n", start)
    if start < 0 or end < 0:
        raise ValueError("reviewed texture-block function not found")
    body = text[start:end]
    loop_head = "\tmove\t$12,$0\n\tmove\t$10,$5\n"
    if body.count(loop_head) != 1:
        raise ValueError("reviewed texture-block loop head not found")
    split = body.index(loop_head)
    head, loop = body[:split], body[split:]
    if re.search(r"\$(9|10|11|12)\b", head):
        raise ValueError("reviewed texture-block loop registers used early")
    loop_map = {"9": "12", "10": "9", "11": "10", "12": "11"}
    loop = re.sub(
        r"\$(9|10|11|12)\b", lambda m: "$" + loop_map[m.group(1)], loop
    )
    block_start = loop.find("\tli\t$4,2048\t\t\t# 0x00000800\n")
    block_end = loop.find("\tor\t$2,$6,$2\n", block_start)
    if block_start < 0 or block_end < 0:
        raise ValueError("reviewed texture-block tile size span not found")
    block_end += len("\tor\t$2,$6,$2\n")
    block = re.sub(
        r"\$(4|6)\b", lambda m: "$6" if m.group(1) == "4" else "$4",
        loop[block_start:block_end],
    )
    loop = loop[:block_start] + block + loop[block_end:]
    loop = replace_once(loop, (
        "\tlw\t$2,0($5)\n"
        "\t#nop\n"
        "\tmove\t$3,$2\n"
        " #APP\n"
        " #NO_APP\n"
        "\tli\t$4,655360\t\t\t# 0x000a0000\n"
    ), (
        "\tlw\t$2,0($5)\n"
        "\tli\t$4,655360\t\t\t# 0x000a0000\n"
        "\tmove\t$3,$2\n"
        " #APP\n"
        " #NO_APP\n"
    ), "reviewed texture-block mode constant")
    loop = replace_once(loop, (
        "\tlw\t$2,0($9)\n"
        "\t#nop\n"
        "\tlw\t$2,12($2)\n"
        "\taddu\t$11,$11,1\n"
    ), (
        "\tlw\t$2,0($9)\n"
        "\taddu\t$11,$11,1\n"
        "\tlw\t$2,12($2)\n"
    ), "reviewed texture-block counter increment")
    text = text[:start] + head + loop + text[end:]
    return normalize_standard_return(text)


def normalize_board_item_seeding(text: str) -> str:
    def swap_registers(span: str, left: str, right: str) -> str:
        marker = "$@board_seed_swap"
        return span.replace(left, marker).replace(right, left).replace(marker, right)

    text = replace_once(text, (
        "\tlw\t$5,%lo(D_800581F0)($4)\n\tmove\t$4,$2\n"
        "\tsw\t$4,%lo(D_800B936C)($3)\n\tlh\t$2,4($5)\n"
    ), (
        "\tlw\t$4,%lo(D_800581F0)($4)\n\tmove\t$5,$2\n"
        "\tsw\t$5,%lo(D_800B936C)($3)\n\tlh\t$2,4($4)\n"
    ), "reviewed board-seeding entry allocation")
    text = replace_once(text, (
        "\tmove\t$14,$5\n\tmove\t$15,$4\n #APP\n #NO_APP\n"
        "\tmove\t$17,$3\n\tli\t$16,-1\t\t\t# 0xffffffff\n"
        "\tmove\t$13,$10\n\tli\t$12,8212\t\t\t# 0x00002014\n$L6:\n"
    ), (
        "\tmove\t$14,$4\n\tmove\t$17,$3\n\tmove\t$15,$5\n #APP\n #NO_APP\n"
        "\tli\t$16,-1\t\t\t# 0xffffffff\n"
        "\tli\t$12,8212\t\t\t# 0x00002014\n\tmove\t$13,$10\n$L6:\n"
    ), "reviewed board-seeding preheader schedule")
    start = text.find("\tli\t$12,8212\t\t\t# 0x00002014\n\tmove\t$13,$10\n$L6:\n")
    end = text.find("\taddu\t$12,$12,528\n", start)
    if start < 0 or end < 0:
        raise ValueError("reviewed board-seeding row loop not found")
    end += len("\taddu\t$12,$12,528\n")
    text = text[:start] + swap_registers(text[start:end], "$12", "$13") + text[end:]
    text = replace_once(text, "$L5:\n\taddu\t$12,$12,528\n", "$L5:\n\taddu\t$13,$13,528\n",
                        "reviewed board-seeding row increment order")
    text = replace_once(text, "\tbne\t$2,$0,$L6\n\taddu\t$13,$13,528\n",
                        "\tbne\t$2,$0,$L6\n\taddu\t$12,$12,528\n",
                        "reviewed board-seeding row increment order")
    store = (
        "\taddu\t$3,$11,$7\n\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$4,%lo(D_80054810)($17)\n\t#nop\n"
        "\tsll\t$2,$4,3\n\taddu\t$2,$2,$15\n\taddu\t$4,$4,1\n"
        "\tsw\t$3,0($2)\n\tsh\t$16,8($3)\n\tsw\t$10,4($2)\n"
    )
    text = replace_once(text, store, swap_registers(store, "$2", "$3"),
                        "reviewed board-seeding slot store allocation")
    return normalize_standard_return(text)


def normalize_player_scoreboard(text: str) -> str:
    text = replace_once(text, (
        "\tsw\t$0,32($sp)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tblez\t$2,$L9\n"
        "\tsw\t$0,36($sp)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tblez\t$2,$L9\n"
        "\tsw\t$0,32($sp)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tsw\t$0,36($sp)\n"
    ), "reviewed scoreboard loop-state initialization")
    text = replace_once(text, (
        "\tlw\t$2,36($sp)\n\tlw\t$4,248($22)\n\taddu\t$17,$2,83\n"
    ), (
        "\tlw\t$8,36($sp)\n\tlw\t$4,248($22)\n\taddu\t$17,$8,83\n"
    ), "reviewed scoreboard first row coordinate")
    old = (
        "\tli\t$4,255\t\t\t# 0x000000ff\n"
        "\tmove\t$5,$4\n\tmove\t$6,$4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80033144\n\tmove\t$7,$4\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlw\t$2,36($sp)\n"
        "\tli\t$4,41\t\t\t# 0x00000029\n"
    )
    new = (
        "\tlw\t$8,36($sp)\n"
        "\tli\t$4,255\t\t\t# 0x000000ff\n"
        "\tmove\t$5,$4\n\tmove\t$6,$4\n\tmove\t$7,$4\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80033144\n\taddu\t$17,$8,81\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli\t$4,41\t\t\t# 0x00000029\n"
    )
    text = replace_once(text, old, new, "reviewed scoreboard selected-player coordinate")
    text = replace_once(text, (
        "\tli\t$4,41\t\t\t# 0x00000029\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80032870\n\taddu\t$17,$2,81\n"
    ), (
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80032870\n\tli\t$4,41\t\t\t# 0x00000029\n"
    ), "reviewed scoreboard resource lookup delay slot")

    start = "$L24:\n"
    end = "\tlw\t$2,36($sp)\n\tlui\t$3,%hi(D_80057D5C) # high\n"
    if text.count(start) != 1 or text.count(end) != 1:
        raise ValueError("reviewed scoreboard inner-loop bounds not found")
    head, rest = text.split(start, 1)
    body, tail = rest.split(end, 1)
    body = body.replace("$fp", "$R30").replace("$23", "$fp").replace("$R30", "$23")
    text = head + start + body + end + tail
    text = replace_once(text, (
        "$L24:\n\tmove\t$21,$20\n\tlw\t$23,36($sp)\n\tmove\t$19,$22\n"
    ), (
        "$L24:\n\tlw\t$23,36($sp)\n\tmove\t$21,$20\n\tmove\t$19,$22\n"
    ), "reviewed scoreboard row-coordinate load placement")

    text = replace_once(text, (
        "\tjal\tfunc_80013724\n\taddu\t$19,$19,2\n"
    ), (
        "\tjal\tfunc_80013724\n\taddu\t$17,$23,87\n"
    ), "reviewed scoreboard text-coordinate refresh")
    text = replace_once(text, (
        "\tlh\t$6,504($22)\n\taddu\t$20,$20,1\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8003A57C\n\taddu\t$21,$21,56\n"
    ), (
        "\tlh\t$6,504($22)\n\taddu\t$21,$21,56\n\taddu\t$19,$19,2\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_8003A57C\n\taddu\t$20,$20,1\n"
    ), "reviewed scoreboard inner-loop increments")
    text = replace_once(text, "\tbne\t$2,$0,$L16\n\taddu\t$17,$23,87\n",
                        "\tbnel\t$2,$0,$L16\n\taddu\t$17,$23,87\n",
                        "reviewed scoreboard likely loop branch")
    text = replace_once(text, (
        "\tlw\t$2,36($sp)\n"
        "\tlui\t$3,%hi(D_80057D5C) # high\n"
        "\taddu\t$2,$2,24\n\tsw\t$2,36($sp)\n"
        "\tlw\t$2,32($sp)\n"
        "\tlw\t$3,%lo(D_80057D5C)($3)\n"
        "\taddu\t$2,$2,1\n\tsw\t$2,32($sp)\n"
        "\tslt\t$2,$2,$3\n\tbne\t$2,$0,$L11\n"
    ), (
        "\tlw\t$8,36($sp)\n"
        "\tlui\t$2,%hi(D_80057D5C) # high\n"
        "\taddu\t$8,$8,24\n\tsw\t$8,36($sp)\n"
        "\tlw\t$8,32($sp)\n"
        "\tlw\t$2,%lo(D_80057D5C)($2)\n"
        "\taddu\t$8,$8,1\n\tslt\t$2,$8,$2\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tbne\t$2,$0,$L11\n\tsw\t$8,32($sp)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
    ), "reviewed scoreboard outer-loop update")
    return normalize_standard_return(text)


def normalize(text: str) -> str:
    if "\t.ent\tfunc_800124B0\n" in text:
        return normalize_round_controller_create(text)
    if "\t.ent\tfunc_80012324\n" in text:
        return normalize_round_state_reset(text)
    if "\t.ent\tfunc_800299A8\n" in text:
        return normalize_zone_choice_input(text)
    if "\t.ent\tfunc_8000DE80\n" in text:
        return normalize_room_grid_tile(text)
    if "\t.ent\tfunc_8002B6E0\n" in text:
        return normalize_other_player_vectors(text)
    if "\t.ent\tfunc_8001361C\n" in text:
        return normalize_indexed_resource_pointer(text)
    if "\t.ent\tfunc_800247C8\n" in text:
        return normalize_board_player_input(text)
    if "\t.ent\tfunc_8000D0AC\n" in text:
        return normalize_sprite_display_list(text)
    if "\t.ent\tfunc_8002903C\n" in text:
        return normalize_wrapped_actor_index(text)
    if "\t.ent\tfunc_80016DB0\n" in text:
        return normalize_menu_entry_reset(text)
    if "\t.ent\tfunc_8002D4C0\n" in text:
        return normalize_packed_state_dispatch(text)
    if "\t.ent\tfunc_800222E8\n" in text:
        return normalize_tile_collision_search(text)
    if "\t.ent\tfunc_800255AC\n" in text:
        return normalize_available_position_search(text)
    if "\t.ent\tfunc_80025E48\n" in text:
        return normalize_completion_state_scan(text)
    if "\t.ent\tfunc_8000E630\n" in text:
        return normalize_e4_func_8000E630(text)
    if "\t.ent\tfunc_8000E680\n" in text:
        return normalize_e4_func_8000E680(text)
    if "\t.ent\tfunc_8000DB84\n" in text:
        return normalize_e4_func_8000DB84(text)
    if "\t.ent\tfunc_80004A18\n" in text:
        return normalize_e4_func_80004A18(text)
    if "\t.ent\tfunc_8000E1D0\n" in text:
        return normalize_e4_func_8000E1D0(text)
    if "\t.ent\tfunc_8000B71C\n" in text:
        return normalize_e4_func_8000B71C(text)
    if "\t.ent\tfunc_80006C18\n" in text:
        return normalize_e4_func_80006C18(text)
    if "\t.ent\tfunc_80017144\n" in text:
        return normalize_standard_return(normalize_d8_func_80017144(text))
    if "\t.ent\tfunc_80018A34\n" in text:
        return normalize_standard_return(normalize_d8_func_80018A34(text))
    if "\t.ent\tfunc_80010358\n" in text:
        return normalize_d6_func_80010358(text)
    if "\t.ent\tfunc_80029DC4\n" in text:
        return normalize_d5_func_80029DC4(text)
    if "\t.ent\tfunc_8001B08C\n" in text:
        return normalize_d3_func_8001B08C(text)
    if "\t.ent\tfunc_8001AA80\n" in text:
        return normalize_d3_func_8001AA80(text)
    if "\t.ent\tfunc_8001E7FC\n" in text:
        return normalize_d3_func_8001E7FC(text)
    if "\t.ent\tfunc_8000715C\n" in text:
        return normalize_d1_func_8000715C(text)
    if "\t.ent\tfunc_800092C4\n" in text:
        return normalize_d1_func_800092C4(text)
    if "\t.ent\tfunc_80005798\n" in text:
        return normalize_d1_func_80005798(text)
    if "\t.ent\tfunc_8002FEFC\n" in text:
        return normalize_c08_func_8002FEFC(text)
    if "\t.ent\tfunc_80035890\n" in text:
        return normalize_c08_func_80035890(text)
    if "\t.ent\tfunc_800304A4\n" in text:
        return normalize_c08_func_800304A4(text)
    if "\t.ent\tfunc_8001DF10\n" in text:
        return normalize_c04_func_8001DF10(text)
    if "\t.ent\tfunc_800191EC\n" in text:
        return normalize_c03_func_800191EC(text)
    if "\t.ent\tfunc_80010BD8\n" in text:
        return normalize_c02_func_80010BD8(text)
    if "\t.ent\tfunc_8000E17C\n" in text:
        return normalize_c01_func_8000E17C(text)
    if "\t.ent\tfunc_8000A63C\n" in text:
        return normalize_c01_func_8000A63C(text)
    if "\t.ent\tfunc_80009610\n" in text:
        return normalize_c00_func_80009610(text)
    if "\t.ent\tfunc_80004954\n" in text:
        return normalize_c00_host_read_registers(text)
    if "\t.ent\tfunc_8001298C\n" in text:
        return normalize_mode_switch_store(text)
    if "\t.ent\tfunc_8002E5B8\n" in text:
        return normalize_best_slot_epilogue(text)
    if "\t.ent\tfunc_8002F6D0\n" in text:
        return normalize_meter_index_temp(text)
    if "\t.ent\tfunc_80031C2C\n" in text:
        return normalize_slot_record_clear(text)
    if "\t.ent\tfunc_80027F48\n" in text:
        return normalize_frame_quota_return(text)
    if "\t.ent\tfunc_80008B80\n" in text:
        return normalize_point_distance(text)
    if "\t.ent\tfunc_80009BE0\n" in text:
        return normalize_marker_slot_search(text)
    if "\t.ent\tfunc_8000DC38\n" in text:
        return normalize_grid_cell_search(text)
    if "\t.ent\tfunc_8000E078\n" in text:
        return normalize_room_point_search(text)
    if "\t.ent\tfunc_8000F150\n" in text:
        return normalize_camera_focus_setup(text)
    if "\t.ent\tfunc_80010B0C\n" in text:
        return normalize_camera_heading(text)
    if "\t.ent\tfunc_80034978\n" in text:
        return normalize_two_player_minigame(text)
    if "\t.ent\tfunc_80026824\n" in text:
        return normalize_board_scene_renderer(text)
    if "\t.ent\tfunc_8000F604\n" in text:
        return normalize_camera_follow_math(text)
    if "\t.ent\tfunc_8000FDAC\n" in text:
        return normalize_camera_mode_setup(text)
    if "\t.ent\tfunc_8000E32C\n" in text:
        return normalize_particle_spray(text)
    if "\t.ent\tfunc_8002637C\n" in text:
        return normalize_apply_board_pattern(text)
    if "\t.ent\tfunc_80026560\n" in text:
        return normalize_apply_board_pattern(text, "$L16", False)
    if "\t.ent\tfunc_80025730\n" in text:
        return normalize_board_event_dispatch(text)
    if "\t.ent\tfunc_80034808\n" in text:
        return normalize_player_object_reset(text)
    if "\t.ent\tfunc_800345EC\n" in text:
        return normalize_player_marker_copies(text)
    if "\t.ent\tfunc_80030BE8\n" in text:
        return normalize_sound_select_store(text)
    if "\t.ent\tfunc_80030F74\n" in text:
        return normalize_scene_list_tail_call(text)
    if "\t.ent\tfunc_8003376C\n" in text:
        return normalize_frame_toggle_epilogue(text)
    if "\t.ent\tfunc_800344A8\n" in text:
        return normalize_marker_position_copy(text)
    if "\t.ent\tfunc_80031EDC\n" in text:
        return normalize_stick_choice_registers(text)
    if "\t.ent\tfunc_8003187C\n" in text:
        return normalize_marker_camera_tail(text)
    if "\t.ent\tfunc_80033290\n" in text:
        return normalize_triangle_draw_registers(text)
    if "\t.ent\tfunc_8002B178\n" in text:
        return normalize_nearest_target_message(text)
    if "\t.ent\tfunc_8002D0A8\n" in text:
        return normalize_angle_dial_update(text)
    if "\t.ent\tfunc_8002DE5C\n" in text:
        return normalize_trade_capacity_score(text)
    if "\t.ent\tfunc_8002EBF0\n" in text:
        return normalize_trade_state_machine(text)
    if "\t.ent\tfunc_8000E7C8\n" in text:
        return normalize_camera_state_machine(text)
    if "\t.ent\tfunc_8002C714\n" in text:
        return normalize_player_item_row(text)
    if "\t.ent\tfunc_80028C18\n" in text:
        return normalize_boot_timer_tail(text)
    if "\t.ent\tfunc_80028E9C\n" in text:
        return normalize_state_flag_clear_tail(text)
    if "\t.ent\tfunc_80029314\n" in text:
        return normalize_actor_angle_step(text)
    if "\t.ent\tfunc_8002A1B4\n" in text:
        return normalize_baby_banish_sequence(text)
    if "\t.ent\tfunc_80027FD4\n" in text:
        return normalize_level_exit_frames(text)
    if "\t.ent\tfunc_80020050\n" in text:
        return normalize_hunt_list_reset(text)
    if "\t.ent\tfunc_8001C404\n" in text:
        return normalize_bonus_spawn_table(text)
    if "\t.ent\tfunc_80013EE0\n" in text:
        return normalize_rsp_task_submit(text)
    if "\t.ent\tfunc_80013BC8\n" in text:
        return normalize_frame_display_list_start(text)
    if "\t.ent\tfunc_80016FE4\n" in text:
        return normalize_menu_icon_draw_tail(text)
    if "\t.ent\tfunc_8001749C\n" in text:
        return normalize_menu_icon_draw_tail(text)
    if "\t.ent\tfunc_800146B0\n" in text:
        return normalize_material_fog_off_commands(text)
    if "\t.ent\tfunc_80014814\n" in text:
        return normalize_material_lighting_commands(text)
    if "\t.ent\tfunc_80014F00\n" in text:
        return normalize_scene_material_renderer(text)
    if "\t.ent\tfunc_80032A20\n" in text:
        return normalize_sprite_glyph_renderer(text)
    if "\t.ent\tfunc_80012DBC\n" in text:
        return normalize_byte_from_float_trunc(text)
    if "\t.ent\tfunc_80010850\n" in text:
        return normalize_camera_orbit_copy(text)
    if "\t.ent\tfunc_80013358\n" in text:
        return normalize_resource_slot_load(text)
    if "\t.ent\tfunc_8001116C\n" in text:
        return normalize_controller_stick_deadzone(text)
    if "\t.ent\tfunc_80011A40\n" in text:
        return normalize_preview_model_loop(text)
    if "\t.ent\tfunc_8000B910\n" in text:
        return normalize_depth_node_insert(text)
    if "\t.ent\tfunc_8000BB80\n" in text:
        return normalize_camera_depth_node_insert(text)
    if "\t.ent\tfunc_8000C034\n" in text:
        return normalize_far_node_append(text)
    if "\t.ent\tfunc_8000C674\n" in text:
        return normalize_animation_frame_step(text)
    if "\t.ent\tfunc_8000CAB0\n" in text:
        return normalize_pair_value_copy(text)
    if "\t.ent\tfunc_8000D838\n" in text:
        return normalize_fade_state_step(text)
    if "\t.ent\tfunc_8000F800\n" in text:
        return normalize_camera_matrix_commands(text)
    if "\t.ent\tfunc_8000D970\n" in text:
        return normalize_scene_grid_extent(text)
    if "\t.ent\tfunc_80008E20\n" in text:
        return normalize_random_handle_refresh(text)
    if "\t.ent\tfunc_80009930\n" in text:
        return normalize_result_message_states(text)
    if "\t.ent\tfunc_800065FC\n" in text:
        return normalize_warp_tile_scan(text)
    if "\t.ent\tfunc_8000771C\n" in text:
        return normalize_turn_step_update(text)
    if "\t.ent\tfunc_8001330C\n" in text:
        return normalize_resource_range(text)
    if "\t.ent\tfunc_8002F504\n" in text:
        return normalize_player_meter_update(text)
    if "\t.ent\tfunc_8002C8CC\n" in text or "\t.ent\tfunc_8002CA8C\n" in text:
        return normalize_large_leaf_frame(text)
    if "\t.ent\tfunc_8002673C\n" in text:
        return normalize_object_array_init(text)
    if "\t.ent\tfunc_80006468\n" in text:
        return normalize_actor_facing(text)
    if "\t.ent\tfunc_80025D60\n" in text:
        return normalize_owner_count(text, "$L13")
    if "\t.ent\tfunc_80025CCC\n" in text:
        return normalize_owner_count(text, "$L14")
    if "\t.ent\tfunc_8000DDAC\n" in text:
        return normalize_room_index_store(text)
    if "\t.ent\tfunc_80030E2C\n" in text:
        return normalize_runtime_callback_setup(text)
    if "\t.ent\tfunc_800327A0\n" in text:
        return normalize_state_finish_branch(text)
    if "\t.ent\tfunc_80011CC4\n" in text:
        return normalize_effect_loop_branch(text)
    if "\t.ent\tfunc_8000AE0C\n" in text:
        return normalize_transition_state(text)
    if "\t.ent\tfunc_80005654\n" in text:
        return normalize_object_cleanup(text)
    if "\t.ent\tfunc_800138FC\n" in text:
        return normalize_text_width_accumulation(text)
    if "\t.ent\tfunc_800094A4\n" in text:
        return normalize_unique_actor_test(text)
    if "\t.ent\tfunc_80025C20\n" in text:
        return normalize_owner_search(text)
    if "\t.ent\tfunc_8000C490\n" in text:
        return normalize_node_result(text)
    if "\t.ent\tfunc_8003079C\n" in text:
        return normalize_scene_start(text)
    if "\t.ent\tfunc_80031328\n" in text:
        return normalize_random_slot_assignment(text)
    if "\t.ent\tfunc_80034350\n" in text:
        return normalize_player_marker_draw(text)
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
    if "\t.ent\tfunc_80019A4C\n" in text:
        return normalize_display_list_relocation(text)
    if "\t.ent\tfunc_80023860\n" in text:
        return normalize_object_tile_event(text)
    if "\t.ent\tfunc_80023F50\n" in text:
        return normalize_object_activation_filter(text)
    if "\t.ent\tfunc_8001D91C\n" in text:
        return normalize_main_state_driver(text)
    if "\t.ent\tfunc_80015DF4\n" in text:
        return normalize_actor_creation(text)
    if "\t.ent\tfunc_8001BF78\n" in text:
        return normalize_prize_wheel(text)
    if "\t.ent\tfunc_8001E2B0\n" in text:
        return normalize_scene_camera_pass(text)
    if "\t.ent\tfunc_80015740\n" in text:
        return normalize_large_render_pass(text)
    if "\t.ent\tfunc_8001885C\n" in text:
        return normalize_label_menu_draw(text)
    if "\t.ent\tfunc_8001D1CC\n" in text:
        return normalize_player_screen_positions(text)
    if "\t.ent\tfunc_8001D2F4\n" in text:
        return normalize_selected_screen_positions(text)
    if "\t.ent\tfunc_8001D130\n" in text:
        return normalize_single_player_screen_positions(text)
    if "\t.ent\tfunc_80035D0C\n" in text:
        return normalize_spin_object_setup(text)
    if "\t.ent\tfunc_80035A2C\n" in text:
        return normalize_spin_object_motion(text)
    if "\t.ent\tfunc_80035BD0\n" in text:
        return normalize_spin_object_targets(text)
    if "\t.ent\tfunc_80035E74\n" in text:
        return normalize_random_spin_objects(text)
    if "\t.ent\tfunc_8000A750\n" in text:
        return normalize_debris_spawn(text)
    if "\t.ent\tfunc_8001D418\n" in text:
        return normalize_board_space_label(text)
    if "\t.ent\tfunc_80009634\n" in text:
        return normalize_actor_event_completion(text)
    if "\t.ent\tfunc_8001FC2C\n" in text:
        return normalize_frame_display_lists(text)
    if "\t.ent\tfunc_8002E930\n" in text:
        return normalize_player_scoreboard(text)
    if "\t.ent\tfunc_80031408\n" in text:
        return normalize_board_item_seeding(text)
    if "\t.ent\tfunc_8001F588\n" in text:
        return normalize_texture_block_loads(text)
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
