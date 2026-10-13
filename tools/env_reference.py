import argparse
import re
import sys
from pathlib import Path

PREFIXES = ("APS5_",)
DOC = "docs/dev/ENVIRONMENT.md"
ROOTS = ("core",)
EXTENSIONS = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx", ".inl", ".ipp", ".py"}
SKIP = {"build", "build-linux", "build-relinker", "3rdparty", ".git"}


def pattern(prefixes):
    name = "(?:" + "|".join(re.escape(prefix) for prefix in prefixes) + ")[A-Za-z0-9_]+"
    return re.compile(
        r"(?:std::)?getenv\(\s*\"(" + name + r")\"\s*\)"
        r"|IsEnvironmentSet\(\s*\"(" + name + r")\"\s*\)"
        r"|kibSetting\(\s*\"(" + name + r")\""
        r"|MaskFromEnvironment\(\s*\"(" + name + r")\"\s*\)"
    )


def documented_pattern(prefixes):
    name = "(?:" + "|".join(re.escape(prefix) for prefix in prefixes) + ")[A-Za-z0-9_]+"
    return re.compile(r"`(" + name + r")`")


def source_files(root):
    root = Path(root)
    if not root.is_dir():
        return
    for path in sorted(root.rglob("*")):
        if path.suffix not in EXTENSIONS or any(part in SKIP for part in path.parts):
            continue
        yield path


def reads(roots, prefixes=PREFIXES):
    read = pattern(prefixes)
    found = {}
    for root in roots:
        for path in source_files(root):
            try:
                text = path.read_text(encoding="utf-8")
            except (OSError, UnicodeDecodeError):
                continue
            for number, line in enumerate(text.splitlines(), 1):
                for match in read.finditer(line):
                    name = next(group for group in match.groups() if group)
                    found.setdefault(name, []).append((path.as_posix(), number))
    for sites in found.values():
        sites.sort()
    return found


def documented(path, prefixes=PREFIXES):
    try:
        return set(documented_pattern(prefixes).findall(Path(path).read_text(encoding="utf-8")))
    except (OSError, UnicodeDecodeError):
        return None


def list_names(found, out):
    for name in sorted(found):
        for path, number in found[name]:
            out.write(f"{name}\t{path}:{number}\n")


def check(found, doc, prefixes, out):
    names = documented(doc, prefixes)
    if names is None:
        out.write(f"error: cannot read {doc}\n")
        return 1
    missing = sorted(set(found) - names)
    stale = sorted(names - set(found))
    for name in missing:
        path, number = found[name][0]
        out.write(f"error: {name} is read at {path}:{number} but has no row in {doc}\n")
    for name in stale:
        out.write(f"error: {name} has a row in {doc} but the code no longer reads it\n")
    if missing or stale:
        out.write(f"{len(missing)} name(s) undocumented, {len(stale)} stale\n")
        return 1
    out.write(f"{len(found)} name(s), all documented\n")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(
        description="list the environment names the code reads, or check them against a reference document")
    parser.add_argument("--check", action="store_true",
                        help="compare the names the code reads with the document and fail on a mismatch")
    parser.add_argument("--doc", default=DOC, help="reference document to check (default: %(default)s)")
    parser.add_argument("--root", action="append", default=None,
                        help="directory to scan (default: %s); repeatable" % ", ".join(ROOTS))
    parser.add_argument("--prefix", action="append", default=None,
                        help="name prefix to scan (default: %s); repeatable" % ", ".join(PREFIXES))
    args = parser.parse_args(argv)
    roots = args.root if args.root else ROOTS
    prefixes = tuple(args.prefix) if args.prefix else PREFIXES
    found = reads(roots, prefixes)
    if args.check:
        return check(found, args.doc, prefixes, sys.stdout)
    list_names(found, sys.stdout)
    return 0


if __name__ == "__main__":
    sys.exit(main())
