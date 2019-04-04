//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  vesion 0.1.1
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

// Enum variable must be in range (-MAGIC_ENUM_RANGE, MAGIC_ENUM_RANGE). If you need a larger range, redefine the macro MAGIC_ENUM_RANGE.
#if !defined(MAGIC_ENUM_RANGE)
#  define MAGIC_ENUM_RANGE 128
#endif

namespace magic_enum {

static_assert(MAGIC_ENUM_RANGE > 0,
              "MAGIC_ENUM_RANGE must be positive and greater than zero.");
static_assert(MAGIC_ENUM_RANGE % 8 == 0,
              "MAGIC_ENUM_RANGE must be a multiple of 8.");
static_assert(MAGIC_ENUM_RANGE < std::numeric_limits<int>::max(),
              "MAGIC_ENUM_RANGE must be less INT_MAX.");

namespace detail {

[[nodiscard]] constexpr bool is_name_char(char c, bool front) noexcept {
  return (!front && c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

template <typename E, E V>
[[nodiscard]] constexpr std::optional<std::string_view> enum_to_string_impl() noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::enum_to_string require enum type.");
#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9)
  std::string_view name{__PRETTY_FUNCTION__};
  constexpr auto suffix = sizeof("]") - 1;
#elif defined(_MSC_VER)
  std::string_view name{__FUNCSIG__};
  constexpr auto suffix = sizeof(">(void) noexcept") - 1;
#else
  return std::nullopt; // Unsupported compiler.
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
    return std::nullopt; // Enum variable does not have name.
  }
#endif
}

template <typename E, int V>
struct enum_to_string_impl_t final {
  [[nodiscard]] constexpr std::optional<std::string_view> operator()(int value) const noexcept {
    static_assert(std::is_enum_v<E>, "magic_enum::enum_to_string require enum type.");
    if constexpr (V > std::numeric_limits<std::underlying_type_t<E>>::max()) {
      return std::nullopt; // Enum variable out of range.
    }

    switch (value - V) {
      case 0:
        return enum_to_string_impl<E, static_cast<E>(V)>();
      case 1:
        return enum_to_string_impl<E, static_cast<E>(V + 1)>();
      case 2:
        return enum_to_string_impl<E, static_cast<E>(V + 2)>();
      case 3:
        return enum_to_string_impl<E, static_cast<E>(V + 3)>();
      case 4:
        return enum_to_string_impl<E, static_cast<E>(V + 4)>();
      case 5:
        return enum_to_string_impl<E, static_cast<E>(V + 5)>();
      case 6:
        return enum_to_string_impl<E, static_cast<E>(V + 6)>();
      case 7:
        return enum_to_string_impl<E, static_cast<E>(V + 7)>();
      default:
        return enum_to_string_impl_t<E, V + 8>{}(value);
    }
  }
};

template <typename E>
struct enum_to_string_impl_t<E, MAGIC_ENUM_RANGE> final {
  [[nodiscard]] constexpr std::optional<std::string_view> operator()(int) const noexcept {
    static_assert(std::is_enum_v<E>, "magic_enum::enum_to_string require enum type.");
    return std::nullopt; // Enum variable out of range MAGIC_ENUM_RANGE.
  }
};

template <typename E, int V>
struct enum_from_string_impl_t final {
  [[nodiscard]] constexpr std::optional<E> operator()(std::string_view name) const noexcept {
    static_assert(std::is_enum_v<E>, "magic_enum::enum_from_string require enum type.");
    if constexpr (V > std::numeric_limits<std::underlying_type_t<E>>::max()) {
      return std::nullopt; // Enum variable out of range.
    }

    if (enum_to_string_impl<E, static_cast<E>(V)>() == name) {
      return static_cast<E>(V);
    } else if (enum_to_string_impl<E, static_cast<E>(V + 1)>() == name) {
      return static_cast<E>(V + 1);
    } else if (enum_to_string_impl<E, static_cast<E>(V + 2)>() == name) {
      return static_cast<E>(V + 2);
    } else if (enum_to_string_impl<E, static_cast<E>(V + 3)>() == name) {
      return static_cast<E>(V + 3);
    } else if (enum_to_string_impl<E, static_cast<E>(V + 4)>() == name) {
      return static_cast<E>(V + 4);
    } else if (enum_to_string_impl<E, static_cast<E>(V + 5)>() == name) {
      return static_cast<E>(V + 5);
    } else if (enum_to_string_impl<E, static_cast<E>(V + 6)>() == name) {
      return static_cast<E>(V + 6);
    } else if (enum_to_string_impl<E, static_cast<E>(V + 7)>() == name) {
      return static_cast<E>(V + 7);
    } else {
      return enum_from_string_impl_t<E, V + 8>{}(name);
    }
  }
};

template <typename E>
struct enum_from_string_impl_t<E, MAGIC_ENUM_RANGE> final {
  [[nodiscard]] constexpr std::optional<E> operator()(std::string_view) const noexcept {
    static_assert(std::is_enum_v<E>, "magic_enum::enum_from_string require enum type.");
    return std::nullopt; // Enum variable out of range MAGIC_ENUM_RANGE.
  }
};

} // namespace detail

// enum_to_string(enum) obtains string enum name from enum variable.
template <typename T, typename = std::enable_if_t<std::is_enum_v<std::decay_t<T>>>>
[[nodiscard]] constexpr std::optional<std::string_view> enum_to_string(T value) noexcept {
  constexpr bool s = std::is_signed_v<std::underlying_type_t<std::decay_t<T>>>;
  constexpr int min = s ? -MAGIC_ENUM_RANGE : 0;
  if (static_cast<int>(value) >= MAGIC_ENUM_RANGE || static_cast<int>(value) <= -MAGIC_ENUM_RANGE) {
    return std::nullopt; // Enum variable out of range MAGIC_ENUM_RANGE.
  }
  return detail::enum_to_string_impl_t<std::decay_t<T>, min>{}(static_cast<int>(value));
}

// enum_to_string<enum>() obtains string enum name from static storage enum variable.
template <auto V, typename = std::enable_if_t<std::is_enum_v<std::decay_t<decltype(V)>>>>
[[nodiscard]] constexpr std::optional<std::string_view> enum_to_string() noexcept {
  return detail::enum_to_string_impl<decltype(V), V>();
}

// enum_from_string(name) obtains enum variable from enum string name.
template <typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
[[nodiscard]] constexpr std::optional<E> enum_from_string(std::string_view name) noexcept {
  constexpr bool s = std::is_signed_v<std::underlying_type_t<E>>;
  constexpr int min = s ? -MAGIC_ENUM_RANGE : 0;
  return detail::enum_from_string_impl_t<E, min>{}(name);
}

} // namespace magic_enum
