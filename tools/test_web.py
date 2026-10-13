import argparse
from pathlib import Path
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "core/relinker/relinker/tests"))
from test_guest_intel_trampolines import main_fixture
from test_guest_module_directories import module_with_symbol


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--build", type=Path, required=True)
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="anyps5-web-") as directory:
        fixture = Path(directory)
        (fixture / "main.elf").write_bytes(main_fixture())
        (fixture / "module.elf").write_bytes(module_with_symbol(True))
        subprocess.run(["node", str(ROOT / "demo/web/tests/archive.test.mjs"), str(args.build.resolve() / "web"), str(fixture)], check=True)


if __name__ == "__main__":
    main()
