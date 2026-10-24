# MORT speech streams (resource archive 1)

All 380 records of resource archive 1 (ROM 0x21AC00..0x4DB6D0, stored direct) are MORT streams.
Total 2,879,376 bytes; all 380 rebuilt byte-for-byte by `tools/mort.py` (`roundtrip(rom)`).

## Code path
- func_8001361C returns the cart address of archive-1 record N; func_8001F1C4 passes it to
  func_800433F4 (start stream; allocates a 0x1200-byte channel, calls init func_80043D90 with a
  0xA00-byte output ring). func_80043554 tests whether a channel handle is still active.
- func_80043E54 (per-tick service): DMAs 0x400-byte chunks into a 0x1000-byte ring at
  channel+0x1A0, on the first chunk reads the header (channel+0x11A0 = frame count,
  +0x11A2 = rate, +0x11A4 = word count & 0xFFFFFF), sets bit position 0x60, then calls
  func_800447E0 once per 160-sample frame until the frame count reaches 0.
- func_800432C0 uses the rate field: pitch ratio = (rate << 15) / output_rate / 32768.

## Header (big-endian, 12 bytes)
| off | type | meaning |
|---|---|---|
| 0 | char[4] | `MORT` |
| 4 | u16 | frame count (each frame = 160 mono samples) |
| 6 | u16 | sample rate, Hz (16000 in all records) |
| 8 | u32 | total length in 32-bit words incl. header (top byte 0 in all records) |

## Bitstream
32-bit big-endian words, bits consumed LSB-first within a word (func_800447E0 bit reader),
starting at bit 96. Per frame, when both run counters are zero, read 1 bit:
`1` -> silence run, length = 4 bits + 1; `0` -> speech run, length = 7 bits + 1.
Silence frames output 160 zero samples and consume no bits (and do not touch decoder state).
Speech frame (260 bits) = GSM 06.10 full-rate parameters:
LARc[8] with widths 6,6,5,5,4,4,3,3; then 4 subframes of Nc:7, bc:2, Mc:2, xmaxc:6, xMc[13]:3.
After the last frame the stream is padded with zero bits to the word boundary (0..31 bits,
zero in all 380 records); runs end exactly at the frame count in every record.

## Synthesis
func_80044468 -> per subframe func_800456E0 (RPE/APCM inverse, FAC = 0x47FF + mant*2048,
grid Mc), func_800457D4 (long-term predictor, QLB = 3277/11469/21299/32767, Nc kept if
outside 40..120, initial 40); then func_80044270 (LARc -> LARpp with GSM MIC/B/INVA,
double-buffered via +0x188) and func_80045A54 (LAR interpolation 13/14/13/120 samples,
LAR->rp, 8-tap lattice, de-emphasis 28180, x2, clip, & 0xFFF8). This is the GSM 06.10
decoder, implemented with 32-bit register arithmetic and no saturation in the lattice/LTP;
`tools/mort.py` emulates that register-width behaviour. A byte-packed GSM unpacker
(the function immediately after func_80044468) also exists but is not used by this path.

## Rebuild / verification
`parse` recovers header, run list, every coded parameter and the tail bits; `build`
re-serialises them. This is an exact re-encoding of the parameter stream (not a re-encode
from PCM; GSM is lossy). Executed result: 380/380 records equal to ROM bytes.

## Decoded output (diagnostic)
`python3 tools/mort.py --wav` writes private_extracted/mort/NNN.wav (mono, 16-bit, header rate).
Sample count = frames * 160 (proven from the decoder schedule); total 16,657,120 samples.
The PCM has not been compared against hardware/emulator output, so it is diagnostic only.
