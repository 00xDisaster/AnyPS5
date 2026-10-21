import contextlib
import io
import shutil
import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import annotate_module
import nid_names

IMPORT_NAME = "sceKernelGetProcessTime"
EXPORT_NAME = "sceKernelUsleep"
CODE_ADDRESS = 0x1000
DATA_ADDRESS = 0x2000
SLOT = 0x2800
CALL_ADDRESS = 0x1006
EXPORT_ADDRESS = 0x100B


def code_bytes():
    stub = b"\xff\x25" + struct.pack("<i", SLOT - (CODE_ADDRESS + 6))
    call = b"\xe8" + struct.pack("<i", CODE_ADDRESS - (CALL_ADDRESS + 5))
    return stub + call + b"\xc3"


def data_bytes(tags):
    strings = b"\0%s#A#A\0%s#A#A\0" % (nid_names.compute_nid(IMPORT_NAME).encode(), nid_names.compute_nid(EXPORT_NAME).encode())
    symbols = struct.pack("<IBBHQQ", 0, 0, 0, 0, 0, 0)
    symbols += struct.pack("<IBBHQQ", 1, 0x12, 0, 0, 0, 0)
    symbols += struct.pack("<IBBHQQ", 1 + (len(strings) - 1) // 2, 0x12, 0, 1, EXPORT_ADDRESS, 1)
    hash_table = struct.pack("<IIIIII", 1, 3, 0, 0, 0, 0)
    relocation = struct.pack("<QQq", SLOT, (1 << 32) | 7, 0)
    values = {4: DATA_ADDRESS, 5: DATA_ADDRESS + 0x60, 6: DATA_ADDRESS + 0x18, 10: len(strings), 11: 24,
              23: DATA_ADDRESS + 0x100, 2: 24, 20: 7}
    dynamic = b"".join(struct.pack("<qQ", tag, values[tag]) for tag in tags) + struct.pack("<qQ", 0, 0)
    image = bytearray(0x808)
    for offset, blob in ((0, hash_table), (0x18, symbols), (0x60, strings), (0x100, relocation), (0x200, dynamic)):
        image[offset:offset + len(blob)] = blob
    return bytes(image)


def build_module(tags=(4, 5, 6, 10, 11, 23, 2, 20), executable=True, machine=0x3E):
    code_flags = 5 if executable else 6
    headers = [(1, code_flags, 0x1000, CODE_ADDRESS, len(code_bytes())),
               (1, 6, 0x2000, DATA_ADDRESS, 0x808),
               (2, 6, 0x2200, DATA_ADDRESS + 0x200, 16 * (len(tags) + 1))]
    image = bytearray(0x2808)
    image[:64] = struct.pack("<16sHHIQQQIHHHHHH", b"\x7fELF\x02\x01\x01" + bytes(9), 3, machine, 1, 0, 64, 0, 0, 64, 56, len(headers), 0, 0, 0)
    for index, (kind, flags, offset, vaddr, size) in enumerate(headers):
        image[64 + index * 56:120 + index * 56] = struct.pack("<IIQQQQQQ", kind, flags, offset, vaddr, vaddr, size, size, 0x1000)
    image[0x1000:0x1000 + len(code_bytes())] = code_bytes()
    image[0x2000:0x2808] = data_bytes(tags)
    return bytes(image)


class AnnotateModuleTests(unittest.TestCase):
    def setUp(self):
        self.assertIsNotNone(shutil.which("objdump"), "binutils objdump is required to run these tests")
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        self.database = self.root / "db.csv"
        self.database.write_text("%s %s#A#A\n%s %s\n" % (
            nid_names.compute_nid(IMPORT_NAME), IMPORT_NAME, nid_names.compute_nid(EXPORT_NAME), EXPORT_NAME + "_nid_postfix"),
            encoding="utf-8")

    def run_tool(self, module, *extra):
        path = self.root / "module.prx"
        path.write_bytes(module)
        stdout = io.StringIO()
        with contextlib.redirect_stdout(stdout):
            annotate_module.main([str(path), "--out", str(self.root / "out")] + list(extra))
        return stdout.getvalue()

    def read(self, suffix):
        return (self.root / ("out" + suffix)).read_text(encoding="utf-8")

    def test_names_imports_stubs_calls_and_exports(self):
        summary = self.run_tool(build_module(), "--db", str(self.database))
        self.assertIn("1 imports, 1 exports, 1 stubs", summary)
        import_nid = nid_names.compute_nid(IMPORT_NAME)
        self.assertEqual("%#x %s#A#A %s\n" % (SLOT, import_nid, IMPORT_NAME), self.read(".imports.txt"))
        self.assertEqual("%#x %s#A#A %s\n" % (EXPORT_ADDRESS, nid_names.compute_nid(EXPORT_NAME), EXPORT_NAME), self.read(".exports.txt"))
        listing = self.read(".asm").splitlines()
        stub = next(line for line in listing if "jmp" in line)
        self.assertTrue(stub.endswith("; GOT " + IMPORT_NAME), stub)
        call = next(line for line in listing if "call" in line)
        self.assertTrue(call.endswith("; -> " + IMPORT_NAME), call)
        self.assertIn("; ===== export " + EXPORT_NAME, listing)
        self.assertLess(listing.index("; ===== export " + EXPORT_NAME), next(i for i, line in enumerate(listing) if "ret" in line))
        self.assertEqual([], [path.name for path in self.root.iterdir() if path.suffix == ".bin"])

    def test_unknown_nid_prints_the_nid(self):
        empty = self.root / "empty.csv"
        empty.write_text("", encoding="utf-8")
        self.run_tool(build_module(), "--db", str(empty))
        nid = nid_names.compute_nid(IMPORT_NAME)
        self.assertEqual("%#x %s#A#A %s\n" % (SLOT, nid, nid), self.read(".imports.txt"))
        self.assertIn("; -> " + nid, self.read(".asm"))

    def test_no_names_skips_the_database(self):
        self.run_tool(build_module(), "--no-names")
        self.assertIn("; -> " + nid_names.compute_nid(IMPORT_NAME), self.read(".asm"))

    def test_mismatching_database_name_is_not_used(self):
        wrong = self.root / "wrong.csv"
        wrong.write_text("%s sceKernelOther\n" % nid_names.compute_nid(IMPORT_NAME), encoding="utf-8")
        self.run_tool(build_module(), "--db", str(wrong))
        self.assertNotIn("sceKernelOther", self.read(".asm"))

    def test_missing_hash_fails(self):
        with self.assertRaises(SystemExit) as raised:
            self.run_tool(build_module(tags=(5, 6, 10, 11, 23, 2, 20)), "--no-names")
        self.assertIn("neither DT_HASH nor DT_GNU_HASH", str(raised.exception))

    def test_missing_symtab_fails(self):
        with self.assertRaises(SystemExit) as raised:
            self.run_tool(build_module(tags=(4, 5, 10, 11, 23, 2, 20)), "--no-names")
        self.assertIn("DT_SYMTAB", str(raised.exception))

    def test_no_executable_segment_fails(self):
        with self.assertRaises(SystemExit) as raised:
            self.run_tool(build_module(executable=False), "--no-names")
        self.assertIn("no executable, non-writable PT_LOAD", str(raised.exception))

    def test_wrong_machine_fails(self):
        with self.assertRaises(SystemExit) as raised:
            self.run_tool(build_module(machine=0xB7), "--no-names")
        self.assertIn("not an x86-64 ELF", str(raised.exception))


if __name__ == "__main__":
    unittest.main()
