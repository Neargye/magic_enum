// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT

#if MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 1

#  define MAGIC_ENUM_NO_ASSERT
#  define MAGIC_ENUM_ASSERT(...) static_cast<void>(0)
#  include <magic_enum/magic_enum.hpp>

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 2

#  include <magic_enum/magic_enum.hpp>

enum class InvalidFlagsSetting { value };

template <>
struct magic_enum::customize::enum_range<InvalidFlagsSetting> {
  static constexpr int is_flags = 1;
};

static_assert(magic_enum::enum_count<InvalidFlagsSetting>() == 1);

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

// The empty name must be rejected even when no name is built.
constexpr auto count = magic_enum::enum_count<EmptyCustomName>();

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 10

#  include <magic_enum/magic_enum.hpp>

#  include <limits>

#  if MAGIC_ENUM_TEST_ENUM_FORM == 1
enum class RangeProbe : int { Value };
using RangeBound = int;
#  else
enum class RangeProbe : long long { Value };
using RangeBound = long long;
#  endif

template <>
struct magic_enum::customize::enum_range<RangeProbe> {
#  if MAGIC_ENUM_TEST_ENUM_FORM == 1
  static constexpr RangeBound min = (std::numeric_limits<int>::min)();
  static constexpr RangeBound max = (std::numeric_limits<int>::max)();
#  else
  // Must not be truncated to [0, 10].
  static constexpr RangeBound min = RangeBound{1} << 32;
  static constexpr RangeBound max = (RangeBound{1} << 32) + 10;
#  endif
};

constexpr auto count = magic_enum::enum_count<RangeProbe>();

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

#else
#  error Unknown MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE.
#endif

int main() {}
