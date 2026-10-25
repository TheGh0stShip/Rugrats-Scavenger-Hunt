from __future__ import annotations

import csv
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


class ExecutableInventoryTests(unittest.TestCase):
    def test_nonfunction_intervals_cover_the_exact_remainder(self) -> None:
        with (ROOT / "analysis/executable_nonfunction_inventory.tsv").open() as stream:
            rows = list(csv.DictReader(stream, delimiter="\t"))
        self.assertEqual(sum(int(row["size"], 0) for row in rows), 9448)
        self.assertEqual(len(rows), 63)
        self.assertEqual(
            sum(int(row["size"], 0) for row in rows
                if row["classification"] == "f3dex_1_23_text"),
            5168,
        )
        self.assertEqual(
            sum(int(row["size"], 0) for row in rows
                if row["classification"] == "rsp_boot_text"),
            208,
        )
        self.assertTrue(all(len(row["sha256"]) == 64 for row in rows))


if __name__ == "__main__":
    unittest.main()
