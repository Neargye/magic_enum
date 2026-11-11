// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include "test_enums.hpp"

#if defined(MAGIC_ENUM_CALLING_CONVENTION) || defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
#  error Internal macro leaked from magic_enum.hpp.
#endif

#include "test_helpers.hpp"

#include <array>
#include <cctype>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

enum class DenseNegative { MinusTwo = -2, MinusOne = -1, Zero = 0 };

template <typename, typename, int = 0>
struct templated_scope {
  enum class Value { ONE = 1, TWO = 4 };
  enum class Collision { magic_enum_detail_enum_definition = 1 };
  enum class Empty {};
  enum class Opaque : unsigned;
};
using TemplatedValue = templated_scope<int, long>::Value;

template <int I>
using NumericFirstValue = typename templated_scope<void(int, int), int(*)[2], I>::Value;

struct IncompleteEnumArgument;
template <typename T, typename = void>
struct HasEnumName : std::false_type {};
template <typename T>
struct HasEnumName<T, std::void_t<decltype(magic_enum::enum_name(std::declval<T>()))>> : std::true_type {};

TEST_CASE("class-template enums reflect before enumerator access") {
  using namespace magic_enum;
  // Each first lookup uses a fresh enum, without naming any enumerator.
  static_assert(enum_name(static_cast<NumericFirstValue<0>>(1)) == "ONE");
  static_assert(enum_name<static_cast<NumericFirstValue<1>>(4)>() == "TWO");
  static_assert(enum_count<NumericFirstValue<2>>() == 2);
  static_assert(enum_values<NumericFirstValue<3>>().size() == 2);
  static_assert(enum_names<NumericFirstValue<4>>()[1] == "TWO");
  static_assert(enum_entries<NumericFirstValue<5>>()[0].second == "ONE");
  static_assert(enum_cast<NumericFirstValue<6>>(4) == static_cast<NumericFirstValue<6>>(4));
  static_assert(enum_cast<NumericFirstValue<7>>("TWO") == static_cast<NumericFirstValue<7>>(4));
  static_assert(enum_count<NumericFirstValue<8>, as_common<>>() == 2);
  static_assert(enum_index(static_cast<NumericFirstValue<9>>(4)) == 1);
  static_assert(enum_count<const NumericFirstValue<10>&>() == 2);
  static_assert(enum_name(static_cast<templated_scope<int, void>::Collision>(1)) == "magic_enum_detail_enum_definition");
  static_assert(enum_count<templated_scope<int, void>::Empty>() == 0);
#if !defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  static_assert(enum_count<templated_scope<int, void>::Opaque>() == 0);
#endif
  static_assert(!HasEnumName<int>::value);
  static_assert(!HasEnumName<IncompleteEnumArgument>::value);
}

enum class crc_hack {
  b5a7b602ab754d7ab30fb42c4fb28d82
};

enum class crc_hack_2 {
  b5a7b602ab754d7ab30fb42c4fb28d82,
  d19f2e9e82d14b96be4fa12b8a27ee9f
};

enum class hash_case_collision {
  EII1mjfIgJ,
  E7DvAyiJL6
};

// FNV-1a hashes of these names collide, so enum_cast falls back to CRC32.
enum class fnv_hack {
  costarring,
  liquid
};

#if defined(MAGIC_ENUM_ENABLE_HASH)
static_assert(magic_enum::detail::constexpr_hash_t{}("costarring") == magic_enum::detail::constexpr_hash_t{}("liquid"));
static_assert(!magic_enum::detail::has_unique_hashes_v<&magic_enum::detail::names_v<fnv_hack, magic_enum::detail::enum_subtype::common>, magic_enum::detail::constexpr_hash_t>);

struct identity_hash {
  constexpr int operator()(int value) const noexcept { return value; }
};

constexpr auto make_large_hash_input(bool duplicate) noexcept {
  std::array<int, 33> values{};
  for (std::size_t i = 0; i < values.size(); ++i) {
    values[i] = static_cast<int>((i * 17) % values.size());
  }
  if (duplicate) {
    values.back() = values.front();
  }
  return values;
}

static constexpr auto unique_large_hash_input = make_large_hash_input(false);
static constexpr auto duplicate_large_hash_input = make_large_hash_input(true);
static_assert(magic_enum::detail::has_unique_hashes_v<&unique_large_hash_input, identity_hash>);
static_assert(!magic_enum::detail::has_unique_hashes_v<&duplicate_large_hash_input, identity_hash>);

// Unused hash paths must not require a secondary hash or a callable hash.
static_assert(magic_enum::detail::has_usable_hash<&unique_large_hash_input, identity_hash>());
static_assert(!magic_enum::detail::has_usable_hash<&unique_large_hash_input, void, false>());
#endif

enum class duplicate_custom_name { first, second };
template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<duplicate_custom_name>(duplicate_custom_name value) noexcept {
  switch (value) {
    case duplicate_custom_name::first:
    case duplicate_custom_name::second:
      return "same";
    default:
      return default_tag;
  }
}

namespace We::Need::To::Go::Deeper {
    enum class Dimension : short { Overworld = 1000, Nether, TheEnd = Overworld + 128 };
    enum class Flaggy : std::uint64_t { Flag0 = 1 << 0, Flag32 = std::uint64_t(1) << 32 };

    auto magic_enum_define_range_adl(Dimension)
    {
      return magic_enum::customize::adl_info().minmax<1000,1000+128>();
    }

    auto magic_enum_define_range_adl(Flaggy)
    {
      return magic_enum::customize::adl_info().flag<true>();
    }
}
using We::Need::To::Go::Deeper::Dimension;
using We::Need::To::Go::Deeper::Flaggy;

struct FriendRangeHolder {
  enum class Level { Level_Low = 500, Level_High = 501 };

  friend constexpr auto magic_enum_define_range_adl(Level) {
    return magic_enum::customize::adl_info().minmax<500, 501>().prefix<sizeof("Level_") - 1>();
  }
};

template <typename T>
struct FriendRangeTemplate {
  enum class Level { Level_Low = 500, Level_High = 501 };

  friend constexpr auto magic_enum_define_range_adl(Level) {
    return magic_enum::customize::adl_info().minmax<500, 501>().prefix<sizeof("Level_") - 1>();
  }
};

enum CStyleEnum {
    CStyleEnum_A = -36,
    CStyleEnum_B,
    CStyleEnum_C,
    CStyleEnum_D,
    CStyleEnum_F,
    CStyleEnum_G,
    CStyleEnum_H = 36
};

template <>
struct magic_enum::customize::enum_range<CStyleEnum> {
    static constexpr auto prefix_length = sizeof("CStyleEnum_") - 1;
    static constexpr int min = -100;
    static constexpr int max = 100;
};

enum class BoolTest : bool { Yay, Nay };

using namespace magic_enum;
using namespace magic_enum_tests;

