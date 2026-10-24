#!/usr/bin/env python3
"""MORT speech streams (resource archive 1, all 380 records).

Container (big-endian):
  0x00 char[4]  'MORT'
  0x04 u16      frame count (decoder calls; one call = 160 output samples)
  0x06 u16      sample rate in Hz (0x3E80 = 16000 in every record)
  0x08 u32      total size in 32-bit words, header included (runtime masks with 0x00FFFFFF)
  0x0C ...      bitstream

Bitstream: the reader (func_800447E0) takes 32-bit big-endian words and consumes bits
LSB-first inside each word, carrying across word boundaries.  Before a frame, if both
run counters are zero, a 1-bit flag is read: 1 -> silence run of (4 bits)+1 frames,
0 -> speech run of (7 bits)+1 frames.  A silence frame outputs 160 zero samples and
consumes no bits.  A speech frame is a GSM 06.10 full-rate parameter set (260 bits):
  LARc[8] widths 6,6,5,5,4,4,3,3 ; then 4 subframes of Nc:7 bc:2 Mc:2 xmaxc:6 xMc[13]:3
Synthesis (func_80044468 -> 800456E0 RPE, 800457D4 LTP, 80044270/80045A54 short-term
filter + de-emphasis 28180 + x2 upscale + clip + &0xFFF8) is the GSM 06.10 decoder.
The rate field is used at playback (func_800432C0: ratio = rate<<15 / output_rate).

Rebuild: the container and bitstream are re-serialised from parsed fields (header, run
structure, every coded parameter, trailing bits).  This is an exact re-encoding of the
parameter stream, not a re-encode from PCM (GSM encoding from PCM is lossy).
"""
import hashlib
import json
import os
import struct
import sys
import wave

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import archive  # noqa: E402

ROM_PATH = os.environ.get("ROM", "baseroms/us/baserom.z64")
ARCH1 = (0x21AC00, 0x4DB6D0)
OUT_DIR = os.path.join(HERE, "..", "private_extracted", "mort")
LAR_BITS = (6, 6, 5, 5, 4, 4, 3, 3)
SUB_BITS = (7, 2, 2, 6) + (3,) * 13
FRAME_SAMPLES = 160


class BitReader:
    def __init__(self, data: bytes, pos: int):
        self.data, self.pos = data, pos

    def read(self, n: int) -> int:
        v = 0
        for i in range(n):
            w = self.pos >> 5
            if 4 * w + 4 > len(self.data):
                raise ValueError("bitstream overrun")
            word = struct.unpack_from(">I", self.data, 4 * w)[0]
            v |= ((word >> (self.pos & 31)) & 1) << i
            self.pos += 1
        return v


class BitWriter:
    def __init__(self):
        self.bits = []

    def write(self, v: int, n: int):
        for i in range(n):
            self.bits.append((v >> i) & 1)

    def words(self) -> bytes:
        out = bytearray()
        for w in range(0, len(self.bits), 32):
            val = 0
            for i, b in enumerate(self.bits[w:w + 32]):
                val |= b << i
            out += struct.pack(">I", val)
        return bytes(out)


def parse(data: bytes) -> dict:
    if data[:4] != b"MORT":
        raise ValueError("bad magic")
    frames, rate, words = struct.unpack_from(">HHI", data, 4)
    if (words & 0xFFFFFF) * 4 != len(data):
        raise ValueError("word count does not match length")
    br = BitReader(data, 96)
    runs, params = [], []
    a = b = 0
    for _ in range(frames):
        if a == 0 and b == 0:
            if br.read(1):
                b = br.read(4) + 1
                runs.append(["silence", b])
            else:
                a = br.read(7) + 1
                runs.append(["speech", a])
        if b:
            b -= 1
            continue
        lar = [br.read(n) for n in LAR_BITS]
        subs = [[br.read(n) for n in SUB_BITS] for _ in range(4)]
        params.append({"lar": lar, "sub": subs})
        a -= 1
    tail_bits = len(data) * 8 - br.pos
    tail = br.read(tail_bits) if tail_bits else 0
    return {"frames": frames, "rate": rate, "words_field": words, "runs": runs,
            "params": params, "end_bit": br.pos - tail_bits, "tail_bits": tail_bits,
            "tail_value": tail, "open_run": [a, b]}


