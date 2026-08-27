// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <new>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "aliases.hpp"

#include <magic_enum/magic_enum_all.hpp>

#include "test_helpers.hpp"

#include <sstream>
#include <type_traits>

using namespace magic_enum;
using namespace magic_enum::bitwise_operators;
using namespace magic_enum_tests;

static_assert(std::is_copy_constructible_v<MyStringView>);
static_assert(!std::is_copy_assignable_v<MyStringView>);
static_assert(!std::is_move_assignable_v<MyStringView>);

enum class Color { RED = 1, GREEN = 2, BLUE = 4 };
template <>
struct magic_enum::customize::enum_range<Color> {
  static constexpr bool is_flags = true;
};

TEST_CASE("optional") {
  constexpr auto cr = enum_cast<Color>("RED");
  REQUIRE(cr.has_value());
  REQUIRE(cr.value() == Color::RED);

  constexpr auto cn = enum_cast<Color>("NONE");
  REQUIRE_FALSE(cn.has_value());
}

TEST_CASE("string") {
  auto cr = enum_flags_name(Color::RED);
  REQUIRE_FALSE(cr.empty());
  REQUIRE(std::string_view{cr.data(), cr.size()} == "RED");

  auto crg = enum_flags_name(Color::RED | Color::GREEN);
  REQUIRE_FALSE(crg.empty());
  REQUIRE(std::string_view{crg.data(), crg.size()} == "RED|GREEN");

  auto cn = enum_flags_name(Color{0});
  REQUIRE(cn.empty());
  REQUIRE(cn.size() == 0);

  std::ostringstream stream;
  {
    using namespace magic_enum::ostream_operators;
    stream << (Color::RED | Color::GREEN);
  }
  REQUIRE(stream.str() == "RED|GREEN");
}

TEST_CASE("string_view") {
  auto cr = enum_name(Color::RED);
  REQUIRE_FALSE(cr.empty());
  REQUIRE(cr.compare("RED") == 0);

  auto cn = enum_name(Color{0});
  REQUIRE(cn.empty());
  REQUIRE(cn.size() == 0);
}

TEST_CASE("string_view lifetime and null termination") {
  const auto static_name = enum_name<Color::BLUE>();
  require_null_terminated(static_name, "BLUE");

  const auto value_name = enum_name(Color::RED);
  require_null_terminated(value_name, "RED");

  const auto invalid_name = enum_name(Color{0});
  require_null_terminated(invalid_name, "");

  const auto type_name = enum_type_name<Color>();
  require_null_terminated(type_name, "Color");

  const auto array_name = enum_names<Color>()[1];
  require_null_terminated(array_name, "GREEN");

  const auto entry_name = enum_entries<Color>()[2].second;
  require_null_terminated(entry_name, "BLUE");

  for (MyStringView name : enum_names<Color>()) {
    require_null_terminated(name);
  }
  for (const auto& entry : enum_entries<Color>()) {
    require_null_terminated(entry.second);
  }
}

TEST_CASE("string containers") {
  const magic_enum::containers::bitset<Color> colors{Color::RED, Color::BLUE};

  const auto names = colors.to_string();
  REQUIRE(std::string_view{names.data(), names.size()} == "RED|BLUE");

  const auto raw = colors.to_string(magic_enum::containers::raw_access);
  REQUIRE(std::string_view{raw.data(), raw.size()} == "101");

  std::ostringstream stream;
  stream << colors;
  REQUIRE(stream.str() == "RED|BLUE");
}
