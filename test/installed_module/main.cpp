// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT

#include <version>

#ifdef MAGIC_ENUM_TEST_FMT
#  include <fmt/format.h>
#endif

#ifdef MAGIC_ENUM_TEST_IMPORT_STD
import std;
#else
#  include <functional>
#  include <sstream>
#  include <type_traits>
#  if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
#    include <format>
#  endif
#endif

import magic_enum;

enum class Color { RED, GREEN, BLUE };
enum class Directions { LEFT = 1, RIGHT = 2 };
enum class ReflectionRange { Low = -1000, High = 1000 };

template <>
struct magic_enum::customize::enum_range<Directions> {
  static constexpr bool is_flags = true;
};

template <>
struct magic_enum::customize::enum_range<ReflectionRange> {
  static constexpr int min = -1;
  static constexpr int max = 1;
};

static_assert(magic_enum::enum_count<Color>() == 3);
static_assert(magic_enum::enum_integer(Color::GREEN) == 1);
static_assert(magic_enum::string_view{"BLUE"}.size() == 4);
static_assert(magic_enum::optional<int>{7}.value() == 7);
static_assert(magic_enum::enum_cast<Color>("GREEN").value() == Color::GREEN);
static_assert(magic_enum::enum_flags_cast<Directions>("LEFT|RIGHT").value() == static_cast<Directions>(3));

constexpr bool switch_fallback = true;
constexpr auto switch_eq_red = [](auto value) { return value() == Color::RED; };
static_assert(std::is_same_v<decltype(magic_enum::enum_switch(switch_eq_red, Color::GREEN, switch_fallback)), bool>);
static_assert(!magic_enum::enum_switch(switch_eq_red, Color::GREEN, switch_fallback));
static_assert(magic_enum::enum_switch(switch_eq_red, static_cast<Color>(-1), switch_fallback));

struct ImmovableSwitchResult {
  int value;
  constexpr ImmovableSwitchResult(int v = 0) : value{v} {}
  ImmovableSwitchResult(const ImmovableSwitchResult&) = delete;
};

static_assert(magic_enum::enum_switch<const ImmovableSwitchResult&>(switch_eq_red, Color::RED).value == 1);
static_assert(magic_enum::enum_switch<ImmovableSwitchResult, magic_enum::as_common<>>(switch_eq_red, static_cast<Color>(-1)).value == 0);

#ifdef MAGIC_ENUM_TEST_STD_REFLECTION
constexpr auto reflection_range_values = magic_enum::enum_values<ReflectionRange>();
static_assert(reflection_range_values.size() == 2);
static_assert(reflection_range_values[0] == ReflectionRange::Low);
static_assert(reflection_range_values[1] == ReflectionRange::High);
static_assert(magic_enum::enum_reflected(ReflectionRange::High));
static_assert(!magic_enum::enum_reflected(ReflectionRange{0}));
#endif

constexpr magic_enum::containers::bitset<Color> color_bits{magic_enum::containers::raw_access, 5ULL};

int main() {
  if (std::hash<magic_enum::containers::bitset<Color>>{}(color_bits) !=
      std::hash<unsigned long long>{}(5ULL)) {
    return 1;
  }
  {
    using namespace magic_enum::iostream_operators;

    std::ostringstream output;
    output << Color::GREEN << ' ' << static_cast<Directions>(3);
    if (output.str() != "GREEN LEFT|RIGHT") {
      return 2;
    }

    Color parsed_color = Color::RED;
    std::istringstream input{"BLUE"};
    input >> parsed_color;
    if (!input || parsed_color != Color::BLUE) {
      return 3;
    }
  }
  {
    std::ostringstream output;
    output << color_bits;
    if (output.str() != "RED|BLUE") {
      return 4;
    }

    magic_enum::containers::bitset<Color> parsed_bits;
    std::istringstream input{"RED|BLUE"};
    input >> parsed_bits;
    if (!input || parsed_bits.to_ullong(magic_enum::containers::raw_access) != 5ULL) {
      return 5;
    }
  }
#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
  if (std::format("{}", Color::GREEN) != "GREEN") {
    return 6;
  }
#endif
#ifdef MAGIC_ENUM_TEST_FMT
  if (fmt::format("{}", Color::BLUE) != "BLUE") {
    return 7;
  }
#endif
}
