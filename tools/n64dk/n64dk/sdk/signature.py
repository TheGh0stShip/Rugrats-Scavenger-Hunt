"""Function signatures from relocatable MIPS objects.

A signature is the function's instruction words with every relocated field
masked (``jal``/``j`` targets, ``%hi``/``%lo`` immediates, absolute data words),
hashed together with the function size.  Only hashes and mask positions are
kept, so a signature database never contains code bytes.

Two masks are used:

* the *relocation mask* from the object's relocation records (exact), and
* the *canonical mask*, a rule that can be applied to an unrelocated ROM image:
  jump targets, ``lui`` immediates and the immediates of ``addiu``/``ori`` and
  loads/stores whose base register is not ``$sp``.  It is a superset of the
  relocation mask for compiler output and is used for indexing and for
  lower-confidence "variant" matches.
"""
from __future__ import annotations

import hashlib
import struct
from dataclasses import dataclass, field

from .fingerprint import function_features
from ..elf import Elf, R_MIPS_26, R_MIPS_32, R_MIPS_HI16, R_MIPS_LO16

R_MIPS_GPREL16 = 7
R_MIPS_GOT16, R_MIPS_CALL16 = 9, 11
JR_RA = 0x03E00008
KEY_WORDS = 4

# Field masks applied to a word: the bits kept.
KEEP_26 = 0xFC000000
KEEP_16 = 0xFFFF0000
KEEP_NONE = 0
_KEEP_BY_RELOC = {R_MIPS_26: KEEP_26, R_MIPS_HI16: KEEP_16, R_MIPS_LO16: KEEP_16,
                  R_MIPS_GPREL16: KEEP_16, R_MIPS_GOT16: KEEP_16, R_MIPS_CALL16: KEEP_16,
                  R_MIPS_32: KEEP_NONE}
_IMM_OPS = {0x08, 0x09, 0x0D, 0x18, 0x19}  # addi, addiu, ori, daddi, daddiu
_LS_OPS = set(range(0x20, 0x40)) - {0x2F, 0x33, 0x37, 0x3B}  # loads/stores incl. cop1/ld/sd


def canon_keep(word: int) -> int:
    """Bits of ``word`` kept by the canonical mask."""
    op = word >> 26
    if op in (2, 3):
        return KEEP_26
    if op == 0x0F:
        return KEEP_16
    if op in _IMM_OPS or op in _LS_OPS:
        return KEEP_16 if (word >> 21) & 31 != 29 else 0xFFFFFFFF
    return 0xFFFFFFFF


def canon_words(words) -> list[int]:
    return [w & canon_keep(w) for w in words]


def digest(words, size: int | None = None) -> str:
    h = hashlib.blake2b(digest_size=8)
    if size is not None:
        h.update(struct.pack(">I", size))
    h.update(struct.pack(f">{len(words)}I", *words))
    return h.hexdigest()


