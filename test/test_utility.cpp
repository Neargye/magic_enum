// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include "test_enums.hpp"
#include "test_flags_enums.hpp"

#include <magic_enum/magic_enum_utility.hpp>

#include "test_helpers.hpp"

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <string_view>
#include <tuple>
#include <type_traits>

using namespace magic_enum;
using namespace magic_enum_tests;

template <Color C>
constexpr std::string_view DoWork() {
  return "default";
}

template <>
constexpr std::string_view DoWork<Color::GREEN>() {
  return "override";
}

struct LvalueOnlyForEach {
  template <typename T>
  constexpr int operator()(T value) & noexcept {
    return enum_integer(value());
  }
};

struct ForEachCommaResult {
  ForEachCommaResult operator,(ForEachCommaResult) const = delete;
};

TEST_CASE("enum_for_each") {
  SUBCASE("discarded results with overloaded comma") {
    std::array<Color, 3> visited{};
    std::size_t calls = 0;
    enum_for_each<Color>([&](auto value) {
      visited[calls++] = value;
      if constexpr (value != Color::RED) {
        return ForEachCommaResult{};
      }
    });
    REQUIRE(calls == visited.size());
    REQUIRE(visited == enum_values<Color>());
  }

  SUBCASE("rvalue argument") {
    constexpr auto values = enum_for_each<Color>(RvalueOnlyArgument{});
    static_assert(values[0] == 7 && values[1] == 7 && values[2] == 7);
  }

  SUBCASE("member function pointer") {
    enum class Single { value };
    constexpr auto member = &enum_constant<Single::value>::operator();
    constexpr auto values = enum_for_each<Single>(member);
    static_assert(std::is_same_v<std::remove_const_t<decltype(values)>, std::array<Single, 1>>);
    static_assert(values[0] == Single::value);
    REQUIRE(enum_for_each<Single>(std::ref(member))[0] == Single::value);
  }

  SUBCASE("no return type") {
    underlying_type_t<Color> sum{};
    enum_for_each<Color>([&sum](auto val) {
      constexpr underlying_type_t<Color> v = enum_integer(val());
      sum += v;
    });
    REQUIRE(sum == 10);
  }

  SUBCASE("same return type") {
    constexpr auto workResults = enum_for_each<Color>([](auto val) {
      return DoWork<val>();
    });
    REQUIRE(workResults == std::array<std::string_view, 3>{{"default", "override", "default"}});

    constexpr auto colorValues = enum_for_each<Color>(LvalueOnlyForEach{});
    REQUIRE(colorValues == std::array<int, 3>{{-12, 7, 15}});
  }

  SUBCASE("reference and value return types") {
    int value = 7;
    const auto results = enum_for_each<Color>([&](auto color) -> decltype(auto) {
      if constexpr (color == Color::RED) {
        return (value);
      } else {
        return int{value};
      }
    });
    static_assert(std::is_same_v<std::remove_const_t<decltype(results)>, std::tuple<int, int, int>>);
    value = 9;
    REQUIRE(std::get<0>(results) == 7);
  }

  SUBCASE("different return type") {
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

#endif

TEST_CASE("enum_next_value") {
  REQUIRE(enum_next_value(Color::RED) == Color::GREEN);
  REQUIRE(enum_next_value(Color::RED, 2) == Color::BLUE);
  REQUIRE(enum_next_value(Color::RED, 1) == Color::GREEN);
  REQUIRE(enum_next_value(Color::RED, 0) == Color::RED);
  REQUIRE(enum_next_value(Color::BLUE, -2) == Color::RED);
  REQUIRE(enum_next_value(Color::BLUE, -1) == Color::GREEN);
  REQUIRE_FALSE(enum_next_value(Color::BLUE).has_value());
  REQUIRE_FALSE(enum_next_value(Color::RED, -1).has_value());
  REQUIRE_FALSE(enum_next_value(Color::RED, 10).has_value());
  REQUIRE_FALSE(enum_next_value(Color::GREEN, (std::numeric_limits<std::ptrdiff_t>::max)()).has_value());
  REQUIRE_FALSE(enum_next_value(Color::GREEN, (std::numeric_limits<std::ptrdiff_t>::min)()).has_value());
}

TEST_CASE("enum_next_value_circular") {
  REQUIRE(enum_next_value_circular(Color::RED) == Color::GREEN);
  REQUIRE(enum_next_value_circular(Color::RED, 2) == Color::BLUE);
  REQUIRE(enum_next_value_circular(Color::RED, 1) == Color::GREEN);
  REQUIRE(enum_next_value_circular(Color::RED, 0) == Color::RED);
  REQUIRE(enum_next_value_circular(Color::BLUE, -2) == Color::RED);
  REQUIRE(enum_next_value_circular(Color::BLUE, -1) == Color::GREEN);
  REQUIRE(enum_next_value_circular(Color::BLUE) == Color::RED);
  REQUIRE(enum_next_value_circular(Color::BLUE, 4) == Color::RED);
  REQUIRE(enum_next_value_circular(Color::BLUE, 3) == Color::BLUE);
  REQUIRE(enum_next_value_circular(Color::BLUE, 2) == Color::GREEN);
  REQUIRE(enum_next_value_circular(Color::BLUE, 1) == Color::RED);
  REQUIRE(enum_next_value_circular(Color::BLUE, 0) == Color::BLUE);
  REQUIRE(enum_next_value_circular(Color::RED, -1) == Color::BLUE);
  REQUIRE(enum_next_value_circular(Color::RED, -2) == Color::GREEN);
  REQUIRE(enum_next_value_circular(Color::RED, -3) == Color::RED);
  REQUIRE(enum_next_value_circular(Color::RED, -4) == Color::BLUE);
  REQUIRE(enum_next_value_circular(Color::GREEN, (std::numeric_limits<std::ptrdiff_t>::max)()) == Color::BLUE);
  REQUIRE(enum_next_value_circular(Color::GREEN, (std::numeric_limits<std::ptrdiff_t>::min)()) == Color::BLUE);
}

TEST_CASE("enum_prev_value") {
  REQUIRE(enum_prev_value(Color::BLUE) == Color::GREEN);
  REQUIRE(enum_prev_value(Color::BLUE, 2) == Color::RED);
  REQUIRE(enum_prev_value(Color::BLUE, 1) == Color::GREEN);
  REQUIRE(enum_prev_value(Color::RED, -2) == Color::BLUE);
  REQUIRE(enum_prev_value(Color::RED, -1) == Color::GREEN);
  REQUIRE(enum_prev_value(Color::BLUE, 0) == Color::BLUE);
  REQUIRE_FALSE(enum_prev_value(Color::RED).has_value());
  REQUIRE_FALSE(enum_prev_value(Color::BLUE, -1).has_value());
  REQUIRE_FALSE(enum_prev_value(Color::BLUE, 10).has_value());
  REQUIRE_FALSE(enum_prev_value(Color::GREEN, (std::numeric_limits<std::ptrdiff_t>::max)()).has_value());
  REQUIRE_FALSE(enum_prev_value(Color::GREEN, (std::numeric_limits<std::ptrdiff_t>::min)()).has_value());
}

TEST_CASE("enum_prev_value_circular") {
  REQUIRE(enum_prev_value_circular(Color::RED) == Color::BLUE);
  REQUIRE(enum_prev_value_circular(Color::RED, 2) == Color::GREEN);
  REQUIRE(enum_prev_value_circular(Color::RED, 1) == Color::BLUE);
  REQUIRE(enum_prev_value_circular(Color::RED, 0) == Color::RED);
  REQUIRE(enum_prev_value_circular(Color::BLUE, -2) == Color::GREEN);
  REQUIRE(enum_prev_value_circular(Color::BLUE, -1) == Color::RED);
  REQUIRE(enum_prev_value_circular(Color::BLUE) == Color::GREEN);
  REQUIRE(enum_prev_value_circular(Color::BLUE, 4) == Color::GREEN);
  REQUIRE(enum_prev_value_circular(Color::BLUE, 3) == Color::BLUE);
  REQUIRE(enum_prev_value_circular(Color::BLUE, 2) == Color::RED);
  REQUIRE(enum_prev_value_circular(Color::BLUE, 1) == Color::GREEN);
  REQUIRE(enum_prev_value_circular(Color::BLUE, 0) == Color::BLUE);
  REQUIRE(enum_prev_value_circular(Color::RED, -1) == Color::GREEN);
  REQUIRE(enum_prev_value_circular(Color::RED, -2) == Color::BLUE);
  REQUIRE(enum_prev_value_circular(Color::RED, -3) == Color::RED);
  REQUIRE(enum_prev_value_circular(Color::RED, -4) == Color::GREEN);
  REQUIRE(enum_prev_value_circular(Color::GREEN, (std::numeric_limits<std::ptrdiff_t>::max)()) == Color::RED);
  REQUIRE(enum_prev_value_circular(Color::GREEN, (std::numeric_limits<std::ptrdiff_t>::min)()) == Color::RED);
}

enum class UtilityEmpty {};

TEST_CASE("empty enum next and prev") {
  static_assert(enum_count<UtilityEmpty>() == 0);
  static_assert(!enum_next_value(UtilityEmpty{}).has_value());
  static_assert(!enum_prev_value(UtilityEmpty{}, -1).has_value());

  // The circular functions have no valid argument here, but they must compile without a modulo by zero warning.
  volatile bool call_circular = false;
  if (call_circular) {
    CHECK(enum_next_value_circular(UtilityEmpty{}) == enum_prev_value_circular(UtilityEmpty{}));
  }
}

namespace flags_test {

#if defined(MAGIC_ENUM_SUPPORTED_CONSTEXPR_FOR)

TEST_CASE("flags enum_for_each") {
  REQUIRE(enum_for_each<Directions>([](auto val) { return val(); }) == enum_values<Directions>());
  REQUIRE(enum_for_each<BoolFlags>([](auto val) { return val(); }) == enum_values<BoolFlags>());

  std::size_t calls = 0;
  enum_for_each<Numbers>([&calls](auto) { ++calls; });
  REQUIRE(calls == enum_count<Numbers>());
}

TEST_CASE("flags constexpr_for") {
  constexpr_for<0, magic_enum::enum_count<Color>(), 1>([](auto i) {
    [[maybe_unused]] Foo<Color, magic_enum::enum_value<Color, i>()> bar{};
  });
}

#endif

TEST_CASE("flags enum_next_value") {
  REQUIRE(enum_next_value(Color::RED) == Color::GREEN);
  REQUIRE(enum_next_value(Color::RED, 2) == Color::BLUE);
  REQUIRE(enum_next_value(Color::RED, 1) == Color::GREEN);
  REQUIRE(enum_next_value(Color::RED, 0) == Color::RED);
  REQUIRE(enum_next_value(Color::BLUE, -2) == Color::RED);
  REQUIRE(enum_next_value(Color::BLUE, -1) == Color::GREEN);
  REQUIRE_FALSE(enum_next_value(Color::BLUE).has_value());
  REQUIRE_FALSE(enum_next_value(Color::RED, -1).has_value());
  REQUIRE_FALSE(enum_next_value(Color::RED, 10).has_value());
}

TEST_CASE("flags enum_next_value_circular") {
  REQUIRE(enum_next_value_circular(Color::RED) == Color::GREEN);
  REQUIRE(enum_next_value_circular(Color::RED, 2) == Color::BLUE);
  REQUIRE(enum_next_value_circular(Color::RED, 1) == Color::GREEN);
  REQUIRE(enum_next_value_circular(Color::RED, 0) == Color::RED);
  REQUIRE(enum_next_value_circular(Color::BLUE, -2) == Color::RED);
  REQUIRE(enum_next_value_circular(Color::BLUE, -1) == Color::GREEN);
  REQUIRE(enum_next_value_circular(Color::BLUE) == Color::RED);
  REQUIRE(enum_next_value_circular(Color::BLUE, 4) == Color::RED);
  REQUIRE(enum_next_value_circular(Color::BLUE, 3) == Color::BLUE);
  REQUIRE(enum_next_value_circular(Color::BLUE, 2) == Color::GREEN);
  REQUIRE(enum_next_value_circular(Color::BLUE, 1) == Color::RED);
  REQUIRE(enum_next_value_circular(Color::BLUE, 0) == Color::BLUE);
  REQUIRE(enum_next_value_circular(Color::RED, -1) == Color::BLUE);
  REQUIRE(enum_next_value_circular(Color::RED, -2) == Color::GREEN);
  REQUIRE(enum_next_value_circular(Color::RED, -3) == Color::RED);
  REQUIRE(enum_next_value_circular(Color::RED, -4) == Color::BLUE);
}

TEST_CASE("flags enum_prev_value") {
  REQUIRE(enum_prev_value(Color::BLUE) == Color::GREEN);
  REQUIRE(enum_prev_value(Color::BLUE, 2) == Color::RED);
  REQUIRE(enum_prev_value(Color::BLUE, 1) == Color::GREEN);
  REQUIRE(enum_prev_value(Color::RED, -2) == Color::BLUE);
  REQUIRE(enum_prev_value(Color::RED, -1) == Color::GREEN);
  REQUIRE(enum_prev_value(Color::BLUE, 0) == Color::BLUE);
  REQUIRE_FALSE(enum_prev_value(Color::RED).has_value());
  REQUIRE_FALSE(enum_prev_value(Color::BLUE, -1).has_value());
  REQUIRE_FALSE(enum_prev_value(Color::BLUE, 10).has_value());
}

TEST_CASE("flags enum_prev_value_circular") {
  REQUIRE(enum_prev_value_circular(Color::RED) == Color::BLUE);
  REQUIRE(enum_prev_value_circular(Color::RED, 2) == Color::GREEN);
  REQUIRE(enum_prev_value_circular(Color::RED, 1) == Color::BLUE);
  REQUIRE(enum_prev_value_circular(Color::RED, 0) == Color::RED);
  REQUIRE(enum_prev_value_circular(Color::BLUE, -2) == Color::GREEN);
  REQUIRE(enum_prev_value_circular(Color::BLUE, -1) == Color::RED);
  REQUIRE(enum_prev_value_circular(Color::BLUE) == Color::GREEN);
  REQUIRE(enum_prev_value_circular(Color::BLUE, 4) == Color::GREEN);
  REQUIRE(enum_prev_value_circular(Color::BLUE, 3) == Color::BLUE);
  REQUIRE(enum_prev_value_circular(Color::BLUE, 2) == Color::RED);
  REQUIRE(enum_prev_value_circular(Color::BLUE, 1) == Color::GREEN);
  REQUIRE(enum_prev_value_circular(Color::BLUE, 0) == Color::BLUE);
  REQUIRE(enum_prev_value_circular(Color::RED, -1) == Color::GREEN);
  REQUIRE(enum_prev_value_circular(Color::RED, -2) == Color::BLUE);
  REQUIRE(enum_prev_value_circular(Color::RED, -3) == Color::RED);
  REQUIRE(enum_prev_value_circular(Color::RED, -4) == Color::GREEN);
}

} // namespace flags_test
