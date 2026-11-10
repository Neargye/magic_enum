// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_bitwise.hpp>
#include <magic_enum/magic_enum_containers.hpp>
#include <magic_enum/magic_enum_flags.hpp>

#include <cstdint>

#if !defined(MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS)
#  error "This test requires MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS."
#endif

namespace no_zero {

enum class Flags : std::uint8_t { None = 0, A = 1, B = 2, All = 3 };
enum class AutoFlags : unsigned { None = 0, A = 1, B = 2 };
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(AutoFlags);
enum class SignedFlags : std::int8_t { High = -128, None = 0, Low = 1 };
enum class BoolFlags : bool { Off = false, On = true };
enum class OnlyZero : std::uint8_t { None = 0 };

} // namespace no_zero

template <>
struct magic_enum::customize::enum_range<no_zero::Flags> {
  static constexpr bool is_flags = true;
};
template <>
struct magic_enum::customize::enum_range<no_zero::SignedFlags> {
  static constexpr bool is_flags = true;
};
template <>
struct magic_enum::customize::enum_range<no_zero::BoolFlags> {
  static constexpr bool is_flags = true;
};
template <>
struct magic_enum::customize::enum_range<no_zero::OnlyZero> {
  static constexpr bool is_flags = true;
};

using namespace magic_enum;
using no_zero::AutoFlags;
using no_zero::BoolFlags;
using no_zero::Flags;
using no_zero::OnlyZero;
using no_zero::SignedFlags;

TEST_CASE("MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS leaves zero out of flag tables") {
  static_assert(is_flags_enum_v<Flags>);
  static_assert(enum_count<Flags>() == 2);
  static_assert(enum_values<Flags>()[0] == Flags::A);
  static_assert(enum_index(Flags::B) == std::size_t{1});
  static_assert(!enum_index(Flags::None).has_value());
  static_assert(!enum_contains(Flags::None));
  static_assert(enum_reflected(Flags::None));
  static_assert(!enum_flags_cast<Flags>(0).has_value());
  static_assert(!enum_flags_cast<Flags>("None").has_value());
  static_assert(enum_flags_cast<Flags>("A|B") == Flags::All);
  static_assert(enum_flags_contains(Flags::All));
  static_assert(!enum_flags_contains(Flags::None));
  CHECK(enum_name(Flags::None).empty());
  CHECK(enum_flags_name(Flags::None).empty());
  CHECK(enum_flags_name(Flags::All) == "A|B");

  static_assert(is_flags_enum_v<AutoFlags>);
  static_assert(enum_count<AutoFlags>() == 2);
  CHECK(enum_flags_name(AutoFlags::None).empty());
}

TEST_CASE("MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS keeps zero in common reflection") {
  static_assert(enum_count<Flags, as_common<>>() == 4);
  static_assert(enum_values<Flags, as_common<>>()[0] == Flags::None);
  static_assert(enum_cast<Flags, as_common<>>(0) == Flags::None);
  CHECK(enum_name<as_common<>>(Flags::None) == "None");
}

TEST_CASE("MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS with signed, bool and zero-only flags") {
  static_assert(enum_count<SignedFlags>() == 2);
  static_assert(enum_values<SignedFlags>()[0] == SignedFlags::High);
  static_assert(enum_values<SignedFlags>()[1] == SignedFlags::Low);
  CHECK(enum_flags_name(static_cast<SignedFlags>(-127)) == "High|Low");

  static_assert(enum_count<BoolFlags>() == 1);
  static_assert(enum_values<BoolFlags>()[0] == BoolFlags::On);
  static_assert(!enum_flags_cast<BoolFlags>("Off").has_value());

  static_assert(enum_count<OnlyZero>() == 0);
}

TEST_CASE("MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS in containers") {
  CHECK(containers::array<Flags, int>{}.size() == 2);

  const auto keys = containers::set<Flags>{Flags::A, Flags::B};
  CHECK(keys.size() == 2);
  CHECK(containers::set<Flags>::from_mask(Flags::None).empty());

  const auto bits = containers::bitset<Flags>{Flags::All};
  CHECK(bits.size() == 2);
  CHECK(bits.all());
  CHECK(containers::bitset<Flags>{}.to_string().empty());
  CHECK(bits.to_string() == "A|B");
}