def build(p: dict) -> bytes:
    bw = BitWriter()
    bw.write(0, 96)
    it = iter(p["params"])
    for kind, n in p["runs"]:
        if kind == "silence":
            bw.write(1, 1); bw.write(n - 1, 4)
        else:
            bw.write(0, 1); bw.write(n - 1, 7)
            # the final run may be cut short by the frame count
        cnt = n if kind == "silence" else n
        if kind == "speech":
            for _ in range(cnt):
                try:
                    fr = next(it)
                except StopIteration:
                    break
                for v, w in zip(fr["lar"], LAR_BITS):
                    bw.write(v, w)
                for s in fr["sub"]:
                    for v, w in zip(s, SUB_BITS):
                        bw.write(v, w)
    bw.write(p["tail_value"], p["tail_bits"])
    body = bw.words()
    return b"MORT" + struct.pack(">HHI", p["frames"], p["rate"], p["words_field"]) + body[12:]


# ---------------- GSM 06.10 synthesis (diagnostic; register-width emulation) -------------
def _s32(x):
    x &= 0xFFFFFFFF
    return x - (1 << 32) if x & 0x80000000 else x


def _s16(x):
    x &= 0xFFFF
    return x - 0x10000 if x & 0x8000 else x


def _mr(a, b):
    return _s32(_s32(a * b) + 0x4000) >> 15


MIC = (-32, -32, -16, -16, -8, -8, -4, -4)
B = (0, 0, 2048, -2560, 94, -1792, -341, -1144)
INVA = (13107, 13107, 13107, 13107, 19223, 17476, 31454, 29708)
QLB = (3277, 11469, 21299, 32767)
NRFAC = (29128, 26215, 23832, 21846, 20165, 18725, 17476, 16384)


class Decoder:
    def __init__(self):
        self.dp = [0] * 160
        self.v = [0] * 9
        self.msr = 0
        self.nrp = 40
        self.larpp = [[0] * 8, [0] * 8]
        self.j = 0

    def frame(self, fr) -> list:
        for s in fr["sub"]:
            nc, bc, mc, xmaxc = s[:4]
            xmc = s[4:]
            exp = (xmaxc >> 3) - 1 if xmaxc > 15 else 0
            mant = xmaxc - (exp << 3)
            if mant == 0:
                exp, mant = -4, 7
            else:
                while mant <= 7:
                    mant = mant << 1 | 1
                    exp -= 1
                mant -= 8
            t1 = 0x47FF + (mant << 11)
            t2 = 6 - exp
            t3 = 1 << (t2 - 1)
            xmp = [_s16(((_s32(((x << 13) - 0x7000) * t1) + 0x4000) >> 15) + t3 >> t2) for x in xmc]
            erp = [0] * 40
            for i, x in enumerate(xmp):
                erp[mc + 3 * i] = x
            if 40 <= nc <= 120:
                self.nrp = nc
            brp = QLB[bc]
            self.dp = self.dp[40:] + [0] * 40
            for k in range(40):
                self.dp[120 + k] = _s16(_mr(self.dp[120 + k - self.nrp], brp) + erp[k])
        cur = self.larpp[self.j]
        for i, c in enumerate(fr["lar"]):
            t = _s32((c + MIC[i]) << 10) - (B[i] << 1)
            cur[i] = _s16((_s32(t * INVA[i]) + 0x4000 >> 15) << 1)
        prev = self.larpp[self.j ^ 1]
        out = []
        sri_in = self.dp
        for lo, hi, mode in ((0, 13, 0), (13, 27, 1), (27, 40, 2), (40, 160, 3)):
            rp = []
            for c, p in zip(cur, prev):
                if mode == 0:
                    x = (c >> 2) + (p >> 1) + (p >> 2)
                elif mode == 1:
                    x = (c >> 1) + (p >> 1)
                elif mode == 2:
                    x = (p >> 2) + (c >> 1) + (c >> 2)
                else:
                    x = c
                neg = x < 0
                x = -x if neg else x
                if x < 11059:
                    x <<= 1
                elif x < 20070:
                    x += 11059
                else:
                    x = min((x >> 2) + 26112, 0x7FFF)
                rp.append(_s16(-x if neg else x))
            v = self.v
            for k in range(lo, hi):
                sri = sri_in[k]
                for i in range(7, -1, -1):
                    sri = sri - _mr(v[i], rp[i])
                    if i < 7:
                        v[i + 1] = _mr(rp[i], sri) + v[i]
                v[0] = sri
                t = sri + _mr(28180, self.msr)
                self.msr = t
                t <<= 1
                if ((t >> 15) + 1) >> 1:
                    t = -0x8000 if t < 0 else 0x7FFF
                out.append(_s16(t & 0xFFF8))
            self.msr = _s16(self.msr)
        self.j ^= 1
        return out


