// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <version>

#ifdef MAGIC_ENUM_TEST_FMT
#  include <fmt/format.h>
#endif

#ifdef MAGIC_ENUM_TEST_IMPORT_STD
import std;
#else
#  include <functional>
#  include <sstream>
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

static_assert(!magic_enum::is_flags_enum<Color>::value);
static_assert(!magic_enum::is_flags_v<Color>);
static_assert(magic_enum::is_flags_enum<Directions>::value);
static_assert(magic_enum::is_flags_v<Directions>);
static_assert(magic_enum::enum_reflected<Directions>(Directions::RIGHT));
static_assert(!magic_enum::enum_reflected<Directions>(static_cast<Directions>(3)));
static_assert(magic_enum::enum_cast<Color>("GREEN").value() == Color::GREEN);
static_assert(magic_enum::enum_contains<Color>("BLUE"));
static_assert(magic_enum::enum_flags_cast<Directions>("LEFT|RIGHT").value() == static_cast<Directions>(3));
static_assert(magic_enum::enum_flags_contains<Directions>("LEFT|RIGHT"));

#ifdef MAGIC_ENUM_TEST_STD_REFLECTION
constexpr auto reflection_range_values = magic_enum::enum_values<ReflectionRange>();
static_assert(reflection_range_values.size() == 2);
static_assert(reflection_range_values[0] == ReflectionRange::Low);
static_assert(reflection_range_values[1] == ReflectionRange::High);
static_assert(magic_enum::enum_reflected(ReflectionRange::Low));
static_assert(!magic_enum::enum_reflected(ReflectionRange{0}));
#endif

// Keep this list in sync with the public declarations exported by magic_enum.cppm.
// Using-declarations verify reachability without relying on unrelated standard
// library operators in a consumer that imports only magic_enum.
namespace module_api {

using magic_enum::Enum;
using magic_enum::char_type;
using magic_enum::as_common;
using magic_enum::as_flags;
using magic_enum::case_insensitive;
using magic_enum::enum_cast;
using magic_enum::enum_constant;
using magic_enum::enum_contains;
using magic_enum::enum_count;
using magic_enum::enum_entries;
using magic_enum::enum_flags_cast;
using magic_enum::enum_flags_contains;
using magic_enum::enum_flags_name;
using magic_enum::enum_flags_test;
using magic_enum::enum_flags_test_any;
using magic_enum::enum_for_each;
using magic_enum::enum_fuse;
using magic_enum::enum_index;
using magic_enum::enum_integer;
using magic_enum::enum_name;
using magic_enum::enum_names;
using magic_enum::enum_next_value;
using magic_enum::enum_next_value_circular;
using magic_enum::enum_prev_value;
using magic_enum::enum_prev_value_circular;
using magic_enum::enum_reflected;
using magic_enum::enum_switch;
using magic_enum::enum_type_name;
using magic_enum::enum_underlying;
using magic_enum::enum_value;
using magic_enum::enum_values;
using magic_enum::is_flags_enum;
using magic_enum::is_flags_v;
using magic_enum::is_magic_enum_supported;
using magic_enum::is_scoped_enum;
using magic_enum::is_scoped_enum_v;
using magic_enum::is_unscoped_enum;
using magic_enum::is_unscoped_enum_v;
using magic_enum::optional;
using magic_enum::string;
using magic_enum::string_view;
using magic_enum::underlying_type;
using magic_enum::underlying_type_t;

using magic_enum::customize::adl_info;
using magic_enum::customize::adl_info_holder;
using magic_enum::customize::customize_t;
using magic_enum::customize::default_tag;
using magic_enum::customize::enum_name;
using magic_enum::customize::enum_range;
using magic_enum::customize::enum_type_name;
using magic_enum::customize::invalid_tag;

using magic_enum::containers::array;
using magic_enum::containers::bitset;
using magic_enum::containers::comparator_indexing;
using magic_enum::containers::default_indexing;
using magic_enum::containers::get;
using magic_enum::containers::make_array;
using magic_enum::containers::name_greater;
using magic_enum::containers::name_greater_case_insensitive;
using magic_enum::containers::name_less;
using magic_enum::containers::name_less_case_insensitive;
using magic_enum::containers::raw_access;
using magic_enum::containers::set;
using magic_enum::containers::to_array;

using magic_enum::bitwise_operators::operator&;
using magic_enum::bitwise_operators::operator&=;
using magic_enum::bitwise_operators::operator|;
using magic_enum::bitwise_operators::operator|=;
using magic_enum::bitwise_operators::operator^;
using magic_enum::bitwise_operators::operator^=;
using magic_enum::bitwise_operators::operator~;
using magic_enum::iostream_operators::operator<<;
using magic_enum::iostream_operators::operator>>;
using magic_enum::istream_operators::operator>>;
using magic_enum::ostream_operators::operator<<;

#ifdef MAGIC_ENUM_TEST_FMT
using fmt::formatter;
#endif

} // namespace module_api

