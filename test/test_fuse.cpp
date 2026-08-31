// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <magic_enum/magic_enum_fuse.hpp>

#include <cstdint>
#include <type_traits>

enum class Color { RED, GREEN, BLUE };
enum class Direction { UP, DOWN };

constexpr auto red_up = magic_enum::enum_fuse(Color::RED, Direction::UP);
constexpr auto blue_down = magic_enum::enum_fuse(Color::BLUE, Direction::DOWN);

static_assert(std::is_same_v<decltype(red_up), const magic_enum::optional<std::uintmax_t>>);
static_assert(red_up.has_value());
static_assert(blue_down.has_value());
static_assert(*red_up != *blue_down);

int main() {
  switch (*magic_enum::enum_fuse(Color::BLUE, Direction::DOWN)) {
    case *red_up:
      return 1;
    case *blue_down:
      return 0;
    default:
      return 2;
  }
}
