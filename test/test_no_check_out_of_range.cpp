// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include <magic_enum/magic_enum.hpp>

#if !defined(MAGIC_ENUM_NO_CHECK_OUT_OF_RANGE)
#  error "This test requires MAGIC_ENUM_NO_CHECK_OUT_OF_RANGE."
#endif

enum class NearRange { InRange = 0, Outside = 1 };
template <>
struct magic_enum::customize::enum_range<NearRange> {
  static constexpr int min = 0;
  static constexpr int max = 0;
};

enum class NearDefaultRange { InRange = 5, Outside = 129 };

using namespace magic_enum;

TEST_CASE("MAGIC_ENUM_NO_CHECK_OUT_OF_RANGE skips values just outside the range") {
  static_assert(enum_count<NearRange>() == 1);
  static_assert(!enum_contains(NearRange::Outside));
  static_assert(!enum_cast<NearRange>(1).has_value());
  CHECK(enum_name(NearRange::InRange) == "InRange");
  CHECK(enum_name(NearRange::Outside).empty());

  static_assert(enum_count<NearDefaultRange>() == 1);
  static_assert(!enum_contains(NearDefaultRange::Outside));
  CHECK(enum_name(NearDefaultRange::Outside).empty());
}
