import contextlib
import io
import json
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import pr_overlap


def pull(number):
    return {"number": number, "head": {"sha": f"{number:040x}"}, "user": {"login": "someone"}, "body": ""}


class ProgressTests(unittest.TestCase):
    def capture(self, function, *args):
        stderr = io.StringIO()
        with contextlib.redirect_stderr(stderr):
            result = function(*args)
        return result, stderr.getvalue().splitlines()

    def test_listing_reports_the_number_of_open_pull_requests(self):
        pages = json.dumps([pull(n) for n in range(1, 101)]) + "\n" + json.dumps([pull(n) for n in range(101, 151)])
        with patch.object(pr_overlap, "gh", return_value=pages), patch.object(pr_overlap, "REPO", "owner/project", create=True):
            prs, lines = self.capture(pr_overlap.open_prs, "main")
        self.assertEqual(len(prs), 150)
        self.assertEqual(lines, ["pr_overlap: 150 open pull requests against main"])

    def test_fetch_reports_the_heads_fetched_after_each_batch(self):
        prs = {n: {"ref": f"refs/pr/{n}", "sha": f"{n:040x}"} for n in range(1, 251)}
        calls = []
        with patch.object(pr_overlap, "git", side_effect=lambda *args: calls.append(args) or ""), \
                patch.object(pr_overlap, "REPO", "owner/project", create=True):
            _, lines = self.capture(pr_overlap.fetch, "main", prs)
        self.assertEqual(len(calls), 3)
        self.assertEqual(lines, [
            "pr_overlap: fetched 99/250 pull request heads",
            "pr_overlap: fetched 199/250 pull request heads",
            "pr_overlap: fetched 250/250 pull request heads",
        ])

    def test_fetch_without_pull_requests_still_reports_once(self):
        with patch.object(pr_overlap, "git", return_value=""), patch.object(pr_overlap, "REPO", "owner/project", create=True):
            _, lines = self.capture(pr_overlap.fetch, "main", {})
        self.assertEqual(lines, ["pr_overlap: fetched 0/0 pull request heads"])


if __name__ == "__main__":
    unittest.main()
