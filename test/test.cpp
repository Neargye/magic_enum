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

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#undef  MAGIC_ENUM_RANGE_MIN
#define MAGIC_ENUM_RANGE_MIN -120
#undef  MAGIC_ENUM_RANGE_MAX
#define MAGIC_ENUM_RANGE_MAX 120
#include <magic_enum.hpp>
#include <magic_enum_fuse.hpp>

#include <array>
#include <cctype>
#include <string_view>
#include <sstream>

enum class Color { RED = -12, GREEN = 7, BLUE = 15 };
template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<Color>(Color value) noexcept {
  switch (value) {
    case Color::RED:
      return "red";
    default:
      return default_tag;
  }
}

enum class Numbers : int { one = 1, two, three, many = 127 };

enum Directions { Up = 85, Down = -42, Right = 120, Left = -120 };

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
enum class Language : int { 日本語 = 10, 한국어 = 20, English = 30, 😃 = 40 };
#endif

enum number : unsigned long {
  one = 100,
  two = 200,
  three = 300,
  four = 400,

#if defined(MAGIC_ENUM_SUPPORTED_ALIASES)
  _1 = one,
  _2 = two,
  _3 = three,
  _4 = four
#endif
};

enum class crc_hack {
  b5a7b602ab754d7ab30fb42c4fb28d82
};

enum class crc_hack_2 {
  b5a7b602ab754d7ab30fb42c4fb28d82,
  d19f2e9e82d14b96be4fa12b8a27ee9f
};

template <>
struct magic_enum::customize::enum_range<number> {
  static constexpr int min = 100;
  static constexpr int max = 300;
};

enum class MaxUsedAsInvalid : std::uint8_t {
  ONE,
  TWO = 63,
  INVALID = std::numeric_limits<std::uint8_t>::max()
};
template <>
struct magic_enum::customize::enum_range<MaxUsedAsInvalid> {
  static constexpr int min = 0;
  static constexpr int max = 64;
};

enum class Binary : bool {
  ONE,
  TWO
};
template <>
struct magic_enum::customize::enum_range<Binary> {
  static constexpr int min = 0;
  static constexpr int max = 64;
};

enum class BoolTest : bool { Yay, Nay };

using namespace magic_enum;

