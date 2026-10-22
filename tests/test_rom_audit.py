from __future__ import annotations

import importlib.util
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "audit_rom", ROOT / "tools/audit_rom.py"
)
AUDIT = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
SPEC.loader.exec_module(AUDIT)


class RomAuditTests(unittest.TestCase):
    def test_signature_census_includes_overlapping_occurrences(self) -> None:
        self.assertEqual(AUDIT.find_all(b"aaaa", b"aa"), [0, 1, 2])

    def test_declared_ranges_cover_rom(self) -> None:
        ranges = (
            (0, AUDIT.PROGRAM_START),
            (AUDIT.PROGRAM_START, AUDIT.PROGRAM_END),
            (AUDIT.PROGRAM_END, 0x21AC00),
            (0x21AC00, 0x4DB6D0),
            (0x4DB6D0, 0xF4D840),
            (0xF4D840, 0xF4D850),
            (0xF4D850, AUDIT.ROM_SIZE),
        )
        self.assertEqual(ranges[0][0], 0)
        self.assertEqual(ranges[-1][1], AUDIT.ROM_SIZE)
        self.assertTrue(all(left[1] == right[0] for left, right in zip(ranges, ranges[1:])))

    def test_program_sections_cover_initial_image(self) -> None:
        self.assertEqual(sum(end - start for _, start, end in AUDIT.PROGRAM_SECTIONS), 0x56E80)
        self.assertEqual(
            sum(end - start for kind, start, end in AUDIT.PROGRAM_SECTIONS if kind == "text"),
            0x4EEC0,
        )

    def test_physical_ledger_is_gap_free(self) -> None:
        report = {
            "program_sections": [
                {"kind": kind, "start": start, "end": end}
                for kind, start, end in AUDIT.PROGRAM_SECTIONS
            ],
            "audio_graphics_ranges": [
                {"kind": kind, "start": start, "end": end}
                for kind, start, end in AUDIT.AUDIO_GRAPHICS_RANGES
            ],
            "audio_banks": [
                {
                    "name": name,
                    "pointer_start": pointer_start,
                    "wave_start": wave_start,
                    "wave_end": wave_end,
                    "samples": [],
                }
                for name, pointer_start, wave_start, wave_end in AUDIT.AUDIO_BANKS
            ],
            "archives": [
                {
                    "name": name,
                    "start": start,
                    "end": end,
                    "index_bytes": end - start,
                    "records": [],
                }
                for name, start, end in AUDIT.ARCHIVES
            ],
        }
        rows = AUDIT.physical_ledger(report)
        self.assertEqual(rows[0]["start"], 0)
        self.assertEqual(rows[-1]["end"], AUDIT.ROM_SIZE)
        self.assertTrue(
            all(left["end"] == right["start"] for left, right in zip(rows, rows[1:]))
        )
        self.assertEqual(sum(row["size"] for row in rows), AUDIT.ROM_SIZE)


if __name__ == "__main__":
    unittest.main()
