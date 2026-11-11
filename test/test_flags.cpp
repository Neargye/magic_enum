// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include "test_flags_enums.hpp"

#include <magic_enum/magic_enum_bitwise.hpp>
#include <magic_enum/magic_enum_flags.hpp>
#include <magic_enum/magic_enum_fuse.hpp>
#include <magic_enum/magic_enum_switch.hpp>

#include "test_fixed_string.hpp"
#include "test_helpers.hpp"

#include <array>
#include <cctype>
#include <limits>
#include <string>
#include <string_view>

template <int>
struct templated_flags_scope {
  enum class Value : unsigned { None = 0, A = 1, B = 4 };
};

TEST_CASE("class-template flags reflect before enumerator access") {
  using namespace magic_enum;
  CHECK(enum_flags_name(static_cast<templated_flags_scope<0>::Value>(5)) == "A|B");
  static_assert(enum_flags_cast<templated_flags_scope<1>::Value>("A|B") == static_cast<templated_flags_scope<1>::Value>(5));
  static_assert(enum_flags_contains(static_cast<templated_flags_scope<2>::Value>(5)));
  CHECK(enum_flags_name(static_cast<templated_flags_scope<3>::Value>(0)) == "None");
}

enum class InvalidFlagsSetting { Value };
template <>
struct magic_enum::customize::enum_range<InvalidFlagsSetting> {
  static constexpr int is_flags = 1;
};

static_assert(!magic_enum::is_flags_enum_v<InvalidFlagsSetting&>);

enum class ZeroFlags : std::uint8_t { None = 0, A = 1, B = 2, Both = 3 };
template <>
struct magic_enum::customize::enum_range<ZeroFlags> {
  static constexpr bool is_flags = true;
};

enum class OnlyZeroFlags : std::uint8_t { None = 0 };
template <>
struct magic_enum::customize::enum_range<OnlyZeroFlags> {
  static constexpr bool is_flags = true;
};

enum class HighBitFlags : unsigned { A = 1, Both = 3, High = 128 };
template <>
struct magic_enum::customize::enum_range<HighBitFlags> {
  static constexpr bool is_flags = true;
};

enum class SignedZeroFlags : std::int8_t { High = -128, None = 0, Low = 1 };
template <>
struct magic_enum::customize::enum_range<SignedZeroFlags> {
  static constexpr bool is_flags = true;
};

enum class HiddenZeroFlags : std::uint8_t { None = 0, A = 1, B = 2 };
template <>
struct magic_enum::customize::enum_range<HiddenZeroFlags> {
  static constexpr bool is_flags = true;
};
template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<HiddenZeroFlags>(HiddenZeroFlags value) noexcept {
  return value == HiddenZeroFlags::None ? customize_t{invalid_tag} : customize_t{default_tag};
}

enum class RenamedZeroFlags : std::uint8_t { None = 0, A = 1, B = 2 };
template <>
struct magic_enum::customize::enum_range<RenamedZeroFlags> {
  static constexpr bool is_flags = true;
};
template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<RenamedZeroFlags>(RenamedZeroFlags value) noexcept {
  return value == RenamedZeroFlags::None ? customize_t{"Nothing"} : customize_t{default_tag};
}

using namespace magic_enum;
using namespace magic_enum::bitwise_operators;
using namespace magic_enum_tests;

using namespace flags_test;

struct LvalueOnlyPredicate {
  constexpr bool operator()(char lhs, char rhs) & noexcept { return lhs == rhs; }
};

TEST_CASE("enum_reflected") {
  REQUIRE(enum_reflected<Color>(Color::RED));
  REQUIRE(enum_reflected<as_flags<>>(Color::RED));
  REQUIRE(enum_reflected<Color, as_flags<>>(Color::BLUE));
  REQUIRE(enum_reflected<Directions>(Directions::Left));
  REQUIRE(enum_reflected<Directions>(Directions::Right));
  REQUIRE(enum_reflected<Directions>(std::uint64_t{1} << 62));
  REQUIRE(enum_reflected<Directions>(Directions::NoDirection));
  REQUIRE_FALSE(enum_reflected<Directions>(Directions::Left | Directions::Down));
  REQUIRE_FALSE(enum_reflected<Directions>(std::uint64_t{3}));
  REQUIRE(enum_reflected(BoolFlags::Disabled));
  REQUIRE(enum_reflected(BoolFlags::Enabled));
  REQUIRE(enum_reflected<BoolFlags, as_common<>>(BoolFlags::Disabled));
  REQUIRE(enum_reflected<BoolFlags, as_common<>>(BoolFlags::Enabled));
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(enum_reflected<Directions, as_common<>>(Directions::Left));
#else
  REQUIRE_FALSE(enum_reflected<Directions, as_common<>>(Directions::Left));
#endif
  REQUIRE_FALSE(enum_contains<Directions>(std::uint64_t{1} << 62));
}

TEST_CASE("as_common<> on flags allows bits outside the common range") {
  using namespace magic_enum;
  static_assert(enum_name<as_common<>>(HighBitFlags::Both) == "Both");
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  static_assert(enum_count<HighBitFlags, as_common<>>() == 3);
#else
  static_assert(enum_count<HighBitFlags, as_common<>>() == 2);
#endif
}

TEST_CASE("enum_reflected separates the flags domain from membership and masks") {
  enum class ReflF : std::uint8_t { None = 0, A = 1, B = 2, Both = 3 };
  constexpr auto flags = as_flags<true>;

  static_assert(!enum_reflected<ReflF, flags>(257));
  static_assert(!enum_reflected<ReflF, flags>(-1));
  static_assert(enum_reflected<ReflF, flags>(0));
  static_assert(enum_reflected<ReflF, flags>(4));
  static_assert(enum_reflected<ReflF, flags>(128));
  static_assert(!enum_contains<ReflF, flags>(4));
  static_assert(!enum_reflected<ReflF, flags>(3));
  static_assert(!enum_reflected<flags>(ReflF::Both));
  static_assert(enum_flags_contains<ReflF>(3));
  static_assert(!enum_flags_contains<ReflF>(4));

  static_assert(enum_reflected<BoolFlags>(0));
  static_assert(enum_reflected<BoolFlags>(1));
  static_assert(!enum_reflected<BoolFlags>(2));

  enum class ReflInt8Flags : std::int8_t { Low = 1, High = -128 };
  enum class ReflInt64Flags : std::int64_t { Low = 1 };
  static_assert(enum_reflected<ReflInt8Flags, flags>(-128));
  static_assert(enum_reflected<ReflInt8Flags, flags>(64));
  static_assert(!enum_reflected<ReflInt8Flags, flags>(-127));
  static_assert(!enum_reflected<ReflInt8Flags, flags>(128));
  static_assert(enum_reflected<ReflInt64Flags, flags>((std::numeric_limits<std::int64_t>::min)()));
  static_assert(!enum_reflected<ReflInt64Flags, flags>((std::numeric_limits<std::uint64_t>::max)()));

  enum class ReflEmptyFlags : std::uint8_t {};
  static_assert(enum_reflected<ReflEmptyFlags, flags>(0));
  static_assert(enum_reflected<ReflEmptyFlags, flags>(64));
  static_assert(!enum_reflected<ReflEmptyFlags, flags>(65));
}

