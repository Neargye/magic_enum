import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


SCRIPT = Path(__file__).resolve().parents[1] / "actions/setup-linux-compiler/setup.sh"
BASH = os.environ.get("CI_TEST_BASH") or shutil.which("bash")
GCC_KEY = "C8EC952E2A0E1FBDC5090F6A2C277A0A352154E5"
LLVM_KEY = "6084F3CF814B57C1CF12EFD515CF4D18AF4F7421"

PRELUDE = r'''
if [[ "$OSTYPE" == msys* ]]; then
  SETUP_SCRIPT="$(cygpath -u "$SETUP_SCRIPT")"
  CALL_LOG="$(cygpath -u "$CALL_LOG")"
  GITHUB_ENV="$(cygpath -u "$GITHUB_ENV")"
  TMPDIR="$(cygpath -u "$TMPDIR")"
fi
command() { [[ "$1" == "-v" && "$INSTALLED" == "true" ]]; }
lsb_release() { echo "$CODENAME"; }
apt-cache() {
  echo "apt-cache $*" >> "$CALL_LOG"
  if [[ "$CACHE_RESULT" != 0 ]]; then return "$CACHE_RESULT"; fi
  shift
  for package in "$@"; do
    [[ "$package" == "$OMIT_PACKAGE" ]] && continue
    echo "$package:"
    if [[ "$AVAILABLE" == "true" && "$package" != "$MISSING_PACKAGE" ]]; then
      echo "  Candidate: 1.0"
    else
      echo "  Candidate: (none)"
    fi
  done
}
curl() {
  echo "curl $*" >> "$CALL_LOG"
  while [[ "$1" != "--output" ]]; do shift; done
  if [[ "$DOWNLOAD_RESULT" != 0 ]]; then return "$DOWNLOAD_RESULT"; fi
  echo key > "$2"
}
gpg() {
  echo "gpg $*" >> "$CALL_LOG"
  if [[ "$GPG_RESULT" != 0 ]]; then return "$GPG_RESULT"; fi
  printf 'pub:::::::::\nfpr:::::::::%s:\nsub:::::::::\nfpr:::::::::SUBKEY:\n' "$KEY"
  if [[ "$EXTRA_KEY" == "true" ]]; then
    printf 'pub:::::::::\nfpr:::::::::OTHERKEY:\n'
  fi
}
updates=0
sudo() {
  echo "sudo $*" >> "$CALL_LOG"
  case "$1" in
    timeout)
      case " $* " in
        *" update "*)
          updates=$((updates + 1))
          if [[ "$updates" == "$FAIL_UPDATE" ]]; then return "$UPDATE_RESULT"; fi
          ;;
        *" install "*) return "$INSTALL_RESULT" ;;
        *) return 99 ;;
      esac
      ;;
    tee) cat >> "$CALL_LOG" ;;
    install) ;;
    *) return 99 ;;
  esac
}
source "$SETUP_SCRIPT"
'''