static_assert(is_magic_enum_supported, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");

struct LvalueOnlyPredicate {
  constexpr bool operator()(char lhs, char rhs) & noexcept { return lhs == rhs; }
};

struct RefQualifiedPredicate {
  constexpr bool operator()(char lhs, char rhs) & noexcept(false) { return lhs == rhs; }
  constexpr bool operator()(char lhs, char rhs) && noexcept { return lhs == rhs; }
};

TEST_CASE("unqualified names stay within their view") {
  static constexpr char name[] = {'E', ':', ':', 'A'};
  constexpr auto result = magic_enum::detail::unqualified({name, sizeof(name)});
  static_assert(result.str_ == name + 3 && result.size_ == 1);
  CHECK(std::string_view{result.str_, result.size_} == "A");
  static_assert(magic_enum::detail::unqualified(magic_enum::detail::cname_ref{}).size_ == 0);
}

TEST_CASE("customization descriptors expose tag and name") {
  using customize::customize_t;
  constexpr customize_t named{"label"};
  constexpr customize_t viewed{string_view{"label-extra", 5}};
  static_assert(named.name == "label");
  static_assert(viewed.name == named.name);
  static_assert(viewed.tag == named.tag);
  static_assert(named.tag != customize::default_tag.tag);
  static_assert(named.tag != customize::invalid_tag.tag);
  static_assert(customize::default_tag.tag != customize::invalid_tag.tag);
  static_assert(customize::default_tag.name.empty());
  static_assert(customize::invalid_tag.name.empty());
  constexpr customize_t copied{named};
  static_assert(copied.tag == named.tag && copied.name == named.name);
  auto assigned = customize::default_tag;
  assigned = named;
  CHECK(assigned.tag == named.tag);
  CHECK(assigned.name == named.name);
}

TEST_CASE("bool enum indices use the reflected minimum") {
  enum class BoolIndexBoth : bool { False, True };
  enum class BoolIndexTrue : bool { True = true };
  static_assert(enum_index(BoolIndexBoth::False).value() == 0);
  static_assert(enum_index(BoolIndexBoth::True).value() == 1);
  static_assert(enum_index(BoolIndexTrue::True).value() == 0);
  static_assert(!enum_index(static_cast<BoolIndexTrue>(false)).has_value());
  static_assert(enum_name(BoolIndexTrue::True) == "True");
  static_assert(!enum_cast<BoolIndexTrue>(false).has_value());
}

TEST_CASE("bool enum text lookup does not convert pointers to true") {
  static_assert(enum_cast<BoolTest>("Yay") == BoolTest::Yay);
  static_assert(enum_cast<const BoolTest&>("Nay") == BoolTest::Nay);
  static_assert(!enum_cast<BoolTest>("INVALID").has_value());
  static_assert(!enum_cast<BoolTest>("").has_value());
  static_assert(enum_contains<BoolTest>("Yay"));
  static_assert(!enum_contains<BoolTest>("INVALID"));
  static_assert(!enum_contains<BoolTest>(""));
  static_assert(enum_cast<BoolTest, as_common<>>("Yay") == BoolTest::Yay);
  static_assert(enum_cast<BoolTest>("yay", case_insensitive) == BoolTest::Yay);
  static_assert(enum_cast<BoolTest>("Yay", ConstReferencePredicate{}) == BoolTest::Yay);
  static_assert(enum_contains<BoolTest>("Yay", ConstReferencePredicate{}));
  constexpr bool nothrow_name = std::is_nothrow_constructible_v<string_view, decltype("Yay")>;
  static_assert(noexcept(enum_cast<BoolTest>("Yay")) == nothrow_name);
  static_assert(noexcept(enum_contains<BoolTest>("Yay")) == nothrow_name);
  static_assert(noexcept(enum_cast<BoolTest>(std::declval<string_view>())));
  static_assert(noexcept(enum_contains<BoolTest>(std::declval<string_view>())));
  static_assert(!noexcept(enum_cast<BoolTest>("Yay", ThrowingReferencePredicate{})));

  constexpr const char* false_name = "Yay";
  constexpr const char* invalid_name = "INVALID";
  static_assert(enum_cast<BoolTest>(false_name) == BoolTest::Yay);
  static_assert(!enum_cast<BoolTest>(invalid_name).has_value());
  static_assert(enum_cast<BoolTest>(string_view{"Yay-extra", 3}) == BoolTest::Yay);
  static_assert(enum_cast<BoolTest>({"Yay-extra", 3}) == BoolTest::Yay);

  for (const char* name : {"Yay", "Nay", "INVALID", "", "yay", "Yay-extra"}) {
    const auto expected = enum_cast<BoolTest>(string_view{name});
    CHECK(enum_cast<BoolTest>(name) == expected);
    CHECK(enum_contains<BoolTest>(name) == expected.has_value());
    const std::string owned{name};
    CHECK(enum_cast<BoolTest>(owned) == expected);
    CHECK(enum_cast<BoolTest>(std::string{name}) == expected);
    CHECK(enum_contains<BoolTest>(owned) == expected.has_value());
  }
  char mutable_name[] = "Yay";
  char* mutable_pointer = mutable_name;
  CHECK(enum_cast<BoolTest>(mutable_name) == BoolTest::Yay);
  CHECK(enum_cast<BoolTest>(mutable_pointer) == BoolTest::Yay);
  CHECK(enum_contains<BoolTest>(mutable_pointer));
  CHECK(enum_cast<BoolTest>(false) == BoolTest::Yay);
  CHECK(enum_cast<BoolTest>(true) == BoolTest::Nay);
  CHECK(enum_cast<BoolTest>(0) == BoolTest::Yay);
  CHECK(enum_cast<BoolTest>(1) == BoolTest::Nay);
  REQUIRE_THROWS_AS(static_cast<void>(enum_cast<BoolTest>("Yay", ThrowingReferencePredicate{})), int);

  struct BorrowedName {
    BorrowedName() = default;
    BorrowedName(const BorrowedName&) = delete;
    operator string_view() & noexcept { return "Yay"; }
    operator bool() const noexcept { return true; }
  };
  BorrowedName borrowed;
  CHECK(enum_cast<BoolTest>(borrowed) == BoolTest::Yay);
  CHECK(enum_contains<BoolTest>(borrowed));
  static_assert(noexcept(enum_cast<BoolTest>(borrowed)));

  struct TemporaryName {
    operator string_view() && noexcept { return "Yay"; }
  };
  CHECK(enum_cast<BoolTest>(TemporaryName{}) == BoolTest::Yay);
  CHECK(enum_contains<BoolTest>(TemporaryName{}));

  struct ThrowingName {
    bool should_throw = true;
    // Keep the conversion opaque to MSVC's unreachable-code analysis.
    DOCTEST_NOINLINE operator string_view() const {
      if (should_throw) {
        throw 42;
      }
      return "Yay";
    }
  };
  static_assert(!noexcept(enum_cast<BoolTest>(ThrowingName{})));
  static_assert(!noexcept(enum_contains<BoolTest>(ThrowingName{})));
  CHECK_THROWS_AS(static_cast<void>(enum_cast<BoolTest>(ThrowingName{})), int);
  CHECK_THROWS_AS(static_cast<void>(enum_contains<BoolTest>(ThrowingName{})), int);
  CHECK(enum_cast<BoolTest>(ThrowingName{false}) == BoolTest::Yay);
  CHECK(enum_contains<BoolTest>(ThrowingName{false}));
}

TEST_CASE("bool enum reflection excludes unnamed values") {
  enum class EmptyBool : bool {};
  enum class FalseBool : bool { Off = false };
  enum class TrueBool : bool { On = true };

  static_assert(enum_count<EmptyBool>() == 0);
  static_assert(enum_count<FalseBool>() == 1);
  static_assert(enum_count<TrueBool>() == 1);
  static_assert(enum_name(static_cast<EmptyBool>(false)).empty());
  static_assert(enum_name(static_cast<EmptyBool>(true)).empty());
  static_assert(enum_name(static_cast<FalseBool>(true)).empty());
  static_assert(enum_name(static_cast<TrueBool>(false)).empty());
  static_assert(enum_name(FalseBool::Off) == "Off");
  static_assert(enum_name(TrueBool::On) == "On");
  static_assert(enum_name<FalseBool::Off>() == "Off");
  static_assert(enum_name<TrueBool::On>() == "On");
  static_assert(enum_name<BoolTest::Yay>() == "Yay");
  static_assert(enum_name<BoolTest::Nay>() == "Nay");

  static_assert(!enum_cast<EmptyBool>(false).has_value());
  static_assert(!enum_cast<EmptyBool>(true).has_value());
  static_assert(!enum_index(static_cast<EmptyBool>(false)).has_value());
  static_assert(!enum_index(static_cast<EmptyBool>(true)).has_value());

  CHECK_FALSE(enum_cast<FalseBool>(true).has_value());
  CHECK_FALSE(enum_cast<TrueBool>(false).has_value());
  CHECK_FALSE(enum_contains(static_cast<FalseBool>(true)));
  CHECK_FALSE(enum_contains(static_cast<TrueBool>(false)));
}

TEST_CASE("enum_cast") {
  SUBCASE("string") {
    constexpr auto cr = enum_cast<Color>("red");
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(enum_cast<Color&>("GREEN").value() == Color::GREEN);
    REQUIRE(enum_cast<Color>("blue", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).value() == Color::BLUE);
    REQUIRE(enum_cast<Color>("GREEN", LvalueOnlyPredicate{}).value() == Color::GREEN);
    REQUIRE(enum_contains<Color>("GREEN", LvalueOnlyPredicate{}));
    REQUIRE_FALSE(enum_cast<Color>("None").has_value());
    static_assert(!noexcept(enum_cast<Color>("GREEN", RefQualifiedPredicate{})));
    static_assert(enum_cast<Color>("GREEN", ConstReferencePredicate{}) == Color::GREEN);
    static_assert(enum_contains<Color>("GREEN", ConstReferencePredicate{}));
    static_assert(!noexcept(enum_cast<Color>("GREEN", ThrowingReferencePredicate{})));
    static_assert(!noexcept(enum_contains<Color>("GREEN", ThrowingReferencePredicate{})));
    REQUIRE_THROWS_AS(static_cast<void>(enum_cast<Color>("GREEN", ThrowingReferencePredicate{})), int);
    REQUIRE_THROWS_AS(static_cast<void>(enum_contains<Color>("GREEN", ThrowingReferencePredicate{})), int);

    constexpr auto dim = enum_cast<Dimension>("Nether");
    REQUIRE(dim.value() == Dimension::Nether);
    REQUIRE(enum_cast<Dimension&>("Nether").value() == Dimension::Nether);
    REQUIRE(enum_cast<Dimension>("theend", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).value() == Dimension::TheEnd);
    REQUIRE_FALSE(enum_cast<Dimension>("Aether").has_value());

    constexpr auto cstyle = enum_cast<CStyleEnum>("A");
    REQUIRE(cstyle.value() == CStyleEnum_A);
    REQUIRE(enum_cast<const CStyleEnum&>("H").value() == CStyleEnum_H);
    REQUIRE_FALSE(enum_cast<CStyleEnum>("CStyleEnum_H").has_value());
    REQUIRE(enum_cast<CStyleEnum>("d", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }) == CStyleEnum_D);
    REQUIRE_FALSE(enum_cast<CStyleEnum>("Q").has_value());

    constexpr auto no = enum_cast<Numbers>("one");
    REQUIRE(no.value() == Numbers::one);
    REQUIRE(enum_cast<Numbers>("two").value() == Numbers::two);
    REQUIRE(enum_cast<Numbers>("three").value() == Numbers::three);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(enum_cast<Numbers>("many") == Numbers::many);
#else
    REQUIRE_FALSE(enum_cast<Numbers>("many").has_value());
#endif
    REQUIRE_FALSE(enum_cast<Numbers>("None").has_value());

    constexpr auto dr = enum_cast<Directions>("Right");
    REQUIRE(enum_cast<Directions&>("Up").value() == Directions::Up);
    REQUIRE(enum_cast<const Directions>("Down").value() == Directions::Down);
    REQUIRE(dr.value() == Directions::Right);
    REQUIRE(enum_cast<Directions>("Left").value() == Directions::Left);
    REQUIRE_FALSE(enum_cast<Directions>("None").has_value());
    // The default predicate skips names of other lengths.
    static_assert(enum_cast<Directions>("Up") == Directions::Up);
    static_assert(enum_cast<Directions>("Right") == Directions::Right);
    static_assert(!enum_cast<Directions>("").has_value());
    static_assert(!enum_cast<Directions>("U").has_value());
    static_assert(!enum_cast<Directions>("Rights").has_value());
    REQUIRE(enum_cast<Directions>("RIGHT", case_insensitive) == Directions::Right);

    constexpr auto dr2 = enum_cast<Directions>("RIGHT", case_insensitive);
    REQUIRE(dr2.value() == Directions::Right);
    REQUIRE(enum_cast<Directions&>("up", case_insensitive).value() == Directions::Up);
    REQUIRE(enum_cast<const Directions>("dOwN", case_insensitive).value() == Directions::Down);
    REQUIRE_FALSE(enum_cast<Directions>("Left-", case_insensitive).has_value());

    constexpr auto nt = enum_cast<number>("three");
    REQUIRE(enum_cast<number>("one").value() == number::one);
    REQUIRE(enum_cast<number>("two").value() == number::two);
    REQUIRE(nt.value() == number::three);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(enum_cast<number>("four") == number::four);
#else
    REQUIRE_FALSE(enum_cast<number>("four").has_value());
#endif
    REQUIRE_FALSE(enum_cast<number>("None").has_value());

    REQUIRE(enum_cast<crc_hack>("b5a7b602ab754d7ab30fb42c4fb28d82").has_value());
    REQUIRE_FALSE(enum_cast<crc_hack>("d19f2e9e82d14b96be4fa12b8a27ee9f").has_value());

    constexpr auto crc = enum_cast<crc_hack_2>("b5a7b602ab754d7ab30fb42c4fb28d82");
    REQUIRE(crc.value() == crc_hack_2::b5a7b602ab754d7ab30fb42c4fb28d82);
    REQUIRE(enum_cast<crc_hack_2>("d19f2e9e82d14b96be4fa12b8a27ee9f").value() == crc_hack_2::d19f2e9e82d14b96be4fa12b8a27ee9f);
    REQUIRE(enum_cast<hash_case_collision>("EII1mjfIgJ") == hash_case_collision::EII1mjfIgJ);
    REQUIRE(enum_cast<hash_case_collision>("E7DvAyiJL6") == hash_case_collision::E7DvAyiJL6);
    REQUIRE(enum_cast<fnv_hack>("costarring") == fnv_hack::costarring);
    REQUIRE(enum_cast<fnv_hack>("liquid") == fnv_hack::liquid);
    REQUIRE_FALSE(enum_cast<fnv_hack>("liquids").has_value());
    REQUIRE(enum_cast<duplicate_custom_name>("same") == duplicate_custom_name::first);

    constexpr auto bool_name = string_view{"Yay"};
    static_assert(enum_cast<BoolTest>(bool_name) == BoolTest::Yay);
  }

  SUBCASE("integer") {
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    constexpr auto cr = enum_cast<Color>(-12);
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(enum_cast<Color&>(7).value() == Color::GREEN);
    REQUIRE(enum_cast<Color>(static_cast<int>(cm[2])).value() == Color::BLUE);
    REQUIRE_FALSE(enum_cast<Color>(0).has_value());

    constexpr auto no = enum_cast<Numbers>(1);
    REQUIRE(no.value() == Numbers::one);
    REQUIRE(enum_cast<Numbers>(2).value() == Numbers::two);
    REQUIRE(enum_cast<Numbers>(3).value() == Numbers::three);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(enum_cast<Numbers>(127) == Numbers::many);
#else
    REQUIRE_FALSE(enum_cast<Numbers>(127).has_value());
#endif
    REQUIRE_FALSE(enum_cast<Numbers>(0).has_value());

    constexpr auto dr = enum_cast<Directions>(120);
    REQUIRE(enum_cast<Directions&>(85).value() == Directions::Up);
    REQUIRE(enum_cast<const Directions>(-42).value() == Directions::Down);
    REQUIRE(dr.value() == Directions::Right);
    REQUIRE(enum_cast<Directions>(-120).value() == Directions::Left);
    REQUIRE_FALSE(enum_cast<Directions>(0).has_value());
#if defined(MAGIC_ENUM_ENABLE_HASH)
    using DirectionsUnderlying = magic_enum::underlying_type_t<Directions>;
    static_assert(!enum_cast<Directions>((std::numeric_limits<DirectionsUnderlying>::max)()).has_value());
#endif

    constexpr auto nt = enum_cast<number>(300);
    REQUIRE(enum_cast<number>(100).value() == number::one);
    REQUIRE(enum_cast<number>(200).value() == number::two);
    REQUIRE(nt.value() == number::three);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(enum_cast<number>(400) == number::four);
#else
    REQUIRE_FALSE(enum_cast<number>(400).has_value());
#endif
    REQUIRE_FALSE(enum_cast<number>(0).has_value());

    REQUIRE(enum_cast<BoolTest>(false).has_value());
    REQUIRE(enum_cast<BoolTest>(0).has_value());

    constexpr auto hostile = enum_cast<hostile_enum_comparison::Value>(4);
    REQUIRE(hostile.has_value());
    REQUIRE(enum_integer(*hostile) == 4);
  }
}

