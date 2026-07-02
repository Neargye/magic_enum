// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <new>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = std::wstring_view;
#define MAGIC_ENUM_USING_ALIAS_STRING      using string      = std::wstring;
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_iostream.hpp>

#include "test_helpers.hpp"

#include <array>
#include <cctype>
#include <string_view>

enum class Color { RED = -12, GREEN = 7, BLUE = 15 };
template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<Color>(Color value) noexcept {
  switch (value) {
    case Color::RED:
      return L"red";
    default:
      return default_tag;
  }
}

using namespace magic_enum;
using namespace magic_enum_tests;

static_assert(is_magic_enum_supported, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");

TEST_CASE("enum_cast") {
  SUBCASE("string") {
    constexpr auto cr = enum_cast<Color>(L"red");
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(enum_cast<Color&>(L"GREEN").value() == Color::GREEN);
    REQUIRE(enum_cast<Color>(L"blue", [](wchar_t lhs, wchar_t rhs) { return std::tolower(lhs) == std::tolower(rhs); }).value() == Color::BLUE);
    REQUIRE_FALSE(enum_cast<Color>(L"None").has_value());
  }

  SUBCASE("integer") {
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    constexpr auto cr = enum_cast<Color>(-12);
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(enum_cast<Color&>(7).value() == Color::GREEN);
    REQUIRE(enum_cast<Color>(static_cast<int>(cm[2])).value() == Color::BLUE);
    REQUIRE_FALSE(enum_cast<Color>(0).has_value());
  }
}

TEST_CASE("enum_values") {
  REQUIRE(std::is_same_v<decltype(enum_values<Color>()), const std::array<Color, 3>&>);

  constexpr auto& s1 = enum_values<Color&>();
  REQUIRE(s1 == std::array<Color, 3>{{Color::RED, Color::GREEN, Color::BLUE}});
}

TEST_CASE("enum_count") {
  constexpr auto s1 = enum_count<Color&>();
  REQUIRE(s1 == 3);
}

TEST_CASE("enum_name") {
  SUBCASE("automatic storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = enum_name(cr);
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    Color cb = Color::BLUE;
    REQUIRE(cr_name == L"red");
    REQUIRE(enum_name<Color&>(cb) == L"BLUE");
    REQUIRE(enum_name<as_flags<false>>(cm[1]) == L"GREEN");
    REQUIRE(enum_name<as_common<true>>(cm[1]) == L"GREEN");
    REQUIRE(enum_name<as_flags<false>>(static_cast<Color>(0)).empty());
  }

  SUBCASE("static storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = enum_name<cr>();
    constexpr Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    REQUIRE(cr_name == L"red");
    REQUIRE(enum_name<Color::BLUE>() == L"BLUE");
    REQUIRE(enum_name<cm[1]>() == L"GREEN");
  }
}

TEST_CASE("enum_names") {
  REQUIRE(std::is_same_v<decltype(enum_names<Color>()), const std::array<std::wstring_view, 3>&>);

  constexpr auto& s1 = enum_names<Color&>();
  REQUIRE(s1 == std::array<std::wstring_view, 3>{{L"red", L"GREEN", L"BLUE"}});
}

TEST_CASE("enum_entries") {
  REQUIRE(std::is_same_v<decltype(enum_entries<Color>()), const std::array<std::pair<Color, std::wstring_view>, 3>&>);

  constexpr auto& s1 = enum_entries<Color&>();
  REQUIRE(s1 == std::array<std::pair<Color, std::wstring_view>, 3>{{{Color::RED, L"red"}, {Color::GREEN, L"GREEN"}, {Color::BLUE, L"BLUE"}}});
}

TEST_CASE("wstring_view lifetime and null termination") {
  std::wstring_view static_name{};
  static_name = enum_name<Color::BLUE>();
  require_null_terminated(static_name, L"BLUE");

  std::wstring_view customized_name{};
  customized_name = enum_name(Color::RED);
  require_null_terminated(customized_name, L"red");

  std::wstring_view invalid_name{};
  invalid_name = enum_name(static_cast<Color>(0));
  require_null_terminated(invalid_name, L"");

  std::wstring_view type_name{};
  type_name = enum_type_name<Color>();
  require_null_terminated(type_name, L"Color");

  std::wstring_view array_name{};
  array_name = enum_names<Color>()[1];
  require_null_terminated(array_name, L"GREEN");

  std::wstring_view entry_name{};
  entry_name = enum_entries<Color>()[2].second;
  require_null_terminated(entry_name, L"BLUE");

  for (std::wstring_view name : enum_names<Color>()) {
    require_null_terminated(name);
  }
  for (const auto& entry : enum_entries<Color>()) {
    require_null_terminated(entry.second);
  }
}

TEST_CASE("ostream_operators") {
  require_ostream(std::make_optional(Color::RED), L"red");
  require_ostream(Color::GREEN, L"GREEN");
  require_ostream(Color::BLUE, L"BLUE");
  require_ostream(static_cast<Color>(0), L"0");
  require_ostream(std::make_optional(static_cast<Color>(0)), L"0");
}

TEST_CASE("istream_operators") {
  require_istream(Color::GREEN, L"GREEN");
  require_istream(Color::BLUE, L"BLUE");
}
