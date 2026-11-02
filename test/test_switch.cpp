// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include "test_enums.hpp"
#include "test_flags_enums.hpp"

#include <magic_enum/magic_enum_bitwise.hpp>
#include <magic_enum/magic_enum_fuse.hpp>
#include <magic_enum/magic_enum_switch.hpp>

#include "test_helpers.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

#if defined(MAGIC_ENUM_ENABLE_HASH)

enum class paged_hash_value : unsigned {};

template <std::size_t... I>
constexpr auto make_paged_hash_values(std::index_sequence<I...>) noexcept {
  return std::array<paged_hash_value, sizeof...(I)>{{static_cast<paged_hash_value>(I)...}};
}

static constexpr auto paged_hash_values = make_paged_hash_values(std::make_index_sequence<257>{});

// No hashes remain above the largest one or below the smallest one, so unused case slots take values between them.
static constexpr std::array<paged_hash_value, 3> extreme_hash_values{{paged_hash_value{0}, static_cast<paged_hash_value>((std::numeric_limits<unsigned>::max)()), paged_hash_value{2}}};

struct paged_hash_search {
  unsigned value;
};

static int paged_hash_call_count = 0;

struct paged_counting_hash {
  using secondary_hash = paged_counting_hash;

  constexpr unsigned operator()(paged_hash_value value) const noexcept { return static_cast<unsigned>(value); }

  unsigned operator()(paged_hash_search searched) const noexcept {
    ++paged_hash_call_count;
    return searched.value;
  }
};

#endif

template <typename E>
constexpr auto switched_index(E value) noexcept {
  return magic_enum::detail::index_switch<magic_enum::detail::value_cases<E, magic_enum::detail::enum_subtype::common>>(static_cast<magic_enum::detail::switch_t<E>>(value));
}
static_assert(switched_index(Color::BLUE) == std::size_t{2});
static_assert(switched_index(number::_3) == std::size_t{2});

using namespace magic_enum;
using namespace magic_enum_tests;

template <typename... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

#if defined(MAGIC_ENUM_ENABLE_HASH)
TEST_CASE("hash switch computes the searched hash once across pages") {
  const auto not_found = (std::numeric_limits<std::size_t>::max)();
  const auto lookup = [](paged_hash_search searched) {
    return magic_enum::detail::hash_switch<&paged_hash_values, paged_counting_hash>(
        [](std::size_t i) { return i; },
        searched,
        [] { return (std::numeric_limits<std::size_t>::max)(); },
        [](paged_hash_value lhs, paged_hash_search rhs) { return static_cast<unsigned>(lhs) == rhs.value; });
  };

  paged_hash_call_count = 0;
  REQUIRE(lookup({256}) == 256);
  REQUIRE(paged_hash_call_count == 1);

  paged_hash_call_count = 0;
  REQUIRE(lookup({999}) == not_found);
  REQUIRE(paged_hash_call_count == 1);
}

TEST_CASE("hash switch with hashes at both limits of the type") {
  const auto not_found = (std::numeric_limits<std::size_t>::max)();
  const auto lookup = [](unsigned searched) {
    return magic_enum::detail::hash_switch<&extreme_hash_values, paged_counting_hash>(
        [](std::size_t i) { return i; },
        paged_hash_search{searched},
        [] { return (std::numeric_limits<std::size_t>::max)(); },
        [](paged_hash_value lhs, paged_hash_search rhs) { return static_cast<unsigned>(lhs) == rhs.value; });
  };

  REQUIRE(lookup(0) == 0);
  REQUIRE(lookup((std::numeric_limits<unsigned>::max)()) == 1);
  REQUIRE(lookup(2) == 2);
  REQUIRE(lookup(1) == not_found);
  REQUIRE(lookup(3) == not_found);
}
#endif

struct RefQualifiedSwitcher {
  template <typename T>
  constexpr int operator()(T) & { return 1; }

  template <typename T>
  constexpr int operator()(T) && { return 2; }
};

struct LvalueSwitchFallback {
  constexpr LvalueSwitchFallback() = default;
  constexpr LvalueSwitchFallback(int) {}
  LvalueSwitchFallback(const LvalueSwitchFallback&) = delete;

  constexpr operator int() & { return -1; }
};

struct ImmovableSwitchResult {
  int value;
  constexpr ImmovableSwitchResult(int v = 0) : value{v} {}
  ImmovableSwitchResult(const ImmovableSwitchResult&) = delete;
};

struct CustomSwitchResult {
  constexpr operator int() const noexcept { return 7; }
};

