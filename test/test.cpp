// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 Daniil Goncharov <neargye@gmail.com>.
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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#define MAGIC_ENUM_RANGE 120
#include <magic_enum.hpp>

#include <array>

enum class Color { RED = -12, GREEN = 7, BLUE = 15 };

enum class Numbers : char { one = 10, two = 20, three = 30 };

enum Directions { Up = 85, Down = -42, Right = 119, Left = -119 };

enum number : unsigned long { one = 10, two = 20, three = 30 };

TEST_CASE("magic_enum::enum_to_string(enum)") {
  Color cr = Color::RED;
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  REQUIRE(magic_enum::enum_to_string(cr).value() == "RED");
  REQUIRE(magic_enum::enum_to_string(Color::BLUE).value() == "BLUE");
  REQUIRE(magic_enum::enum_to_string(cm[1]).value() == "GREEN");
  REQUIRE_FALSE(magic_enum::enum_to_string(static_cast<Color>(MAGIC_ENUM_RANGE)).has_value());
  REQUIRE_FALSE(magic_enum::enum_to_string(static_cast<Color>(-MAGIC_ENUM_RANGE)).has_value());

  Numbers no = Numbers::one;
  REQUIRE(magic_enum::enum_to_string(no).value() == "one");
  REQUIRE(magic_enum::enum_to_string(Numbers::two).value() == "two");
  REQUIRE(magic_enum::enum_to_string(Numbers::three).value() == "three");
  REQUIRE_FALSE(magic_enum::enum_to_string(static_cast<Numbers>(MAGIC_ENUM_RANGE)).has_value());
  REQUIRE_FALSE(magic_enum::enum_to_string(static_cast<Numbers>(-MAGIC_ENUM_RANGE)).has_value());

  Directions dr = Directions::Right;
  REQUIRE(magic_enum::enum_to_string(Directions::Up).value() == "Up");
  REQUIRE(magic_enum::enum_to_string(Directions::Down).value() == "Down");
  REQUIRE(magic_enum::enum_to_string(dr).value() == "Right");
  REQUIRE(magic_enum::enum_to_string(Directions::Left).value() == "Left");
  REQUIRE_FALSE(magic_enum::enum_to_string(static_cast<Directions>(MAGIC_ENUM_RANGE)).has_value());
  REQUIRE_FALSE(magic_enum::enum_to_string(static_cast<Directions>(-MAGIC_ENUM_RANGE)).has_value());

  number nt = number::three;
  REQUIRE(magic_enum::enum_to_string(number::one).value() == "one");
  REQUIRE(magic_enum::enum_to_string(number::two).value() == "two");
  REQUIRE(magic_enum::enum_to_string(nt).value() == "three");
  REQUIRE_FALSE(magic_enum::enum_to_string(static_cast<number>(MAGIC_ENUM_RANGE)).has_value());
  REQUIRE_FALSE(magic_enum::enum_to_string(static_cast<number>(-MAGIC_ENUM_RANGE)).has_value());
}

TEST_CASE("magic_enum::enum_to_string<enum>()") {
  constexpr Color cr = Color::RED;
  constexpr Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  REQUIRE(magic_enum::enum_to_string<cr>().value() == "RED");
  REQUIRE(magic_enum::enum_to_string<Color::BLUE>().value() == "BLUE");
  REQUIRE(magic_enum::enum_to_string<cm[1]>().value() == "GREEN");
  REQUIRE_FALSE(magic_enum::enum_to_string<static_cast<Color>(MAGIC_ENUM_RANGE)>().has_value());
  REQUIRE_FALSE(magic_enum::enum_to_string<static_cast<Color>(-MAGIC_ENUM_RANGE)>().has_value());

  constexpr Numbers no = Numbers::one;
  REQUIRE(magic_enum::enum_to_string<no>().value() == "one");
  REQUIRE(magic_enum::enum_to_string<Numbers::two>().value() == "two");
  REQUIRE(magic_enum::enum_to_string<Numbers::three>().value() == "three");
  REQUIRE_FALSE(magic_enum::enum_to_string<static_cast<Numbers>(MAGIC_ENUM_RANGE)>().has_value());
  REQUIRE_FALSE(magic_enum::enum_to_string<static_cast<Numbers>(-MAGIC_ENUM_RANGE)>().has_value());

  constexpr Directions dr = Directions::Right;
  REQUIRE(magic_enum::enum_to_string<Directions::Up>().value() == "Up");
  REQUIRE(magic_enum::enum_to_string<Directions::Down>().value() == "Down");
  REQUIRE(magic_enum::enum_to_string<dr>().value() == "Right");
  REQUIRE(magic_enum::enum_to_string<Directions::Left>().value() == "Left");
  REQUIRE_FALSE(magic_enum::enum_to_string<static_cast<Directions>(MAGIC_ENUM_RANGE)>().has_value());
  REQUIRE_FALSE(magic_enum::enum_to_string<static_cast<Directions>(-MAGIC_ENUM_RANGE)>().has_value());

  constexpr number nt = number::three;
  REQUIRE(magic_enum::enum_to_string<number::one>().value() == "one");
  REQUIRE(magic_enum::enum_to_string<number::two>().value() == "two");
  REQUIRE(magic_enum::enum_to_string<nt>().value() == "three");
  REQUIRE_FALSE(magic_enum::enum_to_string<static_cast<number>(MAGIC_ENUM_RANGE)>().has_value());
  REQUIRE_FALSE(magic_enum::enum_to_string<static_cast<number>(-MAGIC_ENUM_RANGE)>().has_value());
}