TEST_CASE("bool flag text lookup does not convert pointers to true") {
  static_assert(enum_cast<BoolFlags>("Enabled") == BoolFlags::Enabled);
  static_assert(enum_cast<BoolFlags>("Disabled") == BoolFlags::Disabled);
  static_assert(!enum_cast<BoolFlags>("INVALID").has_value());
  static_assert(!enum_cast<BoolFlags>("").has_value());
  static_assert(enum_cast<BoolFlags, as_common<>>("Disabled") == BoolFlags::Disabled);
  static_assert(enum_contains<BoolFlags>("Disabled"));
  static_assert(!enum_contains<BoolFlags>("INVALID"));
  static_assert(enum_flags_cast<BoolFlags>("Enabled") == BoolFlags::Enabled);
  static_assert(enum_flags_cast<BoolFlags>("Disabled") == BoolFlags::Disabled);
  static_assert(!enum_flags_cast<BoolFlags>("INVALID").has_value());
  static_assert(!enum_flags_cast<BoolFlags>("").has_value());
  static_assert(!enum_flags_contains<BoolFlags>("INVALID"));
  static_assert(!enum_flags_contains<BoolFlags>(""));
  const char* invalid = "INVALID";
  CHECK_FALSE(enum_flags_cast<BoolFlags>(invalid).has_value());
  CHECK_FALSE(enum_flags_contains<BoolFlags>(invalid));
  CHECK(enum_flags_cast<BoolFlags>("enabled", '|', case_insensitive) == BoolFlags::Enabled);
}

TEST_CASE("bool flags reflect the named zero") {
  constexpr auto enabled_name = string_view{"Enabled"};
  constexpr auto common_values = enum_values<BoolFlags, as_common<>>();
  static_assert(common_values.size() == 2);
  static_assert(common_values[0] == BoolFlags::Disabled);
  static_assert(common_values[1] == BoolFlags::Enabled);
  constexpr auto flag_values = enum_values<BoolFlags>();
  static_assert(flag_values.size() == 2);
  static_assert(flag_values[0] == BoolFlags::Disabled);
  static_assert(flag_values[1] == BoolFlags::Enabled);
  CHECK(enum_flags_name(BoolFlags::Disabled) == "Disabled");
  CHECK(enum_flags_name(BoolFlags::Enabled) == "Enabled");
  static_assert(enum_flags_cast<BoolFlags>(false) == BoolFlags::Disabled);
  static_assert(enum_flags_cast<BoolFlags>(true) == BoolFlags::Enabled);
  static_assert(enum_flags_cast<BoolFlags>(enabled_name) == BoolFlags::Enabled);
  static_assert(enum_flags_cast<BoolFlags>(string_view{"Disabled"}) == BoolFlags::Disabled);
  static_assert(enum_cast<BoolFlags, as_common<>>(string_view{"Disabled"}) == BoolFlags::Disabled);
  static_assert(enum_cast<BoolFlags, as_common<>>(string_view{"Enabled"}) == BoolFlags::Enabled);
  static_assert(enum_contains<BoolFlags, as_common<>>(string_view{"Disabled"}));
  static_assert(enum_flags_contains<BoolFlags>(enabled_name));
  static_assert(enum_flags_contains<BoolFlags>(string_view{"Disabled"}));
  static_assert(enum_flags_test(BoolFlags::Enabled, BoolFlags::Enabled));
  static_assert(!enum_flags_test(BoolFlags::Disabled, BoolFlags::Enabled));
  static_assert(enum_flags_test_any(BoolFlags::Enabled, BoolFlags::Enabled));
  static_assert(!enum_flags_test_any(BoolFlags::Disabled, BoolFlags::Enabled));
}

TEST_CASE("named composite flags require every constituent bit") {
  // A named combination does not supply an otherwise unnamed bit, see #409.
  enum class MissingBit : std::uint8_t { System = 1, Null = 2, Binary = 4, Autoincrement = 12 };
  enum class CompleteBits : std::uint8_t { System = 1, Null = 2, Binary = 4, Empty = 8, Autoincrement = 12 };

  static_assert(enum_count<MissingBit, as_flags<false>>() == 4);
  static_assert(enum_count<MissingBit, as_flags<true>>() == 3);
  static_assert(enum_count<CompleteBits, as_flags<true>>() == 4);
  static_assert(enum_cast<MissingBit, as_flags<false>>(12) == MissingBit::Autoincrement);
  static_assert(!enum_flags_cast<MissingBit>(12).has_value());
  static_assert(enum_flags_cast<CompleteBits>(12) == CompleteBits::Autoincrement);
  static_assert(!enum_flags_cast<CompleteBits>("Autoincrement").has_value());
  CHECK(enum_flags_name(MissingBit::Autoincrement).empty());
  CHECK(enum_flags_name(CompleteBits::Autoincrement) == "Binary|Empty");
  // The compile-time form names any declared value regardless of the flags mode.
  static_assert(enum_name<CompleteBits::Autoincrement>() == "Autoincrement");
  CHECK(enum_name<as_flags<true>>(CompleteBits::Autoincrement).empty());
}

TEST_CASE("signed flags include the sign bit") {
  enum class Int8Flags : std::int8_t { Low = 1, Mid = 4, High = -128 };
  enum class Int64Flags : std::int64_t { Low = 1, High = std::numeric_limits<std::int64_t>::min() };
  constexpr auto flags = as_flags<true>;

  constexpr auto values = enum_values<Int8Flags, flags>();
  static_assert(values.size() == 3);
  static_assert(values[0] == Int8Flags::High);
  static_assert(values[1] == Int8Flags::Low);
  static_assert(values[2] == Int8Flags::Mid);
  static_assert(enum_min<Int8Flags, flags>() == Int8Flags::High);
  static_assert(enum_max<Int8Flags, flags>() == Int8Flags::Mid);
  static_assert(enum_index<Int8Flags, flags>(Int8Flags::High).value() == 0);
  static_assert(enum_value<Int8Flags, flags>(0) == Int8Flags::High);
  static_assert(enum_reflected<Int8Flags, flags>(std::int8_t{-128}));
  static_assert(enum_cast<Int8Flags, flags>(-128) == Int8Flags::High);
  static_assert(enum_flags_cast<Int8Flags>(-127) == static_cast<Int8Flags>(-127));
  static_assert(!enum_flags_cast<Int8Flags>(-126).has_value());
  static_assert(enum_flags_cast<Int8Flags>("Low|High") == static_cast<Int8Flags>(-127));
  CHECK(enum_name<Int8Flags, flags>(Int8Flags::High) == "High");
  CHECK(enum_flags_name(static_cast<Int8Flags>(-127)) == "High|Low");

  static_assert(enum_count<Int64Flags, flags>() == 2);
  static_assert(enum_values<Int64Flags, flags>()[0] == Int64Flags::High);
  static_assert(enum_flags_cast<Int64Flags>("High|Low") == static_cast<Int64Flags>(std::numeric_limits<std::int64_t>::min() + 1));
  CHECK(enum_flags_name(static_cast<Int64Flags>(std::numeric_limits<std::int64_t>::min() + 1)) == "High|Low");
}

TEST_CASE("unfixed flags reach the top bit of their range") {
  // The ranges are [0, 2^31 - 1] and [-2^30, 2^30 - 1], below the limits of the underlying type.
  enum UnfixedHigh { UH_Low = 1, UH_High = 1 << 30 };
  enum UnfixedSigned { US_Neg = -1, US_Low = 1, US_High = 1 << 29 };
  constexpr auto flags = as_flags<true>;

  static_assert(enum_count<UnfixedHigh, flags>() == 2);
  static_assert(enum_values<UnfixedHigh, flags>()[1] == UH_High);
  static_assert(enum_count<UnfixedSigned, flags>() == 2);
  static_assert(enum_values<UnfixedSigned, flags>()[1] == US_High);
  CHECK(enum_flags_name(UH_High) == "UH_High");
  CHECK(enum_flags_name(US_Low | US_High) == "US_Low|US_High");
}

