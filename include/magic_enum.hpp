//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  vesion 0.2.0
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 Daniil Goncharov <neargye@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstddef>
#include <type_traits>
#include <limits>
#include <string_view>
#include <optional>
#include <array>
#include <algorithm>

// Enum value must be in range (-MAGIC_ENUM_RANGE, MAGIC_ENUM_RANGE). If you need a larger range, redefine the macro MAGIC_ENUM_RANGE.
#if !defined(MAGIC_ENUM_RANGE)
#  define MAGIC_ENUM_RANGE 128
#endif

namespace magic_enum {

static_assert(MAGIC_ENUM_RANGE > 0,
              "MAGIC_ENUM_RANGE must be positive and greater than zero.");
static_assert(MAGIC_ENUM_RANGE < std::numeric_limits<int>::max(),
              "MAGIC_ENUM_RANGE must be less INT_MAX.");

namespace detail {

[[nodiscard]] constexpr bool is_name_char(char c, bool front) noexcept {
  return (!front && c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

template <typename E, E V>
[[nodiscard]] constexpr std::string_view enum_to_string_impl() noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::enum_to_string require enum type.");
#if defined(__clang__)
  std::string_view name{__PRETTY_FUNCTION__};
  constexpr auto suffix = sizeof("]") - 1;
#elif defined(__GNUC__) && __GNUC__ >= 9
  std::string_view name{__PRETTY_FUNCTION__};
  constexpr auto suffix = sizeof("; std::string_view = std::basic_string_view<char>]") - 1;
#elif defined(_MSC_VER)
  std::string_view name{__FUNCSIG__};
  constexpr auto suffix = sizeof(">(void) noexcept") - 1;
#else
  return {}; // Unsupported compiler.
#endif

#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9) || defined(_MSC_VER)
  name.remove_suffix(suffix);
  for (std::size_t i = name.size(); i > 0; --i) {
    if (!is_name_char(name[i - 1], false)) {
      name.remove_prefix(i);
      break;
    }
  }

  if (name.length() > 0 && is_name_char(name.front(), true)) {
    return name;
  } else {
    return {}; // Enum value does not have name.
  }
#endif
}

template <typename E, int O, int... I>
[[nodiscard]] constexpr std::string_view enum_to_string_impl(int value, std::integer_sequence<int, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::enum_to_string require enum type.");
  constexpr int n = sizeof...(I);
  constexpr std::array<std::string_view, n> names{{enum_to_string_impl<E, static_cast<E>(I + O)>()...}};

  return names[value - O];
}

template <typename E, int O, int... I>
[[nodiscard]] constexpr std::optional<E> enum_from_string_impl(std::string_view name, std::integer_sequence<int, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::enum_from_string require enum type.");
  std::optional<E> value{std::nullopt};
  (void)(((enum_to_string_impl<E, static_cast<E>(I + O)>() == name) ? (value = static_cast<E>(I + O), false) : true) && ...);

  return value;
}

template <typename E, int O, int... I>
[[nodiscard]] constexpr decltype(auto) enum_to_sequence_impl(std::integer_sequence<int, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::enum_to_sequence require enum type.");
  constexpr int n = sizeof...(I);
  constexpr std::array<bool, n> valid{{!enum_to_string_impl<E, static_cast<E>(I + O)>().empty()...}};
  constexpr int num_valid = ((valid[I] ? 1 : 0) + ...);

  std::array<E, num_valid> sequence{};
  for (int i = 0, v = 0; i < n && v < num_valid; ++i) {
    if (valid[i]) {
      sequence[v++] = static_cast<E>(i + O);
    }
  }

  return sequence;
}

template <typename E, int O, int... I>
[[nodiscard]] constexpr decltype(auto) enum_to_string_sequence_impl(std::integer_sequence<int, I...> s) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::enum_to_string_sequence require enum type.");
  constexpr int n = sizeof...(I);
  constexpr std::array<bool, n> valid{{!enum_to_string_impl<E, static_cast<E>(I + O)>().empty()...}};
  constexpr int num_valid = ((valid[I] ? 1 : 0) + ...);

  std::array<std::string_view, num_valid> sequence{};
  for (int i = 0, v = 0; i < n && v < num_valid; ++i) {
    if (valid[i]) {
      sequence[v++] = enum_to_string_impl<E, O>(i + O, s);
    }
  }

  return sequence;
}

} // namespace detail

// enum_to_string(enum) obtains string enum name from enum variable.
template <typename T, typename = std::enable_if_t<std::is_enum_v<std::decay_t<T>>>>
[[nodiscard]] constexpr std::optional<std::string_view> enum_to_string(T value) noexcept {
  using D = std::decay_t<T>;
  using U = std::underlying_type_t<D>;
  using C = std::common_type_t<int, U>;
  constexpr int min = std::max<C>(std::is_signed_v<U> ? -MAGIC_ENUM_RANGE : 0, std::numeric_limits<U>::min());
  constexpr int max = std::min<C>(MAGIC_ENUM_RANGE, std::numeric_limits<U>::max());
  constexpr int range = max - min + 1;

  if (static_cast<int>(value) > max || static_cast<int>(value) < min) {
    return std::nullopt; // Enum value out of range.
  }

  const auto name = detail::enum_to_string_impl<D, min>(static_cast<int>(value), std::make_integer_sequence<int, range>{});
  if (name.empty()) {
    return std::nullopt;
  } else {
    return name; // Enum value does not have name.
  }
}

// enum_to_string<enum>() obtains string enum name from static storage enum variable.
template <auto V, typename = std::enable_if_t<std::is_enum_v<std::decay_t<decltype(V)>>>>
[[nodiscard]] constexpr std::optional<std::string_view> enum_to_string() noexcept {
  constexpr auto name = detail::enum_to_string_impl<decltype(V), V>();

  if (name.empty()) {
    return std::nullopt;
  } else {
    return name; // Enum value does not have name.
  }
}

// enum_from_string(name) obtains enum value from enum string name.
template <typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
[[nodiscard]] constexpr std::optional<E> enum_from_string(std::string_view name) noexcept {
  using U = std::underlying_type_t<E>;
  using C = std::common_type_t<int, U>;
  constexpr int min = std::max<C>(std::is_signed_v<U> ? -MAGIC_ENUM_RANGE : 0, std::numeric_limits<U>::min());
  constexpr int max = std::min<C>(MAGIC_ENUM_RANGE, std::numeric_limits<U>::max());
  constexpr int range = max - min + 1;

  return detail::enum_from_string_impl<E, min>(name, std::make_integer_sequence<int, range>{});
}

// enum_to_sequence<enum>() obtains value enum sequence.
template <typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
[[nodiscard]] constexpr decltype(auto) enum_to_sequence() noexcept {
  using U = std::underlying_type_t<E>;
  using C = std::common_type_t<int, U>;
  constexpr int min = std::max<C>(std::is_signed_v<U> ? -MAGIC_ENUM_RANGE : 0, std::numeric_limits<U>::min());
  constexpr int max = std::min<C>(MAGIC_ENUM_RANGE, std::numeric_limits<U>::max());
  constexpr int range = max - min + 1;

  return detail::enum_to_sequence_impl<E, min>(std::make_integer_sequence<int, range>{});
}

// enum_to_string_sequence<enum>() obtains string enum name sequence.
template <typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
[[nodiscard]] constexpr decltype(auto) enum_to_string_sequence() noexcept {
  using U = std::underlying_type_t<E>;
  using C = std::common_type_t<int, U>;
  constexpr int min = std::max<C>(std::is_signed_v<U> ? -MAGIC_ENUM_RANGE : 0, std::numeric_limits<U>::min());
  constexpr int max = std::min<C>(MAGIC_ENUM_RANGE, std::numeric_limits<U>::max());
  constexpr int range = max - min + 1;

  return detail::enum_to_string_sequence_impl<E, min>(std::make_integer_sequence<int, range>{});
}

} // namespace magic_enum
