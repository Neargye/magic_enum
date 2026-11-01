// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <string>
#include <string_view>

#if MAGIC_ENUM_TEST_CHARACTER_WIDTH == 8
#  define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = std::u8string_view;
#  define MAGIC_ENUM_USING_ALIAS_STRING      using string      = std::u8string;
#  define MAGIC_ENUM_TEST_LITERAL(x)         u8##x
#elif MAGIC_ENUM_TEST_CHARACTER_WIDTH == 16
#  define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = std::u16string_view;
#  define MAGIC_ENUM_USING_ALIAS_STRING      using string      = std::u16string;
#  define MAGIC_ENUM_TEST_LITERAL(x)         u##x
#elif MAGIC_ENUM_TEST_CHARACTER_WIDTH == 32
#  define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = std::u32string_view;
#  define MAGIC_ENUM_USING_ALIAS_STRING      using string      = std::u32string;
#  define MAGIC_ENUM_TEST_LITERAL(x)         U##x
#else
#  error MAGIC_ENUM_TEST_CHARACTER_WIDTH must be 8, 16, or 32.
#endif

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_flags.hpp>

#include "test_fixed_string.hpp"

#include <array>
#include <type_traits>

constexpr magic_enum::char_type custom_name[] = {'R', 'e', 'n', 'a', 'm', 'e', 'd'};
enum class CharacterNames { First = 1, Second = 2 };
enum class CharacterWideValue { Low = 1, High = 1'000'000 };
enum class CharacterFlags { A = 1, B = 2, AB = 3 };

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<CharacterNames>(CharacterNames value) noexcept {
  if (value == CharacterNames::First) {
    return magic_enum::string_view{custom_name, std::size(custom_name)};
  }
  return default_tag;
}

template <>
struct magic_enum::customize::enum_range<CharacterFlags> {
  static constexpr bool is_flags = true;
};

#if MAGIC_ENUM_TEST_CHARACTER_WIDTH == 8
static_assert(std::is_same_v<magic_enum::char_type, char8_t>);
#elif MAGIC_ENUM_TEST_CHARACTER_WIDTH == 16
static_assert(std::is_same_v<magic_enum::char_type, char16_t>);
#else
static_assert(std::is_same_v<magic_enum::char_type, char32_t>);
#endif

static_assert(magic_enum::enum_type_name<CharacterNames>() == MAGIC_ENUM_TEST_LITERAL("CharacterNames"));
static_assert(magic_enum::enum_name(CharacterNames::First) == MAGIC_ENUM_TEST_LITERAL("Renamed"));
static_assert(magic_enum::enum_name<CharacterNames::Second>() == MAGIC_ENUM_TEST_LITERAL("Second"));
static_assert(magic_enum::enum_cast<CharacterNames>(MAGIC_ENUM_TEST_LITERAL("Second")) == CharacterNames::Second);

constexpr auto& character_names = magic_enum::enum_names<CharacterNames>();
static_assert(character_names == std::array<magic_enum::string_view, 2>{MAGIC_ENUM_TEST_LITERAL("Renamed"), MAGIC_ENUM_TEST_LITERAL("Second")});
static_assert(character_names[0].data()[character_names[0].size()] == magic_enum::char_type{});

constexpr auto& character_entries = magic_enum::enum_entries<CharacterNames>();
static_assert(character_entries[0].first == CharacterNames::First);
static_assert(character_entries[1].second == MAGIC_ENUM_TEST_LITERAL("Second"));
static_assert(character_entries[1].second.data()[character_entries[1].second.size()] == magic_enum::char_type{});

#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
static_assert(magic_enum::enum_count<CharacterWideValue>() == 2);
static_assert(magic_enum::enum_name(CharacterWideValue::High) == MAGIC_ENUM_TEST_LITERAL("High"));
#else
static_assert(magic_enum::enum_count<CharacterWideValue>() == 1);
static_assert(magic_enum::enum_name(CharacterWideValue::High).empty());
#endif

static_assert(magic_enum::enum_flags_cast<CharacterFlags>(MAGIC_ENUM_TEST_LITERAL("A|B")) == CharacterFlags::AB);
static_assert(!magic_enum::enum_flags_cast<CharacterFlags>(MAGIC_ENUM_TEST_LITERAL("A|Unknown")).has_value());
static_assert(magic_enum::enum_flags_name<CharacterFlags, magic_enum_tests::FixedString<8, magic_enum::char_type>>(CharacterFlags::AB) == MAGIC_ENUM_TEST_LITERAL("A|B"));

int main() {
  const auto flags_name = magic_enum::enum_flags_name(CharacterFlags::AB);
  return flags_name == MAGIC_ENUM_TEST_LITERAL("A|B") && flags_name.c_str()[flags_name.size()] == magic_enum::char_type{} ? 0 : 1;
}

#undef MAGIC_ENUM_TEST_LITERAL