@dataclass
class Signature:
    name: str
    size: int                      # bytes
    exact: str                     # hash of relocation-masked words
    canon: str                     # hash of canonical-masked words
    key: str                       # hash of the first KEY_WORDS canonical words
    prefix: str                    # hash of the first KEY_WORDS raw words (unmasked)
    masks: list[tuple[int, int]]   # (word index, kept bits) for relocated words
    calls: list[tuple[int, str]] = field(default_factory=list)  # (word index, callee)
    local: bool = False
    obj: str = ""
    features: dict | None = None   # compiler idiom features (not stored per signature)

    def keep_vector(self) -> list[int]:
        keep = [0xFFFFFFFF] * (self.size // 4)
        for i, k in self.masks:
            keep[i] = k
        return keep


def exact_from_words(words, masks) -> str:
    keep = dict(masks)
    return digest([w & keep.get(i, 0xFFFFFFFF) for i, w in enumerate(words)], len(words) * 4)


def _trim(words: list[int]) -> list[int]:
    """Drop alignment padding (trailing zero words) but keep a delay-slot nop."""
    n = len(words)
    while n and words[n - 1] == 0:
        n -= 1
    if n < len(words) and n and _has_delay_slot(words[n - 1]):
        n += 1
    return words[:n]


def _has_delay_slot(w: int) -> bool:
    op = w >> 26
    if op == 0:
        return (w & 0x3F) in (8, 9)  # jr, jalr
    if op == 1:
        return True
    if op in (2, 3, 4, 5, 6, 7) or op in (0x14, 0x15, 0x16, 0x17):
        return True
    if op == 0x11 and (w >> 21) & 31 == 8:  # bc1x
        return True
    return False


def signatures_from_elf(elf: Elf, obj: str = "", include_local: bool = True) -> list[Signature]:
    """One signature per function symbol in ``.text`` (sizes from the next symbol when unset)."""
    text = elf.section(".text")
    if text is None or not text.data:
        return []
    starts: dict[int, tuple[str, bool]] = {}
    for s in elf.symbols:
        if s.section != text.index or not s.name or s.type == 3:  # skip section symbols
            continue
        if s.name.startswith((".", "$")) or s.name.endswith((".s", ".c", ".o")):
            continue
        is_func = s.type == 2
        is_global = s.bind in (1, 2)
        if not (is_func or is_global):
            continue
        if not is_global and not include_local:
            continue
        if s.value not in starts or (is_global and starts[s.value][1]):
            starts[s.value] = (s.name, not is_global)
    if not starts:
        return []
    words_all = list(struct.unpack(f">{len(text.data) // 4}I", text.data[: len(text.data) // 4 * 4]))
    relocs = sorted(elf.relocations(text), key=lambda r: r[0])
    by_word: dict[int, tuple[int, int]] = {}
    for off, rtype, sidx, _add in relocs:
        by_word[off // 4] = (rtype, sidx)
    func_at = {v: n for v, (n, _l) in starts.items()}
    ordered = sorted(starts)
    out = []
    for i, start in enumerate(ordered):
        end = ordered[i + 1] if i + 1 < len(ordered) else len(words_all) * 4
        if start % 4:
            continue
        words = _trim(words_all[start // 4: end // 4])
        if not words:
            continue
        masks, calls = [], []
        for j in range(len(words)):
            r = by_word.get(start // 4 + j)
            if r is None:
                continue
            rtype, sidx = r
            masks.append((j, _KEEP_BY_RELOC.get(rtype, KEEP_16 if rtype not in (R_MIPS_32,) else 0)))
            if rtype == R_MIPS_26 and words[j] >> 26 == 3:
                callee = _reloc_target_name(elf, sidx, words[j], func_at, text.index)
                if callee:
                    calls.append((j, callee))
        keep = dict(masks)
        rmasked = [w & keep.get(j, 0xFFFFFFFF) for j, w in enumerate(words)]
        cmasked = [w & keep.get(j, 0xFFFFFFFF) & canon_keep(w) for j, w in enumerate(words)]
        name, local = starts[start]
        out.append(Signature(
            name=name, size=len(words) * 4, exact=digest(rmasked, len(words) * 4),
            canon=digest(cmasked, len(words) * 4), key=digest(cmasked[:KEY_WORDS]),
            prefix=digest(words[:KEY_WORDS]), masks=masks, calls=calls, local=local, obj=obj,
            features=function_features(words)))
    return out


def _reloc_target_name(elf: Elf, sidx: int, word: int, func_at: dict[int, str], text_index: int) -> str | None:
    if sidx >= len(elf.symbols):
        return None
    sym = elf.symbols[sidx]
    if sym.type == 3 or not sym.name:  # section-relative: addend is in the instruction
        if sym.section == text_index:
            return func_at.get(((word & 0x3FFFFFF) << 2) + sym.value)
        return None
    return sym.name


def read_archive(data: bytes) -> list[tuple[str, bytes]]:
    """Members of a System V / GNU ``ar`` archive."""
    if not data.startswith(b"!<arch>\n"):
        raise ValueError("not an ar archive")
    pos, names, out = 8, b"", []
    while pos + 60 <= len(data):
        hdr = data[pos:pos + 60]
        name = hdr[:16].decode("ascii", "replace").rstrip()
        size = int(hdr[48:58].decode().strip() or 0)
        body = data[pos + 60: pos + 60 + size]
        pos += 60 + size + (size & 1)
        if name == "//":
            names = body
            continue
        if name in ("/", "/SYM64/", "__.SYMDEF", "__.SYMDEF SORTED"):
            continue
        if name.startswith("/") and name[1:].isdigit():
            off = int(name[1:])
            name = names[off:names.index(b"\n", off)].decode().rstrip("/")
        out.append((name.rstrip("/"), body))
    return out


def signatures_from_file(data: bytes, obj: str = "") -> list[Signature]:
    if data.startswith(b"!<arch>\n"):
        out = []
        for member, body in read_archive(data):
            if body[:4] == b"\x7fELF":
                out.extend(signatures_from_elf(Elf(body), member))
        return out
    return signatures_from_elf(Elf(data), obj)
