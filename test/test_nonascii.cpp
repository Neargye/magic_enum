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

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#undef  MAGIC_ENUM_RANGE_MIN
#define MAGIC_ENUM_RANGE_MIN -120
#undef  MAGIC_ENUM_RANGE_MAX
#define MAGIC_ENUM_RANGE_MAX 120
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_fuse.hpp>
#include <magic_enum/magic_enum_iostream.hpp>

#include <array>
#include <cctype>
#include <string_view>
#include <sstream>

enum class Language : int { 日本語 = 10, 한국어 = 20, English = 30, 😃 = 40, TVÅ = 50 };

enum class LanguageFlag : int {
  日本語 = 1 << 1,
  한국어 = 1 << 2,
  English = 1 << 3,
  😃 = 1 << 4
};

using namespace magic_enum;

static_assert(is_magic_enum_supported, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");

TEST_CASE("enum_cast") {
  SECTION("string") {
      constexpr auto lang = enum_cast<Language>("日本語");
      REQUIRE(enum_cast<Language&>("한국어").value() == Language::한국어);
      REQUIRE(enum_cast<const Language>("English").value() == Language::English);
      REQUIRE(lang.value() == Language::日本語);
      REQUIRE(enum_cast<Language>("😃").value() == Language::😃);
      REQUIRE(enum_cast<Language>("TVÅ").value() == Language::TVÅ);
      REQUIRE_FALSE(enum_cast<Language>("Französisch").has_value());
  }

  SECTION("integer") {
    constexpr auto lang = enum_cast<Language>(10);
    REQUIRE(enum_cast<Language&>(20).value() == Language::한국어);
    REQUIRE(enum_cast<const Language>(30).value() == Language::English);
    REQUIRE(lang.value() == Language::日本語);
    REQUIRE(enum_cast<Language>(40).value() == Language::😃);
    REQUIRE_FALSE(enum_cast<Language>(0).has_value());
  }
}

TEST_CASE("enum_integer") {
    constexpr auto lang = enum_integer(Language::日本語);
    Language korean = Language::한국어;
    REQUIRE(enum_integer<Language&>(korean) == 20);
    REQUIRE(enum_integer<const Language>(Language::English) == 30);
    REQUIRE(enum_integer(Language::😃) == 40);
    REQUIRE(lang == 10);
    REQUIRE(enum_integer(static_cast<Language>(0)) == 0);
}

TEST_CASE("enum_index") {
    constexpr auto lang = enum_index<Language>(Language::日本語);
    Language korean = Language::한국어;
    REQUIRE(enum_index<Language&>(korean) == 1);
    REQUIRE(enum_index<const Language>(Language::English).value() == 2);
    REQUIRE(enum_index(Language::😃) == 3);
    REQUIRE(lang.value() == 0);
    REQUIRE_FALSE(enum_index(static_cast<Language>(0)).has_value());
}

TEST_CASE("enum_contains") {
  SECTION("value") {
    constexpr auto lang = enum_contains(Language::日本語);
    Language korean = Language::한국어;
    REQUIRE(enum_contains<Language&>(korean));
    REQUIRE(enum_contains<const Language>(Language::English));
    REQUIRE(enum_contains(Language::😃));
    REQUIRE(lang);
  }

  SECTION("integer") {
    constexpr auto lang = enum_integer(Language::日本語);
    REQUIRE(enum_contains<Language&>(lang));
    REQUIRE(enum_contains<const Language>(Language::한국어));
    REQUIRE(enum_contains<Language>(Language::😃));
    REQUIRE_FALSE(enum_contains<Language>(static_cast<Language>(0)));
  }

  SECTION("string") {
    auto lang = std::string{"日本語"};
    REQUIRE(enum_contains<Language&>("한국어"));
    REQUIRE(enum_contains<Language>("English"));
    REQUIRE(enum_contains<const Language>(lang));
    REQUIRE(enum_contains<Language>("😃"));
    REQUIRE_FALSE(enum_contains<Language>("None"));
  }
}

TEST_CASE("enum_value") {
    constexpr auto lang = enum_value<Language>(3);
    REQUIRE(enum_value<Language&>(0) == Language::日本語);
    REQUIRE(enum_value<const Language>(1) == Language::한국어);
    REQUIRE(enum_value<Language>(2) == Language::English);
    REQUIRE(lang == Language::😃);
}

TEST_CASE("enum_values") {
  constexpr auto& s7 = enum_values<const Language>();
  REQUIRE(s7 == std::array<Language, 5>{{Language::日本語, Language::한국어, Language::English, Language::😃, Language::TVÅ}});
}

TEST_CASE("enum_count") {
  constexpr auto s7 = enum_count<Language>();
  REQUIRE(s7 == 5);
}

TEST_CASE("enum_name") {
  SECTION("automatic storage") {
    constexpr Language lang = Language::日本語;
    constexpr auto lang_name = enum_name(lang);
    Language lk = Language::한국어;
    REQUIRE(enum_name<Language&>(lk) == "한국어");
    REQUIRE(enum_name<const Language>(Language::English) == "English");
    REQUIRE(lang_name == "日本語");
    REQUIRE(enum_name(Language::😃) == "😃");
    REQUIRE(enum_name(Language::TVÅ) == "TVÅ");
    REQUIRE(enum_name(static_cast<Language>(0)).empty());
  }

  SECTION("static storage") {
    constexpr Language lang = Language::日本語;
    constexpr auto lang_name = enum_name<lang>();
    REQUIRE(enum_name<Language::한국어>() == "한국어");
    REQUIRE(enum_name<Language::English>() == "English");
    REQUIRE(lang_name == "日本語");
    REQUIRE(enum_name<Language::😃>() == "😃");
  }
}

TEST_CASE("enum_names") {
  constexpr auto& s5 = enum_names<const Language>();
  REQUIRE(s5 == std::array<std::string_view, 5>{{"日本語", "한국어", "English", "😃", "TVÅ"}});
}

TEST_CASE("enum_entries") {
  constexpr auto& s5 = enum_entries<const Language>();
  REQUIRE(s5 == std::array<std::pair<Language, std::string_view>, 5>{{{Language::日本語, "日本語"}, {Language::한국어, "한국어"}, {Language::English, "English"}, {Language::😃, "😃"}, {Language::TVÅ, "TVÅ"}}});
}

TEST_CASE("ostream_operators") {
  auto test_ostream = [](auto e, std::string name) {
    using namespace magic_enum::ostream_operators;
    std::stringstream ss;
    ss << e;
    REQUIRE(ss);
    REQUIRE(ss.str() == name);
  };

  test_ostream(std::make_optional(Language::日本語), "日本語");
  test_ostream(Language::한국어, "한국어");
  test_ostream(Language::English, "English");
  test_ostream(Language::😃, "😃");
  test_ostream(static_cast<Language>(0), "0");
  test_ostream(std::make_optional(static_cast<Language>(0)), "0");
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

  test_istream(Language::한국어, "한국어");
  test_istream(Language::English, "English");
  test_istream(Language::😃, "😃");
}

TEST_CASE("bitwise_operators") {
  using namespace magic_enum::bitwise_operators;

  SECTION("operator^") {
    REQUIRE(enum_integer(~Language::日本語) == ~enum_integer(Language::日本語));
  }

  SECTION("operator|") {
    REQUIRE(enum_integer(Language::日本語 | Language::한국어) == (enum_integer(Language::日本語) | enum_integer(Language::한국어)));
  }

  SECTION("operator&") {
    REQUIRE(enum_integer(Language::日本語 & Language::한국어) == (enum_integer(Language::日本語) & enum_integer(Language::한국어)));

  }

  SECTION("operator^") {
    REQUIRE(enum_integer(Language::日本語 ^ Language::한국어) == (enum_integer(Language::日本語) ^ enum_integer(Language::한국어)));
  }

  SECTION("operator|=") {
    Language x5 = Language::日本語;
    x5 |= Language::한국어;
    REQUIRE(enum_integer(x5) == (enum_integer(Language::日本語) | enum_integer(Language::한국어)));
  }

  SECTION("operator&=") {
    Language x5 = Language::日本語;
    x5 &= Language::한국어;
    REQUIRE(enum_integer(x5) == (enum_integer(Language::日本語) & enum_integer(Language::한국어)));
  }

  SECTION("operator^=") {
    Language x5 = Language::日本語;
    x5 ^= Language::한국어;
    REQUIRE(enum_integer(x5) == (enum_integer(Language::日本語) ^ enum_integer(Language::한국어)));
  }
}
TEST_CASE("type_traits") {
  REQUIRE_FALSE(is_unscoped_enum_v<Language>);
}

TEST_CASE("enum_type_name") {
  REQUIRE(enum_type_name<const Language&>() == "Language");
}

TEST_CASE("extrema") {
  SECTION("min") {
    REQUIRE(magic_enum::customize::enum_range<Language>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min<Language, as_common<>>() == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::min_v<Language, as_common<>> == 10);
  }

  SECTION("max") {
    REQUIRE(magic_enum::customize::enum_range<Language>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max<Language, as_common<>>() == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::max_v<Language, as_common<>> == 50);
  }
}

/* LanguageFlag tests */
TEST_CASE("flag enum_cast") {
  SECTION("string") {
    constexpr auto lang = enum_cast<LanguageFlag>("日本語");
    REQUIRE(enum_cast<LanguageFlag&>("한국어").value() == LanguageFlag::한국어);
    REQUIRE(enum_cast<const LanguageFlag>("English").value() == LanguageFlag::English);
    REQUIRE(lang.value() == LanguageFlag::日本語);
    REQUIRE(enum_cast<LanguageFlag>("😃").value() == LanguageFlag::😃);
    REQUIRE_FALSE(enum_cast<LanguageFlag>("None").has_value());
  }

  SECTION("integer") {
    constexpr auto lang = enum_cast<LanguageFlag>(1 << 1);
    REQUIRE(enum_cast<LanguageFlag&>(1 << 2).value() == LanguageFlag::한국어);
    REQUIRE(enum_cast<const LanguageFlag>(1 << 3).value() == LanguageFlag::English);
    REQUIRE(lang.value() == LanguageFlag::日本語);
    REQUIRE(enum_cast<LanguageFlag>(1 << 4).value() == LanguageFlag::😃);
    REQUIRE_FALSE(enum_cast<LanguageFlag>(0).has_value());
  }
}

TEST_CASE("flag enum_index") {
    constexpr auto lang = enum_index<LanguageFlag>(LanguageFlag::日本語);
    LanguageFlag korean = LanguageFlag::한국어;
    REQUIRE(enum_index<LanguageFlag&>(korean).value() == 1);
    REQUIRE(enum_index<const LanguageFlag>(LanguageFlag::English).value() == 2);
    REQUIRE(enum_index(LanguageFlag::😃).value() == 3);
    REQUIRE(lang.value() == 0);
    REQUIRE_FALSE(enum_index(static_cast<LanguageFlag>(0)).has_value());
}

TEST_CASE("flag enum_contains") {
  SECTION("value") {
    constexpr auto lang = enum_index<LanguageFlag>(LanguageFlag::日本語);
    LanguageFlag korean = LanguageFlag::한국어;
    REQUIRE(enum_contains<LanguageFlag&>(korean));
    REQUIRE(enum_contains<const LanguageFlag>(LanguageFlag::English));
    REQUIRE(enum_contains(LanguageFlag::😃));
    REQUIRE(lang);
    REQUIRE_FALSE(enum_contains(static_cast<LanguageFlag>(0)));
  }

  SECTION("integer") {
    constexpr auto lang = enum_contains<LanguageFlag&>(1 << 1);
    REQUIRE(lang);
    REQUIRE(enum_contains<const LanguageFlag>(1 << 2));
    REQUIRE(enum_contains<LanguageFlag>(1 << 3));
    REQUIRE(enum_contains<LanguageFlag>(1 << 4));
    REQUIRE_FALSE(enum_contains(static_cast<LanguageFlag>(0)));
  }

  SECTION("string") {
    auto lang = std::string{"日本語"};
    REQUIRE(enum_contains<LanguageFlag&>("한국어"));
    REQUIRE(enum_contains<LanguageFlag>("English"));
    REQUIRE(enum_contains<const LanguageFlag>(lang));
    REQUIRE(enum_contains<LanguageFlag>("😃"));
    REQUIRE_FALSE(enum_contains<LanguageFlag>("None"));
  }
}


TEST_CASE("flag enum_value") {
  constexpr auto lang = enum_value<LanguageFlag>(3);
  REQUIRE(enum_value<LanguageFlag&>(0) == LanguageFlag::日本語);
  REQUIRE(enum_value<const LanguageFlag>(1) == LanguageFlag::한국어);
  REQUIRE(enum_value<LanguageFlag>(2) == LanguageFlag::English);
  REQUIRE(lang == LanguageFlag::😃);
}

TEST_CASE("flag enum_values") {
  constexpr auto& s5 = enum_values<const LanguageFlag>();
  REQUIRE(s5 == std::array<LanguageFlag, 4>{{LanguageFlag::日本語, LanguageFlag::한국어, LanguageFlag::English, LanguageFlag::😃}});
}

TEST_CASE("flag enum_count") {
  constexpr auto s5 = enum_count<LanguageFlag>();
  REQUIRE(s5 == 4);
}

TEST_CASE("flag enum_name") {
  SECTION("automatic storage") {
    constexpr LanguageFlag lang = LanguageFlag::日本語;
    constexpr auto lang_name = enum_name(lang);
    LanguageFlag lk = LanguageFlag::한국어;
    REQUIRE(enum_name<LanguageFlag&>(lk) == "한국어");
    REQUIRE(enum_name<const LanguageFlag>(LanguageFlag::English) == "English");
    REQUIRE(lang_name == "日本語");
    REQUIRE(enum_name(LanguageFlag::😃) == "😃");
    REQUIRE(enum_name(static_cast<LanguageFlag>(0)).empty());
  }
}

TEST_CASE("flag enum_flags_name") {
  constexpr LanguageFlag lang = LanguageFlag::日本語;
  auto lang_name = enum_flags_name(lang);
  LanguageFlag lk = LanguageFlag::한국어;
  REQUIRE(enum_flags_name<LanguageFlag&>(lk) == "한국어");
  REQUIRE(enum_flags_name<const LanguageFlag>(LanguageFlag::English) == "English");
  REQUIRE(lang_name == "日本語");
  REQUIRE(enum_flags_name(LanguageFlag::😃) == "😃");
  REQUIRE(enum_flags_name(static_cast<LanguageFlag>(0)).empty());
}

TEST_CASE("flag enum_names") {
  constexpr auto& s5 = enum_names<const LanguageFlag>();
  REQUIRE(s5 == std::array<std::string_view, 4>{{"日本語", "한국어", "English", "😃"}});
}

TEST_CASE("flag enum_entries") {
  constexpr auto& s5 = enum_entries<const LanguageFlag>();
  REQUIRE(s5 == std::array<std::pair<LanguageFlag, std::string_view>, 4>{{{LanguageFlag::日本語, "日本語"}, {LanguageFlag::한국어, "한국어"}, {LanguageFlag::English, "English"}, {LanguageFlag::😃, "😃"}}});
}

TEST_CASE("flag ostream_operators") {
  auto test_ostream = [](auto e, std::string name) {
    using namespace magic_enum::ostream_operators;
    std::stringstream ss;
    ss << e;
    REQUIRE(ss.str() == name);
  };

  test_ostream(std::make_optional(LanguageFlag::日本語), "日本語");
  test_ostream(LanguageFlag::한국어, "한국어");
  test_ostream(LanguageFlag::English, "English");
  test_ostream(LanguageFlag::😃, "😃");
  test_ostream(static_cast<LanguageFlag>(0), "0");
  test_ostream(std::make_optional(static_cast<LanguageFlag>(0)), "0");
}

TEST_CASE("flag istream_operators") {
  auto test_istream = [](const auto e, std::string name) {
    using namespace magic_enum::istream_operators;
    std::istringstream ss(name);
    std::decay_t<decltype(e)> v;
    ss >> v;
    REQUIRE(v == e);
    REQUIRE(ss);
  };

  test_istream(LanguageFlag::한국어, "한국어");
  test_istream(LanguageFlag::English, "English");
  test_istream(LanguageFlag::😃, "😃");
}


TEST_CASE("flag bitwise_operators") {
  using namespace magic_enum::bitwise_operators;
  SECTION("operator~") {
    REQUIRE(enum_integer(~LanguageFlag::日本語) == ~enum_integer(LanguageFlag::日本語));
  }

  SECTION("operator|") {
    REQUIRE(enum_integer(LanguageFlag::日本語 | LanguageFlag::한국어) == (enum_integer(LanguageFlag::日本語) | enum_integer(LanguageFlag::한국어)));
  }

  SECTION("operator&") {
    REQUIRE(enum_integer(LanguageFlag::日本語 & LanguageFlag::한국어) == (enum_integer(LanguageFlag::日本語) & enum_integer(LanguageFlag::한국어)));
  }

  SECTION("operator^") {
    REQUIRE(enum_integer(LanguageFlag::日本語 ^ LanguageFlag::한국어) == (enum_integer(LanguageFlag::日本語) ^ enum_integer(LanguageFlag::한국어)));
  }

  SECTION("operator|=") {
    LanguageFlag x5 = LanguageFlag::日本語;
    x5 |= LanguageFlag::한국어;
    REQUIRE(enum_integer(x5) == (enum_integer(LanguageFlag::日本語) | enum_integer(LanguageFlag::한국어)));
  }

  SECTION("operator&=") {
    LanguageFlag x5 = LanguageFlag::日本語;
    x5 &= LanguageFlag::한국어;
    REQUIRE(enum_integer(x5) == (enum_integer(LanguageFlag::日本語) & enum_integer(LanguageFlag::한국어)));
  }

  SECTION("operator^=") {
    LanguageFlag x5 = LanguageFlag::日本語;
    x5 ^= LanguageFlag::한국어;
    REQUIRE(enum_integer(x5) == (enum_integer(LanguageFlag::日本語) ^ enum_integer(LanguageFlag::한국어)));
  }
}
