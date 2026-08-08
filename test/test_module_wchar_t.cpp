// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT

import magic_enum;

enum class Color { RED, GREEN, BLUE };

using String = magic_enum::string;
using StringView = magic_enum::string_view;
using Char = magic_enum::char_type;

static_assert(StringView{L"GREEN"}.size() == 5);
static_assert(sizeof(Char) == sizeof(wchar_t));
static_assert(magic_enum::enum_name(Color::BLUE).size() == 4);
static_assert(magic_enum::enum_name(Color::BLUE)[0] == L'B');

int main() {
  const String value{L"GREEN"};
  return value.size() == 5 ? 0 : 1;
}
