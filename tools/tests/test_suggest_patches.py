import hashlib
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import suggest_patches


class PatchAdvisorTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        self.catalog = self.root / "catalog"
        self.catalog.mkdir()
        self.input = self.root / "eboot.bin"
        self.input.write_bytes(b"main executable")
        self.module = self.root / "Media" / "Modules" / "module.prx"
        self.module.parent.mkdir(parents=True)
        self.module.write_bytes(b"module version one")
        self.param = self.root / "param.json"
        self.param.write_text(json.dumps({"titleId": "TEST00001", "contentVersion": "01.000.000"}))
        self.relinker = self.root / "relinker"
        self.relinker.write_bytes(b"fixture")
        (self.catalog / "patch.toml").write_text('format = 1\n[[patch]]\ntarget = "Media/Modules/module.prx"\nva = 0x1000\nexpected = "00"\nvalue = "01"\n')
        self.entry = {"titleId": "TEST00001", "contentVersion": "01.000.000", "patch": "patch.toml",
                      "targets": {"Media/Modules/module.prx": hashlib.sha256(self.module.read_bytes()).hexdigest()}}
        self.write_index()

    def write_index(self):
        (self.catalog / "index.json").write_text(json.dumps({"format": 1, "entries": [self.entry]}))

    def advise(self, *extra):
        return suggest_patches.main(["--catalog", str(self.catalog), "--param-json", str(self.param),
                                     "--input", str(self.input), "--relinker", str(self.relinker),
                                     "--module-dir", "Media/Modules", *extra])

    def additional_executable(self):
        executable = self.root / "picker" / "picker.bin"
        executable.parent.mkdir()
        executable.write_bytes(b"secondary executable")
        (self.catalog / "patch.toml").write_text('format = 1\n[[patch]]\ntarget = "picker/picker.bin"\nva = 0x1000\nexpected = "00"\nvalue = "01"\n')
        self.entry["targets"] = {"picker/picker.bin": hashlib.sha256(executable.read_bytes()).hexdigest()}
        self.write_index()
        return executable

    @patch.object(suggest_patches.subprocess, "run")
    def test_verified_match_requires_explicit_application(self, run):
        run.return_value.returncode = 0
        self.assertEqual(self.advise(), 0)
        self.assertEqual(run.call_args.args[0], [str(self.relinker), "--verify-patches",
                                                 str(self.catalog / "patch.toml"), "--module-dir", "Media/Modules",
                                                 str(self.input)])
        self.assertFalse((self.root / "relinker-patches.toml").exists())

    @patch.object(suggest_patches.subprocess, "run")
    def test_changed_module_is_not_sent_to_relinker(self, run):
        self.module.write_bytes(b"module version two")
        self.assertEqual(self.advise(), 1)
        run.assert_not_called()

    @patch.object(suggest_patches.subprocess, "run")
    def test_relinker_rejection_is_not_a_recommendation(self, run):
        run.return_value.returncode = 1
        run.return_value.stdout = "mismatch"
        run.return_value.stderr = ""
        self.assertEqual(self.advise(), 1)

    @patch.object(suggest_patches.subprocess, "run")
    def test_catalog_cannot_escape_directory(self, run):
        self.entry["patch"] = "../outside.toml"
        self.write_index()
        self.assertEqual(self.advise(), 2)
        run.assert_not_called()

    @patch.object(suggest_patches.subprocess, "run")
    def test_hashes_cover_all_patch_targets(self, run):
        self.entry["targets"] = {"eboot.bin": hashlib.sha256(self.input.read_bytes()).hexdigest()}
        self.write_index()
        self.assertEqual(self.advise(), 2)
        run.assert_not_called()

    @patch.object(suggest_patches.subprocess, "run")
    def test_additional_executable_is_hashed_and_verified(self, run):
        self.additional_executable()
        run.return_value.returncode = 0
        self.assertEqual(self.advise("--executable", "picker/picker.bin"), 0)
        self.assertEqual(run.call_args.args[0], [str(self.relinker), "--verify-patches",
                                                 str(self.catalog / "patch.toml"), "--module-dir", "Media/Modules",
                                                 "--executable", "picker/picker.bin", str(self.input)])

    @patch.object(suggest_patches.subprocess, "run")
    def test_changed_additional_executable_is_not_verified(self, run):
        executable = self.additional_executable()
        executable.write_bytes(b"different executable")
        self.assertEqual(self.advise("--executable", "picker/picker.bin"), 1)
        run.assert_not_called()

    @patch.object(suggest_patches.subprocess, "run")
    def test_additional_executable_cannot_escape_module_parent(self, run):
        self.assertEqual(self.advise("--executable", "../outside.bin"), 2)
        run.assert_not_called()


if __name__ == "__main__":
    unittest.main()
