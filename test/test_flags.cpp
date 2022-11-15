// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2022 Daniil Goncharov <neargye@gmail.com>.
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

#if defined(__clang__)
#  pragma clang diagnostic push
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4244) // warning C4244: 'argument': conversion from 'const T' to 'unsigned int', possible loss of data.
#endif

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <magic_enum.hpp>
#include <magic_enum_fuse.hpp>

#include <array>
#include <cctype>
#include <string_view>
#include <sstream>

enum class Color { RED = 1, GREEN = 2, BLUE = 4 };

enum class Numbers : int {
  one = 1 << 1,
  two = 1 << 2,
  three = 1 << 3,
  many = 1 << 30,
};

enum Directions : std::uint64_t {
  Left = std::uint64_t{1} << 10,
  Down = std::uint64_t{1} << 20,
  Up = std::uint64_t{1} << 31,
  Right = std::uint64_t{1} << 63,
};

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
enum class Language : int {
  日本語 = 1 << 1,
  한국어 = 1 << 2,
  English = 1 << 3,
  😃 = 1 << 4
};
#endif

enum number : unsigned long {
  one = 1 << 1,
  two = 1 << 2,
  three = 1 << 3,
  four = 1 << 4,

#if defined(MAGIC_ENUM_SUPPORTED_ALIASES)
  _1 = one,
  _2 = two,
  _3 = three,
  _4 = four
#endif
};
template <>
struct magic_enum::customize::enum_range<number> {
  static constexpr int min = 100;
  static constexpr int max = 300;
};

using namespace magic_enum;
using namespace magic_enum::bitwise_operators;

TEST_CASE("enum_cast") {
  SECTION("string") {
    constexpr auto cr = enum_cast<Color>("RED");
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(enum_cast<Color&>("GREEN").value() == Color::GREEN);
    REQUIRE(enum_cast<Color>("blue", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).value() == Color::BLUE);
    REQUIRE_FALSE(enum_cast<Color&>("blue|RED", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).has_value());
    REQUIRE_FALSE(enum_cast<Color&>("GREEN|RED").has_value());
    REQUIRE_FALSE(enum_cast<Color&>("GREEN|RED|RED").has_value());
    REQUIRE_FALSE(enum_cast<Color&>("GREEN|RED|None").has_value());
    REQUIRE_FALSE(enum_cast<Color>("None").has_value());

    REQUIRE(enum_flags_cast<Color&>("GREEN").value() == Color::GREEN);
    REQUIRE(enum_flags_cast<Color>("blue", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).value() == Color::BLUE);
    REQUIRE(enum_flags_cast<Color&>("blue|RED", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).value() == (Color::BLUE | Color::RED));
    REQUIRE(enum_flags_cast<Color&>("GREEN|RED").value() == (Color::GREEN | Color::RED));
    REQUIRE(enum_flags_cast<Color&>("GREEN|RED|RED").value() == (Color::GREEN | Color::RED));
    REQUIRE_FALSE(enum_flags_cast<Color&>("GREEN|RED|None").has_value());
    REQUIRE_FALSE(enum_flags_cast<Color>("None").has_value());

    constexpr auto no = enum_cast<Numbers>("one");
    REQUIRE(no.value() == Numbers::one);
    REQUIRE(enum_cast<Numbers>("two").value() == Numbers::two);
    REQUIRE(enum_cast<Numbers>("three").value() == Numbers::three);
    REQUIRE(enum_cast<Numbers>("many") == Numbers::many);
    REQUIRE_FALSE(enum_cast<Numbers>("None").has_value());

    constexpr auto dr = enum_cast<Directions>("Right");
    REQUIRE(enum_cast<Directions&>("Up").value() == Directions::Up);
    REQUIRE(enum_cast<const Directions>("Down").value() == Directions::Down);
    REQUIRE(dr.value() == Directions::Right);
    REQUIRE(enum_cast<Directions>("Left").value() == Directions::Left);
    REQUIRE_FALSE(enum_cast<Directions>("None").has_value());

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    constexpr auto lang = enum_cast<Language>("日本語");
    REQUIRE(enum_cast<Language&>("한국어").value() == Language::한국어);
    REQUIRE(enum_cast<const Language>("English").value() == Language::English);
    REQUIRE(lang.value() == Language::日本語);
    REQUIRE(enum_cast<Language>("😃").value() == Language::😃);
    REQUIRE_FALSE(enum_cast<Language>("None").has_value());
#endif

    constexpr auto nto = enum_flags_cast<number>("three|one");
    REQUIRE(enum_cast<number>("one").value() == number::one);
    REQUIRE(enum_cast<number>("two").value() == number::two);
    REQUIRE(enum_cast<number>("three").value() == number::three);
    REQUIRE(enum_cast<number>("four") == number::four);
    REQUIRE(nto.value() == (number::three | number::one));
    REQUIRE_FALSE(enum_cast<number>("None").has_value());
  }

  SECTION("integer") {
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    constexpr auto cr = enum_cast<Color>(1);
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(enum_cast<Color&>(2).value() == Color::GREEN);
    REQUIRE(enum_cast<Color>(static_cast<int>(cm[2])).value() == Color::BLUE);
    REQUIRE_FALSE(enum_cast<Color>(1 | 2).has_value());
    REQUIRE_FALSE(enum_cast<Color>(1 | 2 | 1).has_value());
    REQUIRE_FALSE(enum_cast<Color>(1 | 2 | 8).has_value());
    REQUIRE_FALSE(enum_cast<Color>(0).has_value());

    REQUIRE(enum_flags_cast<Color&>(2).value() == Color::GREEN);
    REQUIRE(enum_flags_cast<Color>(static_cast<int>(cm[2])).value() == Color::BLUE);
    REQUIRE(enum_flags_cast<Color>(1 | 2).value() == (Color::GREEN | Color::RED));
    REQUIRE(enum_flags_cast<Color>(1 | 2 | 1).value() == (Color::GREEN | Color::RED));
    REQUIRE_FALSE(enum_flags_cast<Color>(1 | 2 | 8).has_value());
    REQUIRE_FALSE(enum_flags_cast<Color>(0).has_value());

    constexpr auto no = enum_cast<Numbers>(2);
    REQUIRE(no.value() == Numbers::one);
    REQUIRE(enum_cast<Numbers>(4).value() == Numbers::two);
    REQUIRE(enum_cast<Numbers>(8).value() == Numbers::three);
    REQUIRE(enum_cast<Numbers>(1 << 30).value() == Numbers::many);
    REQUIRE_FALSE(enum_cast<Numbers>(127).has_value());
    REQUIRE_FALSE(enum_cast<Numbers>(0).has_value());

    constexpr auto dr = enum_cast<Directions>(std::uint64_t{1} << 63);
    REQUIRE(enum_cast<Directions&>(std::uint64_t{1} << 31).value() == Directions::Up);
    REQUIRE(enum_cast<const Directions>(std::uint64_t{1} << 20).value() == Directions::Down);
    REQUIRE(dr.value() == Directions::Right);
    REQUIRE(enum_cast<Directions>(std::uint64_t{1} << 10).value() == Directions::Left);
    REQUIRE_FALSE(enum_cast<Directions>(0).has_value());

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    constexpr auto lang = enum_cast<Language>(1 << 1);
    REQUIRE(enum_cast<Language&>(1 << 2).value() == Language::한국어);
    REQUIRE(enum_cast<const Language>(1 << 3).value() == Language::English);
    REQUIRE(lang.value() == Language::日本語);
    REQUIRE(enum_cast<Language>(1 << 4).value() == Language::😃);
    REQUIRE_FALSE(enum_cast<Language>(0).has_value());
#endif

    constexpr auto nto = enum_flags_cast<number>(2 | 8);
    REQUIRE(enum_cast<number>(1 << 1).value() == number::one);
    REQUIRE(enum_cast<number>(1 << 2).value() == number::two);
    REQUIRE(enum_cast<number>(1 << 3).value() == number::three);
    REQUIRE(enum_cast<number>(1 << 4).value() == number::four);
    REQUIRE(nto.value() == (number::three | number::one));
    REQUIRE_FALSE(enum_cast<number>(0).has_value());
  }
}