static_assert(is_magic_enum_supported, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");

TEST_CASE("enum_cast") {
  SECTION("string") {
    constexpr auto cr = enum_cast<Color>("red");
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(enum_cast<Color&>("GREEN").value() == Color::GREEN);
    REQUIRE(enum_cast<Color>("blue", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).value() == Color::BLUE);
    REQUIRE_FALSE(enum_cast<Color>("None").has_value());

    constexpr auto no = enum_cast<Numbers>("one");
    REQUIRE(no.value() == Numbers::one);
    REQUIRE(enum_cast<Numbers>("two").value() == Numbers::two);
    REQUIRE(enum_cast<Numbers>("three").value() == Numbers::three);
    REQUIRE_FALSE(enum_cast<Numbers>("many").has_value());
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
    REQUIRE_FALSE(enum_cast<Language>("Französisch").has_value());
#else // !defined(MAGIC_ENUM_ENABLE_NONASCII)
    constexpr auto dr2 = enum_cast<Directions>("RIGHT", case_insensitive);
    REQUIRE(dr2.value() == Directions::Right);
    REQUIRE(enum_cast<Directions&>("up", case_insensitive).value() == Directions::Up);
    REQUIRE(enum_cast<const Directions>("dOwN", case_insensitive).value() == Directions::Down);
    REQUIRE_FALSE(enum_cast<Directions>("Left-", case_insensitive).has_value());
#endif

    constexpr auto nt = enum_cast<number>("three");
    REQUIRE(enum_cast<number>("one").value() == number::one);
    REQUIRE(enum_cast<number>("two").value() == number::two);
    REQUIRE(nt.value() == number::three);
    REQUIRE_FALSE(enum_cast<number>("four").has_value());
    REQUIRE_FALSE(enum_cast<number>("None").has_value());

    REQUIRE(magic_enum::enum_cast<crc_hack>("b5a7b602ab754d7ab30fb42c4fb28d82").has_value());
    REQUIRE_FALSE(magic_enum::enum_cast<crc_hack>("d19f2e9e82d14b96be4fa12b8a27ee9f").has_value());

    constexpr auto crc = magic_enum::enum_cast<crc_hack_2>("b5a7b602ab754d7ab30fb42c4fb28d82");
    REQUIRE(crc.value() == crc_hack_2::b5a7b602ab754d7ab30fb42c4fb28d82);
    REQUIRE(magic_enum::enum_cast<crc_hack_2>("d19f2e9e82d14b96be4fa12b8a27ee9f").value() == crc_hack_2::d19f2e9e82d14b96be4fa12b8a27ee9f);

    REQUIRE(magic_enum::enum_cast<BoolTest>("Nay").has_value());
  }

  SECTION("integer") {
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    constexpr auto cr = enum_cast<Color>(-12);
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(enum_cast<Color&>(7).value() == Color::GREEN);
    REQUIRE(enum_cast<Color>(static_cast<int>(cm[2])).value() == Color::BLUE);
    REQUIRE_FALSE(enum_cast<Color>(0).has_value());

    constexpr auto no = enum_cast<Numbers>(1);
    REQUIRE(no.value() == Numbers::one);
    REQUIRE(enum_cast<Numbers>(2).value() == Numbers::two);
    REQUIRE(enum_cast<Numbers>(3).value() == Numbers::three);
    REQUIRE_FALSE(enum_cast<Numbers>(127).has_value());
    REQUIRE_FALSE(enum_cast<Numbers>(0).has_value());

    constexpr auto dr = enum_cast<Directions>(120);
    REQUIRE(enum_cast<Directions&>(85).value() == Directions::Up);
    REQUIRE(enum_cast<const Directions>(-42).value() == Directions::Down);
    REQUIRE(dr.value() == Directions::Right);
    REQUIRE(enum_cast<Directions>(-120).value() == Directions::Left);
    REQUIRE_FALSE(enum_cast<Directions>(0).has_value());

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    constexpr auto lang = enum_cast<Language>(10);
    REQUIRE(enum_cast<Language&>(20).value() == Language::한국어);
    REQUIRE(enum_cast<const Language>(30).value() == Language::English);
    REQUIRE(lang.value() == Language::日本語);
    REQUIRE(enum_cast<Language>(40).value() == Language::😃);
    REQUIRE_FALSE(enum_cast<Language>(0).has_value());
#endif

    constexpr auto nt = enum_cast<number>(300);
    REQUIRE(enum_cast<number>(100).value() == number::one);
    REQUIRE(enum_cast<number>(200).value() == number::two);
    REQUIRE(nt.value() == number::three);
    REQUIRE_FALSE(enum_cast<number>(400).has_value());
    REQUIRE_FALSE(enum_cast<number>(0).has_value());

    REQUIRE(enum_cast<BoolTest>(false).has_value());
    REQUIRE(enum_cast<BoolTest>(0).has_value());
  }
}

TEST_CASE("enum_integer") {
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  constexpr auto cr = enum_integer(Color::RED);
  Color cg = Color::GREEN;
  REQUIRE(cr == -12);
  REQUIRE(enum_integer<Color&>(cg) == 7);
  REQUIRE(enum_integer(cm[2]) == 15);
  REQUIRE(enum_integer(static_cast<Color>(0)) == 0);

  constexpr auto no = enum_integer(Numbers::one);
  REQUIRE(no == 1);
  REQUIRE(enum_integer(Numbers::two) == 2);
  REQUIRE(enum_integer(Numbers::three) == 3);
  REQUIRE(enum_integer(Numbers::many) == 127);
  REQUIRE(enum_integer(static_cast<Numbers>(0)) == 0);

  constexpr auto dr = enum_integer(Directions::Right);
  Directions dl = Directions::Left;
  REQUIRE(enum_integer<Directions&>(dl) == -120);
  REQUIRE(enum_integer<const Directions>(Directions::Down) == -42);
  REQUIRE(enum_integer(Directions::Up) == 85);
  REQUIRE(dr == 120);
  REQUIRE(enum_integer(static_cast<Directions>(0)) == 0);

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    constexpr auto lang = enum_integer(Language::日本語);
    Language korean = Language::한국어;
    REQUIRE(enum_integer<Language&>(korean) == 20);
    REQUIRE(enum_integer<const Language>(Language::English) == 30);
    REQUIRE(enum_integer(Language::😃) == 40);
    REQUIRE(lang == 10);
    REQUIRE(enum_integer(static_cast<Language>(0)) == 0);
#endif

  constexpr auto nt = enum_integer(number::three);
  REQUIRE(enum_integer(number::one) == 100);
  REQUIRE(enum_integer(number::two) == 200);
  REQUIRE(nt == 300);
  REQUIRE(enum_integer(number::four) == 400);
  REQUIRE(enum_integer(static_cast<number>(0)) == 0);

  REQUIRE(enum_integer(BoolTest::Yay) == false);
}

TEST_CASE("enum_index") {
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  constexpr auto cr = enum_index(Color::RED);
  Color cg = Color::GREEN;
  REQUIRE(cr.value() == 0);
  REQUIRE(enum_index<Color::RED>() == 0);
  REQUIRE(enum_index<Color&>(cg).value() == 1);
  REQUIRE(enum_index(cm[2]).value() == 2);
  REQUIRE_FALSE(enum_index(static_cast<Color>(0)).has_value());

  constexpr auto no = enum_index(Numbers::one);
  REQUIRE(no.value() == 0);
  REQUIRE(enum_index<Numbers::one>() == 0);
  REQUIRE(enum_index(Numbers::two).value() == 1);
  REQUIRE(enum_index(Numbers::three).value() == 2);
  REQUIRE_FALSE(enum_index(Numbers::many).has_value());
  REQUIRE_FALSE(enum_index(static_cast<Numbers>(0)).has_value());

  constexpr auto dr = enum_index(Directions::Right);
  Directions dl = Directions::Left;
  REQUIRE(enum_index<Directions::Left>() == 0);
  REQUIRE(enum_index<Directions&>(dl).value() == 0);
  REQUIRE(enum_index<const Directions>(Directions::Down).value() == 1);
  REQUIRE(enum_index(Directions::Up).value() == 2);
  REQUIRE(dr.value() == 3);
  REQUIRE_FALSE(enum_index(static_cast<Directions>(0)).has_value());

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    constexpr auto lang = enum_index<Language>(Language::日本語);
    Language korean = Language::한국어;
    REQUIRE(enum_index<Language&>(korean) == 1);
    REQUIRE(enum_index<const Language>(Language::English).value() == 2);
    REQUIRE(enum_index(Language::😃) == 3);
    REQUIRE(lang.value() == 0);
    REQUIRE_FALSE(enum_index(static_cast<Language>(0)).has_value());
#endif

  constexpr auto nt = enum_index(number::three);
  REQUIRE(enum_index<number::one>() == 0);
  REQUIRE(enum_index(number::one).value() == 0);
  REQUIRE(enum_index(number::two).value() == 1);
  REQUIRE(nt.value() == 2);
  REQUIRE_FALSE(enum_index(number::four).has_value());
  REQUIRE_FALSE(enum_index(static_cast<number>(0)).has_value());

  REQUIRE(enum_index<BoolTest::Yay>() == 0);
}

TEST_CASE("enum_contains") {
  SECTION("value") {
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    constexpr auto cr = enum_contains(Color::RED);
    Color cg = Color::GREEN;
    REQUIRE(cr);
    REQUIRE(enum_contains<Color&>(cg));
    REQUIRE(enum_contains(cm[2]));
    REQUIRE_FALSE(enum_contains(static_cast<Color>(0)));

    constexpr auto no = enum_contains(Numbers::one);
    REQUIRE(no);
    REQUIRE(enum_contains(Numbers::two));
    REQUIRE(enum_contains(Numbers::three));
    REQUIRE_FALSE(enum_contains(Numbers::many));
    REQUIRE_FALSE(enum_contains(static_cast<Numbers>(0)));

    constexpr auto dr = enum_contains(Directions::Right);
    Directions dl = Directions::Left;
    REQUIRE(enum_contains<Directions&>(dl));
    REQUIRE(enum_contains<const Directions>(Directions::Down));
    REQUIRE(enum_contains(Directions::Up));
    REQUIRE(dr);
    REQUIRE_FALSE(enum_contains(static_cast<Directions>(0)));

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    constexpr auto lang = enum_contains(Language::日本語);
    Language korean = Language::한국어;
    REQUIRE(enum_contains<Language&>(korean));
    REQUIRE(enum_contains<const Language>(Language::English));
    REQUIRE(enum_contains(Language::😃));
    REQUIRE(lang);
    REQUIRE_FALSE(enum_contains(static_cast<Directions>(0)));
#endif

    constexpr auto nt = enum_contains(number::three);
    REQUIRE(enum_contains(number::one));
    REQUIRE(enum_contains<number&>(number::two));
    REQUIRE(nt);
    REQUIRE_FALSE(enum_contains(number::four));
    REQUIRE_FALSE(enum_contains(static_cast<number>(0)));

    REQUIRE(enum_contains(BoolTest::Yay));
  }

  SECTION("integer") {
    REQUIRE(enum_contains<Color>(-12));
    REQUIRE(enum_contains<Color&>(7));
    REQUIRE(enum_contains<Color>(15));
    REQUIRE_FALSE(enum_contains<Color>(42));
    REQUIRE_FALSE(enum_contains<Color>(-120));
    REQUIRE_FALSE(enum_contains<Color>(0));

    constexpr auto no = enum_integer(Numbers::one);
    REQUIRE(enum_contains<Numbers>(no));
    REQUIRE(enum_contains<Numbers>(enum_integer(Numbers::two)));
    REQUIRE(enum_contains<Numbers>(enum_integer(Numbers::three)));
    REQUIRE_FALSE(enum_contains<Numbers>(enum_integer(Numbers::many)));

    constexpr auto dr = enum_integer(Directions::Right);
    REQUIRE(enum_contains<Directions&>(dr));
    REQUIRE(enum_contains<const Directions>(Directions::Down));
    REQUIRE(enum_contains<Directions>(Directions::Up));
    REQUIRE_FALSE(enum_contains<Directions>(static_cast<Directions>(0)));

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    constexpr auto lang = enum_integer(Language::日本語);
    REQUIRE(enum_contains<Language&>(lang));
    REQUIRE(enum_contains<const Language>(Language::한국어));
    REQUIRE(enum_contains<Language>(Language::😃));
    REQUIRE_FALSE(enum_contains<Language>(static_cast<Language>(0)));
#endif

    constexpr auto nt = enum_contains<number>(number::three);
    REQUIRE(enum_contains<number>(number::one));
    REQUIRE(enum_contains<number>(100));
    REQUIRE(enum_contains<number>(200));
    REQUIRE(enum_contains<number>(300));
    REQUIRE(enum_contains<number>(number::two));
    REQUIRE(nt);
    REQUIRE_FALSE(enum_contains<number>(number::four));
    REQUIRE_FALSE(enum_contains<number>(111));
    REQUIRE_FALSE(enum_contains<number>(0));

    REQUIRE(enum_contains<BoolTest>(false));
    REQUIRE(enum_contains<BoolTest>(0));
  }

  SECTION("string") {
    constexpr auto cr = "red";
    REQUIRE(enum_contains<Color>(cr));
    REQUIRE(enum_contains<Color&>("GREEN"));
    REQUIRE(enum_contains<Color>("blue", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }));
    REQUIRE_FALSE(enum_contains<Color>("None"));

    constexpr auto no = std::string_view{"one"};
    REQUIRE(enum_contains<Numbers>(no));
    REQUIRE(enum_contains<Numbers>("two"));
    REQUIRE(enum_contains<Numbers>("three"));
    REQUIRE_FALSE(enum_contains<Numbers>("many"));
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
#else
    auto dr2 = std::string{"RIGHT"};
    REQUIRE(enum_contains<const Directions>(dr2, case_insensitive));
    REQUIRE(enum_contains<Directions&>("up", case_insensitive));
    REQUIRE(enum_contains<Directions>("dOwN", case_insensitive));
    REQUIRE_FALSE(enum_contains<Directions>("Left-", case_insensitive));
#endif

    constexpr auto nt = enum_contains<number>("three");
    REQUIRE(enum_contains<number>("one"));
    REQUIRE(enum_contains<number>("two"));
    REQUIRE(nt);
    REQUIRE_FALSE(enum_contains<number>("four"));
    REQUIRE_FALSE(enum_contains<number>("None"));

    REQUIRE(enum_contains<BoolTest>("Yay"));
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

  REQUIRE(enum_value<Numbers, 0>() == Numbers::one);
  REQUIRE(enum_value<Numbers, 1>() == Numbers::two);
  REQUIRE(enum_value<Numbers, 2>() == Numbers::three);

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

  REQUIRE(enum_value<number, 0>() == number::one);
  REQUIRE(enum_value<number, 1>() == number::two);
  REQUIRE(enum_value<number, 2>() == number::three);

  REQUIRE(enum_value<BoolTest>(0) == BoolTest::Yay);
  REQUIRE(enum_value<BoolTest, 0>() == BoolTest::Yay);
}

TEST_CASE("enum_values") {
  REQUIRE(std::is_same_v<decltype(magic_enum::enum_values<Color>()), const std::array<Color, 3>&>);

  constexpr auto& s1 = enum_values<Color&>();
  REQUIRE(s1 == std::array<Color, 3>{{Color::RED, Color::GREEN, Color::BLUE}});

  constexpr auto& s2 = enum_values<Numbers>();
  REQUIRE(s2 == std::array<Numbers, 3>{{Numbers::one, Numbers::two, Numbers::three}});

  constexpr auto& s3 = enum_values<const Directions>();
  REQUIRE(s3 == std::array<Directions, 4>{{Directions::Left, Directions::Down, Directions::Up, Directions::Right}});

  constexpr auto& s4 = enum_values<number>();
  REQUIRE(s4 == std::array<number, 3>{{number::one, number::two, number::three}});

  constexpr auto& s5 = enum_values<Binary>();
  REQUIRE(s5 == std::array<Binary, 2>{{Binary::ONE, Binary::TWO}});

  constexpr auto& s6 = enum_values<MaxUsedAsInvalid>();
  REQUIRE(s6 == std::array<MaxUsedAsInvalid, 2>{{MaxUsedAsInvalid::ONE, MaxUsedAsInvalid::TWO}});

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  constexpr auto& s7 = enum_values<const Language>();
  REQUIRE(s7 == std::array<Language, 4>{{Language::日本語, Language::한국어, Language::English, Language::😃}});
#endif
}

TEST_CASE("enum_count") {
  constexpr auto s1 = enum_count<Color&>();
  REQUIRE(s1 == 3);

  constexpr auto s2 = enum_count<Numbers>();
  REQUIRE(s2 == 3);

  constexpr auto s3 = enum_count<const Directions>();
  REQUIRE(s3 == 4);

  constexpr auto s4 = enum_count<number>();
  REQUIRE(s4 == 3);

  constexpr auto s5 = enum_count<Binary>();
  REQUIRE(s5 == 2);

  constexpr auto s6 = enum_count<MaxUsedAsInvalid>();
  REQUIRE(s6 == 2);

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  constexpr auto s7 = enum_count<Language>();
  REQUIRE(s7 == 4);
#endif
}

TEST_CASE("enum_name") {
  SECTION("automatic storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = enum_name(cr);
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    Color cb = Color::BLUE;
    REQUIRE(cr_name == "red");
    REQUIRE(enum_name<Color&>(cb) == "BLUE");
    REQUIRE(enum_name<as_flags<false>>(cm[1]) == "GREEN");
    REQUIRE(enum_name<detail::value_type::default_value>(static_cast<Color>(0)).empty());

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = enum_name(no);
    REQUIRE(no_name == "one");
    REQUIRE(enum_name<Numbers, as_flags<false>>(Numbers::two) == "two");
    REQUIRE(enum_name<as_flags<false>, Numbers>(Numbers::three) == "three");
    REQUIRE(enum_name(Numbers::many).empty());
    REQUIRE(enum_name(static_cast<Numbers>(0)).empty());

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = enum_name(dr);
    Directions du = Directions::Up;
    REQUIRE(enum_name<Directions&>(du) == "Up");
    REQUIRE(enum_name<const Directions>(Directions::Down) == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(enum_name(Directions::Left) == "Left");
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

    constexpr number nt = number::three;
    constexpr auto nt_name = enum_name(nt);
    REQUIRE(enum_name(number::one) == "one");
    REQUIRE(enum_name(number::two) == "two");
    REQUIRE(nt_name == "three");
    REQUIRE(enum_name(number::four).empty());
    REQUIRE(enum_name(static_cast<number>(0)).empty());

    REQUIRE(enum_name(MaxUsedAsInvalid::ONE) == "ONE");
  }

  SECTION("static storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = enum_name<cr>();
    constexpr Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    REQUIRE(cr_name == "red");
    REQUIRE(enum_name<Color::BLUE>() == "BLUE");
    REQUIRE(enum_name<cm[1]>() == "GREEN");

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = enum_name<no>();
    REQUIRE(no_name == "one");
    REQUIRE(enum_name<Numbers::two>() == "two");
    REQUIRE(enum_name<Numbers::three>() == "three");
    REQUIRE(enum_name<Numbers::many>() == "many");

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = enum_name<dr>();
    REQUIRE(enum_name<Directions::Up>() == "Up");
    REQUIRE(enum_name<Directions::Down>() == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(enum_name<Directions::Left>() == "Left");

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    constexpr Language lang = Language::日本語;
    constexpr auto lang_name = enum_name<lang>();
    REQUIRE(enum_name<Language::한국어>() == "한국어");
    REQUIRE(enum_name<Language::English>() == "English");
    REQUIRE(lang_name == "日本語");
    REQUIRE(enum_name<Language::😃>() == "😃");
#endif

    constexpr number nt = number::three;
    constexpr auto nt_name = enum_name<nt>();
    REQUIRE(enum_name<number::one>() == "one");
    REQUIRE(enum_name<number::two>() == "two");
    REQUIRE(nt_name == "three");
    REQUIRE(enum_name<number::four>() == "four");

    REQUIRE(enum_name<Binary::ONE>() == "ONE");
    REQUIRE(enum_name<MaxUsedAsInvalid::ONE>() == "ONE");
  }
}

TEST_CASE("enum_names") {
  REQUIRE(std::is_same_v<decltype(magic_enum::enum_names<Color>()), const std::array<std::string_view, 3>&>);

  constexpr auto& s1 = enum_names<Color&>();
  REQUIRE(s1 == std::array<std::string_view, 3>{{"red", "GREEN", "BLUE"}});

  constexpr auto& s2 = enum_names<Numbers>();
  REQUIRE(s2 == std::array<std::string_view, 3>{{"one", "two", "three"}});

  constexpr auto& s3 = enum_names<const Directions>();
  REQUIRE(s3 == std::array<std::string_view, 4>{{"Left", "Down", "Up", "Right"}});

  constexpr auto& s4 = enum_names<number>();
  REQUIRE(s4 == std::array<std::string_view, 3>{{"one", "two", "three"}});

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  constexpr auto& s5 = enum_names<const Language>();
  REQUIRE(s5 == std::array<std::string_view, 4>{{"日本語", "한국어", "English", "😃"}});
#endif
}

TEST_CASE("enum_entries") {
  REQUIRE(std::is_same_v<decltype(magic_enum::enum_entries<Color>()), const std::array<std::pair<Color, std::string_view>, 3>&>);

  constexpr auto& s1 = enum_entries<Color&>();
  REQUIRE(s1 == std::array<std::pair<Color, std::string_view>, 3>{{{Color::RED, "red"}, {Color::GREEN, "GREEN"}, {Color::BLUE, "BLUE"}}});

  constexpr auto& s2 = enum_entries<Numbers>();
  REQUIRE(s2 == std::array<std::pair<Numbers, std::string_view>, 3>{{{Numbers::one, "one"}, {Numbers::two, "two"}, {Numbers::three, "three"}}});

  constexpr auto& s3 = enum_entries<Directions&>();
  REQUIRE(s3 == std::array<std::pair<Directions, std::string_view>, 4>{{{Directions::Left, "Left"}, {Directions::Down, "Down"}, {Directions::Up, "Up"}, {Directions::Right, "Right"}}});

  constexpr auto& s4 = enum_entries<number>();
  REQUIRE(s4 == std::array<std::pair<number, std::string_view>, 3>{{{number::one, "one"}, {number::two, "two"}, {number::three, "three"}}});

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
    REQUIRE(ss);
    REQUIRE(ss.str() == name);
  };

  test_ostream(std::make_optional(Color::RED), "red");
  test_ostream(Color::GREEN, "GREEN");
  test_ostream(Color::BLUE, "BLUE");
  test_ostream(static_cast<Color>(0), "0");
  test_ostream(std::make_optional(static_cast<Color>(0)), "0");

  test_ostream(std::make_optional(Numbers::one), "one");
  test_ostream(Numbers::two, "two");
  test_ostream(Numbers::three, "three");
  test_ostream(Numbers::many, "127");
  test_ostream(static_cast<Numbers>(0), "0");
  test_ostream(std::make_optional(static_cast<Numbers>(0)), "0");

  test_ostream(std::make_optional(Directions::Up), "Up");
  test_ostream(Directions::Down, "Down");
  test_ostream(Directions::Right, "Right");
  test_ostream(Directions::Left, "Left");
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
  test_ostream(number::four, "400");
  test_ostream(static_cast<number>(0), "0");
  test_ostream(std::make_optional(static_cast<number>(0)), "0");
}

TEST_CASE("istream_operators") {
  auto test_istream = [](const auto e, std::string name) {
    using namespace magic_enum::istream_operators;
    std::istringstream ss(name);
    std::decay_t<decltype(e)> v;
    ss >> v;
    REQUIRE(ss);
    REQUIRE(v == e);
  };

  test_istream(Color::GREEN, "GREEN");
  test_istream(Color::BLUE, "BLUE");

  test_istream(Numbers::two, "two");
  test_istream(Numbers::three, "three");

  test_istream(Directions::Down, "Down");
  test_istream(Directions::Right, "Right");
  test_istream(Directions::Left, "Left");

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  test_istream(Language::한국어, "한국어");
  test_istream(Language::English, "English");
  test_istream(Language::😃, "😃");
#endif

  test_istream(number::two, "two");
  test_istream(number::three, "three");
}

TEST_CASE("bitwise_operators") {
  using namespace magic_enum::bitwise_operators;

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

TEST_CASE("type_traits") {
  REQUIRE_FALSE(is_unscoped_enum_v<Color>);
  REQUIRE_FALSE(is_unscoped_enum_v<Numbers>);
  REQUIRE(is_unscoped_enum_v<Directions>);
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  REQUIRE_FALSE(is_unscoped_enum_v<Language>);
#endif
  REQUIRE(is_unscoped_enum_v<number>);

  REQUIRE(is_scoped_enum_v<Color>);
  REQUIRE(is_scoped_enum_v<Numbers>);
  REQUIRE_FALSE(is_scoped_enum_v<Directions>);
  REQUIRE_FALSE(is_scoped_enum_v<number>);
}

TEST_CASE("enum_type_name") {
  REQUIRE(enum_type_name<Color&>() == "Color");
  REQUIRE(enum_type_name<const Numbers>() == "Numbers");
  REQUIRE(enum_type_name<const Directions&>() == "Directions");
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  REQUIRE(enum_type_name<const Language&>() == "Language");
#endif
  REQUIRE(enum_type_name<number>() == "number");
}

#if defined(MAGIC_ENUM_SUPPORTED_ALIASES)
TEST_CASE("aliases") {
  REQUIRE(enum_count<number>() == 3);

  REQUIRE(enum_name(number::one) == enum_name(number::_1));
  REQUIRE(enum_name(number::two) == enum_name(number::_2));
  REQUIRE(enum_name(number::three) == enum_name(number::_3));
  REQUIRE(enum_name(number::four) == enum_name(number::_4));

  REQUIRE(enum_integer(number::one) == enum_integer(number::_1));
  REQUIRE(enum_integer(number::two) == enum_integer(number::_2));
  REQUIRE(enum_integer(number::three) == enum_integer(number::_3));
  REQUIRE(enum_integer(number::four) == enum_integer(number::_4));

  REQUIRE_FALSE(enum_cast<number>("_1").has_value());
  REQUIRE_FALSE(enum_cast<number>("_2").has_value());
  REQUIRE_FALSE(enum_cast<number>("_3").has_value());
  REQUIRE_FALSE(enum_cast<number>("_4").has_value());
}
#endif

TEST_CASE("extrema") {
  enum class BadColor : std::uint64_t {
    RED,
    GREEN,
    YELLOW,
    // The value NONE is ignored (out of range).
    // However, it affects the value of min_v. When reflected_min_v was incorrect,
    // the presence of NONE caused miv_v to be equal to -1, which was then cast to unsigned,
    // leading to a value of 18446744073709551615 (numeric_limit_max of uint64_t).
    NONE = std::numeric_limits<std::uint64_t>::max()
  };

  REQUIRE(magic_enum::enum_name<BadColor>(BadColor::NONE).empty());
  REQUIRE_FALSE(magic_enum::enum_cast<BadColor>(std::numeric_limits<std::uint64_t>::max()).has_value());
  REQUIRE_FALSE(magic_enum::enum_contains<BadColor>(std::numeric_limits<std::uint64_t>::max()));
  REQUIRE_FALSE(magic_enum::enum_contains<BadColor>(BadColor::NONE));

  SECTION("min") {
    REQUIRE(magic_enum::customize::enum_range<BadColor>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min_v<BadColor, false> == 0);
    REQUIRE(magic_enum::detail::min_v<BadColor> == 0);

    REQUIRE(magic_enum::customize::enum_range<Color>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min_v<Color, false> == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::min_v<Color> == -12);

    REQUIRE(magic_enum::customize::enum_range<Numbers>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min_v<Numbers, false> == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::min_v<Numbers> == 1);

    REQUIRE(magic_enum::customize::enum_range<Directions>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min_v<Directions, false> == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::min_v<Directions> == -120);

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    REQUIRE(magic_enum::customize::enum_range<Language>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min_v<Language, false> == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::min_v<Language> == 10);
#endif

    REQUIRE(magic_enum::customize::enum_range<number>::min == 100);
    REQUIRE(magic_enum::detail::reflected_min_v<number, false> == 100);
    REQUIRE(magic_enum::detail::min_v<number> == 100);

    REQUIRE(magic_enum::detail::reflected_min_v<Binary, false> == 0);
    REQUIRE(magic_enum::detail::min_v<Binary> == false);

    REQUIRE(magic_enum::detail::reflected_min_v<MaxUsedAsInvalid,false> == 0);
    REQUIRE(magic_enum::detail::min_v<MaxUsedAsInvalid> == 0);
  }

  SECTION("max") {
    REQUIRE(magic_enum::customize::enum_range<BadColor>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max_v<BadColor, false> == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::max_v<BadColor> == 2);

    REQUIRE(magic_enum::customize::enum_range<Color>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max_v<Color, false> == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::max_v<Color> == 15);

    REQUIRE(magic_enum::customize::enum_range<Numbers>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max_v<Numbers, false> == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::max_v<Numbers> == 3);

    REQUIRE(magic_enum::customize::enum_range<Directions>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max_v<Directions, false> == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::max_v<Directions> == 120);

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    REQUIRE(magic_enum::customize::enum_range<Language>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max_v<Language, false> == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::max_v<Language> == 40);
#endif

    REQUIRE(magic_enum::customize::enum_range<number>::max == 300);
    REQUIRE(magic_enum::detail::reflected_max_v<number, false> == 300);
    REQUIRE(magic_enum::detail::max_v<number> == 300);

    REQUIRE(magic_enum::detail::reflected_max_v<Binary, false> == 1);
    REQUIRE(magic_enum::detail::max_v<Binary> == true);

    REQUIRE(magic_enum::detail::reflected_max_v<MaxUsedAsInvalid, false> == 64);
    REQUIRE(magic_enum::detail::max_v<MaxUsedAsInvalid> == 63);
  }
}

TEST_CASE("cmp_less") {
  using magic_enum::detail::cmp_less;

  constexpr std::uint64_t uint64_t_min = std::numeric_limits<std::uint64_t>::min();
  constexpr std::uint32_t uint32_t_min = std::numeric_limits<std::uint32_t>::min();
  constexpr std::uint32_t uint32_t_max = std::numeric_limits<std::uint32_t>::max();
  constexpr std::uint64_t uint64_t_max = std::numeric_limits<std::uint64_t>::max();

  constexpr std::int64_t int64_t_min = std::numeric_limits<std::int64_t>::min();
  constexpr std::int32_t int32_t_min = std::numeric_limits<std::int32_t>::min();
  constexpr std::int32_t int32_t_max = std::numeric_limits<std::int32_t>::max();
  constexpr std::int64_t int64_t_max = std::numeric_limits<std::int64_t>::max();

  // Also testing with offset to avoid corner cases.
  // Two variables to avoid hidden casts:
  constexpr std::int64_t offset_int64_t = 17;
  constexpr std::int32_t offset_int32_t = 17;

  SECTION("same signedness") {
    REQUIRE(cmp_less(-5, -3));
    REQUIRE(cmp_less(27U, 49U));
  }

  SECTION("same signedness, different width") {
    REQUIRE(cmp_less(uint32_t_max, uint64_t_max));
    REQUIRE_FALSE(cmp_less(uint64_t_max, uint32_t_max));
    REQUIRE(cmp_less(int64_t_min, int32_t_min));
    REQUIRE_FALSE(cmp_less(int32_t_min, int64_t_min));
    REQUIRE(cmp_less(int64_t_min + offset_int64_t, int32_t_min + offset_int32_t));
    REQUIRE_FALSE(cmp_less(int32_t_min + offset_int32_t, int64_t_min + offset_int64_t));
  }

  SECTION("left signed, right unsigned") {
    REQUIRE(cmp_less(-5, 3U));
    REQUIRE(cmp_less(3, 5U));
  }

  SECTION("left signed, right unsigned, different width") {
    REQUIRE(cmp_less(int32_t_max, uint64_t_max));
    REQUIRE_FALSE(cmp_less(int64_t_max, uint32_t_max));
    REQUIRE(cmp_less(int32_t_min, uint64_t_min));
    REQUIRE(cmp_less(int64_t_min, uint32_t_min));
    REQUIRE(cmp_less(int32_t_max - offset_int32_t, uint64_t_max));
    REQUIRE_FALSE(cmp_less(int64_t_max - offset_int64_t, uint32_t_max));
    REQUIRE(cmp_less(int32_t_min + offset_int32_t, uint64_t_min));
    REQUIRE(cmp_less(int64_t_min + offset_int64_t, uint32_t_min));
  }

  SECTION("left unsigned, right signed") {
    REQUIRE_FALSE(cmp_less(3U, -5));
    REQUIRE(cmp_less(3U, 5));
  }

  SECTION("left unsigned, right signed, different width") {
    REQUIRE(cmp_less(uint32_t_max, int64_t_max));
    REQUIRE_FALSE(cmp_less(uint64_t_max, int32_t_max));
    REQUIRE_FALSE(cmp_less(uint32_t_min, int64_t_min));
    REQUIRE_FALSE(cmp_less(uint64_t_min, int32_t_min));
    REQUIRE(cmp_less(uint32_t_max, int64_t_max - offset_int32_t));
    REQUIRE_FALSE(cmp_less(uint64_t_max, int32_t_max - offset_int64_t));
    REQUIRE_FALSE(cmp_less(uint32_t_min, int64_t_min + offset_int32_t));
    REQUIRE_FALSE(cmp_less(uint64_t_min, int32_t_min + offset_int64_t));
  }

  SECTION("bool, right") {
    REQUIRE(cmp_less(true, 5));
    REQUIRE(cmp_less(false, 1));
    REQUIRE_FALSE(cmp_less(false, -1));
  }

  SECTION("left, bool") {
    REQUIRE_FALSE(cmp_less(5, true));
    REQUIRE_FALSE(cmp_less(1, false));
    REQUIRE(cmp_less(-1, false));
  }
}

template <Color C>
constexpr std::string_view DoWork() {
  return "default";
}

template <>
constexpr std::string_view DoWork<Color::GREEN>() {
  return "override";
}

TEST_CASE("enum_for_each") {
  SECTION("no return type") {
    underlying_type_t<Color> sum{};
    enum_for_each<Color>([&sum](auto val) {
      constexpr underlying_type_t<Color> v = enum_integer(val());
      sum += v;
    });
    REQUIRE(sum == 10);
  }

  SECTION("same return type") {
    constexpr auto workResults = enum_for_each<Color>([](auto val) {
      return DoWork<val>();
    });
    REQUIRE(workResults == std::array<std::string_view, 3>{"default", "override", "default"});
  }

  SECTION("different return type") {
    constexpr auto colorInts = enum_for_each<Color>([](auto val) {
      return val;
    });

    REQUIRE(std::is_same_v<std::remove_const_t<decltype(colorInts)>,
                           std::tuple<enum_constant<Color::RED>,
                                      enum_constant<Color::GREEN>,
                                      enum_constant<Color::BLUE>>>);
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

  constexpr_for<0, magic_enum::enum_count<Numbers>(), 1>([](auto i) {
    [[maybe_unused]] Foo<Numbers, magic_enum::enum_value<Numbers, i>()> bar{};
  });
}

#if defined(_MSC_VER)
# pragma warning(push)
# pragma warning(disable : 4064)
#endif

static int switch_case_2d(Color color, Directions direction) {
  switch (magic_enum::enum_fuse(color, direction).value()) {
    case magic_enum::enum_fuse(Color::RED, Directions::Up).value():
      return 1;
    case magic_enum::enum_fuse(Color::BLUE, Directions::Down).value():
      return 2;
    default:
      return 0;
  }
}

enum class Index { zero = 0, one = 1, two = 2 };

static int switch_case_3d(Color color, Directions direction, Index index) {
  switch (magic_enum::enum_fuse(color, direction, index).value()) {
    case magic_enum::enum_fuse(Color::RED, Directions::Up, Index::zero).value():
      return 1;
    case magic_enum::enum_fuse(Color::BLUE, Directions::Up, Index::zero).value():
      return 2;
    default:
      return 0;
  }
}

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

TEST_CASE("multdimensional-switch-case") {
  REQUIRE(switch_case_2d(Color::RED, Directions::Up) == 1);
  REQUIRE(switch_case_2d(Color::RED, Directions::Down) == 0);
  REQUIRE(switch_case_2d(Color::BLUE, Directions::Up) == 0);
  REQUIRE(switch_case_2d(Color::BLUE, Directions::Down) == 2);
  REQUIRE(switch_case_3d(Color::RED, Directions::Up, Index::zero) == 1);
  REQUIRE(switch_case_3d(Color::BLUE, Directions::Up, Index::zero) == 2);
  REQUIRE(switch_case_3d(Color::BLUE, Directions::Up, Index::one) == 0);
  REQUIRE(switch_case_3d(Color::BLUE, Directions::Up, Index::two) == 0);
}

#endif

#if defined(__cpp_lib_format)

#include <magic_enum_format.hpp>

TEST_CASE("format-base") {
  REQUIRE(std::format("{}", Color::RED) == "red");
  REQUIRE(std::format("{}", Color{0}) == "0");
  REQUIRE(std::format("Test-{:~^10}.", Color::RED) == "Test-~~~red~~~~.");
}

#endif
