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


def normalize_find_record_pair(text: str) -> str:
    text = replace_once(
        text,
        "\tbeql\t$2,$6,.L27\n",
        "\tbeq\t$2,$6,.L27\n",
        "second-key match branch",
    )
    return replace_once(
        text,
        "\tbeql\t$2,$5,.L27\n",
        "\tbeq\t$2,$5,.L27\n",
        "first-key match branch",
    )


def normalize_check_pak_channel(text: str) -> str:
    text = replace_once(
        text,
        "\tsw\t$18,24($sp)\n"
        "\tmove\t$18,$4\n"
        "\tla\t$4,D_80058B30\n"
        "\tsw\t$16,16($sp)\n"
        "\tla\t$16,D_80058B9C\n"
        "\tmove\t$5,$16\n"
        "\tsw\t$31,32($sp)\n"
        "\tsw\t$19,28($sp)\n"
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "\tjal\tfunc_8004E270\n"
        "\tsw\t$17,20($sp)\n",
        "\tsw\t$18,24($sp)\n"
        "\tmove\t$18,$4\n"
        "\tsw\t$17,20($sp)\n"
        "\tmove\t$17,$0\n"
        "\tlui\t$4,%hi(D_80058B30)\n"
        "\taddiu\t$4,$4,%lo(D_80058B30)\n"
        "\tsw\t$16,16($sp)\n"
        "\tlui\t$16,%hi(D_80058B9C)\n"
        "\taddiu\t$5,$16,%lo(D_80058B9C)\n"
        "\tsw\t$31,32($sp)\n"
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "\tjal\tfunc_8004E270\n"
        "\tsw\t$19,28($sp)\n",
        "pak-channel prologue scheduling",
    )
    text = replace_once(
        text,
        "\tbne\t$2,$0,.L2\n\tmove\t$17,$0\n",
        "\tbne\t$2,$0,.L2\n\tmove\t$16,$17\n",
        "pak-channel initialized loop counter",
    )
    text = replace_once(
        text,
        "\tlbu\t$2,0($16)\n",
        "\tlbu\t$2,%lo(D_80058B9C)($16)\n",
        "pak-channel status offset",
    )
    text = replace_once(
        text,
        "\tmove\t$16,$0\n\taddu\t$2,$2,$18\n",
        "\taddu\t$2,$2,$18\n",
        "pak-channel redundant loop-counter reset",
    )
    return replace_once(
        text,
        "\tla\t$19,D_80058990\n",
        "\tlui\t$2,%hi(D_80058990)\n\taddiu\t$19,$2,%lo(D_80058990)\n",
        "pak-channel record base address",
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


def normalize_game_object_timer(text: str) -> str:
    text = replace_once(text,
        "\t.frame\t$sp,8,$31\t\t# vars= 8, regs= 0/0, args= 0, extra= 0\n",
        "\t.frame\t$sp,0,$31\t\t# vars= 0, regs= 0/0, args= 0, extra= 0\n",
        "game-object timer empty frame")
    text = replace_once(text, (
        "\tlh\t$2,404($4)\n\tsubu\t$sp,$sp,8\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tblez\t$2,.L2\n\tmove\t$3,$2\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\taddu\t$2,$3,-1\n"
    ), (
        "\tlh\t$2,404($4)\n\tlhu\t$3,404($4)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tblez\t$2,.L2\n\taddu\t$2,$3,-1\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
    ), "game-object timer unsigned decrement")
    text = replace_once(text,
        "\tlw\t$3,D_800B92F4\n",
        "\tlui\t$2,%hi(D_800B92F4)\n\tlw\t$3,%lo(D_800B92F4)($2)\n",
        "game-object timer state address register")
    return replace_once(text,
        ".L3:\n\taddu\t$sp,$sp,8\n\tj\t$31\n",
        ".L3:\n\tj\t$31\n",
        "game-object timer empty epilogue")


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


def normalize_scene_shutdown(text: str) -> str:
    text = replace_once(
        text,
        "\tlw\t$4,D_80059DA8\n",
        "\tlui\t$2,0x8006\n\tlw\t$4,-0x6258($2)\n",
        "scene-shutdown initial argument load",
    )
    return replace_once(
        text,
        "\tmove\t$5,$0\n",
        "\tmove\t$5,$4\n",
        "scene-shutdown paired zero argument",
    )


def normalize_actor_mode_setup(text: str) -> str:
    return replace_once(
        text,
        "\tmove\t$6,$0\n",
        "\tmove\t$6,$4\n",
        "actor-mode reused zero argument",
    )


def normalize_grid_actor_lookup(text: str) -> str:
    return replace_once(
        text,
        "\tlw\t$4,D_80057D40\n",
        "\tlui\t$2,0x8005\n\tlw\t$4,0x7d40($2)\n",
        "grid-actor lookup argument load",
    )


def normalize_scene_handle_call(text: str) -> str:
    return replace_once(
        text,
        "\tlw\t$4,D_80059DA8\n",
        "\tlui\t$2,0x8006\n\tlw\t$4,-0x6258($2)\n",
        "scene-handle call argument load",
    )


def normalize_state_global_store(text: str) -> str:
    return replace_once(
        text,
        "\tsw\t$2,D_800B9344\n",
        "\tlui\t$3,0x800c\n\tsw\t$2,-0x6cbc($3)\n",
        "state global store address",
    )


def normalize_conditional_state_store(text: str) -> str:
    text = normalize_state_global_store(text)
    return replace_once(
        text,
        "\tbeq\t$2,$0,.L2\n\tli\t$2,0x00000005\t\t# 5\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlui\t$3,0x800c\n",
        "\tbeq\t$2,$0,.L2\n\tlui\t$3,0x800c\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli\t$2,0x00000005\t\t# 5\n",
        "conditional state-store scheduling",
    )


def normalize_sequence_state_store(text: str) -> str:
    text = normalize_state_global_store(text)
    return replace_once(
        text,
        "\tli\t$2,0x00000001\t\t# 1\n"
        "\tlui\t$3,0x800c\n\tsw\t$2,-0x6cbc($3)\n"
        "\tlw\t$31,16($sp)\n\taddu\t$sp,$sp,24\n\tj\t$31\n",
        "\tlui\t$3,0x800c\n\tli\t$2,0x00000001\t\t# 1\n"
        "\tlw\t$31,16($sp)\n\tsw\t$2,-0x6cbc($3)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\taddu\t$sp,$sp,24\n"
        "\t.set\tmacro\n\t.set\treorder\n",
        "sequence state-store epilogue scheduling",
    )


def normalize_runtime_reset_store(text: str) -> str:
    text = replace_once(
        text,
        "\tsw\t$0,D_800545E0\n",
        "\tlui\t$2,0x8005\n\tsw\t$0,0x45e0($2)\n",
        "runtime-reset global store address",
    )
    return replace_once(
        text,
        "\tlui\t$2,0x8005\n\tsw\t$0,0x45e0($2)\n"
        "\tlw\t$31,16($sp)\n\taddu\t$sp,$sp,24\n\tj\t$31\n",
        "\tlui\t$2,0x8005\n\tlw\t$31,16($sp)\n"
        "\tsw\t$0,0x45e0($2)\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tj\t$31\n\taddu\t$sp,$sp,24\n"
        "\t.set\tmacro\n\t.set\treorder\n",
        "runtime-reset epilogue scheduling",
    )


def normalize_mode_dispatch(text: str) -> str:
    return replace_once(
        text,
        "\tlw\t$3,D_80052F18\n\tsubu\t$sp,$sp,24\n",
        "\tsubu\t$sp,$sp,24\n\tlui\t$2,0x8005\n\tlw\t$3,0x2f18($2)\n",
        "mode-dispatch prologue and global load",
    )


def normalize_idle_thread(text: str) -> str:
    return replace_once(
        text,
        "\tmove\t$4,$0\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80049090\n\tmove\t$5,$0\n",
        "\tmove\t$4,$0\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80049090\n\tmove\t$5,$4\n",
        "idle-thread paired zero argument",
    )


def normalize_actor_position(text: str) -> str:
    return replace_once(
        text,
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80005598\n\tsw\t$2,292($16)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tlh\t$5,336($16)\n\tlw\t$7,328($16)\n"
        "\taddu\t$4,$sp,16\n",
        "\t.set\tnoreorder\n\t.set\tnomacro\n"
        "\tjal\tfunc_80005598\n\tsw\t$2,292($16)\n"
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\taddu\t$4,$sp,16\n\tlh\t$5,336($16)\n"
        "\tlw\t$7,328($16)\n",
        "actor-position output argument scheduling",
    )


def normalize_c06_func_80027978(text: str) -> str:
    """Swap the table pointer ($3) and loaded entry ($2) registers in the
    identifier table scan; the return-value writes to $2 are unchanged."""
    old = (
        "\tla\t$3,D_80054268\n.L2:\n\tlw\t$2,0($3)\n\t#nop\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tbeql\t$2,$4,.L4\n"
        "\tli\t$2,0x00000001\t\t# 1\n\t.set\tmacro\n\t.set\treorder\n\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tbne\t$2,$5,.L2\n\taddu\t$3,$3,4\n"
    )
    new = (
        "\tla\t$2,D_80054268\n.L2:\n\tlw\t$3,0($2)\n\t#nop\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tbeql\t$3,$4,.L4\n"
        "\tli\t$2,0x00000001\t\t# 1\n\t.set\tmacro\n\t.set\treorder\n\n"
        "\t.set\tnoreorder\n\t.set\tnomacro\n\tbne\t$3,$5,.L2\n\taddu\t$2,$2,4\n"
    )
    if text.count(old) != 1:
        raise ValueError("func_80027978 table-scan register pattern not found")
    return text.replace(old, new)


def normalize_c09_func_8003ACF0(text: str) -> str:
    """Swap the loaded word ($6) and xor result ($3) registers in the
    word-at-a-time scan of func_8003ACF0; values and sources are unchanged."""
    start = text.find("\t.ent\tfunc_8003ACF0\n")
    head = text.find(".L9:\n\tlw\t$6,0($4)\n\t#nop\n\txor\t$3,$6,$5\n", start)
    tail = text.find("\n.L7:\n\tmove\t$2,$0\n", head)
    if start < 0 or head < 0 or tail < 0:
        raise ValueError("func_8003ACF0 word-scan register pattern not found")
    body = text[head:tail]
    if body.count("$3") != 5 or body.count("$6") != 6:
        raise ValueError("func_8003ACF0 word-scan register counts changed")
    body = body.replace("$3", "$@").replace("$6", "$3").replace("$@", "$6")
    text = text[:head] + body + text[tail:]
    # The retail assembler filled the return delay slot with the frame pop.
    epilogue = "\taddu\t$sp,$sp,8\n\tj\t$31\n\t.end\tfunc_8003ACF0\n"
    if text.count(epilogue) != 1:
        raise ValueError("func_8003ACF0 epilogue pattern not found")
    return text.replace(epilogue,
        "\t.set\tnoreorder\n\tj\t$31\n\taddu\t$sp,$sp,8\n\t.set\treorder\n\t.end\tfunc_8003ACF0\n")


def normalize_d1_func_80006E88(text):
    # Pure reorders: retail schedules the call constant before the angle load,
    # and the parity mask before the angle load in both turn arms.
    text = replace_once(text, (
        "\tlw\t$2,296($16)\n"
        "\tli\t$4,0x00000006\t\t# 6\n"
    ), (
        "\tli\t$4,0x00000006\t\t# 6\n"
        "\tlw\t$2,296($16)\n"
    ), "reviewed turn-finish call constant scheduling")
    old = (
        "\tlw\t$7,292($16)\n"
        "\tandi\t$2,$4,0x0001\n"
    )
    new = (
        "\tandi\t$2,$4,0x0001\n"
        "\tlw\t$7,292($16)\n"
    )
    if text.count(old) != 2:
        raise ValueError("reviewed turn-arm parity scheduling not found twice")
    return text.replace(old, new)


def normalize_formatted_output_core(text: str) -> str:
    """Reproduce retail's frame gap and s2/s3 pointer allocation."""
    if "\t.frame\t$sp,128,$31" in text:
        text = replace_once(text, "\t.frame\t$sp,128,$31", "\t.frame\t$sp,144,$31",
                            "formatted-output frame declaration")
        text = replace_once(text, "\tsubu\t$sp,$sp,128\n", "\tsubu\t$sp,$sp,144\n",
                            "formatted-output frame allocation")
        for register, old_offset, new_offset in (
            ("31", 116, 132), ("fp", 112, 128), ("23", 108, 124),
            ("22", 104, 120), ("21", 100, 116), ("20", 96, 112),
            ("19", 92, 108), ("18", 88, 104), ("17", 84, 100),
            ("16", 80, 96),
        ):
            text = replace_once(
                text, f"\tsw\t${register},{old_offset}($sp)\n",
                f"\tsw\t${register},{new_offset}($sp)\n",
                "formatted-output saved-register store",
            )
            text = replace_once(
                text, f"\tlw\t${register},{old_offset}($sp)\n",
                f"\tlw\t${register},{new_offset}($sp)\n",
                "formatted-output saved-register load",
            )
        text = replace_once(text, "\ts.d\t$f20,120($sp)\n", "\ts.d\t$f20,136($sp)\n",
                            "formatted-output floating save")
        text = replace_once(text, "\tl.d\t$f20,120($sp)\n", "\tl.d\t$f20,136($sp)\n",
                            "formatted-output floating restore")
        text = replace_once(text, "\taddu\t$sp,$sp,128\n", "\taddu\t$sp,$sp,144\n",
                            "formatted-output frame release")
    elif text.count("\t.frame\t$sp,144,$31") != 1:
        raise ValueError("formatted-output frame declaration not found uniquely")

    body_start = "\tmove\t$22,$5\n"
    body_end = "\n\tlw\t$31,132($sp)\n"
    if text.count(body_start) != 1 or text.count(body_end) != 1:
        raise ValueError("formatted-output register body not found")
    head, rest = text.split(body_start, 1)
    body, tail = rest.split(body_end, 1)
    body = body_start + body
    saved_pair = re.compile(r"\$(18|19)\b")
    body = saved_pair.sub(
        lambda match: "$19" if match.group(1) == "18" else "$18", body
    )

    body = replace_once(
        body,
        "\tbne\t$16,$0,.L6\n",
        "\tbne\t$3,$0,.L6\n",
        "formatted-output signed literal zero test",
    )
    for accumulator in ("23", "17"):
        old = (
            f"\taddu\t$3,$3,${accumulator}\n"
            "\tsll\t$3,$3,1\n"
            "\tsll\t$2,$16,24\n"
            "\tsra\t$2,$2,24\n"
            "\taddu\t$2,$2,-48\n"
            f"\taddu\t${accumulator},$3,$2\n"
        )
        new = (
            f"\taddu\t$3,$3,${accumulator}\n"
            "\tsll\t$3,$3,1\n"
            "\taddu\t$3,$3,-48\n"
            "\tsll\t$2,$16,24\n"
            "\tsra\t$2,$2,24\n"
            f"\taddu\t${accumulator},$3,$2\n"
        )
        body = replace_once(
            body, old, new, "formatted-output decimal accumulation order"
        )

    body = replace_once(
        body,
        "\tbnel\t$2,$0,.L94\n"
        "\tori\t$20,$20,0x0100\n",
        "\tbeq\t$2,$0,.L94\n"
        "\tmove\t$7,$18\n"
        "\tori\t$20,$20,0x0100\n",
        "formatted-output exponent sign branch",
    )
    body = replace_once(
        body,
        "\tjal\tfunc_8003ADD0\n"
        "\tmove\t$4,$18\n",
        "\tjal\tfunc_8003ADD0\n"
        "\tmove\t$4,$7\n",
        "formatted-output exceptional-value destination",
    )
    body = replace_once(
        body,
        "\taddu\t$6,$6,-1\n"
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "\tjal\tfunc_800391BC\n"
        "\tmove\t$7,$18\n",
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "\tjal\tfunc_800391BC\n"
        "\taddu\t$6,$6,-1\n",
        "formatted-output exponent call scheduling",
    )
    return head + body + body_end + tail


def normalize_stream_channel_mixer(text: str) -> str:
    """Reproduce the reviewed register web and FP hazard schedule of the mixer."""
    text = replace_once(
        text,
        "\tmove\t$9,$6\n"
        "\tmove\t$8,$7\n"
        "\tsw\t$17,76($sp)\n"
        "\tmove\t$17,$0\n"
        "\tsw\t$22,96($sp)\n"
        "\tmove\t$22,$0\n"
        "\tsw\t$31,108($sp)\n"
        "\tsw\t$fp,104($sp)\n"
        "\tsw\t$23,100($sp)\n"
        "\tsw\t$21,92($sp)\n"
        "\tsw\t$20,88($sp)\n"
        "\tsw\t$18,80($sp)\n"
        "\tsw\t$16,72($sp)\n",
        "\tmove\t$8,$7\n"
        "\tmove\t$9,$6\n"
        "\tsw\t$22,96($sp)\n"
        "\tmove\t$22,$0\n"
        "\tsw\t$23,100($sp)\n"
        "\tmove\t$23,$0\n"
        "\tsw\t$31,108($sp)\n"
        "\tsw\t$fp,104($sp)\n"
        "\tsw\t$21,92($sp)\n"
        "\tsw\t$20,88($sp)\n"
        "\tsw\t$18,80($sp)\n"
        "\tsw\t$17,76($sp)\n"
        "\tsw\t$16,72($sp)\n",
        "stream-mixer entry register web",
    )
    text = replace_once(
        text,
        "\tli\t$20,0x0000ffff\t\t# 65535\n"
        "\tli\t$18,0x00000003\t\t# 3\n"
        "\tmove\t$16,$0\n",
        "\tli\t$18,0x0000ffff\t\t# 65535\n"
        "\tli\t$17,0x00000003\t\t# 3\n"
        "\tmove\t$16,$0\n",
        "stream-mixer scan constants",
    )
    scan_start = text.index(".L5:\n")
    scan_end = text.index(".L9:\n", scan_start)
    scan = text[scan_start:scan_end]
    for old, temporary in (("$20", "$30"), ("$18", "$26"),
                           ("$17", "$27"), ("$22", "$28")):
        scan = scan.replace(old, temporary)
    for temporary, new in (("$30", "$18"), ("$26", "$17"),
                           ("$27", "$22"), ("$28", "$23")):
        scan = scan.replace(temporary, new)
    text = text[:scan_start] + scan + text[scan_end:]
    inner_start = text.index(".L13:\n")
    inner_end = text.index(".L15:\n", inner_start)
    inner = text[inner_start:inner_end]
    inner = inner.replace("$22", "$30").replace("$23", "$22").replace("$30", "$23")
    inner = inner.replace("$11", "$30").replace("$12", "$11").replace("$30", "$12")
    inner = inner.replace("\tsw\t$12,60($sp)\n", "\tsw\t$11,60($sp)\n")
    inner = inner.replace("\tsw\t$11,64($sp)\n", "\tsw\t$12,64($sp)\n")
    inner = inner.replace("\tlw\t$11,64($sp)\n", "\tlw\t$11,60($sp)\n")
    inner = inner.replace("\tlw\t$12,60($sp)\n", "\tlw\t$12,64($sp)\n")
    text = text[:inner_start] + inner + text[inner_end:]
    outer_start = text.index(".L9:\n")
    outer_end = text.index(".L13:\n", outer_start)
    outer = text[outer_start:outer_end]
    outer = outer.replace("$11", "$30").replace("$12", "$11").replace("$30", "$12")
    text = text[:outer_start] + outer + text[outer_end:]
    text = replace_once(
        text,
        "\tli\t$11,0x0c000000\t\t# 201326592\n",
        "",
        "stream-mixer deferred mix opcode",
    )
    text = replace_once(
        text,
        "\t.set\tmacro\n\t.set\treorder\n\n\tmove\t$21,$0\n",
        "\t.set\tmacro\n\t.set\treorder\n\n"
        "\tli\t$11,0x0c000000\t\t# 201326592\n\tmove\t$21,$0\n",
        "stream-mixer mix opcode placement",
    )
    text = replace_once(
        text,
        "\tlw\t$3,4564($6)\n"
        "\tlw\t$2,4596($6)\n"
        "\tmtc1\t$fp,$f0\n"
        "\tcvt.s.w\t$f0,$f0\n"
        "\tl.s\t$f2,4592($6)\n"
        "\taddu\t$3,$3,$2\n"
        "\tli\t$2,-858993459\t\t\t# 0xcccccccd\n"
        "\tmultu\t$3,$2\n"
        "\tmul.s\t$f2,$f2,$f0\n"
        "\tl.s\t$f0,4588($6)\n"
        "\t#nop\n"
        "\tadd.s\t$f0,$f0,$f2\n",
        "\t.set\tnoreorder\n"
        "\tlw\t$3,4564($6)\n"
        "\tlw\t$2,4596($6)\n"
        "\tl.s\t$f2,4592($6)\n"
        "\tmtc1\t$fp,$f0\n"
        "\tnop\n"
        "\tcvt.s.w\t$f0,$f0\n"
        "\taddu\t$3,$3,$2\n"
        "\tmul.s\t$f2,$f2,$f0\n"
        "\tli\t$2,-858993459\t\t\t# 0xcccccccd\n"
        "\tmultu\t$3,$2\n"
        "\tl.s\t$f0,4588($6)\n"
        "\tadd.s\t$f0,$f0,$f2\n"
        "\t.set\treorder\n",
        "stream-mixer pitch calculation schedule",
    )
    wrap_start = text.index("\tlw\t$3,4576($6)\n", text.index(".L17:\n"))
    wrap_end = text.index(".L20:\n", wrap_start)
    wrap = (
        "\t.set\tnoreorder\n"
        "\taddu\t$16,$16,8\n"
        "\taddu\t$17,$17,16\n"
        "\tlw\t$3,4576($6)\n"
        "\taddu\t$19,$19,16\n"
        "\tsll\t$2,$5,1\n"
        "\tli\t$13,-8\t\t\t# 0xfffffff8\n"
        "\taddu\t$3,$3,$2\n"
        "\tsubu\t$4,$3,$10\n"
        "\tandi\t$20,$4,0x0007\n"
        "\tsubu\t$4,$4,$20\n"
        "\tli\t$3,0x00000a00\t\t# 2560\n"
        "\tsubu\t$3,$3,$5\n"
        "\tsll\t$3,$3,1\n"
        "\taddu\t$3,$3,$20\n"
        "\taddu\t$3,$3,7\n"
        "\tandi\t$2,$3,0x0ff8\n"
        "\tsll\t$2,$2,12\n"
        "\tor\t$2,$2,$12\n"
        "\tsw\t$2,-4($16)\n"
        "\tsw\t$4,0($16)\n"
        "\taddu\t$16,$16,8\n"
        "\tand\t$3,$3,$13\n"
        "\taddu\t$3,$3,368\n"
        "\tlw\t$4,4576($6)\n"
        "\tsll\t$2,$7,1\n"
        "\taddu\t$2,$2,-5105\n"
        "\tandi\t$2,$2,0x0ff8\n"
        "\tsll\t$2,$2,12\n"
        "\tandi\t$3,$3,0x0ff8\n"
        "\tli\t$13,0x04000000\t\t# 67108864\n"
        "\tor\t$3,$3,$13\n"
        "\tor\t$2,$2,$3\n"
        "\tsw\t$2,-4($16)\n"
        "\tj\t.L21\n"
        "\tsubu\t$4,$4,$10\n"
        "\t.set\treorder\n\n"
    )
    text = text[:wrap_start] + wrap + text[wrap_end:]
    text = replace_once(
        text,
        ".L21:\n\tlw\t$2,D_800C4300\n\tli.s\t$f0,3.27680000000000000000e4\n"
        "\tsw\t$4,0($16)\n",
        ".L21:\n\tsw\t$4,0($16)\n\tlw\t$2,D_800C4300\n"
        "\tli.s\t$f0,3.27680000000000000000e4\n",
        "stream-mixer shared address store schedule",
    )
    # The original assembler scheduled the integer HI result between the
    # float conversion's mfc1 and mtc1, avoiding the emitted hazard nop.
    text = replace_once(
        text,
        "\tmfhi\t$13\n\t#nop\n\t#nop\n\tsrl\t$5,$13,11\n"
        "\ttrunc.w.s $f2,$f0,$2\n\tmfc1\t$22,$f2\n\t#nop\n\tmtc1\t$22,$f2\n",
        "\ttrunc.w.s $f2,$f0,$2\n\tmfc1\t$22,$f2\n"
        "\tmfhi\t$13\n\tsrl\t$5,$13,11\n\tmtc1\t$22,$f2\n",
        "stream-mixer float conversion schedule",
    )
    return text


def normalize(text: str, unit: str) -> str:
    if unit == "func_800398E0":
        # This unit uses the original assembler's reorder pass.  Its reviewed
        # transformations preserve that input mode instead of applying the
        # builtin no-reorder scheduler used by the units below.
        return normalize_formatted_output_core(text)
    elif unit == "func_8004381C":
        return normalize_stream_channel_mixer(text)
    elif unit == "func_800196E8":
        text = normalize_state_copy(text)
    elif unit == "func_80006E88":
        text = normalize_d1_func_80006E88(text)
    elif unit == "func_8003ACF0":
        text = normalize_c09_func_8003ACF0(text)
    elif unit == "func_80027978":
        text = normalize_c06_func_80027978(text)
    elif unit == "func_8001D0F0":
        text = normalize_guarded_callback(text)
    elif unit == "func_80007FF8":
        text = normalize_find_record_pair(text)
    elif unit == "func_80010EF8":
        text = normalize_check_pak_channel(text)
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
    elif unit == "func_8002E478":
        text = normalize_game_object_timer(text)
    elif unit == "func_8001AB7C":
        text = normalize_entry_copy_selection(text)
    elif unit == "func_80024328":
        text = normalize_grid_direction_probe(text)
    elif unit == "func_80019740":
        text = normalize_known_identifier_test(text)
    elif unit == "func_80016AA8":
        text = normalize_counted_byte_update(text)
    elif unit == "func_80031250":
        text = normalize_scene_shutdown(text)
    elif unit == "func_80006E30":
        text = normalize_actor_mode_setup(text)
    elif unit == "func_80005610":
        text = normalize_grid_actor_lookup(text)
    elif unit == "func_8003099C":
        text = normalize_scene_handle_call(text)
    elif unit == "func_80030B30":
        text = normalize_conditional_state_store(text)
    elif unit == "func_800309C4":
        text = normalize_sequence_state_store(text)
    elif unit == "func_8002EBC4":
        text = normalize_runtime_reset_store(text)
    elif unit == "func_8000711C":
        text = normalize_mode_dispatch(text)
    elif unit == "func_80027A1C":
        text = normalize_idle_thread(text)
    elif unit == "func_80006534":
        text = normalize_actor_position(text)
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
