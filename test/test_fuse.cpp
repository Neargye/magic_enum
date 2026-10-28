// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <magic_enum/magic_enum_fuse.hpp>

#include <array>
#include <cstdint>
#include <type_traits>

enum class Color { RED, GREEN, BLUE };
enum class Direction { UP, DOWN };

constexpr auto red_up = magic_enum::enum_fuse(Color::RED, Direction::UP);
constexpr auto blue_down = magic_enum::enum_fuse(Color::BLUE, Direction::DOWN);

#if defined(MAGIC_ENUM_NO_TYPESAFE_ENUM_FUSE)
static_assert(std::is_same_v<decltype(red_up), const magic_enum::optional<std::uintmax_t>>);
#else
static_assert(std::is_enum_v<typename decltype(red_up)::value_type>);
#endif
static_assert(red_up.has_value());
static_assert(blue_down.has_value());
static_assert(*red_up != *blue_down);

enum class One { A };
enum class Two { A = -11, B = 23 };
enum class Three { A, B, C };
enum class Four { A, B, C, D };
enum class Five { A, B, C, D, E };
enum class Six { A, B, C, D, E, F };
enum class Eight { A, B, C, D, E, F, G, H };

// #386: check the public bijection rather than the bit-width formula.
template <typename E, typename F>
constexpr bool unique_fused_values() {
  using result_type = decltype(magic_enum::enum_fuse(magic_enum::enum_value<E, 0>(), magic_enum::enum_value<F, 0>()));
  std::array<typename result_type::value_type, magic_enum::enum_count<E>() * magic_enum::enum_count<F>()> seen{};
  std::size_t count = 0;
  for (auto e : magic_enum::enum_values<E>()) {
    for (auto f : magic_enum::enum_values<F>()) {
      const auto fused = magic_enum::enum_fuse(e, f);
      if (!fused) {
        return false;
      }
      for (std::size_t i = 0; i < count; ++i) {
        if (seen[i] == *fused) {
          return false;
        }
      }
      seen[count++] = *fused;
    }
  }
  return count == seen.size();
}

static_assert(unique_fused_values<One, Five>());
static_assert(unique_fused_values<Five, One>());
static_assert(unique_fused_values<Two, Three>());
static_assert(unique_fused_values<Three, Two>());
static_assert(unique_fused_values<Four, Three>());
static_assert(unique_fused_values<Five, Five>());
static_assert(unique_fused_values<Six, Three>());
static_assert(unique_fused_values<Two, Six>());
static_assert(unique_fused_values<Eight, Three>());

using forward_type = decltype(magic_enum::enum_fuse(Five::A, Two::A));
using reverse_type = decltype(magic_enum::enum_fuse(Two::A, Five::A));
using three_types = decltype(magic_enum::enum_fuse(Five::A, Two::A, One::A));
#if defined(MAGIC_ENUM_NO_TYPESAFE_ENUM_FUSE)
static_assert(std::is_same_v<forward_type, magic_enum::optional<std::uintmax_t>>);
static_assert(std::is_same_v<forward_type, reverse_type>);
static_assert(std::is_same_v<forward_type, three_types>);
#else
// #143: argument types, order, and arity belong to the fused type.
static_assert(!std::is_same_v<forward_type, reverse_type>);
static_assert(!std::is_same_v<forward_type, three_types>);
#endif

int main() {
  switch (static_cast<std::uintmax_t>(*magic_enum::enum_fuse(Color::BLUE, Direction::DOWN))) {
    case static_cast<std::uintmax_t>(*red_up):
      return 1;
    case static_cast<std::uintmax_t>(*blue_down):
      return 0;
    default:
      return 2;
  }
}
