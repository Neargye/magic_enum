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

constexpr auto color_array = magic_enum::containers::make_array<Color>(1, 2, 3);
static_assert(color_array[Color::GREEN] == 2);

constexpr magic_enum::containers::bitset<Color> color_bits {magic_enum::containers::raw_access, 5ULL};
static_assert(color_bits.test(Color::RED));
static_assert(!color_bits.test(Color::GREEN));
static_assert(color_bits.test(Color::BLUE));

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

int main() {}
