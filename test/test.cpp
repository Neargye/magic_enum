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

#define MAGIC_ENUM_RANGE_MIN -120
#define MAGIC_ENUM_RANGE_MAX 120
#include <magic_enum.hpp>

#include <array>
#include <string_view>
#include <sstream>

enum class Color { RED = -12, GREEN = 7, BLUE = 15 };

enum class Numbers : int { one = 10, two = 20, three = 30, many = 127 };

enum Directions { Up = 85, Down = -42, Right = 120, Left = -120 };

enum number : unsigned long { one = 100, two = 200, three = 300, four = 400 };

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
    REQUIRE_FALSE(magic_enum::enum_cast<Numbers>("many").has_value());
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
    REQUIRE_FALSE(magic_enum::enum_cast<number>("four").has_value());
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
    REQUIRE_FALSE(magic_enum::enum_cast<Numbers>(127).has_value());
    REQUIRE_FALSE(magic_enum::enum_cast<Numbers>(0).has_value());

    constexpr auto dr = magic_enum::enum_cast<Directions>(120);
    REQUIRE(magic_enum::enum_cast<Directions>(85).value() == Directions::Up);
    REQUIRE(magic_enum::enum_cast<Directions>(-42).value() == Directions::Down);
    REQUIRE(dr.value() == Directions::Right);
    REQUIRE(magic_enum::enum_cast<Directions>(-120).value() == Directions::Left);
    REQUIRE_FALSE(magic_enum::enum_cast<Directions>(0).has_value());

    constexpr auto nt = magic_enum::enum_cast<number>(300);
    REQUIRE(magic_enum::enum_cast<number>(100).value() == number::one);
    REQUIRE(magic_enum::enum_cast<number>(200).value() == number::two);
    REQUIRE(nt.value() == number::three);
    REQUIRE_FALSE(magic_enum::enum_cast<number>(400).has_value());
    REQUIRE_FALSE(magic_enum::enum_cast<number>(0).has_value());
  }
}