@unittest.skipUnless(BASH, "Bash is required")
class LinuxCompilerTests(unittest.TestCase):
    def setup_compiler(self, compiler="gcc-16", stdlib="libstdc++", expected=0, **options):
        with tempfile.TemporaryDirectory(prefix="magic-enum-compiler-") as directory:
            work = Path(directory)
            calls_file = work / "calls"
            env_file = work / "env"
            calls_file.write_text("")
            env_file.write_text("")
            env = dict(os.environ, CC=compiler, STDLIB=stdlib, INSTALLED="false", AVAILABLE="true",
                       CACHE_RESULT="0", DOWNLOAD_RESULT="0", GPG_RESULT="0", INSTALL_RESULT="0",
                       FAIL_UPDATE="0", UPDATE_RESULT="100", MISSING_PACKAGE="", OMIT_PACKAGE="", EXTRA_KEY="false",
                       CODENAME="noble", KEY=GCC_KEY if compiler.startswith("gcc-") else LLVM_KEY,
                       SETUP_SCRIPT=str(SCRIPT), CALL_LOG=str(calls_file), GITHUB_ENV=str(env_file), TMPDIR=str(work))
            env.update(options)
            result = subprocess.run([BASH, "-e", "-o", "pipefail"], input=PRELUDE, env=env,
                                    capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, expected, result.stdout + result.stderr)
            cxx = compiler.replace("gcc-", "g++-").replace("clang-", "clang++-")
            self.assertEqual(env_file.read_text(), "" if expected else f"CC={compiler}\nCXX={cxx}\n")
            calls = calls_file.read_text()
            self.assertNotIn("add-apt-repository", calls)
            self.assertNotIn("api.launchpad.net", calls)
            for line in calls.splitlines():
                if " apt-get " in line:
                    self.assertIn("timeout --kill-after=10s 5m apt-get", line)
                    self.assertIn("-o Acquire::Retries=2", line)
                    self.assertIn("-o Acquire::http::Timeout=20", line)
                    self.assertIn("-o Acquire::https::Timeout=20", line)
                    if " update " in line:
                        self.assertIn("--error-on=any", line)
                if line.startswith("curl "):
                    self.assertIn("--retry 2 --retry-all-errors", line)
                    self.assertIn("--connect-timeout 10 --max-time 30", line)
                    self.assertIn("--fail", line)
            # The EXIT trap must remove the temporary key on success and failure.
            self.assertEqual({p.name for p in work.iterdir()}, {"calls", "env"})
            return calls

    def test_installed_compiler_skips_network(self):
        for compiler in ("gcc-16", "clang-19"):
            with self.subTest(compiler=compiler):
                self.assertEqual(self.setup_compiler(compiler, INSTALLED="true"), "")

    def test_distribution_packages_do_not_add_repository(self):
        for compiler, stdlib, packages in (
            ("gcc-16", "libstdc++", "g++-16"),
            ("clang-19", "libstdc++", "clang-19"),
            ("clang-19", "libc++", "clang-19 libclang-rt-19-dev libc++-19-dev libc++abi-19-dev clang-tools-19"),
        ):
            with self.subTest(compiler=compiler, stdlib=stdlib):
                calls = self.setup_compiler(compiler, stdlib)
                self.assertIn(f"install -y {packages}", calls)
                self.assertLess(calls.index("update --error-on=any"), calls.index("apt-cache policy"))
                self.assertEqual(calls.count("update --error-on=any"), 1)
                self.assertNotIn("curl ", calls)
                self.assertNotIn("sources.list.d", calls)

    def test_missing_packages_add_signed_repository(self):
        for compiler, repository, source, key in (
            ("gcc-16", "ubuntu-toolchain-r-test", "https://ppa.launchpadcontent.net/ubuntu-toolchain-r/test/ubuntu noble main", GCC_KEY),
            ("clang-23", "llvm", "https://apt.llvm.org/noble/ llvm-toolchain-noble-23 main", LLVM_KEY),
        ):
            with self.subTest(compiler=compiler):
                calls = self.setup_compiler(compiler, AVAILABLE="false")
                self.assertIn(f"deb [signed-by=/etc/apt/keyrings/{repository}.asc] {source}", calls)
                self.assertIn("sudo install -D -m 0644", calls)
                self.assertIn("gpg --batch --show-keys --with-colons", calls)
                self.assertEqual(calls.count("update --error-on=any"), 2)
                self.assertLess(calls.index("apt-cache policy"), calls.index("curl "))
                self.assertLess(calls.index("gpg "), calls.index("sudo install"))
                if compiler.startswith("gcc-"):
                    self.assertIn(f"search=0x{key}", calls)

    def test_distribution_install_failure_does_not_add_repository(self):
        calls = self.setup_compiler(expected=23, INSTALL_RESULT="23")
        self.assertIn("install -y g++-16", calls)
        self.assertNotIn("curl ", calls)

    def test_repository_uses_runner_codename(self):
        calls = self.setup_compiler(AVAILABLE="false", CODENAME="resolute")
        self.assertIn("/ubuntu resolute main", calls)
        self.assertNotIn("noble", calls)

    def test_missing_libcxx_dependency_adds_repository(self):
        for field in ("MISSING_PACKAGE", "OMIT_PACKAGE"):
            with self.subTest(field=field):
                calls = self.setup_compiler("clang-19", "libc++", INSTALLED="true",
                                            **{field: "clang-tools-19"})
                self.assertIn("sources.list.d/llvm.list", calls)
                self.assertIn("libc++-19-dev libc++abi-19-dev clang-tools-19", calls)

    def test_setup_failures_do_not_publish_compiler(self):
        for options, expected, forbidden in (
            ({"FAIL_UPDATE": "1"}, 100, "apt-cache policy"),
            ({"FAIL_UPDATE": "1", "UPDATE_RESULT": "124"}, 124, "apt-cache policy"),
            ({"CACHE_RESULT": "31"}, 31, "curl "),
            ({"DOWNLOAD_RESULT": "28"}, 28, "gpg "),
            ({"GPG_RESULT": "2"}, 2, "sudo install"),
            ({"KEY": "WRONGKEY"}, 1, "sudo install"),
            ({"KEY": ""}, 1, "sudo install"),
            ({"EXTRA_KEY": "true"}, 1, "sudo install"),
            ({"FAIL_UPDATE": "2"}, 100, "install -y"),
            ({"INSTALL_RESULT": "23"}, 23, ""),
        ):
            with self.subTest(options=options):
                calls = self.setup_compiler(expected=expected, AVAILABLE="false", **options)
                if forbidden:
                    self.assertNotIn(forbidden, calls)

    def test_invalid_configuration_has_no_side_effects(self):
        for compiler, stdlib in (("gcc-16", "libc++"), ("clang-19", "other"), ("other", "libstdc++")):
            with self.subTest(compiler=compiler, stdlib=stdlib):
                self.assertEqual(self.setup_compiler(compiler, stdlib, expected=1), "")


if __name__ == "__main__":
    unittest.main()
