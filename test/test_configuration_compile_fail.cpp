// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT

#if MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 1

#  define MAGIC_ENUM_NO_ASSERT
#  define MAGIC_ENUM_ASSERT(...) static_cast<void>(0)
#  include <magic_enum/magic_enum.hpp>

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 2

#  include <magic_enum/magic_enum_switch.hpp>

enum class InvalidFlagsSetting { value };

template <>
struct magic_enum::customize::enum_range<InvalidFlagsSetting> {
  static constexpr int is_flags = 1;
};

// An explicit subtype must not hide the check.
#  if !defined(MAGIC_ENUM_TEST_ENUM_FORM)
static_assert(magic_enum::enum_count<InvalidFlagsSetting>() == 1);
#  elif MAGIC_ENUM_TEST_ENUM_FORM == 1
static_assert(magic_enum::enum_count<InvalidFlagsSetting, magic_enum::as_common<>>() == 1);
#  elif MAGIC_ENUM_TEST_ENUM_FORM == 2
static_assert(magic_enum::enum_reflected<InvalidFlagsSetting, magic_enum::as_flags<>>(1));
#  elif MAGIC_ENUM_TEST_ENUM_FORM == 3
static_assert(magic_enum::enum_switch<int, magic_enum::as_common<>>([](auto) { return 1; }, InvalidFlagsSetting::value) == 1);
#  else
static_assert(magic_enum::enum_switch<int, magic_enum::as_flags<>>([](auto) { return 1; }, InvalidFlagsSetting::value, 0) == 1);
#  endif

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 3

#  define MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION
#  include <magic_enum/magic_enum.hpp>

#  if !defined(MAGIC_ENUM_TEST_RANGE_BOUNDARY)
#    define MAGIC_ENUM_TEST_RANGE_BOUNDARY 0
#  endif

#  if defined(MAGIC_ENUM_TEST_RANGE_UNFIXED)
// Clang before 20 probes an enum without a fixed underlying type with n() per value.
enum RangeProbe {
  InRange = MAGIC_ENUM_TEST_RANGE_BOUNDARY,
  Outside = MAGIC_ENUM_TEST_RANGE_BOUNDARY + MAGIC_ENUM_TEST_RANGE_OFFSET,
};
#  else
enum class RangeProbe : std::int64_t {
  InRange = MAGIC_ENUM_TEST_RANGE_BOUNDARY,
  Outside = static_cast<std::int64_t>(MAGIC_ENUM_TEST_RANGE_BOUNDARY) + MAGIC_ENUM_TEST_RANGE_OFFSET,
};
#  endif

template <>
struct magic_enum::customize::enum_range<RangeProbe> {
  static constexpr int min = MAGIC_ENUM_TEST_RANGE_BOUNDARY;
  static constexpr int max = MAGIC_ENUM_TEST_RANGE_BOUNDARY;
};

#  if defined(MAGIC_ENUM_TEST_RANGE_RENAMED)
// A renamed declared value is still named outside the range.
template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<RangeProbe>(RangeProbe value) noexcept {
  return value == RangeProbe::Outside ? customize_t{"Renamed"} : customize_t{default_tag};
}
#  endif

static_assert(magic_enum::enum_count<RangeProbe>() == 1);

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 4

#  define MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION
#  include <magic_enum/magic_enum.hpp>

enum class DefaultRangeProbe { InRange = 5, Outside = 129 };
static_assert(magic_enum::enum_count<DefaultRangeProbe>() == 1);

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 5

#  include <magic_enum/magic_enum.hpp>

#  if MAGIC_ENUM_TEST_ENUM_FORM == 1
enum class CountProbe;
#  elif MAGIC_ENUM_TEST_ENUM_FORM == 2
enum class CountProbe : unsigned;
#  elif MAGIC_ENUM_TEST_ENUM_FORM == 3
enum CountProbe : unsigned;
#  elif MAGIC_ENUM_TEST_ENUM_FORM == 4
enum class CountProbe {};
#  else
#    error Unknown MAGIC_ENUM_TEST_ENUM_FORM.
#  endif

auto enum_name_probe(CountProbe value) { return magic_enum::enum_name(value); }

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 6

#  include <magic_enum/magic_enum.hpp>
#  include <magic_enum/magic_enum_switch.hpp>
#  include <magic_enum/magic_enum_utility.hpp>

#  include <type_traits>

enum class DefinedEnum { Value };
enum class DefinedFlags { First = 1, Second = 2 };
enum class EmptyEnum {};
enum EmptyUnscoped {};
enum class EmptyUnsigned : unsigned {};
enum class EmptyBool : bool {};
enum class OutOfRange { Value = 1000 };
enum class FilteredEnum { Value };
enum class ZeroOnly { Zero = 0 };

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<FilteredEnum>(FilteredEnum) noexcept {
  return invalid_tag;
}