TEST_CASE("flags name with a custom String") {
  using Name = FixedString<16>;
  static_assert(std::is_same_v<decltype(enum_flags_name(Color::RED)), std::string>);
  static_assert(std::is_same_v<decltype(enum_flags_name<Color, Name>(Color::RED)), Name>);
  static_assert(enum_flags_name<Color, Name>(Color::RED | Color::BLUE) == "RED|BLUE");
  static_assert(enum_flags_name<const Color&, Name>(Color::RED | Color::BLUE, ',') == "RED,BLUE");
  static_assert(enum_flags_name<ZeroFlags, Name>(ZeroFlags::None) == "None");
  static_assert(enum_flags_name<ZeroFlags, Name>(static_cast<ZeroFlags>(4)).empty());
  static_assert(enum_flags_name<Color, Name>(Color{0}).empty());
  static_assert(!enum_flags_name<Color, Name>(Color::RED | Color::GREEN).overflow);

  // Overflow belongs to String. This one drops the tail and records it.
  constexpr auto truncated = enum_flags_name<Color, FixedString<4>>(Color::RED | Color::GREEN);
  static_assert(truncated == "RED|" && truncated.overflow);

  auto runtime = Color::GREEN;
  runtime |= Color::BLUE;
  CHECK(enum_flags_name<Color, Name>(runtime) == "GREEN|BLUE");
  CHECK(enum_flags_name<Color, std::string>(runtime) == "GREEN|BLUE");
}

TEST_CASE("named zero is a flag value") {
  constexpr auto values = enum_values<ZeroFlags>();
  static_assert(values.size() == 3);
  static_assert(values[0] == ZeroFlags::None);
  static_assert(values[1] == ZeroFlags::A);
  static_assert(values[2] == ZeroFlags::B);
  static_assert(enum_index(ZeroFlags::None) == std::size_t{0});
  static_assert(enum_index(ZeroFlags::B) == std::size_t{2});
  static_assert(enum_value<ZeroFlags>(0) == ZeroFlags::None);
  static_assert(enum_value<ZeroFlags>(2) == ZeroFlags::B);
  static_assert(enum_min<ZeroFlags>() == ZeroFlags::None);
  static_assert(enum_contains(ZeroFlags::None));
  static_assert(!enum_contains(ZeroFlags::Both));
  static_assert(enum_name(ZeroFlags::None) == "None");
  CHECK(enum_flags_name(ZeroFlags::None) == "None");
  CHECK(enum_flags_name(ZeroFlags::Both) == "A|B");
  CHECK(enum_flags_name(ZeroFlags::A) == "A");
  CHECK(enum_flags_name(static_cast<ZeroFlags>(4)).empty());
  static_assert(enum_flags_cast<ZeroFlags>(0) == ZeroFlags::None);
  static_assert(enum_flags_cast<ZeroFlags>({}) == ZeroFlags::None);
  static_assert(enum_flags_cast<ZeroFlags>(3) == ZeroFlags::Both);
  static_assert(enum_flags_contains<ZeroFlags>(0));
  static_assert(enum_flags_contains<ZeroFlags>({}));
  static_assert(enum_flags_contains(ZeroFlags::None));
  static_assert(enum_flags_cast<ZeroFlags>("None") == ZeroFlags::None);
  static_assert(enum_flags_cast<ZeroFlags>("None|A") == ZeroFlags::A);
  static_assert(enum_flags_cast<ZeroFlags>("None|None") == ZeroFlags::None);
  static_assert(enum_flags_cast<ZeroFlags>("None|") == ZeroFlags::None);
  static_assert(!enum_flags_cast<ZeroFlags>("").has_value());
  static_assert(!enum_flags_cast<ZeroFlags>("|None").has_value());
  static_assert(!enum_flags_cast<ZeroFlags>("Both").has_value());
  static_assert(!enum_flags_test(ZeroFlags::Both, ZeroFlags::None));
  static_assert(!enum_flags_test(ZeroFlags::None, ZeroFlags::None));
  static_assert(!enum_flags_test_any(ZeroFlags::Both, ZeroFlags::None));
  static_assert(enum_switch<int>([](auto val) { return val() == ZeroFlags::None ? 1 : 2; }, ZeroFlags::None, -1) == 1);
  static_assert(enum_fuse(ZeroFlags::None, ZeroFlags::A) != enum_fuse(ZeroFlags::A, ZeroFlags::None));

  for (std::size_t i = 0; i < enum_count<ZeroFlags>(); ++i) {
    CHECK(enum_index(enum_value<ZeroFlags>(i)) == i);
  }
}

TEST_CASE("only named zero is a non-empty flag enum") {
  static_assert(enum_count<OnlyZeroFlags>() == 1);
  static_assert(enum_values<OnlyZeroFlags>()[0] == OnlyZeroFlags::None);
  static_assert(enum_index(OnlyZeroFlags::None) == std::size_t{0});
  static_assert(enum_value<OnlyZeroFlags>(0) == OnlyZeroFlags::None);
  CHECK(enum_flags_name(OnlyZeroFlags::None) == "None");
  CHECK(enum_flags_name(static_cast<OnlyZeroFlags>(1)).empty());
  static_assert(enum_flags_cast<OnlyZeroFlags>(0) == OnlyZeroFlags::None);
  static_assert(!enum_flags_cast<OnlyZeroFlags>(1).has_value());
  static_assert(enum_flags_cast<OnlyZeroFlags>("None") == OnlyZeroFlags::None);
}

TEST_CASE("named zero precedes the bits after the sign bit") {
  constexpr auto values = enum_values<SignedZeroFlags>();
  static_assert(values.size() == 3);
  static_assert(values[0] == SignedZeroFlags::High);
  static_assert(values[1] == SignedZeroFlags::None);
  static_assert(values[2] == SignedZeroFlags::Low);
  static_assert(enum_index(SignedZeroFlags::None) == std::size_t{1});
  static_assert(enum_value<SignedZeroFlags>(1) == SignedZeroFlags::None);
  static_assert(enum_flags_cast<SignedZeroFlags>(0) == SignedZeroFlags::None);
  CHECK(enum_flags_name(SignedZeroFlags::None) == "None");
  CHECK(enum_flags_name(static_cast<SignedZeroFlags>(-127)) == "High|Low");

  for (std::size_t i = 0; i < enum_count<SignedZeroFlags>(); ++i) {
    CHECK(enum_index(enum_value<SignedZeroFlags>(i)) == i);
  }
}

TEST_CASE("customization decides whether zero is a flag value") {
  // invalid_tag hides zero from every reflection path, not only from flags.
  static_assert(enum_count<HiddenZeroFlags>() == 2);
  static_assert(enum_values<HiddenZeroFlags>()[0] == HiddenZeroFlags::A);
  static_assert(!enum_flags_cast<HiddenZeroFlags>(0).has_value());
  static_assert(!enum_flags_cast<HiddenZeroFlags>("None").has_value());
  static_assert(enum_reflected<HiddenZeroFlags>(0));
  static_assert(!enum_cast<HiddenZeroFlags, as_common<>>(0).has_value());
  CHECK(enum_flags_name(HiddenZeroFlags::None).empty());
  CHECK(enum_name<as_common<>>(HiddenZeroFlags::None).empty());

  // A custom name renames a declared zero. An undeclared one is a compile-fail test.
  static_assert(enum_count<RenamedZeroFlags>() == 3);
  static_assert(enum_flags_cast<RenamedZeroFlags>("Nothing") == RenamedZeroFlags::None);
  CHECK(enum_flags_name(RenamedZeroFlags::None) == "Nothing");
}

TEST_CASE("flags string lookup keeps function pointer types") {
  using Pred = std::equal_to<>;
  constexpr optional<Color> (*cast)(string_view, char_type, Pred) noexcept = &enum_flags_cast<Color>;
  constexpr bool (*contains)(string_view, char_type, Pred) noexcept = &enum_flags_contains<Color>;
  REQUIRE(cast("RED|GREEN", '|', Pred{}) == (Color::RED | Color::GREEN));
  REQUIRE(contains("RED", '|', Pred{}));
  REQUIRE_FALSE(contains("None", '|', Pred{}));
}

