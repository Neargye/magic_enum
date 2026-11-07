// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#pragma once

// Enums shared by test.cpp and the topic suites. All TUs of one executable must use the same range configuration.
#if defined(NEARGYE_MAGIC_ENUM_HPP)
#  error test_enums.hpp must be included before magic_enum.hpp.
#endif

#include "magic_enum_test_config.hpp"
#include <magic_enum/magic_enum.hpp>

#include <cstdint>
#include <limits>

enum class Color { RED = -12, GREEN = 7, BLUE = 15 };

namespace hostile_enum_comparison {

enum class Value { First = 1, Second = 4 };

constexpr bool operator==(Value, Value) = delete;
constexpr bool operator!=(Value, Value) = delete;

} // namespace hostile_enum_comparison

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<Color>(Color value) noexcept {
  switch (value) {
    case Color::RED:
      return "red";
    default:
      return default_tag;
  }
}

enum class Numbers : int { one = 1, two, three, many = 127 };

enum class WithCount { First, Second, Count };
template <>
struct magic_enum::customize::enum_range<WithCount> {
  static constexpr int min = 0;
  static constexpr int max = static_cast<int>(WithCount::Count) - 1;
};
template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<WithCount>(WithCount value) noexcept {
  return value == WithCount::Count ? invalid_tag : default_tag;
}

enum Directions { Up = 85, Down = -42, Right = 120, Left = -120 };

enum number : unsigned long {
  one = 100,
  two = 200,
  three = 300,
  four = 400,
  _1 = one,
  _2 = two,
  _3 = three,
  _4 = four
};
template <>
struct magic_enum::customize::enum_range<number> {
  static constexpr int min = 100;
  static constexpr int max = 300;
};

enum class MaxUsedAsInvalid : std::uint8_t {
  ONE,
  TWO = 63,
  INVALID = std::numeric_limits<std::uint8_t>::max()
};
template <>
struct magic_enum::customize::enum_range<MaxUsedAsInvalid> {
  static constexpr int min = 0;
  static constexpr int max = 64;
};

enum class Binary : bool {
  ONE,
  TWO
};
template <>
struct magic_enum::customize::enum_range<Binary> {
  static constexpr int min = 0;
  static constexpr int max = 64;
};