TEST_CASE("enum_integer") {
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  constexpr auto cr = enum_integer(Color::RED);
  constexpr auto cr_underlying = enum_underlying(Color::RED);
  static_assert(std::is_same_v<decltype(enum_integer(Color::RED)), int>);
  static_assert(std::is_same_v<decltype(enum_underlying(Color::RED)), int>);
  static_assert(noexcept(enum_integer(Color::RED)));
  static_assert(noexcept(enum_underlying(Color::RED)));
  Color cg = Color::GREEN;
  REQUIRE(cr == -12);
  REQUIRE(cr_underlying == -12);
  REQUIRE(enum_integer<Color&>(cg) == 7);
  REQUIRE(enum_underlying<Color&>(cg) == 7);
  REQUIRE(enum_integer(cm[2]) == 15);
  REQUIRE(enum_underlying(cm[2]) == 15);
  REQUIRE(enum_integer(static_cast<Color>(0)) == 0);
  REQUIRE(enum_underlying(static_cast<Color>(0)) == 0);

  constexpr auto no = enum_integer(Numbers::one);
  REQUIRE(no == 1);
  REQUIRE(enum_integer(Numbers::two) == 2);
  REQUIRE(enum_integer(Numbers::three) == 3);
  REQUIRE(enum_integer(Numbers::many) == 127);
  REQUIRE(enum_integer(static_cast<Numbers>(0)) == 0);

  constexpr auto dr = enum_integer(Directions::Right);
  Directions dl = Directions::Left;
  REQUIRE(enum_integer<Directions&>(dl) == -120);
  REQUIRE(enum_integer<const Directions>(Directions::Down) == -42);
  REQUIRE(enum_integer(Directions::Up) == 85);
  REQUIRE(dr == 120);
  REQUIRE(enum_integer(static_cast<Directions>(0)) == 0);

  constexpr auto nt = enum_integer(number::three);
  REQUIRE(enum_integer(number::one) == 100);
  REQUIRE(enum_integer(number::two) == 200);
  REQUIRE(nt == 300);
  REQUIRE(enum_integer(number::four) == 400);
  REQUIRE(enum_integer(static_cast<number>(0)) == 0);

  REQUIRE(enum_integer(BoolTest::Yay) == false);
}

TEST_CASE("enum_index") {
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  constexpr auto cr = enum_index(Color::RED);
  Color cg = Color::GREEN;
  REQUIRE(cr.value() == 0);
  REQUIRE(enum_index<Color::RED>() == 0);
  REQUIRE(enum_index<Color&>(cg).value() == 1);
  REQUIRE(enum_index(cm[2]).value() == 2);
  REQUIRE_FALSE(enum_index(static_cast<Color>(0)).has_value());

  constexpr auto no = enum_index(Numbers::one);
  REQUIRE(no.value() == 0);
  REQUIRE(enum_index<Numbers::one>() == 0);
  REQUIRE(enum_index(Numbers::two).value() == 1);
  REQUIRE(enum_index(Numbers::three).value() == 2);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(enum_index(Numbers::many).value() == 3);
#else
  REQUIRE_FALSE(enum_index(Numbers::many).has_value());
#endif
  REQUIRE_FALSE(enum_index(static_cast<Numbers>(0)).has_value());

  constexpr auto dr = enum_index(Directions::Right);
  Directions dl = Directions::Left;
  REQUIRE(enum_index<Directions::Left>() == 0);
  REQUIRE(enum_index<Directions&>(dl).value() == 0);
  REQUIRE(enum_index<const Directions>(Directions::Down).value() == 1);
  REQUIRE(enum_index(Directions::Up).value() == 2);
  REQUIRE(dr.value() == 3);
  REQUIRE_FALSE(enum_index(static_cast<Directions>(0)).has_value());

  constexpr auto nt = enum_index(number::three);
  REQUIRE(enum_index<number::one>() == 0);
  REQUIRE(enum_index(number::one).value() == 0);
  REQUIRE(enum_index(number::two).value() == 1);
  REQUIRE(nt.value() == 2);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(enum_index(number::four).value() == 3);
#else
  REQUIRE_FALSE(enum_index(number::four).has_value());
#endif
  REQUIRE_FALSE(enum_index(static_cast<number>(0)).has_value());

  REQUIRE(enum_index<BoolTest::Yay>() == 0);

  constexpr auto hostile = enum_index(hostile_enum_comparison::Value::Second);
  REQUIRE(hostile.value() == 1);
}

TEST_CASE("integer cast checks the value before narrowing") {
  enum class U8 : std::uint8_t { One = 1, Max = 100 };
  enum class I8 : std::int8_t { Neg = -100, One = 1 };

  static_assert(enum_cast<U8>(1) == U8::One);
  static_assert(!enum_cast<U8>(257).has_value());
  static_assert(!enum_contains<U8>(257));
  static_assert(enum_cast<I8>(-100) == I8::Neg);
  static_assert(!enum_cast<I8>(156).has_value());
  static_assert(!enum_cast<I8>(257).has_value());
  static_assert(!enum_contains<I8>(-255));
  static_assert(enum_cast<Binary>(1) == Binary::TWO);
  static_assert(!enum_cast<Binary>(2).has_value());

  int negative = -255;
  REQUIRE_FALSE(enum_cast<U8>(static_cast<std::uintmax_t>(negative)).has_value());
  REQUIRE_FALSE(enum_contains<U8>(static_cast<std::uintmax_t>(negative)));
  REQUIRE_FALSE(enum_cast<U8>(negative).has_value());
  REQUIRE_FALSE(enum_contains<U8>(negative));

  // Wider arguments keep their sign, so UINT64_MAX does not wrap to -1.
  enum class Small : std::int8_t { MinusOne = -1, One = 1 };
  static_assert(enum_cast<Small>(-1) == Small::MinusOne);
  static_assert(enum_cast<Small>(std::uint64_t{1}) == Small::One);
  static_assert(!enum_cast<Small>((std::numeric_limits<std::uint64_t>::max)()).has_value());
  static_assert(!enum_contains<Small>((std::numeric_limits<std::uintmax_t>::max)()));
  static_assert(!enum_cast<U8>(-1).has_value());
  static_assert(!enum_cast<U8>((std::numeric_limits<std::int64_t>::min)()).has_value());

  // Unscoped enums of another type are checked through their underlying value.
  enum Raw : int { RawOne = 1, RawBig = 257 };
  static_assert(enum_cast<U8>(RawOne) == U8::One);
  static_assert(!enum_cast<U8>(RawBig).has_value());
  static_assert(!enum_contains<U8>(RawBig));
}

template <typename E, typename V, typename = void>
struct can_reflect : std::false_type {};

template <typename E, typename V>
struct can_reflect<E, V, std::void_t<decltype(enum_reflected<E>(std::declval<V>()))>> : std::true_type {};

template <typename E, typename V, typename = void>
struct can_contain : std::false_type {};

template <typename E, typename V>
struct can_contain<E, V, std::void_t<decltype(enum_contains<E>(std::declval<V>()))>> : std::true_type {};

