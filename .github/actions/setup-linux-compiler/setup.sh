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

apt_get() {
  sudo timeout --kill-after=10s 5m apt-get -o Acquire::Retries=2 \
    -o Acquire::http::Timeout=20 -o Acquire::https::Timeout=20 "$@"
}

if ! command -v "$CXX" > /dev/null 2>&1 || [[ "$STDLIB" == "libc++" ]]; then
  if [[ "$STDLIB" == "libc++" ]]; then
    clang_version="${CC#clang-}"
    packages+=("libclang-rt-${clang_version}-dev" "libc++-${clang_version}-dev" "libc++abi-${clang_version}-dev" "clang-tools-${clang_version}")
  fi
  apt_get update --error-on=any
  candidates="$(LC_ALL=C apt-cache policy "${packages[@]}")"
  if ! awk -v count="${#packages[@]}" '/Candidate:/ && $2 != "(none)" { found++ } END { exit found != count }' <<< "$candidates"; then
    ubuntu_codename="$(lsb_release -cs)"
    case "$CC" in
      gcc-*)
        repository=ubuntu-toolchain-r-test
        fingerprint=C8EC952E2A0E1FBDC5090F6A2C277A0A352154E5
        key_url="https://keyserver.ubuntu.com/pks/lookup?op=get&search=0x${fingerprint}"
        source="https://ppa.launchpadcontent.net/ubuntu-toolchain-r/test/ubuntu ${ubuntu_codename} main"
        ;;
      clang-*)
        repository=llvm
        fingerprint=6084F3CF814B57C1CF12EFD515CF4D18AF4F7421
        key_url=https://apt.llvm.org/llvm-snapshot.gpg.key
        source="https://apt.llvm.org/${ubuntu_codename}/ llvm-toolchain-${ubuntu_codename}-${CC#clang-} main"
        ;;
    esac
    key_file="$(mktemp)"
    trap 'rm -f "$key_file"' EXIT
    curl --fail --silent --show-error --location --retry 2 --retry-all-errors \
      --connect-timeout 10 --max-time 30 --output "$key_file" "$key_url"
    # Accept exactly one primary key, with the repository's published fingerprint.
    actual_fingerprint="$(gpg --batch --show-keys --with-colons "$key_file" |
      awk -F: '$1 == "pub" { primary = 1 } $1 == "fpr" && primary { print $10; primary = 0 }')"
    if [[ "$actual_fingerprint" != "$fingerprint" ]]; then
      echo "Unexpected signing key for $repository: $actual_fingerprint" >&2
      exit 1
    fi
    sudo install -D -m 0644 "$key_file" "/etc/apt/keyrings/${repository}.asc"
    echo "deb [signed-by=/etc/apt/keyrings/${repository}.asc] $source" | sudo tee "/etc/apt/sources.list.d/${repository}.list" > /dev/null
    apt_get update --error-on=any
  fi
  apt_get install -y "${packages[@]}"
fi

echo "CC=$CC" >> "$GITHUB_ENV"
echo "CXX=$CXX" >> "$GITHUB_ENV"
