// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

// Included first to check that the header is self-contained.
#include <magic_enum/magic_enum_bitwise.hpp>

#if defined(NEARGYE_MAGIC_ENUM_HPP)
#  error magic_enum_bitwise.hpp must not include magic_enum.hpp.
#endif

namespace standalone {

enum class Mask : unsigned { A = 1, B = 2 };
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(Mask);

static_assert((Mask::A | Mask::B) == static_cast<Mask>(3));

} // namespace standalone

using namespace magic_enum::bitwise_operators;

#define MAGIC_ENUM_TEST_ADL_BEFORE_INCLUDE
#include "test_adl.hpp"
#include "test_operators.hpp"

bool adl_lookup_in_other_tu() {
  return !magic_enum::is_flags_v<adl_lookup_test::Wrong> && magic_enum::is_flags_v<adl_lookup_test::Right>;
}

#include <sstream>

operators_test::Bits operators_test::combine_in_other_tu(Bits lhs, Bits rhs) {
  return lhs | rhs;
}

std::string operators_test::print_in_other_tu(Bits value) {
  std::ostringstream stream;
  stream << value;
  return stream.str();
}
