import argparse
import hashlib
import json
import re
import subprocess
import sys
import tomllib
from pathlib import Path, PurePosixPath


SHA256 = re.compile(r"[0-9a-f]{64}\Z")


def confined(root, name):
    if not isinstance(name, str) or not name or "\\" in name:
        raise ValueError(f"invalid relative path: {name!r}")
    path = PurePosixPath(name)
    if path.is_absolute() or any(part in (".", "..") for part in name.split("/")):
        raise ValueError(f"invalid relative path: {name!r}")
    resolved = (root / Path(*path.parts)).resolve()
    if not resolved.is_relative_to(root.resolve()) or not resolved.is_file():
        raise ValueError(f"file missing or outside directory: {name}")
    return resolved


def sha256(path):
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for chunk in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def load_catalog(directory, title, version):
    data = json.loads(confined(directory, "index.json").read_text(encoding="utf-8"))
    if not isinstance(data, dict) or set(data) != {"format", "entries"} or type(data["format"]) is not int or data["format"] != 1 or not isinstance(data["entries"], list):
        raise ValueError("catalog must have format 1 and an entries array")
    entries = []
    for number, entry in enumerate(data["entries"], 1):
        if not isinstance(entry, dict) or (entry.get("titleId"), entry.get("contentVersion")) != (title, version):
            continue
        if set(entry) != {"titleId", "contentVersion", "patch", "targets"}:
            raise ValueError(f"entry {number}: invalid fields")
        if any(not isinstance(entry[key], str) or not entry[key] for key in ("titleId", "contentVersion")):
            raise ValueError(f"entry {number}: invalid title or version")
        patch = confined(directory, entry["patch"])
        targets = entry["targets"]
        if not isinstance(targets, dict) or not targets:
            raise ValueError(f"entry {number}: targets must be a nonempty object")
        if any(not isinstance(name, str) or not isinstance(digest, str) or not SHA256.fullmatch(digest) for name, digest in targets.items()):
            raise ValueError(f"entry {number}: invalid target digest")
        parsed = tomllib.loads(patch.read_text(encoding="utf-8"))
        patches = parsed.get("patch")
        if not isinstance(patches, list) or not patches or any(not isinstance(item, dict) or not isinstance(item.get("target"), str) for item in patches):
            raise ValueError(f"entry {number}: patch file has no valid targets")
        if set(targets) != {item["target"] for item in patches}:
            raise ValueError(f"entry {number}: target hashes must cover exactly the patch targets")
        entries.append((number, entry, patch))
    return entries


def main(argv=None):
    parser = argparse.ArgumentParser(description="Recommend a local patch only after metadata, hashes and relinker verification match")
    parser.add_argument("--catalog", required=True, type=Path)
    parser.add_argument("--param-json", required=True, type=Path)
    parser.add_argument("--input", required=True, type=Path)
    parser.add_argument("--relinker", required=True, type=Path)
    parser.add_argument("--sce-module-path", type=Path)
    parser.add_argument("--module-dir", action="append", default=[])
    parser.add_argument("--executable", action="append", default=[])
    parser.add_argument("--skip-sce-module", action="store_true")
    parser.add_argument("--timeout-seconds", type=int, default=120)
    args = parser.parse_args(argv)
    try:
        metadata = json.loads(args.param_json.read_text(encoding="utf-8"))
        if not isinstance(metadata, dict):
            raise ValueError("param.json must be an object")
        title = metadata.get("titleId")
        version = metadata.get("contentVersion")
        if not isinstance(title, str) or not title or not isinstance(version, str) or not version:
            raise ValueError("param.json needs nonempty titleId and contentVersion")
        if not args.input.is_file() or not args.relinker.is_file():
            raise ValueError("input and relinker must be files")
        if args.timeout_seconds <= 0:
            raise ValueError("timeout-seconds must be positive")
        root = (args.sce_module_path or args.input.parent).resolve()
        if not root.is_dir():
            raise ValueError("module parent must be a directory")
        for executable in args.executable:
            confined(root, executable)
        matches = []
        for number, entry, patch in load_catalog(args.catalog.resolve(), title, version):
            errors = []
            for name, expected in entry["targets"].items():
                try:
                    source = args.input.resolve() if name == args.input.name else confined(root, name)
                    actual = sha256(source)
                    if actual != expected:
                        errors.append(f"{name}: SHA-256 mismatch ({actual})")
                except (OSError, ValueError) as error:
                    errors.append(f"{name}: {error}")
            if errors:
                print(f"Catalog entry {number} ({entry['patch']}): rejected: {'; '.join(errors)}")
                continue
            command = [str(args.relinker), "--verify-patches", str(patch)]
            if args.skip_sce_module:
                command.append("--skip-sce-module")
            if args.sce_module_path:
                command.extend(("--sce-module-path", str(args.sce_module_path)))
            for module_dir in args.module_dir:
                command.extend(("--module-dir", module_dir))
            for executable in args.executable:
                command.extend(("--executable", executable))
            command.append(str(args.input))
            try:
                result = subprocess.run(command, capture_output=True, text=True, timeout=args.timeout_seconds)
            except subprocess.TimeoutExpired:
                raise ValueError(f"catalog entry {number}: relinker verification exceeded {args.timeout_seconds} seconds") from None
            if result.returncode:
                detail = (result.stdout + result.stderr).strip()
                print(f"Catalog entry {number} ({entry['patch']}): rejected by relinker: {detail}")
                continue
            matches.append((number, entry, patch))
        if len(matches) == 1:
            number, entry, patch = matches[0]
            print(f"Recommended: {patch} (catalog entry {number})")
            for name, digest in entry["targets"].items():
                print(f"  {name} SHA-256 {digest}")
            print(f"Use --patches {patch} explicitly when converting.")
            return 0
        if len(matches) > 1:
            print("No recommendation: multiple verified catalog entries match.")
            return 1
        print("No recommendation: no catalog entry passed all checks.")
        return 1
    except (OSError, ValueError, json.JSONDecodeError, tomllib.TOMLDecodeError) as error:
        print(f"Patch advisor: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