static_assert(magic_enum::enum_count<const DefinedEnum&>() == 1);
static_assert(magic_enum::enum_count<DefinedFlags, magic_enum::as_flags<true>>() == 2);
static_assert(magic_enum::enum_count<ZeroOnly>() == 1);
static_assert(magic_enum::enum_count<ZeroOnly, magic_enum::as_flags<true>>() == 1);
static_assert(magic_enum::enum_name<OutOfRange::Value>() == "Value");

template <typename E>
constexpr std::size_t guarded_count() {
  if constexpr (magic_enum::enum_count<E>() > 0) {
    return magic_enum::enum_values<E>().size();
  } else {
    return 0;
  }
}

struct ReturnsOne {
  template <typename V>
  constexpr int operator()(V) const noexcept { return 1; }
};

template <typename E>
constexpr int for_each_calls() {
  int calls = 0;
  magic_enum::enum_for_each<E>([&calls](auto) { ++calls; });
  return calls;
}

// enum_count() == 0 is a supported check, including when every value is filtered out, see #451.
template <typename E>
constexpr bool empty_reflection() {
  static_assert(magic_enum::enum_count<const E&>() == 0);
  static_assert(guarded_count<E>() == 0);
#  if defined(MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM)
  static_assert(magic_enum::enum_values<E>().empty());
  // MSVC 19.29 rejects a temporary empty array in a consteval return.
  static_assert(magic_enum::enum_names<E>().empty());
  static_assert(magic_enum::enum_entries<E>().empty());
  static_assert(magic_enum::enum_name(static_cast<E>(0)).empty());
  static_assert(!magic_enum::enum_index(static_cast<E>(0)).has_value());
  static_assert(!magic_enum::enum_cast<E>(0).has_value());
  static_assert(!magic_enum::enum_cast<E>(magic_enum::string_view{"Value"}).has_value());
  static_assert(for_each_calls<E>() == 0);
  static_assert(std::is_void_v<decltype(magic_enum::enum_for_each<E>(ReturnsOne{}))>);
  static_assert(std::is_void_v<decltype(magic_enum::enum_switch(ReturnsOne{}, static_cast<E>(0)))>);
  static_assert(magic_enum::enum_switch<int>(ReturnsOne{}, static_cast<E>(0)) == 0);
  static_assert(magic_enum::enum_switch(ReturnsOne{}, static_cast<E>(0), 2) == 2);
#  endif
  static_assert(magic_enum::enum_reflected<E>(0));
  return true;
}
static_assert(empty_reflection<EmptyEnum>());
static_assert(empty_reflection<EmptyUnscoped>());
static_assert(empty_reflection<EmptyUnsigned>());
static_assert(empty_reflection<EmptyBool>());
static_assert(empty_reflection<OutOfRange>());
static_assert(empty_reflection<FilteredEnum>());
static_assert(guarded_count<DefinedEnum>() == 1);

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 7

#  include <magic_enum/magic_enum_fuse.hpp>

enum class DefinedFuseEnum { Value };
enum class EmptyFuseEnum {};
constexpr auto fused = magic_enum::enum_fuse(DefinedFuseEnum::Value, static_cast<EmptyFuseEnum>(0));

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 8

#  include <magic_enum/magic_enum.hpp>

#  if MAGIC_ENUM_TEST_ENUM_FORM == 1
enum class ValuesProbe {};
#  else
enum class ValuesProbe { Value = 1000 };
#  endif
constexpr auto values = magic_enum::enum_values<ValuesProbe>();

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 9

#  include <magic_enum/magic_enum.hpp>

enum class EmptyCustomName { First, Second };

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<EmptyCustomName>(EmptyCustomName value) noexcept {
  return value == EmptyCustomName::Second ? customize_t{""} : default_tag;
}

#  if defined(MAGIC_ENUM_TEST_FORCE_FALLBACK)
template <>
constexpr bool magic_enum::detail::batch_supported<EmptyCustomName>() noexcept {
  return false;
}
#  endif

// The empty name must be rejected even when no name is built.
constexpr auto count = magic_enum::enum_count<EmptyCustomName>();

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 10

#  include <magic_enum/magic_enum.hpp>

#  include <limits>

#  if MAGIC_ENUM_TEST_ENUM_FORM == 2
enum class RangeProbe : long long { Value };
using RangeBound = long long;
#  else
enum class RangeProbe : int { Value };
using RangeBound = int;
#  endif

template <>
struct magic_enum::customize::enum_range<RangeProbe> {
#  if MAGIC_ENUM_TEST_ENUM_FORM == 1
  static constexpr RangeBound min = (std::numeric_limits<int>::min)();
  static constexpr RangeBound max = (std::numeric_limits<int>::max)();
#  elif MAGIC_ENUM_TEST_ENUM_FORM == 2
  // Must not be truncated to [0, 10].
  static constexpr RangeBound min = RangeBound{1} << 32;
  static constexpr RangeBound max = (RangeBound{1} << 32) + 10;
#  else
  static constexpr RangeBound min = 10;
  static constexpr RangeBound max = 5;
#  endif
};

