import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "tools"))
import audio  # noqa: E402

ROM = os.environ.get("ROM", "baseroms/us/baserom.z64")


@unittest.skipUnless(os.path.exists(ROM), "base ROM not present")
class AudioTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.rom = open(ROM, "rb").read()

    def test_roundtrip_rows_cover_area_exactly(self):
        rows = audio.roundtrip(self.rom)
        self.assertEqual(rows[0]["start"], audio.AREA[0])
        self.assertEqual(rows[-1]["end"], audio.AREA[1])
        for a, b in zip(rows, rows[1:]):
            self.assertEqual(a["end"], b["start"])
        self.assertTrue(all(r["verified"] for r in rows))

    def test_full_area_rebuild(self):
        self.assertEqual(audio.rebuild_area(self.rom), self.rom[audio.AREA[0]:audio.AREA[1]])

    def test_pointer_banks_reserialize(self):
        for s, e in (audio.PTR1, audio.PTR2):
            bank = audio.parse_ptrbank(self.rom[s:e])
            self.assertEqual(audio.build_ptrbank(bank), self.rom[s:e])

    def test_pointer_rebuild_recomputes_pointers(self):
        s, e = audio.PTR1
        bank = audio.parse_ptrbank(self.rom[s:e])
        bank.layout.insert(1, ("pad", b"\0" * 8))   # shift everything after the header
        moved = audio.build_ptrbank(bank)
        again = audio.parse_ptrbank(moved)
        self.assertEqual([w.base for w in again.waves], [w.base for w in bank.waves])
        self.assertEqual(again.books[0].coefs, bank.books[0].coefs)

    def test_sample_count(self):
        sm = audio.samples(self.rom)
        self.assertEqual(len(sm), 178)
        self.assertEqual(sum(1 for s in sm if s["bank"] == "bank1"), 78)
        self.assertEqual(sum(s["rom_end"] - s["rom_start"] for s in sm), 1807650)
        self.assertTrue(all(s["verified"] for s in sm))

    def test_fx_bank(self):
        fx = audio.parse_fx(self.rom[audio.FX1[0]:audio.FX1[1]])
        self.assertEqual(fx.count, 112)
        self.assertEqual(len(fx.wave_map), 74)
        self.assertEqual(audio.build_fx(fx), self.rom[audio.FX1[0]:audio.FX1[1]])

    def test_vadpcm_frame_reencode_contains_original(self):
        sm = audio.samples(self.rom)[0]
        pcm = audio.decode_vadpcm(sm["_data"], sm["_book"])
        for f in range(0, 40):
            st = pcm[16 * f - 8:16 * f] if f else [0] * 8
            self.assertIn(sm["_data"][9 * f:9 * f + 9], audio.reencode_frame(sm["_book"], st, pcm[16 * f:16 * f + 16]))


@unittest.skipUnless(os.path.exists(ROM), "base ROM not present")
class FxTest(unittest.TestCase):
    def test_fx_decoded_commands(self):
        rom = open(ROM, "rb").read()
        fx = audio.parse_fx(rom[audio.FX1[0]:audio.FX1[1]])
        self.assertTrue(all(t[-1][1] == 0x80 for t in fx.streams))
        self.assertEqual([i for i, g in enumerate(fx.gaps) if g], [111])
        slots = {t[2][0][0] for s in fx.streams for t in s if t[0] == "cmd" and t[1] == 0x81}
        self.assertEqual(slots, set(range(74)))


if __name__ == "__main__":
    unittest.main()
