import argparse
import json
from pathlib import Path
import re
import subprocess
import sys


def read_reply(build_dir):
    reply_dir = build_dir / ".cmake" / "api" / "v1" / "reply"
    indexes = list(reply_dir.glob("index-*.json"))
    if not indexes:
        raise ValueError(f"{reply_dir}: no CMake File API index; configure with the codemodel-v2 query first")
    index = max(indexes, key=lambda path: path.stat().st_mtime_ns)
    document = json.loads(index.read_text(encoding="utf-8"))
    codemodel = document.get("reply", {}).get("codemodel-v2")
    if not codemodel:
        raise ValueError(f"{index}: missing codemodel-v2 reply")
    model = json.loads((reply_dir / codemodel["jsonFile"]).read_text(encoding="utf-8"))
    configurations = model.get("configurations", [])
    if len(configurations) != 1:
        raise ValueError(f"{index}: expected one CMake configuration, found {len(configurations)}")
    targets = {}
    for target in configurations[0].get("targets", []):
        targets[target["id"]] = json.loads((reply_dir / target["jsonFile"]).read_text(encoding="utf-8"))
    return Path(model["paths"]["source"]).resolve(), targets


def is_libc_link(name, target):
    if name == "libc" or name == "libs":
        return True
    link = target.get("link", {})
    fragments = link.get("commandFragments", [])
    command = " ".join(fragment.get("fragment", "") for fragment in fragments)
    return re.search(r"(?i)(?:^|[/\\\s])libc(?:\.prx|\.dll\.a)(?:$|\s)", command) is not None


def links_guest_libc(target, targets, visited=None):
    visited = set() if visited is None else visited
    target_id = target["id"]
    if target_id in visited:
        return False
    visited.add(target_id)
    if is_libc_link(target["name"], target):
        return True
    for dependency in target.get("dependencies", []):
        linked_target = targets.get(dependency["id"])
        if linked_target is not None and links_guest_libc(linked_target, targets, visited):
            return True
    return False


def has_product_library_dependency(target, targets, source_root, visited=None):
    visited = set() if visited is None else visited
    target_id = target["id"]
    if target_id in visited:
        return False
    visited.add(target_id)
    for dependency in target.get("dependencies", []):
        linked_target = targets.get(dependency["id"])
        if linked_target is None:
            continue
        if linked_target.get("type") in {"STATIC_LIBRARY", "SHARED_LIBRARY", "MODULE_LIBRARY"}:
            source_dir = (source_root / linked_target["paths"]["source"]).resolve()
            try:
                source_dir.relative_to(source_root / "core" / "libs")
                in_project_libs = True
            except ValueError:
                in_project_libs = False
            try:
                source_dir.relative_to(source_root / "core" / "libs" / "tests")
                in_test_tree = True
            except ValueError:
                in_test_tree = False
            if in_project_libs and not in_test_tree:
                return True
        if has_product_library_dependency(linked_target, targets, source_root, visited):
            return True
    return False


def source_relative_path(source_root, source):
    try:
        return source.relative_to(source_root).as_posix()
    except ValueError:
        return None


def host_only_test_targets(source_root, targets, changed_paths=None):
    selected = []
    for target in targets.values():
        if target.get("type") != "EXECUTABLE":
            continue
        target_source_dir = (source_root / target["paths"]["source"]).resolve()
        try:
            target_source_dir.relative_to(source_root / "core" / "libs")
        except ValueError:
            continue
        sources = [source_root / source["path"] for source in target.get("sources", []) if not source.get("isGenerated")]
        if not any("tests" in source.parts and source.is_relative_to(source_root / "core" / "libs") for source in sources):
            continue
        if links_guest_libc(target, targets) or has_product_library_dependency(target, targets, source_root):
            continue
        selected.append(target["name"])
    selected = sorted(set(selected), key=str.casefold)
    if changed_paths is None:
        return selected
    changed = {Path(path).as_posix() for path in changed_paths if path}
    changed_source = {
        relative
        for target in targets.values()
        if target.get("name") in selected
        for item in target.get("sources", []) if not item.get("isGenerated")
        for relative in [source_relative_path(source_root, source_root / item["path"])]
        if relative is not None
    }
    matched_sources = changed & changed_source
    if matched_sources:
        return [
            target["name"] for target in targets.values()
            if target.get("name") in selected
            and any(
                source_relative_path(source_root, source_root / item["path"]) in matched_sources
                for item in target.get("sources", []) if not item.get("isGenerated")
            )
        ]
    broad_change = any(
        path in {"CMakeLists.txt", ".github/workflows/build.yml", "tools/build_mingw_host_only_tests.py"}
        or path.startswith("core/libs/") and (
            path.endswith((".h", ".hpp", ".hh", ".inl")) or Path(path).name == "CMakeLists.txt"
        )
        for path in changed
    )
    return selected if broad_change else []


def main(argv=None):
    parser = argparse.ArgumentParser(description="build core/libs test executables that do not link guest libc")
    parser.add_argument("build_dir", type=Path)
    parser.add_argument("changed_paths", nargs="?", type=Path)
    parser.add_argument("--list-only", action="store_true")
    args = parser.parse_args(argv)
    try:
        source_root, targets = read_reply(args.build_dir.resolve())
        changed_paths = args.changed_paths.read_text(encoding="utf-8").splitlines() if args.changed_paths else None
        selected = host_only_test_targets(source_root, targets, changed_paths)
        if not selected:
            print("No changed host-only core/libs test executables require a link check")
            return 0
        print("Host-only core/libs test executables:")
        print("\n".join(selected))
        if not args.list_only:
            subprocess.run(["cmake", "--build", str(args.build_dir), "--parallel", "3", "--target", *selected], check=True)
    except (OSError, KeyError, TypeError, ValueError, json.JSONDecodeError, subprocess.CalledProcessError) as error:
        print(f"{parser.prog}: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
