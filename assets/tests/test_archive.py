"""Tests for the resource codec and archive rebuilder. Needs the retail ROM (path in ROM env var)."""
import hashlib
import os
import random
import struct
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "tools"))
import archive  # noqa: E402
import rlz  # noqa: E402

ROM_PATH = os.environ.get("ROM", "baseroms/us/baserom.z64")
SHA1 = "fb2a62f1625630d6f0beb5fd00a32e12155d50e8"


def load_rom():
    if not os.path.exists(ROM_PATH):
        raise unittest.SkipTest("retail ROM not available")
    rom = open(ROM_PATH, "rb").read()
    if hashlib.sha1(rom).hexdigest() != SHA1:
        raise unittest.SkipTest("ROM SHA-1 mismatch")
    return rom


class CodecTests(unittest.TestCase):
    def test_tokens(self):
        # literal "ab", run of 5 'x', back-reference to "ab" (offset 7, length 2 is < 3 so it stays literal)
        stream = bytes([2]) + b"ab" + bytes([0x45]) + b"x" + bytes([0x80 | 0, (7 << 5) | 2]) + b"\0"
        out, end = rlz.decode(stream)
        self.assertEqual(out, b"abxxxxxab")
        self.assertEqual(end, len(stream))

    def test_encode_decode_random(self):
        rnd = random.Random(1)
        for size in (0, 1, 2, 3, 63, 64, 500, 5000):
            data = bytes(rnd.choice(b"aab\0\0\0xyz") for _ in range(size))
            enc = rlz.encode(data)
            self.assertEqual(rlz.decode(enc)[0], data)

    def test_retail_streams_reencode_exactly(self):
        rom = load_rom()
        start, end = archive.ARCHIVES["resource_archive_2"]
        meta = archive.parse(rom, start, end)
        compressed = [r for r in meta["records"] if r["codec"] == "rlz"]
        self.assertEqual(len(compressed), 148)
        for r in compressed:
            raw, stop = rlz.decode(rom, r["rom_start"])
            self.assertEqual(len(raw), r["logical_size"])
            self.assertEqual(rlz.encode(raw), rom[r["rom_start"]:stop], f"record {r['index']}")


class ArchiveTests(unittest.TestCase):
    def test_full_archive_roundtrip(self):
        rom = load_rom()
        with tempfile.TemporaryDirectory() as tmp:
            for name, (start, end) in archive.ARCHIVES.items():
                meta = archive.extract(rom, name, tmp)
                rebuilt, report = archive.build(meta, os.path.join(tmp, name))
                self.assertTrue(all(r["match"] for r in report), name)
                self.assertEqual(rebuilt, rom[start:end], name)

    def test_index_counts(self):
        rom = load_rom()
        counts = {n: struct.unpack_from(">I", rom, s)[0] for n, (s, e) in archive.ARCHIVES.items()}
        self.assertEqual(counts, {"resource_archive_1": 380, "resource_archive_2": 531})


if __name__ == "__main__":
    unittest.main()