TEST_CASE("enum_index") {
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  constexpr auto cr = enum_index(Color::RED);
  Color cg = Color::GREEN;
  REQUIRE(cr.value() == 0);
  REQUIRE(enum_index<Color&>(cg).value() == 1);
  REQUIRE(enum_index(cm[2]).value() == 2);
  REQUIRE_FALSE(enum_index<Color>(Color::RED | Color::GREEN).has_value());
  REQUIRE_FALSE(enum_index<Color>(Color::RED | Color::GREEN | Color::RED).has_value());
  REQUIRE_FALSE(enum_index<Color>(Color::RED | Color{8}).has_value());
  REQUIRE_FALSE(enum_index(static_cast<Color>(0)).has_value());

  constexpr auto no = enum_index(Numbers::one);
  REQUIRE(no.value() == 0);
  REQUIRE(enum_index(Numbers::two).value() == 1);
  REQUIRE(enum_index(Numbers::three).value() == 2);
  REQUIRE(enum_index(Numbers::many).value() == 3);
  REQUIRE_FALSE(enum_index(static_cast<Numbers>(0)).has_value());

  constexpr auto dr = enum_index(Directions::Right);
  Directions dl = Directions::Left;
  REQUIRE(enum_index<Directions&>(dl).value() == 0);
  REQUIRE(enum_index<const Directions>(Directions::Down).value() == 1);
  REQUIRE(enum_index(Directions::Up).value() == 2);
  REQUIRE(dr.value() == 3);
  REQUIRE_FALSE(enum_index(static_cast<Directions>(0)).has_value());

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    constexpr auto lang = enum_index<Language>(Language::日本語);
    Language korean = Language::한국어;
    REQUIRE(enum_index<Language&>(korean).value() == 1);
    REQUIRE(enum_index<const Language>(Language::English).value() == 2);
    REQUIRE(enum_index(Language::😃).value() == 3);
    REQUIRE(lang.value() == 0);
    REQUIRE_FALSE(enum_index(static_cast<Language>(0)).has_value());
#endif

  constexpr auto nto = enum_index(number::three | number::one);
  REQUIRE(enum_index(number::one).value() == 0);
  REQUIRE(enum_index(number::two).value() == 1);
  REQUIRE(enum_index(number::three).value() == 2);
  REQUIRE(enum_index(number::four).value() == 3);
  REQUIRE_FALSE(nto.has_value());
  REQUIRE_FALSE(enum_index(static_cast<number>(0)).has_value());
}

