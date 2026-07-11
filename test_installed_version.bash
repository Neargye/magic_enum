#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
build_root="${repo_root}/build-installed-version"
source_build_dir="${build_root}/source"
install_dir="${build_root}/install"
cmake_consumer_build_dir="${build_root}/cmake-consumer"
pkgconfig_consumer_build_dir="${build_root}/pkgconfig-consumer"

test_installed_consumer() {
  local build_dir="$1"
  shift

  CMAKE_PREFIX_PATH="${install_dir}" cmake -S "${repo_root}" -B "${build_dir}" \
    -DCMAKE_BUILD_TYPE=Debug \
    "$@"
  cmake --build "${build_dir}" --parallel
  ctest --test-dir "${build_dir}" --output-on-failure --no-tests=error
}

cmake -E remove_directory "${build_root}"

cmake -S "${repo_root}" -B "${source_build_dir}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="${install_dir}" \
  -DMAGIC_ENUM_OPT_BUILD_EXAMPLES=OFF \
  -DMAGIC_ENUM_OPT_BUILD_TESTS=OFF \
  -DMAGIC_ENUM_OPT_INSTALL=ON \
  -DMAGIC_ENUM_OPT_VERIFY_INTERFACE_HEADER_SETS=ON
cmake --build "${source_build_dir}" \
  --target magic_enum_verify_interface_header_sets \
  --parallel
cmake --install "${source_build_dir}"

test_installed_consumer "${cmake_consumer_build_dir}" \
  -DMAGIC_ENUM_OPT_TEST_INSTALLED_VERSION=ON

test_installed_consumer "${pkgconfig_consumer_build_dir}" \
  -DMAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG=ON \
  -DPKG_CONFIG_USE_CMAKE_PREFIX_PATH=ON

