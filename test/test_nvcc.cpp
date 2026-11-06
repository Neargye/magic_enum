// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_containers.hpp>
#include <magic_enum/magic_enum_flags.hpp>
#include <magic_enum/magic_enum_fuse.hpp>
#include <magic_enum/magic_enum_switch.hpp>
#include <magic_enum/magic_enum_utility.hpp>

#include <cstdint>
#include <limits>
#include <string_view>
#include <type_traits>

static_assert(magic_enum::is_magic_enum_supported);

namespace nvcc {

template <int>
struct TemplateScope {
  enum class Value { One = 1, Four = 4 };
  enum class Collision { magic_enum_detail_enum_definition = 1 };
};

TEST_CASE("nvcc template enums before enumerator access") {
  // Each first lookup uses a fresh enum.
  static_assert(magic_enum::enum_count<TemplateScope<0>::Value>() == 2);
  static_assert(magic_enum::enum_name<static_cast<TemplateScope<1>::Value>(4)>() == "Four");
  static_assert(magic_enum::enum_name(static_cast<TemplateScope<2>::Value>(1)) == "One");
  static_assert(magic_enum::enum_values<TemplateScope<3>::Value>().size() == 2);
  static_assert(magic_enum::enum_names<TemplateScope<4>::Value>()[1] == "Four");
  static_assert(magic_enum::enum_entries<TemplateScope<5>::Value>()[0].second == "One");
  static_assert(magic_enum::enum_cast<TemplateScope<6>::Value>("Four") == static_cast<TemplateScope<6>::Value>(4));
  static_assert(magic_enum::enum_name(static_cast<TemplateScope<7>::Collision>(1)) == "magic_enum_detail_enum_definition");
  REQUIRE(magic_enum::enum_flags_name(static_cast<TemplateScope<8>::Value>(5)) == "One|Four");
}

template <typename T>
struct identity {
  using type = T;
};

// nvcc reuses the spelling of the first instantiation for a value, so each enum is first used through an alias.
enum class CountFirst { Negative = -7, One = 1, Four = 4 };
using CountFirstAlias = CountFirst;
static_assert(magic_enum::enum_count<const CountFirstAlias&>() == 3);
static_assert(magic_enum::enum_name(CountFirst::Negative) == "Negative");
static_assert(magic_enum::enum_name(static_cast<CountFirst>(2)).empty());
static_assert(magic_enum::enum_cast<CountFirst>("Four") == CountFirst::Four);
static_assert(magic_enum::enum_index(CountFirst::One) == 1);

enum class NameFirst { One = 1, Four = 4 };
static_assert(magic_enum::enum_name<identity<NameFirst>::type::Four>() == "Four");
static_assert(magic_enum::enum_type_name<identity<NameFirst>::type>() == "NameFirst");
static_assert(magic_enum::enum_count<NameFirst>() == 2);

enum Plain { PlainFirst = 1, PlainLast = 4 };
static_assert(magic_enum::enum_count<std::decay_t<Plain>>() == 2);
static_assert(magic_enum::enum_name(PlainLast) == "PlainLast");

enum class Narrow : bool { Off, On };
static_assert(magic_enum::enum_count<Narrow>() == 2);
static_assert(magic_enum::enum_name(Narrow::On) == "On");

enum class Small : std::int8_t { Min = -128, Zero = 0, Max = 127 };
static_assert(magic_enum::enum_count<Small>() == 3);
static_assert(magic_enum::enum_cast<Small>(-128) == Small::Min);
static_assert(!magic_enum::enum_cast<Small>(128).has_value());

struct Holder {
  enum class Member { A, B };
};
static_assert(magic_enum::enum_count<Holder::Member>() == 2);
static_assert(magic_enum::enum_name(Holder::Member::B) == "B");

enum class Ranged { First = 300, Last = 400 };

enum class Renamed { A, B };

enum class Bits : std::uint64_t { Low = 1, Mid = 1 << 4, High = std::uint64_t{1} << 63 };

enum class SignedBits : std::int8_t { Low = 1, High = -128 };

namespace a_namespace_with_a_long_name_so_that_every_scanned_signature_is_long_enough {
enum class LongScan { First = -128, Middle = 0, Last = 127 };
} // namespace a_namespace_with_a_long_name_so_that_every_scanned_signature_is_long_enough

} // namespace nvcc