static_assert(magic_enum::optional<int>{42}.value() == 42);
static_assert(magic_enum::string_view{"GREEN"}.size() == 5);
static_assert(magic_enum::customize::adl_info_holder<true, 1, 4>::is_flags);

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_type_name<Color>() noexcept {
  return "Paint";
}

constexpr auto color_type_name = magic_enum::enum_type_name<Color>();
static_assert(color_type_name.size() == 5);
static_assert(color_type_name[0] == 'P');

constexpr auto color_array = magic_enum::containers::make_array<Color>(1, 2, 3);
static_assert(color_array[Color::GREEN] == 2);
static_assert(magic_enum::containers::get<Color::BLUE>(color_array) == 3);

constexpr magic_enum::containers::bitset<Color> color_bits {magic_enum::containers::raw_access, 5ULL};
static_assert(color_bits.test(Color::RED));
static_assert(!color_bits.test(Color::GREEN));
static_assert(color_bits.test(Color::BLUE));

using ColorBitset = magic_enum::containers::bitset<Color>;

struct GreenOnly {
  template <Color C>
  constexpr int operator()(magic_enum::enum_constant<C>) const requires (C == Color::GREEN) {
    return 42;
  }
};

static_assert(magic_enum::enum_switch(GreenOnly{}, Color::GREEN) == 42);

using ColorNameLess = magic_enum::containers::name_less<>;
using ColorNameGreater = magic_enum::containers::name_greater<>;
using ColorNameLessCaseInsensitive = magic_enum::containers::name_less_case_insensitive;
using ColorNameGreaterCaseInsensitive = magic_enum::containers::name_greater_case_insensitive;
using ColorIndex = magic_enum::containers::default_indexing<Color>;
using ColorNameIndex = magic_enum::containers::comparator_indexing<ColorNameLess>;

static_assert(ColorNameLess{}(Color::BLUE, Color::GREEN));
static_assert(ColorNameGreater{}(Color::GREEN, Color::BLUE));
static_assert(ColorNameLessCaseInsensitive{}(Color::BLUE, Color::GREEN));
static_assert(ColorNameGreaterCaseInsensitive{}(Color::GREEN, Color::BLUE));
static_assert(*ColorIndex::at(Color::GREEN) == 1);
static_assert(*ColorNameIndex::at(Color::GREEN) == 1);

constexpr magic_enum::containers::set<Color, ColorNameLess> color_set {Color::RED, Color::BLUE};
static_assert(color_set.contains(Color::BLUE));

int main() {
  if (std::hash<ColorBitset>{}(color_bits) != std::hash<unsigned long long>{}(5ULL)) {
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
    Directions parsed_directions = Directions::LEFT;
    std::istringstream input{"BLUE LEFT|RIGHT"};
    input >> parsed_color >> parsed_directions;
    if (!input || parsed_color != Color::BLUE || parsed_directions != static_cast<Directions>(3)) {
      return 3;
    }
  }
  {
    std::ostringstream output;
    output << color_bits;
    if (output.str() != "RED|BLUE") {
      return 4;
    }

    ColorBitset parsed_bits;
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
  return 0;
}