TEST_CASE("enum_contains") {
  SECTION("value") {
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    constexpr auto cr = enum_contains(Color::RED);
    Color cg = Color::GREEN;
    REQUIRE(cr);
    REQUIRE(enum_contains<Color&>(cg));
    REQUIRE(enum_contains(cm[2]));
    REQUIRE(enum_contains<Color, as_flags<>>(Color::RED | Color::GREEN));
    REQUIRE(enum_contains<Color, as_flags<true>>(Color::RED | Color::GREEN | Color::GREEN));
    REQUIRE_FALSE(enum_contains<Color>(Color::RED | Color::GREEN));
    REQUIRE_FALSE(enum_contains<Color>(Color::RED | Color::GREEN | Color::GREEN));
    REQUIRE_FALSE(enum_contains<Color>(Color::RED | Color{8}));
    REQUIRE_FALSE(enum_contains(static_cast<Color>(0)));

    REQUIRE(enum_flags_contains<Color&>(cg));
    REQUIRE(enum_flags_contains(cm[2]));
    REQUIRE(enum_flags_contains<Color>(Color::RED | Color::GREEN));
    REQUIRE(enum_flags_contains<Color>(Color::RED | Color::GREEN | Color::GREEN));
    REQUIRE_FALSE(enum_flags_contains<Color>(Color::RED | Color{8}));
    REQUIRE_FALSE(enum_flags_contains(static_cast<Color>(0)));

    constexpr auto no = enum_contains(Numbers::one);
    REQUIRE(no);
    REQUIRE(enum_contains(Numbers::two));
    REQUIRE(enum_contains(Numbers::three));
    REQUIRE(enum_contains(Numbers::many));
    REQUIRE_FALSE(enum_contains(static_cast<Numbers>(0)));

    constexpr auto dr = enum_contains(Directions::Right);
    Directions dl = Directions::Left;
    REQUIRE(enum_contains<Directions&>(dl));
    REQUIRE(enum_contains<const Directions>(Directions::Down));
    REQUIRE(enum_contains(Directions::Up));
    REQUIRE(dr);
    REQUIRE_FALSE(enum_contains(static_cast<Directions>(0)));

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    constexpr auto lang = enum_index<Language>(Language::日本語);
    Language korean = Language::한국어;
    REQUIRE(enum_contains<Language&>(korean));
    REQUIRE(enum_contains<const Language>(Language::English));
    REQUIRE(enum_contains(Language::😃));
    REQUIRE(lang);
    REQUIRE_FALSE(enum_contains(static_cast<Language>(0)));
#endif

    constexpr auto nto = enum_contains(number::three | number::one);
    REQUIRE(enum_contains(number::one));
    REQUIRE(enum_contains<number&>(number::two));
    REQUIRE(enum_contains(number::one));
    REQUIRE(enum_contains(number::four));
    REQUIRE_FALSE(nto);
    REQUIRE_FALSE(enum_contains(static_cast<number>(0)));

    REQUIRE(enum_flags_contains(number::three | number::one));
  }

  SECTION("integer") {
    REQUIRE(enum_contains<Color>(1));
    REQUIRE(enum_contains<Color&>(2));
    REQUIRE(enum_contains<const Color>(4));
    REQUIRE(enum_contains<Color, as_flags<>>(1 | 2));
    REQUIRE(enum_contains<Color, as_flags<true>>(1 | 2 | 1));
    REQUIRE_FALSE(enum_contains<Color>(1 | 2));
    REQUIRE_FALSE(enum_contains<Color>(1 | 2 | 1));
    REQUIRE_FALSE(enum_contains<Color>(1 | 2 | 8));
    REQUIRE_FALSE(enum_contains<Color>(0));

    REQUIRE(enum_flags_contains<Color>(1));
    REQUIRE(enum_flags_contains<Color&>(2));
    REQUIRE(enum_flags_contains<Color>(4));
    REQUIRE(enum_flags_contains<Color>(1 | 2));
    REQUIRE(enum_flags_contains<Color>(1 | 2 | 1));
    REQUIRE_FALSE(enum_flags_contains<Color>(1 | 2 | 8));
    REQUIRE_FALSE(enum_flags_contains<Color>(0));

    constexpr auto no = enum_contains<Numbers>(1 << 1);
    REQUIRE(no);
    REQUIRE(enum_contains<Numbers>(1 << 2));
    REQUIRE(enum_contains<Numbers>(1 << 3));
    REQUIRE(enum_contains<Numbers>(1 << 30));

    constexpr auto dr = enum_contains<Directions&>(std::uint64_t{1} << 63);
    REQUIRE(dr);
    REQUIRE(enum_contains<const Directions>(std::uint64_t{1} << 10));
    REQUIRE(enum_contains<Directions>(std::uint64_t{1} << 20));
    REQUIRE(enum_contains<Directions>(std::uint64_t{1} << 31));
    REQUIRE_FALSE(enum_contains<Directions>(static_cast<Directions>(0)));

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    constexpr auto lang = enum_contains<Language&>(1 << 1);
    REQUIRE(lang);
    REQUIRE(enum_contains<const Language>(1 << 2));
    REQUIRE(enum_contains<Language>(1 << 3));
    REQUIRE(enum_contains<Language>(1 << 4));
    REQUIRE_FALSE(enum_contains(static_cast<Language>(0)));
#endif

    constexpr auto nto = enum_contains<number>(8 | 2);
    REQUIRE(enum_contains<number>(1 << 1));
    REQUIRE(enum_contains<number>(1 << 2));
    REQUIRE(enum_contains<number>(1 << 3));
    REQUIRE(enum_contains<number>(1 << 4));
    REQUIRE_FALSE(enum_contains<number>(8 | 2 | 16));
    REQUIRE_FALSE(enum_contains<number>(8 | 16 | 16));
    REQUIRE_FALSE(nto);
    REQUIRE_FALSE(enum_contains<number>(8 | 64));
    REQUIRE_FALSE(enum_contains<number>(0));
  }

  SECTION("string") {
    constexpr auto cr = "RED";
    REQUIRE(enum_contains<Color>(cr));
    REQUIRE(enum_contains<Color&>("GREEN"));
    REQUIRE(enum_contains<const Color>("blue", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }));
    REQUIRE(enum_contains<Color&, as_flags<>>("blue|RED", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }));
    REQUIRE(enum_contains<Color&, as_flags<true>>("GREEN|RED"));
    REQUIRE(enum_contains<Color, as_flags<true>>("GREEN|RED|RED"));
    REQUIRE_FALSE(enum_contains<Color&>("blue|RED", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }));
    REQUIRE_FALSE(enum_contains<Color&>("GREEN|RED"));
    REQUIRE_FALSE(enum_contains<Color&>("GREEN|RED|RED"));
    REQUIRE_FALSE(enum_contains<Color>("GREEN|RED|None"));
    REQUIRE_FALSE(enum_contains<Color>("None"));

    REQUIRE(enum_flags_contains<Color&>("GREEN"));
    REQUIRE(enum_flags_contains<Color>("blue", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }));
    REQUIRE(enum_flags_contains<Color>("blue|RED", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }));
    REQUIRE(enum_flags_contains<Color>("GREEN|RED"));
    REQUIRE(enum_flags_contains<Color>("GREEN|RED|RED"));
    REQUIRE_FALSE(enum_flags_contains<Color>("GREEN|RED|None"));
    REQUIRE_FALSE(enum_flags_contains<Color>("None"));

    constexpr auto no = std::string_view{"one"};
    REQUIRE(enum_contains<Numbers>(no));
    REQUIRE(enum_contains<Numbers>("two"));
    REQUIRE(enum_contains<Numbers>("three"));
    REQUIRE(enum_contains<Numbers>("many"));
    REQUIRE_FALSE(enum_contains<Numbers>("None"));

    auto dr = std::string{"Right"};
    REQUIRE(enum_contains<Directions&>("Up"));
    REQUIRE(enum_contains<Directions>("Down"));
    REQUIRE(enum_contains<const Directions>(dr));
    REQUIRE(enum_contains<Directions>("Left"));
    REQUIRE_FALSE(enum_contains<Directions>("None"));

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    auto lang = std::string{"日本語"};
    REQUIRE(enum_contains<Language&>("한국어"));
    REQUIRE(enum_contains<Language>("English"));
    REQUIRE(enum_contains<const Language>(lang));
    REQUIRE(enum_contains<Language>("😃"));
    REQUIRE_FALSE(enum_contains<Language>("None"));
