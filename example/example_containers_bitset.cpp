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

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4244) // warning C4244: 'argument': conversion from 'const T' to 'unsigned int', possible loss of data.
#endif

#include <iostream>

#include <magic_enum_containers.hpp>

enum class Color { RED = 1, GREEN = 2, BLUE = 4 };

int main() {

  auto color_bitset = magic_enum::containers::bitset<Color>();
  color_bitset.set(Color::GREEN);
  color_bitset.set(Color::BLUE);

  std::cout << std::boolalpha;
  std::cout << color_bitset.size() << std::endl; // 3 == magic_enum::enum_count<Color>()
  std::cout << color_bitset.all() << std::endl; // false
  std::cout << color_bitset.any() << std::endl; // true
  std::cout << color_bitset.none() << std::endl; // false
  std::cout << color_bitset.count() << std::endl; // 2
  std::cout << color_bitset.test(Color::RED) << std::endl; // false
  std::cout << color_bitset.test(Color::GREEN) << std::endl; // true
  std::cout << color_bitset.test(Color::BLUE) << std::endl; // true

  return 0;
}