template <>
struct std::common_type<CustomSwitchResult, CustomSwitchResult> { using type = int; };

template <>
struct std::common_type<int, CustomSwitchResult> { using type = long; };

template <>
struct std::common_type<CustomSwitchResult, int> { using type = long; };

#define SWITCH_PAGE_VALUES(p) p##0, p##1, p##2, p##3, p##4, p##5, p##6, p##7, p##8, p##9
// 520 values span three switch pages.
enum class SwitchPages : short {
  SWITCH_PAGE_VALUES(A), SWITCH_PAGE_VALUES(B), SWITCH_PAGE_VALUES(C), SWITCH_PAGE_VALUES(D), SWITCH_PAGE_VALUES(E),
  SWITCH_PAGE_VALUES(F), SWITCH_PAGE_VALUES(G), SWITCH_PAGE_VALUES(H), SWITCH_PAGE_VALUES(I), SWITCH_PAGE_VALUES(J),
  SWITCH_PAGE_VALUES(K), SWITCH_PAGE_VALUES(L), SWITCH_PAGE_VALUES(M), SWITCH_PAGE_VALUES(N), SWITCH_PAGE_VALUES(O),
  SWITCH_PAGE_VALUES(P), SWITCH_PAGE_VALUES(Q), SWITCH_PAGE_VALUES(R), SWITCH_PAGE_VALUES(S), SWITCH_PAGE_VALUES(T),
  SWITCH_PAGE_VALUES(U), SWITCH_PAGE_VALUES(V), SWITCH_PAGE_VALUES(W), SWITCH_PAGE_VALUES(X), SWITCH_PAGE_VALUES(Y),
  SWITCH_PAGE_VALUES(Z), SWITCH_PAGE_VALUES(AA), SWITCH_PAGE_VALUES(AB), SWITCH_PAGE_VALUES(AC), SWITCH_PAGE_VALUES(AD),
  SWITCH_PAGE_VALUES(AE), SWITCH_PAGE_VALUES(AF), SWITCH_PAGE_VALUES(AG), SWITCH_PAGE_VALUES(AH), SWITCH_PAGE_VALUES(AI),
  SWITCH_PAGE_VALUES(AJ), SWITCH_PAGE_VALUES(AK), SWITCH_PAGE_VALUES(AL), SWITCH_PAGE_VALUES(AM), SWITCH_PAGE_VALUES(AN),
  SWITCH_PAGE_VALUES(AO), SWITCH_PAGE_VALUES(AP), SWITCH_PAGE_VALUES(AQ), SWITCH_PAGE_VALUES(AR), SWITCH_PAGE_VALUES(AS),
  SWITCH_PAGE_VALUES(AT), SWITCH_PAGE_VALUES(AU), SWITCH_PAGE_VALUES(AV), SWITCH_PAGE_VALUES(AW), SWITCH_PAGE_VALUES(AX),
  SWITCH_PAGE_VALUES(AY), SWITCH_PAGE_VALUES(AZ)
};

// 522 sparse values span three switch pages.
enum class SparsePages : short {
  SWITCH_PAGE_VALUES(A), SWITCH_PAGE_VALUES(B), SWITCH_PAGE_VALUES(C), SWITCH_PAGE_VALUES(D), SWITCH_PAGE_VALUES(E),
  SWITCH_PAGE_VALUES(F), SWITCH_PAGE_VALUES(G), SWITCH_PAGE_VALUES(H), SWITCH_PAGE_VALUES(I), SWITCH_PAGE_VALUES(J),
  SWITCH_PAGE_VALUES(K), SWITCH_PAGE_VALUES(L), SWITCH_PAGE_VALUES(M), SWITCH_PAGE_VALUES(N), SWITCH_PAGE_VALUES(O),
  SWITCH_PAGE_VALUES(P), SWITCH_PAGE_VALUES(Q), SWITCH_PAGE_VALUES(R), SWITCH_PAGE_VALUES(S), SWITCH_PAGE_VALUES(T),
  SWITCH_PAGE_VALUES(U), SWITCH_PAGE_VALUES(V), SWITCH_PAGE_VALUES(W), SWITCH_PAGE_VALUES(X), SWITCH_PAGE_VALUES(Y),
  SWITCH_PAGE_VALUES(Z), SWITCH_PAGE_VALUES(AA), SWITCH_PAGE_VALUES(AB), SWITCH_PAGE_VALUES(AC), SWITCH_PAGE_VALUES(AD),
  SWITCH_PAGE_VALUES(AE), SWITCH_PAGE_VALUES(AF), SWITCH_PAGE_VALUES(AG), SWITCH_PAGE_VALUES(AH), SWITCH_PAGE_VALUES(AI),
  SWITCH_PAGE_VALUES(AJ), SWITCH_PAGE_VALUES(AK), SWITCH_PAGE_VALUES(AL), SWITCH_PAGE_VALUES(AM), SWITCH_PAGE_VALUES(AN),
  SWITCH_PAGE_VALUES(AO), SWITCH_PAGE_VALUES(AP), SWITCH_PAGE_VALUES(AQ), SWITCH_PAGE_VALUES(AR), SWITCH_PAGE_VALUES(AS),
  SWITCH_PAGE_VALUES(AT), SWITCH_PAGE_VALUES(AU), SWITCH_PAGE_VALUES(AV), SWITCH_PAGE_VALUES(AW), SWITCH_PAGE_VALUES(AX),
  SWITCH_PAGE_VALUES(AY), SWITCH_PAGE_VALUES(AZ), Neg = -10, Far = 600
};
#undef SWITCH_PAGE_VALUES

