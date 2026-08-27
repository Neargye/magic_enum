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
#  if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
#    include <format>
#  endif
#endif

import magic_enum;

enum class Color { RED, GREEN, BLUE };
enum class ReflectionRange { Low = -1000, High = 1000 };

template <>
struct magic_enum::customize::enum_range<ReflectionRange> {
  static constexpr int min = -1;
  static constexpr int max = 1;
};

static_assert(magic_enum::enum_count<Color>() == 3);
static_assert(magic_enum::enum_integer(Color::GREEN) == 1);
static_assert(magic_enum::string_view{"BLUE"}.size() == 4);
static_assert(magic_enum::optional<int>{7}.value() == 7);

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
    return 3;
  }
#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
  if (std::format("{}", Color::GREEN) != "GREEN") {
    return 1;
  }
#endif
#ifdef MAGIC_ENUM_TEST_FMT
  if (fmt::format("{}", Color::BLUE) != "BLUE") {
    return 2;
  }
#endif
}
