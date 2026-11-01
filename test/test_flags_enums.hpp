// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#pragma once

// Flag enums shared by test_flags.cpp and the topic suites.
#include <magic_enum/magic_enum.hpp>

#include <cstdint>

namespace flags_test {

enum class Color { RED = 1, GREEN = 2, BLUE = 4 };

enum class BoolFlags : bool { Disabled = false, Enabled = true };

enum class BoolEnabledFlag : bool { Enabled = true };

namespace Namespace {
    enum class Numbers : int {
        none = 0,
        one = 1 << 1,
        two = 1 << 2,
        three = 1 << 3,
        many = 1 << 30,
    };
    inline auto magic_enum_define_range_adl(Numbers)
    {
      return magic_enum::customize::adl_info().flag<true>();
    }
}
using Namespace::Numbers;

enum Directions : std::uint64_t {
  NoDirection = 0,
  Left = std::uint64_t{1} << 10,
  Down = std::uint64_t{1} << 20,
  Up = std::uint64_t{1} << 31,
  Right = std::uint64_t{1} << 63,
};

enum number : unsigned long {
  no_number = 0,
  one = 1 << 1,
  two = 1 << 2,
  three = 1 << 3,
  four = 1 << 4,
  _1 = one,
  _2 = two,
  _3 = three,
  _4 = four
};

enum CStyleFlags {
    CStyleFlags_A = 1 << 0,
    CStyleFlags_B = 1 << 1,
    CStyleFlags_C = 1 << 2,
    CStyleFlags_D = 1 << 3,
    CStyleFlags_E = 1 << 4,
    CStyleFlags_F = 1 << 5,
    CStyleFlags_G = 1 << 6,
    CStyleFlags_H = 1 << 7,
    CStyleFlags_I = 1 << 8,
};

} // namespace flags_test

template <>
struct magic_enum::customize::enum_range<flags_test::Color> {
  static constexpr bool is_flags = true;
};

template <>
struct magic_enum::customize::enum_range<flags_test::BoolFlags> {
  static constexpr bool is_flags = true;
};

template <>
struct magic_enum::customize::enum_range<flags_test::BoolEnabledFlag> {
  static constexpr bool is_flags = true;
};

template <>
struct magic_enum::customize::enum_range<flags_test::Directions> {
  static constexpr bool is_flags = true;
};

template <>
struct magic_enum::customize::enum_range<flags_test::number> {
  static constexpr bool is_flags = true;
};

template <>
struct magic_enum::customize::enum_range<flags_test::CStyleFlags> {
    static constexpr bool is_flags = true;
    static constexpr auto prefix_length = sizeof("CStyleFlags_")-1;
};