def decode(data: bytes) -> list:
    p = parse(data)
    d = Decoder()
    pcm, it = [], iter(p["params"])
    # replay the frame schedule
    a = b = 0
    runs = iter(p["runs"])
    for _ in range(p["frames"]):
        if a == 0 and b == 0:
            kind, n = next(runs)
            if kind == "silence":
                b = n
            else:
                a = n
        if b:
            b -= 1
            pcm += [0] * FRAME_SAMPLES
        else:
            a -= 1
            pcm += d.frame(next(it))
    return pcm


def write_wav(path: str, pcm: list, rate: int):
    with wave.open(path, "wb") as w:
        w.setnchannels(1); w.setsampwidth(2); w.setframerate(rate)
        w.writeframes(struct.pack("<%dh" % len(pcm), *pcm))


def records(rom: bytes):
    return archive.parse(rom, *ARCH1)["records"]


def roundtrip(rom: bytes) -> list:
    rows, seen = [], set()
    for r in records(rom):
        s, e = r["rom_start"], r["rom_end"]
        if (s, e) in seen:
            continue
        seen.add((s, e))
        data = rom[s:e]
        try:
            p = parse(data)
            ok = build(p) == data
            ev = (f"rec {r['index']}: frames={p['frames']} rate={p['rate']} speech={len(p['params'])} "
                  f"tail_bits={p['tail_bits']} tail={p['tail_value']}; rebuild==rom bytes: {ok}")
        except ValueError as ex:
            ok, ev = False, f"rec {r['index']}: parse error {ex}"
        rows.append({"start": s, "end": e, "category": "audio", "subtype": "speech_stream",
                     "owner": "resource_archive_1", "format": "MORT (GSM 06.10 params, run-coded silence)",
                     "extraction": "tools/mort.py decode -> private_extracted/mort/NNN.wav",
                     "rebuild": "reserialise header+runs+parameters+tail bits",
                     "verified": ok, "evidence": ev})
    return rows


def main(argv) -> int:
    rom = open(ROM_PATH, "rb").read()
    rows = roundtrip(rom)
    okb = sum(r["end"] - r["start"] for r in rows if r["verified"])
    print(f"MORT: {sum(r['verified'] for r in rows)}/{len(rows)} unique ranges exact, {okb} bytes")
    if "--wav" in argv or "--tsv" in argv:
        os.makedirs(OUT_DIR, exist_ok=True)
        lines = ["index\trom_start\trom_end\tframes\trate\twords_field\tspeech_frames\tsilence_frames"
                 "\truns\ttail_bits\tsamples\tchannels\tduration_s\tsha256\tverified"]
        for r in records(rom):
            data = rom[r["rom_start"]:r["rom_end"]]
            p = parse(data)
            ok = build(p) == data
            n = p["frames"] * FRAME_SAMPLES
            if "--wav" in argv:
                write_wav(os.path.join(OUT_DIR, "%03d.wav" % r["index"]), decode(data), p["rate"])
            lines.append("\t".join(map(str, [
                r["index"], "0x%X" % r["rom_start"], "0x%X" % r["rom_end"], p["frames"], p["rate"],
                "0x%08X" % p["words_field"], len(p["params"]), p["frames"] - len(p["params"]),
                len(p["runs"]), p["tail_bits"], n, 1, "%.3f" % (n / p["rate"]),
                hashlib.sha256(data).hexdigest(), ok])))
        if "--tsv" in argv:
            path = os.path.join(HERE, "..", "inventory", "mort.tsv")
            open(path, "w").write("\n".join(lines) + "\n")
    return 0 if all(r["verified"] for r in rows) else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
