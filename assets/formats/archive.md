# Resource archives

Two archives in the retail ROM:

| archive | ROM range | records | rlz-coded | cart base used by code |
|---|---|---|---|---|
| resource_archive_1 | 0x21AC00..0x4DB6D0 | 380 | 0 | `D_80052EB0[4]` = 0xB021AC00 (index read into `D_80058C48`) |
| resource_archive_2 | 0x4DB6D0..0xF4D840 | 531 | 148 | `D_80052EB0[0]` = 0xB04DB6D0 (index read into `D_80058C38` by `func_80012FB0`) |

## Index (big-endian)

| offset | type | field |
|---|---|---|
| 0x00 | u32 | record count |
| 0x04 | u32 | reserved (0 in both archives) |
| 0x08 + 12*i | u32 | payload offset from archive start (16-byte aligned) |
| +4 | u32 | logical (decoded) size |
| +8 | u32 | stored size; 0 = stored directly (`logical` bytes), otherwise an rlz stream of this many bytes |

Readers: `func_8001313C` / `func_80013238` (direct DMA or DMA into the staging buffer `D_80052EC8`, then `func_800166F0` decode), `func_8001361C` (archive 1 cart address).

## Fill

Bytes between the index end, payloads and the archive end are alignment fill. They are kept verbatim in
`archive.json` (`fills`), so the rebuild is exact even where the fill is not zero.

## Tools

* `tools/archive.py ROM OUTDIR` extracts every record (decoded payload) plus `archive.json`, rebuilds both
  archives from those files (re-encoding rlz records with `tools/rlz.py`), and compares each record and each
  whole archive with the ROM.
* `tests/test_archive.py` checks the codec, every compressed record re-encoding, and both full archives.
