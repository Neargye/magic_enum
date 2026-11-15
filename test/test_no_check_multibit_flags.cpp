// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_bitwise.hpp>
#include <magic_enum/magic_enum_flags.hpp>

#if !defined(MAGIC_ENUM_NO_CHECK_MULTIBIT_FLAGS)
#  error "This test requires MAGIC_ENUM_NO_CHECK_MULTIBIT_FLAGS."
#endif

enum class Access : unsigned { Read = 1, Write = 2, ReadWrite = 3 };
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(Access);

using namespace magic_enum;

TEST_CASE("MAGIC_ENUM_NO_CHECK_MULTIBIT_FLAGS keeps automatic flags and drops multibit values") {
  static_assert(is_flags_enum_v<Access>);
  static_assert(enum_count<Access>() == 2);
  static_assert(!enum_contains(Access::ReadWrite));
  CHECK(enum_name(Access::ReadWrite).empty());
  CHECK(enum_flags_name(Access::ReadWrite) == "Read|Write");
}
