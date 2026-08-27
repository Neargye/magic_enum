// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <magic_enum/magic_enum_all.hpp>

#include <array>
#include <cstdint>
#include <limits>

enum class Dense { A = -2, B = -1, C = 0, D = 1, E = 2 };
enum class Empty {};
enum class Sparse : long long { FarLow = -1'000'000, InRange = 17, FarHigh = 900'000'000 };
enum class Unordered { C = 30, A = 10, B = 20 };
enum class Aliased { First = 1, AliasFirst = 1, Second = 2, AliasSecond = 2 };
enum class AliasBeforeCanonical { Alias = 1, Canonical = 1 };
enum class Gapped { A = 1, B = 3 };
enum Unfixed { UnfixedZero = 0, UnfixedOne = 1 };
enum class Customized { Declared = 1 };
enum class InvalidCustomized { Keep = 1, Drop = 2 };
enum class Prefixed { Prefix_A = 1, Prefix_B = 2 };
enum class PrefixedAlias { Prefix_A = 1, B = 1 };
enum class HugeRange { Low = -1'000'000, High = 1'000'000 };
enum class WideFlags : std::uint32_t { None = 0, A = 1u, B = 2u, AB = 3u, High = 1u << 31 };
enum class Extreme : long long {
  Min = (std::numeric_limits<long long>::min)(),
  Max = (std::numeric_limits<long long>::max)(),
};

namespace hostile_enum_comparison {

enum class Aliased { First = 1, AliasFirst = 1, Second = 4 };

constexpr bool operator==(Aliased, Aliased) = delete;
constexpr bool operator!=(Aliased, Aliased) = delete;

} // namespace hostile_enum_comparison

inline constexpr char customized_declared_name[] = "RenamedTail";

template <typename>
struct Holder {
  enum class Nested { B = 2, A = 1 };
};

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<Customized>(Customized value) noexcept {
  if (value == Customized::Declared) {
    return magic_enum::string_view{customized_declared_name, sizeof("Renamed") - 1};
  }
  if (static_cast<int>(value) == 2) {
    return "Synthetic";
  }
  return default_tag;
}

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<InvalidCustomized>(InvalidCustomized value) noexcept {
  return value == InvalidCustomized::Drop ? invalid_tag : default_tag;
}

template <>
struct magic_enum::customize::enum_range<Prefixed> {
  static constexpr int min = 0;
  static constexpr int max = 3;
  static constexpr std::size_t prefix_length = sizeof("Prefix_") - 1;
};

template <>
struct magic_enum::customize::enum_range<PrefixedAlias> {
  static constexpr std::size_t prefix_length = sizeof("Prefix_") - 1;
};

template <>
struct magic_enum::customize::enum_range<WideFlags> {
  static constexpr bool is_flags = true;
};

template <>
struct magic_enum::customize::enum_range<HugeRange> {
  static constexpr int min = -1;
  static constexpr int max = 1;
};

static_assert(magic_enum::detail::is_reflected<Dense, magic_enum::detail::enum_subtype::common>::value);
static_assert(magic_enum::detail::is_reflected<const Dense&, magic_enum::detail::enum_subtype::common>::value);
static_assert(magic_enum::detail::is_reflected_v<volatile Dense, magic_enum::detail::enum_subtype::common>);
static_assert(!magic_enum::detail::is_reflected_v<Empty, magic_enum::detail::enum_subtype::common>);
static_assert(!magic_enum::detail::is_reflected_v<int, magic_enum::detail::enum_subtype::common>);

TEST_CASE("canonical common-enum behavior") {
  constexpr auto dense_values = magic_enum::enum_values<Dense>();
  static_assert(dense_values == std::array{Dense::A, Dense::B, Dense::C, Dense::D, Dense::E});

  constexpr auto unordered_values = magic_enum::enum_values<Unordered>();
  static_assert(unordered_values == std::array{Unordered::A, Unordered::B, Unordered::C});
  static_assert(magic_enum::enum_names<Unordered>() == std::array<magic_enum::string_view, 3>{"A", "B", "C"});
  static_assert(magic_enum::enum_entries<Unordered>()[1].first == Unordered::B);
  static_assert(magic_enum::enum_entries<Unordered>()[1].second == "B");

  static_assert(magic_enum::enum_count<Gapped>() == 2);
  static_assert(!magic_enum::enum_contains(Gapped{2}));
  static_assert(!magic_enum::enum_cast<Gapped>(2).has_value());
  static_assert(magic_enum::enum_index(Gapped::B).value() == 1);
  static_assert(!magic_enum::enum_reflected<Empty>(0));
  using UnfixedUnderlying = magic_enum::underlying_type_t<Unfixed>;
  static_assert(!magic_enum::enum_reflected<Unfixed>((std::numeric_limits<UnfixedUnderlying>::max)()));

#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  static_assert(magic_enum::enum_values<Sparse>() == std::array{Sparse::FarLow, Sparse::InRange, Sparse::FarHigh});
  static_assert(magic_enum::enum_name(Sparse::FarLow) == "FarLow");
  static_assert(magic_enum::enum_values<Extreme>() == std::array{Extreme::Min, Extreme::Max});
  static_assert(magic_enum::enum_index(Extreme::Min) == 0);
  static_assert(magic_enum::enum_index(Extreme::Max) == 1);
  static_assert(magic_enum::enum_value<Extreme, 0>() == Extreme::Min);
  static_assert(magic_enum::enum_cast<Extreme>((std::numeric_limits<long long>::min)()) == Extreme::Min);
#else
  static_assert(magic_enum::enum_values<Sparse>() == std::array{Sparse::InRange});
  static_assert(magic_enum::enum_name(Sparse::FarLow).empty());
#endif
  static_assert(magic_enum::enum_name<Sparse::FarLow>() == "FarLow");
  static_assert(magic_enum::enum_name<Sparse::FarHigh>() == "FarHigh");
  static_assert(magic_enum::enum_name<Extreme::Min>() == "Min");
  static_assert(magic_enum::enum_name<Extreme::Max>() == "Max");

  using Nested = Holder<void>::Nested;
  static_assert(magic_enum::enum_values<Nested>() == std::array{Nested::A, Nested::B});
  static_assert(magic_enum::enum_type_name<Nested>() == "Nested");
}

