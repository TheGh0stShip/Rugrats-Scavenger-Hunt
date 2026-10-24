import os, sys, unittest
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'tools'))
import datasrc

ROM = datasrc.ROM_PATH


@unittest.skipUnless(os.path.exists(ROM), 'base ROM missing')
class InitializedDataTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.rom = open(ROM, 'rb').read()

    def test_roundtrip_covers_all_bytes(self):
        rows = sorted(datasrc.roundtrip(self.rom), key=lambda r: r['start'])
        covered = sum(r['end'] - r['start'] for r in rows)
        has_asm = os.path.isdir(datasrc.ASM)
        self.assertEqual(covered, 32704 if has_asm else 32216)
        for a, b in zip(rows, rows[1:]):
            self.assertLessEqual(a['end'], b['start'])
        self.assertTrue(all(r['verified'] for r in rows))
        code = sum(r['end'] - r['start'] for r in rows if r['category'] == 'program_code')
        self.assertEqual(code, 488 if has_asm else 0)
        self.assertEqual(covered - code, 32216)
        ranges = [(a, b) for a, b, _ in datasrc.RANGES]
        for r in rows:
            self.assertTrue(any(a <= r['start'] and r['end'] <= b for a, b in ranges))

    def test_literal_codecs(self):
        for bs in [b'abc\0', b'a"b\\c?\n\t\x01\xff\0', b'??=\0']:
            self.assertEqual(datasrc.c_unescape(datasrc.c_escape(bs)), bs)
        self.assertEqual(datasrc.f32_literal(0x3FC00000), '1.5f')
        self.assertIsNone(datasrc.f32_literal(0x7FC00000))
        self.assertIsNone(datasrc.f64_literal(0x0000000000000001))

    def test_typed_objects(self):
        objs = [o for o, u, ok in datasrc.verify(self.rom)]
        vi = [o for o in objs if o.get('stype') == 'OSViMode']
        self.assertGreaterEqual(len(vi), 42)
        self.assertTrue(any(o['name'] == 'osViModeNtscLan1' for o in vi))
        self.assertEqual([o['size'] for o in objs if o['rep'] == 'ucode-data'], [0x800, 0x2B0])
        raw = sum(o['size'] for o in objs if o['rep'] == 'typed-raw')
        self.assertLess(raw, 1000)
        import json
        txt = open(os.path.join(datasrc.SRC, 'rodata_800004F8.json')).read()
        self.assertNotIn('unconfirmed', txt)
        for o in objs:
            if o['rep'] == 'typed-raw': self.assertTrue(o.get('reason'))

    def test_mismatch_detected(self):
        bad = bytearray(self.rom); bad[0x2000] ^= 0xFF
        rows = datasrc.roundtrip(bytes(bad))
        self.assertEqual(sum(1 for r in rows if not r['verified']), 1)

    @unittest.skipUnless(os.path.exists(datasrc.TOOLCHAIN + '/kmc-gcc-2.7.2/gcc'), 'toolchain missing')
    def test_compile_sample(self):
        units, _ = datasrc.load_units()
        res = datasrc.compile_units(self.rom, units[:3])
        self.assertTrue(all(v[0] for v in res.values()), res)


if __name__ == '__main__':
    unittest.main()
