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

#include <iostream>

#include <magic_enum.hpp>

enum Color { RED = -10, BLUE = 0, GREEN = 10 };

int main() {
  // Enum variable to string name.
  Color c1 = Color::RED;
  auto c1_name = yae::enum_name(c1);
  if (c1_name.has_value()) {
    std::cout << c1_name.value() << std::endl; // RED
  }

  // String enum name sequence.
  constexpr auto color_names = yae::enum_names<Color>();
  std::cout << "Color names:";
  for (auto n : color_names) {
    std::cout << " " << n;
  }
  std::cout << std::endl;
  // Color names: RED BLUE GREEN

  // String name to enum value.
  auto c2 = yae::enum_cast<Color>("BLUE");
  if (c2.has_value() && c2.value() == Color::BLUE) {
    std::cout << "BLUE = " << c2.value() << std::endl; // BLUE = 0
  }

  // Integer value to enum value.
  auto c3 = yae::enum_cast<Color>(10);
  if (c3.has_value() && c3.value() == Color::GREEN) {
    std::cout << "GREEN = " << c3.value() << std::endl; // GREEN = 10
  }

  using namespace yae::ops; // out-of-the-box stream operator for enums.
  // ostream operator for enum.
  std::cout << "Color: " << c1 << " " << c2 << " " << c3 << std::endl; // Color: RED BLUE GREEN

  // Number of enum values.
  std::cout << "Color enum size: " << yae::enum_count<Color>() << std::endl; // Color enum size: 3

  // Indexed access to enum value.
  std::cout << "Color[0] = " << yae::enum_value<Color>(0) << std::endl; // Color[0] = RED

  // Enum value sequence.
  constexpr auto colors = yae::enum_values<Color>();
  std::cout << "Colors sequence:";
  for (Color c : colors) {
    std::cout << " " << c; // ostream operator for enum.
  }
  std::cout << std::endl;
  // Color sequence: RED BLUE GREEN

  return 0;
}