template <>
struct magic_enum::customize::enum_range<nvcc::Ranged> {
  static constexpr int min = 300;
  static constexpr int max = 400;
};

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<nvcc::Renamed>(nvcc::Renamed value) noexcept {
  return value == nvcc::Renamed::B ? customize_t{"Bee"} : default_tag;
}

template <>
struct magic_enum::customize::enum_range<nvcc::Bits> {
  static constexpr bool is_flags = true;
};

template <>
struct magic_enum::customize::enum_range<nvcc::SignedBits> {
  static constexpr bool is_flags = true;
};

using namespace nvcc;

TEST_CASE("nvcc values, names and entries") {
  constexpr auto values = magic_enum::enum_values<CountFirst>();
  static_assert(values[0] == CountFirst::Negative && values[2] == CountFirst::Four);
  constexpr auto entries = magic_enum::enum_entries<CountFirst>();
  REQUIRE(entries[1].second == "One");
  REQUIRE(magic_enum::enum_name(CountFirst::Four) == "Four");
  REQUIRE(magic_enum::enum_type_name<CountFirst>() == "CountFirst");
  REQUIRE(magic_enum::enum_cast<CountFirst>("four", magic_enum::case_insensitive) == CountFirst::Four);
}

TEST_CASE("nvcc custom range and names") {
  static_assert(magic_enum::enum_count<Ranged>() == 2);
  static_assert(magic_enum::enum_name(Ranged::Last) == "Last");
  static_assert(magic_enum::enum_name(Renamed::A) == "A");
  static_assert(magic_enum::enum_name(Renamed::B) == "Bee");
  static_assert(magic_enum::enum_cast<Renamed>("Bee") == Renamed::B);
}

TEST_CASE("nvcc long signatures") {
  using a_namespace_with_a_long_name_so_that_every_scanned_signature_is_long_enough::LongScan;
  static_assert(magic_enum::enum_count<LongScan>() == 3);
  static_assert(magic_enum::enum_name(LongScan::Last) == "Last");
}

TEST_CASE("nvcc flags") {
  static_assert(magic_enum::enum_count<Bits>() == 3);
  static_assert(magic_enum::enum_name(Bits::High) == "High");
  REQUIRE(magic_enum::enum_flags_name(static_cast<Bits>(17)) == "Low|Mid");
  REQUIRE(magic_enum::enum_flags_cast<Bits>("Mid|High") == static_cast<Bits>((std::uint64_t{1} << 63) | 16));
  static_assert(!magic_enum::enum_flags_contains<Bits>(2));

  static_assert(magic_enum::enum_count<SignedBits>() == 2);
  REQUIRE(magic_enum::enum_flags_name(static_cast<SignedBits>(-127)) == "High|Low");
}

TEST_CASE("nvcc switch, fuse, utility and containers") {
  constexpr auto twice = [](auto value) { return static_cast<int>(value()) * 2; };
  static_assert(magic_enum::enum_switch(twice, CountFirst::Four, 0) == 8);
  static_assert(magic_enum::enum_switch(twice, static_cast<CountFirst>(2), -1) == -1);

  static_assert(magic_enum::enum_fuse(CountFirst::One, Narrow::On).has_value());

  int sum = 0;
  magic_enum::enum_for_each<CountFirst>([&sum](auto value) { sum += static_cast<int>(value()); });
  REQUIRE(sum == -2);

  magic_enum::containers::array<Holder::Member, int> counts{{1, 2}};
  REQUIRE(counts[Holder::Member::B] == 2);
  magic_enum::containers::bitset<Holder::Member> bits{Holder::Member::A};
  REQUIRE(bits.to_string() == "A");
}