TEST_CASE("magic_enum::enum_from_string(name)") {
  REQUIRE(magic_enum::enum_from_string<Color>("RED").value() == Color::RED);
  REQUIRE(magic_enum::enum_from_string<Color>("GREEN").value() == Color::GREEN);
  REQUIRE(magic_enum::enum_from_string<Color>("BLUE").value() == Color::BLUE);
  REQUIRE_FALSE(magic_enum::enum_from_string<Color>("None").has_value());

  REQUIRE(magic_enum::enum_from_string<Numbers>("one").value() == Numbers::one);
  REQUIRE(magic_enum::enum_from_string<Numbers>("two").value() == Numbers::two);
  REQUIRE(magic_enum::enum_from_string<Numbers>("three").value() == Numbers::three);
  REQUIRE_FALSE(magic_enum::enum_from_string<Numbers>("None").has_value());

  REQUIRE(magic_enum::enum_from_string<Directions>("Up").value() == Directions::Up);
  REQUIRE(magic_enum::enum_from_string<Directions>("Down").value() == Directions::Down);
  REQUIRE(magic_enum::enum_from_string<Directions>("Right").value() == Directions::Right);
  REQUIRE(magic_enum::enum_from_string<Directions>("Left").value() == Directions::Left);
  REQUIRE_FALSE(magic_enum::enum_from_string<Directions>("None").has_value());

  REQUIRE(magic_enum::enum_from_string<number>("one").value() == number::one);
  REQUIRE(magic_enum::enum_from_string<number>("two").value() == number::two);
  REQUIRE(magic_enum::enum_from_string<number>("three").value() == number::three);
  REQUIRE_FALSE(magic_enum::enum_from_string<number>("None").has_value());
}

TEST_CASE("magic_enum::enum_sequence<enum>()") {
  constexpr auto mge_s1 = magic_enum::enum_to_sequence<Color>();
  REQUIRE(mge_s1 == std::array<Color, 3>{Color::RED, Color::GREEN, Color::BLUE});

  constexpr auto mge_s2 = magic_enum::enum_to_sequence<Numbers>();
  REQUIRE(mge_s2 == std::array<Numbers, 3>{Numbers::one, Numbers::two, Numbers::three});

  constexpr auto mge_s3 = magic_enum::enum_to_sequence<Directions>();
  REQUIRE(mge_s3 == std::array<Directions, 4>{Directions::Left, Directions::Down, Directions::Up, Directions::Right});

  constexpr auto mge_s4 = magic_enum::enum_to_sequence<number>();
  REQUIRE(mge_s4 == std::array<number, 3>{number::one, number::two, number::three});
}

TEST_CASE("magic_enum::enum_to_string_sequence<enum>()") {
  constexpr auto mge_s1 = magic_enum::enum_to_string_sequence<Color>();
  REQUIRE(mge_s1 == std::array<std::string_view, 3>{"RED", "GREEN", "BLUE"});

  constexpr auto mge_s2 = magic_enum::enum_to_string_sequence<Numbers>();
  REQUIRE(mge_s2 == std::array<std::string_view, 3>{"one", "two", "three"});

  constexpr auto mge_s3 = magic_enum::enum_to_string_sequence<Directions>();
  REQUIRE(mge_s3 == std::array<std::string_view, 4>{"Left", "Down", "Up", "Right"});

  constexpr auto mge_s4 = magic_enum::enum_to_string_sequence<number>();
  REQUIRE(mge_s4 == std::array<std::string_view, 3>{"one", "two", "three"});
}