TEST_CASE("enum_reflected checks the value before narrowing") {
  enum class ReflU8 : std::uint8_t { One = 1, Max = 100 };
  enum class ReflI8 : std::int8_t { Neg = -100, One = 1 };
  enum class ReflSmall : std::int8_t { MinusOne = -1, One = 1 };
  enum class ReflU64 : std::uint64_t { One = 1 };
  enum class ReflEmpty {};

  static_assert(enum_reflected<ReflU8>(1));
  static_assert(enum_reflected<ReflU8>(100));
  static_assert(!enum_reflected<ReflU8>(257));
  static_assert(!enum_reflected<ReflU8>(-1));
  static_assert(!enum_reflected<ReflU8>((std::numeric_limits<std::int64_t>::min)()));
  static_assert(!enum_reflected<ReflI8>(156));
  static_assert(!enum_reflected<ReflSmall>((std::numeric_limits<std::uint64_t>::max)()));
  static_assert(enum_reflected<ReflSmall>(-1));
  static_assert(!enum_reflected<ReflU64>((std::numeric_limits<std::int64_t>::min)()));
  static_assert(enum_reflected<BoolTest>(0));
  static_assert(enum_reflected<BoolTest>(1));
  static_assert(enum_reflected<BoolTest>(true));
  static_assert(!enum_reflected<BoolTest>(2));
  static_assert(enum_reflected<ReflEmpty>(0));
  static_assert(enum_reflected<ReflU8>(2));
  static_assert(!enum_contains<ReflU8>(2));

  // Explicit narrowing is the caller's choice.
  static_assert(enum_reflected<ReflU8>(static_cast<std::uint8_t>(257)));

  enum ReflRaw : int { ReflRawOne = 1, ReflRawBig = 257 };
  static_assert(enum_reflected<ReflU8>(ReflRawOne));
  static_assert(!enum_reflected<ReflU8>(ReflRawBig));

#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  static_assert(enum_reflected<ReflU8>(200));
  static_assert(enum_reflected<ReflI8>(-128));
  static_assert(enum_reflected<ReflU64>((std::numeric_limits<std::uint64_t>::max)()));
  static_assert(enum_reflected<ReflEmpty>(121));
#else
  static_assert(!enum_reflected<ReflU8>(200));
  static_assert(!enum_reflected<ReflI8>(-128));
  static_assert(!enum_reflected<ReflU64>((std::numeric_limits<std::uint64_t>::max)()));
  static_assert(!enum_reflected<ReflEmpty>(121));
  static_assert(enum_reflected<ReflEmpty>(120));
  static_assert(enum_reflected<ReflEmpty>(-120));
#endif

  static_assert(can_reflect<ReflU8, int>::value && can_contain<ReflU8, int>::value);
  static_assert(can_reflect<ReflU8, bool>::value && can_contain<ReflU8, bool>::value);
  static_assert(can_reflect<ReflU8, ReflU8>::value && can_contain<ReflU8, ReflU8>::value);
  static_assert(can_reflect<ReflU8, ReflRaw>::value && can_contain<ReflU8, ReflRaw>::value);
  static_assert(!can_reflect<ReflU8, double>::value && !can_contain<ReflU8, double>::value);
  static_assert(!can_reflect<ReflU8, ReflI8>::value && !can_contain<ReflU8, ReflI8>::value);
  static_assert(!can_reflect<int, int>::value && !can_contain<int, int>::value);

  // An enum prvalue cannot bind to a non-const reference E.
  static_assert(can_reflect<ReflRaw&, ReflRaw&>::value && can_contain<ReflRaw&, ReflRaw&>::value);
  static_assert(!can_reflect<ReflRaw&, ReflRaw>::value && !can_contain<ReflRaw&, ReflRaw>::value);
  static_assert(can_reflect<const ReflRaw&, ReflRaw>::value && can_contain<const ReflRaw&, ReflRaw>::value);
  ReflRaw raw = ReflRawOne;
  REQUIRE(enum_reflected<ReflRaw&>(raw));
  ReflU8 u8 = ReflU8::One;
  REQUIRE(enum_reflected<ReflU8&>(u8));
  REQUIRE(enum_reflected<const ReflU8&>(1));
  REQUIRE_FALSE(enum_reflected<const ReflU8&>(257));

  constexpr bool (*from_int)(int) noexcept = &enum_reflected<ReflU8>;
  constexpr bool (*from_u64)(std::uint64_t) noexcept = &enum_reflected<ReflU8>;
  constexpr bool (*from_enum)(ReflU8) noexcept = &enum_reflected<ReflU8>;
  REQUIRE(from_int(1));
  REQUIRE_FALSE(from_int(257));
  REQUIRE_FALSE(from_u64((std::numeric_limits<std::uint64_t>::max)()));
  REQUIRE(from_enum(ReflU8::Max));
}

TEST_CASE("string lookup keeps function pointer types") {
  using Pred = std::equal_to<>;
  constexpr optional<Color> (*cast)(string_view, Pred) noexcept = &enum_cast<Color>;
  constexpr bool (*contains)(string_view, Pred) noexcept = &enum_contains<Color>;
  REQUIRE(cast("red", Pred{}) == Color::RED);
  REQUIRE(contains("red", Pred{}));
  REQUIRE_FALSE(contains("None", Pred{}));
}

TEST_CASE("braced arguments preserve zero behavior") {
  enum class BracedCastEnum { Zero, One };
  constexpr auto zero = enum_cast<BracedCastEnum>({});
  REQUIRE(zero == BracedCastEnum::Zero);
  REQUIRE(enum_contains<BracedCastEnum>({}));
  static_assert(enum_reflected<BracedCastEnum>({}));
}

TEST_CASE("enum_contains") {
  SUBCASE("value") {
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    constexpr auto cr = enum_contains(Color::RED);
    Color cg = Color::GREEN;
    REQUIRE(cr);
    REQUIRE(enum_contains<Color&>(cg));
    REQUIRE(enum_contains(cm[2]));
    REQUIRE_FALSE(enum_contains(static_cast<Color>(0)));

    constexpr auto no = enum_contains(Numbers::one);
    REQUIRE(no);
    REQUIRE(enum_contains(Numbers::two));
    REQUIRE(enum_contains(Numbers::three));
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(enum_contains(Numbers::many));
#else
    REQUIRE_FALSE(enum_contains(Numbers::many));
#endif
    REQUIRE_FALSE(enum_contains(static_cast<Numbers>(0)));

    constexpr auto dr = enum_contains(Directions::Right);
    Directions dl = Directions::Left;
    REQUIRE(enum_contains<Directions&>(dl));
    REQUIRE(enum_contains<const Directions>(Directions::Down));
    REQUIRE(enum_contains(Directions::Up));
    REQUIRE(dr);
    REQUIRE_FALSE(enum_contains(static_cast<Directions>(0)));

    constexpr auto nt = enum_contains(number::three);
    REQUIRE(enum_contains(number::one));
    // A reference E binds the enum value overload, so the argument must be an lvalue.
    number nv = number::two;
    REQUIRE(enum_contains<number&>(nv));
    REQUIRE(nt);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(enum_contains(number::four));
#else
    REQUIRE_FALSE(enum_contains(number::four));
#endif
    REQUIRE_FALSE(enum_contains(static_cast<number>(0)));

    REQUIRE(enum_contains(BoolTest::Yay));
  }

  SUBCASE("integer") {
    REQUIRE(enum_contains<Color>(-12));
    REQUIRE(enum_contains<Color&>(7));
    REQUIRE(enum_contains<Color>(15));
    REQUIRE_FALSE(enum_contains<Color>(42));
    REQUIRE_FALSE(enum_contains<Color>(-120));
    REQUIRE_FALSE(enum_contains<Color>(0));

    constexpr auto no = enum_integer(Numbers::one);
    REQUIRE(enum_contains<Numbers>(no));
    REQUIRE(enum_contains<Numbers>(enum_integer(Numbers::two)));
    REQUIRE(enum_contains<Numbers>(enum_integer(Numbers::three)));
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(enum_contains<Numbers>(enum_integer(Numbers::many)));
#else
    REQUIRE_FALSE(enum_contains<Numbers>(enum_integer(Numbers::many)));
#endif

    constexpr auto dr = enum_integer(Directions::Right);
    REQUIRE(enum_contains<Directions&>(dr));
    REQUIRE(enum_contains<const Directions>(Directions::Down));
    REQUIRE(enum_contains<Directions>(Directions::Up));
    REQUIRE_FALSE(enum_contains<Directions>(static_cast<Directions>(0)));

    constexpr auto nt = enum_contains<number>(number::three);
    REQUIRE(enum_contains<number>(number::one));
    REQUIRE(enum_contains<number>(100));
    REQUIRE(enum_contains<number>(200));
    REQUIRE(enum_contains<number>(300));
    REQUIRE(enum_contains<number>(number::two));
    REQUIRE(nt);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(enum_contains<number>(number::four));
#else
    REQUIRE_FALSE(enum_contains<number>(number::four));
#endif
    REQUIRE_FALSE(enum_contains<number>(111));
    REQUIRE_FALSE(enum_contains<number>(0));

    REQUIRE(enum_contains<BoolTest>(false));
    REQUIRE(enum_contains<BoolTest>(0));
  }

  SUBCASE("string") {
    constexpr auto cr = "red";
    REQUIRE(enum_contains<Color>(cr));
    REQUIRE(enum_contains<Color&>("GREEN"));
    REQUIRE(enum_contains<Color>("blue", [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }));
    REQUIRE_FALSE(enum_contains<Color>("None"));

    constexpr auto no = std::string_view{"one"};
    REQUIRE(enum_contains<Numbers>(no));
    REQUIRE(enum_contains<Numbers>("two"));
    REQUIRE(enum_contains<Numbers>("three"));
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(enum_contains<Numbers>("many"));
#else
    REQUIRE_FALSE(enum_contains<Numbers>("many"));
#endif
    REQUIRE_FALSE(enum_contains<Numbers>("None"));

    auto dr = std::string{"Right"};
    REQUIRE(enum_contains<Directions&>("Up"));
    REQUIRE(enum_contains<Directions>("Down"));
    REQUIRE(enum_contains<const Directions>(dr));
    REQUIRE(enum_contains<Directions>("Left"));
    REQUIRE_FALSE(enum_contains<Directions>("None"));

    auto dr2 = std::string{"RIGHT"};
    REQUIRE(enum_contains<const Directions>(dr2, case_insensitive));
    REQUIRE(enum_contains<Directions&>("up", case_insensitive));
    REQUIRE(enum_contains<Directions>("dOwN", case_insensitive));
    REQUIRE_FALSE(enum_contains<Directions>("Left-", case_insensitive));

    constexpr auto nt = enum_contains<number>("three");
    REQUIRE(enum_contains<number>("one"));
    REQUIRE(enum_contains<number>("two"));
    REQUIRE(nt);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(enum_contains<number>("four"));
#else
    REQUIRE_FALSE(enum_contains<number>("four"));
#endif
    REQUIRE_FALSE(enum_contains<number>("None"));

    REQUIRE(enum_contains<BoolTest>(string_view{"Yay"}));
  }
}

