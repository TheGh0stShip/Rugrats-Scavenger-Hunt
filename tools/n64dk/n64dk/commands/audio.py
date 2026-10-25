"""``n64dk audio``: detect, parse and optionally export every audio structure in a ROM."""
from __future__ import annotations

import json
from pathlib import Path

from ..assets import audio
from ..cli import command
from ..ledger import Ledger
from ..rom import Rom


def _args(p):
    p.add_argument("rom", help="ROM image (z64, v64 or n64 byte order)")
    p.add_argument("--out", help="write inventory.json and intervals.tsv (plus exports) here")
    p.add_argument("--wav", action="store_true", help="decode every sample to WAV under OUT/wav")
    p.add_argument("--midi", action="store_true", help="write sequences (MIDI, compact MIDI, M64, libmus songs) under OUT/seq")
    p.add_argument("--rate", type=int, default=0, help="WAV sample rate override (default: bank rate, else 22050)")


@command("audio", "Audio inventory: libaudio ctl/tbl, MIDI, libmus, seqfile banks; byte-exact intervals")
def audio_cmd(args):
    rom = Rom.load(args.rom)
    data = rom.data
    result = audio.scan(data)
    inv = result.inventory()
    intervals = result.intervals
    problems = audio.check_intervals(intervals)
    inv["overlaps"] = problems
    if args.out:
        out = Path(args.out)
        out.mkdir(parents=True, exist_ok=True)
        Ledger.write_tsv(intervals, out / "intervals.tsv")
        written = {"wav": 0, "seq": 0}
        if args.wav:
            (out / "wav").mkdir(exist_ok=True)
            for name, s in result.samples():
                rate = args.rate or s.rate or 22050
                pcm = s.decode(data)
                (out / "wav" / f"{name}_{s.start:06X}.wav").write_bytes(audio.wav_bytes(pcm, rate, s.loop))
                written["wav"] += 1
        if args.midi:
            (out / "seq").mkdir(exist_ok=True)
            for m in result.midi:
                (out / "seq" / f"midi_{m['start']:06X}.mid").write_bytes(data[m["start"]:m["end"]])
                written["seq"] += 1
            for c in result.containers:
                for name, a, b, ext in c.sequences:
                    path = out / "seq" / f"{name}.{ext}"
                    if ext == "mid" and any(m["start"] == a for m in result.midi):
                        continue
                    path.write_bytes(data[a:b])
                    written["seq"] += 1
        inv["written"] = written
        (out / "inventory.json").write_text(json.dumps(inv, indent=1, default=str) + "\n")
    if args.format == "json":
        return inv
    lines = []
    for c in inv["containers"]:
        extra = {k: v for k, v in c.items() if k not in ("format", "kind", "name", "start", "end", "size", "banks", "waves")}
        lines.append(f"{c['format']:8s} {c['kind']:12s} 0x{c['start']:07X}..0x{c['end']:07X} {c['size']:8d} {extra}")
    for m in inv["midi"]:
        lines.append(f"midi     SMF          0x{m['start']:07X}..0x{m['end']:07X} {m['end'] - m['start']:8d} "
                     f"tracks={m['tracks']} events={m['events']} pad={m['pad_end'] - m['end']}")
    lines.append(f"counts: {inv['counts']}")
    lines.append(f"bytes: {inv['bytes']}")
    lines.append(f"overlaps: {len(problems)}")
    if args.out:
        lines.append(f"written: {inv['written']} -> {args.out}")
    return "\n".join(lines)


audio_cmd.arguments = _args
