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

#include <iostream>
#include <string>

#include <magic_enum.hpp>

enum class AnimalFlags : std::uint64_t { HasClaws = 1 << 10, CanFly = 1 << 20, EatsFish = 1 << 30, Endangered = std::uint64_t{1} << 40 };
// Add specialization `is_flags` to force define that enum are flags.
//template <>
//struct magic_enum::customize::enum_range<AnimalFlags> {
//  static constexpr bool is_flags = true;
//};

int main() {
  // Enum-flags variable to string name.
  AnimalFlags f1 = AnimalFlags::Endangered;
  auto f1_name = magic_enum::enum_name(f1);
  std::cout << f1_name << std::endl; // Endangered

  // String enum-flags name sequence.
  constexpr auto names = magic_enum::enum_names<AnimalFlags>();
  std::cout << "AnimalFlags names:";
  for (const auto& n : names) {
    std::cout << " " << n;
  }
  std::cout << std::endl;
  // AnimalFlags names: HasClaws CanFly EatsFish Endangered

  // String name to enum-flags value.
  auto f2 = magic_enum::enum_cast<AnimalFlags>("EatsFish|CanFly");
  if (f2.has_value()) {
    std::cout << "EatsFish|CanFly = " << magic_enum::enum_integer(f2.value()) << std::endl; // CanFly|EatsFish = 1074790400
  }

  // Integer value to enum-flags value.
  auto f3 = magic_enum::enum_cast<AnimalFlags>(1073742848);
  if (f3.has_value()) {
    std::cout << magic_enum::enum_flags_name(f3.value()) << " = " << magic_enum::enum_integer(f3.value()) << std::endl; // HasClaws|EatsFish = 1073742848
  }

  // Enum-flags value to integer value.
  auto f4_integer = magic_enum::enum_integer(AnimalFlags::HasClaws);
  std::cout << "HasClaws = " << f4_integer << std::endl; // HasClaws = 1024

  using namespace magic_enum::ostream_operators; // out-of-the-box ostream operator for enum-flags.
  // Ostream operator for enum-flags.
  std::cout << f1 << " " << f2 << " " << f3 << std::endl; // Endangered CanFly|EatsFish HasClaws|EatsFish

  // Number of enum-flags values.
  std::cout << "AnimalFlags enum size: " << magic_enum::enum_count<AnimalFlags>() << std::endl; // AnimalFlags enum size: 4

  // Indexed access to enum-flags value.
  std::cout << "AnimalFlags[0] = " << magic_enum::enum_value<AnimalFlags>(0) << std::endl; // AnimalFlags[0] = HasClaws

  // Enum-flags value sequence.
  constexpr auto values = magic_enum::enum_values<AnimalFlags>();
  std::cout << "AnimalFlags values:";
  for (const auto f : values) {
    std::cout << " " << f; // Ostream operator for enum-flags.
  }
  std::cout << std::endl;
  // AnimalFlags sequence: HasClaws CanFly EatsFish Endangered

  using namespace magic_enum::bitwise_operators; // out-of-the-box bitwise operators for all enums.
  // Support operators: ~, |, &, ^, |=, &=, ^=.
  AnimalFlags flag = AnimalFlags::HasClaws | AnimalFlags::CanFly;
  std::cout << flag << std::endl; // HasClaws|CanFly

  // Enum-flags pair (value, string name) sequence.
  constexpr auto entries = magic_enum::enum_entries<AnimalFlags>();
  std::cout << "AnimalFlags entries:";
  for (const auto& e : entries) {
    std::cout << " "  << e.second << " = " << magic_enum::enum_integer(e.first);
  }
  std::cout << std::endl;
  // AnimalFlags entries: AnimalFlags entries: HasClaws = 1024 CanFly = 1048576 EatsFish = 1073741824 Endangered = 1099511627776

  return 0;
}
