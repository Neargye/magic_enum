// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT

#include <version>

#ifdef MAGIC_ENUM_TEST_IMPORT_STD
import std;
#elif defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
#  include <format>
#endif

#ifdef MAGIC_ENUM_TEST_FMT
#  include <fmt/format.h>
#endif

import magic_enum;

enum class Color { RED, GREEN, BLUE };

static_assert(magic_enum::enum_count<Color>() == 3);
static_assert(magic_enum::enum_integer(Color::GREEN) == 1);
static_assert(magic_enum::string_view{"BLUE"}.size() == 4);
static_assert(magic_enum::optional<int>{7}.value() == 7);

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
