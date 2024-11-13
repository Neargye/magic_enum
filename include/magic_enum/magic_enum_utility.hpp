//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.9.7
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2024 Daniil Goncharov <neargye@gmail.com>.
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

#ifndef NEARGYE_MAGIC_ENUM_UTILITY_HPP
#define NEARGYE_MAGIC_ENUM_UTILITY_HPP

#include "magic_enum.hpp"

namespace magic_enum {

namespace detail {

template <typename E, enum_subtype S, typename F, std::size_t... I>
constexpr auto for_each(F&& f, std::index_sequence<I...>) {
  constexpr bool has_void_return = (std::is_void_v<std::invoke_result_t<F, enum_constant<values_v<E, S>[I]>>> || ...);
  constexpr bool all_same_return = (std::is_same_v<std::invoke_result_t<F, enum_constant<values_v<E, S>[0]>>, std::invoke_result_t<F, enum_constant<values_v<E, S>[I]>>> && ...);

  if constexpr (has_void_return) {
    (f(enum_constant<values_v<E, S>[I]>{}), ...);
  } else if constexpr (all_same_return) {
    return std::array{f(enum_constant<values_v<E, S>[I]>{})...};
  } else {
    return std::tuple{f(enum_constant<values_v<E, S>[I]>{})...};
  }
}

template <typename E, enum_subtype S, typename F,std::size_t... I>
constexpr bool all_invocable(std::index_sequence<I...>) {
  if constexpr (count_v<E, S> == 0) {
    return false;
  } else {
    return (std::is_invocable_v<F, enum_constant<values_v<E, S>[I]>> && ...);
  }
}

} // namespace magic_enum::detail

template <typename E, detail::enum_subtype S = detail::subtype_v<E>, typename F, detail::enable_if_t<E, int> = 0>
constexpr auto enum_for_each(F&& f) {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_for_each requires enum type.");
  static_assert(detail::is_reflected_v<D, S>, "magic_enum requires enum implementation and valid max and min.");
  constexpr auto sep = std::make_index_sequence<detail::count_v<D, S>>{};

  if constexpr (detail::all_invocable<D, S, F>(sep)) {
    return detail::for_each<D, S>(std::forward<F>(f), sep);
  } else {
    static_assert(detail::always_false_v<D>, "magic_enum::enum_for_each requires invocable of all enum value.");
  }
}

template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_next_value(E value, std::ptrdiff_t n = 1) noexcept -> detail::enable_if_t<E, optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  constexpr std::ptrdiff_t count = detail::count_v<D, S>;

  if (const auto i = enum_index<D, S>(value)) {
    const std::ptrdiff_t index = (static_cast<std::ptrdiff_t>(*i) + n);
    if (index >= 0 && index < count) {
      return enum_value<D, S>(static_cast<std::size_t>(index));
    }
  }
  return {};
}

template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_next_value_circular(E value, std::ptrdiff_t n = 1) noexcept -> detail::enable_if_t<E, std::decay_t<E>> {
  using D = std::decay_t<E>;
  constexpr std::ptrdiff_t count = detail::count_v<D, S>;

  if (const auto i = enum_index<D, S>(value)) {
    const std::ptrdiff_t index = ((((static_cast<std::ptrdiff_t>(*i) + n) % count) + count) % count);
    if (index >= 0 && index < count) {
      return enum_value<D, S>(static_cast<std::size_t>(index));
    }
  }
  return MAGIC_ENUM_ASSERT(false), value;
}

template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_prev_value(E value, std::ptrdiff_t n = 1) noexcept -> detail::enable_if_t<E, optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  constexpr std::ptrdiff_t count = detail::count_v<D, S>;

  if (const auto i = enum_index<D, S>(value)) {
    const std::ptrdiff_t index = (static_cast<std::ptrdiff_t>(*i) - n);
    if (index >= 0 && index < count) {
      return enum_value<D, S>(static_cast<std::size_t>(index));
    }
  }
  return {};
}

template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_prev_value_circular(E value, std::ptrdiff_t n = 1) noexcept -> detail::enable_if_t<E, std::decay_t<E>> {
  using D = std::decay_t<E>;
  constexpr std::ptrdiff_t count = detail::count_v<D, S>;

  if (const auto i = enum_index<D, S>(value)) {
    const std::ptrdiff_t index = ((((static_cast<std::ptrdiff_t>(*i) - n) % count) + count) % count);
    if (index >= 0 && index < count) {
      return enum_value<D, S>(static_cast<std::size_t>(index));
    }
  }
  return MAGIC_ENUM_ASSERT(false), value;
}

} // namespace magic_enum

#endif // NEARGYE_MAGIC_ENUM_UTILITY_HPP