TEST_CASE("enum_value") {
  REQUIRE(enum_value<DenseNegative>(0) == DenseNegative::MinusTwo);
  REQUIRE(enum_value<DenseNegative>(1) == DenseNegative::MinusOne);
  REQUIRE(enum_value<DenseNegative>(2) == DenseNegative::Zero);
  REQUIRE(enum_value<DenseNegative, 0>() == DenseNegative::MinusTwo);
  REQUIRE(enum_value<DenseNegative, 1>() == DenseNegative::MinusOne);
  REQUIRE(enum_value<DenseNegative, 2>() == DenseNegative::Zero);

  constexpr auto cr = enum_value<Color>(0);
  REQUIRE(cr == Color::RED);
  REQUIRE(enum_value<Color&>(1) == Color::GREEN);
  REQUIRE(enum_value<Color>(2) == Color::BLUE);

  REQUIRE(enum_value<Color, 0>() == Color::RED);
  REQUIRE(enum_value<Color, 1>() == Color::GREEN);
  REQUIRE(enum_value<Color, 2>() == Color::BLUE);

  constexpr auto no = enum_value<Numbers>(0);
  REQUIRE(no == Numbers::one);
  REQUIRE(enum_value<Numbers>(1) == Numbers::two);
  REQUIRE(enum_value<Numbers>(2) == Numbers::three);

  REQUIRE(enum_value<Numbers, 0>() == Numbers::one);
  REQUIRE(enum_value<Numbers, 1>() == Numbers::two);
  REQUIRE(enum_value<Numbers, 2>() == Numbers::three);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(enum_value<Numbers, 3>() == Numbers::many);
#endif

  constexpr auto dr = enum_value<Directions>(3);
  REQUIRE(enum_value<Directions&>(0) == Directions::Left);
  REQUIRE(enum_value<const Directions>(1) == Directions::Down);
  REQUIRE(enum_value<Directions>(2) == Directions::Up);
  REQUIRE(dr == Directions::Right);

  REQUIRE(enum_value<Directions, 0>() == Directions::Left);
  REQUIRE(enum_value<Directions, 1>() == Directions::Down);
  REQUIRE(enum_value<Directions, 2>() == Directions::Up);
  REQUIRE(enum_value<Directions, 3>() == Directions::Right);

  constexpr auto nt = enum_value<number>(2);
  REQUIRE(enum_value<number>(0) == number::one);
  REQUIRE(enum_value<number>(1) == number::two);
  REQUIRE(nt == number::three);

  REQUIRE(enum_value<number, 0>() == number::one);
  REQUIRE(enum_value<number, 1>() == number::two);
  REQUIRE(enum_value<number, 2>() == number::three);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(enum_value<number, 3>() == number::four);
#endif

  REQUIRE(enum_value<BoolTest>(0) == BoolTest::Yay);
  REQUIRE(enum_value<BoolTest, 0>() == BoolTest::Yay);
}

TEST_CASE("enum_values") {
  REQUIRE(std::is_same_v<decltype(enum_values<Color>()), const std::array<Color, 3>&>);

  constexpr auto& s1 = enum_values<Color&>();
  REQUIRE(s1 == std::array<Color, 3>{{Color::RED, Color::GREEN, Color::BLUE}});

  constexpr auto& s2 = enum_values<Numbers>();
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(s2 == std::array<Numbers, 4>{{Numbers::one, Numbers::two, Numbers::three, Numbers::many}});
#else
  REQUIRE(s2 == std::array<Numbers, 3>{{Numbers::one, Numbers::two, Numbers::three}});
#endif

  constexpr auto& s3 = enum_values<const Directions>();
  REQUIRE(s3 == std::array<Directions, 4>{{Directions::Left, Directions::Down, Directions::Up, Directions::Right}});

  constexpr auto& s4 = enum_values<number>();
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(s4 == std::array<number, 4>{{number::one, number::two, number::three, number::four}});
#else
  REQUIRE(s4 == std::array<number, 3>{{number::one, number::two, number::three}});
#endif

  constexpr auto& s5 = enum_values<Binary>();
  REQUIRE(s5 == std::array<Binary, 2>{{Binary::ONE, Binary::TWO}});

  constexpr auto& s6 = enum_values<MaxUsedAsInvalid>();
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(s6 == std::array<MaxUsedAsInvalid, 3>{{MaxUsedAsInvalid::ONE, MaxUsedAsInvalid::TWO, MaxUsedAsInvalid::INVALID}});
#else
  REQUIRE(s6 == std::array<MaxUsedAsInvalid, 2>{{MaxUsedAsInvalid::ONE, MaxUsedAsInvalid::TWO}});
#endif

  constexpr auto& s7 = enum_values<Dimension>();
  REQUIRE(s7 == std::array<Dimension, 3>{{Dimension::Overworld, Dimension::Nether, Dimension::TheEnd}});

  constexpr auto& s8 = enum_values<Flaggy>();
  REQUIRE(s8 == std::array<Flaggy, 2>{{Flaggy::Flag0, Flaggy::Flag32}});

}

TEST_CASE("friend range hook") {
  using Holder = FriendRangeHolder::Level;
  using Templated = FriendRangeTemplate<int>::Level;
  static_assert(enum_count<Holder>() == 2);
  static_assert(enum_count<Templated>() == 2);
  REQUIRE(enum_name(Holder::Level_High) == "High");
  REQUIRE(enum_name(Templated::Level_Low) == "Low");
  REQUIRE(enum_cast<Templated>("High") == Templated::Level_High);
}

TEST_CASE("enum_count") {
  constexpr auto s1 = enum_count<Color&>();
  REQUIRE(s1 == 3);

  constexpr auto s2 = enum_count<Numbers>();
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(s2 == 4);
#else
  REQUIRE(s2 == 3);
#endif

  constexpr auto s3 = enum_count<const Directions>();
  REQUIRE(s3 == 4);

  constexpr auto s4 = enum_count<number>();
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(s4 == 4);
#else
  REQUIRE(s4 == 3);
#endif

  constexpr auto s5 = enum_count<Binary>();
  REQUIRE(s5 == 2);

  constexpr auto s6 = enum_count<MaxUsedAsInvalid>();
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(s6 == 3);
#else
  REQUIRE(s6 == 2);
#endif

  REQUIRE(enum_count<TemplatedValue>() == 2);
}

TEST_CASE("enum_min_max") {
  constexpr auto color_min = enum_min<Color&>();
  constexpr auto color_max = enum_max<const Color>();
  REQUIRE(color_min == Color::RED);
  REQUIRE(color_max == Color::BLUE);

  REQUIRE(enum_min<Directions>() == Directions::Left);
  REQUIRE(enum_max<Directions>() == Directions::Right);
  REQUIRE(enum_min<number>() == number::one);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(enum_max<number>() == number::four);
#else
  REQUIRE(enum_max<number>() == number::three);
#endif
  REQUIRE(enum_min<Binary>() == Binary::ONE);
  REQUIRE(enum_max<Binary>() == Binary::TWO);
}

enum lt1 { s1, loooooooooooooooooooong1 };
enum lt2 : unsigned { s2, loooooooooooooooooooong2 };
enum class lt3 { s3, loooooooooooooooooooong3 };
enum class lt4 : unsigned { s4, loooooooooooooooooooong4 };
class foo1 {
 public:
  enum class lt5 { s5, loooooooooooooooooooong5 };
};
class foo2 {
 public:
  enum lt6 : int { s6, loooooooooooooooooooong6 };
};

namespace boo1 {
enum lt1 { s1, loooooooooooooooooooong1 };
enum lt2 : unsigned { s2, loooooooooooooooooooong2 };
enum class lt3 { s3, loooooooooooooooooooong3 };
enum class lt4 : unsigned { s4, loooooooooooooooooooong4 };
class foo1 {
 public:
  enum class lt5 { s5, loooooooooooooooooooong5 };
};
class foo2 {
 public:
  enum lt6 : int { s6, loooooooooooooooooooong6 };
};
} // namespace boo1

namespace boo2 {
namespace boo3 {
enum lt1 { s1, loooooooooooooooooooong1 };
enum lt2 : unsigned { s2, loooooooooooooooooooong2 };
enum class lt3 { s3, loooooooooooooooooooong3 };
enum class lt4 : unsigned { s4, loooooooooooooooooooong4 };
class foo1 {
 public:
  enum class lt5 { s5, loooooooooooooooooooong5 };
};
class foo2 {
 public:
  enum lt6 : int { s6, loooooooooooooooooooong6 };
};
} // namespace boo2::boo3
} // namespace boo2

namespace {
enum a_lt1 { s1, loooooooooooooooooooong1 };
enum a_lt2 : unsigned { s2, loooooooooooooooooooong2 };
enum class a_lt3 { s3, loooooooooooooooooooong3 };
enum class a_lt4 : unsigned { s4, loooooooooooooooooooong4 };
class a_foo1 {
 public:
  enum class a_lt5 { s5, loooooooooooooooooooong5 };
};
class a_foo2 {
 public:
  enum a_lt6 : int { s6, loooooooooooooooooooong6 };
};
} // namespace

typedef enum { TypedefAnonFirst, TypedefAnonSecond } TypedefAnon;
typedef enum : unsigned { TypedefAnonFixedFirst } TypedefAnonFixed;

enum { UnnamedFirst, UnnamedSecond };
// MSVC mangles unnamed enums of one namespace to the same name, so reflecting two of them fails to link.
namespace unnamed_fixed {
enum : unsigned char { UnnamedFixedFirst = 1, UnnamedFixedSecond = 2 };
} // namespace unnamed_fixed
using namespace unnamed_fixed;
namespace {
enum { UnnamedHiddenFirst, UnnamedHiddenSecond };
} // namespace
struct UnnamedScope {
  enum { First, Second } value;
};
template <typename>
struct UnnamedTemplateScope {
  enum { First = 1, Second = 2 } value;
};

template <typename E>
constexpr bool has_type_name = !magic_enum::detail::type_name_v<E>.str().empty();

enum class LargeNumbers {
    First = -1024,
    Second = 1024
};

