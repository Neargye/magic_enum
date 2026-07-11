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

import magic_enum;

enum class Color { RED, GREEN, BLUE };
enum class Directions { LEFT = 1, RIGHT = 2 };

template <>
struct magic_enum::customize::enum_range<Directions> {
  static constexpr bool is_flags = true;
};

static_assert(!magic_enum::is_flags_enum<Color>::value);
static_assert(!magic_enum::is_flags_v<Color>);
static_assert(magic_enum::is_flags_enum<Directions>::value);
static_assert(magic_enum::is_flags_v<Directions>);

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
  return std::hash<ColorBitset>{}(color_bits) == std::hash<unsigned long long>{}(5ULL) ? 0 : 1;
}
