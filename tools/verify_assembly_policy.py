#!/usr/bin/env python3
"""Keep ordinary game assembly visible and prevent the fallback list growing."""

from __future__ import annotations

import argparse
from pathlib import Path


AUDITED_GAME_ASSEMBLY = {
    (0x9D94, 0x130, "src/game/find_record_state_80009194.S"),
    (0x13BB0, 0x18C, "src/game/initialize_resource_tables_80012fb0.S"),
    (0x14C84, 0x188, "src/game/patch_display_commands_80014084.S"),
    (0x1C1FC, 0x1B4, "src/game/prepare_player_menu_8001b5fc.S"),
    (0x1FDC4, 0x148, "src/game/start_rotating_stream_8001f1c4.S"),
    (0x20450, 0x1A4, "src/game/emit_scaled_textured_rectangle_8001f850.S"),
    (0x20640, 0x144, "src/game/emit_textured_rectangle_8001fa40.S"),
    (0x2C034, 0xFC, "src/game/prepare_cursor_motion_8002b434.S"),
    (0x2C130, 0x1B0, "src/game/prepare_menu_transition_8002b530.S"),
    (0x2F078, 0xCC, "src/game/update_object_timer_8002e478.S"),
    (0x58BC, 0x534, "src/game/debug_stub_exception_handler.S"),
}


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "manifest", type=Path, nargs="?",
        default=Path("config/us/matching_asm_units.tsv"),
    )
    args = parser.parse_args()

    actual: set[tuple[int, int, str]] = set()
    for number, line in enumerate(args.manifest.read_text().splitlines(), 1):
        if not line or line.startswith("#"):
            continue
        fields = line.split("\t")
        if len(fields) != 5:
            raise SystemExit(f"{args.manifest}:{number}: expected five fields")
        row = (int(fields[0], 0), int(fields[1], 0), fields[2])
        if fields[2].startswith("src/game/"):
            actual.add(row)

    unexpected = actual - AUDITED_GAME_ASSEMBLY
    if unexpected:
        details = "\n".join(
            f"  0x{start:X} 0x{size:X} {source}"
            for start, size, source in sorted(unexpected)
        )
        raise SystemExit(f"unexpected game assembly unit(s):\n{details}")

    print(
        f"assembly policy verified: {len(actual)} audited game unit(s); "
        f"the allowlist has {len(AUDITED_GAME_ASSEMBLY) - len(actual)} retired slot(s)"
    )


if __name__ == "__main__":
    main()
