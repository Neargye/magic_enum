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

#include <magic_enum.hpp>

#include <array>
#include <string_view>
#include <sstream>

enum class Color { RED = -12, GREEN = 7, BLUE = 15 };

enum class Numbers : char { one = 10, two = 20, three = 30 };

enum Directions { Up = 85, Down = -42, Right = 119, Left = -119 };

enum number : unsigned long { one = 100, two = 200, three = 300 };

namespace magic_enum {
template <>
struct enum_range<number> {
  static constexpr int min = 100;
  static constexpr int max = 300;
};
}

TEST_CASE("enum_cast") {
  SECTION("string") {
#if defined(_MSC_VER) && _MSC_VER < 1920
#  define constexpr // Visual Studio 2017 have bug with string_view constexpr compare.
#endif

    constexpr auto cr = magic_enum::enum_cast<Color>("RED");
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(magic_enum::enum_cast<Color>("GREEN").value() == Color::GREEN);
    REQUIRE(magic_enum::enum_cast<Color>("BLUE").value() == Color::BLUE);
    REQUIRE_FALSE(magic_enum::enum_cast<Color>("None").has_value());

    constexpr auto no = magic_enum::enum_cast<Numbers>("one");
    REQUIRE(no.value() == Numbers::one);
    REQUIRE(magic_enum::enum_cast<Numbers>("two").value() == Numbers::two);
    REQUIRE(magic_enum::enum_cast<Numbers>("three").value() == Numbers::three);
    REQUIRE_FALSE(magic_enum::enum_cast<Numbers>("None").has_value());

    constexpr auto dr = magic_enum::enum_cast<Directions>("Right");
    REQUIRE(magic_enum::enum_cast<Directions>("Up").value() == Directions::Up);
    REQUIRE(magic_enum::enum_cast<Directions>("Down").value() == Directions::Down);
    REQUIRE(dr.value() == Directions::Right);
    REQUIRE(magic_enum::enum_cast<Directions>("Left").value() == Directions::Left);
    REQUIRE_FALSE(magic_enum::enum_cast<Directions>("None").has_value());

    constexpr auto nt = magic_enum::enum_cast<number>("three");
    REQUIRE(magic_enum::enum_cast<number>("one").value() == number::one);
    REQUIRE(magic_enum::enum_cast<number>("two").value() == number::two);
    REQUIRE(nt.value() == number::three);
    REQUIRE_FALSE(magic_enum::enum_cast<number>("None").has_value());

#undef constexpr
  }

  SECTION("integer") {
    constexpr auto cr = magic_enum::enum_cast<Color>(-12);
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(magic_enum::enum_cast<Color>(7).value() == Color::GREEN);
    REQUIRE(magic_enum::enum_cast<Color>(15).value() == Color::BLUE);
    REQUIRE_FALSE(magic_enum::enum_cast<Color>(0).has_value());

    constexpr auto no = magic_enum::enum_cast<Numbers>(10);
    REQUIRE(no.value() == Numbers::one);
    REQUIRE(magic_enum::enum_cast<Numbers>(20).value() == Numbers::two);
    REQUIRE(magic_enum::enum_cast<Numbers>(30).value() == Numbers::three);
    REQUIRE_FALSE(magic_enum::enum_cast<Numbers>(0).has_value());

    constexpr auto dr = magic_enum::enum_cast<Directions>(119);
    REQUIRE(magic_enum::enum_cast<Directions>(85).value() == Directions::Up);
    REQUIRE(magic_enum::enum_cast<Directions>(-42).value() == Directions::Down);
    REQUIRE(dr.value() == Directions::Right);
    REQUIRE(magic_enum::enum_cast<Directions>(-119).value() == Directions::Left);
    REQUIRE_FALSE(magic_enum::enum_cast<Directions>(0).has_value());

    constexpr auto nt = magic_enum::enum_cast<number>(300);
    REQUIRE(magic_enum::enum_cast<number>(100).value() == number::one);
    REQUIRE(magic_enum::enum_cast<number>(200).value() == number::two);
    REQUIRE(nt.value() == number::three);
    REQUIRE_FALSE(magic_enum::enum_cast<number>(0).has_value());
  }
}

TEST_CASE("enum_value") {
 constexpr auto cr = magic_enum::enum_value<Color>(0);
  REQUIRE(cr == Color::RED);
  REQUIRE(magic_enum::enum_value<Color>(1) == Color::GREEN);
  REQUIRE(magic_enum::enum_value<Color>(2) == Color::BLUE);

  constexpr auto no = magic_enum::enum_value<Numbers>(0);
  REQUIRE(no == Numbers::one);
  REQUIRE(magic_enum::enum_value<Numbers>(1) == Numbers::two);
  REQUIRE(magic_enum::enum_value<Numbers>(2) == Numbers::three);

  constexpr auto dr = magic_enum::enum_value<Directions>(3);
  REQUIRE(magic_enum::enum_value<Directions>(0) == Directions::Left);
  REQUIRE(magic_enum::enum_value<Directions>(1) == Directions::Down);
  REQUIRE(magic_enum::enum_value<Directions>(2) == Directions::Up);
  REQUIRE(dr == Directions::Right);

  constexpr auto nt = magic_enum::enum_value<number>(2);
  REQUIRE(magic_enum::enum_value<number>(0) == number::one);
  REQUIRE(magic_enum::enum_value<number>(1) == number::two);
  REQUIRE(nt == number::three);
}

