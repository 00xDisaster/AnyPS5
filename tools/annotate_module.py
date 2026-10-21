#!/usr/bin/env python3
import argparse
import re
import shutil
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import nid_names

PT_LOAD = 1
PT_DYNAMIC = 2
PF_X = 1
PF_W = 2
DT_NULL = 0
DT_HASH = 4
DT_STRTAB = 5
DT_SYMTAB = 6
DT_RELA = 7
DT_RELASZ = 8
DT_JMPREL = 23
DT_PLTRELSZ = 2
DT_GNU_HASH = 0x6ffffef5
SYMBOL_SIZE = 24
RELA_SIZE = 24

LINE_ADDRESS = re.compile(r"^\s*([0-9a-f]+):")
STUB = re.compile(r"^\s*([0-9a-f]+):\s+jmp\s+\*(?:-?0x[0-9a-f]+)?\(%rip\)\s+# 0x([0-9a-f]+)")
BRANCH = re.compile(r"\b(?:call|callq|jmp|jmpq)\s+0x([0-9a-f]+)")
REFERENCE = re.compile(r"# 0x([0-9a-f]+)")


class ModuleError(Exception):
    pass


class Module:
    def __init__(self, data):
        self.data = data
        self.loads = []
        self.dynamic = None
        self.read_headers()
        self.tags = self.read_tags()
        self.strtab = self.address(self.require_tag(DT_STRTAB, "DT_STRTAB"))
        self.symtab = self.address(self.require_tag(DT_SYMTAB, "DT_SYMTAB"))
        self.symbol_count = self.count_symbols()

    def unpack(self, form, offset):
        size = struct.calcsize(form)
        if offset < 0 or offset + size > len(self.data):
            raise ModuleError("read of %d bytes at file offset %#x is outside the file" % (size, offset))
        return struct.unpack_from(form, self.data, offset)

    def read_headers(self):
        if len(self.data) < 64 or self.data[:4] != b"\x7fELF":
            raise ModuleError("not an ELF file")
        if self.data[4] != 2 or self.data[5] != 1:
            raise ModuleError("not a 64-bit little-endian ELF")
        if self.unpack("<H", 0x12)[0] != 0x3E:
            raise ModuleError("not an x86-64 ELF")
        phoff, = self.unpack("<Q", 0x20)
        phentsize, phnum = self.unpack("<HH", 0x36)
        for index in range(phnum):
            kind, flags, offset, vaddr, _, filesz, _, _ = self.unpack("<IIQQQQQQ", phoff + index * phentsize)
            if kind == PT_LOAD:
                self.loads.append((vaddr, offset, filesz, flags))
            elif kind == PT_DYNAMIC and self.dynamic is None:
                self.dynamic = (offset, filesz)
        if self.dynamic is None:
            raise ModuleError("no PT_DYNAMIC program header")

    def read_tags(self):
        tags = {}
        offset, size = self.dynamic
        for position in range(offset, offset + size, 16):
            tag, value = self.unpack("<qQ", position)
            if tag == DT_NULL:
                break
            tags.setdefault(tag, value)
        return tags

    def require_tag(self, tag, name):
        if tag not in self.tags:
            raise ModuleError("the dynamic section has no %s" % name)
        return self.tags[tag]

    def address(self, vaddr):
        for start, offset, size, _ in self.loads:
            if start <= vaddr < start + size:
                return offset + vaddr - start
        raise ModuleError("address %#x is outside every PT_LOAD segment" % vaddr)

    def count_symbols(self):
        if DT_HASH in self.tags:
            return self.unpack("<I", self.address(self.tags[DT_HASH]) + 4)[0]
        if DT_GNU_HASH in self.tags:
            return self.count_gnu_symbols(self.address(self.tags[DT_GNU_HASH]))
        raise ModuleError("the dynamic section has neither DT_HASH nor DT_GNU_HASH, so the symbol count is unknown")

    def count_gnu_symbols(self, table):
        buckets, symoffset, bloom_size, _ = self.unpack("<IIII", table)
        buckets_at = table + 16 + bloom_size * 8
        chains_at = buckets_at + buckets * 4
        last = max((self.unpack("<I", buckets_at + 4 * index)[0] for index in range(buckets)), default=0)
        if last < symoffset:
            return symoffset
        while not self.unpack("<I", chains_at + 4 * (last - symoffset))[0] & 1:
            last += 1
        return last + 1

    def symbol(self, index):
        if index >= self.symbol_count:
            raise ModuleError("symbol index %d is outside the %d symbols of the module" % (index, self.symbol_count))
        name, _, _, shndx, value, _ = self.unpack("<IBBHQQ", self.symtab + index * SYMBOL_SIZE)
        start = self.strtab + name
        end = self.data.find(b"\0", start)
        if end < 0:
            raise ModuleError("symbol %d has an unterminated name" % index)
        return self.data[start:end].decode("utf-8", errors="replace"), shndx, value

    def relocation_tables(self):
        yield self.tags.get(DT_RELA), self.tags.get(DT_RELASZ, 0)
        yield self.tags.get(DT_JMPREL), self.tags.get(DT_PLTRELSZ, 0)

    def imports(self):
        slots = {}
        for table, size in self.relocation_tables():
            if table is None:
                continue
            start = self.address(table)
            for position in range(start, start + size - size % RELA_SIZE, RELA_SIZE):
                offset, info, _ = self.unpack("<QQq", position)
                if info >> 32:
                    slots[offset] = self.symbol(info >> 32)[0]
        return slots

    def exports(self):
        found = {}
        for index in range(1, self.symbol_count):
            name, shndx, value = self.symbol(index)
            if shndx != 0 and value and name:
                found.setdefault(value, name)
        return found

    def code_segment(self):
        for vaddr, offset, size, flags in self.loads:
            if flags & PF_X and not flags & PF_W:
                if offset + size > len(self.data):
                    raise ModuleError("the executable PT_LOAD segment extends past the end of the file")
                return vaddr, self.data[offset:offset + size]
        raise ModuleError("no executable, non-writable PT_LOAD segment")


