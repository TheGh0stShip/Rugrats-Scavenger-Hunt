import os
import sys
import unittest

ROOT = os.path.join(os.path.dirname(__file__), "..")
sys.path.insert(0, os.path.join(ROOT, "tools"))
import audio_songs as A  # noqa: E402

PAY = A.load_archive2(ROOT)
if len(PAY) != 25:
    # no local extraction: read the song records straight from the ROM
    import archive  # noqa: E402
    import rlz  # noqa: E402
    _rom_path = os.environ.get("ROM", "baseroms/us/baserom.z64")
    if os.path.exists(_rom_path):
        _rom = open(_rom_path, "rb").read()
        _s, _e = archive.ARCHIVES["resource_archive_2"]
        PAY = {}
        for _r in archive.parse(_rom, _s, _e)["records"]:
            _p = (rlz.decode(_rom, _r["rom_start"])[0] if _r["codec"] == "rlz"
                  else _rom[_r["rom_start"]:_r["rom_end"]])
            if _p[:4] == bytes.fromhex("00000215"):
                PAY[_r["index"]] = _p


@unittest.skipUnless(len(PAY) == 25, "archive 2 song payloads not extracted")
class SongTest(unittest.TestCase):
    def test_all_songs_rebuild_exactly(self):
        rows = A.songs_roundtrip(PAY)
        self.assertEqual(len(rows), 25)
        self.assertTrue(all(r["verified"] for r in rows), [r for r in rows if not r["verified"]])

    def test_streams_terminate_and_indices_valid(self):
        for i, d in PAY.items():
            s = A.parse_song(d)
            sm = A.song_summary(s)
            for off, (kind, _) in s["items"].items():
                if kind in ("channel", "master"):
                    self.assertIn(s["body"][off][-1][1], (0x80, 0x96), (i, hex(off)))
                    st = A.stream_stats(s["body"][off])
                    self.assertTrue(all(e < sm["envelopes"] for e in st["env_cmds"]))
                    self.assertTrue(all(w < len(s["waves"]) for w in st["wave_cmds"]))
            self.assertTrue(max(s["waves"]) < 100)

    def test_rebuild_relocates(self):
        d = PAY[511]
        s = A.parse_song(d)
        s["body"][(0, "pad")] = s["body"][(0, "pad")] + b"\0" * 4
        built = A.build_song(s)
        self.assertEqual(len(built), len(d) + 4)
        moved = A.parse_song(built)
        self.assertEqual(A.song_summary(moved)["events"], A.song_summary(A.parse_song(d))["events"])

    def test_token_codec(self):
        d = bytes([0x81, 0x83, 0x05, 0x30, 0x7C, 0x10, 0x8B, 0x08, 0x31, 0x60, 0x80])
        toks = A.tokenize(d, 0, len(d), True)
        self.assertEqual(A.emit(toks), d)
        self.assertEqual(toks[0][2][0], (0x305, True))


if __name__ == "__main__":
    unittest.main()
