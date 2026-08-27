// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_flags.hpp>

enum class 言語 : int { 日本語 = 10, 한국어 = 20, English = 30, TVÅ = 40 };

enum class LanguageFlag : int { 日本語 = 1, 한국어 = 2 };

template <>
struct magic_enum::customize::enum_range<LanguageFlag> {
  static constexpr bool is_flags = true;
};

static_assert(magic_enum::enum_type_name<言語>() == "言語");
static_assert(magic_enum::enum_name(言語::日本語) == "日本語");
static_assert(magic_enum::enum_cast<言語>("한국어") == 言語::한국어);

constexpr auto& language_names = magic_enum::enum_names<言語>();
static_assert(language_names.size() == 4);
static_assert(language_names[0] == "日本語");
static_assert(language_names[1] == "한국어");
static_assert(language_names[3] == "TVÅ");
static_assert(language_names[1].data()[language_names[1].size()] == '\0');

constexpr auto& language_entries = magic_enum::enum_entries<言語>();
static_assert(language_entries[0].first == 言語::日本語);
static_assert(language_entries[0].second == "日本語");
static_assert(language_entries[3].second == "TVÅ");
static_assert(language_entries[3].second.data()[language_entries[3].second.size()] == '\0');

int main() {
  const auto flags_name = magic_enum::enum_flags_name(static_cast<LanguageFlag>(3));
  return flags_name == "日本語|한국어" && flags_name.c_str()[flags_name.size()] == '\0' ? 0 : 1;
}