class Namer:
    def __init__(self, db):
        self.db = db

    def resolve(self, symbol):
        nid = symbol.split("#", 1)[0]
        name = nid_names.strip_nid_postfix(self.db.get(nid, ""))
        return name if name and nid_names.compute_nid(name) == nid else nid


def disassemble(code, vaddr):
    with tempfile.TemporaryDirectory() as directory:
        blob = Path(directory) / "text.bin"
        blob.write_bytes(code)
        command = ["objdump", "-D", "-b", "binary", "-m", "i386:x86-64", "--no-show-raw-insn",
                   "--adjust-vma=%d" % vaddr, str(blob)]
        result = subprocess.run(command, capture_output=True, text=True, encoding="utf-8", errors="replace")
    if result.returncode != 0:
        raise ModuleError("objdump failed: %s" % result.stderr.strip())
    return result.stdout.splitlines()


def find_stubs(listing, slots, namer):
    stubs = {}
    for line in listing:
        match = STUB.match(line)
        if match and int(match.group(2), 16) in slots:
            stubs[int(match.group(1), 16)] = namer.resolve(slots[int(match.group(2), 16)])
    return stubs


def line_note(line, slots, stubs, namer):
    note = ""
    branch = BRANCH.search(line)
    if branch and int(branch.group(1), 16) in stubs:
        note = "   ; -> " + stubs[int(branch.group(1), 16)]
    reference = REFERENCE.search(line)
    if reference and int(reference.group(1), 16) in slots:
        note = "   ; GOT " + namer.resolve(slots[int(reference.group(1), 16)])
    return note


def annotate_listing(listing, slots, exports, stubs, namer):
    out = []
    for line in listing:
        head = LINE_ADDRESS.match(line)
        if head and int(head.group(1), 16) in exports:
            out.append("")
            out.append("; ===== export " + namer.resolve(exports[int(head.group(1), 16)]))
        out.append(line + line_note(line, slots, stubs, namer))
    return out


def write_lines(path, lines):
    with open(path, "w", encoding="utf-8", newline="\n") as handle:
        for line in lines:
            handle.write(line + "\n")


def annotate(module_path, prefix, namer):
    module = Module(Path(module_path).read_bytes())
    slots = module.imports()
    exports = module.exports()
    vaddr, code = module.code_segment()
    listing = disassemble(code, vaddr)
    stubs = find_stubs(listing, slots, namer)
    write_lines(prefix + ".imports.txt", ["%#x %s %s" % (slot, slots[slot], namer.resolve(slots[slot])) for slot in sorted(slots)])
    write_lines(prefix + ".exports.txt", ["%#x %s %s" % (address, exports[address], namer.resolve(exports[address])) for address in sorted(exports)])
    write_lines(prefix + ".asm", annotate_listing(listing, slots, exports, stubs, namer))
    return "%s: %d imports, %d exports, %d stubs, %d lines" % (prefix, len(slots), len(exports), len(stubs), len(listing))


def parse_arguments(argv):
    parser = argparse.ArgumentParser(
        description="Annotated disassembly of a relinked guest module (no section headers). Writes PREFIX.imports.txt, "
                    "PREFIX.exports.txt and PREFIX.asm, with imports, PLT stubs, GOT references and exports named "
                    "through the NID database. Requires binutils objdump.")
    parser.add_argument("module", help="relinked guest module, for example libc.prx.guest.prx")
    parser.add_argument("--out", help="output prefix (default: the module path)")
    parser.add_argument("--db", help="NID database CSV (downloaded to a cache dir by default)")
    parser.add_argument("--no-names", action="store_true", help="print NIDs without loading the database")
    return parser.parse_args(argv)


def main(argv=None):
    args = parse_arguments(sys.argv[1:] if argv is None else argv)
    if shutil.which("objdump") is None:
        sys.exit("binutils objdump is required but was not found in PATH")
    try:
        db = {} if args.no_names else nid_names.load_db(Path(args.db) if args.db else nid_names.DEFAULT_CACHE)
        print(annotate(args.module, args.out or args.module, Namer(db)))
    except (ModuleError, OSError, UnicodeError) as error:
        sys.exit("%s: %s" % (args.module, error))
    return 0


if __name__ == "__main__":
    sys.exit(main())
