"""Loader tracing: ROM ranges the program itself names, and the code that loads them.

Two evidence sources, both game-agnostic:

* **Code.**  A linear constant propagator walks the main segment tracking
  ``lui``/``addiu``/``ori``/``addu``/``subu`` constants.  At every ``jal`` the argument
  registers (and outgoing stack arguments ``0x10..0x1C($sp)``) are captured.  A call
  that receives two ROM addresses (cartridge PI form ``0xB0000000 + offset`` or raw
  offsets) with ``start < end``, or a ROM address plus a size computed as
  ``end - start``, yields a range.  Callees that receive such ranges at two or more
  sites are reported as DMA wrappers; their argument roles are then reused to read
  single-address sites (start plus a constant size).  The ``subu`` of two ROM constants
  is evidence on its own (``size = end - start`` for linker symbols).
* **Data.**  Initialized data is scanned for ascending runs of ROM addresses (offset
  tables: each entry ends where the next begins), parallel start/end arrays, and
  records holding ``start, end`` pairs at a fixed stride (segment/course tables).

Raw-offset evidence is much weaker than PI-form evidence (any small ascending integers
look like offsets), so raw data tables need a repeated record stride or agreement with a
validated compressed stream.
"""
from __future__ import annotations

import struct
from collections import defaultdict
from dataclasses import dataclass

from .. import mips

PI_BASE = 0xB0000000
MAX_RANGE = 8 << 20
ARG_REGS = (4, 5, 6, 7)  # a0..a3
CALLER_SAVED = set(range(1, 16)) | {24, 25}
STACK_ARGS = (0x10, 0x14, 0x18, 0x1C)


@dataclass(frozen=True)
class RomRange:
    start: int
    end: int
    form: str  # "pi" or "raw"
    source: str  # "call", "wrapper", "size", "store", "pairs", "records", "table"
    loader: int | None  # VRAM of the function holding the site (code evidence)
    site: int  # VRAM of the call / subu / sw instruction, or of the table entry
    callee: int | None = None
    space: str = "vram"  # "rom" when ``site`` is a ROM offset (tables outside the main segment)

    @property
    def size(self) -> int:
        return self.end - self.start

    def as_dict(self) -> dict:
        return {"start": f"0x{self.start:X}", "end": f"0x{self.end:X}", "form": self.form,
                "source": self.source, "loader": self.loader and f"0x{self.loader:08X}",
                "site": f"0x{self.site:08X}", "site_space": self.space,
                "callee": self.callee and f"0x{self.callee:08X}"}


def rom_form(value: int, rom_size: int, min_raw: int = 0x1000) -> tuple[int, str] | None:
    """Map a 32-bit value to a ROM offset if it can name one."""
    if PI_BASE + min_raw <= value <= PI_BASE + rom_size:
        return value - PI_BASE, "pi"
    if min_raw <= value <= rom_size:
        return value, "raw"
    return None


@dataclass
class CallSite:
    site: int
    callee: int
    function: int | None
    args: dict  # slot -> (value, provenance or None); slots 0..3 regs, 4..7 stack


_MEM_OPS = {0x20, 0x21, 0x23, 0x24, 0x25, 0x27, 0x28, 0x29, 0x2B, 0x31, 0x35, 0x37, 0x39, 0x3D, 0x3F, 0x09}


def _s16(v: int) -> int:
    return v - 0x10000 if v & 0x8000 else v


