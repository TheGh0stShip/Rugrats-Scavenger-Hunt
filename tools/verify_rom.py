#!/usr/bin/env python3
"""Verify the exact retail input without modifying it."""

from __future__ import annotations

import hashlib
import pathlib
import sys

EXPECTED_SHA1 = "fb2a62f1625630d6f0beb5fd00a32e12155d50e8"
EXPECTED_SIZE = 0x1000000
EXPECTED_MAGIC = bytes.fromhex("80371240")


def main() -> int:
    if len(sys.argv) != 2:
        print(f"usage: {sys.argv[0]} ROM", file=sys.stderr)
        return 2
    path = pathlib.Path(sys.argv[1])
    try:
        data = path.read_bytes()
    except OSError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    digest = hashlib.sha1(data).hexdigest()
    errors = []
    if len(data) != EXPECTED_SIZE:
        errors.append(f"size {len(data):#x}, expected {EXPECTED_SIZE:#x}")
    if data[:4] != EXPECTED_MAGIC:
        errors.append(f"magic {data[:4].hex()}, expected {EXPECTED_MAGIC.hex()}")
    if digest != EXPECTED_SHA1:
        errors.append(f"SHA-1 {digest}, expected {EXPECTED_SHA1}")
    if errors:
        print("invalid Rugrats USA ROM: " + "; ".join(errors), file=sys.stderr)
        return 1
    print(f"verified {path}: {digest}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