#endif

    constexpr auto nto = enum_contains<number>("three|one");
    REQUIRE(enum_contains<number>("one"));
    REQUIRE(enum_contains<number>("two"));
    REQUIRE(enum_contains<number>("three"));
    REQUIRE(enum_contains<number>("four"));
    REQUIRE_FALSE(nto);
    REQUIRE_FALSE(enum_contains<number>("None"));

    REQUIRE(enum_flags_contains<number>("three|one"));
  }
}

TEST_CASE("enum_value") {
  constexpr auto cr = enum_value<Color>(0);
  REQUIRE(cr == Color::RED);
  REQUIRE(enum_value<Color&>(1) == Color::GREEN);
  REQUIRE(enum_value<Color>(2) == Color::BLUE);

  REQUIRE(enum_value<Color, 0>() == Color::RED);
  REQUIRE(enum_value<Color, 1>() == Color::GREEN);
  REQUIRE(enum_value<Color, 2>() == Color::BLUE);

  constexpr auto no = enum_value<Numbers>(0);
  REQUIRE(no == Numbers::one);
  REQUIRE(enum_value<Numbers>(1) == Numbers::two);
  REQUIRE(enum_value<Numbers>(2) == Numbers::three);
  REQUIRE(enum_value<Numbers>(3) == Numbers::many);

  REQUIRE(enum_value<Numbers, 0>() == Numbers::one);
  REQUIRE(enum_value<Numbers, 1>() == Numbers::two);
  REQUIRE(enum_value<Numbers, 2>() == Numbers::three);
  REQUIRE(enum_value<Numbers, 3>() == Numbers::many);

  constexpr auto dr = enum_value<Directions>(3);
  REQUIRE(enum_value<Directions&>(0) == Directions::Left);
  REQUIRE(enum_value<const Directions>(1) == Directions::Down);
  REQUIRE(enum_value<Directions>(2) == Directions::Up);
  REQUIRE(dr == Directions::Right);

  REQUIRE(enum_value<Directions, 0>() == Directions::Left);
  REQUIRE(enum_value<Directions, 1>() == Directions::Down);
  REQUIRE(enum_value<Directions, 2>() == Directions::Up);
  REQUIRE(enum_value<Directions, 3>() == Directions::Right);

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  constexpr auto lang = enum_value<Language>(3);
  REQUIRE(enum_value<Language&>(0) == Language::日本語);
  REQUIRE(enum_value<const Language>(1) == Language::한국어);
  REQUIRE(enum_value<Language>(2) == Language::English);
  REQUIRE(lang == Language::😃);
#endif

  constexpr auto nt = enum_value<number>(2);
  REQUIRE(enum_value<number>(0) == number::one);
  REQUIRE(enum_value<number>(1) == number::two);
  REQUIRE(nt == number::three);
  REQUIRE(enum_value<number>(3) == number::four);

  REQUIRE(enum_value<number, 0>() == number::one);
  REQUIRE(enum_value<number, 1>() == number::two);
  REQUIRE(enum_value<number, 2>() == number::three);
  REQUIRE(enum_value<number, 3>() == number::four);
}