def trace_calls(code: bytes, vram: int, rom_size: int,
                refs: dict[int, int] | None = None) -> tuple[list[CallSite], list[RomRange]]:
    """Linear constant propagation; returns call sites with constant arguments and
    ``subu`` end-start evidence."""
    known: dict[int, tuple[int, tuple | None]] = {}
    stack: dict[int, tuple[int, tuple | None]] = {}
    calls: list[CallSite] = []
    sizes: list[RomRange] = []
    func = None
    pending_call = None
    last_store: dict[int, tuple] = {}
    returned = 0
    fresh = False
    stored: dict[int, tuple] = {}
    words = len(code) // 4
    for i in range(words):
        word = struct.unpack_from(">I", code, 4 * i)[0]
        ins = mips.decode(word, vram + 4 * i)
        if returned == 1 and word:
            func, returned, fresh = vram + 4 * i, 0, True  # first word after return + delay slot
        elif returned > 1:
            returned -= 1
        name = ins.getOpcodeName()
        rs, rt, rd = (word >> 21) & 31, (word >> 16) & 31, (word >> 11) & 31
        imm = word & 0xFFFF
        dest = None
        value = None
        if refs is not None and rs in known and (word >> 26) in _MEM_OPS:
            refs.setdefault((known[rs][0] + _s16(imm)) & 0xFFFFFFFF, func)
        if name == "addiu" and rt == mips.SP and rs == mips.SP and _s16(imm) < 0:
            if not fresh:
                func = ins.vram
            fresh = False
            last_store.clear()
            known.clear()
            stack.clear()
        if name == "lui":
            dest, value = rt, (imm << 16, None)
        elif name in ("addiu", "daddiu"):
            dest = rt
            if rs == 0:
                value = (_s16(imm) & 0xFFFFFFFF, None)
            elif rs in known:
                value = ((known[rs][0] + _s16(imm)) & 0xFFFFFFFF, None)
        elif name == "ori":
            dest = rt
            if rs == 0:
                value = (imm, None)
            elif rs in known:
                value = (known[rs][0] | imm, None)
        elif name in ("addu", "or", "subu", "daddu", "dsubu"):
            dest = rd
            a = (0, None) if rs == 0 else known.get(rs)
            b = (0, None) if rt == 0 else known.get(rt)
            if a is not None and b is not None:
                if name in ("subu", "dsubu"):
                    value = ((a[0] - b[0]) & 0xFFFFFFFF, ("diff", a[0], b[0]))
                    hi, lo = rom_form(a[0], rom_size), rom_form(b[0], rom_size)
                    if hi and lo and hi[1] == lo[1] and 0 < hi[0] - lo[0] <= MAX_RANGE:
                        sizes.append(RomRange(lo[0], hi[0], hi[1], "size", func, ins.vram))
                elif name == "or":
                    value = (a[0] | b[0], a[1] or b[1])
                else:
                    value = ((a[0] + b[0]) & 0xFFFFFFFF, a[1] if b[0] == 0 else b[1] if a[0] == 0 else None)
        elif name == "sw" and rs != mips.SP and rt in known:
            r = rom_form(known[rt][0], rom_size)
            if r and rs in known:
                stored[(known[rs][0] + _s16(imm)) & 0xFFFFFFFF] = (r[0], r[1], func, ins.vram)
            elif r and r[1] == "pi":
                prev = last_store.get(rs)
                off = _s16(imm)
                if prev and prev[0] == func and prev[1] + 4 == off and 0 < r[0] - prev[2] <= MAX_RANGE:
                    sizes.append(RomRange(prev[2], r[0], "pi", "store", func, ins.vram))
                last_store[rs] = (func, off, r[0])
        elif name == "sw" and rs == mips.SP:
            off = _s16(imm)
            if off in STACK_ARGS:
                if rt == 0:
                    stack[off] = (0, None)
                elif rt in known:
                    stack[off] = known[rt]
                else:
                    stack.pop(off, None)
        else:
            d = ins.getDestinationGpr()
            if d is not None:
                dest = mips.reg(d)
        if dest is not None and dest != 0:
            if value is None:
                known.pop(dest, None)
            else:
                known[dest] = value
        if pending_call is not None:  # this was the delay slot
            site, target = pending_call
            args = {k: known[r] for k, r in enumerate(ARG_REGS) if r in known}
            for k, off in enumerate(STACK_ARGS):
                if off in stack:
                    args[4 + k] = stack[off]
            calls.append(CallSite(site, target, func, args))
            for r in CALLER_SAVED:
                known.pop(r, None)
            stack.clear()
            pending_call = None
        if ins.isJumpWithAddress() and ins.doesLink():
            pending_call = (ins.vram, ins.getInstrIndexAsVram())
        elif word == mips.JR_RA:
            returned = 2
            known.clear()
            stack.clear()
    # Constant ROM addresses stored to adjacent absolute words form (start, end) pairs.
    words_at = {addr // 4: (v[0], v[1]) for addr, v in stored.items() if addr % 4 == 0}
    for k in paired_words(words_at):
        a, form, fn, site = stored[4 * k]
        if form == "pi" or a % 4 == stored[4 * k + 4][0] % 4 == 0:
            sizes.append(RomRange(a, stored[4 * k + 4][0], form, "store", fn, site))
    return calls, sizes


def paired_words(words: dict[int, tuple[int, str]]) -> list[int]:
    """Indices ``k`` where ``words[k], words[k+1]`` read as a (start, end) pair.

    In each run of consecutive words the pairing phase (even/odd) with more valid
    pairs wins, so ``{start, end, start, end}`` records do not also yield the bogus
    ``end -> next start`` pairs between them."""
    def valid(k):
        a, b = words.get(k), words.get(k + 1)
        return bool(a and b and a[1] == b[1] and 0 < b[0] - a[0] <= MAX_RANGE)

    out = []
    keys = sorted(words)
    i = 0
    while i < len(keys):
        j = i
        while j + 1 < len(keys) and keys[j + 1] == keys[j] + 1:
            j += 1
        run = keys[i:j + 1]
        phases = [[k for k in run[p::2] if valid(k)] for p in (0, 1)]
        best = max(phases, key=len)
        other = phases[1] if best is phases[0] else phases[0]
        out += best
        if len(best) == len(other) and best != other:
            out += other  # ambiguous: keep both phases
        i = j + 1
    return out


def _pairs_in_call(c: CallSite, rom_size: int):
    """(start_slot, end_slot, kind, start, end, form) for each ROM range the call names."""
    roms = {k: (rom_form(v, rom_size), prov) for k, (v, prov) in c.args.items()}
    out = []
    for k, (r, prov) in roms.items():
        if prov and prov[0] == "diff":
            hi, lo = rom_form(prov[1], rom_size), rom_form(prov[2], rom_size)
            if hi and lo and hi[1] == lo[1] and 0 < hi[0] - lo[0] <= MAX_RANGE:
                start_slot = next((s for s, (rr, _) in roms.items() if rr and rr[0] == lo[0]), None)
                out.append((start_slot, k, "size", lo[0], hi[0], hi[1]))
    slots = sorted(k for k, (r, _) in roms.items() if r)
    for i, a in enumerate(slots):
        for b in slots[i + 1:]:
            ra, rb = roms[a][0], roms[b][0]
            if ra[1] != rb[1]:
                continue
            if ra[1] == "raw" and (ra[0] < 0x1000 or rb[0] - ra[0] < 0x10):
                continue
            if 0 < rb[0] - ra[0] <= MAX_RANGE:
                out.append((a, b, "end", ra[0], rb[0], ra[1]))
    return out


def code_ranges(code: bytes, vram: int, rom_size: int, stream_starts: set[int] = frozenset(),
                refs: dict[int, int] | None = None) -> tuple[list[RomRange], dict]:
    """ROM ranges evidenced by call arguments; also returns detected DMA wrappers
    ``{callee: {"sites": n, "start_slot": s, "end_slot": e, "kind": "end"|"size"}}``."""
    calls, sizes = trace_calls(code, vram, rom_size, refs)
    out: list[RomRange] = []
    strong: set[RomRange] = set()
    roles: dict[int, dict] = defaultdict(lambda: defaultdict(int))
    for c in calls:
        for s_slot, e_slot, kind, a, b, form in _pairs_in_call(c, rom_size):
            r = RomRange(a, b, form, "call", c.function, c.site, c.callee)
            out.append(r)
            if kind == "size":
                strong.add(r)
            if s_slot is not None and (form == "pi" or kind == "size"):
                roles[c.callee][(s_slot, e_slot, kind)] += 1
    wrappers = {}
    for callee, counts in roles.items():
        (s_slot, e_slot, kind), n = max(counts.items(), key=lambda kv: kv[1])
        if n >= 2:
            wrappers[callee] = {"sites": n, "start_slot": s_slot, "end_slot": e_slot, "kind": kind}
    # Two raw constants passed together are weak evidence (sizes, coordinates and
    # counts look the same); keep them only for a recurring wrapper or a known stream.
    out = [r for r in out if r.form == "pi" or r in strong or r.callee in wrappers or r.start in stream_starts]
    seen = {(r.site, r.start) for r in out}
    for c in calls:
        w = wrappers.get(c.callee)
        if not w or w["kind"] != "size":
            continue
        sv, ev = c.args.get(w["start_slot"]), c.args.get(w["end_slot"])
        if not sv or not ev:
            continue
        r = rom_form(sv[0], rom_size)
        if r and 0 < ev[0] <= MAX_RANGE and r[0] + ev[0] <= rom_size and (c.site, r[0]) not in seen:
            out.append(RomRange(r[0], r[0] + ev[0], r[1], "wrapper", c.function, c.site, c.callee))
    sites = {r.site for r in out}
    out += [s for s in sizes if s.site not in sites and not any(o.loader == s.loader and o.start == s.start for o in out)]
    return out, wrappers


def table_ranges(data: bytes, vram: int, rom_size: int, stream_starts: set[int] = frozenset(),
                 min_pi_run: int = 2, min_raw_run: int = 4, min_records: int = 3,
                 strict_raw: bool = False) -> list[RomRange]:
    """ROM ranges evidenced by tables in initialized data (``data`` mapped at ``vram``)."""
    n = len(data) // 4
    words = struct.unpack_from(f">{n}I", data)
    forms = [rom_form(w, rom_size) for w in words]
    out: list[RomRange] = []

    # Ascending runs of same-form addresses: offset tables.
    i = 0
    runs = []
    while i < n:
        if not forms[i]:
            i += 1
            continue
        j = i + 1
        while j < n and forms[j] and forms[j][1] == forms[i][1] and forms[j][0] > forms[j - 1][0] \
                and forms[j][0] - forms[j - 1][0] <= MAX_RANGE:
            j += 1
        form = forms[i][1]
        vals = [forms[k][0] for k in range(i, j)]
        ok = j - i >= (min_pi_run if form == "pi" else min_raw_run)
        if ok and form == "raw":
            ok = sum(v in stream_starts for v in vals) >= 2 or (
                not strict_raw and j - i >= 2 * min_raw_run and all(v % 8 == 0 for v in vals))
        if ok:
            runs.append((i, j, form, vals))
            for k in range(len(vals) - 1):
                out.append(RomRange(vals[k], vals[k + 1], form, "table", None, vram + 4 * (i + k)))
        i = max(j, i + 1)

    # Parallel arrays: a block of 2n same-form words (any order) whose first half are
    # starts and second half ends, every start < end, and the n ranges disjoint.
    parallel: set[int] = set()
    i = 0
    while i < n:
        if not forms[i]:
            i += 1
            continue
        j = i
        while j < n and forms[j] and forms[j][1] == forms[i][1]:
            j += 1
        block = [forms[k][0] for k in range(i, j)]
        form = forms[i][1]
        for half in range(len(block) // 2, 1, -1):
            for lead in range(0, len(block) - 2 * half + 1):
                st, en = block[lead:lead + half], block[lead + half:lead + 2 * half]
                spans = sorted(zip(st, en))
                if all(0 < y - x <= MAX_RANGE for x, y in spans) and \
                        all(a[1] <= b[0] for a, b in zip(spans, spans[1:])) and \
                        (form == "pi" or sum(x in stream_starts for x in st) >= 2):
                    for k, (x, y) in enumerate(zip(st, en)):
                        out.append(RomRange(x, y, form, "pairs", None, vram + 4 * (i + lead + k)))
                    parallel.update(range(i + lead, i + lead + 2 * half))
                    break
            else:
                continue
            break
        i = j
    if parallel:
        out = [r for r in out if r.source == "pairs" or (r.site - vram) // 4 not in parallel]

    # Adjacent (start, end) pairs: PI form always; raw form when repeated at a stride.
    pair_at = {}
    for k in paired_words({k: f for k, f in enumerate(forms) if f and k not in parallel}):
        pair_at[k] = (forms[k][0], forms[k + 1][0], forms[k][1])
    accepted = set()
    for k, (a, b, form) in pair_at.items():
        if not strict_raw and (form == "pi" or a in stream_starts):
            accepted.add(k)
    raw_keys = sorted(k for k, p in pair_at.items() if p[2] == "raw" or strict_raw)
    raw_set = set(raw_keys)
    for k in raw_keys:
        if any(v % 4 for v in pair_at[k][:2]):
            continue
        for stride in range(2, 33):
            chain = [k]
            while chain[-1] + stride in raw_set and not any(v % 4 for v in pair_at[chain[-1] + stride][:2]):
                chain.append(chain[-1] + stride)
            if len(chain) >= min_records:
                spans = sorted(pair_at[c][:2] for c in chain)
                hits = sum(pair_at[c][0] in stream_starts for c in chain)
                if all(x[1] <= y[0] for x, y in zip(spans, spans[1:])) and (not strict_raw or hits >= 2):
                    accepted.update(chain)
                    break
    for k in sorted(accepted):
        a, b, form = pair_at[k]
        out.append(RomRange(a, b, form, "records", None, vram + 4 * k))
    return out


_RANK = {"call": 0, "wrapper": 1, "size": 2, "store": 3, "pairs": 4, "records": 5, "table": 6}
MAX_WEAK_RANGE = 1 << 20


def _pow2(v: int) -> bool:
    return v > 0 and v & (v - 1) == 0


def plausible(ranges: list[RomRange], stream_starts: set[int] = frozenset()) -> tuple[list[RomRange], dict]:
    """Drop ranges that contradict each other or look like other numbers.

    Items the program loads cannot partially overlap (nesting is fine: a bank and its
    members), so of two partially overlapping ranges the one with stronger evidence (code
    over tables; then the smaller) is kept.  Ranges over 1 MiB resting on table evidence
    or on raw numbers, and raw ranges whose start and end are both powers of two (size
    tables read as offsets), are dropped unless a validated stream starts there.  Returns
    the kept ranges and the reasons for the dropped ones."""
    dropped: dict[str, int] = defaultdict(int)
    kept: list[RomRange] = []
    for r in sorted(ranges, key=lambda r: (_RANK.get(r.source, 9), r.size, r.start)):
        anchored = r.start in stream_starts
        weak = r.source in ("pairs", "records", "table")
        if not anchored and r.size > MAX_WEAK_RANGE and (weak or r.form == "raw"):
            dropped["over 1 MiB on table or raw-number evidence"] += 1
            continue
        if r.form == "raw" and not anchored:
            if _pow2(r.start) and _pow2(r.end):
                dropped["power-of-two bounds (a size table)"] += 1
                continue
        if any(k.start < r.end and r.start < k.end and not (k.start <= r.start and r.end <= k.end)
               and not (r.start <= k.start and k.end <= r.end) for k in kept):
            dropped["partial overlap with stronger evidence"] += 1
            continue
        kept.append(r)
    return sorted(kept, key=lambda r: (r.start, r.end)), dict(dropped)


def dedupe(ranges: list[RomRange]) -> list[RomRange]:
    """One entry per (start, end): keep the strongest source, then the first site."""
    rank = {"call": 0, "wrapper": 1, "size": 2, "store": 3, "pairs": 4, "records": 5, "table": 6}
    best: dict[tuple[int, int], RomRange] = {}
    for r in sorted(ranges, key=lambda r: (rank.get(r.source, 9), r.site)):
        best.setdefault((r.start, r.end), r)
    return sorted(best.values(), key=lambda r: (r.start, r.end))


@dataclass
class LoaderEvidence:
    ranges: list[RomRange]
    wrappers: dict
    rejected: dict = None   # reason -> count, from :func:`plausible`

    def as_dict(self) -> dict:
        return {"ranges": [r.as_dict() for r in self.ranges],
                "wrappers": {f"0x{k:08X}": v for k, v in self.wrappers.items()},
                "rejected": self.rejected or {}}


def trace(rom_data: bytes, main_rom: int, main_vram: int, main_size: int,
          stream_starts: set[int] = frozenset(), whole_rom_tables: bool = True,
          exclude: list[tuple[int, int]] = (), segment: bytes | None = None) -> LoaderEvidence:
    """All loader evidence: code and tables of the main segment, plus (optionally)
    tables anywhere else in the ROM (reported with ROM-offset sites).  Outside the
    main segment only stride-repeated records agreeing with two or more known stream
    starts are accepted, and ``exclude`` spans (e.g. compressed streams) are skipped.

    ``segment`` gives the main segment's bytes when they are not in the ROM as is (a
    loader's unpacked program, :mod:`n64dk.image`); ``main_rom`` is then ignored and the
    whole ROM after the boot area is searched for tables."""
    import bisect
    import dataclasses

    seg = segment if segment is not None else rom_data[main_rom:main_rom + main_size]
    rom_size = len(rom_data)
    starts = set(stream_starts)
    refs: dict[int, int] = {}
    code, wrappers = code_ranges(seg, main_vram, rom_size, starts, refs)
    tables = table_ranges(seg, main_vram, rom_size, starts)
    # A table's loader is the function referencing the nearest address at or below it.
    keys = sorted(a for a, f in refs.items() if f is not None and main_vram <= a < main_vram + main_size)
    attributed = []
    for r in tables:
        i = bisect.bisect_right(keys, r.site) - 1
        if i >= 0 and r.site - keys[i] < 0x800:
            r = dataclasses.replace(r, loader=refs[keys[i]])
        attributed.append(r)
    extra: list[RomRange] = []
    if whole_rom_tables:
        spans = ((0x1000, rom_size),) if segment is not None else ((0x1000, main_rom), (main_rom + main_size, rom_size))
        for lo, hi in spans:
            if hi - lo >= 8:
                for r in table_ranges(rom_data[lo:hi], lo, rom_size, starts, strict_raw=True):
                    if r.source == "records" and not any(a <= r.site < b for a, b in exclude):
                        extra.append(dataclasses.replace(r, space="rom"))
    kept, rejected = plausible(dedupe(code + attributed + extra), starts)
    return LoaderEvidence(kept, wrappers, rejected)
