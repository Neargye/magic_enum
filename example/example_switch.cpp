// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2022 Daniil Goncharov <neargye@gmail.com>.
// Copyright (c) 2020 - 2022 Bela Schaum <schaumb@gmail.com>.
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
#define MAGIC_ENUM_ENABLE_HASH
#include <magic_enum_switch.hpp>

enum class Color { RED, BLUE, GREEN };

template <Color C>
constexpr std::string_view DoWork() {
  return "default";
}

template <>
constexpr std::string_view DoWork<Color::GREEN>() {
  return "override";
}

// Helper type for the visitor pattern.
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

int main() {
  Color c = Color::RED;

  auto lambda = [] (auto value) {
    std::cout << DoWork<value>() << std::endl;
  };

  magic_enum::enum_switch(lambda, c); // prints "default"

  c = Color::GREEN;

  magic_enum::enum_switch(lambda, c); // prints "override"

  // with object, explicit enum type
  auto switcher1 = overloaded{
    [] (magic_enum::enum_constant<Color::BLUE>) {
      std::cout << "Blue" << std::endl;
    },
    [] (magic_enum::enum_constant<Color::RED>) {
      std::cout << "Red" << std::endl;
    }
  };

  magic_enum::enum_switch(switcher1, Color::GREEN); // prints nothing
  magic_enum::enum_switch(switcher1, Color::BLUE); // prints "Blue"
  magic_enum::enum_switch(switcher1, Color::RED); // prints "Red"

  // explicit result type
  auto switcher2 = overloaded{
    [] (magic_enum::enum_constant<Color::GREEN>) {
      return "called with green argument";
    },
    [] (Color other) { // default case
      auto name = magic_enum::enum_name(other); // not empty
      return "default: " + std::string{name};
    }
  };

  std::cout << magic_enum::enum_switch<std::string>(switcher2, Color::GREEN) << std::endl; // prints "called with green argument"
  std::cout << magic_enum::enum_switch<std::string>(switcher2, Color::RED) << std::endl; // prints "default: RED"

  auto empty = magic_enum::enum_switch<std::string>(switcher2, static_cast<Color>(-3)); // returns an empty string
  assert(empty.empty());

  // result with default object
  std::cout << magic_enum::enum_switch<std::string>(switcher2, static_cast<Color>(-3), "unrecognized") << std::endl; // prints "unrecognized"

  auto switcher3 = overloaded{
    [] (magic_enum::enum_constant<Color::RED>) -> std::optional<std::string> {
      return "red result";
    },
    [] (magic_enum::enum_constant<Color::BLUE>) -> std::optional<std::string> {
      return std::nullopt;
    }
  };

  std::cout << std::boolalpha;
  std::cout << magic_enum::enum_switch(switcher3, Color::GREEN, std::make_optional("cica")).value() << std::endl; // prints default: "cica"
  std::cout << magic_enum::enum_switch(switcher3, Color::RED, std::make_optional("cica")).value() << std::endl; // prints: "red result"
  std::cout << magic_enum::enum_switch(switcher3, Color::BLUE, std::make_optional("cica")).has_value() << std::endl; // prints: false
}
