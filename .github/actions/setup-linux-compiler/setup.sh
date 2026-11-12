#!/usr/bin/env bash
set -euo pipefail

case "$CC" in
  gcc-*) CXX="g++-${CC#gcc-}"; packages=("$CXX") ;;
  # clang-tools provides clang-scan-deps, which CMake needs for modules.
  clang-*) CXX="clang++-${CC#clang-}"; packages=("$CC" "clang-tools-${CC#clang-}") ;;
  *) echo "Unsupported compiler: $CC" >&2; exit 1 ;;
esac
if [[ "$STDLIB" != "libstdc++" && ( "$STDLIB" != "libc++" || "$CC" != clang-* ) ]]; then
  echo "Unsupported standard library: $CC / $STDLIB" >&2
  exit 1
fi

apt_get() {
  sudo timeout --kill-after=10s 5m apt-get -o Acquire::Retries=2 \
    -o Acquire::http::Timeout=20 -o Acquire::https::Timeout=20 "$@"
}

if ! command -v "$CXX" > /dev/null 2>&1 || [[ "$STDLIB" == "libc++" ]] ||
   { [[ "$CC" == clang-* ]] && ! command -v "clang-scan-deps-${CC#clang-}" > /dev/null 2>&1; }; then
  if [[ "$STDLIB" == "libc++" ]]; then
    clang_version="${CC#clang-}"
    packages+=("libclang-rt-${clang_version}-dev" "libc++-${clang_version}-dev" "libc++abi-${clang_version}-dev")
  fi
  apt_get update --error-on=any
  candidates="$(LC_ALL=C apt-cache policy "${packages[@]}")"
  # GCC comes from the distribution only. apt.llvm.org provides the Clang versions it does not package.
  if [[ "$CC" == clang-* ]] && ! awk -v count="${#packages[@]}" '/Candidate:/ && $2 != "(none)" { found++ } END { exit found != count }' <<< "$candidates"; then
    ubuntu_codename="$(lsb_release -cs)"
    fingerprint=6084F3CF814B57C1CF12EFD515CF4D18AF4F7421
    key_file="$(mktemp)"
    trap 'rm -f "$key_file"' EXIT
    curl --fail --silent --show-error --location --retry 2 --retry-all-errors \
      --connect-timeout 10 --max-time 30 --output "$key_file" https://apt.llvm.org/llvm-snapshot.gpg.key
    # Accept exactly one primary key, with the repository's published fingerprint.
    actual_fingerprint="$(gpg --batch --show-keys --with-colons "$key_file" |
      awk -F: '$1 == "pub" { primary = 1 } $1 == "fpr" && primary { print $10; primary = 0 }')"
    if [[ "$actual_fingerprint" != "$fingerprint" ]]; then
      echo "Unexpected signing key for apt.llvm.org: $actual_fingerprint" >&2
      exit 1
    fi
    sudo install -D -m 0644 "$key_file" /etc/apt/keyrings/llvm.asc
    echo "deb [signed-by=/etc/apt/keyrings/llvm.asc] https://apt.llvm.org/${ubuntu_codename}/ llvm-toolchain-${ubuntu_codename}-${CC#clang-} main" |
      sudo tee /etc/apt/sources.list.d/llvm.list > /dev/null
    apt_get update --error-on=any
  fi
  apt_get install -y "${packages[@]}"
fi

"$CXX" --version
echo "CC=$CC" >> "$GITHUB_ENV"
echo "CXX=$CXX" >> "$GITHUB_ENV"
