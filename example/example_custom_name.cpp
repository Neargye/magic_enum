// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2020 Daniil Goncharov <neargye@gmail.com>.
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

enum class Color : int { RED = -10, BLUE = 0, GREEN = 10 };

// Сustom definitions of names for enum.
// Specialization of `enum_name` must be injected in `namespace magic_enum::customize`.
template <>
constexpr std::string_view magic_enum::customize::enum_name<Color>(Color value) noexcept {
  switch (value) {
    case Color::RED:
      return std::string_view{"the red color"};
    case Color::BLUE:
      return std::string_view{"The BLUE"};
    default:
      return std::string_view{}; // "Empty string for default or unknow value."
  }
}

int main() {
  std::cout << magic_enum::enum_name(Color::RED) << std::endl; // "the red color"
  std::cout << magic_enum::enum_name(Color::BLUE) << std::endl; // "The BLUE"
  std::cout << magic_enum::enum_name(Color::GREEN) << std::endl; // "GREEN"

  std::cout << std::boolalpha;
  std::cout << (magic_enum::enum_cast<Color>("the red color").value() == Color::RED) << std::endl; // "true"

  return 0;
}
