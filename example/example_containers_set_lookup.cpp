// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.
// Copyright (c) 2022 - 2023 Bela Schaum <schaumb@gmail.com>.
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

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4244) // warning C4244: 'argument': conversion from 'const T' to 'unsigned int', possible loss of data.
#endif

#include <array>
#include <iostream>
#include <string_view>

#include <magic_enum/magic_enum_containers.hpp>

enum class Color { RED = 1, GREEN = 2, BLUE = 4 };
template <>
struct magic_enum::customize::enum_range<Color> {
  static constexpr bool is_flags = true;
};

int main() {
  std::cout << std::boolalpha;

  magic_enum::containers::set<Color> colors;
  const Color red = Color::RED;
  colors.insert(colors.cbegin(), red);
  colors.insert(colors.cbegin(), Color::GREEN);
  colors.emplace(Color::BLUE);
  colors.emplace_hint(colors.cbegin(), Color::BLUE);

  std::array<Color, 1> extra {Color::RED};
  colors.insert(extra.begin(), extra.end());
  colors.insert({Color::GREEN, Color::BLUE});

  std::cout << colors.empty() << std::endl;
  std::cout << colors.size() << std::endl;
  std::cout << colors.max_size() << std::endl;

  std::cout << colors.count(Color::GREEN) << std::endl;
  std::cout << colors.contains(Color::GREEN) << std::endl;
  std::cout << (colors.find(Color::GREEN) != colors.end()) << std::endl;
  std::cout << (colors.lower_bound(Color::GREEN) != colors.end()) << std::endl;
  std::cout << (colors.upper_bound(Color::GREEN) != colors.end()) << std::endl;
  auto by_key = colors.equal_range(Color::GREEN);
  std::cout << (by_key.first != by_key.second) << std::endl;

  colors.erase(colors.cbegin());
  colors.erase(colors.cbegin(), colors.cend());
  colors.insert({Color::RED, Color::GREEN, Color::BLUE});
  std::cout << colors.erase(Color::RED) << std::endl;
  std::cout << colors.erase_if([](Color c) { return c == Color::BLUE; }) << std::endl;

  using name_set = magic_enum::containers::set<Color, magic_enum::containers::name_less<>>;
  name_set by_name {Color::RED, Color::GREEN, Color::BLUE};

  std::string_view key = "GREEN";
  std::cout << by_name.count(key) << std::endl;
  std::cout << by_name.contains(key) << std::endl;
  std::cout << (by_name.find(key) != by_name.end()) << std::endl;
  std::cout << (by_name.lower_bound(key) != by_name.end()) << std::endl;
  std::cout << (by_name.upper_bound(key) != by_name.end()) << std::endl;
  auto by_name_range = by_name.equal_range(key);
  std::cout << (by_name_range.first != by_name_range.second) << std::endl;

  auto cmp_key = by_name.key_comp();
  auto cmp_value = by_name.value_comp();
  std::cout << cmp_key(Color::BLUE, Color::RED) << std::endl;
  std::cout << cmp_value(Color::BLUE, Color::RED) << std::endl;

  magic_enum::containers::set<Color> lhs {Color::RED};
  magic_enum::containers::set<Color> rhs {Color::GREEN, Color::BLUE};
  std::cout << (lhs == rhs) << std::endl;
  std::cout << (lhs != rhs) << std::endl;
  std::cout << (lhs < rhs) << std::endl;
  std::cout << (lhs <= rhs) << std::endl;
  std::cout << (lhs > rhs) << std::endl;
  std::cout << (lhs >= rhs) << std::endl;

  lhs.swap(rhs);
  std::cout << lhs.size() << std::endl;
  lhs.clear();
  std::cout << lhs.empty() << std::endl;

  return 0;
}