template <>
struct magic_enum::customize::enum_range<SparsePages> {
  static constexpr int min = -10;
  static constexpr int max = 600;
};

template <>
struct magic_enum::customize::enum_range<SwitchPages> {
  static constexpr int min = 0;
  static constexpr int max = 519;
};

// No values remain above the largest enumerator, so unused case slots take values below the smallest one.
enum class SwitchNearMax : int { A = 2147483647 - 3, B = 2147483647 - 1, C = 2147483647 };

template <>
struct magic_enum::customize::enum_range<SwitchNearMax> {
  static constexpr int min = 2147483647 - 8;
  static constexpr int max = 2147483647;
};

// Narrow underlying types are switched on as int.
enum class SwitchInt8 : std::int8_t { Min = -128, NegOne = -1, Zero = 0, Max = 127 };

template <>
struct magic_enum::customize::enum_range<SwitchInt8> {
  static constexpr int min = -128;
  static constexpr int max = 127;
};

TEST_CASE("sparse lookup across switch pages") {
  static_assert(enum_count<SparsePages>() == 522);
  static_assert(enum_index(SparsePages::Neg) == std::size_t{0});
  static_assert(enum_index(SparsePages::A0) == std::size_t{1});
  static_assert(enum_index(SparsePages::Z4) == std::size_t{255});
  static_assert(enum_index(SparsePages::Z5) == std::size_t{256});
  static_assert(enum_index(SparsePages::Far) == std::size_t{521});
  static_assert(enum_cast<SparsePages>(600) == SparsePages::Far);
  static_assert(!enum_cast<SparsePages>(520).has_value());
  for (int i = -20; i <= 620; ++i) {
    const auto value = static_cast<SparsePages>(i);
    const bool member = (i >= 0 && i <= 519) || i == -10 || i == 600;
    const auto index = enum_index(value);
    REQUIRE(index.has_value() == member);
    REQUIRE(enum_cast<SparsePages>(static_cast<short>(i)).has_value() == member);
    if (index) {
      REQUIRE(enum_value<SparsePages>(*index) == value);
    }
  }

  static_assert(enum_index(SwitchNearMax::A) == std::size_t{0});
  static_assert(enum_index(SwitchNearMax::C) == std::size_t{2});
  static_assert(enum_cast<SwitchNearMax>(2147483647) == SwitchNearMax::C);
  static_assert(!enum_cast<SwitchNearMax>(2147483647 - 2).has_value());
  static_assert(!enum_cast<SwitchNearMax>((std::numeric_limits<int>::min)()).has_value());
  static_assert(!enum_index(static_cast<SwitchNearMax>((std::numeric_limits<int>::min)() + 1)).has_value());

  for (int i = -128; i <= 127; ++i) {
    const bool member = i == -128 || i == -1 || i == 0 || i == 127;
    REQUIRE(enum_cast<SwitchInt8>(static_cast<std::int8_t>(i)).has_value() == member);
    REQUIRE(enum_index(static_cast<SwitchInt8>(i)).has_value() == member);
  }
}

struct AnySwitchCase {
  template <typename T>
  constexpr int operator()(T) const { return 1; }
};

template <typename E, typename = void>
inline constexpr bool switch_accepts_v = false;

template <typename E>
inline constexpr bool switch_accepts_v<E, std::void_t<decltype(magic_enum::enum_switch(AnySwitchCase{}, std::declval<E>()))>> = true;

