import os
import sys
import unittest

import importlib.util

_P = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "tools", "platform.py")
_spec = importlib.util.spec_from_file_location("rom_tools_platform", _P)
plat = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(plat)

ROM = os.environ.get("ROM", "baseroms/us/baserom.z64")


@unittest.skipUnless(os.path.exists(ROM), "base ROM not available")
class PlatformTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.rom = open(ROM, "rb").read()

    def test_checksum(self):
        h = plat.parse_header(self.rom)
        self.assertEqual(plat.cic6102_checksum(self.rom), (h["crc1"], h["crc2"]))

    def test_checksum_detects_change(self):
        r = bytearray(self.rom)
        r[0x2000] ^= 1
        h = plat.parse_header(self.rom)
        self.assertNotEqual(plat.cic6102_checksum(bytes(r)), (h["crc1"], h["crc2"]))

    def test_header_rebuild(self):
        self.assertEqual(plat.build_header(plat.parse_header(self.rom), self.rom), self.rom[:0x40])

    def test_ipl3(self):
        self.assertEqual(plat.identify_ipl3(self.rom)["cic"], "6102")

    def test_dlblock(self):
        p = plat.parse_dlblock(self.rom)
        self.assertEqual(plat.build_dlblock(p), self.rom[0x57E80:0x57FD0])
        lr = plat.loader_record(self.rom)
        self.assertEqual((lr["rom_start"], lr["rom_end"], lr["ram_dst"]), (0x57E80, 0x57FD0, 0x800C7E30))

    def test_rows(self):
        rows = plat.roundtrip(self.rom)
        self.assertTrue(all(r["verified"] for r in rows))
        self.assertEqual(sum(r["end"] - r["start"] for r in rows), 0xB3910)


if __name__ == "__main__":
    unittest.main()
