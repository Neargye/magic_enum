// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#ifndef MAGIC_ENUM_REFLECTION_COMPILE_FAIL_CASE
#  error MAGIC_ENUM_REFLECTION_COMPILE_FAIL_CASE must select compile-fail scenario.
#endif

#if MAGIC_ENUM_REFLECTION_COMPILE_FAIL_CASE == 5
#  include <string>
#  include <string_view>
#  define MAGIC_ENUM_USING_ALIAS_STRING using string = std::basic_string<unsigned char>;
#  define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = std::basic_string_view<unsigned char>;
#endif

#include <magic_enum/magic_enum.hpp>

#if MAGIC_ENUM_REFLECTION_COMPILE_FAIL_CASE == 1

enum class Synthetic { Declared = 1 };

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<Synthetic>(Synthetic value) noexcept {
  return value == Synthetic{2} ? customize_t{"Synthetic"} : default_tag;
}

constexpr auto synthetic_name = magic_enum::enum_name<Synthetic{2}>();

#elif MAGIC_ENUM_REFLECTION_COMPILE_FAIL_CASE == 2

enum class EqualPrefix { A };

template <>
struct magic_enum::customize::enum_range<EqualPrefix> {
  static constexpr std::size_t prefix_length = 1;
};

constexpr auto equal_prefix_name = magic_enum::enum_name<EqualPrefix::A>();

#elif MAGIC_ENUM_REFLECTION_COMPILE_FAIL_CASE == 3

enum class GreaterPrefix { A };

template <>
struct magic_enum::customize::enum_range<GreaterPrefix> {
  static constexpr std::size_t prefix_length = 2;
};

constexpr auto greater_prefix_names = magic_enum::enum_names<GreaterPrefix>();

#elif MAGIC_ENUM_REFLECTION_COMPILE_FAIL_CASE == 4

enum class ForwardDeclared : unsigned;

constexpr auto incomplete_values = magic_enum::enum_values<ForwardDeclared>();

#elif MAGIC_ENUM_REFLECTION_COMPILE_FAIL_CASE == 5

// Including the header selects standard reflection and diagnoses the unsupported character type.

#else

#  error Unknown MAGIC_ENUM_REFLECTION_COMPILE_FAIL_CASE.

#endif

int main() {}
