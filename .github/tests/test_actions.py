import contextlib
import io
import os
from pathlib import Path
import runpy
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest import mock


ACTIONS = Path(__file__).resolve().parents[1] / "actions"
BASH = os.environ.get("CI_TEST_BASH") or shutil.which("bash")


class ActionTests(unittest.TestCase):
    def setUp(self):
        directory = tempfile.TemporaryDirectory(prefix="magic-enum-ci-")
        self.addCleanup(directory.cleanup)
        self.work = Path(directory.name)
        self.summary = self.work / "summary.md"
        self.env = dict(os.environ, RUNNER_OS="Windows" if os.name == "nt" else "Linux",
                        RUNNER_TEMP=str(self.work), GITHUB_STEP_SUMMARY=str(self.summary))

    def timed(self, shell, script):
        env = dict(self.env, TIMED_NAME="Build | test", TIMED_SHELL=shell, TIMED_RUN=script,
                   TIMED_BASH=BASH, TIMED_DIRECTORY=str(self.work), TIMED_MACOS_DIAGNOSTICS="false")
        return subprocess.run([sys.executable, str(ACTIONS / "timed-run/run.py")], env=env,
                              capture_output=True, text=True, timeout=30)

    def test_exit_status_and_fail_fast(self):
        cases = [("bash", "echo ok", 0), ("bash", "exit 19", 19),
                 ("bash", "false\necho SHOULD_NOT_RUN", 1),
                 ("bash", "false | cat\necho SHOULD_NOT_RUN", 1)]
        if os.name == "nt":
            python = sys.executable.replace("'", "''")
            cases += [("pwsh", "Write-Output ok", 0), ("pwsh", "exit 19", 19),
                      ("pwsh", f"& '{python}' -c 'import sys; sys.exit(23)'\nWrite-Output SHOULD_NOT_RUN", 1),
                      ("pwsh", "Write-Error broken\nWrite-Output SHOULD_NOT_RUN", 1)]
        for summary_fails in (False, True):
            for shell, script, expected in cases:
                with self.subTest(shell=shell, script=script, summary_fails=summary_fails):
                    self.summary.write_text("")
                    self.env["GITHUB_STEP_SUMMARY"] = str(self.work if summary_fails else self.summary)
                    result = self.timed(shell, script)
                    self.assertEqual(result.returncode, expected, result.stdout + result.stderr)
                    self.assertNotIn("SHOULD_NOT_RUN", result.stdout)
                    if summary_fails:
                        self.assertIn("Cannot write timing summary:", result.stdout)
                    else:
                        self.assertIn("- Build | test:", self.summary.read_text())
                        self.assertIn(f"exit {expected}\n", self.summary.read_text())
                    self.assertFalse(list(self.work.glob("magic-enum-step-*")))

    def test_working_directory_and_environment_files(self):
        work = self.work / "directory with spaces"
        work.mkdir()
        self.work = work
        self.env.update(GITHUB_ENV=str(work / "env"), GITHUB_OUTPUT=str(work / "output"))
        result = self.timed("bash", 'echo "VALUE=with spaces" >> "$GITHUB_ENV"\n'
                            'echo "result=ok" >> "$GITHUB_OUTPUT"\necho done > marker')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual((work / "env").read_text().strip(), "VALUE=with spaces")
        self.assertEqual((work / "output").read_text().strip(), "result=ok")
        self.assertEqual((work / "marker").read_text().strip(), "done")

    def test_build_summary_failures(self):
        build = self.work / "build"
        compiler_dir = build / "CMakeFiles/3.29.6"
        compiler_dir.mkdir(parents=True)
        (build / "CMakeCache.txt").write_text(
            "CMAKE_CACHE_MAJOR_VERSION:INTERNAL=3\nCMAKE_CACHE_MINOR_VERSION:INTERNAL=29\n"
            "CMAKE_CACHE_PATCH_VERSION:INTERNAL=6\nCMAKE_GENERATOR:INTERNAL=Ninja\n")
        (compiler_dir / "CMakeCXXCompiler.cmake").write_text(
            'set(CMAKE_CXX_COMPILER "cl.exe")\nset(CMAKE_CXX_COMPILER_ID "MSVC")\n'
            'set(CMAKE_CXX_COMPILER_VERSION "19.40.33811.0")\n')
        cases = [(False, False, False), (True, False, False), (False, True, False),
                 (False, False, True), (True, True, True)]
        for hardware_fails, compiler_fails, summary_fails in cases:
            with self.subTest(hardware=hardware_fails, compiler=compiler_fails, summary=summary_fails):
                env = dict(self.env, SUMMARY_BUILD_DIR=str(self.work / "missing" if compiler_fails else build),
                           SUMMARY_COMPILER="", SUMMARY_MESON_BUILD="", SUMMARY_BAZEL_DIR="",
                           GITHUB_STEP_SUMMARY=str(self.work if summary_fails else self.summary))
                output = io.StringIO()
                with mock.patch.dict(os.environ, env, clear=True), contextlib.redirect_stdout(output):
                    with mock.patch("os.cpu_count", side_effect=OSError("hardware unavailable") if hardware_fails else None,
                                    return_value=2):
                        runpy.run_path(str(ACTIONS / "build-summary/summary.py"))
                report = output.getvalue()
                if not compiler_fails:
                    self.assertIn("| Compiler | MSVC 19.40.33811.0 |", report)
                if hardware_fails:
                    self.assertIn("Hardware diagnostics unavailable:", report)
                if compiler_fails:
                    self.assertIn("Compiler diagnostics unavailable:", report)
                if summary_fails:
                    self.assertIn("Cannot write build summary:", report)


if __name__ == "__main__":
    unittest.main()