template <typename E, typename = void>
inline constexpr bool switch_fallback_accepts_v = false;

template <typename E>
inline constexpr bool switch_fallback_accepts_v<E, std::void_t<decltype(magic_enum::enum_switch(AnySwitchCase{}, std::declval<E>(), 0))>> = true;

// A non-enum drops out of overload resolution instead of failing inside the tables.
static_assert(switch_accepts_v<Color>);
static_assert(!switch_accepts_v<int>);
static_assert(switch_fallback_accepts_v<Color>);
static_assert(!switch_fallback_accepts_v<int>);

TEST_CASE("enum_switch") {
  SUBCASE("member function pointer") {
    constexpr auto member = &enum_constant<Color::GREEN>::operator();
    static_assert(enum_switch(member, Color::GREEN) == Color::GREEN);
    static_assert(enum_switch(member, Color::RED) == Color{});
    static_assert(enum_switch(member, Color::GREEN, Color::BLUE) == Color::GREEN);
    static_assert(enum_switch(member, Color::RED, Color::BLUE) == Color::BLUE);
    static_assert(enum_switch(member, static_cast<Color>(0), Color::BLUE) == Color::BLUE);
    REQUIRE(enum_switch(std::ref(member), Color::GREEN) == Color::GREEN);
  }

  SUBCASE("fallback value categories") {
    constexpr auto eq_red = [](auto val) { return val() == Color::RED; };
    constexpr bool fallback = true;
    bool mutable_fallback = true;

    static_assert(std::is_same_v<decltype(enum_switch(eq_red, Color::GREEN, fallback)), bool>);
    static_assert(std::is_same_v<decltype(enum_switch(eq_red, Color::GREEN, mutable_fallback)), bool>);
    static_assert(!enum_switch(eq_red, Color::GREEN, fallback));
    static_assert(!enum_switch(eq_red, Color::GREEN, true));
    static_assert(!enum_switch(eq_red, Color::GREEN, std::move(fallback)));
    static_assert(enum_switch(eq_red, Color::RED, fallback));
    static_assert(enum_switch(eq_red, static_cast<Color>(0), fallback));
    REQUIRE_FALSE(enum_switch(eq_red, Color::BLUE, mutable_fallback));
    REQUIRE(mutable_fallback);

    constexpr auto integer = [](auto) { return 7; };
    long wide_fallback = -1;
    const long const_wide_fallback = -1;
    static_assert(std::is_same_v<decltype(enum_switch(integer, Color::RED, wide_fallback)), long>);
    static_assert(std::is_same_v<decltype(enum_switch(integer, Color::RED, const_wide_fallback)), long>);
    static_assert(std::is_same_v<decltype(enum_switch(integer, Color::RED, -1L)), long>);
    REQUIRE(enum_switch(integer, Color::RED, wide_fallback) == 7L);
    REQUIRE(enum_switch(integer, static_cast<Color>(0), const_wide_fallback) == -1L);
    LvalueSwitchFallback converted_fallback;
    static_assert(std::is_same_v<decltype(enum_switch(integer, Color::RED, converted_fallback)), int>);
    REQUIRE(enum_switch(integer, Color::RED, converted_fallback) == 7);
    REQUIRE(enum_switch(integer, static_cast<Color>(0), converted_fallback) == -1);
  }

  SUBCASE("partial invocable with lvalue fallback") {
    int calls = 0;
    const int fallback = -1;
    const auto switcher = overloaded{
      [&calls](enum_constant<Color::RED>) { ++calls; return 1; },
      [&calls](enum_constant<Color::BLUE>) { ++calls; return 2; }
    };

    REQUIRE(enum_switch(switcher, Color::RED, fallback) == 1);
    REQUIRE(calls == 1);
    REQUIRE(enum_switch(switcher, Color::BLUE, fallback) == 2);
    REQUIRE(calls == 2);
    REQUIRE(enum_switch(switcher, Color::GREEN, fallback) == -1);
    REQUIRE(enum_switch(switcher, static_cast<Color>(0), fallback) == -1);
    REQUIRE(calls == 2);
  }

  SUBCASE("reference results") {
    int stored = 7;
    const int fallback = -1;
    const auto reference = [&stored](auto) -> int& { return stored; };
    static_assert(std::is_same_v<decltype(enum_switch(reference, Color::RED, fallback)), int>);
    static_assert(std::is_same_v<decltype(enum_switch<const int&>(reference, Color::RED)), int>);
    auto result = enum_switch(reference, Color::RED, fallback);
    stored = 9;
    REQUIRE(result == 7);
    REQUIRE(enum_switch<const int&>(reference, static_cast<Color>(0)) == 0);

    const auto text = [](auto) { return std::string{"matched"}; };
    const std::string default_text = "fallback";
    static_assert(std::is_same_v<decltype(enum_switch(text, Color::RED, default_text)), std::string>);
    REQUIRE(enum_switch(text, Color::RED, default_text) == "matched");
    REQUIRE(enum_switch(text, static_cast<Color>(0), "fallback") == "fallback");
  }

  SUBCASE("partial invocable with reference results") {
    int stored = 7;
    const auto first = [&stored](enum_constant<Color::RED>) -> const int& { return stored; };
    const auto last = [&stored](enum_constant<Color::BLUE>) -> const int& { return stored; };
    LvalueSwitchFallback fallback;

    static_assert(std::is_same_v<decltype(enum_switch(first, Color::RED)), int>);
    static_assert(std::is_same_v<decltype(enum_switch(last, Color::BLUE)), int>);
    static_assert(std::is_same_v<decltype(enum_switch(first, Color::RED, fallback)), int>);
    REQUIRE(enum_switch(first, Color::RED) == 7);
    REQUIRE(enum_switch(last, Color::BLUE) == 7);
    REQUIRE(enum_switch(first, Color::GREEN) == 0);
    REQUIRE(enum_switch(first, Color::RED, fallback) == 7);
    REQUIRE(enum_switch(first, Color::GREEN, fallback) == -1);
    REQUIRE(enum_switch(first, static_cast<Color>(0), fallback) == -1);
  }

  SUBCASE("move-only fallback") {
    const auto switcher = [](enum_constant<Color::RED>) { return std::make_unique<int>(7); };
    auto fallback = std::make_unique<int>(-1);
    auto matched = enum_switch(switcher, Color::RED, std::move(fallback));
    REQUIRE(*matched == 7);
    REQUIRE(fallback != nullptr);
    REQUIRE(*fallback == -1);
    auto unmatched = enum_switch(switcher, Color::GREEN, std::move(fallback));
    REQUIRE(*unmatched == -1);
    REQUIRE(fallback == nullptr);
  }

  SUBCASE("explicit result without fallback") {
    using Result = std::unique_ptr<int>;
    const auto switcher = [](auto) { return std::make_unique<int>(7); };
    const auto partial = [](enum_constant<Color::RED>) { return std::make_unique<int>(7); };
    static_assert(std::is_same_v<decltype(enum_switch<const Result>(switcher, Color::RED)), Result>);
    static_assert(std::is_same_v<decltype(enum_switch<const Result&, as_common<>>(partial, Color::RED)), Result>);
    REQUIRE(*enum_switch<const Result>(switcher, Color::RED) == 7);
    REQUIRE(*enum_switch<const Result&, as_common<>>(partial, Color::RED) == 7);
    REQUIRE(enum_switch<const Result&>(partial, Color::GREEN) == nullptr);
    REQUIRE(enum_switch<const Result, as_common<>>(switcher, static_cast<Color>(0)) == nullptr);
    enum class Flag { A = 1, B = 2 };
    REQUIRE(*enum_switch<const Result, as_flags<>>(switcher, Flag::B) == 7);
    REQUIRE(enum_switch<const Result, as_flags<>>(switcher, static_cast<Flag>(3)) == nullptr);

    constexpr auto integer = [](auto) { return 7; };
    static_assert(enum_switch<ImmovableSwitchResult>(integer, Color::RED).value == 7);
    static_assert(enum_switch<ImmovableSwitchResult>(integer, static_cast<Color>(0)).value == 0);
    static_assert(enum_switch<const ImmovableSwitchResult&, as_common<>>(integer, Color::RED).value == 7);
    static_assert(enum_switch<const ImmovableSwitchResult&, as_common<>>(integer, static_cast<Color>(0)).value == 0);
  }

  SUBCASE("explicit subtype overloads") {
    constexpr auto integer = [](auto) { return 7; };
    constexpr long fallback = -1;
    static_assert(std::is_same_v<decltype(enum_switch<const long&, as_common<>>(integer, Color::RED, fallback)), long>);
    static_assert(enum_switch<const long&, as_common<>>(integer, Color::RED, fallback) == 7L);
    static_assert(enum_switch<const long&, as_common<>>(integer, static_cast<Color>(0), fallback) == -1L);
    static_assert(enum_switch<const long&, as_common<>>(integer, Color::RED) == 7L);
    enum class Flag { A = 1, B = 2, C = 4 };
    static_assert(enum_switch<const long&, as_flags<>>(integer, Flag::B, fallback) == 7L);
    static_assert(enum_switch<const long&, as_flags<>>(integer, static_cast<Flag>(3), fallback) == -1L);
  }

  SUBCASE("callable and argument categories") {
    using V = enum_constant<Color::GREEN>;
    static_assert(std::is_invocable_v<RvalueOnlyArgument, V>);
    static_assert(!std::is_invocable_v<RvalueOnlyArgument, V&>);
    static_assert(!std::is_invocable_v<RvalueOnlyArgument, const V&>);
    static_assert(enum_switch(RvalueOnlyArgument{}, Color::GREEN, 0) == 7);
    RefQualifiedSwitcher switcher;
    REQUIRE(enum_switch(switcher, Color::GREEN, 0) == 1);
    REQUIRE(enum_switch(RefQualifiedSwitcher{}, Color::GREEN, 0) == 2);
    int calls = 0;
    enum_switch([&calls](auto) { ++calls; }, Color::GREEN);
    enum_switch([&calls](auto) { ++calls; }, static_cast<Color>(0));
    REQUIRE(calls == 1);
  }

  SUBCASE("complete invocable") {
    constexpr auto red = enum_switch<int>([](auto val) {
      return enum_integer(val());
    }, Color::RED);

    REQUIRE(red == -12);
    REQUIRE(enum_switch<int>([](auto val) {
      return enum_integer(val());
    }, Color::GREEN) == 7);

    REQUIRE(enum_switch<int>([](auto val) {
      return enum_integer(val());
    }, hostile_enum_comparison::Value::Second) == 4);
  }

  SUBCASE("partial invocable falls back to default result") {
    const auto switcher = overloaded{
      [](enum_constant<Color::RED>) { return 1; },
      [](enum_constant<Color::BLUE>) { return 2; }
    };

    REQUIRE(enum_switch<int>(switcher, Color::RED, -1) == 1);
    REQUIRE(enum_switch<int>(switcher, Color::BLUE, -1) == 2);
    REQUIRE(enum_switch<int>(switcher, Color::GREEN, -1) == -1);
  }

  SUBCASE("invalid enum value falls back to default result") {
    REQUIRE(enum_switch<int>([](auto val) {
      return enum_integer(val());
    }, static_cast<Color>(0)) == 0);
    REQUIRE(enum_switch<int>([](auto val) {
      return enum_integer(val());
    }, static_cast<Color>(0), -1) == -1);
  }

  SUBCASE("multiple pages and case labels") {
    constexpr auto integer = [](auto val) { return static_cast<long long>(enum_integer(val())); };

    static_assert(enum_count<SwitchPages>() == 520);
    static_assert(enum_switch(integer, SwitchPages::A0) == 0);
    static_assert(enum_switch(integer, SwitchPages::Z5) == 255);
    static_assert(enum_switch(integer, SwitchPages::AA6) == 266);
    static_assert(enum_switch(integer, SwitchPages::AZ9) == 519);
    static_assert(enum_switch(integer, static_cast<SwitchPages>(520), -1LL) == -1);
    static_assert(enum_switch(integer, static_cast<SwitchPages>(-1), -1LL) == -1);
    for (auto value : enum_values<SwitchPages>()) {
      REQUIRE(enum_switch(integer, value) == enum_integer(value));
    }

    static_assert(enum_switch(integer, SwitchNearMax::A) == 2147483647LL - 3);
    static_assert(enum_switch(integer, SwitchNearMax::C) == 2147483647LL);
    static_assert(enum_switch(integer, static_cast<SwitchNearMax>(2147483647 - 2), -1LL) == -1);
    static_assert(enum_switch(integer, static_cast<SwitchNearMax>(-2147483647 - 1), -1LL) == -1);
    REQUIRE(enum_switch(integer, SwitchNearMax::B) == 2147483647LL - 1);
  }

  SUBCASE("partial invocable across multiple pages") {
    constexpr auto last = [](enum_constant<SwitchPages::AZ9>) { return 519; };
    static_assert(std::is_same_v<decltype(enum_switch(last, SwitchPages::AZ9)), int>);
    static_assert(enum_switch(last, SwitchPages::AZ9) == 519);
    static_assert(enum_switch(last, SwitchPages::A0) == 0);
    static_assert(enum_switch(last, SwitchPages::Z5, -1) == -1);
    static_assert(enum_switch(last, static_cast<SwitchPages>(520), -1) == -1);

    constexpr auto mixed = overloaded{
      [](enum_constant<SwitchPages::A0>) { return 1; },
      [](enum_constant<SwitchPages::AZ9>) { return 519L; }
    };
    static_assert(std::is_same_v<decltype(enum_switch(mixed, SwitchPages::A0)), long>);
    static_assert(enum_switch(mixed, SwitchPages::A0) == 1L);
    static_assert(enum_switch(mixed, SwitchPages::AZ9) == 519L);
    static_assert(enum_switch(mixed, SwitchPages::AA6, -1L) == -1L);
    for (auto value : enum_values<SwitchPages>()) {
      REQUIRE(enum_switch(last, value, -1) == (value == SwitchPages::AZ9 ? 519 : -1));
    }
  }

  SUBCASE("custom common type preserves the fold order") {
    enum class One { Value };
    enum class Two { First, Last };
    constexpr auto custom = [](auto) { return CustomSwitchResult{}; };
    static_assert(std::is_same_v<decltype(enum_switch(custom, One::Value)), int>);
    static_assert(std::is_same_v<decltype(enum_switch(custom, Two::First)), int>);
    // The first two results combine to int, then int and the third combine to long.
    static_assert(std::is_same_v<decltype(enum_switch(custom, Color::RED)), long>);
    static_assert(std::is_same_v<decltype(enum_switch(custom, SwitchPages::AZ9)), long>);
    static_assert(enum_switch(custom, One::Value) == 7);
    static_assert(enum_switch(custom, Two::Last) == 7);
    static_assert(enum_switch(custom, Color::GREEN) == 7L);
    static_assert(enum_switch(custom, SwitchPages::AZ9) == 7L);
    static_assert(enum_switch(custom, static_cast<Two>(2)) == 0);
    static_assert(enum_switch(custom, static_cast<SwitchPages>(520)) == 0L);
    REQUIRE(enum_switch(custom, Color::BLUE) == 7L);
    REQUIRE(enum_switch(custom, static_cast<Color>(0)) == 0L);
  }

  SUBCASE("narrow and bool underlying types") {
    constexpr auto integer = [](auto val) { return static_cast<int>(enum_integer(val())); };

    static_assert(enum_switch(integer, Binary::ONE, -1) == 0);
    static_assert(enum_switch(integer, Binary::TWO, -1) == 1);
    static_assert(enum_switch(integer, MaxUsedAsInvalid::TWO, -1) == 63);
    // Outside enum_range for the scanning backend, but standard reflection still sees it.
    static_assert(enum_switch(integer, MaxUsedAsInvalid::INVALID, -1) == (enum_contains(MaxUsedAsInvalid::INVALID) ? 255 : -1));
    static_assert(enum_switch(integer, static_cast<MaxUsedAsInvalid>(64), -1) == -1);
    static_assert(enum_switch(integer, SwitchInt8::Min, 1000) == -128);
    static_assert(enum_switch(integer, SwitchInt8::Max, 1000) == 127);
    for (int i = -128; i <= 127; ++i) {
      const auto value = static_cast<SwitchInt8>(i);
      REQUIRE(enum_switch(integer, value, 1000) == (enum_contains(value) ? i : 1000));
    }
  }

  SUBCASE("different result types use common type") {
    constexpr auto mixed = [](auto val) {
      if constexpr (decltype(val)::value == Color::RED) {
        return 1;
      } else {
        return 2L;
      }
    };

    static_assert(std::is_same_v<decltype(enum_switch(mixed, Color::GREEN)), long>);
    static_assert(enum_switch(mixed, Color::RED) == 1L);
    static_assert(enum_switch(mixed, Color::BLUE) == 2L);
    static_assert(enum_switch(mixed, static_cast<Color>(0)) == 0L);
    static_assert(enum_switch(mixed, static_cast<Color>(0), -1) == -1L);
  }
}

