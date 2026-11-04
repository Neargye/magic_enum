import json
import os
from pathlib import Path
import platform
import re
import shutil
import subprocess


def output(*args, cwd=None):
    return subprocess.check_output(args, cwd=cwd, text=True).strip()


def cmake_values(path):
    return dict(re.findall(r'^set\((\w+) "?([^"\n]*?)"?\)$', path.read_text(encoding="utf-8"), re.MULTILINE))


def hardware_rows():
    runner = os.environ.get("RUNNER_OS")
    if runner == "Windows":
        import ctypes
        import winreg

        memory_kib = ctypes.c_ulonglong()
        if not ctypes.windll.kernel32.GetPhysicallyInstalledSystemMemory(ctypes.byref(memory_kib)):
            raise ctypes.WinError()
        memory = memory_kib.value * 1024
        with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r"HARDWARE\DESCRIPTION\System\CentralProcessor\0") as key:
            cpu = winreg.QueryValueEx(key, "ProcessorNameString")[0].strip()
    elif runner == "macOS":
        memory = int(output("sysctl", "-n", "hw.memsize"))
        cpu = output("sysctl", "-n", "machdep.cpu.brand_string")
    else:
        memory = os.sysconf("SC_PHYS_PAGES") * os.sysconf("SC_PAGE_SIZE")
        info = Path("/proc/cpuinfo").read_text()
        model = re.search(r"^model name\s*:\s*(.+)$", info, re.MULTILINE)
        cpu = model[1] if model else platform.machine()
    return [("Runner", f"{runner} / {platform.machine()}"), ("CPU", cpu),
            ("Logical CPUs", os.cpu_count()), ("RAM (GiB)", f"{memory / 2**30:.1f}")]


def compiler_rows():
    rows = []
    compiler_version = ""
    compiler = os.environ.get("SUMMARY_COMPILER")
    meson_build = os.environ.get("SUMMARY_MESON_BUILD")
    bazel_dir = os.environ.get("SUMMARY_BAZEL_DIR")
    if compiler:
        compiler = shutil.which(compiler) or compiler
    elif meson_build:
        data = json.loads(output("meson", "introspect", "--compilers", meson_build))["host"]["cpp"]
        compiler = " ".join(data["exelist"])
        compiler_version = data["full_version"] or f"{data['id']} {data['version']}"
        rows.append(("Meson", output("meson", "--version")))
    elif bazel_dir:
        data = json.loads(output("bazelisk", "aquery", '--output=jsonproto',
                                 'mnemonic("CppCompile", //...)', '--config=ci', cwd=bazel_dir))
        compilers = {action["arguments"][0] for action in data["actions"] if action["mnemonic"] == "CppCompile"}
        if len(compilers) != 1:
            raise RuntimeError(f"Expected one Bazel C++ compiler, got {compilers}")
        compiler = compilers.pop()
        if not Path(compiler).is_absolute():
            compiler = str(Path(output("bazelisk", "info", "execution_root", cwd=bazel_dir)) / compiler)
        rows.append(("Bazel", output("bazelisk", "--version", cwd=bazel_dir)))
    else:
        build_dir = Path(os.environ.get("SUMMARY_BUILD_DIR", "build"))
        cache = dict(re.findall(r"^([^/#][^:]*):[^=]+=(.*)$", (build_dir / "CMakeCache.txt").read_text(encoding="utf-8"), re.MULTILINE))
        version = ".".join(cache[f"CMAKE_CACHE_{part}_VERSION"] for part in ("MAJOR", "MINOR", "PATCH"))
        compiler_file = build_dir / "CMakeFiles" / version / "CMakeCXXCompiler.cmake"
        if not compiler_file.is_file():
            # Visual Studio bundles CMake versions with a suffix such as -msvc1.
            compiler_file, = (build_dir / "CMakeFiles").glob(f"{version}-*/CMakeCXXCompiler.cmake")
            version = compiler_file.parent.name
        data = cmake_values(compiler_file)
        compiler = data["CMAKE_CXX_COMPILER"]
        compiler_id = data["CMAKE_CXX_COMPILER_ID"]
        if compiler_id == "MSVC":
            # cl.exe does not need to be on PATH for Visual Studio generator builds.
            compiler_version = f"MSVC {data['CMAKE_CXX_COMPILER_VERSION']}"
        rows.extend((("CMake", version), ("Generator", cache["CMAKE_GENERATOR"])))
        if data.get("CMAKE_CXX_COMPILER_ARCHITECTURE_ID"):
            rows.append(("Target", data["CMAKE_CXX_COMPILER_ARCHITECTURE_ID"]))

    if not compiler_version:
        compiler_version = output(compiler, "--version").splitlines()[0]
    rows.insert(0, ("Compiler", compiler_version))
    rows.insert(1, ("Executable", (shutil.which(compiler) or compiler).replace("\\", "/")))
    if os.environ.get("RUNNER_OS") == "macOS":
        rows.append(("Xcode", output("xcodebuild", "-version").replace("\n", ", ")))

    return rows


def cell(value):
    return str(value).replace("|", "\\|").replace("\n", " ")


rows = []
for label, collect in (("Compiler", compiler_rows), ("Hardware", hardware_rows)):
    try:
        rows.extend(collect())
    except Exception as error:
        print(f"{label} diagnostics unavailable: {error}")
        rows.append((label, "Unavailable, see log"))

summary = "## Build environment\n\n| Item | Value |\n| --- | --- |\n"
summary += "".join(f"| {key} | {cell(value)} |\n" for key, value in rows)
print(summary)
try:
    with open(os.environ["GITHUB_STEP_SUMMARY"], "a", encoding="utf-8") as destination:
        destination.write(summary + "\n")
except OSError as error:
    print(f"Cannot write build summary: {error}")
