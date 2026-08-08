// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT

import magic_enum;

enum class Color { RED, GREEN, BLUE };

using Optional = magic_enum::optional<int>;
using String = magic_enum::string;
using StringView = magic_enum::string_view;
using Char = magic_enum::char_type;

static_assert(Optional{42}.value() == 42);
static_assert(StringView{"GREEN"}.size() == 5);
static_assert(sizeof(Char) == sizeof(char));
static_assert(magic_enum::enum_name(Color::BLUE).size() == 4);

int main() {
  const String value{"GREEN", 5};
  return value.compare("GREEN");
}
