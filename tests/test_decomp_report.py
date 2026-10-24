from __future__ import annotations

import importlib.util
import json
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
        self.assertEqual(totals["matched_code"], "114284")
        self.assertEqual(totals["total_data"], "32704")
        self.assertEqual(totals["matched_data"], "640")

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
        self.assertEqual(categories["program_data"]["matched_data"], "640")
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

    def test_public_tracker_metadata_matches_report(self) -> None:
        report = REPORT.build_report()
        totals = report["measures"]
        readme = (ROOT / "README.md").read_text()
        manifest_text = (ROOT / ".recomp.json").read_text()
        manifest = json.loads(manifest_text)
        workflow = (ROOT / ".github/workflows/progress.yml").read_text()

        code_summary = (
            f"{totals['matched_functions']:,} / {totals['total_functions']:,} functions; "
            f"{int(totals['matched_code']):,} / {int(totals['total_code']):,} code bytes"
        )
        data_summary = (
            f"{int(totals['matched_data']):,} / "
            f"{int(totals['total_data']):,} bytes"
        )
        self.assertIn(code_summary, readme)
        self.assertIn(data_summary, readme)
        self.assertIn("NRGE_report", workflow)
        self.assertIn("us_report", workflow)
        self.assertIn("https://api.decmp.dev/action/v1.1.0.mjs", workflow)
        self.assertIn("DECMP_VERSION: NRGE", workflow)
        self.assertEqual(manifest["original"]["serial"], "NRGE")
        self.assertEqual(manifest["original"]["sha1"],
                         "fb2a62f1625630d6f0beb5fd00a32e12155d50e8")
        self.assertIn(f"{int(totals['matched_code']):,}", manifest["notes"])
        self.assertIn(f"{totals['matched_functions']:,}", manifest["notes"])


if __name__ == "__main__":
    unittest.main()
