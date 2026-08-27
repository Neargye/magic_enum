// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT

import magic_enum;

enum class Color { RED, GREEN, BLUE };

using Optional = magic_enum::optional<int>;
using StringView = magic_enum::string_view;
using Char = magic_enum::char_type;

static_assert(Optional{42}.value() == 42);
static_assert(StringView{"GREEN"}.size() == 5);
static_assert(sizeof(Char) == sizeof(char));
static_assert(magic_enum::enum_name(Color::BLUE).size() == 4);

int main() {
  const magic_enum::containers::bitset<Color> colors{Color::RED, Color::BLUE};
  const auto names = colors.to_string();
  const auto raw = colors.to_string(magic_enum::containers::raw_access);
  const auto names_match = StringView{names.data(), names.size()} == StringView{"RED|BLUE"};
  const auto raw_match = StringView{raw.data(), raw.size()} == StringView{"101"};
  return names_match && raw_match ? 0 : 1;
}
