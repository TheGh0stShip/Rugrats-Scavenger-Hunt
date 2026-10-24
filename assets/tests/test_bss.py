import os, subprocess, sys, tempfile, unittest
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "tools"))
import bss


class TestBss(unittest.TestCase):
    def setUp(self):
        self.rows = bss.load_tsv()

    def test_coverage(self):
        s = bss.validate(self.rows)
        self.assertTrue(s["ok"], s["errors"])
        self.assertEqual(s["covered_bytes"], bss.BSS_END - bss.BSS_START)
        self.assertEqual(s["referenced_bytes"] + s["gap_bytes"], s["covered_bytes"])

    def test_detects_overlap_and_gap(self):
        r = [dict(x) for x in self.rows]
        r[1]["vram_start"] -= 1; r[1]["size"] += 1
        self.assertFalse(bss.validate(r)["ok"])
        r = [dict(x) for x in self.rows]
        del r[1]
        self.assertFalse(bss.validate(r)["ok"])

    def test_header_matches_tsv(self):
        h = open(os.path.join(bss.ROOT, "src", "bss", "bss.h")).read()
        for x in self.rows:
            self.assertIn(" %s" % x["symbol"], h)

    def test_rescan_is_stable(self):
        if not os.path.exists(bss.DISASM):
            self.skipTest("disassembly not present")
        new = bss.build()
        self.assertEqual([(x["vram_start"], x["symbol"], x["size"]) for x in new],
                         [(x["vram_start"], x["symbol"], x["size"]) for x in self.rows])

    def test_roundtrip_claims_no_rom(self):
        self.assertEqual(bss.roundtrip(b""), [])

    def test_kmc_link_addresses(self):
        tc = os.environ.get("N64DK_TOOLCHAIN", "../BattleTanx/.toolchain")
        gcc = os.path.join(tc, "kmc-gcc-2.7.2", "gcc")
        bin_ = os.path.join(tc, "mips-binutils", "usr", "bin")
        if not os.path.exists(gcc):
            self.skipTest("toolchain not present")
        env = dict(os.environ, LD_LIBRARY_PATH=os.path.join(tc, "mips-binutils", "usr", "lib", "x86_64-linux-gnu"))
        d = tempfile.mkdtemp()
        src = os.path.join(bss.ROOT, "src", "bss")
        subprocess.run([gcc, "-c", "-G0", "-mips3", "-mgp32", "-mfp32", "-O2", "-fno-common",
                        "-DBSS_STANDALONE", "-B" + os.path.join(tc, "kmc-gcc-2.7.2") + "/",
                        "-o", os.path.join(d, "bss.o"), os.path.join(src, "bss.c")],
                       check=True, env=env, capture_output=True)
        subprocess.run([os.path.join(bin_, "mips-linux-gnu-ld"), "-T", os.path.join(src, "bss.ld"),
                        "-o", os.path.join(d, "bss.elf"), os.path.join(d, "bss.o")], check=True, env=env)
        out = subprocess.run([os.path.join(bin_, "mips-linux-gnu-nm"), "-n", os.path.join(d, "bss.elf")],
                             check=True, env=env, capture_output=True, text=True).stdout
        nm = {l.split()[2]: int(l.split()[0], 16) for l in out.splitlines() if len(l.split()) == 3}
        main = [r for r in self.rows if r["kind"] != "alias"]
        for r in main:
            self.assertEqual(nm.get(r["symbol"]), r["vram_start"], r["symbol"])
        self.assertEqual(nm["_bss_start"], bss.BSS_START)
        self.assertEqual(nm["_bss_end"], bss.BSS_END)

    def test_clear_sizes_applied(self):
        n = [r for r in self.rows if "size=memset" in r["evidence"] or "size=bzero" in r["evidence"]]
        self.assertGreater(len(n), 0)
        for r in n:
            m = __import__("re").search(r"\((0x[0-9A-F]+)\)", r["evidence"])
            self.assertEqual(r["size"], int(m.group(1), 16))

    def test_aliases_inside_parent(self):
        main = [r for r in self.rows if r["kind"] != "alias"]
        for a in (r for r in self.rows if r["kind"] == "alias"):
            self.assertTrue(any(m["vram_start"] <= a["vram_start"] and a["vram_end"] <= m["vram_end"] for m in main))


if __name__ == "__main__":
    unittest.main()
