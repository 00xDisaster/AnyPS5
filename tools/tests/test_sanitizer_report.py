from pathlib import Path
import contextlib
import io
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import sanitizer_report


def case(name, status="run", body=""):
    return f'<testcase name="{name}" classname="{name}" time="0.1" status="{status}">{body}</testcase>'


def suite(*cases):
    return f'<testsuite name="(empty)" tests="{len(cases)}">' + "".join(cases) + "</testsuite>"


class SanitizerReportTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)

    def write(self, text, name="ctest.xml"):
        path = Path(self.directory.name) / name
        path.write_text(text, encoding="utf-8")
        return path

    def run_main(self, *args):
        stdout, stderr = io.StringIO(), io.StringIO()
        with contextlib.redirect_stdout(stdout), contextlib.redirect_stderr(stderr):
            code = sanitizer_report.main([str(arg) for arg in args])
        return code, stdout.getvalue(), stderr.getvalue()

    def test_classifies_passed_skipped_failed_and_not_run_tests(self):
        path = self.write(suite(
            case("passes"),
            case("fails", "fail", '<failure message=""/>'),
            case("skips", "run", '<skipped message="SKIP_RETURN_CODE=77"/>'),
            case("never_built", "notrun"),
            case("switched_off", "disabled"),
        ))
        self.assertEqual(sanitizer_report.results(path), {
            "passes": "passed", "fails": "failed", "skips": "skipped", "never_built": "failed", "switched_off": "skipped",
        })

    def test_summary_counts_and_sorts_the_failed_tests(self):
        text, failing = sanitizer_report.summarize("ASan", {"b": "failed", "a": "failed", "c": "passed", "d": "skipped"})
        self.assertTrue(failing)
        self.assertEqual(text, "### ASan\n\n4 tests: 1 passed, 1 skipped, 2 failed.\n\n"
                               "| Failed test |\n| - |\n| `a` |\n| `b` |\n\n")

    def test_summary_without_failures_has_no_table(self):
        text, failing = sanitizer_report.summarize("ASan", {"a": "passed"})
        self.assertFalse(failing)
        self.assertNotIn("| Failed test |", text)

    def test_exit_code_follows_the_failed_tests(self):
        self.assertEqual(self.run_main(self.write(suite(case("passes"))))[0], 0)
        code, stdout, _ = self.run_main(self.write(suite(case("fails", "fail", '<failure message=""/>'))))
        self.assertEqual(code, 1)
        self.assertIn("| `fails` |", stdout)

    def test_unreadable_reports_exit_with_2(self):
        for text in ("<results/>", suite('<testcase status="run"/>'), "not xml"):
            with self.subTest(text=text):
                code, stdout, stderr = self.run_main(self.write(text))
                self.assertEqual(code, 2)
                self.assertEqual(stdout, "")
                self.assertTrue(stderr.startswith("sanitizer_report: "))
        code, _, stderr = self.run_main(Path(self.directory.name) / "missing.xml")
        self.assertEqual(code, 2)

    def test_summary_file_is_appended_to(self):
        summary = self.write("existing\n", "summary.md")
        code, stdout, _ = self.run_main(self.write(suite(case("passes"))), "--title", "Nightly", "--summary", summary)
        self.assertEqual(code, 0)
        self.assertEqual(summary.read_text(encoding="utf-8"), "existing\n" + stdout)
        self.assertTrue(stdout.startswith("### Nightly\n"))


if __name__ == "__main__":
    unittest.main()
