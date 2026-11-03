// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include <magic_enum/magic_enum_all.hpp>

#include <array>
#include <cstdint>
#include <limits>

enum class Dense { A = -2, B = -1, C = 0, D = 1, E = 2 };
enum class Empty {};
enum class Deprecated { Old [[deprecated]] = 1, Current = 2 };
enum class Opaque : std::uint8_t;
enum class OpaqueFlags : std::int8_t;
enum class NoNameScan { A = 1 };
enum class Sparse : long long { FarLow = -1'000'000, InRange = 17, FarHigh = 900'000'000 };
enum class Unordered { C = 30, A = 10, B = 20 };
enum class Aliased { First = 1, AliasFirst = 1, Second = 2, AliasSecond = 2 };
enum class AliasBeforeCanonical { Alias = 1, Canonical = 1 };
enum class UnorderedAliases { High = 30, First = 10, Middle = 20, Alias = 10 };
enum class FilteredAliases { High = 30, Hidden = 5, First = 10, Renamed = 20, Alias = 10 };
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
struct magic_enum::customize::enum_range<OpaqueFlags> {
  static constexpr bool is_flags = true;
};

// Non-constexpr customization makes accidental name scanning fail to compile.
template <>
magic_enum::customize::customize_t magic_enum::customize::enum_name<NoNameScan>(NoNameScan) noexcept {
  return invalid_tag;
}

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
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<FilteredAliases>(FilteredAliases value) noexcept {
  if (value == FilteredAliases::Hidden) {
    return invalid_tag;
  }
  if (value == FilteredAliases::Renamed) {
    return "Custom";
  }
  return default_tag;
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

TEST_CASE("reflection domain does not require enum names") {
  static_assert(magic_enum::enum_reflected<Opaque>(0));
  static_assert(magic_enum::enum_reflected<Opaque>(1));
  static_assert(!magic_enum::enum_reflected<Opaque>(256));
  static_assert(!magic_enum::enum_reflected<Opaque>(-1));
  static_assert(magic_enum::enum_reflected<OpaqueFlags>(0));
  static_assert(magic_enum::enum_reflected<OpaqueFlags>(-128));
  static_assert(magic_enum::enum_reflected<OpaqueFlags>(64));
  static_assert(!magic_enum::enum_reflected<OpaqueFlags>(-127));
  static_assert(!magic_enum::enum_reflected<OpaqueFlags>(128));
  static_assert(magic_enum::enum_reflected<NoNameScan>(0));
  static_assert(magic_enum::enum_reflected<NoNameScan>(1));
  static_assert(magic_enum::enum_reflected<NoNameScan, magic_enum::as_flags<>>(0));
  static_assert(magic_enum::enum_reflected<NoNameScan, magic_enum::as_flags<>>(2));
  static_assert(!magic_enum::enum_reflected<NoNameScan, magic_enum::as_flags<>>(3));
}

#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
template <const char* Name>
struct ReflectedName {
  static constexpr auto value = Name;
};

static_assert(ReflectedName<magic_enum::enum_name(Dense::A).data()>::value[0] == 'A');
#endif

TEST_CASE("deprecated enumerators remain reflected") {
  static_assert(magic_enum::enum_values<Deprecated>() == std::array{Deprecated{1}, Deprecated::Current});
  static_assert(magic_enum::enum_names<Deprecated>() == std::array<magic_enum::string_view, 2>{"Old", "Current"});
  static_assert(magic_enum::enum_cast<Deprecated>("Old") == Deprecated{1});
}

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
  static_assert(magic_enum::enum_reflected<Empty>(0));
  using UnfixedUnderlying = magic_enum::underlying_type_t<Unfixed>;

#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  static_assert(magic_enum::enum_values<UnorderedAliases>() == std::array{UnorderedAliases::First, UnorderedAliases::Middle, UnorderedAliases::High});
  static_assert(magic_enum::enum_names<UnorderedAliases>() == std::array<magic_enum::string_view, 3>{"First", "Middle", "High"});
  static_assert(magic_enum::enum_reflected<Unfixed>((std::numeric_limits<UnfixedUnderlying>::max)()));
  static_assert(magic_enum::enum_values<Sparse>() == std::array{Sparse::FarLow, Sparse::InRange, Sparse::FarHigh});
  static_assert(magic_enum::enum_name(Sparse::FarLow) == "FarLow");
  static_assert(magic_enum::enum_values<Extreme>() == std::array{Extreme::Min, Extreme::Max});
  static_assert(magic_enum::enum_index(Extreme::Min) == 0);
  static_assert(magic_enum::enum_index(Extreme::Max) == 1);
  static_assert(magic_enum::enum_value<Extreme, 0>() == Extreme::Min);
  static_assert(magic_enum::enum_cast<Extreme>((std::numeric_limits<long long>::min)()) == Extreme::Min);
  static_assert(magic_enum::enum_cast<Extreme>((std::numeric_limits<long long>::max)()) == Extreme::Max);
  static_assert(!magic_enum::enum_cast<Extreme>((std::numeric_limits<long long>::min)() + 1).has_value());
  static_assert(!magic_enum::enum_contains(static_cast<Extreme>(0)));
  static_assert(magic_enum::enum_switch([](auto v) { return magic_enum::enum_integer(v()); }, Extreme::Max, 0LL) == (std::numeric_limits<long long>::max)());
#else
  static_assert(!magic_enum::enum_reflected<Unfixed>((std::numeric_limits<UnfixedUnderlying>::max)()));
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

TEST_CASE("unordered aliases with hidden and customized names") {
  static_assert(magic_enum::enum_count<FilteredAliases>() == 3);
  static_assert(magic_enum::enum_value<FilteredAliases, 0>() == FilteredAliases::First);
  static_assert(magic_enum::enum_value<FilteredAliases, 1>() == FilteredAliases::Renamed);
  static_assert(magic_enum::enum_value<FilteredAliases, 2>() == FilteredAliases::High);
  static_assert(magic_enum::enum_name(FilteredAliases::Alias) == "First");
  static_assert(magic_enum::enum_name(FilteredAliases::Renamed) == "Custom");
  static_assert(!magic_enum::enum_contains(FilteredAliases::Hidden));
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
  // Customization renames or hides declared values, but cannot add one.
  static_assert(magic_enum::enum_values<Customized>() == std::array{Customized::Declared});
  static_assert(magic_enum::enum_name(Customized{2}).empty());
  static_assert(!magic_enum::enum_cast<Customized>("Synthetic").has_value());
  static_assert(magic_enum::detail::n<Customized{2}>().size_ == 0);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  static_assert(magic_enum::enum_values<HugeRange>() == std::array{HugeRange::Low, HugeRange::High});
  static_assert(magic_enum::enum_reflected(HugeRange::Low));
  static_assert(magic_enum::enum_reflected(HugeRange::High));
#else
  static_assert(!magic_enum::enum_reflected(HugeRange::Low));
  static_assert(!magic_enum::enum_reflected(HugeRange::High));
#endif
  static_assert(magic_enum::enum_reflected<HugeRange>(0));

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
  static_assert(flag_values == std::array{WideFlags::None, WideFlags::A, WideFlags::B, WideFlags::High});
  CHECK(magic_enum::enum_flags_name(WideFlags::AB) == "A|B");
  CHECK(magic_enum::enum_flags_name(WideFlags::None) == "None");
  static_assert(magic_enum::enum_flags_cast<WideFlags>("A|B") == WideFlags::AB);
  static_assert(!magic_enum::enum_flags_cast<WideFlags>(std::uint32_t{4}).has_value());
  static_assert(magic_enum::enum_reflected<WideFlags>(std::uint32_t{4}));
  static_assert(magic_enum::enum_reflected<WideFlags>(std::uint32_t{0}));
  static_assert(!magic_enum::enum_reflected(WideFlags::AB));

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