TEST_CASE("aliases remain one-name-per-value") {
  static_assert(magic_enum::enum_count<Aliased>() == 2);
  static_assert(magic_enum::enum_name(Aliased::First) == "First");
  static_assert(magic_enum::enum_name(Aliased::AliasFirst) == "First");
  static_assert(magic_enum::enum_cast<Aliased>("AliasFirst") == magic_enum::optional<Aliased>{});
  static_assert(magic_enum::enum_name(AliasBeforeCanonical::Canonical) == "Alias");

#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  constexpr auto enumerators = magic_enum::detail::reflection::enumerators_v<Aliased>;
  static_assert(enumerators.size() == 4);
  static_assert(std::meta::identifier_of(enumerators[0]) == "First");
  static_assert(std::meta::identifier_of(enumerators[1]) == "AliasFirst");
  static_assert(std::meta::identifier_of(enumerators[2]) == "Second");
  static_assert(std::meta::identifier_of(enumerators[3]) == "AliasSecond");

  using HostileAliased = hostile_enum_comparison::Aliased;
  static_assert(magic_enum::enum_count<HostileAliased>() == 2);
  static_assert(magic_enum::enum_name<HostileAliased::First>() == "First");
  static_assert(magic_enum::enum_name(HostileAliased::AliasFirst) == "First");
  static_assert(magic_enum::enum_index(HostileAliased::Second).value() == 1);
  static_assert(magic_enum::enum_reflected(HostileAliased::Second));
#endif
}

TEST_CASE("customization and range semantics") {
  static_assert(magic_enum::enum_name(Customized::Declared) == "Renamed");
  static_assert(magic_enum::enum_name(Customized::Declared).data()[magic_enum::enum_name(Customized::Declared).size()] == '\0');
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  static_assert(magic_enum::enum_values<Customized>() == std::array{Customized::Declared});
  static_assert(!magic_enum::detail::reflection::contains(Customized{2}));
  static_assert(magic_enum::enum_name(Customized{2}).empty());
  static_assert(!magic_enum::enum_cast<Customized>("Synthetic").has_value());
  static_assert(magic_enum::enum_values<HugeRange>() == std::array{HugeRange::Low, HugeRange::High});
  static_assert(magic_enum::enum_reflected(HugeRange::Low));
  static_assert(magic_enum::enum_reflected(HugeRange::High));
  static_assert(!magic_enum::enum_reflected<HugeRange>(0));
#else
  static_assert(magic_enum::enum_values<Customized>() == std::array{Customized::Declared, Customized{2}});
  static_assert(magic_enum::enum_name(Customized{2}) == "Synthetic");
  static_assert(magic_enum::enum_name<Customized{2}>() == "Synthetic");
  static_assert(magic_enum::enum_cast<Customized>("Synthetic") == Customized{2});
  static_assert(!magic_enum::enum_reflected(HugeRange::Low));
  static_assert(!magic_enum::enum_reflected(HugeRange::High));
  static_assert(!magic_enum::enum_reflected<HugeRange>(0));
#endif

  static_assert(magic_enum::enum_values<InvalidCustomized>() == std::array{InvalidCustomized::Keep});
  static_assert(magic_enum::enum_name(InvalidCustomized::Drop).empty());
  static_assert(magic_enum::enum_reflected(InvalidCustomized::Drop));

  static_assert(magic_enum::enum_names<Prefixed>() == std::array<magic_enum::string_view, 2>{"A", "B"});
  static_assert(magic_enum::enum_cast<Prefixed>("A") == Prefixed::Prefix_A);
  static_assert(magic_enum::enum_values<PrefixedAlias>() == std::array{PrefixedAlias::Prefix_A});
  static_assert(magic_enum::enum_name(PrefixedAlias::B) == "A");
}

TEST_CASE("flags and dependent APIs keep canonical indexing") {
  constexpr auto flag_values = magic_enum::enum_values<WideFlags>();
  static_assert(flag_values == std::array{WideFlags::A, WideFlags::B, WideFlags::High});
  CHECK(magic_enum::enum_flags_name(WideFlags::AB) == "A|B");
  static_assert(magic_enum::enum_flags_cast<WideFlags>("A|B") == WideFlags::AB);
  static_assert(!magic_enum::enum_flags_cast<WideFlags>(std::uint32_t{4}).has_value());
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  static_assert(!magic_enum::enum_reflected<WideFlags>(std::uint32_t{4}));
#else
  static_assert(magic_enum::enum_reflected<WideFlags>(std::uint32_t{4}));
#endif

  constexpr auto visited = magic_enum::enum_for_each<Unordered>([](auto value) {
    return magic_enum::enum_integer(value());
  });
  static_assert(visited == std::array{10, 20, 30});

  static_assert(magic_enum::enum_switch<int>([](auto value) {
    return magic_enum::enum_integer(value());
  }, Unordered::B) == 20);

  static_assert(magic_enum::enum_fuse(Dense::A, Unordered::B).has_value());

  constexpr magic_enum::containers::array<Unordered, int> indexed{{1, 2, 3}};
  static_assert(indexed[Unordered::A] == 1);
  static_assert(indexed[Unordered::C] == 3);
}
