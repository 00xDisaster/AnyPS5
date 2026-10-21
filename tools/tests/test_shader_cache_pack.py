import re
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import shader_cache_pack


def write_entry(directory: Path, key: bytes, payload: bytes, source_version: int = 0x0123456789abcdef) -> tuple[Path, bytes]:
    header = shader_cache_pack.ENTRY_HEADER.pack(
        shader_cache_pack.ENTRY_MAGIC,
        shader_cache_pack.FORMAT_VERSION,
        source_version,
        len(key),
        len(payload),
        shader_cache_pack.hash_bytes(key),
        shader_cache_pack.hash_bytes(payload),
    )
    data = bytearray(header)
    data.extend(key)
    data.extend(b"\0" * ((-len(data)) % 8))
    data.extend(payload)
    first, second = shader_cache_pack.key_hashes(key)
    path = directory / f"{first:016x}{second:016x}.bin"
    path.write_bytes(data)
    return path, bytes(data)


class ShaderCachePackTests(unittest.TestCase):
    def test_format_version_matches_loader(self):
        header = Path(__file__).resolve().parents[2] / "core/shader/recompiler/ShaderDiskCache.hpp"
        match = re.search(r"FormatVersion\s*=\s*(\d+)", header.read_text(encoding="utf-8"))
        self.assertIsNotNone(match)
        self.assertEqual(shader_cache_pack.FORMAT_VERSION, int(match.group(1)))

    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.directory = Path(self.temporary.name) / "0123456789abcdef"
        self.directory.mkdir()
        self.output = Path(self.temporary.name) / "prebuilt"

    def tearDown(self):
        self.temporary.cleanup()

    def test_builds_sorted_pack_with_valid_index(self):
        first, first_data = write_entry(self.directory, b"first-key-0123456789", b"first payload")
        second, second_data = write_entry(self.directory, b"second-key-012345678", b"second payload")
        entries, size, source_version = shader_cache_pack.build_pack(self.directory, self.output)
        self.assertEqual((entries, source_version), (2, 0x0123456789abcdef))
        raw = self.output.read_bytes()
        magic, packed_version, packed_size, count, index_hash = shader_cache_pack.HEADER.unpack_from(raw)
        self.assertEqual(magic, shader_cache_pack.PACK_MAGIC)
        self.assertEqual(packed_version, source_version)
        self.assertEqual(packed_size, size)
        self.assertEqual(count, 2)
        index = raw[shader_cache_pack.HEADER.size:shader_cache_pack.HEADER.size + 2 * shader_cache_pack.RECORD.size]
        self.assertEqual(index_hash, shader_cache_pack.hash_bytes(index))
        records = [shader_cache_pack.RECORD.unpack_from(index, offset) for offset in (0, shader_cache_pack.RECORD.size)]
        self.assertEqual(records, sorted(records))
        stored = {}
        for record_first, record_second, offset, record_size in records:
            stored[(record_first, record_second)] = raw[offset:offset + record_size]
        self.assertEqual(stored[shader_cache_pack.key_hashes(b"first-key-0123456789")], first_data)
        self.assertEqual(stored[shader_cache_pack.key_hashes(b"second-key-012345678")], second_data)
        self.assertTrue(first.exists() and second.exists())

    def test_refuses_overwrite_without_force(self):
        write_entry(self.directory, b"key-0123456789abcdef", b"payload")
        self.output.write_bytes(b"existing")
        with self.assertRaises(shader_cache_pack.PackError):
            shader_cache_pack.build_pack(self.directory, self.output)
        self.assertEqual(self.output.read_bytes(), b"existing")
        shader_cache_pack.build_pack(self.directory, self.output, force=True)
        self.assertNotEqual(self.output.read_bytes(), b"existing")

    def test_rejects_corrupt_payload(self):
        path, _ = write_entry(self.directory, b"key-0123456789abcdef", b"payload")
        data = bytearray(path.read_bytes())
        data[-1] ^= 0x40
        path.write_bytes(data)
        with self.assertRaises(shader_cache_pack.PackError):
            shader_cache_pack.build_pack(self.directory, self.output)
        self.assertFalse(self.output.exists())

    def test_rejects_mixed_source_versions(self):
        write_entry(self.directory, b"one-0123456789abcdef", b"payload", 1)
        write_entry(self.directory, b"two-0123456789abcdef", b"payload", 2)
        with self.assertRaises(shader_cache_pack.PackError):
            shader_cache_pack.build_pack(self.directory, self.output)

    def test_rejects_name_that_does_not_match_key(self):
        path, _ = write_entry(self.directory, b"key-0123456789abcdef", b"payload")
        wrong = self.directory / ("0" * 32 + ".bin")
        path.rename(wrong)
        with self.assertRaises(shader_cache_pack.PackError):
            shader_cache_pack.build_pack(self.directory, self.output)


if __name__ == "__main__":
    unittest.main()
