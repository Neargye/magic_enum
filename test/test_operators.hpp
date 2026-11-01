// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#ifndef MAGIC_ENUM_TEST_OPERATORS_HPP
#define MAGIC_ENUM_TEST_OPERATORS_HPP

#include <magic_enum/magic_enum_bitwise.hpp>
#include <magic_enum/magic_enum_iostream.hpp>

#include <string>

namespace operators_test {

enum class Bits : unsigned { A = 1, B = 2, C = 4 };

// Included by two TUs, so the macro definitions must be inline.
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(Bits);
MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS(Bits);

Bits combine_in_other_tu(Bits lhs, Bits rhs);
std::string print_in_other_tu(Bits value);

} // namespace operators_test

template <>
struct magic_enum::customize::enum_range<operators_test::Bits> {
  static constexpr bool is_flags = true;
};

#endif // MAGIC_ENUM_TEST_OPERATORS_HPP