TEST_CASE("enum_values") {
  REQUIRE(std::is_same_v<decltype(magic_enum::enum_values<Color>()), const std::array<Color, 3>&>);

  constexpr auto& s1 = enum_values<Color&>();
  REQUIRE(s1 == std::array<Color, 3>{{Color::RED, Color::GREEN, Color::BLUE}});

  constexpr auto& s2 = enum_values<Numbers>();
  REQUIRE(s2 == std::array<Numbers, 4>{{Numbers::one, Numbers::two, Numbers::three, Numbers::many}});

  constexpr auto& s3 = enum_values<const Directions>();
  REQUIRE(s3 == std::array<Directions, 4>{{Directions::Left, Directions::Down, Directions::Up, Directions::Right}});

  constexpr auto& s4 = enum_values<number>();
  REQUIRE(s4 == std::array<number, 4>{{number::one, number::two, number::three, number::four}});

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  constexpr auto& s5 = enum_values<const Language>();
  REQUIRE(s5 == std::array<Language, 4>{{Language::日本語, Language::한국어, Language::English, Language::😃}});
#endif
}

TEST_CASE("enum_count") {
  constexpr auto s1 = enum_count<Color&>();
  REQUIRE(s1 == 3);

  constexpr auto s2 = enum_count<Numbers>();
  REQUIRE(s2 == 4);

  constexpr auto s3 = enum_count<const Directions>();
  REQUIRE(s3 == 4);

  constexpr auto s4 = enum_count<number>();
  REQUIRE(s4 == 4);

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  constexpr auto s5 = enum_count<Language>();
  REQUIRE(s5 == 4);
#endif
}