// enum_reflected must reject the same enum_range as the scan.
#  if defined(MAGIC_ENUM_TEST_REFLECTED)
constexpr auto reflected = magic_enum::enum_reflected(RangeProbe::Value);
#  else
constexpr auto count = magic_enum::enum_count<RangeProbe>();
#  endif

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 11

#  include <magic_enum/magic_enum_utility.hpp>

enum class ForEachEnum { First, Second };

struct ForEachVisitor {
#  if MAGIC_ENUM_TEST_ENUM_FORM == 1
  template <typename T>
  void operator()(T) && {}
#  else
  void operator()(magic_enum::enum_constant<ForEachEnum::First>) const {}
#  endif
};

void for_each_probe() { magic_enum::enum_for_each<ForEachEnum>(ForEachVisitor{}); }

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 12

#  define __PRETTY_FUNCTION__ __FUNCTION__
#  include <magic_enum/magic_enum.hpp>

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 13

#  include <magic_enum/magic_enum_containers.hpp>

enum class OnlyZeroFlags : unsigned { None = 0 };

template <>
struct magic_enum::customize::enum_range<OnlyZeroFlags> {
  static constexpr bool is_flags = true;
};

static_assert(magic_enum::enum_count<OnlyZeroFlags>() == 1);
static_assert(magic_enum::containers::set<OnlyZeroFlags>{OnlyZeroFlags::None}.size() == 1);
magic_enum::containers::bitset<OnlyZeroFlags> only_zero_bits;

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 14

#  include <magic_enum/magic_enum_bitwise.hpp>

MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(int);

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 15

#  include <magic_enum/magic_enum_iostream.hpp>

MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS(int);

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 16

#  include <magic_enum/magic_enum_flags.hpp>

#  include <string>

enum class WideNameFlags { A = 1, B = 2 };

template <>
struct magic_enum::customize::enum_range<WideNameFlags> {
  static constexpr bool is_flags = true;
};

auto wide_flags_name = magic_enum::enum_flags_name<WideNameFlags, std::wstring>(WideNameFlags::A);

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 17

#  include <magic_enum/magic_enum_containers.hpp>

#  include <string>

enum class WideNameBits { A, B };

auto wide_bitset_name = magic_enum::containers::bitset<WideNameBits>{}.to_string<std::wstring>();

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 18

#  include <magic_enum/magic_enum_containers.hpp>

enum class EmptySetKeys {};

magic_enum::containers::set<EmptySetKeys> empty_keys;

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 19

#  include <magic_enum/magic_enum_utility.hpp>

enum class EmptyCircular {};

#  if MAGIC_ENUM_TEST_ENUM_FORM == 1
auto circular = magic_enum::enum_next_value_circular(EmptyCircular{});
#  else
auto circular = magic_enum::enum_prev_value_circular(EmptyCircular{});
#  endif

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 20

#  include <magic_enum/magic_enum.hpp>
enum class InvalidHook { A = 1 };
struct Config {};
struct FinalConfig final {};
#  if MAGIC_ENUM_TEST_ENUM_FORM == 1
void magic_enum_define_range_adl(InvalidHook);
#  elif MAGIC_ENUM_TEST_ENUM_FORM == 2
int magic_enum_define_range_adl(InvalidHook);
#  elif MAGIC_ENUM_TEST_ENUM_FORM == 3
const Config& magic_enum_define_range_adl(InvalidHook);
#  else
FinalConfig magic_enum_define_range_adl(InvalidHook);
#  endif
static_assert(magic_enum::enum_count<InvalidHook>() == 1);

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 21

#  define MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION
#  include <magic_enum/magic_enum.hpp>

enum class SyntheticName { Declared = 1 };

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<SyntheticName>(SyntheticName value) noexcept {
  return value == SyntheticName{2} ? customize_t{"Synthetic"} : customize_t{default_tag};
}

constexpr auto synthetic_name = magic_enum::enum_name<SyntheticName{2}>();

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 22

#  define MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION
#  include <magic_enum/magic_enum.hpp>

enum class UndeclaredZero : unsigned { A = 1, B = 2 };

#  if defined(MAGIC_ENUM_TEST_FLAGS)
template <>
struct magic_enum::customize::enum_range<UndeclaredZero> {
  static constexpr bool is_flags = true;
};
#  endif

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<UndeclaredZero>(UndeclaredZero value) noexcept {
  return value == UndeclaredZero{} ? customize_t{"None"} : customize_t{default_tag};
}

// Without the check the name is dropped and the count stays 2.
constexpr auto count = magic_enum::enum_count<UndeclaredZero>();

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 23

#  define MAGIC_ENUM_REQUIRE_STD_REFLECTION
#  include <magic_enum/magic_enum.hpp>

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 24

#  define MAGIC_ENUM_REQUIRE_STD_REFLECTION
#  define MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION
#  include <magic_enum/magic_enum.hpp>

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 25

#  include <magic_enum/magic_enum.hpp>

enum { UnnamedFirst, UnnamedSecond };

constexpr auto unnamed_type_name = magic_enum::enum_type_name<decltype(UnnamedFirst)>();

#else
#  error Unknown MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE.
#endif

int main() {}