TEST_CASE("enum_values") {
  constexpr auto s1 = magic_enum::enum_values<Color>();
  REQUIRE(s1 == std::array<Color, 3>{{Color::RED, Color::GREEN, Color::BLUE}});

  constexpr auto s2 = magic_enum::enum_values<Numbers>();
  REQUIRE(s2 == std::array<Numbers, 3>{{Numbers::one, Numbers::two, Numbers::three}});

  constexpr auto s3 = magic_enum::enum_values<Directions>();
  REQUIRE(s3 == std::array<Directions, 4>{{Directions::Left, Directions::Down, Directions::Up, Directions::Right}});

  constexpr auto s4 = magic_enum::enum_values<number>();
  REQUIRE(s4 == std::array<number, 3>{{number::one, number::two, number::three}});
}

TEST_CASE("enum_count") {
  constexpr auto s1 = magic_enum::enum_count<Color>();
  REQUIRE(s1 == 3);

  constexpr auto s2 = magic_enum::enum_count<Numbers>();
  REQUIRE(s2 == 3);

  constexpr auto s3 = magic_enum::enum_count<Directions>();
  REQUIRE(s3 == 4);

  constexpr auto s4 = magic_enum::enum_count<number>();
  REQUIRE(s4 == 3);
}

TEST_CASE("enum_name") {
  constexpr Color cr = Color::RED;
  constexpr auto cr_name = magic_enum::enum_name(cr);
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  REQUIRE(cr_name.value() == "RED");
  REQUIRE(magic_enum::enum_name(Color::BLUE).value() == "BLUE");
  REQUIRE(magic_enum::enum_name(cm[1]).value() == "GREEN");
  REQUIRE_FALSE(magic_enum::enum_name(static_cast<Color>(0)).has_value());

  constexpr Numbers no = Numbers::one;
  constexpr auto no_name = magic_enum::enum_name(no);
  REQUIRE(no_name.value() == "one");
  REQUIRE(magic_enum::enum_name(Numbers::two).value() == "two");
  REQUIRE(magic_enum::enum_name(Numbers::three).value() == "three");
  REQUIRE_FALSE(magic_enum::enum_name(static_cast<Numbers>(0)).has_value());

  constexpr Directions dr = Directions::Right;
  constexpr auto dr_name = magic_enum::enum_name(dr);
  REQUIRE(magic_enum::enum_name(Directions::Up).value() == "Up");
  REQUIRE(magic_enum::enum_name(Directions::Down).value() == "Down");
  REQUIRE(dr_name.value() == "Right");
  REQUIRE(magic_enum::enum_name(Directions::Left).value() == "Left");
  REQUIRE_FALSE(magic_enum::enum_name(static_cast<Directions>(0)).has_value());

  constexpr number nt = number::three;
  constexpr auto nt_name = magic_enum::enum_name(nt);
  REQUIRE(magic_enum::enum_name(number::one).value() == "one");
  REQUIRE(magic_enum::enum_name(number::two).value() == "two");
  REQUIRE(nt_name.value() == "three");
  REQUIRE_FALSE(magic_enum::enum_name(static_cast<number>(0)).has_value());
}

TEST_CASE("enum_names") {
  constexpr auto s1 = magic_enum::enum_names<Color>();
  REQUIRE(s1 == std::array<std::string_view, 3>{{"RED", "GREEN", "BLUE"}});

  constexpr auto s2 = magic_enum::enum_names<Numbers>();
  REQUIRE(s2 == std::array<std::string_view, 3>{{"one", "two", "three"}});

  constexpr auto s3 = magic_enum::enum_names<Directions>();
  REQUIRE(s3 == std::array<std::string_view, 4>{{"Left", "Down", "Up", "Right"}});

  constexpr auto s4 = magic_enum::enum_names<number>();
  REQUIRE(s4 == std::array<std::string_view, 3>{{"one", "two", "three"}});
}

TEST_CASE("operator<<") {
  auto test_ostream = [](auto e, std::string_view name) {
    using namespace magic_enum::ops;
    std::stringstream ss;
    ss << e;
    REQUIRE(ss.str() == name);
  };

  test_ostream(Color::RED, "RED");
  test_ostream(Color::GREEN, "GREEN");
  test_ostream(Color::BLUE, "BLUE");
  test_ostream((Color)0, "");

  test_ostream(Numbers::one, "one");
  test_ostream(Numbers::two, "two");
  test_ostream(Numbers::three, "three");
  test_ostream((Numbers)0, "");

  test_ostream(Directions::Up, "Up");
  test_ostream(Directions::Down, "Down");
  test_ostream(Directions::Right, "Right");
  test_ostream(Directions::Left, "Left");
  test_ostream((Directions)0, "");

  test_ostream(number::one, "one");
  test_ostream(number::two, "two");
  test_ostream(number::three, "three");
  test_ostream((number)0, "");
}
