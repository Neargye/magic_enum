#!/usr/bin/env bash
set -euo pipefail

case "$CC" in
  gcc-*) CXX="g++-${CC#gcc-}"; packages=("$CXX") ;;
  clang-*) CXX="clang++-${CC#clang-}"; packages=("$CC") ;;
  *) echo "Unsupported compiler: $CC" >&2; exit 1 ;;
esac
if [[ "$STDLIB" != "libstdc++" && ( "$STDLIB" != "libc++" || "$CC" != clang-* ) ]]; then
  echo "Unsupported standard library: $CC / $STDLIB" >&2
  exit 1
fi
if ! command -v "$CXX" > /dev/null 2>&1 || [[ "$STDLIB" == "libc++" ]]; then
  case "$CC" in
    clang-*)
      wget -qO - https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor | sudo tee /etc/apt/keyrings/llvm-archive-keyring.gpg > /dev/null
      clang_version="${CC#clang-}"
      ubuntu_codename="$(lsb_release -cs)"
      echo "deb [signed-by=/etc/apt/keyrings/llvm-archive-keyring.gpg] https://apt.llvm.org/${ubuntu_codename}/ llvm-toolchain-${ubuntu_codename}-${clang_version} main" | sudo tee /etc/apt/sources.list.d/llvm.list
      if [[ "$STDLIB" == "libc++" ]]; then
        packages+=("libclang-rt-${clang_version}-dev" "libc++-${clang_version}-dev" "libc++abi-${clang_version}-dev" "clang-tools-${clang_version}")
      fi
      ;;
    gcc-*) sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y ;;
  esac
  sudo apt-get update
  sudo apt-get install -y "${packages[@]}"
fi
echo "CC=$CC" >> "$GITHUB_ENV"
echo "CXX=$CXX" >> "$GITHUB_ENV"
