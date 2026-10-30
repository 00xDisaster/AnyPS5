import json
import os
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

SCRIPT = 'compile_database_inventory.py'
TOOLS = Path(__file__).resolve().parents[1]


class Tests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)

    def run_tool(self, *args):
        return subprocess.run([sys.executable, str(TOOLS / SCRIPT), *map(str, args)], capture_output=True,
                              encoding="utf-8", timeout=10, env={**os.environ, "PATH": "", "PYTHONIOENCODING": "ascii"})

    def test_groups_raw_variants_without_commands_or_writes(self):
        path=self.root/"编译.json"
        items=[{"directory":"dir","file":"b.cpp","arguments":["missing-tool","-D名字=1","b.cpp"]},
               {"directory":"dir","file":"a.c","command":"missing-tool a.c"},
               {"directory":"dir","file":"b.cpp","command":"missing-tool -DALT b.cpp"}]
        path.write_text(json.dumps(items,ensure_ascii=False),encoding="utf-8-sig")
        saved=path.read_bytes(),path.stat().st_mtime_ns
        r=self.run_tool(path)
        self.assertEqual(r.returncode,0,r.stderr)
        data=json.loads(r.stdout)
        self.assertEqual(data["entries"],3)
        self.assertEqual([(u["file"],u["variants"]) for u in data["units"]],[("a.c",1),("b.cpp",2)])
        self.assertEqual(data["units"][1]["records"],[{"arguments":items[0]["arguments"]},{"command":items[2]["command"]}])
        self.assertEqual(saved,(path.read_bytes(),path.stat().st_mtime_ns))
        self.assertEqual(list(self.root.iterdir()),[path])

    def test_empty_and_invalid_databases(self):
        path=self.root/"db.json"
        for value in ({},[{}],[{"directory":"x","file":"y","arguments":[]}],"bad"):
            path.write_text(json.dumps(value),encoding="utf-8")
            r=self.run_tool(path)
            self.assertEqual(r.returncode,2,r.stdout)
            self.assertEqual(r.stdout,"")
        path.write_text("[]",encoding="utf-8")
        self.assertEqual(json.loads(self.run_tool(path).stdout),{"entries":0,"units":[]})


if __name__ == "__main__":
    unittest.main()
