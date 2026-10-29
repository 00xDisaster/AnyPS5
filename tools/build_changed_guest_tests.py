import argparse
import json
from pathlib import Path
import subprocess
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
import build_mingw_host_only_tests as host_only


def guest_test_targets(source_root, targets, changed_paths=None):
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
        if not host_only.links_guest_libc(target):
            continue
        selected.append(target["name"])
    selected = sorted(set(selected), key=str.casefold)
    if changed_paths is None:
        return selected
    changed = {Path(path).as_posix() for path in changed_paths if path}
    broad_change = any(
        path in {"CMakeLists.txt", ".github/workflows/build.yml", "tools/build_mingw_host_only_tests.py", "tools/build_changed_guest_tests.py"}
        or path.startswith("core/libs/") and (
            path.endswith((".h", ".hpp", ".hh", ".inl")) or Path(path).name == "CMakeLists.txt"
        )
        for path in changed
    )
    if broad_change:
        return selected
    changed_source = {
        relative
        for target in targets.values()
        if target.get("name") in selected
        for item in target.get("sources", []) if not item.get("isGenerated")
        for relative in [host_only.source_relative_path(source_root, source_root / item["path"])]
        if relative is not None
    }
    matched_sources = changed & changed_source
    if matched_sources:
        return [
            target["name"] for target in targets.values()
            if target.get("name") in selected
            and any(
                host_only.source_relative_path(source_root, source_root / item["path"]) in matched_sources
                for item in target.get("sources", []) if not item.get("isGenerated")
            )
        ]
    return []


def target_artifact_names(target):
    names = set()
    for artifact in target.get("artifacts", []):
        name = Path(artifact.get("path", "")).name
        if name:
            if name.casefold().endswith(".exe"):
                name = name[:-4]
            names.add(name)
    names.add(target.get("name", ""))
    names.discard("")
    return names


def ctest_names(build_dir, target_names):
    result = subprocess.run(
        ["ctest", "--test-dir", str(build_dir), "--show-only=json-v1"],
        capture_output=True, text=True, check=True)
    document = json.loads(result.stdout)
    wanted = set(target_names)
    names = []
    for test in document.get("tests", []):
        command = [str(part) for part in test.get("command", [])]
        if not command:
            continue
        executable = Path(command[-1]).name
        if executable.casefold().endswith(".exe"):
            executable = executable[:-4]
        if executable in wanted:
            names.append(test["name"])
    return sorted(set(names), key=str.casefold)


def main(argv=None):
    parser = argparse.ArgumentParser(description="build and run changed core/libs tests that link guest libc")
    parser.add_argument("build_dir", type=Path)
    parser.add_argument("changed_paths", nargs="?", type=Path)
    args = parser.parse_args(argv)
    try:
        if args.changed_paths is not None:
            listed = {Path(path).as_posix() for path in args.changed_paths.read_text(encoding='utf-8').splitlines() if path}
            reaches = any(path == 'CMakeLists.txt' or path.startswith(('core/libs/', '.github/workflows/build.yml', 'tools/')) for path in listed)
            if not reaches:
                print("No changed guest core/libs tests require a run")
                return 0
        source_root, targets = host_only.read_reply(args.build_dir.resolve())
        changed_paths = args.changed_paths.read_text(encoding="utf-8").splitlines() if args.changed_paths else None
        selected = guest_test_targets(source_root, targets, changed_paths)
        if not selected:
            print("No changed guest core/libs tests require a run")
            return 0
        print("Guest core/libs tests:")
        print("\n".join(selected))
        subprocess.run(["cmake", "--build", str(args.build_dir), "--parallel", "3", "--target", "libs"], check=True)
        subprocess.run(["cmake", "--build", str(args.build_dir), "--parallel", "3", "--target", *selected], check=True)
        by_name = {target.get("name"): target for target in targets.values()}
        artifacts = set()
        for name in selected:
            artifacts |= target_artifact_names(by_name[name])
        names = ctest_names(args.build_dir, artifacts)
        if not names:
            print(f"{parser.prog}: no ctest registration found for the built guest tests", file=sys.stderr)
            return 1
        pattern = "^(" + "|".join(name.replace("+", r"\+") for name in names) + ")$"
        subprocess.run(["ctest", "--test-dir", str(args.build_dir), "--output-on-failure", "--timeout", "120", "--parallel", "4", "-R", pattern], check=True)
    except (OSError, KeyError, TypeError, ValueError, json.JSONDecodeError, subprocess.CalledProcessError) as error:
        print(f"{parser.prog}: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