#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1920
#  define MAGIC_ENUM_SUPPORTED_CONSTEXPR_FOR 1
#endif

#if defined(MAGIC_ENUM_SUPPORTED_CONSTEXPR_FOR)

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4064) // MSVC treats the opaque enum returned by enum_fuse as incomplete in switch statements.
#endif

static int switch_case_2d(Color color, Directions direction) {
  switch (enum_fuse(color, direction).value()) {
    case enum_fuse(Color::RED, Directions::Up).value():
      return 1;
    case enum_fuse(Color::BLUE, Directions::Down).value():
      return 2;
    default:
      return 0;
  }
}

enum class Index { zero = 0, one = 1, two = 2 };
enum class FuseFive { zero, one, two, three, four };

static_assert(magic_enum::detail::fuse_bit_width<FuseFive>() == 3);
static_assert(magic_enum::detail::fuse_bit_width<FuseFive>() * 21 <= sizeof(std::uintmax_t) * 8);
static_assert(magic_enum::detail::fuse_bit_width<FuseFive>() * 22 > sizeof(std::uintmax_t) * 8);

static int switch_case_3d(Color color, Directions direction, Index index) {
  switch (enum_fuse(color, direction, index).value()) {
    case enum_fuse(Color::RED, Directions::Up, Index::zero).value():
      return 1;
    case enum_fuse(Color::BLUE, Directions::Up, Index::zero).value():
      return 2;
    default:
      return 0;
  }
}

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

