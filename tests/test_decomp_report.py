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
        self.assertEqual(totals["matched_code"], "18736")
        self.assertEqual(totals["total_data"], "32704")
        self.assertEqual(totals["matched_data"], "240")

        categories = {
            category["id"]: category["measures"]
            for category in report["categories"]
        }
        self.assertEqual(set(categories), {"code", "program_data"})
        self.assertEqual(categories["program_data"]["total_code"], "0")
        self.assertEqual(categories["program_data"]["total_data"], "32704")


if __name__ == "__main__":
    unittest.main()