TEST_CASE("enum_integer") {
  constexpr auto cr = magic_enum::enum_integer(Color::RED);
  REQUIRE(cr == -12);
  REQUIRE(magic_enum::enum_integer(Color::GREEN) == 7);
  REQUIRE(magic_enum::enum_integer(Color::BLUE) == 15);
  REQUIRE(magic_enum::enum_integer(static_cast<Color>(0)) == 0);

  constexpr auto no = magic_enum::enum_integer(Numbers::one);
  REQUIRE(no == 10);
  REQUIRE(magic_enum::enum_integer(Numbers::two) == 20);
  REQUIRE(magic_enum::enum_integer(Numbers::three) == 30);
  REQUIRE(magic_enum::enum_integer(Numbers::many) == 127);
  REQUIRE(magic_enum::enum_integer(static_cast<Numbers>(0)) == 0);

  constexpr auto dr = magic_enum::enum_integer(Directions::Right);
  REQUIRE(magic_enum::enum_integer(Directions::Left) == -120);
  REQUIRE(magic_enum::enum_integer(Directions::Down) == -42);
  REQUIRE(magic_enum::enum_integer(Directions::Up) == 85);
  REQUIRE(dr == 120);
  REQUIRE(magic_enum::enum_integer(static_cast<Directions>(0)) == 0);

  constexpr auto nt = magic_enum::enum_integer(number::three);
  REQUIRE(magic_enum::enum_integer(number::one) == 100);
  REQUIRE(magic_enum::enum_integer(number::two) == 200);
  REQUIRE(nt == 300);
  REQUIRE(magic_enum::enum_integer(number::four) == 400);
  REQUIRE(magic_enum::enum_integer(static_cast<number>(0)) == 0);
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
  SECTION("automatic storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = magic_enum::enum_name(cr);
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    REQUIRE(cr_name == "RED");
    REQUIRE(magic_enum::enum_name(Color::BLUE) == "BLUE");
    REQUIRE(magic_enum::enum_name(cm[1]) == "GREEN");
    REQUIRE(magic_enum::enum_name(static_cast<Color>(0)).empty());

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = magic_enum::enum_name(no);
    REQUIRE(no_name == "one");
    REQUIRE(magic_enum::enum_name(Numbers::two) == "two");
    REQUIRE(magic_enum::enum_name(Numbers::three) == "three");
    REQUIRE(magic_enum::enum_name(Numbers::many).empty());
    REQUIRE(magic_enum::enum_name(static_cast<Numbers>(0)).empty());

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = magic_enum::enum_name(dr);
    REQUIRE(magic_enum::enum_name(Directions::Up) == "Up");
    REQUIRE(magic_enum::enum_name(Directions::Down) == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(magic_enum::enum_name(Directions::Left) == "Left");
    REQUIRE(magic_enum::enum_name(static_cast<Directions>(0)).empty());

    constexpr number nt = number::three;
    constexpr auto nt_name = magic_enum::enum_name(nt);
    REQUIRE(magic_enum::enum_name(number::one) == "one");
    REQUIRE(magic_enum::enum_name(number::two) == "two");
    REQUIRE(nt_name == "three");
    REQUIRE(magic_enum::enum_name(number::four).empty());
    REQUIRE(magic_enum::enum_name(static_cast<number>(0)).empty());
  }

  SECTION("static storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = magic_enum::enum_name<cr>();
    constexpr Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    REQUIRE(cr_name == "RED");
    REQUIRE(magic_enum::enum_name<Color::BLUE>() == "BLUE");
    REQUIRE(magic_enum::enum_name<cm[1]>() == "GREEN");
    REQUIRE(magic_enum::enum_name<static_cast<Color>(0)>().empty());

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = magic_enum::enum_name<no>();
    REQUIRE(no_name == "one");
    REQUIRE(magic_enum::enum_name<Numbers::two>() == "two");
    REQUIRE(magic_enum::enum_name<Numbers::three>() == "three");
    REQUIRE(magic_enum::enum_name<Numbers::many>() == "many");
    REQUIRE(magic_enum::enum_name<static_cast<Numbers>(0)>().empty());

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = magic_enum::enum_name<dr>();
    REQUIRE(magic_enum::enum_name<Directions::Up>() == "Up");
    REQUIRE(magic_enum::enum_name<Directions::Down>() == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(magic_enum::enum_name<Directions::Left>() == "Left");
    REQUIRE(magic_enum::enum_name<static_cast<Directions>(0)>().empty());

    constexpr number nt = number::three;
    constexpr auto nt_name = magic_enum::enum_name<nt>();
    REQUIRE(magic_enum::enum_name<number::one>() == "one");
    REQUIRE(magic_enum::enum_name<number::two>() == "two");
    REQUIRE(nt_name == "three");
    REQUIRE(magic_enum::enum_name<number::four>() == "four");
    REQUIRE(magic_enum::enum_name<static_cast<number>(0)>().empty());
  }
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

TEST_CASE("enum_entries") {
  constexpr auto s1 = magic_enum::enum_entries<Color>();
  REQUIRE(s1 == std::array<std::pair<Color, std::string_view>, 3>{{{Color::RED, "RED"}, {Color::GREEN, "GREEN"}, {Color::BLUE, "BLUE"}}});

  constexpr auto s2 = magic_enum::enum_entries<Numbers>();
  REQUIRE(s2 == std::array<std::pair<Numbers, std::string_view>, 3>{{{Numbers::one, "one"}, {Numbers::two, "two"}, {Numbers::three, "three"}}});

  constexpr auto s3 = magic_enum::enum_entries<Directions>();
  REQUIRE(s3 == std::array<std::pair<Directions, std::string_view>, 4>{{{Directions::Left, "Left"}, {Directions::Down, "Down"}, {Directions::Up, "Up"}, {Directions::Right, "Right"}}});

  constexpr auto s4 = magic_enum::enum_entries<number>();
  REQUIRE(s4 == std::array<std::pair<number, std::string_view>, 3>{{{number::one, "one"}, {number::two, "two"}, {number::three, "three"}}});
}

TEST_CASE("ostream_operators") {
  auto test_ostream = [](auto e, std::string_view name) {
    using namespace magic_enum::ostream_operators;
    std::stringstream ss;
    ss << e;
    REQUIRE(ss.str() == name);
  };

  test_ostream(Color::RED, "RED");
  test_ostream(Color::GREEN, "GREEN");
  test_ostream(Color::BLUE, "BLUE");
  test_ostream(static_cast<Color>(0), "0");

  test_ostream(Numbers::one, "one");
  test_ostream(Numbers::two, "two");
  test_ostream(Numbers::three, "three");
  test_ostream(Numbers::many, "127");
  test_ostream(static_cast<Numbers>(0), "0");

  test_ostream(Directions::Up, "Up");
  test_ostream(Directions::Down, "Down");
  test_ostream(Directions::Right, "Right");
  test_ostream(Directions::Left, "Left");
  test_ostream(static_cast<Directions>(0), "0");

  test_ostream(number::one, "one");
  test_ostream(number::two, "two");
  test_ostream(number::three, "three");
  test_ostream(number::four, "400");
  test_ostream(static_cast<number>(0), "0");
}

TEST_CASE("bitwise_operators") {
}

TEST_CASE("type_traits") {
  REQUIRE_FALSE(magic_enum::is_unscoped_enum_v<Color>);
  REQUIRE_FALSE(magic_enum::is_unscoped_enum_v<Numbers>);
  REQUIRE(magic_enum::is_unscoped_enum_v<Directions>);
  REQUIRE(magic_enum::is_unscoped_enum_v<number>);

  REQUIRE(magic_enum::is_scoped_enum_v<Color>);
  REQUIRE(magic_enum::is_scoped_enum_v<Numbers>);
  REQUIRE_FALSE(magic_enum::is_scoped_enum_v<Directions>);
  REQUIRE_FALSE(magic_enum::is_scoped_enum_v<number>);

  REQUIRE(magic_enum::is_fixed_enum_v<Color>);
  REQUIRE(magic_enum::is_fixed_enum_v<Numbers>);
  REQUIRE_FALSE(magic_enum::is_fixed_enum_v<Directions>);
  REQUIRE(magic_enum::is_fixed_enum_v<number>);
}
