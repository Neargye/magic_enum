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
static_assert(magic_enum::enum_count<ZeroOnly, magic_enum::as_flags<true>>() == 0);
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

// #451: zero is a supported probe, including when all values were filtered out.
template <typename E>
constexpr bool empty_reflection() {
  static_assert(magic_enum::enum_count<const E&>() == 0);
  static_assert(guarded_count<E>() == 0);
#  if defined(MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM)
  static_assert(magic_enum::enum_values<E>().empty());
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
#  else
  static_assert(!magic_enum::enum_reflected<E>(0));
#  endif
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

#else
#  error Unknown MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE.
#endif

int main() {}
