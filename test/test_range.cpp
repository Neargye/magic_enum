// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 0
#include <magic_enum/magic_enum.hpp>

enum class Single { value };

TEST_CASE("single-value reflection range") {
  static_assert(magic_enum::enum_count<Single>() == 1);
  static_assert(magic_enum::enum_value<Single, 0>() == Single::value);
}
