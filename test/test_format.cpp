// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include "test_enums.hpp"
#include "test_flags_enums.hpp"

#include <magic_enum/magic_enum_bitwise.hpp>

#include <cstddef>
#include <type_traits>
#include <vector>

#if __has_include(<fmt/format.h>)
#  define FMT_HEADER_ONLY
// GCC 15+ reports a false -Warray-bounds in fmt 9 format_decimal when the header-only float path is inlined.
#  if defined(__GNUC__) && !defined(__clang__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Warray-bounds"
#  endif
#  include <fmt/format.h>
#  include <fmt/ranges.h>
#  if defined(__GNUC__) && !defined(__clang__)
#    pragma GCC diagnostic pop
#  endif
#  define MAGIC_ENUM_TEST_HAS_FMT
#endif

#if defined(MAGIC_ENUM_TEST_REQUIRE_FMT) && !defined(MAGIC_ENUM_TEST_HAS_FMT)
#  error "MAGIC_ENUM_TEST_REQUIRE_FMT requires <fmt/format.h>."
#endif

#include <magic_enum/magic_enum_format.hpp>

#if defined(MAGIC_ENUM_NO_STD_FORMAT) && __has_include(<format>) && ((defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L)
#  include <format>
#endif

#include <magic_enum/magic_enum_all.hpp>

enum class FormatNoValues {};
enum class FormatLongName { EnumeratorWithANameLongerThanSmallStringStorage };

TEST_CASE("format configuration preserves reflection") {
  CHECK(magic_enum::enum_name(Color::RED) == "red");
  CHECK(magic_enum::enum_flags_name(flags_test::Color::RED) == "RED");
#if defined(MAGIC_ENUM_NO_STD_FORMAT) && defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
  static_assert(!std::is_default_constructible_v<std::formatter<Color, char>>);
#endif
#if defined(MAGIC_ENUM_NO_FMT_FORMAT) && defined(MAGIC_ENUM_TEST_HAS_FMT)
  static_assert(!fmt::is_formattable<Color>::value);
#endif
}

#if !defined(MAGIC_ENUM_NO_STD_FORMAT) && defined(__cpp_lib_format) && __cpp_lib_format >= 201907L

TEST_CASE("format-base") {
  REQUIRE(std::format("{}", Color::RED) == "red");
  REQUIRE(std::format("{}", FormatLongName::EnumeratorWithANameLongerThanSmallStringStorage) == "EnumeratorWithANameLongerThanSmallStringStorage");
  REQUIRE(std::format("{:>12.8}", FormatLongName::EnumeratorWithANameLongerThanSmallStringStorage) == "    Enumerat");
  REQUIRE(std::format("{}", Color{0}) == "0");
  REQUIRE(std::format("{}", static_cast<FormatNoValues>(7)) == "7");
  static_assert(!std::is_default_constructible_v<std::formatter<std::byte, char>>);
  REQUIRE(std::format("{}", std::to_integer<unsigned>(std::byte{5})) == "5");
}

#endif

#if defined(MAGIC_ENUM_TEST_HAS_FMT)

TEST_CASE("fmt byte formatting") {
  REQUIRE(fmt::format("{}", std::byte{5}) == "5");
  REQUIRE(fmt::format("{}", std::vector<std::byte>{std::byte{5}, std::byte{255}}) == "[5, 255]");
}

#endif

#if !defined(MAGIC_ENUM_NO_FMT_FORMAT) && defined(MAGIC_ENUM_TEST_HAS_FMT)

TEST_CASE("format-fmt") {
  REQUIRE(fmt::format("{}", Color::GREEN) == "GREEN");
  REQUIRE(fmt::format("{}", FormatLongName::EnumeratorWithANameLongerThanSmallStringStorage) == "EnumeratorWithANameLongerThanSmallStringStorage");
  REQUIRE(fmt::format("{:>12.8}", FormatLongName::EnumeratorWithANameLongerThanSmallStringStorage) == "    Enumerat");
  REQUIRE(fmt::format("{}", std::vector<Color>{Color::GREEN}) == "[\"GREEN\"]");
  REQUIRE(fmt::format("{}", Color{0}) == "0");
  REQUIRE(fmt::format("{}", static_cast<FormatNoValues>(7)) == "7");
}

#endif

namespace flags_test {

using namespace magic_enum::bitwise_operators;

#if !defined(MAGIC_ENUM_NO_STD_FORMAT) && defined(__cpp_lib_format) && __cpp_lib_format >= 201907L

TEST_CASE("flags format-base") {
  REQUIRE(std::format("Test-{:~^11}.", Color::RED | Color::GREEN) == "Test-~RED|GREEN~.");
}

#endif

#if !defined(MAGIC_ENUM_NO_FMT_FORMAT) && defined(MAGIC_ENUM_TEST_HAS_FMT)

TEST_CASE("flags format-fmt") {
  REQUIRE(fmt::format("{}", Color::RED | Color::GREEN) == "RED|GREEN");
}

#endif

} // namespace flags_test

namespace auto_format_test {
enum class Bits { A = 1, B = 2, All = 3 };
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(Bits);
}

#if !defined(MAGIC_ENUM_NO_STD_FORMAT) && defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
TEST_CASE("automatic flags format") {
  REQUIRE(std::format("{}", auto_format_test::Bits::All) == "A|B");
}
#endif
#if !defined(MAGIC_ENUM_NO_FMT_FORMAT) && defined(MAGIC_ENUM_TEST_HAS_FMT)
TEST_CASE("automatic flags fmt") {
  REQUIRE(fmt::format("{}", auto_format_test::Bits::All) == "A|B");
}
#endif
