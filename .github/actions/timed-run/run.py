import os
from pathlib import Path
import subprocess
import sys
import tempfile
import time


name = os.environ["TIMED_NAME"]
shell = os.environ["TIMED_SHELL"]
script = os.environ["TIMED_RUN"]
if shell == "bash":
    command = [os.environ["TIMED_BASH"], "--noprofile", "--norc", "-e", "-o", "pipefail"]
    suffix = ".sh"
elif shell == "pwsh":
    command = ["pwsh", "-NoLogo", "-NoProfile", "-NonInteractive", "-ExecutionPolicy", "Bypass", "-File"]
    script = "$ErrorActionPreference = 'Stop'\n$PSNativeCommandUseErrorActionPreference = $true\n" + script
    script += "\nif (Test-Path -LiteralPath variable:LASTEXITCODE) { exit $LASTEXITCODE }\n"
    suffix = ".ps1"
else:
    raise ValueError(f"Unsupported shell: {shell}")

detailed = os.environ["TIMED_MACOS_DIAGNOSTICS"] == "true" and os.environ["RUNNER_OS"] == "macOS"
with tempfile.TemporaryDirectory(prefix="magic-enum-step-", dir=os.environ["RUNNER_TEMP"]) as directory:
    path = Path(directory) / ("run" + suffix)
    path.write_text(script, encoding="utf-8", newline="\n")
    command.append(str(path))
    if detailed:
        diagnostics = Path(__file__).with_name("macos-diagnostics.sh")
        command = [os.environ["TIMED_BASH"], str(diagnostics), name, *command]
    start = time.monotonic()
    result = subprocess.run(command, cwd=os.environ["TIMED_DIRECTORY"]).returncode
    elapsed = time.monotonic() - start

print(f"{name}: {elapsed:.2f} s, exit code {result}", flush=True)
if not detailed:
    label = name.replace("\n", " ")
    try:
        with open(os.environ["GITHUB_STEP_SUMMARY"], "a", encoding="utf-8") as summary:
            summary.write(f"- {label}: {elapsed:.2f} s, exit {result}\n")
    except OSError as error:
        print(f"Cannot write timing summary: {error}")
sys.exit(result)
