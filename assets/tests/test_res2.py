import os
import sys
import unittest

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import res2  # noqa: E402

ROM = os.environ.get("ROM", "baseroms/us/baserom.z64")
# a few records per family (full run: python3 tools/res2.py)
SAMPLE = {0, 1, 2, 10, 13, 31, 49, 68, 100, 138, 226, 300, 345, 421, 504, 530}


@unittest.skipUnless(os.path.exists(ROM), "ROM not present")
class Res2Test(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.rom = open(ROM, "rb").read()
        cls.res = {r["index"]: r for r in res2.analyse(cls.rom, only=SAMPLE)}

    def test_families(self):
        fam = {i: r["family"] for i, r in self.res.items()}
        self.assertEqual(fam[2], "imagebank")
        self.assertEqual(fam[13], "imagebank")
        self.assertEqual(fam[31], "mdlh")
        self.assertEqual(fam[226], "gridmap")
        self.assertEqual(fam[421], "pairtable")
        self.assertEqual(self.res[421]["proven"], "camera_path")
        self.assertEqual(fam[0], "font")
        self.assertEqual(fam[1], "font")
        self.assertEqual(fam[504], "libmus_song")
        self.assertEqual(fam[530], "empty")

    def test_exact(self):
        for i, r in self.res.items():
            self.assertTrue(r["payload_match"] and r["stored_match"], i)

    def test_png_codec(self):
        rep = self.res[13]["rep"]
        t = rep["tiles"][0]
        png = res2.png_indexed(t["w"], t["h"], t["pixels"], rep["palette"])
        w, h, idx, pal = res2.png_read_indexed(png)
        self.assertEqual((w, h, idx, pal), (48, 24, t["pixels"], rep["palette"]))

    def test_model(self):
        m = self.res[31]["rep"]["model"]
        self.assertEqual(m["vtx_count"], 0x6A)
        self.assertEqual(m["vtx_end"], m["vtx_off"] + 16 * m["vtx_count"])


    def test_animation_table(self):
        rep = self.res[100]["rep"]
        ents = [e for _k, e in rep["anim_table"] if e is not None]
        self.assertEqual(len(ents), len(rep["streams"]))
        for (off, sl, ds, ticks), (q, data, n), a in zip(ents, rep["streams"], rep["anims"]):
            self.assertEqual((q - rep["hdr"][4], n, len(data), ticks), (off, sl, ds, a["ticks"]))

    def test_vertex_split(self):
        rep = self.res[100]["rep"]
        self.assertEqual(sum(a + len(b) for a, b in rep["runs"]), rep["model"]["vtx_count"])
        for a in rep["anims"]:
            self.assertEqual(len(a["vertices"]), rep["anim_vertices"])

    def test_font(self):
        rep = self.res[0]["rep"]
        self.assertEqual((rep["row_bytes"], rep["height"], len(rep["strips"])), (136, 15, 8))
        self.assertEqual(sum(1 for g in rep["glyphs"] if g[5] >= 0), 94)

    def test_patch_slot_walk(self):
        w = self.res[345]["walk"]
        if w is not None:
            self.assertTrue(w.ok)


if __name__ == "__main__":
    unittest.main()
