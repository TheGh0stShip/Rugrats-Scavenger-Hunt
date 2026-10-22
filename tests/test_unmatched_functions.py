from __future__ import annotations

import importlib.util
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "list_unmatched_functions", ROOT / "tools/list_unmatched_functions.py"
)
QUEUE = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
SPEC.loader.exec_module(QUEUE)


class UnmatchedFunctionTests(unittest.TestCase):
    def test_manifest_rom_offsets_map_to_vram(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            manifest = Path(directory) / "units.tsv"
            manifest.write_text("0x1000\t0x8\tunit.c\t-O2\tdeadbeef\n")
            self.assertEqual(QUEUE.load_matched_vrams((manifest,)), {0x80000400})

    def test_queue_is_size_then_address_ordered(self) -> None:
        functions = [
            QUEUE.Function("large", 0x80001000, 0x20, "c"),
            QUEUE.Function("later", 0x80003000, 0x8, "stub"),
            QUEUE.Function("earlier", 0x80002000, 0x8, "stub"),
        ]
        result = QUEUE.unmatched_functions(functions, {0x80003000})
        self.assertEqual([item.name for item in result], ["earlier", "large"])


if __name__ == "__main__":
    unittest.main()
