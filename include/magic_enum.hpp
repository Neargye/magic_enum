//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  vesion 0.3.0-dev
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

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <iosfwd>
#include <limits>
#include <string_view>
#include <optional>
#include <type_traits>
#include <utility>

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

template <typename E>
struct enum_range final {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::detail::enum_range require enum type.");
  using U = std::underlying_type_t<D>;
  using C = std::common_type_t<int, U>;
  static constexpr int min = std::max<C>(std::is_signed_v<U> ? -MAGIC_ENUM_RANGE : 0, std::numeric_limits<U>::min());
  static constexpr int max = std::min<C>(MAGIC_ENUM_RANGE, std::numeric_limits<U>::max());
  static constexpr auto sequence = std::make_integer_sequence<int, max - min + 1>{};

  static constexpr bool contains(int value) noexcept {
    return static_cast<int>(value) <= enum_range<D>::max && static_cast<int>(value) >= enum_range<D>::min;
  }
};

[[nodiscard]] constexpr bool is_name_char(char c, bool front) noexcept {
  return (!front && c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

template <typename E, E V>
[[nodiscard]] constexpr std::string_view name_impl() noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::name_impl require enum type.");
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

template <typename E, int... I>
[[nodiscard]] constexpr decltype(auto) strings_impl(std::integer_sequence<int, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::strings_impl require enum type.");
  constexpr std::array<std::string_view, sizeof...(I)> names{{name_impl<E, static_cast<E>(I + enum_range<E>::min)>()...}};

  return names;
}

template <typename E>
[[nodiscard]] constexpr std::string_view name_impl(int value) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::name_impl require enum type.");
  constexpr auto names = strings_impl<E>(enum_range<E>::sequence);

  return enum_range<E>::contains(value) ? names[value - enum_range<E>::min] : std::string_view{};
}

template <typename E, int... I>
[[nodiscard]] constexpr decltype(auto) values_impl(std::integer_sequence<int, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::values_impl require enum type.");
  constexpr int n = sizeof...(I);
  constexpr std::array<bool, n> valid{{!name_impl<E, static_cast<E>(I + enum_range<E>::min)>().empty()...}};
  constexpr int num_valid = ((valid[I] ? 1 : 0) + ...);

  std::array<E, num_valid> enums{};
  for (int i = 0, v = 0; i < n && v < num_valid; ++i) {
    if (valid[i]) {
      enums[v++] = static_cast<E>(i + enum_range<E>::min);
    }
  }

  return enums;
}

template <typename E, std::size_t... I>
[[nodiscard]] constexpr decltype(auto) names_impl(std::integer_sequence<std::size_t, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::names_impl require enum type.");
  constexpr auto enums = values_impl<E>(enum_range<E>::sequence);
  constexpr std::array<std::string_view, sizeof...(I)> names{{name_impl<E, enums[I]>()...}};

  return names;
}

template <typename E>
[[nodiscard]] constexpr std::optional<E> enum_cast_impl(std::string_view value) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::enum_cast_impl require enum type.");
  constexpr auto values = values_impl<E>(enum_range<E>::sequence);
  constexpr auto count = values.size();
  constexpr auto names = names_impl<E>(std::make_index_sequence<count>{});

  for (std::size_t i = 0; i < count; ++i) {
    if (names[i] == value) {
      return values[i];
    }
  }

  return std::nullopt; // Invalid value or out of range.
}

template<class E>
using enable_if_enum_t = typename std::enable_if<std::is_enum_v<std::decay_t<E>>>::type;

} // namespace magic_enum::detail

// Obtains enum value from enum string name.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr std::optional<E> enum_cast(std::string_view value) noexcept {
  using D = std::decay_t<E>;

  return detail::enum_cast_impl<D>(value);
}

// Obtains enum value from integer value.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr std::optional<E> enum_cast(std::underlying_type_t<E> value) noexcept {
  using D = std::decay_t<E>;

  if (detail::name_impl<D>(static_cast<int>(value)).empty()) {
    return std::nullopt; // Invalid value or out of range.
  } else {
    return static_cast<D>(value);
  }
}

// Obtains indexed access to enum element.
template<typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr E enum_value(std::size_t i) noexcept {
  using D = std::decay_t<E>;
  constexpr auto values = detail::values_impl<D>(detail::enum_range<D>::sequence);

  return assert(i < values.size()), values[i];
}

// Obtains value enum sequence.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr decltype(auto) enum_values() noexcept {
  using D = std::decay_t<E>;
  constexpr auto values = detail::values_impl<D>(detail::enum_range<D>::sequence);

  return values;
}

// Obtains number of enum elements.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr std::size_t enum_count() noexcept {
  using D = std::decay_t<E>;
  constexpr auto count = detail::values_impl<D>(detail::enum_range<D>::sequence).size();

  return count;
}

// Obtains string enum name from enum value.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr std::optional<std::string_view> enum_name(E value) noexcept {
  using D = std::decay_t<E>;
  const auto name = detail::name_impl<D>(static_cast<int>(value));

  if (name.empty()) {
    return std::nullopt;
  } else {
    return name; // Enum value does not have name or out of range.
  }
}

// Obtains string enum name sequence.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr decltype(auto) enum_names() noexcept {
  using D = std::decay_t<E>;
  constexpr auto count = detail::values_impl<D>(detail::enum_range<D>::sequence).size();
  constexpr auto names = detail::names_impl<D>(std::make_index_sequence<count>{});

  return names;
}

namespace ops {

template <typename E, typename = detail::enable_if_enum_t<E>>
std::ostream& operator<<(std::ostream& os, E value) {
  using D = std::decay_t<E>;
  const auto name = detail::name_impl<D>(static_cast<int>(value));

  if (!name.empty()) {
    os << name;
  }

  return os;
}

template <typename E, typename = detail::enable_if_enum_t<E>>
std::ostream& operator<<(std::ostream& os, std::optional<E> value) {
  using D = std::decay_t<E>;

  if (value.has_value()) {
    const auto name = detail::name_impl<D>(static_cast<int>(value.value()));
    if (!name.empty()) {
      os << name;
    }
  }

  return os;
}

} // namespace magic_enum::ops

} // namespace magic_enum
