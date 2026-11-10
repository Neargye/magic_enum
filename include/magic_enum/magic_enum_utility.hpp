//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.9.8
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.
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
#if MAGIC_ENUM_VERSION_MAJOR != 0 || MAGIC_ENUM_VERSION_MINOR != 9 || MAGIC_ENUM_VERSION_PATCH != 8
#  error "magic_enum headers of different versions are mixed. Check the include paths."
#endif

#ifndef MAGIC_ENUM_USE_STD_MODULE
#  include <tuple>
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4459 6244) // Local names can match global names in user code.
#endif

namespace magic_enum {

namespace detail {

template <typename E, enum_subtype S, typename F, std::size_t J>
using enum_for_each_result_t = std::invoke_result_t<F&, enum_constant<values_v<E, S>[J]>>;

template <typename E, enum_subtype S, typename F, std::size_t... J>
constexpr auto for_each(F&& f, std::index_sequence<J...>) {
  if constexpr (sizeof...(J) == 0) {
    static_cast<void>(f);
  } else if constexpr ((std::is_invocable_v<F&, enum_constant<values_v<E, S>[J]>> && ...)) {
    if constexpr ((std::is_void_v<enum_for_each_result_t<E, S, F, J>> || ...)) {
      (static_cast<void>(detail::invoke_constant(f, enum_constant<values_v<E, S>[J]>{})), ...);
    } else if constexpr ((std::is_same_v<enum_for_each_result_t<E, S, F, 0>, enum_for_each_result_t<E, S, F, J>> && ...)) {
      return std::array<std::decay_t<enum_for_each_result_t<E, S, F, 0>>, sizeof...(J)>{{detail::invoke_constant(f, enum_constant<values_v<E, S>[J]>{})...}};
    } else {
      return std::tuple<std::decay_t<enum_for_each_result_t<E, S, F, J>>...>{detail::invoke_constant(f, enum_constant<values_v<E, S>[J]>{})...};
    }
  } else {
    static_assert(always_false_v<E>, "magic_enum::enum_for_each requires the callable to be invocable with every enum value.");
  }
}

template <typename E, enum_subtype S, bool Reverse>
constexpr E circular_value(E value, std::ptrdiff_t n) noexcept {
  constexpr std::ptrdiff_t count = count_v<E, S>;
  if constexpr (count != 0) {
    if (const auto i = index_of<E, S>(static_cast<std::underlying_type_t<E>>(value)); i < count_v<E, S>) {
      const auto offset = n % count;
      auto index = (static_cast<std::ptrdiff_t>(i) + (Reverse ? -offset : offset)) % count;
      if (index < 0) {
        index += count;
      }
      return enum_value<E, S>(static_cast<std::size_t>(index));
    }
  }
  return MAGIC_ENUM_ASSERT(false), value;
}

} // namespace magic_enum::detail

template <typename E, detail::enum_subtype S = detail::subtype_v<E>, typename F, detail::enable_if_t<E, int> = 0>
constexpr auto enum_for_each(F&& f) {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_for_each requires a non-empty reflected enum.");
  return detail::for_each<D, S>(std::forward<F>(f), std::make_index_sequence<detail::count_v<D, S>>{});
}

template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_next_value(E value, std::ptrdiff_t n = 1) noexcept -> detail::enable_if_t<E, optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_next_value requires a non-empty reflected enum.");
  constexpr std::ptrdiff_t count = detail::count_v<D, S>;

  if (const auto i = detail::index_of<D, S>(static_cast<underlying_type_t<D>>(value)); i < detail::count_v<D, S>) {
    const auto index = static_cast<std::ptrdiff_t>(i);
    if (n >= count - index || n < -index) {
      return {};
    }
    return enum_value<D, S>(static_cast<std::size_t>(index + n));
  }
  return {};
}

template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_next_value_circular(E value, std::ptrdiff_t n = 1) noexcept -> detail::enable_if_t<E, std::decay_t<E>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_next_value_circular requires a non-empty reflected enum.");
  return detail::circular_value<D, S, false>(value, n);
}

template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_prev_value(E value, std::ptrdiff_t n = 1) noexcept -> detail::enable_if_t<E, optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_prev_value requires a non-empty reflected enum.");
  constexpr std::ptrdiff_t count = detail::count_v<D, S>;

  if (const auto i = detail::index_of<D, S>(static_cast<underlying_type_t<D>>(value)); i < detail::count_v<D, S>) {
    const auto index = static_cast<std::ptrdiff_t>(i);
    if (n > index || n <= index - count) {
      return {};
    }
    return enum_value<D, S>(static_cast<std::size_t>(index - n));
  }
  return {};
}

template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_prev_value_circular(E value, std::ptrdiff_t n = 1) noexcept -> detail::enable_if_t<E, std::decay_t<E>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_prev_value_circular requires a non-empty reflected enum.");
  return detail::circular_value<D, S, true>(value, n);
}

} // namespace magic_enum

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif

#endif // NEARGYE_MAGIC_ENUM_UTILITY_HPP