TEST_CASE("enum_name") {
  SECTION("automatic storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = enum_name(cr);
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    Color cb = Color::BLUE;
    REQUIRE(cr_name == "RED");
    REQUIRE(enum_name<Color&>(cb) == "BLUE");
    REQUIRE(enum_name(cm[1]) == "GREEN");
    REQUIRE(enum_name(Color::RED | Color{0}) == "RED");
    REQUIRE(enum_name(Color::RED | Color::GREEN).empty());
    REQUIRE(enum_name(Color::RED | Color{8}).empty());
    REQUIRE(enum_name(static_cast<Color>(0)).empty());

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = enum_name(no);
    REQUIRE(no_name == "one");
    REQUIRE(enum_name(Numbers::two) == "two");
    REQUIRE(enum_name(Numbers::three) == "three");
    REQUIRE(enum_name(Numbers::many) == "many");
    REQUIRE(enum_name(Numbers::many | Numbers::two).empty());
    REQUIRE(enum_name(static_cast<Numbers>(0)).empty());

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = enum_name(dr);
    Directions du = Directions::Up;
    REQUIRE(enum_name<Directions&>(du) == "Up");
    REQUIRE(enum_name<const Directions>(Directions::Down) == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(enum_name(Directions::Left) == "Left");
    REQUIRE(enum_name(Directions::Right | Directions::Up | Directions::Left | Directions::Down).empty());
    REQUIRE(enum_name(static_cast<Directions>(0)).empty());

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    constexpr Language lang = Language::日本語;
    constexpr auto lang_name = enum_name(lang);
    Language lk = Language::한국어;
    REQUIRE(enum_name<Language&>(lk) == "한국어");
    REQUIRE(enum_name<const Language>(Language::English) == "English");
    REQUIRE(lang_name == "日本語");
    REQUIRE(enum_name(Language::😃) == "😃");
    REQUIRE(enum_name(static_cast<Language>(0)).empty());
#endif

    constexpr number nto = number::three | number::one;
    constexpr auto nto_name = enum_name(nto);
    REQUIRE(enum_name(number::one) == "one");
    REQUIRE(enum_name(number::two) == "two");
    REQUIRE(enum_name(number::three) == "three");
    REQUIRE(enum_name(number::four) == "four");
    REQUIRE(nto_name.empty());
    REQUIRE(enum_name(static_cast<number>(0)).empty());
  }
}

TEST_CASE("enum_flags_name") {
  constexpr Color cr = Color::RED;
  auto cr_name = enum_flags_name(cr);
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  Color cb = Color::BLUE;
  REQUIRE(cr_name == "RED");
  REQUIRE(enum_flags_name<Color&>(cb) == "BLUE");
  REQUIRE(enum_flags_name(cm[1]) == "GREEN");
  REQUIRE(enum_flags_name(Color::RED | Color{0}) == "RED");
  REQUIRE(enum_flags_name(Color::RED | Color::GREEN) == "RED|GREEN");
  REQUIRE(enum_flags_name(Color::RED | Color{8}).empty());
  REQUIRE(enum_flags_name(static_cast<Color>(0)).empty());

  constexpr Numbers no = Numbers::one;
  auto no_name = enum_flags_name(no);
  REQUIRE(no_name == "one");
  REQUIRE(enum_flags_name(Numbers::two) == "two");
  REQUIRE(enum_flags_name(Numbers::three) == "three");
  REQUIRE(enum_flags_name(Numbers::many) == "many");
  REQUIRE(enum_flags_name(Numbers::many | Numbers::two) == "two|many");
  REQUIRE(enum_flags_name(static_cast<Numbers>(0)).empty());

  constexpr Directions dr = Directions::Right;
  auto dr_name = enum_flags_name(dr);
  Directions du = Directions::Up;
  REQUIRE(enum_flags_name<Directions&>(du) == "Up");
  REQUIRE(enum_flags_name<const Directions>(Directions::Down) == "Down");
  REQUIRE(dr_name == "Right");
  REQUIRE(enum_flags_name(Directions::Left) == "Left");
  REQUIRE(enum_flags_name(Directions::Right | Directions::Up | Directions::Left | Directions::Down) == "Left|Down|Up|Right");
  REQUIRE(enum_flags_name(static_cast<Directions>(0)).empty());

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  constexpr Language lang = Language::日本語;
  auto lang_name = enum_flags_name(lang);
  Language lk = Language::한국어;
  REQUIRE(enum_flags_name<Language&>(lk) == "한국어");
  REQUIRE(enum_flags_name<const Language>(Language::English) == "English");
  REQUIRE(lang_name == "日本語");
  REQUIRE(enum_flags_name(Language::😃) == "😃");
  REQUIRE(enum_flags_name(static_cast<Language>(0)).empty());
#endif

  constexpr number nto = number::three | number::one;
  auto nto_name = enum_flags_name(nto);
  REQUIRE(enum_flags_name(number::one) == "one");
  REQUIRE(enum_flags_name(number::two) == "two");
  REQUIRE(enum_flags_name(number::three) == "three");
  REQUIRE(enum_flags_name(number::four) == "four");
  REQUIRE(nto_name == "one|three");
  REQUIRE(enum_flags_name(static_cast<number>(0)).empty());
}

TEST_CASE("enum_names") {
  REQUIRE(std::is_same_v<decltype(magic_enum::enum_names<Color>()), const std::array<std::string_view, 3>&>);

  constexpr auto& s1 = enum_names<Color&>();
  REQUIRE(s1 == std::array<std::string_view, 3>{{"RED", "GREEN", "BLUE"}});

  constexpr auto& s2 = enum_names<Numbers>();
  REQUIRE(s2 == std::array<std::string_view, 4>{{"one", "two", "three", "many"}});

  constexpr auto& s3 = enum_names<const Directions>();
  REQUIRE(s3 == std::array<std::string_view, 4>{{"Left", "Down", "Up", "Right"}});

  constexpr auto& s4 = enum_names<number>();
  REQUIRE(s4 == std::array<std::string_view, 4>{{"one", "two", "three", "four"}});

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  constexpr auto& s5 = enum_names<const Language>();
  REQUIRE(s5 == std::array<std::string_view, 4>{{"日本語", "한국어", "English", "😃"}});
#endif
}

TEST_CASE("enum_entries") {
  REQUIRE(std::is_same_v<decltype(magic_enum::enum_entries<Color>()), const std::array<std::pair<Color, std::string_view>, 3>&>);

  constexpr auto& s1 = enum_entries<Color&>();
  REQUIRE(s1 == std::array<std::pair<Color, std::string_view>, 3>{{{Color::RED, "RED"}, {Color::GREEN, "GREEN"}, {Color::BLUE, "BLUE"}}});

  constexpr auto& s2 = enum_entries<Numbers>();
  REQUIRE(s2 == std::array<std::pair<Numbers, std::string_view>, 4>{{{Numbers::one, "one"}, {Numbers::two, "two"}, {Numbers::three, "three"}, {Numbers::many, "many"}}});

  constexpr auto& s3 = enum_entries<Directions&>();
  REQUIRE(s3 == std::array<std::pair<Directions, std::string_view>, 4>{{{Directions::Left, "Left"}, {Directions::Down, "Down"}, {Directions::Up, "Up"}, {Directions::Right, "Right"}}});

  constexpr auto& s4 = enum_entries<number>();
  REQUIRE(s4 == std::array<std::pair<number, std::string_view>, 4>{{{number::one, "one"}, {number::two, "two"}, {number::three, "three"}, {number::four, "four"}}});

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  constexpr auto& s5 = enum_entries<const Language>();
  REQUIRE(s5 == std::array<std::pair<Language, std::string_view>, 4>{{{Language::日本語, "日本語"}, {Language::한국어, "한국어"}, {Language::English, "English"}, {Language::😃, "😃"}}});
#endif
}

TEST_CASE("ostream_operators") {
  auto test_ostream = [](auto e, std::string name) {
    using namespace magic_enum::ostream_operators;
    std::stringstream ss;
    ss << e;
    REQUIRE(ss.str() == name);
  };

  test_ostream(std::make_optional(Color::RED), "RED");
  test_ostream(Color::GREEN, "GREEN");
  test_ostream(Color::BLUE, "BLUE");
  test_ostream(Color::BLUE | Color::RED, "RED|BLUE");
  test_ostream(Color::BLUE | Color::RED | Color::RED, "RED|BLUE");
  test_ostream(static_cast<Color>(0), "0");
  test_ostream(std::make_optional(static_cast<Color>(0)), "0");

  test_ostream(std::make_optional(Numbers::one), "one");
  test_ostream(Numbers::two, "two");
  test_ostream(Numbers::three, "three");
  test_ostream(Numbers::many, "many");
  test_ostream(static_cast<Numbers>(0), "0");
  test_ostream(std::make_optional(static_cast<Numbers>(0)), "0");

  test_ostream(std::make_optional(Directions::Up), "Up");
  test_ostream(Directions::Down, "Down");
  test_ostream(Directions::Right, "Right");
  test_ostream(Directions::Left, "Left");
  test_ostream(Directions::Right | Directions::Left, "Left|Right");
  test_ostream(static_cast<Directions>(0), "0");
  test_ostream(std::make_optional(static_cast<Directions>(0)), "0");

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  test_ostream(std::make_optional(Language::日本語), "日本語");
  test_ostream(Language::한국어, "한국어");
  test_ostream(Language::English, "English");
  test_ostream(Language::😃, "😃");
  test_ostream(static_cast<Language>(0), "0");
  test_ostream(std::make_optional(static_cast<Language>(0)), "0");
#endif

  test_ostream(std::make_optional(number::one), "one");
  test_ostream(number::two, "two");
  test_ostream(number::three, "three");
  test_ostream(number::four, "four");
  test_ostream(number::four | number::one, "one|four");
  test_ostream(static_cast<number>(0), "0");
  test_ostream(std::make_optional(static_cast<number>(0)), "0");
}

TEST_CASE("istream_operators") {
  auto test_istream = [](const auto e, std::string name) {
    using namespace magic_enum::istream_operators;
    std::istringstream ss(name);
    std::decay_t<decltype(e)> v;
    ss >> v;
    REQUIRE(v == e);
    REQUIRE(ss);
  };

  test_istream(Color::GREEN, "GREEN");
  test_istream(Color::BLUE, "BLUE");
  test_istream(Color::BLUE | Color::RED, "RED|BLUE");
  test_istream(Color::BLUE | Color::RED | Color::RED, "RED|BLUE");

  test_istream(Numbers::two, "two");
  test_istream(Numbers::three, "three");
  test_istream(Numbers::many, "many");

  test_istream(Directions::Down, "Down");
  test_istream(Directions::Right, "Right");
  test_istream(Directions::Left, "Left");
  test_istream(Directions::Right | Directions::Left, "Left|Right");

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  test_istream(Language::한국어, "한국어");
  test_istream(Language::English, "English");
  test_istream(Language::😃, "😃");
#endif

  test_istream(number::two, "two");
  test_istream(number::three, "three");
  test_istream(number::four, "four");
  test_istream(number::four | number::one, "one|four");
}

TEST_CASE("bitwise_operators") {
  SECTION("operator^") {
    REQUIRE(enum_integer(~Color::RED) == ~enum_integer(Color::RED));
    REQUIRE(enum_integer(~Numbers::one) == ~enum_integer(Numbers::one));
    REQUIRE(enum_integer(~Directions::Up) == ~enum_integer(Directions::Up));
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    REQUIRE(enum_integer(~Language::日本語) == ~enum_integer(Language::日本語));
#endif
    REQUIRE(enum_integer(~number::one) == ~enum_integer(number::one));
  }

  SECTION("operator|") {
    REQUIRE(enum_integer(Color::RED | Color::BLUE) == (enum_integer(Color::RED) | enum_integer(Color::BLUE)));
    REQUIRE(enum_integer(Numbers::one | Numbers::two) == (enum_integer(Numbers::one) | enum_integer(Numbers::two)));
    REQUIRE(enum_integer(Directions::Up | Directions::Down) == (enum_integer(Directions::Up) | enum_integer(Directions::Down)));
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    REQUIRE(enum_integer(Language::日本語 | Language::한국어) == (enum_integer(Language::日本語) | enum_integer(Language::한국어)));
#endif
    REQUIRE(enum_integer(number::one | number::two) == (enum_integer(number::one) | enum_integer(number::two)));
  }

  SECTION("operator&") {
    REQUIRE(enum_integer(Color::RED & Color::BLUE) == (enum_integer(Color::RED) & enum_integer(Color::BLUE)));
    REQUIRE(enum_integer(Numbers::one & Numbers::two) == (enum_integer(Numbers::one) & enum_integer(Numbers::two)));
    REQUIRE(enum_integer(Directions::Up & Directions::Down) == (enum_integer(Directions::Up) & enum_integer(Directions::Down)));
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    REQUIRE(enum_integer(Language::日本語 & Language::한국어) == (enum_integer(Language::日本語) & enum_integer(Language::한국어)));
#endif
    REQUIRE(enum_integer(number::one & number::two) == (enum_integer(number::one) & enum_integer(number::two)));
  }

  SECTION("operator^") {
    REQUIRE(enum_integer(Color::RED ^ Color::BLUE) == (enum_integer(Color::RED) ^ enum_integer(Color::BLUE)));
    REQUIRE(enum_integer(Numbers::one ^ Numbers::two) == (enum_integer(Numbers::one) ^ enum_integer(Numbers::two)));
    REQUIRE(enum_integer(Directions::Up ^ Directions::Down) == (enum_integer(Directions::Up) ^ enum_integer(Directions::Down)));
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    REQUIRE(enum_integer(Language::日本語 ^ Language::한국어) == (enum_integer(Language::日本語) ^ enum_integer(Language::한국어)));
#endif
    REQUIRE(enum_integer(number::one ^ number::two) == (enum_integer(number::one) ^ enum_integer(number::two)));
  }

  SECTION("operator|=") {
    Color x1 = Color::RED;
    x1 |= Color::BLUE;
    REQUIRE(enum_integer(x1) == (enum_integer(Color::RED) | enum_integer(Color::BLUE)));

    Numbers x2 = Numbers::one;
    x2 |= Numbers::two;
    REQUIRE(enum_integer(x2) == (enum_integer(Numbers::one) | enum_integer(Numbers::two)));

    Directions x3 = Directions::Up;
    x3 |= Directions::Down;
    REQUIRE(enum_integer(x3) == (enum_integer(Directions::Up) | enum_integer(Directions::Down)));

    number x4 = number::one;
    x4 |= number::two;
    REQUIRE(enum_integer(x4) == (enum_integer(number::one) | enum_integer(number::two)));

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    Language x5 = Language::日本語;
    x5 |= Language::한국어;
    REQUIRE(enum_integer(x5) == (enum_integer(Language::日本語) | enum_integer(Language::한국어)));
#endif
  }

  SECTION("operator&=") {
    Color x1 = Color::RED;
    x1 &= Color::BLUE;
    REQUIRE(enum_integer(x1) == (enum_integer(Color::RED) & enum_integer(Color::BLUE)));

    Numbers x2 = Numbers::one;
    x2 &= Numbers::two;
    REQUIRE(enum_integer(x2) == (enum_integer(Numbers::one) & enum_integer(Numbers::two)));

    Directions x3 = Directions::Up;
    x3 &= Directions::Down;
    REQUIRE(enum_integer(x3) == (enum_integer(Directions::Up) & enum_integer(Directions::Down)));

    number x4 = number::one;
    x4 &= number::two;
    REQUIRE(enum_integer(x4) == (enum_integer(number::one) & enum_integer(number::two)));

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    Language x5 = Language::日本語;
    x5 &= Language::한국어;
    REQUIRE(enum_integer(x5) == (enum_integer(Language::日本語) & enum_integer(Language::한국어)));
#endif
  }

  SECTION("operator^=") {
    Color x1 = Color::RED;
    x1 ^= Color::BLUE;
    REQUIRE(enum_integer(x1) == (enum_integer(Color::RED) ^ enum_integer(Color::BLUE)));

    Numbers x2 = Numbers::one;
    x2 ^= Numbers::two;
    REQUIRE(enum_integer(x2) == (enum_integer(Numbers::one) ^ enum_integer(Numbers::two)));

    Directions x3 = Directions::Up;
    x3 ^= Directions::Down;
    REQUIRE(enum_integer(x3) == (enum_integer(Directions::Up) ^ enum_integer(Directions::Down)));

    number x4 = number::one;
    x4 ^= number::two;
    REQUIRE(enum_integer(x4) == (enum_integer(number::one) ^ enum_integer(number::two)));

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    Language x5 = Language::日本語;
    x5 ^= Language::한국어;
    REQUIRE(enum_integer(x5) == (enum_integer(Language::日本語) ^ enum_integer(Language::한국어)));
#endif
  }
}

#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1920
#  define MAGIC_ENUM_SUPPORTED_CONSTEXPR_FOR 1
#endif

#if defined(MAGIC_ENUM_SUPPORTED_CONSTEXPR_FOR)

// from https://artificial-mind.net/blog/2020/10/31/constexpr-for
template <auto Start, auto End, auto Inc, class F>
constexpr void constexpr_for(F&& f) {
  if constexpr (Start < End) {
    f(std::integral_constant<decltype(Start), Start>());
    constexpr_for<Start + Inc, End, Inc>(f);
  }
}

template <typename E, E V>
struct Foo {};

TEST_CASE("constexpr_for") {
  constexpr_for<0, magic_enum::enum_count<Color>(), 1>([](auto i) {
    [[maybe_unused]] Foo<Color, magic_enum::enum_value<Color, i>()> bar{};
  });
}

#endif

#if defined(__cpp_lib_format)

#include <magic_enum_format.hpp>

TEST_CASE("format-base") {
  REQUIRE(std::format("Test-{:~^11}.", Color::RED | Color::GREEN) == "Test-~RED|GREEN~.");
}

#endif
