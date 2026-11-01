// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include "test_enums.hpp"
#include "test_flags_enums.hpp"

#include <magic_enum/magic_enum_bitwise.hpp>

#if __has_include(<fmt/format.h>)
#  define FMT_HEADER_ONLY
// GCC 15+ reports a false -Warray-bounds in fmt 9 format_decimal when the header-only float path is inlined.
#  if defined(__GNUC__) && !defined(__clang__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Warray-bounds"
#  endif
#  include <fmt/format.h>
#  if defined(__GNUC__) && !defined(__clang__)
#    pragma GCC diagnostic pop
#  endif
#  define MAGIC_ENUM_TEST_HAS_FMT
#endif

#if defined(MAGIC_ENUM_TEST_REQUIRE_FMT) && !defined(MAGIC_ENUM_TEST_HAS_FMT)
#  error "MAGIC_ENUM_TEST_REQUIRE_FMT requires <fmt/format.h>."
#endif

#include <magic_enum/magic_enum_format.hpp>

#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L

TEST_CASE("format-base") {
  REQUIRE(std::format("{}", Color::RED) == "red");
  REQUIRE(std::format("{}", Color{0}) == "0");
}

#endif

#if defined(MAGIC_ENUM_TEST_HAS_FMT)

TEST_CASE("format-fmt") {
  REQUIRE(fmt::format("{}", Color::GREEN) == "GREEN");
  REQUIRE(fmt::format("{}", Color{0}) == "0");
}

#endif

namespace flags_test {

using namespace magic_enum::bitwise_operators;

#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L

TEST_CASE("flags format-base") {
  REQUIRE(std::format("Test-{:~^11}.", Color::RED | Color::GREEN) == "Test-~RED|GREEN~.");
}

#endif

#if defined(MAGIC_ENUM_TEST_HAS_FMT)

TEST_CASE("flags format-fmt") {
  REQUIRE(fmt::format("{}", Color::RED | Color::GREEN) == "RED|GREEN");
}

#endif

} // namespace flags_test
