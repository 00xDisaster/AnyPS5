import argparse
import concurrent.futures
import importlib.util
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
LIBS = ROOT / "core/libs"
sys.dont_write_bytecode = True
SPEC = importlib.util.spec_from_file_location("nid_names", ROOT / "tools/nid_names.py")
NIDS = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(NIDS)
DECLARATION = re.compile(r"(?:\bAPS5_VABI\s+|^[ \t]*[A-Za-z_][\w:*&<> \t]*?[ \t]+)(\w+)\s*\([^;{}]*\)\s*(?:noexcept\s*)?;", re.M)
INCLUDE = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.M)
TYPE_TOKEN = re.compile(r"\bnamespace\s*([\w:]*)\s*\{|\bextern\s*\{|\b(?:struct|union|class)\s+(\w+)\s*([;{])|\busing\s+(\w+)\s*=|[{}]")


def code(text):
    return NIDS.CPP_TOKEN.sub(lambda match: match.group() if match.group("code") else re.sub(r"[^\n]", " ", match.group()), text)


def declared_types(text):
    scopes = []
    result = set()
    for token in TYPE_TOKEN.finditer(text):
        namespace, name, ending, alias = token.groups()
        if namespace is not None:
            scopes.append(namespace or None)
        elif name or alias:
            if None not in scopes:
                result.add("::".join([scope for scope in scopes if scope] + [name or alias]))
            if ending == "{":
                scopes.append(name)
        elif token.group().startswith("extern"):
            scopes.append("")
        elif token.group() == "{":
            scopes.append(None)
        elif scopes:
            scopes.pop()
    return result


def reachable(header, visited):
    if header in visited:
        return
    visited.add(header)
    for name in INCLUDE.findall(header.read_text(encoding="utf-8")):
        candidates = {path.resolve() for path in (header.parent / name, LIBS / name) if path.is_file()}
        if len(candidates) != 1:
            raise RuntimeError(f"{header}: include {name} resolves to {len(candidates)} files")
        reachable(candidates.pop(), visited)


def audit():
    umbrellas = sorted((LIBS / "prx").glob("libSce*/libSce*.hpp"))
    if not umbrellas:
        raise RuntimeError("No library umbrella headers found")
    exports = {}
    for umbrella in umbrellas:
        included = set()
        reachable(umbrella.resolve(), included)
        for source in NIDS.source_files(umbrella.parent):
            text = source.read_text(encoding="utf-8")
            aliases = {name for nid, name in NIDS.source_aliases(text)}
            definitions = {match.group(1) for match in NIDS.source_definitions(text)}
            definitions.update(NIDS.source_assembly_definitions(text))
            for name in definitions | aliases:
                if "_nid_no_patch" in name and name not in aliases:
                    continue
                exports.setdefault(name, []).append((source, included))
    declarations = {name: [] for name in exports}
    for source in sorted((ROOT / "core").rglob("*")):
        if source.suffix not in {".cpp", ".h", ".hpp"}:
            continue
        text = code(source.read_text(encoding="utf-8"))
        for match in DECLARATION.finditer(text):
            prefix = match.group()[:match.start(1) - match.start()]
            if re.search(r"\b(?:return|co_return|throw|else|case)\b|&&|\|\|", prefix):
                continue
            if match.group(1) in declarations:
                declarations[match.group(1)].append((source.resolve(), text.count("\n", 0, match.start()) + 1))
    errors = []
    headers = set(umbrellas)
    for name, definitions in exports.items():
        found = declarations[name]
        if len(found) != 1 or found[0][0].suffix not in {".h", ".hpp"}:
            locations = ", ".join(f"{path.relative_to(ROOT)}:{line}" for path, line in found)
            errors.append(f"{name}: expected exactly one header declaration; found {len(found)}: {locations}")
            continue
        header = found[0][0]
        headers.add(header)
        for source, included in definitions:
            if header not in included:
                errors.append(f"{name}: {header.relative_to(ROOT)} is not included by {source.parent} library umbrella")
    if errors:
        raise RuntimeError("\n".join(errors))
    public_types = set()
    for header in headers:
        text = code(header.read_text(encoding="utf-8"))
        public_types.update(declared_types(text))
    for source in sorted((ROOT / "core").rglob("*.cpp")):
        text = code(source.read_text(encoding="utf-8"))
        types = declared_types(text)
        for name in sorted(public_types & types):
            errors.append(f"{source.relative_to(ROOT)}: duplicate public type {name}; include its header")
    if errors:
        raise RuntimeError("\n".join(errors))
    return umbrellas, sorted(headers), len(exports)


def compile_headers(compiler, umbrellas, headers):
    def compile_one(group):
        includes = "".join(f'#include "{header.relative_to(LIBS).as_posix()}"\n' for header in group)
        result = subprocess.run([compiler, "-std=c++20", "-Werror", "-fsyntax-only", "-x", "c++", f"-I{LIBS}", "-"], input=includes * 2, text=True, capture_output=True)
        if result.returncode:
            raise RuntimeError(f"Header compilation failed: {group}\n{result.stdout}{result.stderr}")

    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
        list(pool.map(compile_one, [[header] for header in headers] + [umbrellas]))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--compiler", required=True)
    args = parser.parse_args()
    umbrellas, headers, count = audit()
    compile_headers(args.compiler, umbrellas, headers)
    print(f"{len(umbrellas)} libraries, {count} exports: unique declarations, umbrella coverage and standalone headers verified")


if __name__ == "__main__":
    main()
