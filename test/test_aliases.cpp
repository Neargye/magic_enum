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

#include "aliases.hpp"

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_flags.hpp>
#include <magic_enum/magic_enum_iostream.hpp>

#include "test_helpers.hpp"

using namespace magic_enum;
using namespace magic_enum::bitwise_operators;
using namespace magic_enum_tests;

enum class Color { RED = 1, GREEN = 2, BLUE = 4 };

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
  REQUIRE(cr.compare("RED") == 0);

  auto crg = enum_flags_name(Color::RED | Color::GREEN);
  REQUIRE_FALSE(crg.empty());
  REQUIRE(crg.compare("RED|GREEN") == 0);

  auto cn = enum_flags_name(Color{0});
  REQUIRE(cn.empty());
  REQUIRE(cn.size() == 0);
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
  MyStringView static_name{};
  static_name = enum_name<Color::BLUE>();
  require_null_terminated(static_name, "BLUE");

  MyStringView value_name{};
  value_name = enum_name(Color::RED);
  require_null_terminated(value_name, "RED");

  MyStringView invalid_name{};
  invalid_name = enum_name(Color{0});
  require_null_terminated(invalid_name, "");

  MyStringView type_name{};
  type_name = enum_type_name<Color>();
  require_null_terminated(type_name, "Color");

  MyStringView array_name{};
  array_name = enum_names<Color>()[1];
  require_null_terminated(array_name, "GREEN");

  MyStringView entry_name{};
  entry_name = enum_entries<Color>()[2].second;
  require_null_terminated(entry_name, "BLUE");

  for (MyStringView name : enum_names<Color>()) {
    require_null_terminated(name);
  }
  for (const auto& entry : enum_entries<Color>()) {
    require_null_terminated(entry.second);
  }
}
