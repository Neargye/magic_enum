#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
build_root="${repo_root}/build-installed-version"
source_build_dir="${build_root}/source"
configured_install_dir="${build_root}/configured-install"
install_dir="${build_root}/install"
consumer_prefix="${build_root}/relocated-install"
cmake_consumer_build_dir="${build_root}/cmake-consumer"
pkgconfig_consumer_build_dir="${build_root}/pkgconfig-consumer"
cxx_modules_consumer_build_dir="${build_root}/cxx_modules-consumer"

test_installed_consumer() {
  local build_dir="$1"
  shift

  CMAKE_PREFIX_PATH="${consumer_prefix}" cmake -S "${repo_root}" -B "${build_dir}" \
    -G Ninja \
    -D CMAKE_BUILD_TYPE=Debug \
    "$@"
  cmake --build "${build_dir}" --parallel
  ctest --test-dir "${build_dir}" --output-on-failure --no-tests=error
}

cmake --version
# XXX cmake -E remove_directory "${build_root}"

cmake -S "${repo_root}" -B "${source_build_dir}" -G Ninja \
  -D CMAKE_BUILD_TYPE=Release \
  -D CMAKE_CXX_STANDARD=26 \
  -D CMAKE_EXPERIMENTAL_CXX_IMPORT_STD=f35a9ac6-8463-4d38-8eec-5d6008153e7d \
  -D CMAKE_INSTALL_PREFIX="${configured_install_dir}" \
  -D MAGIC_ENUM_MODULE_IMPORT_STD=YES \
  -D MAGIC_ENUM_MODULE_WITH_FMT=NO \
  -D MAGIC_ENUM_OPT_BUILD_EXAMPLES=OFF \
  -D MAGIC_ENUM_OPT_BUILD_TESTS=OFF \
  -D MAGIC_ENUM_OPT_INSTALL=ON \
  -D MAGIC_ENUM_OPT_VERIFY_INTERFACE_HEADER_SETS=ON \
  -D MAGIC_ENUM_USE_MODULES=ON # XXX --fresh
cmake --build "${source_build_dir}" \
  --target magic_enum_verify_interface_header_sets \
  --parallel
cmake --build "${source_build_dir}" \
  --target all \
  --parallel
cmake --install "${source_build_dir}" --prefix "${install_dir}"
cmake -E copy_directory "${install_dir}" "${consumer_prefix}"

test_installed_consumer "${cxx_modules_consumer_build_dir}" \
  -D CMAKE_CXX_STANDARD=26 \
  -D CMAKE_EXPERIMENTAL_CXX_IMPORT_STD=f35a9ac6-8463-4d38-8eec-5d6008153e7d \
  -D MAGIC_ENUM_MODULE_IMPORT_STD=YES \
  -D MAGIC_ENUM_MODULE_WITH_FMT=NO \
  -D MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION=ON \
  -D MAGIC_ENUM_USE_MODULES=ON \
  -D PKG_CONFIG_USE_CMAKE_PREFIX_PATH=OFF # XXX --fresh

test_installed_consumer "${cmake_consumer_build_dir}" \
  -D MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION=ON

test_installed_consumer "${pkgconfig_consumer_build_dir}" \
  -D MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG=ON \
  -D PKG_CONFIG_USE_CMAKE_PREFIX_PATH=ON