TEST_CASE("multdimensional-switch-case") {
  REQUIRE(switch_case_2d(Color::RED, Directions::Up) == 1);
  REQUIRE(switch_case_2d(Color::RED, Directions::Down) == 0);
  REQUIRE(switch_case_2d(Color::BLUE, Directions::Up) == 0);
  REQUIRE(switch_case_2d(Color::BLUE, Directions::Down) == 2);
  REQUIRE(switch_case_3d(Color::RED, Directions::Up, Index::zero) == 1);
  REQUIRE(switch_case_3d(Color::BLUE, Directions::Up, Index::zero) == 2);
  REQUIRE(switch_case_3d(Color::BLUE, Directions::Up, Index::one) == 0);
  REQUIRE(switch_case_3d(Color::BLUE, Directions::Up, Index::two) == 0);
}

#endif

namespace flags_test {

using namespace magic_enum::bitwise_operators;

TEST_CASE("flags enum_switch") {
  SUBCASE("dispatches flag enumerators") {
    REQUIRE(enum_switch<int>([](auto val) {
      return enum_integer(val());
    }, Color::RED) == 1);
    REQUIRE(enum_switch<int>([](auto val) {
      return enum_integer(val());
    }, Color::BLUE) == 4);
  }

  SUBCASE("combined flags fall back to default result") {
    REQUIRE(enum_switch<int>([](auto val) {
      return enum_integer(val());
    }, Color::RED | Color::GREEN) == 0);
    REQUIRE(enum_switch<int>([](auto val) {
      return enum_integer(val());
    }, Color::RED | Color::GREEN, -1) == -1);
  }

  SUBCASE("partial invocable falls back to default result") {
    const auto switcher = overloaded{
      [](enum_constant<Color::RED>) { return 1; },
      [](enum_constant<Color::BLUE>) { return 4; }
    };

    REQUIRE(enum_switch<int>(switcher, Color::RED, -1) == 1);
    REQUIRE(enum_switch<int>(switcher, Color::BLUE, -1) == 4);
    REQUIRE(enum_switch<int>(switcher, Color::GREEN, -1) == -1);
  }

  SUBCASE("matches enum_index for single bits, zero, and high bits") {
    const auto check = [](auto value) {
      using E = decltype(value);
      constexpr auto none = enum_count<E>();
      const auto index = enum_index(value);
      REQUIRE(enum_switch<std::size_t>([](auto val) { return *enum_index(val()); }, value, enum_count<E>()) == (index ? *index : none));
    };

    for (auto value : enum_values<Directions>()) {
      check(value);
    }
    for (std::size_t i = 0; i < 64; ++i) {
      check(static_cast<Directions>(std::uint64_t{1} << i));
    }
    check(static_cast<Directions>(0));
    check(Directions::Left | Directions::Right);
    check(static_cast<Directions>(~std::uint64_t{0}));

    for (std::size_t i = 0; i < 31; ++i) {
      check(static_cast<Numbers>(1 << i));
    }
    check(Numbers::none);
    check(static_cast<Numbers>(-1));

    check(BoolFlags::Disabled);
    check(BoolFlags::Enabled);

    for (std::size_t i = 0; i < 9; ++i) {
      check(static_cast<CStyleFlags>(1 << i));
    }
    check(static_cast<CStyleFlags>(0));
    check(CStyleFlags_A | CStyleFlags_I);

    static_assert(enum_switch<int>([](auto val) { return val() == Directions::Right ? 63 : 0; }, Directions::Right, -1) == 63);
    static_assert(enum_switch<int>([](auto val) { return val() == Numbers::many ? 30 : 0; }, Numbers::many, -1) == 30);
    static_assert(enum_switch<int>([](auto) { return 0; }, static_cast<Numbers>(1 << 29), -1) == -1);
  }
}

} // namespace flags_test
