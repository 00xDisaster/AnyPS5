#!/usr/bin/env python3
"""Build an AnyPS5 shader-cache pack from loose cache entries.

The loader maps ``shader_cache/prebuilt`` and serves exact-key hits from it
before it consults the loose per-source-version directory.  A pack is one
sequential file instead of thousands of small reads, which matters on a
spinning disk.  Entry names already contain the two 64-bit key hashes, so the
builder only needs the stored key to validate the name and the stored payload;
it does not need to decode shader artifacts.
"""

from __future__ import annotations

import argparse
import os
import struct
import sys
from dataclasses import dataclass
from pathlib import Path

PACK_MAGIC = 0x314B434150535041
ENTRY_MAGIC = 0x43535041
FORMAT_VERSION = 44
HEADER = struct.Struct("<5Q")
RECORD = struct.Struct("<4Q")
ENTRY_HEADER = struct.Struct("<2I5Q")
MASK64 = (1 << 64) - 1
MULT = 0x9E3779B97F4A7C15
MIX1 = 0xFF51AFD7ED558CCD
MIX2 = 0xC4CEB9FE1A85EC53


class PackError(Exception):
    pass


def hash_bytes(data: bytes, seed: int = 0) -> int:
    """Match core/shader/recompiler/ShaderCacheDirectory.cpp's HashBytes."""
    value = (seed ^ ((len(data) * MULT) & MASK64)) & MASK64

    def mix(chunk: int) -> None:
        nonlocal value
        chunk = (chunk * MIX1) & MASK64
        chunk ^= chunk >> 32
        value = ((value ^ chunk) * MULT) & MASK64
        value = ((value << 27) | (value >> 37)) & MASK64

    whole = len(data) - (len(data) % 8)
    for offset in range(0, whole, 8):
        mix(int.from_bytes(data[offset:offset + 8], "little"))
    if whole != len(data):
        mix(int.from_bytes(data[whole:], "little") | (1 << 63))
    value ^= value >> 33
    value = (value * MIX1) & MASK64
    value ^= value >> 33
    value = (value * MIX2) & MASK64
    value ^= value >> 33
    return value


def key_hashes(key: bytes) -> tuple[int, int]:
    return hash_bytes(key, 0x5EED0001), hash_bytes(key, 0x5EED0002)


@dataclass(frozen=True)
class Entry:
    name: str
    path: Path
    first: int
    second: int
    size: int
    source_version: int
    payload_offset: int
    payload_hash: int


def parse_entry(path: Path) -> Entry:
    with path.open("rb") as source:
        header = source.read(ENTRY_HEADER.size)
        if len(header) != ENTRY_HEADER.size:
            raise PackError(f"{path}: short shader-cache header")
        magic, fmt, version, key_bytes, payload_bytes, key_hash, payload_hash = ENTRY_HEADER.unpack(header)
        if magic != ENTRY_MAGIC:
            raise PackError(f"{path}: not a shader-cache entry")
        if fmt != FORMAT_VERSION:
            raise PackError(f"{path}: entry format {fmt}, expected {FORMAT_VERSION}")
        if key_bytes < 20:
            raise PackError(f"{path}: key is only {key_bytes} bytes")
        key = source.read(key_bytes)
        if len(key) != key_bytes:
            raise PackError(f"{path}: truncated key")
    payload_offset = (ENTRY_HEADER.size + key_bytes + 7) & ~7
    expected = payload_offset + payload_bytes
    actual = path.stat().st_size
    if actual != expected:
        raise PackError(f"{path}: size {actual} does not match header size {expected}")
    if hash_bytes(key) != key_hash:
        raise PackError(f"{path}: stored key hash does not match the key")
    first, second = key_hashes(key)
    if path.name != f"{first:016x}{second:016x}.bin":
        raise PackError(f"{path}: key hashes do not match the file name")
    return Entry(path.name, path, first, second, actual, version, payload_offset, payload_hash)


def find_entries(cache_directory: Path) -> list[Entry]:
    entries = []
    for path in cache_directory.iterdir():
        if not path.is_file() or len(path.name) != 36 or path.suffix != ".bin":
            continue
        if any(character not in "0123456789abcdef" for character in path.stem):
            continue
        entries.append(parse_entry(path))
    entries.sort(key=lambda entry: entry.name)
    return entries


def build_pack(cache_directory: Path, output: Path, force: bool = False) -> tuple[int, int, int]:
    """Write one pack atomically, returning (entries, bytes, source version)."""
    if not cache_directory.is_dir():
        raise PackError(f"{cache_directory}: not a directory")
    if output.exists() and not force:
        raise PackError(f"{output}: exists; pass --force to replace it")
    entries = find_entries(cache_directory)
    if not entries:
        raise PackError(f"{cache_directory}: no .bin shader-cache entries")
    source_version = entries[0].source_version
    for entry in entries[1:]:
        if entry.source_version != source_version:
            raise PackError(f"{entry.path}: mixed source versions in one cache directory")

    entries.sort(key=lambda entry: (entry.first, entry.second))
    offset = HEADER.size + len(entries) * RECORD.size
    index = bytearray()
    for entry in entries:
        index += RECORD.pack(entry.first, entry.second, offset, entry.size)
        offset += entry.size
    pack_size = offset
    index_hash = hash_bytes(bytes(index))
    header = HEADER.pack(PACK_MAGIC, source_version, pack_size, len(entries), index_hash)

    temporary = output.with_name(output.name + f".tmp-{os.getpid()}")
    written = 0
    try:
        with temporary.open("wb", buffering=8 << 20) as target:
            target.write(header)
            target.write(index)
            for number, entry in enumerate(entries, 1):
                data = entry.path.read_bytes()
                if len(data) != entry.size or hash_bytes(data[entry.payload_offset:]) != entry.payload_hash:
                    raise PackError(f"{entry.path}: payload changed or is corrupt while packing")
                target.write(data)
                written += len(data)
                if number % 512 == 0 or number == len(entries):
                    print(f"pack {number}/{len(entries)} entries, {written / (1024 * 1024):.0f} MiB", file=sys.stderr, flush=True)
            target.flush()
            os.fsync(target.fileno())
        os.replace(temporary, output)
    except BaseException:
        temporary.unlink(missing_ok=True)
        raise
    return len(entries), pack_size, source_version


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("cache_directory", type=Path, help="source-version directory containing .bin entries")
    parser.add_argument("output", type=Path, nargs="?", help="pack path (default: <cache parent>/prebuilt)")
    parser.add_argument("--force", action="store_true", help="replace an existing pack")
    arguments = parser.parse_args()
    output = arguments.output or arguments.cache_directory.parent / "prebuilt"
    try:
        entries, size, source_version = build_pack(arguments.cache_directory, output, arguments.force)
    except (OSError, PackError) as error:
        print(f"shader_cache_pack: {error}", file=sys.stderr)
        return 1
    print(f"wrote {output}: {entries} entries, {size} bytes, source version {source_version:016x}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