TEST_CASE("enum_name") {
  SUBCASE("automatic storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = enum_name(cr);
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    Color cb = Color::BLUE;
    REQUIRE(cr_name == "red");
    REQUIRE(enum_name<Color&>(cb) == "BLUE");
    REQUIRE(enum_name<as_flags<false>>(cm[1]) == "GREEN");
    REQUIRE(enum_name<as_common<true>>(cm[1]) == "GREEN");
    REQUIRE(enum_name<as_flags<false>>(static_cast<Color>(0)).empty());
    static_assert(noexcept(enum_name<as_flags<false>>(Color::GREEN)));

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = enum_name(no);
    REQUIRE(no_name == "one");
    REQUIRE(enum_name<Numbers, as_flags<false>>(Numbers::two) == "two");
    REQUIRE(enum_name<as_flags<false>, Numbers>(Numbers::three) == "three");
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(enum_name(Numbers::many) == "many");
#else
    REQUIRE(enum_name(Numbers::many).empty());
#endif
    REQUIRE(enum_name(static_cast<Numbers>(0)).empty());

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = enum_name(dr);
    Directions du = Directions::Up;
    REQUIRE(enum_name<Directions&>(du) == "Up");
    REQUIRE(enum_name<const Directions>(Directions::Down) == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(enum_name(Directions::Left) == "Left");
    REQUIRE(enum_name(static_cast<Directions>(0)).empty());
    // The empty name of an invalid value must not point into a destroyed temporary.
    constexpr auto unknown_name = enum_name(static_cast<Directions>(0));
    static_assert(unknown_name.data()[0] == '\0');
    REQUIRE(enum_name(static_cast<Directions>(0)).data()[0] == '\0');

    constexpr number nt = number::three;
    constexpr auto nt_name = enum_name(nt);
    REQUIRE(enum_name(number::one) == "one");
    REQUIRE(enum_name(number::two) == "two");
    REQUIRE(nt_name == "three");
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(enum_name(number::four) == "four");
#else
    REQUIRE(enum_name(number::four).empty());
#endif
    REQUIRE(enum_name(static_cast<number>(0)).empty());

    REQUIRE(enum_name(MaxUsedAsInvalid::ONE) == "ONE");
    REQUIRE(enum_name(TemplatedValue::ONE) == "ONE");

    REQUIRE(enum_name(lt1::s1) == "s1");
    REQUIRE(enum_name(lt1::loooooooooooooooooooong1) == "loooooooooooooooooooong1");
    REQUIRE(enum_name(lt2::s2) == "s2");
    REQUIRE(enum_name(lt2::loooooooooooooooooooong2) == "loooooooooooooooooooong2");
    REQUIRE(enum_name(lt3::s3) == "s3");
    REQUIRE(enum_name(lt3::loooooooooooooooooooong3) == "loooooooooooooooooooong3");
    REQUIRE(enum_name(lt4::s4) == "s4");
    REQUIRE(enum_name(lt4::loooooooooooooooooooong4) == "loooooooooooooooooooong4");
    REQUIRE(enum_name(foo1::lt5::s5) == "s5");
    REQUIRE(enum_name(foo1::lt5::loooooooooooooooooooong5) == "loooooooooooooooooooong5");
    REQUIRE(enum_name(foo2::s6) == "s6");
    REQUIRE(enum_name(foo2::loooooooooooooooooooong6) == "loooooooooooooooooooong6");

    REQUIRE_FALSE(enum_name(static_cast<foo1::lt5>(12)).size());
    REQUIRE_FALSE(enum_name(static_cast<foo2::lt6>(12)).size());

    REQUIRE(enum_name(boo1::lt1::s1) == "s1");
    REQUIRE(enum_name(boo1::lt1::loooooooooooooooooooong1) == "loooooooooooooooooooong1");
    REQUIRE(enum_name(boo1::lt2::s2) == "s2");
    REQUIRE(enum_name(boo1::lt2::loooooooooooooooooooong2) == "loooooooooooooooooooong2");
    REQUIRE(enum_name(boo1::lt3::s3) == "s3");
    REQUIRE(enum_name(boo1::lt3::loooooooooooooooooooong3) == "loooooooooooooooooooong3");
    REQUIRE(enum_name(boo1::lt4::s4) == "s4");
    REQUIRE(enum_name(boo1::lt4::loooooooooooooooooooong4) == "loooooooooooooooooooong4");
    REQUIRE(enum_name(boo1::foo1::lt5::s5) == "s5");
    REQUIRE(enum_name(boo1::foo1::lt5::loooooooooooooooooooong5) == "loooooooooooooooooooong5");
    REQUIRE(enum_name(boo1::foo2::s6) == "s6");
    REQUIRE(enum_name(boo1::foo2::loooooooooooooooooooong6) == "loooooooooooooooooooong6");

    REQUIRE_FALSE(enum_name(static_cast<boo1::foo1::lt5>(12)).size());
    REQUIRE_FALSE(enum_name(static_cast<boo1::foo2::lt6>(12)).size());

    REQUIRE(enum_name(boo2::boo3::lt1::s1) == "s1");
    REQUIRE(enum_name(boo2::boo3::lt1::loooooooooooooooooooong1) == "loooooooooooooooooooong1");
    REQUIRE(enum_name(boo2::boo3::lt2::s2) == "s2");
    REQUIRE(enum_name(boo2::boo3::lt2::loooooooooooooooooooong2) == "loooooooooooooooooooong2");
    REQUIRE(enum_name(boo2::boo3::lt3::s3) == "s3");
    REQUIRE(enum_name(boo2::boo3::lt3::loooooooooooooooooooong3) == "loooooooooooooooooooong3");
    REQUIRE(enum_name(boo2::boo3::lt4::s4) == "s4");
    REQUIRE(enum_name(boo2::boo3::lt4::loooooooooooooooooooong4) == "loooooooooooooooooooong4");
    REQUIRE(enum_name(boo2::boo3::foo1::lt5::s5) == "s5");
    REQUIRE(enum_name(boo2::boo3::foo1::lt5::loooooooooooooooooooong5) == "loooooooooooooooooooong5");
    REQUIRE(enum_name(boo2::boo3::foo2::s6) == "s6");
    REQUIRE(enum_name(boo2::boo3::foo2::loooooooooooooooooooong6) == "loooooooooooooooooooong6");

    REQUIRE_FALSE(enum_name(static_cast<boo2::boo3::foo1::lt5>(12)).size());
    REQUIRE_FALSE(enum_name(static_cast<boo2::boo3::foo2::lt6>(12)).size());

    REQUIRE(enum_name(a_lt1::s1) == "s1");
    REQUIRE(enum_name(a_lt1::loooooooooooooooooooong1) == "loooooooooooooooooooong1");
    REQUIRE(enum_name(a_lt2::s2) == "s2");
    REQUIRE(enum_name(a_lt2::loooooooooooooooooooong2) == "loooooooooooooooooooong2");
    REQUIRE(enum_name(a_lt3::s3) == "s3");
    REQUIRE(enum_name(a_lt3::loooooooooooooooooooong3) == "loooooooooooooooooooong3");
    REQUIRE(enum_name(a_lt4::s4) == "s4");
    REQUIRE(enum_name(a_lt4::loooooooooooooooooooong4) == "loooooooooooooooooooong4");
    REQUIRE(enum_name(a_foo1::a_lt5::s5) == "s5");
    REQUIRE(enum_name(a_foo1::a_lt5::loooooooooooooooooooong5) == "loooooooooooooooooooong5");
    REQUIRE(enum_name(a_foo2::s6) == "s6");
    REQUIRE(enum_name(a_foo2::loooooooooooooooooooong6) == "loooooooooooooooooooong6");

    REQUIRE_FALSE(enum_name(static_cast<a_foo1::a_lt5>(12)).size());
    REQUIRE_FALSE(enum_name(static_cast<a_foo2::a_lt6>(12)).size());
  }

  SUBCASE("static storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = enum_name<cr>();
    constexpr Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    REQUIRE(cr_name == "red");
    REQUIRE(enum_name<Color::BLUE>() == "BLUE");
    REQUIRE(enum_name<cm[1]>() == "GREEN");

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = enum_name<no>();
    REQUIRE(no_name == "one");
    REQUIRE(enum_name<Numbers::two>() == "two");
    REQUIRE(enum_name<Numbers::three>() == "three");
    REQUIRE(enum_name<Numbers::many>() == "many");

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = enum_name<dr>();
    REQUIRE(enum_name<Directions::Up>() == "Up");
    REQUIRE(enum_name<Directions::Down>() == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(enum_name<Directions::Left>() == "Left");

    constexpr number nt = number::three;
    constexpr auto nt_name = enum_name<nt>();
    REQUIRE(enum_name<number::one>() == "one");
    REQUIRE(enum_name<number::two>() == "two");
    REQUIRE(nt_name == "three");
    REQUIRE(enum_name<number::four>() == "four");

    REQUIRE(enum_name<Binary::ONE>() == "ONE");
    REQUIRE(enum_name<MaxUsedAsInvalid::ONE>() == "ONE");

    // An unnamed value has no prefix to strip, so it must not trigger the prefix length check.
    static_assert(magic_enum::detail::enum_name_v<CStyleEnum, static_cast<CStyleEnum>(0)>.str().empty());
  }

  SUBCASE("declared values outside the compiler-specific range") {
    const auto ln_value = GENERATE(LargeNumbers::First, LargeNumbers::Second);
    const auto ln_name = enum_name(ln_value);

#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(ln_name == (ln_value == LargeNumbers::First ? "First" : "Second"));
#else
    REQUIRE(ln_name.empty());
#endif
  }
}

TEST_CASE("enum_names") {
  REQUIRE(std::is_same_v<decltype(enum_names<Color>()), const std::array<std::string_view, 3>&>);

  constexpr auto& s1 = enum_names<Color&>();
  REQUIRE(s1 == std::array<std::string_view, 3>{{"red", "GREEN", "BLUE"}});

  constexpr auto& s2 = enum_names<Numbers>();
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(s2 == std::array<std::string_view, 4>{{"one", "two", "three", "many"}});
#else
  REQUIRE(s2 == std::array<std::string_view, 3>{{"one", "two", "three"}});
#endif

  constexpr auto& s3 = enum_names<const Directions>();
  REQUIRE(s3 == std::array<std::string_view, 4>{{"Left", "Down", "Up", "Right"}});

  constexpr auto& s4 = enum_names<number>();
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(s4 == std::array<std::string_view, 4>{{"one", "two", "three", "four"}});
#else
  REQUIRE(s4 == std::array<std::string_view, 3>{{"one", "two", "three"}});
#endif
}

TEST_CASE("enum_entries") {
  REQUIRE(std::is_same_v<decltype(enum_entries<Color>()), const std::array<std::pair<Color, std::string_view>, 3>&>);

  constexpr auto& s1 = enum_entries<Color&>();
  REQUIRE(s1 == std::array<std::pair<Color, std::string_view>, 3>{{{Color::RED, "red"}, {Color::GREEN, "GREEN"}, {Color::BLUE, "BLUE"}}});

  constexpr auto& s2 = enum_entries<Numbers>();
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(s2 == std::array<std::pair<Numbers, std::string_view>, 4>{{{Numbers::one, "one"}, {Numbers::two, "two"}, {Numbers::three, "three"}, {Numbers::many, "many"}}});
#else
  REQUIRE(s2 == std::array<std::pair<Numbers, std::string_view>, 3>{{{Numbers::one, "one"}, {Numbers::two, "two"}, {Numbers::three, "three"}}});
#endif

  constexpr auto& s3 = enum_entries<Directions&>();
  REQUIRE(s3 == std::array<std::pair<Directions, std::string_view>, 4>{{{Directions::Left, "Left"}, {Directions::Down, "Down"}, {Directions::Up, "Up"}, {Directions::Right, "Right"}}});

  constexpr auto& s4 = enum_entries<number>();
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(s4 == std::array<std::pair<number, std::string_view>, 4>{{{number::one, "one"}, {number::two, "two"}, {number::three, "three"}, {number::four, "four"}}});
#else
  REQUIRE(s4 == std::array<std::pair<number, std::string_view>, 3>{{{number::one, "one"}, {number::two, "two"}, {number::three, "three"}}});
#endif

#if defined(__cpp_generic_lambdas) && __cpp_generic_lambdas >= 201707L
  constexpr auto check_entries = []<std::size_t... J>(std::index_sequence<J...>) {
    constexpr auto entries = magic_enum::enum_entries<Color>();
    return ((entries[J].first == magic_enum::enum_value<Color>(J)) && ...);
  };
  static_assert(check_entries(std::make_index_sequence<magic_enum::enum_count<Color>()>{}));
#endif
}

TEST_CASE("customization excludes Count with either reflection backend") {
  static_assert(enum_count<WithCount>() == 2);
  static_assert(enum_name(WithCount::First) == "First");
  static_assert(enum_name(WithCount::Second) == "Second");
  static_assert(enum_name(WithCount::Count).empty());
  static_assert(!enum_contains(WithCount::Count));
  static_assert(!enum_cast<WithCount>("Count").has_value());
  static_assert(!enum_index(WithCount::Count).has_value());
}

TEST_CASE("string_view lifetime and null termination") {
  std::string_view static_name{};
  static_name = enum_name<Color::BLUE>();
  require_null_terminated(static_name, "BLUE");

  std::string_view customized_name{};
  customized_name = enum_name(Color::RED);
  require_null_terminated(customized_name, "red");

  std::string_view value_name{};
  value_name = enum_name(Directions::Up);
  require_null_terminated(value_name, "Up");

  std::string_view invalid_name{};
  invalid_name = enum_name(static_cast<Numbers>(0));
  require_null_terminated(invalid_name, "");

  std::string_view type_name{};
  type_name = enum_type_name<Numbers>();
  require_null_terminated(type_name, "Numbers");

  std::string_view array_name{};
  array_name = enum_names<Color>()[1];
  require_null_terminated(array_name, "GREEN");

  std::string_view entry_name{};
  entry_name = enum_entries<Directions>()[3].second;
  require_null_terminated(entry_name, "Right");

  std::string_view prefixed_name{};
  prefixed_name = enum_name(CStyleEnum_A);
  require_null_terminated(prefixed_name, "A");

  std::string_view prefixed_entry_name{};
  prefixed_entry_name = enum_entries<CStyleEnum>()[0].second;
  require_null_terminated(prefixed_entry_name, "A");

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
  for (std::string_view name : enum_names<CStyleEnum>()) {
    require_null_terminated(name);
  }
  for (const auto& entry : enum_entries<CStyleEnum>()) {
    require_null_terminated(entry.second);
  }
}

TEST_CASE("type_traits") {
  REQUIRE(std::is_same_v<Enum<Color>, Color>);
  REQUIRE(std::is_same_v<underlying_type_t<const Color&>, int>);
  REQUIRE(std::is_same_v<underlying_type<Directions>::type, std::underlying_type_t<Directions>>);
  static_assert(detail::subtype_v<int> == detail::enum_subtype::common);
  static_assert(detail::subtype_v<Color> == detail::enum_subtype::common);
  static_assert(detail::subtype_v<const Color&> == detail::enum_subtype::common);

  REQUIRE_FALSE(is_unscoped_enum_v<Color>);
  REQUIRE_FALSE(is_unscoped_enum_v<const Color>);
  REQUIRE_FALSE(is_unscoped_enum_v<Color&>);
  REQUIRE_FALSE(is_unscoped_enum_v<Numbers>);
  REQUIRE(is_unscoped_enum_v<Directions>);
  REQUIRE(is_unscoped_enum_v<const Directions>);
  REQUIRE_FALSE(is_unscoped_enum_v<Directions&>);
  REQUIRE(is_unscoped_enum_v<number>);
  REQUIRE_FALSE(is_unscoped_enum_v<int>);

  REQUIRE(is_scoped_enum_v<Color>);
  REQUIRE(is_scoped_enum_v<const Color>);
  REQUIRE_FALSE(is_scoped_enum_v<Color&>);
  REQUIRE(is_scoped_enum_v<Numbers>);
  REQUIRE_FALSE(is_scoped_enum_v<Directions>);
  REQUIRE_FALSE(is_scoped_enum_v<number>);
  REQUIRE_FALSE(is_scoped_enum_v<int>);

  REQUIRE_FALSE(is_flags_enum<Color>::value);
  REQUIRE_FALSE(is_flags_enum_v<Numbers>);
  REQUIRE_FALSE(is_flags_enum_v<Directions>);
  REQUIRE_FALSE(is_flags_enum_v<int>);
}

struct MinusOperand {};

// MSVC prints a value of an enum local to a global operator- as `-::E::A`.
int operator-(MinusOperand, int) {
  enum class MinusLocal { A, B, C };
  static_assert(enum_count<MinusLocal>() == 3);
  static_assert(enum_name(MinusLocal::B) == "B");
  return 0;
}

TEST_CASE("enum local to a global operator-") {
  REQUIRE((MinusOperand{} - 1) == 0);
}

// Local enum type names, see #281.
string_view local_function_type_name() {
  enum class LocalInFunction { A };
  return enum_type_name<LocalInFunction>();
}

struct LocalTypeNameHolder {
  string_view member() const {
    enum class LocalInMember { A };
    return enum_type_name<LocalInMember>();
  }
};

template <typename T>
string_view local_template_type_name() {
  enum class LocalInTemplate { A };
  return enum_type_name<LocalInTemplate>();
}

TEST_CASE("enum_type_name of local enums") {
  REQUIRE(local_function_type_name() == "LocalInFunction");
  REQUIRE(LocalTypeNameHolder{}.member() == "LocalInMember");
  REQUIRE(local_template_type_name<int>() == "LocalInTemplate");
  const auto in_lambda = [] {
    enum class LocalInLambda { A };
    return enum_type_name<LocalInLambda>();
  };
  REQUIRE(in_lambda() == "LocalInLambda");
}

TEST_CASE("enum_type_name") {
  REQUIRE(enum_type_name<Color&>() == "Color");
  REQUIRE(enum_type_name<const Numbers>() == "Numbers");
  REQUIRE(enum_type_name<const Directions&>() == "Directions");
  REQUIRE(enum_type_name<number>() == "number");

  REQUIRE(enum_type_name<lt1>() == "lt1");
  REQUIRE(enum_type_name<lt2>() == "lt2");
  REQUIRE(enum_type_name<lt3>() == "lt3");
  REQUIRE(enum_type_name<lt4>() == "lt4");
  REQUIRE(enum_type_name<foo1::lt5>() == "lt5");
  REQUIRE(enum_type_name<foo2::lt6>() == "lt6");
  REQUIRE(enum_type_name<decltype(foo2::s6)>() == "lt6");

  REQUIRE(enum_type_name<boo1::lt1>() == "lt1");
  REQUIRE(enum_type_name<boo1::lt2>() == "lt2");
  REQUIRE(enum_type_name<boo1::lt3>() == "lt3");
  REQUIRE(enum_type_name<boo1::lt4>() == "lt4");
  REQUIRE(enum_type_name<boo1::foo1::lt5>() == "lt5");
  REQUIRE(enum_type_name<boo1::foo2::lt6>() == "lt6");
  REQUIRE(enum_type_name<decltype(boo1::foo2::s6)>() == "lt6");

  REQUIRE(enum_type_name<boo2::boo3::lt1>() == "lt1");
  REQUIRE(enum_type_name<boo2::boo3::lt2>() == "lt2");
  REQUIRE(enum_type_name<boo2::boo3::lt3>() == "lt3");
  REQUIRE(enum_type_name<boo2::boo3::lt4>() == "lt4");
  REQUIRE(enum_type_name<boo2::boo3::foo1::lt5>() == "lt5");
  REQUIRE(enum_type_name<boo2::boo3::foo2::lt6>() == "lt6");
  REQUIRE(enum_type_name<decltype(boo2::boo3::foo2::s6)>() == "lt6");

  REQUIRE(enum_type_name<a_lt1>() == "a_lt1");
  REQUIRE(enum_type_name<a_lt2>() == "a_lt2");
  REQUIRE(enum_type_name<a_lt3>() == "a_lt3");
  REQUIRE(enum_type_name<a_lt4>() == "a_lt4");
  REQUIRE(enum_type_name<a_foo1::a_lt5>() == "a_lt5");
  REQUIRE(enum_type_name<a_foo2::a_lt6>() == "a_lt6");
  REQUIRE(enum_type_name<decltype(a_foo2::s6)>() == "a_lt6");

  REQUIRE(enum_type_name<TypedefAnon>() == "TypedefAnon");
  REQUIRE(enum_type_name<TypedefAnonFixed>() == "TypedefAnonFixed");
  REQUIRE(enum_name(TypedefAnonSecond) == "TypedefAnonSecond");

  // Unnamed enums have no type name, but their values are reflected.
  static_assert(has_type_name<TypedefAnon>);
  static_assert(!has_type_name<decltype(UnnamedFirst)>);
  static_assert(!has_type_name<decltype(UnnamedFixedFirst)>);
  static_assert(!has_type_name<decltype(UnnamedHiddenFirst)>);
  static_assert(!has_type_name<decltype(UnnamedScope::value)>);
  static_assert(!has_type_name<decltype(UnnamedTemplateScope<int>::value)>);
  REQUIRE(enum_name(UnnamedSecond) == "UnnamedSecond");
  REQUIRE(enum_name(UnnamedFixedSecond) == "UnnamedFixedSecond");
  REQUIRE(enum_name(UnnamedHiddenSecond) == "UnnamedHiddenSecond");
  REQUIRE(enum_count<decltype(UnnamedScope::value)>() == 2);
  REQUIRE(enum_name(UnnamedTemplateScope<int>::Second) == "Second");
}

TEST_CASE("aliases") {
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(enum_count<number>() == 4);
#else
  REQUIRE(enum_count<number>() == 3);
#endif

  REQUIRE(enum_name(number::one) == enum_name(number::_1));
  REQUIRE(enum_name(number::two) == enum_name(number::_2));
  REQUIRE(enum_name(number::three) == enum_name(number::_3));
  REQUIRE(enum_name(number::four) == enum_name(number::_4));

  REQUIRE(enum_integer(number::one) == enum_integer(number::_1));
  REQUIRE(enum_integer(number::two) == enum_integer(number::_2));
  REQUIRE(enum_integer(number::three) == enum_integer(number::_3));
  REQUIRE(enum_integer(number::four) == enum_integer(number::_4));

  REQUIRE_FALSE(enum_cast<number>("_1").has_value());
  REQUIRE_FALSE(enum_cast<number>("_2").has_value());
  REQUIRE_FALSE(enum_cast<number>("_3").has_value());
  REQUIRE_FALSE(enum_cast<number>("_4").has_value());
}

TEST_CASE("extrema") {
  enum class BadColor : std::uint64_t {
    RED,
    GREEN,
    YELLOW,
    // Outside the default range, so the scanning backend must not count it in min and max.
    NONE = std::numeric_limits<std::uint64_t>::max()
  };

#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  REQUIRE(magic_enum::enum_name<BadColor>(BadColor::NONE) == "NONE");
  REQUIRE(enum_cast<BadColor>(std::numeric_limits<std::uint64_t>::max()) == BadColor::NONE);
  REQUIRE(magic_enum::enum_contains<BadColor>(std::numeric_limits<std::uint64_t>::max()));
  REQUIRE(magic_enum::enum_contains<BadColor>(BadColor::NONE));
#else
  REQUIRE(magic_enum::enum_name<BadColor>(BadColor::NONE).empty());
  REQUIRE_FALSE(enum_cast<BadColor>(std::numeric_limits<std::uint64_t>::max()).has_value());
  REQUIRE_FALSE(magic_enum::enum_contains<BadColor>(std::numeric_limits<std::uint64_t>::max()));
  REQUIRE_FALSE(magic_enum::enum_contains<BadColor>(BadColor::NONE));
#endif

  SUBCASE("min") {
    REQUIRE(magic_enum::customize::enum_range<BadColor>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min<BadColor, as_common<>>() == 0);
    REQUIRE(magic_enum::detail::min_v<BadColor, as_common<>> == 0);

    REQUIRE(magic_enum::customize::enum_range<Dimension>::min == 1000);
    REQUIRE(magic_enum::customize::enum_range<Dimension>::max == 1000 + 128);
    REQUIRE_FALSE(magic_enum::is_flags_enum_v<Dimension>);

    REQUIRE(magic_enum::customize::enum_range<Color>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min<Color, as_common<>>() == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::min_v<Color, as_common<>> == -12);

    REQUIRE(magic_enum::customize::enum_range<Numbers>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min<Numbers, as_common<>>() == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::min_v<Numbers, as_common<>> == 1);

    REQUIRE(magic_enum::customize::enum_range<Directions>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min<Directions, as_common<>>() == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::min_v<Directions, as_common<>> == -120);

    REQUIRE(static_cast<int>(magic_enum::customize::enum_range<number>::min) == 100);
    REQUIRE(magic_enum::detail::reflected_min<number, as_common<>>() == 100);
    REQUIRE(magic_enum::detail::min_v<number, as_common<>> == 100);

    REQUIRE(magic_enum::detail::reflected_min<Binary, as_common<>>() == false);
    REQUIRE(magic_enum::detail::min_v<Binary, as_common<>> == false);

    REQUIRE(magic_enum::detail::reflected_min<MaxUsedAsInvalid, as_common<>>() == 0);
    REQUIRE(magic_enum::detail::min_v<MaxUsedAsInvalid, as_common<>> == 0);
  }

  SUBCASE("max") {
    REQUIRE(magic_enum::customize::enum_range<BadColor>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max<BadColor, as_common<>>() == MAGIC_ENUM_RANGE_MAX);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(magic_enum::detail::max_v<BadColor, as_common<>> == std::numeric_limits<std::uint64_t>::max());
#else
    REQUIRE(magic_enum::detail::max_v<BadColor, as_common<>> == 2);
#endif

    REQUIRE(magic_enum::customize::enum_range<Color>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max<Color, as_common<>>() == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::max_v<Color, as_common<>> == 15);

    REQUIRE(magic_enum::customize::enum_range<Numbers>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max<Numbers, as_common<>>() == MAGIC_ENUM_RANGE_MAX);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(magic_enum::detail::max_v<Numbers, as_common<>> == 127);
#else
    REQUIRE(magic_enum::detail::max_v<Numbers, as_common<>> == 3);
#endif

    REQUIRE(magic_enum::customize::enum_range<Directions>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max<Directions, as_common<>>() == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::max_v<Directions, as_common<>> == 120);

    REQUIRE(static_cast<int>(magic_enum::customize::enum_range<number>::max) == 300);
    REQUIRE(magic_enum::detail::reflected_max<number, as_common<>>() == 300);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(magic_enum::detail::max_v<number, as_common<>> == 400);
#else
    REQUIRE(magic_enum::detail::max_v<number, as_common<>> == 300);
#endif

    REQUIRE(magic_enum::detail::reflected_max<Binary, as_common<>>() == true);
    REQUIRE(magic_enum::detail::max_v<Binary, as_common<>> == true);

    REQUIRE(magic_enum::detail::reflected_max<MaxUsedAsInvalid, as_common<>>() == 64);
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
    REQUIRE(magic_enum::detail::max_v<MaxUsedAsInvalid, as_common<>> == 255);
#else
    REQUIRE(magic_enum::detail::max_v<MaxUsedAsInvalid, as_common<>> == 63);
#endif
  }
}

TEST_CASE("cmp_less") {
  using magic_enum::detail::cmp_less;

  constexpr std::uint64_t uint64_t_min = std::numeric_limits<std::uint64_t>::min();
  constexpr std::uint32_t uint32_t_min = std::numeric_limits<std::uint32_t>::min();
  constexpr std::uint32_t uint32_t_max = std::numeric_limits<std::uint32_t>::max();
  constexpr std::uint64_t uint64_t_max = std::numeric_limits<std::uint64_t>::max();

  constexpr std::int64_t int64_t_min = std::numeric_limits<std::int64_t>::min();
  constexpr std::int32_t int32_t_min = std::numeric_limits<std::int32_t>::min();
  constexpr std::int32_t int32_t_max = std::numeric_limits<std::int32_t>::max();
  constexpr std::int64_t int64_t_max = std::numeric_limits<std::int64_t>::max();

  // Also testing with offset to avoid corner cases.
  // Two variables to avoid hidden casts:
  constexpr std::int64_t offset_int64_t = 17;
  constexpr std::int32_t offset_int32_t = 17;

  SUBCASE("same signedness") {
    REQUIRE(cmp_less(-5, -3));
    REQUIRE(cmp_less(27U, 49U));
  }

  SUBCASE("same signedness, different width") {
    REQUIRE(cmp_less(uint32_t_max, uint64_t_max));
    REQUIRE_FALSE(cmp_less(uint64_t_max, uint32_t_max));
    REQUIRE(cmp_less(int64_t_min, int32_t_min));
    REQUIRE_FALSE(cmp_less(int32_t_min, int64_t_min));
    REQUIRE(cmp_less(int64_t_min + offset_int64_t, int32_t_min + offset_int32_t));
    REQUIRE_FALSE(cmp_less(int32_t_min + offset_int32_t, int64_t_min + offset_int64_t));
  }

  SUBCASE("left signed, right unsigned") {
    REQUIRE(cmp_less(-5, 3U));
    REQUIRE(cmp_less(3, 5U));
  }

  SUBCASE("left signed, right unsigned, different width") {
    REQUIRE(cmp_less(int32_t_max, uint64_t_max));
    REQUIRE_FALSE(cmp_less(int64_t_max, uint32_t_max));
    REQUIRE(cmp_less(int32_t_min, uint64_t_min));
    REQUIRE(cmp_less(int64_t_min, uint32_t_min));
    REQUIRE(cmp_less(int32_t_max - offset_int32_t, uint64_t_max));
    REQUIRE_FALSE(cmp_less(int64_t_max - offset_int64_t, uint32_t_max));
    REQUIRE(cmp_less(int32_t_min + offset_int32_t, uint64_t_min));
    REQUIRE(cmp_less(int64_t_min + offset_int64_t, uint32_t_min));
  }

  SUBCASE("left unsigned, right signed") {
    REQUIRE_FALSE(cmp_less(3U, -5));
    REQUIRE(cmp_less(3U, 5));
  }

  SUBCASE("left unsigned, right signed, different width") {
    REQUIRE(cmp_less(uint32_t_max, int64_t_max));
    REQUIRE_FALSE(cmp_less(uint64_t_max, int32_t_max));
    REQUIRE_FALSE(cmp_less(uint32_t_min, int64_t_min));
    REQUIRE_FALSE(cmp_less(uint64_t_min, int32_t_min));
    REQUIRE(cmp_less(uint32_t_max, int64_t_max - offset_int32_t));
    REQUIRE_FALSE(cmp_less(uint64_t_max, int32_t_max - offset_int64_t));
    REQUIRE_FALSE(cmp_less(uint32_t_min, int64_t_min + offset_int32_t));
    REQUIRE_FALSE(cmp_less(uint64_t_min, int32_t_min + offset_int64_t));
  }

  SUBCASE("bool, right") {
    REQUIRE(cmp_less(true, 5));
    REQUIRE(cmp_less(false, 1));
    REQUIRE_FALSE(cmp_less(false, -1));
    REQUIRE(cmp_less(false, std::uint8_t{1}));
    REQUIRE_FALSE(cmp_less(true, std::uint8_t{1}));
  }

  SUBCASE("left, bool") {
    REQUIRE_FALSE(cmp_less(5, true));
    REQUIRE_FALSE(cmp_less(1, false));
    REQUIRE(cmp_less(-1, false));
    REQUIRE_FALSE(cmp_less(std::uint8_t{0}, false));
    REQUIRE_FALSE(cmp_less(std::uint8_t{1}, true));
  }

  REQUIRE_FALSE(cmp_less(false, false));
  REQUIRE(cmp_less(false, true));
}

TEST_CASE("valid_enum") {
  enum Forward2 : uint32_t;
  enum class Forward3;
  enum class Forward4 : uint32_t;
  enum Empty1 {};
  enum Empty2 : uint32_t {};
  enum class Empty3 {};
  enum class Empty4 : uint32_t {};

  REQUIRE(magic_enum::detail::is_reflected_v<Forward2, as_flags<true>>);
  REQUIRE(magic_enum::detail::is_reflected_v<Forward2, as_flags<false>>);
  REQUIRE(magic_enum::detail::is_reflected_v<Forward3, as_flags<true>>);
  REQUIRE(magic_enum::detail::is_reflected_v<Forward3, as_flags<false>>);
  REQUIRE(magic_enum::detail::is_reflected_v<Forward4, as_flags<true>>);
  REQUIRE(magic_enum::detail::is_reflected_v<Forward4, as_flags<false>>);
  REQUIRE(magic_enum::detail::is_reflected_v<Empty1, as_flags<true>>);
  REQUIRE(magic_enum::detail::is_reflected_v<Empty1, as_flags<false>>);
  REQUIRE(magic_enum::detail::is_reflected_v<Empty2, as_flags<true>>);
  REQUIRE(magic_enum::detail::is_reflected_v<Empty2, as_flags<false>>);
  REQUIRE(magic_enum::detail::is_reflected_v<Empty3, as_flags<true>>);
  REQUIRE(magic_enum::detail::is_reflected_v<Empty3, as_flags<false>>);
  REQUIRE(magic_enum::detail::is_reflected_v<Empty4, as_flags<true>>);
  REQUIRE(magic_enum::detail::is_reflected_v<Empty4, as_flags<false>>);
}

TEST_CASE("enum_reflected") {
  REQUIRE(enum_reflected<number>(number::one));
  REQUIRE(enum_reflected<number>(number::three));
  REQUIRE(enum_reflected<number>(100));
  REQUIRE(enum_reflected<number>(101));
  REQUIRE(enum_reflected<number>(234));
  REQUIRE(enum_reflected<number>(300));
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  // Standard reflection has no scan range, so every representable value is in the domain.
  REQUIRE(enum_reflected<number>(number::four));
  REQUIRE(enum_reflected<number>(400));
  REQUIRE(enum_reflected<number>(500));
#else
  REQUIRE_FALSE(enum_reflected<number>(number::four));
  REQUIRE_FALSE(enum_reflected<number>(400));
  REQUIRE_FALSE(enum_reflected<number>(500));
#endif
  REQUIRE_FALSE(enum_reflected<number>(-1));
}
