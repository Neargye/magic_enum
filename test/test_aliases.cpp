// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include "aliases.hpp"

#include <magic_enum/magic_enum_all.hpp>

#include "test_fixed_string.hpp"
#include "test_helpers.hpp"

#include <iomanip>
#include <sstream>

using namespace magic_enum;
using namespace magic_enum::bitwise_operators;
using namespace magic_enum_tests;

enum class Color { RED = 1, GREEN = 2, BLUE = 4 };
template <>
struct magic_enum::customize::enum_range<Color> {
  static constexpr bool is_flags = true;
};

TEST_CASE("bool enum lookup with custom string_view and optional") {
  enum class Binary : bool { FALSE_VALUE = false, TRUE_VALUE = true };
  static_assert(enum_cast<Binary>("FALSE_VALUE").value() == Binary::FALSE_VALUE);
  static_assert(enum_cast<Binary>("TRUE_VALUE").value() == Binary::TRUE_VALUE);
  static_assert(!enum_cast<Binary>("INVALID").has_value());
  static_assert(!enum_cast<Binary>("").has_value());
  static_assert(!enum_contains<Binary>("INVALID"));
  const char* name = "FALSE_VALUE";
  CHECK(enum_cast<Binary>(name).value() == Binary::FALSE_VALUE);
  CHECK(enum_cast<Binary>(MyStringView{name}).value() == Binary::FALSE_VALUE);
  CHECK(enum_contains<Binary>(name));
}

TEST_CASE("name lookup propagates conversion errors with custom string_view") {
  struct NameInput {
    bool fail;
    DOCTEST_NOINLINE operator string_view() const {
      if (fail) {
        throw 42;
      }
      return "RED";
    }
  };
  static_assert(!noexcept(enum_cast<Color>(NameInput{true})));
  static_assert(!noexcept(enum_contains<Color>(NameInput{true})));
  static_assert(!noexcept(enum_flags_cast<Color>(NameInput{true})));
  static_assert(!noexcept(enum_flags_contains<Color>(NameInput{true})));
  CHECK_THROWS_AS(static_cast<void>(enum_cast<Color>(NameInput{true})), int);
  CHECK_THROWS_AS(static_cast<void>(enum_contains<Color>(NameInput{true})), int);
  CHECK_THROWS_AS(static_cast<void>(enum_flags_cast<Color>(NameInput{true})), int);
  CHECK_THROWS_AS(static_cast<void>(enum_flags_contains<Color>(NameInput{true})), int);
  CHECK(enum_cast<Color>(NameInput{false}).value() == Color::RED);
  CHECK(enum_contains<Color>(NameInput{false}));
  CHECK(enum_flags_cast<Color>(NameInput{false}).value() == Color::RED);
  CHECK(enum_flags_contains<Color>(NameInput{false}));
}

TEST_CASE("optional") {
  constexpr auto cr = enum_cast<Color>("RED");
  REQUIRE(cr.has_value());
  REQUIRE(cr.value() == Color::RED);

  constexpr auto cn = enum_cast<Color>("NONE");
  REQUIRE_FALSE(cn.has_value());
}

enum class AliasStep { First, Second };

// Utilities return the custom optional, see #147.
TEST_CASE("optional in fuse and navigation") {
  static_assert(std::is_same_v<decltype(enum_next_value(AliasStep::First)), MyOpt<AliasStep>>);
  static_assert(std::is_same_v<decltype(enum_prev_value(AliasStep::First)), MyOpt<AliasStep>>);
  REQUIRE(enum_next_value(AliasStep::First).value() == AliasStep::Second);
  REQUIRE_FALSE(enum_next_value(AliasStep::Second).has_value());
  REQUIRE(enum_prev_value(AliasStep::Second).value() == AliasStep::First);
  REQUIRE_FALSE(enum_prev_value(AliasStep::First).has_value());

  constexpr auto fused = enum_fuse(AliasStep::Second, AliasStep::First);
  static_assert(std::is_same_v<std::decay_t<decltype(fused)>, MyOpt<std::decay_t<decltype(*fused)>>>);
  REQUIRE(fused.has_value());
#if defined(NDEBUG)
  // An invalid value fails the assertion, or gives an empty optional when asserts are disabled.
  REQUIRE_FALSE(enum_fuse(AliasStep::First, static_cast<AliasStep>(5)).has_value());
#endif
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

TEST_CASE("istream_operators") {
  enum class Direction { Left, Right };

  require_istream(Direction::Right, "Right");
  require_istream(Color::GREEN, "GREEN");
  require_istream(Color::RED | Color::BLUE, "RED|BLUE");

  // Reading an enum only requires constructing the custom optional.
  static_assert(!std::is_copy_assignable_v<optional<Direction>>);
  static_assert(!std::is_move_assignable_v<optional<Color>>);
  const auto check_invalid = [](auto initial) {
    using magic_enum::istream_operators::operator>>;
    for (const auto text : {"UNKNOWN", ""}) {
      std::istringstream stream{text};
      auto value = initial;
      stream >> value;
      REQUIRE(stream.fail());
      REQUIRE(value == initial);
    }
  };
  check_invalid(Direction::Right);
  check_invalid(Color::GREEN);
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

TEST_CASE("string parameter overrides the alias") {
  static_assert(std::is_same_v<decltype(enum_flags_name(Color::RED)), MyString>);
  static_assert(enum_flags_name<Color, FixedString<16>>(Color::RED | Color::GREEN) == "RED|GREEN");
  REQUIRE(enum_flags_name<Color, std::string>(Color::RED | Color::GREEN) == "RED|GREEN");

  constexpr magic_enum::containers::bitset<Color> colors{Color::RED, Color::BLUE};
  static_assert(std::is_same_v<decltype(colors.to_string()), MyString>);
  static_assert(colors.to_string<FixedString<16>>() == "RED|BLUE");
  REQUIRE(colors.to_string<std::string>(magic_enum::containers::raw_access) == "101");
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

TEST_CASE("ostream_operators") {
  using namespace magic_enum::ostream_operators;

  std::ostringstream narrow;
  narrow << std::setfill('_') << std::setw(10) << (Color::RED | Color::BLUE) << '|';
  REQUIRE(narrow.str() == "__RED|BLUE|");

  std::wostringstream wide;
  wide << std::setfill(L'_') << std::setw(10) << (Color::RED | Color::BLUE) << L'|';
  REQUIRE(wide.str() == L"__RED|BLUE|");
}
