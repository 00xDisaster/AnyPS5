import argparse
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

FAILED_STATUSES = ("fail", "notrun")
SKIPPED_STATUSES = ("disabled",)


def results(path):
    root = ET.parse(path).getroot()
    if root.tag not in ("testsuite", "testsuites"):
        raise ValueError(f"{path}: expected a JUnit testsuite or testsuites root, got {root.tag!r}")
    found = {}
    for case in root.iter("testcase"):
        name = case.get("name")
        if not name:
            raise ValueError(f"{path}: testcase has no name")
        if case.find("skipped") is not None or case.get("status") in SKIPPED_STATUSES:
            outcome = "skipped"
        elif case.get("status") in FAILED_STATUSES or case.find("failure") is not None or case.find("error") is not None:
            outcome = "failed"
        else:
            outcome = "passed"
        found[name] = outcome
    return found


def summarize(title, found):
    counts = {outcome: sum(1 for value in found.values() if value == outcome) for outcome in ("passed", "skipped", "failed")}
    failed = sorted(name for name, outcome in found.items() if outcome == "failed")
    lines = [f"### {title}", "",
             f"{len(found)} tests: {counts['passed']} passed, {counts['skipped']} skipped, {counts['failed']} failed.", ""]
    if failed:
        lines += ["| Failed test |", "| - |"] + [f"| `{name}` |" for name in failed] + [""]
    return "\n".join(lines) + "\n", bool(failed)


def main(argv=None):
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            stream.reconfigure(encoding="utf-8", errors="backslashreplace")
    parser = argparse.ArgumentParser(description="summarize a sanitizer test run and fail when a test failed")
    parser.add_argument("junit", type=Path, help="the file written by ctest --output-junit")
    parser.add_argument("--title", default="Sanitizer run", help="heading of the summary")
    parser.add_argument("--summary", type=Path, help="also append the summary to this file, such as $GITHUB_STEP_SUMMARY")
    args = parser.parse_args(argv)
    try:
        text, failing = summarize(args.title, results(args.junit))
    except (OSError, ET.ParseError, ValueError) as error:
        print(f"sanitizer_report: {error}", file=sys.stderr)
        return 2
    sys.stdout.write(text)
    if args.summary:
        with args.summary.open("a", encoding="utf-8") as summary:
            summary.write(text)
    return 1 if failing else 0


if __name__ == "__main__":
    sys.exit(main())
