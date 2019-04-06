// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 Daniil Goncharov <neargye@gmail.com>.
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

#include <magic_enum.hpp>

#include <iostream>

enum Color { RED = -10, BLUE = 0, GREEN = 10 };

int main() {
  // Enum variable to string enum name.
  auto c1 = Color::RED;
  auto c1_name = magic_enum::enum_to_string(c1);
  if (c1_name.has_value()) {
    std::cout << c1_name.value() << std::endl; // RED
  }

  // String enum name to enum variable.
  auto c2 = magic_enum::enum_from_string<Color>("GREEN");
  if (c2.has_value() && c2.value() == Color::GREEN) {
    std::cout << "GREEN = " << c2.value() << std::endl; // GREEN = 10
  }

  // Static storage enum variable to string enum name.
  constexpr auto c3 = Color::BLUE;
  constexpr auto c3_name = magic_enum::enum_to_string<c3>();
  if (c3_name.has_value()) {
    std::cout << c3_name.value() << std::endl; // BLUE
  }

  constexpr auto colors = magic_enum::enum_sequence<Color>();
  std::cout << "Color sequence:";
  for (auto e : colors) {
    std::cout << " " << magic_enum::enum_to_string(e).value();
  }
  std::cout << std::endl;
  // Color sequence: RED BLUE GREEN

  constexpr auto color_names = magic_enum::enum_to_string_sequence<Color>();
  std::cout << "Color sequence:";
  for (auto e : color_names) {
    std::cout << " " << e;
  }
  std::cout << std::endl;
  // Color sequence: RED BLUE GREEN

  return 0;
}
