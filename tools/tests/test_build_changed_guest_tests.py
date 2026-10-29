import contextlib
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import build_changed_guest_tests


class GuestTestTargets(unittest.TestCase):
    def target(self, name, target_type="EXECUTABLE", source="core/libs/tests/Guest.cpp", fragment=""):
        return {
            "artifacts": [{"path": f"tests/{name}.exe"}],
            "id": name,
            "link": {"commandFragments": [{"fragment": fragment}] if fragment else []},
            "name": name,
            "paths": {"build": "core/libs", "source": "core/libs"},
            "sources": [{"path": source}],
            "type": target_type,
        }

    def fixture(self, targets):
        directory = Path(self.temporary.name)
        reply = directory / "build" / ".cmake" / "api" / "v1" / "reply"
        reply.mkdir(parents=True)
        entries = []
        for target in targets:
            filename = f"target-{target['name']}.json"
            (reply / filename).write_text(json.dumps(target), encoding="utf-8")
            entries.append({"id": target["id"], "jsonFile": filename, "name": target["name"]})
        model = {"configurations": [{"targets": entries}], "paths": {"source": str(directory)}}
        (reply / "codemodel.json").write_text(json.dumps(model), encoding="utf-8")
        index = {"reply": {"codemodel-v2": {"jsonFile": "codemodel.json"}}}
        (reply / "index-0001.json").write_text(json.dumps(index), encoding="utf-8")
        return directory / "build"

    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()

    def tearDown(self):
        self.temporary.cleanup()

    def test_selects_core_libs_test_executables_with_guest_libc(self):
        guest = self.target("guest_pad_tests", fragment="libc.prx")
        host = self.target("host_tests")
        product = self.target("libScePad", target_type="SHARED_LIBRARY", source="core/libs/prx/libScePad/Export.cpp")
        build = self.fixture([guest, host, product])
        source_root, targets = build_changed_guest_tests.host_only.read_reply(build.resolve())
        self.assertEqual(build_changed_guest_tests.guest_test_targets(source_root, targets), ["guest_pad_tests"])

    def test_changed_source_selects_only_the_changed_target(self):
        guest_a = self.target("guest_a_tests", source="core/libs/tests/GuestA.cpp", fragment="libc.prx")
        guest_b = self.target("guest_b_tests", source="core/libs/tests/GuestB.cpp", fragment="libc.prx")
        build = self.fixture([guest_a, guest_b])
        source_root, targets = build_changed_guest_tests.host_only.read_reply(build.resolve())
        changed = build_changed_guest_tests.guest_test_targets(source_root, targets, ["core/libs/tests/GuestB.cpp"])
        self.assertEqual(changed, ["guest_b_tests"])

    def test_a_header_change_selects_every_guest_test(self):
        guest_a = self.target("guest_a_tests", source="core/libs/tests/GuestA.cpp", fragment="libc.prx")
        guest_b = self.target("guest_b_tests", source="core/libs/tests/GuestB.cpp", fragment="libc.prx")
        build = self.fixture([guest_a, guest_b])
        source_root, targets = build_changed_guest_tests.host_only.read_reply(build.resolve())
        changed = build_changed_guest_tests.guest_test_targets(source_root, targets, ["core/libs/prx/libScePad/include/Pad.hpp"])
        self.assertEqual(changed, ["guest_a_tests", "guest_b_tests"])

    def test_an_unrelated_change_selects_nothing(self):
        guest = self.target("guest_pad_tests", source="core/libs/tests/Guest.cpp", fragment="libc.prx")
        build = self.fixture([guest])
        source_root, targets = build_changed_guest_tests.host_only.read_reply(build.resolve())
        changed = build_changed_guest_tests.guest_test_targets(source_root, targets, ["core/relinker/codegen/src/Amd64OnlyConverter.cpp"])
        self.assertEqual(changed, [])

    def test_ctest_names_resolve_through_the_command_executable(self):
        listing = {"tests": [
            {"name": "guest_pad", "command": ["ctest", "C:/b/tests/guest_pad_tests.exe"]},
            {"name": "guest_pad_led_state", "command": ["C:/b/tests/guest_pad_led_state_tests.exe"]},
            {"name": "unrelated", "command": ["C:/b/tests/unrelated_tests.exe"]},
        ]}
        with mock.patch.object(build_changed_guest_tests.subprocess, "run", return_value=mock.Mock(stdout=json.dumps(listing))):
            names = build_changed_guest_tests.ctest_names(Path("build"), {"guest_pad_tests", "guest_pad_led_state_tests"})
        self.assertEqual(names, ["guest_pad", "guest_pad_led_state"])

    def test_artifact_names_cover_the_exe_and_the_target(self):
        target = self.target("guest_pad_tests")
        self.assertEqual(build_changed_guest_tests.target_artifact_names(target), {"guest_pad_tests"})


if __name__ == "__main__":
    with contextlib.redirect_stdout(io.StringIO()):
        unittest.main()
