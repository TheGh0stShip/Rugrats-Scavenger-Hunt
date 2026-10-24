"""Tests for the MORT speech stream parser/rebuilder. Needs the retail ROM (ROM env var)."""
import hashlib
import os
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "tools"))
import mort  # noqa: E402

SHA1 = "fb2a62f1625630d6f0beb5fd00a32e12155d50e8"


def load_rom():
    if not os.path.exists(mort.ROM_PATH):
        raise unittest.SkipTest("retail ROM not available")
    rom = open(mort.ROM_PATH, "rb").read()
    if hashlib.sha1(rom).hexdigest() != SHA1:
        raise unittest.SkipTest("ROM SHA-1 mismatch")
    return rom


class SyntheticTests(unittest.TestCase):
    def test_build_parse(self):
        fr = {"lar": [1, 2, 3, 4, 5, 6, 7, 0], "sub": [[50, 1, 2, 30] + [i % 8 for i in range(13)]] * 4}
        p = {"frames": 4, "rate": 16000, "runs": [["silence", 2], ["speech", 2]],
             "params": [fr, fr], "tail_bits": 0, "tail_value": 0}
        nbits = 96 + 5 + 8 + 2 * 260
        p["words_field"] = (nbits + 31) // 32
        p["tail_bits"] = p["words_field"] * 32 - nbits
        data = mort.build(p)
        q = mort.parse(data)
        self.assertEqual(q["params"], p["params"])
        self.assertEqual(q["runs"], p["runs"])
        self.assertEqual(mort.build(q), data)
        pcm = mort.decode(data)
        self.assertEqual(len(pcm), 4 * 160)
        self.assertEqual(pcm[:320], [0] * 320)


class RomTests(unittest.TestCase):
    def test_roundtrip_all(self):
        rows = mort.roundtrip(load_rom())
        self.assertEqual(len(rows), 380)
        self.assertTrue(all(r["verified"] for r in rows))
        self.assertEqual(sum(r["end"] - r["start"] for r in rows), 2879376)


if __name__ == "__main__":
    unittest.main()