TEST_CASE("braced flags arguments preserve zero behavior") {
  constexpr auto none = enum_flags_cast<Numbers>({});
  REQUIRE(none == Numbers::none);
}

TEST_CASE("integer flags cast checks the value before narrowing") {
  enum class U8Flags : std::uint8_t { A = 1, B = 2 };
  enum class I8Flags : std::int8_t { A = 1, High = -128 };

  static_assert(enum_flags_cast<U8Flags>(3) == static_cast<U8Flags>(3));
  static_assert(!enum_flags_cast<U8Flags>(257).has_value());
  static_assert(!enum_flags_contains<U8Flags>(258));
  static_assert(enum_flags_cast<I8Flags>(-127) == static_cast<I8Flags>(-127));
  static_assert(!enum_flags_cast<I8Flags>(129).has_value());
  static_assert(!enum_flags_cast<BoolFlags>(2).has_value());
  static_assert(!enum_flags_contains<BoolFlags>(2));

  // Wider arguments keep their sign, so INT64_MIN does not become bit 63.
  enum class Wide : std::uint64_t { Low = 1, High = std::uint64_t{1} << 63 };
  static_assert(enum_flags_cast<Wide>(std::uint64_t{1} << 63) == Wide::High);
  static_assert(!enum_flags_cast<Wide>((std::numeric_limits<std::int64_t>::min)()).has_value());
  static_assert(!enum_flags_contains<Wide>(-1));
  static_assert(!enum_flags_cast<I8Flags>((std::numeric_limits<std::uint64_t>::max)()).has_value());
  static_assert(enum_flags_cast<I8Flags>(std::int64_t{-128}) == I8Flags::High);
}

