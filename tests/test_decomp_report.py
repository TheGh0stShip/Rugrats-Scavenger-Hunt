from __future__ import annotations

import importlib.util
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "generate_decomp_report", ROOT / "tools/generate_decomp_report.py"
)
REPORT = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
SPEC.loader.exec_module(REPORT)


class DecompReportTests(unittest.TestCase):
    def test_aggregate_totals_exclude_unclassified_rom(self) -> None:
        report = REPORT.build_report()
        totals = report["measures"]

        self.assertEqual(totals["total_code"], "323264")
        self.assertEqual(totals["matched_code"], "27008")
        self.assertEqual(totals["total_data"], "32704")
        self.assertEqual(totals["matched_data"], "240")

        categories = {
            category["id"]: category["measures"]
            for category in report["categories"]
        }
        self.assertEqual(
            set(categories),
            {
                "code",
                "program_data",
                "assets_extracted",
                "asset_inventory",
                "audio_inventory",
                "resource_inventory",
                "rom_inventory",
            },
        )
        self.assertEqual(categories["program_data"]["total_code"], "32704")
        self.assertEqual(categories["program_data"]["total_data"], "32704")
        self.assertEqual(categories["program_data"]["matched_data"], "240")
        self.assertEqual(categories["assets_extracted"]["matched_data"], "0")
        self.assertEqual(categories["asset_inventory"]["matched_data"], "15686080")
        self.assertEqual(categories["rom_inventory"]["matched_data"], "16777216")

    def test_inventory_subcategories_are_ledger_derived(self) -> None:
        report = REPORT.build_report()
        categories = {
            category["id"]: category["measures"]
            for category in report["categories"]
        }
        self.assertEqual(categories["audio_inventory"]["matched_data"], "1846320")
        self.assertEqual(categories["resource_inventory"]["matched_data"], "13839424")


if __name__ == "__main__":
    unittest.main()
