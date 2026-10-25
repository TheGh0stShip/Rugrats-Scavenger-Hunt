"""N64 cartridge images: byte order, header, IPL3/CIC, checksum and boot analysis."""
from __future__ import annotations

import hashlib
import struct
import zlib
from dataclasses import dataclass, field
from pathlib import Path


class RomError(ValueError):
    pass


# CRC32 of the IPL3 bytes 0x40..0x1000 for the retail boot codes.
CIC_BY_IPL3_CRC = {
    0x6170A4A1: "6101",
    0x009E9EA3: "7102",
    0x90BB6CB5: "6102",
    0x0B050EE0: "6103",
    0x98BC2C86: "6105",
    0xACC8580A: "6106",
}
CIC_SEED = {"6101": 0xF8CA4DDC, "7102": 0xF8CA4DDC, "6102": 0xF8CA4DDC,
            "6103": 0xA3886759, "6105": 0xDF26F436, "6106": 0x1FEA617A}
# Entry address relocation applied by some IPL3 variants before jumping to the game.
CIC_ENTRY_ADJUST = {"6103": -0x100000, "6106": -0x200000}
CHECKSUM_START, CHECKSUM_LENGTH = 0x1000, 0x100000


def to_big_endian(data: bytes) -> tuple[bytes, str]:
    """Return the image in z64 (big-endian) order and the detected source order."""
    magic = data[:4]
    if magic == b"\x80\x37\x12\x40":
        return data, "z64"
    if magic == b"\x37\x80\x40\x12":  # 16-bit byte swapped
        swapped = bytearray(data)
        swapped[0::2], swapped[1::2] = data[1::2], data[0::2]
        return bytes(swapped), "v64"
    if magic == b"\x40\x12\x37\x80":  # 32-bit little-endian words
        words = struct.unpack(f"<{len(data) // 4}I", data[: len(data) // 4 * 4])
        return struct.pack(f">{len(words)}I", *words), "n64"
    raise RomError(f"unrecognised N64 image magic {magic.hex()}")


def _u32(x: int) -> int:
    return x & 0xFFFFFFFF


def compute_checksum(rom: bytes, cic: str) -> tuple[int, int]:
    """The libdragon/n64crc header checksum algorithm for the given boot code."""
    seed = CIC_SEED[cic]
    t1 = t2 = t3 = t4 = t5 = t6 = seed
    data = rom[CHECKSUM_START:CHECKSUM_START + CHECKSUM_LENGTH]
    if len(data) < CHECKSUM_LENGTH:
        data = data + bytes(CHECKSUM_LENGTH - len(data))
    words = struct.unpack(f">{CHECKSUM_LENGTH // 4}I", data)
    ipl = struct.unpack(">64I", rom[0x750:0x850]) if cic == "6105" else None
    for i, d in enumerate(words):
        if _u32(t6 + d) < t6:
            t4 = _u32(t4 + 1)
        t6 = _u32(t6 + d)
        t3 ^= d
        shift = d & 0x1F
        r = _u32((d << shift) | (d >> (32 - shift))) if shift else d
        t5 = _u32(t5 + r)
        t2 = t2 ^ r if t2 > d else t2 ^ t6 ^ d
        if cic == "6105":
            t1 = _u32(t1 + (ipl[i & 0x3F] ^ d))
        else:
            t1 = _u32(t1 + (t5 ^ d))
    if cic == "6103":
        return _u32((t6 ^ t4) + t3), _u32((t5 ^ t2) + t1)
    if cic == "6106":
        return _u32(t6 * t4 + t3), _u32(t5 * t2 + t1)
    return _u32(t6 ^ t4 ^ t3), _u32(t5 ^ t2 ^ t1)


@dataclass
class Header:
    pi_config: int
    clock_rate: int
    entry: int
    release: int
    crc1: int
    crc2: int
    title: str
    media: str
    game_id: str
    region: str
    version: int

    @property
    def game_code(self) -> str:
        return self.media + self.game_id + self.region


@dataclass
class Rom:
    data: bytes
    source_order: str = "z64"
    path: Path | None = None
    header: Header = field(init=False)
    cic: str | None = field(init=False)

    def __post_init__(self) -> None:
        if len(self.data) < 0x101000:
            raise RomError("image is smaller than header + IPL3 + checksum span")
        h = struct.unpack_from(">IIIIII", self.data, 0)
        self.header = Header(
            pi_config=h[0], clock_rate=h[1], entry=h[2], release=h[3], crc1=h[4], crc2=h[5],
            title=self.data[0x20:0x34].decode("ascii", "replace").rstrip(" \0"),
            media=chr(self.data[0x3B]), game_id=self.data[0x3C:0x3E].decode("ascii", "replace"),
            region=chr(self.data[0x3E]), version=self.data[0x3F])
        self.cic = CIC_BY_IPL3_CRC.get(zlib.crc32(self.data[0x40:0x1000]))

    @classmethod
    def load(cls, path: str | Path) -> "Rom":
        raw = Path(path).read_bytes()
        data, order = to_big_endian(raw)
        return cls(data, order, Path(path))

    @property
    def sha1(self) -> str:
        return hashlib.sha1(self.data).hexdigest()

    @property
    def size(self) -> int:
        return len(self.data)

    @property
    def entry_vram(self) -> int:
        """Address the IPL3 actually jumps to (some boot codes relocate the header value)."""
        return _u32(self.header.entry + CIC_ENTRY_ADJUST.get(self.cic or "", 0))

    def checksum_ok(self) -> bool | None:
        if self.cic is None:
            return None
        return compute_checksum(self.data, self.cic) == (self.header.crc1, self.header.crc2)

    def u32(self, offset: int) -> int:
        return struct.unpack_from(">I", self.data, offset)[0]

    def u16(self, offset: int) -> int:
        return struct.unpack_from(">H", self.data, offset)[0]

    def words(self, offset: int, count: int) -> tuple[int, ...]:
        return struct.unpack_from(f">{count}I", self.data, offset)

    def summary(self) -> dict:
        h = self.header
        return {"title": h.title, "game_code": h.game_code, "version": h.version,
                "sha1": self.sha1, "size": self.size, "byte_order": self.source_order,
                "cic": self.cic, "checksum_ok": self.checksum_ok(),
                "entry_header": f"0x{h.entry:08X}", "entry": f"0x{self.entry_vram:08X}",
                "crc": f"{h.crc1:08X}/{h.crc2:08X}"}