TEST_CASE("enum_cast") {
  SUBCASE("noncopyable predicate is shared across flag names") {
    struct Predicate {
      int calls = 0;
      Predicate() = default;
      Predicate(const Predicate&) = delete;
      bool operator()(const char& lhs, const char& rhs) & noexcept {
        ++calls;
        return lhs == rhs;
      }
    };
    Predicate predicate;
    REQUIRE(enum_flags_cast<Color, Predicate&>("RED|BLUE", '|', predicate) == (Color::RED | Color::BLUE));
    REQUIRE(predicate.calls > 0);
  }

  SUBCASE("colliding flag names") {
    enum class Flags { costarring = 1, liquid = 2 };
    static_assert(enum_flags_cast<Flags>("costarring") == Flags::costarring);
    static_assert(enum_flags_cast<Flags>("liquid") == Flags::liquid);
    static_assert(enum_flags_cast<Flags>("costarring|liquid") == static_cast<Flags>(3));
    REQUIRE(enum_flags_cast<Flags>("liquid|costarring|liquid") == static_cast<Flags>(3));
    REQUIRE_FALSE(enum_flags_cast<Flags>("liquids").has_value());
  }

  SUBCASE("predicates receive character references") {
    static_assert(enum_flags_cast<Color>("RED|GREEN", '|', ConstReferencePredicate{}) == (Color::RED | Color::GREEN));
    static_assert(enum_flags_contains<Color>("RED|GREEN", '|', ConstReferencePredicate{}));
    static_assert(!noexcept(enum_flags_cast<Color>("RED", '|', ThrowingReferencePredicate{})));
    static_assert(!noexcept(enum_flags_contains<Color>("RED", '|', ThrowingReferencePredicate{})));
    REQUIRE_THROWS_AS(static_cast<void>(enum_flags_cast<Color>("RED", '|', ThrowingReferencePredicate{})), int);
    REQUIRE_THROWS_AS(static_cast<void>(enum_flags_contains<Color>("RED", '|', ThrowingReferencePredicate{})), int);
  }

  SUBCASE("string") {
    constexpr auto cr = enum_cast<Color>("RED");
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(enum_cast<Color&>("GREEN").value() == Color::GREEN);
    REQUIRE(enum_cast<Color>("blue", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).value() == Color::BLUE);
    REQUIRE_FALSE(enum_cast<Color&>("blue|RED", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).has_value());
    REQUIRE_FALSE(enum_cast<Color&>("GREEN|RED").has_value());
    REQUIRE_FALSE(enum_cast<Color&>("GREEN|RED|RED").has_value());
    REQUIRE_FALSE(enum_cast<Color&>("GREEN|RED|None").has_value());
    REQUIRE_FALSE(enum_cast<Color>("None").has_value());

    REQUIRE(enum_flags_cast<Color&>("GREEN").value() == Color::GREEN);
    REQUIRE(enum_flags_cast<Color>("blue", '|', [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).value() == Color::BLUE);
    REQUIRE(enum_flags_cast<Color&>("blue|RED", '|', [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).value() == (Color::BLUE | Color::RED));
    REQUIRE(enum_flags_cast<Color&>("GREEN|RED").value() == (Color::GREEN | Color::RED));
    constexpr auto constexpr_repeated_flags = enum_flags_cast<Color&>("GREEN|RED|RED");
    static_assert(constexpr_repeated_flags.has_value());
    static_assert(*constexpr_repeated_flags == (Color::GREEN | Color::RED));
    REQUIRE(enum_flags_cast<Color&>("GREEN|RED|RED").value() == (Color::GREEN | Color::RED));
    REQUIRE(enum_flags_cast<Color&>("GREEN|").value() == Color::GREEN);
    REQUIRE_FALSE(enum_flags_cast<Color&>("|GREEN").has_value());
    REQUIRE_FALSE(enum_flags_cast<Color&>("GREEN||RED").has_value());
    REQUIRE_FALSE(enum_flags_cast<Color&>("GREEN|RED|None").has_value());
    REQUIRE_FALSE(enum_flags_cast<Color>("None").has_value());

    REQUIRE(enum_flags_cast<CStyleFlags>("A|B|C|D").value() == (CStyleFlags_A | CStyleFlags_B | CStyleFlags_C | CStyleFlags_D));
    REQUIRE(enum_flags_cast<CStyleFlags>("a|e|f", '|', [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).value() == (CStyleFlags_A | CStyleFlags_E | CStyleFlags_F));
    REQUIRE_FALSE(enum_flags_cast<CStyleFlags>("blue|E|F|C", '|', [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).has_value());
    REQUIRE(enum_flags_cast<CStyleFlags>("H|I|F|F|F").value() == (CStyleFlags_H | CStyleFlags_I | CStyleFlags_F));
    REQUIRE(enum_flags_cast<CStyleFlags>("E|B|C|A").value() == (CStyleFlags_A | CStyleFlags_B | CStyleFlags_C | CStyleFlags_E));

    // Custom separator.
    REQUIRE(enum_flags_cast<Color>("GREEN,RED", ',').value() == (Color::GREEN | Color::RED));
    REQUIRE(enum_flags_cast<Color>("GREEN,RED,BLUE", ',').value() == (Color::GREEN | Color::RED | Color::BLUE));
    REQUIRE(enum_flags_cast<Color>("red,green", ',', [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).value() == (Color::RED | Color::GREEN));
    REQUIRE_FALSE(enum_flags_cast<Color>("GREEN,RED,None", ',').has_value());
    REQUIRE(enum_flags_cast<CStyleFlags>("A,B,C", ',').value() == (CStyleFlags_A | CStyleFlags_B | CStyleFlags_C));

    constexpr auto no = enum_cast<Numbers>("one");
    REQUIRE(no.value() == Numbers::one);
    REQUIRE(enum_cast<Numbers>("two").value() == Numbers::two);
    REQUIRE(enum_cast<Numbers>("three").value() == Numbers::three);
    REQUIRE(enum_cast<Numbers>("many") == Numbers::many);
    REQUIRE_FALSE(enum_cast<Numbers>("None").has_value());

    constexpr auto dr = enum_cast<Directions>("Right");
    REQUIRE(enum_cast<Directions&>("Up").value() == Directions::Up);
    REQUIRE(enum_cast<const Directions>("Down").value() == Directions::Down);
    REQUIRE(dr.value() == Directions::Right);
    REQUIRE(enum_cast<Directions>("Left").value() == Directions::Left);
    REQUIRE_FALSE(enum_cast<Directions>("None").has_value());

    constexpr auto nto = enum_flags_cast<number>("three|one");
    REQUIRE(enum_cast<number>("one").value() == number::one);
    REQUIRE(enum_cast<number>("two").value() == number::two);
    REQUIRE(enum_cast<number>("three").value() == number::three);
    REQUIRE(enum_cast<number>("four") == number::four);
    REQUIRE(nto.value() == (number::three | number::one));
    REQUIRE_FALSE(enum_cast<number>("None").has_value());
  }

  SUBCASE("integer") {
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    constexpr auto cr = enum_cast<Color>(1);
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(enum_cast<Color&>(2).value() == Color::GREEN);
    REQUIRE(enum_cast<Color>(static_cast<int>(cm[2])).value() == Color::BLUE);
    REQUIRE_FALSE(enum_cast<Color>(1 | 2).has_value());
    REQUIRE_FALSE(enum_cast<Color>(1 | 2 | 1).has_value());
    REQUIRE_FALSE(enum_cast<Color>(1 | 2 | 8).has_value());
    REQUIRE_FALSE(enum_cast<Color>(0).has_value());

    REQUIRE(enum_flags_cast<Color&>(2).value() == Color::GREEN);
    REQUIRE(enum_flags_cast<Color>(static_cast<int>(cm[2])).value() == Color::BLUE);
    REQUIRE(enum_flags_cast<Color>(1 | 2).value() == (Color::GREEN | Color::RED));
    REQUIRE(enum_flags_cast<Color>(1 | 2 | 1).value() == (Color::GREEN | Color::RED));
    REQUIRE_FALSE(enum_flags_cast<Color>(1 | 2 | 8).has_value());
    REQUIRE_FALSE(enum_flags_cast<Color>(0).has_value());
    REQUIRE_FALSE(enum_flags_cast<number>(1 | 2).has_value());
    REQUIRE_FALSE(enum_flags_contains<number>(1 | 2));

    constexpr auto no = enum_cast<Numbers>(2);
    REQUIRE(no.value() == Numbers::one);
    REQUIRE(enum_cast<Numbers>(4).value() == Numbers::two);
    REQUIRE(enum_cast<Numbers>(8).value() == Numbers::three);
    REQUIRE(enum_cast<Numbers>(1 << 30).value() == Numbers::many);
    REQUIRE_FALSE(enum_cast<Numbers>(127).has_value());
    REQUIRE(enum_cast<Numbers>(0) == Numbers::none);

    constexpr auto dr = enum_cast<Directions>(std::uint64_t{1} << 63);
    REQUIRE(enum_cast<Directions&>(std::uint64_t{1} << 31).value() == Directions::Up);
    REQUIRE(enum_cast<const Directions>(std::uint64_t{1} << 20).value() == Directions::Down);
    REQUIRE(dr.value() == Directions::Right);
    REQUIRE(enum_cast<Directions>(std::uint64_t{1} << 10).value() == Directions::Left);
    REQUIRE(enum_cast<Directions>(0) == Directions::NoDirection);

    constexpr auto nto = enum_flags_cast<number>(2 | 8);
    REQUIRE(enum_cast<number>(1 << 1).value() == number::one);
    REQUIRE(enum_cast<number>(1 << 2).value() == number::two);
    REQUIRE(enum_cast<number>(1 << 3).value() == number::three);
    REQUIRE(enum_cast<number>(1 << 4).value() == number::four);
    REQUIRE(nto.value() == (number::three | number::one));
    REQUIRE(enum_cast<number>(0) == number::no_number);
  }
}

TEST_CASE("enum_index") {
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  constexpr auto cr = enum_index(Color::RED);
  Color cg = Color::GREEN;
  REQUIRE(cr.value() == 0);
  REQUIRE(enum_index<as_flags<>>(Color::GREEN).value() == 1);
  REQUIRE(enum_index<Color&>(cg).value() == 1);
  REQUIRE(enum_index(cm[2]).value() == 2);
  REQUIRE_FALSE(enum_index<Color>(Color::RED | Color::GREEN).has_value());
  REQUIRE_FALSE(enum_index<Color>(Color::RED | Color::GREEN | Color::RED).has_value());
  REQUIRE_FALSE(enum_index<Color>(Color::RED | Color{8}).has_value());
  REQUIRE_FALSE(enum_index(static_cast<Color>(0)).has_value());

  constexpr auto no = enum_index(Numbers::one);
  REQUIRE(no.value() == 1);
  REQUIRE(enum_index(Numbers::two).value() == 2);
  REQUIRE(enum_index(Numbers::three).value() == 3);
  REQUIRE(enum_index(Numbers::many).value() == 4);
  REQUIRE(enum_index(Numbers::none).value() == 0);

  constexpr auto dr = enum_index(Directions::Right);
  Directions dl = Directions::Left;
  REQUIRE(enum_index<Directions&>(dl).value() == 1);
  REQUIRE(enum_index<const Directions>(Directions::Down).value() == 2);
  REQUIRE(enum_index(Directions::Up).value() == 3);
  REQUIRE(dr.value() == 4);
  REQUIRE(enum_index(Directions::NoDirection).value() == 0);

  constexpr auto nto = enum_index(number::three | number::one);
  REQUIRE(enum_index(number::one).value() == 1);
  REQUIRE(enum_index(number::two).value() == 2);
  REQUIRE(enum_index(number::three).value() == 3);
  REQUIRE(enum_index(number::four).value() == 4);
  REQUIRE_FALSE(nto.has_value());
  REQUIRE(enum_index(number::no_number).value() == 0);
}

TEST_CASE("enum_contains") {
  SUBCASE("value") {
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    constexpr auto cr = enum_contains(Color::RED);
    Color cg = Color::GREEN;
    REQUIRE(cr);
    REQUIRE(enum_contains<as_flags<>>(Color::GREEN));
    REQUIRE(enum_contains<Color&>(cg));
    REQUIRE(enum_contains(cm[2]));
    REQUIRE_FALSE(enum_contains(static_cast<Color>(0)));

    REQUIRE(enum_flags_contains<Color&>(cg));
    REQUIRE(enum_flags_contains(cm[2]));
    REQUIRE(enum_flags_contains<Color>(Color::RED | Color::GREEN));
    REQUIRE(enum_flags_contains<Color>(Color::RED | Color::GREEN | Color::GREEN));
    REQUIRE_FALSE(enum_flags_contains<Color>(Color::RED | Color{8}));
    REQUIRE_FALSE(enum_flags_contains(static_cast<Color>(0)));

    constexpr auto no = enum_contains(Numbers::one);
    REQUIRE(no);
    REQUIRE(enum_contains(Numbers::two));
    REQUIRE(enum_contains(Numbers::three));
    REQUIRE(enum_contains(Numbers::many));
    REQUIRE(enum_contains(static_cast<Numbers>(0)));

    constexpr auto dr = enum_contains(Directions::Right);
    Directions dl = Directions::Left;
    REQUIRE(enum_contains<Directions&>(dl));
    REQUIRE(enum_contains<const Directions>(Directions::Down));
    REQUIRE(enum_contains(Directions::Up));
    REQUIRE(dr);
    REQUIRE(enum_contains(static_cast<Directions>(0)));

    constexpr auto nto = enum_contains(number::three | number::one);
    REQUIRE(enum_contains(number::one));
    // A reference E binds the enum value overload, so the argument must be an lvalue.
    number nv = number::two;
    REQUIRE(enum_contains<number&>(nv));
    REQUIRE(enum_contains(number::one));
    REQUIRE(enum_contains(number::four));
    REQUIRE_FALSE(nto);
    REQUIRE(enum_contains(static_cast<number>(0)));

    REQUIRE(enum_flags_contains(number::three | number::one));
  }

  SUBCASE("integer") {
    REQUIRE(enum_contains<Color>(1));
    REQUIRE(enum_contains<Color&>(2));
    REQUIRE(enum_contains<const Color>(4));
    REQUIRE_FALSE(enum_contains<Color>(0));

    REQUIRE(enum_flags_contains<Color>(1));
    REQUIRE(enum_flags_contains<Color&>(2));
    REQUIRE(enum_flags_contains<Color>(4));
    REQUIRE(enum_flags_contains<Color>(1 | 2));
    REQUIRE(enum_flags_contains<Color>(1 | 2 | 1));
    REQUIRE_FALSE(enum_flags_contains<Color>(1 | 2 | 8));
    REQUIRE_FALSE(enum_flags_contains<Color>(0));

    constexpr auto no = enum_contains<Numbers>(1 << 1);
    REQUIRE(no);
    REQUIRE(enum_contains<Numbers>(1 << 2));
    REQUIRE(enum_contains<Numbers>(1 << 3));
    REQUIRE(enum_contains<Numbers>(1 << 30));

    constexpr auto dr = enum_contains<Directions&>(std::uint64_t{1} << 63);
    REQUIRE(dr);
    REQUIRE(enum_contains<const Directions>(std::uint64_t{1} << 10));
    REQUIRE(enum_contains<Directions>(std::uint64_t{1} << 20));
    REQUIRE(enum_contains<Directions>(std::uint64_t{1} << 31));
    REQUIRE(enum_contains<Directions>(static_cast<Directions>(0)));

    constexpr auto nto = enum_contains<number>(8 | 2);
    REQUIRE(enum_contains<number>(1 << 1));
    REQUIRE(enum_contains<number>(1 << 2));
    REQUIRE(enum_contains<number>(1 << 3));
    REQUIRE(enum_contains<number>(1 << 4));
    REQUIRE_FALSE(enum_contains<number>(8 | 2 | 16));
    REQUIRE_FALSE(enum_contains<number>(8 | 16 | 16));
    REQUIRE_FALSE(nto);
    REQUIRE_FALSE(enum_contains<number>(8 | 64));
    REQUIRE(enum_contains<number>(0));
  }

  SUBCASE("string") {
    constexpr auto cr = "RED";
    REQUIRE(enum_contains<Color>(cr));
    REQUIRE(enum_contains<Color&>("GREEN"));
    REQUIRE(enum_contains<const Color>("blue", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }));
    REQUIRE_FALSE(enum_contains<Color&>("blue|RED", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }));
    REQUIRE_FALSE(enum_contains<Color&>("GREEN|RED"));
    REQUIRE_FALSE(enum_contains<Color&>("GREEN|RED|RED"));
    REQUIRE_FALSE(enum_contains<Color>("GREEN|RED|None"));
    REQUIRE_FALSE(enum_contains<Color>("None"));

    REQUIRE(enum_flags_contains<Color&>("GREEN"));
    REQUIRE(enum_flags_contains<Color>("GREEN", '|', LvalueOnlyPredicate{}));
    REQUIRE(enum_flags_contains<Color>("blue", '|', [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }));
    REQUIRE(enum_flags_contains<Color>("blue|RED", '|', [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }));
    REQUIRE(enum_flags_contains<Color>("GREEN|RED"));
    static_assert(enum_flags_contains<Color>("GREEN|RED|RED"));
    REQUIRE(enum_flags_contains<Color>("GREEN|RED|RED"));
    REQUIRE_FALSE(enum_flags_contains<Color>("GREEN|RED|None"));
    REQUIRE_FALSE(enum_flags_contains<Color>("None"));

    // Custom separator.
    REQUIRE(enum_flags_contains<Color>("GREEN,RED", ','));
    REQUIRE(enum_flags_contains<Color>("green,red", ',', [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }));
    REQUIRE_FALSE(enum_flags_contains<Color>("GREEN,RED,None", ','));

    constexpr auto no = std::string_view{"one"};
    REQUIRE(enum_contains<Numbers>(no));
    REQUIRE(enum_contains<Numbers>("two"));
    REQUIRE(enum_contains<Numbers>("three"));
    REQUIRE(enum_contains<Numbers>("many"));
    REQUIRE_FALSE(enum_contains<Numbers>("None"));

    auto dr = std::string{"Right"};
    REQUIRE(enum_contains<Directions&>("Up"));
    REQUIRE(enum_contains<Directions>("Down"));
    REQUIRE(enum_contains<const Directions>(dr));
    REQUIRE(enum_contains<Directions>("Left"));
    REQUIRE_FALSE(enum_contains<Directions>("None"));

    constexpr auto nto = enum_contains<number>("three|one");
    REQUIRE(enum_contains<number>("one"));
    REQUIRE(enum_contains<number>("two"));
    REQUIRE(enum_contains<number>("three"));
    REQUIRE(enum_contains<number>("four"));
    REQUIRE_FALSE(nto);
    REQUIRE_FALSE(enum_contains<number>("None"));

    REQUIRE(enum_flags_contains<number>("three|one"));
  }
}

TEST_CASE("enum_value") {
  constexpr auto cr = enum_value<Color>(0);
  REQUIRE(cr == Color::RED);
  REQUIRE(enum_value<Color&>(1) == Color::GREEN);
  REQUIRE(enum_value<Color>(2) == Color::BLUE);

  REQUIRE(enum_value<Color, 0>() == Color::RED);
  REQUIRE(enum_value<Color, 1>() == Color::GREEN);
  REQUIRE(enum_value<Color, 2>() == Color::BLUE);

  REQUIRE(enum_value<Numbers>(0) == Numbers::none);
  constexpr auto no = enum_value<Numbers>(1);
  REQUIRE(no == Numbers::one);
  REQUIRE(enum_value<Numbers>(2) == Numbers::two);
  REQUIRE(enum_value<Numbers>(3) == Numbers::three);
  REQUIRE(enum_value<Numbers>(4) == Numbers::many);

  REQUIRE(enum_value<Numbers, 1>() == Numbers::one);
  REQUIRE(enum_value<Numbers, 2>() == Numbers::two);
  REQUIRE(enum_value<Numbers, 3>() == Numbers::three);
  REQUIRE(enum_value<Numbers, 4>() == Numbers::many);

  REQUIRE(enum_value<Directions>(0) == Directions::NoDirection);
  constexpr auto dr = enum_value<Directions>(4);
  REQUIRE(enum_value<Directions&>(1) == Directions::Left);
  REQUIRE(enum_value<const Directions>(2) == Directions::Down);
  REQUIRE(enum_value<Directions>(3) == Directions::Up);
  REQUIRE(dr == Directions::Right);

  REQUIRE(enum_value<Directions, 1>() == Directions::Left);
  REQUIRE(enum_value<Directions, 2>() == Directions::Down);
  REQUIRE(enum_value<Directions, 3>() == Directions::Up);
  REQUIRE(enum_value<Directions, 4>() == Directions::Right);

  REQUIRE(enum_value<number>(0) == number::no_number);
  constexpr auto nt = enum_value<number>(3);
  REQUIRE(enum_value<number>(1) == number::one);
  REQUIRE(enum_value<number>(2) == number::two);
  REQUIRE(nt == number::three);
  REQUIRE(enum_value<number>(4) == number::four);

  REQUIRE(enum_value<number, 1>() == number::one);
  REQUIRE(enum_value<number, 2>() == number::two);
  REQUIRE(enum_value<number, 3>() == number::three);
  REQUIRE(enum_value<number, 4>() == number::four);
}

TEST_CASE("enum_values") {
  REQUIRE(std::is_same_v<decltype(enum_values<Color>()), const std::array<Color, 3>&>);

  constexpr auto& s1 = enum_values<Color&>();
  REQUIRE(s1 == std::array<Color, 3>{{Color::RED, Color::GREEN, Color::BLUE}});

  constexpr auto& s2 = enum_values<Numbers>();
  REQUIRE(s2 == std::array<Numbers, 5>{{Numbers::none, Numbers::one, Numbers::two, Numbers::three, Numbers::many}});

  constexpr auto& s3 = enum_values<const Directions>();
  REQUIRE(s3 == std::array<Directions, 5>{{Directions::NoDirection, Directions::Left, Directions::Down, Directions::Up, Directions::Right}});

  constexpr auto& s4 = enum_values<number>();
  REQUIRE(s4 == std::array<number, 5>{{number::no_number, number::one, number::two, number::three, number::four}});
}

TEST_CASE("enum_count") {
  constexpr auto s1 = enum_count<Color&>();
  REQUIRE(s1 == 3);

  constexpr auto s2 = enum_count<Numbers>();
  REQUIRE(s2 == 5);

  constexpr auto s3 = enum_count<const Directions>();
  REQUIRE(s3 == 5);

  constexpr auto s4 = enum_count<number>();
  REQUIRE(s4 == 5);
}

TEST_CASE("enum_min_max") {
  constexpr auto color_min = enum_min<Color&>();
  constexpr auto color_max = enum_max<const Color, as_flags<>>();
  REQUIRE(color_min == Color::RED);
  REQUIRE(color_max == Color::BLUE);

  REQUIRE(enum_min<Directions>() == Directions::NoDirection);
  REQUIRE(enum_max<Directions>() == Directions::Right);
  REQUIRE(enum_min<number>() == number::no_number);
  REQUIRE(enum_max<number>() == number::four);
  REQUIRE(enum_min<number, as_common<>>() == number::no_number);
  REQUIRE(enum_max<number, as_common<>>() == number::four);
  REQUIRE(enum_min<BoolFlags>() == BoolFlags::Disabled);
  REQUIRE(enum_max<BoolFlags>() == BoolFlags::Enabled);
}

TEST_CASE("enum_name") {
  SUBCASE("automatic storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = enum_name(cr);
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    Color cb = Color::BLUE;
    REQUIRE(cr_name == "RED");
    REQUIRE(enum_name<as_flags<>>(Color::GREEN) == "GREEN");
    REQUIRE(enum_name<Color&>(cb) == "BLUE");
    REQUIRE(enum_name(cm[1]) == "GREEN");
    REQUIRE(enum_name(Color::RED | Color{0}) == "RED");
    REQUIRE(enum_name(Color::RED | Color::GREEN).empty());
    REQUIRE(enum_name(Color::RED | Color{8}).empty());
    REQUIRE(enum_name(static_cast<Color>(0)).empty());

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = enum_name(no);
    REQUIRE(no_name == "one");
    REQUIRE(enum_name(Numbers::two) == "two");
    REQUIRE(enum_name(Numbers::three) == "three");
    REQUIRE(enum_name(Numbers::many) == "many");
    REQUIRE(enum_name(Numbers::many | Numbers::two).empty());
    REQUIRE(enum_name(static_cast<Numbers>(0)) == "none");

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = enum_name(dr);
    Directions du = Directions::Up;
    REQUIRE(enum_name<Directions&>(du) == "Up");
    REQUIRE(enum_name<const Directions>(Directions::Down) == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(enum_name(Directions::Left) == "Left");
    REQUIRE(enum_name(Directions::Right | Directions::Up | Directions::Left | Directions::Down).empty());
    REQUIRE(enum_name(static_cast<Directions>(0)) == "NoDirection");

    constexpr number nto = number::three | number::one;
    constexpr auto nto_name = enum_name(nto);
    REQUIRE(enum_name(number::one) == "one");
    REQUIRE(enum_name(number::two) == "two");
    REQUIRE(enum_name(number::three) == "three");
    REQUIRE(enum_name(number::four) == "four");
    REQUIRE(nto_name.empty());
    REQUIRE(enum_name(static_cast<number>(0)) == "no_number");
  }
}

TEST_CASE("enum_flags_name") {
  constexpr Color cr = Color::RED;
  auto cr_name = enum_flags_name(cr);
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  Color cb = Color::BLUE;
  REQUIRE(cr_name == "RED");
  REQUIRE(enum_flags_name<Color&>(cb) == "BLUE");
  REQUIRE(enum_flags_name(cm[1]) == "GREEN");
  REQUIRE(enum_flags_name(Color::RED | Color{0}) == "RED");
  REQUIRE(enum_flags_name(Color::RED | Color::GREEN) == "RED|GREEN");
  REQUIRE(enum_flags_name(Color::RED | Color{8}).empty());
  REQUIRE(enum_flags_name(static_cast<Color>(0)).empty());

  constexpr Numbers no = Numbers::one;
  auto no_name = enum_flags_name(no);
  REQUIRE(no_name == "one");
  REQUIRE(enum_flags_name(Numbers::two) == "two");
  REQUIRE(enum_flags_name(Numbers::three) == "three");
  REQUIRE(enum_flags_name(Numbers::many) == "many");
  REQUIRE(enum_flags_name(Numbers::many | Numbers::two) == "two|many");
  REQUIRE(enum_flags_name(static_cast<Numbers>(0)) == "none");

  constexpr Directions dr = Directions::Right;
  auto dr_name = enum_flags_name(dr);
  Directions du = Directions::Up;
  REQUIRE(enum_flags_name<Directions&>(du) == "Up");
  REQUIRE(enum_flags_name<const Directions>(Directions::Down) == "Down");
  REQUIRE(dr_name == "Right");
  REQUIRE(enum_flags_name(Directions::Left) == "Left");
  REQUIRE(enum_flags_name(Directions::Right | Directions::Up | Directions::Left | Directions::Down) == "Left|Down|Up|Right");
  REQUIRE(enum_flags_name(static_cast<Directions>(0)) == "NoDirection");

  constexpr number nto = number::three | number::one;
  auto nto_name = enum_flags_name(nto);
  REQUIRE(enum_flags_name(number::one) == "one");
  REQUIRE(enum_flags_name(number::two) == "two");
  REQUIRE(enum_flags_name(number::three) == "three");
  REQUIRE(enum_flags_name(number::four) == "four");
  REQUIRE(nto_name == "one|three");
  REQUIRE(enum_flags_name(static_cast<number>(0)) == "no_number");

  REQUIRE(enum_flags_name(CStyleFlags_A | CStyleFlags_B | CStyleFlags_C | CStyleFlags_D) == "A|B|C|D");

  // Custom separator.
  REQUIRE(enum_flags_name(Color::RED | Color::GREEN, ',') == "RED,GREEN");
  REQUIRE(enum_flags_name(Color::RED | Color::GREEN | Color::BLUE, ',') == "RED,GREEN,BLUE");
  REQUIRE(enum_flags_name(Numbers::many | Numbers::two, ',') == "two,many");
  REQUIRE(enum_flags_name(Directions::Right | Directions::Up | Directions::Left | Directions::Down, ',') == "Left,Down,Up,Right");
  REQUIRE(enum_flags_name(CStyleFlags_A | CStyleFlags_B | CStyleFlags_C, ',') == "A,B,C");
}

TEST_CASE("enum_names") {
  REQUIRE(std::is_same_v<decltype(enum_names<Color>()), const std::array<std::string_view, 3>&>);

  constexpr auto& s1 = enum_names<Color&>();
  REQUIRE(s1 == std::array<std::string_view, 3>{{"RED", "GREEN", "BLUE"}});

  constexpr auto& s2 = enum_names<Numbers>();
  REQUIRE(s2 == std::array<std::string_view, 5>{{"none", "one", "two", "three", "many"}});

  constexpr auto& s3 = enum_names<const Directions>();
  REQUIRE(s3 == std::array<std::string_view, 5>{{"NoDirection", "Left", "Down", "Up", "Right"}});

  constexpr auto& s4 = enum_names<number>();
  REQUIRE(s4 == std::array<std::string_view, 5>{{"no_number", "one", "two", "three", "four"}});
}

TEST_CASE("enum_entries") {
  REQUIRE(std::is_same_v<decltype(enum_entries<Color>()), const std::array<std::pair<Color, std::string_view>, 3>&>);

  constexpr auto& s1 = enum_entries<Color&>();
  REQUIRE(s1 == std::array<std::pair<Color, std::string_view>, 3>{{{Color::RED, "RED"}, {Color::GREEN, "GREEN"}, {Color::BLUE, "BLUE"}}});

  constexpr auto& s2 = enum_entries<Numbers>();
  REQUIRE(s2 == std::array<std::pair<Numbers, std::string_view>, 5>{{{Numbers::none, "none"}, {Numbers::one, "one"}, {Numbers::two, "two"}, {Numbers::three, "three"}, {Numbers::many, "many"}}});

  constexpr auto& s3 = enum_entries<Directions&>();
  REQUIRE(s3 == std::array<std::pair<Directions, std::string_view>, 5>{{{Directions::NoDirection, "NoDirection"}, {Directions::Left, "Left"}, {Directions::Down, "Down"}, {Directions::Up, "Up"}, {Directions::Right, "Right"}}});

  constexpr auto& s4 = enum_entries<number>();
  REQUIRE(s4 == std::array<std::pair<number, std::string_view>, 5>{{{number::no_number, "no_number"}, {number::one, "one"}, {number::two, "two"}, {number::three, "three"}, {number::four, "four"}}});
}

TEST_CASE("flags string lifetime and null termination") {
  std::string_view value_name{};
  value_name = enum_name(Color::RED);
  require_null_terminated(value_name, "RED");

  std::string_view static_name{};
  static_name = enum_name<Color::BLUE>();
  require_null_terminated(static_name, "BLUE");

  std::string_view invalid_name{};
  invalid_name = enum_name(Color::RED | Color::GREEN);
  require_null_terminated(invalid_name, "");

  std::string_view array_name{};
  array_name = enum_names<Color>()[1];
  require_null_terminated(array_name, "GREEN");

  std::string_view entry_name{};
  entry_name = enum_entries<Directions>()[4].second;
  require_null_terminated(entry_name, "Right");

  std::string_view prefixed_name{};
  prefixed_name = enum_name(CStyleFlags_A);
  require_null_terminated(prefixed_name, "A");

  std::string_view prefixed_entry_name{};
  prefixed_entry_name = enum_entries<CStyleFlags>()[0].second;
  require_null_terminated(prefixed_entry_name, "A");

  auto flags_name = enum_flags_name(Color::RED | Color::GREEN);
  require_c_str_null_terminated(flags_name, "RED|GREEN");

  auto empty_flags_name = enum_flags_name(static_cast<Color>(0));
  require_c_str_null_terminated(empty_flags_name, "");

  auto prefixed_flags_name = enum_flags_name(CStyleFlags_A | CStyleFlags_C);
  require_c_str_null_terminated(prefixed_flags_name, "A|C");

  for (std::string_view name : enum_names<Color>()) {
    require_null_terminated(name);
  }
  for (std::string_view name : enum_names<Numbers>()) {
    require_null_terminated(name);
  }
  for (const auto& entry : enum_entries<Color>()) {
    require_null_terminated(entry.second);
  }
  for (const auto& entry : enum_entries<Directions>()) {
    require_null_terminated(entry.second);
  }
  for (std::string_view name : enum_names<CStyleFlags>()) {
    require_null_terminated(name);
  }
  for (const auto& entry : enum_entries<CStyleFlags>()) {
    require_null_terminated(entry.second);
  }
}

TEST_CASE("type_traits") {
  static_assert(!detail::is_reflected_v<int, detail::enum_subtype::common>);
  static_assert(detail::subtype_v<Color> == detail::enum_subtype::flags);
  static_assert(detail::subtype_v<const Color&> == detail::enum_subtype::flags);
  static_assert(detail::subtype_v<Numbers> == detail::enum_subtype::flags);
  REQUIRE(is_flags_enum<Color>::value);
  REQUIRE(is_flags_enum_v<const Color>);
  REQUIRE_FALSE(is_flags_enum_v<Color&>);
  REQUIRE(is_flags_enum_v<Numbers>);
  REQUIRE(is_flags_enum_v<Directions>);
  REQUIRE(is_flags_enum_v<number>);
}

TEST_CASE("enum_flags_test") {
  REQUIRE(enum_flags_test(Color::RED|Color::GREEN, Color::RED));
  REQUIRE_FALSE(enum_flags_test(Color::RED|Color::GREEN, Color::BLUE));

  REQUIRE_FALSE(enum_flags_test(Numbers::none, Numbers::none));
  REQUIRE_FALSE(enum_flags_test(Numbers::none, Numbers::one));
  REQUIRE(enum_flags_test(Numbers::one|Numbers::two|Numbers::many, Numbers::many));
  REQUIRE_FALSE(enum_flags_test(Numbers::one|Numbers::two|Numbers::many, Numbers::three));
  REQUIRE_FALSE(enum_flags_test(Numbers::one|Numbers::two|Numbers::many, Numbers::none));

  REQUIRE_FALSE(enum_flags_test(Left|Right, NoDirection));
  REQUIRE(enum_flags_test(Left|Right|Up|Down, Right));
  REQUIRE_FALSE(enum_flags_test(Left|Up|Down, Right));

  REQUIRE(enum_flags_test(number::one|number::two|number::four, number::one));
  REQUIRE_FALSE(enum_flags_test(number::one|number::two|number::four, number::three));
  REQUIRE_FALSE(enum_flags_test(number::one|number::two|number::four, number::no_number));
}

TEST_CASE("enum_flags_test_any") {
  REQUIRE(enum_flags_test_any(Color::RED|Color::GREEN, Color::RED));
  REQUIRE_FALSE(enum_flags_test_any(Color::RED|Color::GREEN, Color::BLUE));

  REQUIRE_FALSE(enum_flags_test_any(Numbers::none, Numbers::none));
  REQUIRE_FALSE(enum_flags_test_any(Numbers::none, Numbers::one));
  REQUIRE(enum_flags_test_any(Numbers::one|Numbers::two|Numbers::many, Numbers::many));
  REQUIRE_FALSE(enum_flags_test_any(Numbers::one|Numbers::two|Numbers::many, Numbers::three));
  REQUIRE_FALSE(enum_flags_test_any(Numbers::one|Numbers::two|Numbers::many, Numbers::none));

  REQUIRE_FALSE(enum_flags_test_any(Left|Right, NoDirection));
  REQUIRE(enum_flags_test_any(Left|Right|Up|Down, Right));
  REQUIRE_FALSE(enum_flags_test_any(Left|Up|Down, Right));

  REQUIRE(enum_flags_test_any(number::one|number::two|number::four, number::one));
  REQUIRE(enum_flags_test_any(number::one|number::two|number::four, number::one|number::two));
  REQUIRE_FALSE(enum_flags_test_any(number::one|number::two|number::four, number::three));
  REQUIRE_FALSE(enum_flags_test_any(number::one|number::two|number::four, number::no_number));

  REQUIRE(enum_flags_test(Numbers::none, Numbers::none) == enum_flags_test_any(Numbers::none, Numbers::none));
  REQUIRE(enum_flags_test(Numbers::one, Numbers::none) == enum_flags_test_any(Numbers::one, Numbers::none));
  REQUIRE(enum_flags_test(Numbers::none, Numbers::one) == enum_flags_test_any(Numbers::none, Numbers::one));
}
