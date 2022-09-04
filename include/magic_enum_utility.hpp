//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.8.1
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2022 Daniil Goncharov <neargye@gmail.com>.
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

#include <tuple>

#include "magic_enum.hpp"

namespace magic_enum {

namespace detail {

template <typename E, typename F, std::size_t... I>
constexpr auto for_each(F&& f, std::index_sequence<I...>) {
  static_assert(is_enum_v<E>, "magic_enum::detail::for_each requires enum type.");
  constexpr bool has_void_return = (std::is_void_v<std::invoke_result_t<F, enum_constant<values_v<E>[I]>>> || ...);
  constexpr bool all_same_return = (std::is_same_v<std::invoke_result_t<F, enum_constant<values_v<E>[0]>>, std::invoke_result_t<F, enum_constant<values_v<E>[I]>>> && ...);

  if constexpr (has_void_return) {
    (f(enum_constant<values_v<E>[I]>{}), ...);
  } else if constexpr (all_same_return) {
    return std::array{f(enum_constant<values_v<E>[I]>{})...};
  } else {
    return std::tuple{f(enum_constant<values_v<E>[I]>{})...};
  }
}

template <typename E, typename F, std::size_t... I>
constexpr bool all_invocable(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::all_invocable requires enum type.");

  if constexpr (count_v<E> == 0) {
    return false;
  } else {
    return (std::is_invocable_v<F, enum_constant<values_v<E>[I]>> && ...);
  }
}

struct default_result_type {};

template <typename T>
struct identity {
  using type = T;
};

struct nonesuch {};

template <typename F, typename V, bool = std::is_invocable_v<F, V>>
struct invoke_result : identity<nonesuch> {};

template <typename F, typename V>
struct invoke_result<F, V, true> : std::invoke_result<F, V> {};

template <typename F, typename V>
using invoke_result_t = typename invoke_result<F, V>::type;

template <typename E, typename F, std::size_t... I>
constexpr auto common_invocable(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::invocable_index requires enum type.");

  if constexpr (count_v<E> == 0) {
    return identity<nonesuch>{};
  } else {
    return std::common_type<invoke_result_t<F, enum_constant<values_v<E>[I]>>...>{};
  }
}

template <typename E, typename F, typename R, std::size_t... I>
constexpr bool result_invocable(std::index_sequence<I...>) noexcept  {
  static_assert(is_enum_v<E>, "magic_enum::detail::result_invocable requires enum type.");

  if constexpr (count_v<E> == 0) {
    return false;
  } else {
    return ((std::is_same_v<invoke_result_t<F, enum_constant<values_v<E>[I]>>, nonesuch> || std::is_invocable_r_v<R, F, enum_constant<values_v<E>[I]>>) && ...);
  }
}

template <typename E, typename Result, typename F>
constexpr auto result_type() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::result_type requires enum type.");

  constexpr auto seq = std::make_index_sequence<detail::count_v<E>>{};
  using R = typename decltype(common_invocable<E, F>(seq))::type;
  if constexpr (std::is_same_v<R, nonesuch>) {
    if constexpr (std::is_same_v<Result, default_result_type>) {
      return identity<void>{};
    } else {
      return identity<nonesuch>{};
    }
  } else if constexpr (result_invocable<E, F, R>(seq)) {
    if constexpr (std::is_same_v<Result, default_result_type>) {
      return identity<R>{};
    } else if constexpr (std::is_convertible_v<R, Result>) {
      return identity<Result>{};
    } else if constexpr (std::is_convertible_v<Result, R>) {
      return identity<R>{};
    } else {
      return identity<nonesuch>{};
    }
  } else {
    if constexpr (std::is_same_v<Result, default_result_type>) {
      return identity<void>{};
    } else if constexpr (result_invocable<E, F, Result>(seq)) {
      return identity<Result>{};
    } else {
      return identity<nonesuch>{};
    }
  }
}

template <bool, typename R>
struct result {};

template <typename R>
struct result<true, R> {
  using type = R;
  static_assert(detail::has_hash<R>, "magic_enum::enum_switch requires no defined MAGIC_ENUM_NO_HASH.");
};

template <typename T, typename Result, typename F, typename BinaryPredicate = std::equal_to<>, typename D = std::decay_t<T>, typename R = typename decltype(result_type<D, Result, F>())::type>
using result_t = typename result<std::is_enum_v<D> && std::is_invocable_r_v<bool, BinaryPredicate, char, char> && !std::is_same_v<R, nonesuch>, R>::type;

} // namespace magic_enum::detail

template <typename E, typename Result = detail::default_result_type, typename F, typename R = detail::result_t<E, Result, F>>
constexpr decltype(auto) enum_switch(F&& f, E value) {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_switch requires enum type.");

  return detail::constexpr_switch<&detail::values_v<D>, detail::case_call_t::value>(
      std::forward<F>(f),
      value,
      detail::default_result_type_lambda<R>);
}

template <typename E, typename Result, typename F, typename R = detail::result_t<E, Result, F>>
constexpr R enum_switch(F&& f, E value, Result&& result) {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_switch requires enum type.");

  return detail::constexpr_switch<&detail::values_v<D>, detail::case_call_t::value>(
      std::forward<F>(f),
      value,
      [&result] { return std::forward<Result>(result); });
}

template <typename E, typename Result = detail::default_result_type, typename BinaryPredicate = std::equal_to<>, typename F, typename R = detail::result_t<E, Result, F, BinaryPredicate>>
constexpr decltype(auto) enum_switch(F&& f, string_view value, BinaryPredicate p = {}) {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_switch requires enum type.");
  static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>, "magic_enum::enum_switch requires bool(char, char) invocable predicate.");

  if (const auto v = enum_cast<D>(value, std::move(p))) {
    return enum_switch<D, Result>(std::forward<F>(f), *v);
  }
  return detail::default_result_type_lambda<R>();
}

template <typename E, typename Result, typename BinaryPredicate = std::equal_to<>, typename F, typename R = detail::result_t<E, Result, F, BinaryPredicate>>
constexpr R enum_switch(F&& f, string_view value, Result&& result, BinaryPredicate p = {}) {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_switch requires enum type.");
  static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>, "magic_enum::enum_switch requires bool(char, char) invocable predicate.");

  if (const auto v = enum_cast<D>(value, std::move(p))) {
    return enum_switch<D, Result>(std::forward<F>(f), *v, std::forward<Result>(result));
  }
  return std::forward<Result>(result);
}

template <typename E, typename Result = detail::default_result_type, typename F, typename R = detail::result_t<E, Result, F>>
constexpr decltype(auto) enum_switch(F&& f, underlying_type_t<E> value) {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_switch requires enum type.");

  if (const auto v = enum_cast<D>(value)) {
    return enum_switch<D, Result>(std::forward<F>(f), *v);
  }
  return detail::default_result_type_lambda<R>();
}

template <typename E, typename Result, typename F, typename R = detail::result_t<E, Result, F>>
constexpr R enum_switch(F&& f, underlying_type_t<E> value, Result&& result) {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_switch requires enum type.");

  if (const auto v = enum_cast<D>(value)) {
    return enum_switch<D, Result>(std::forward<F>(f), *v, std::forward<Result>(result));
  }
  return std::forward<Result>(result);
}

template <typename E, typename F, detail::enable_if_t<E, int> = 0>
constexpr auto enum_for_each(F&& f) {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_for_each requires enum type.");
  constexpr auto seq = std::make_index_sequence<detail::count_v<D>>{};

  if constexpr (detail::all_invocable<D, F>(seq)) {
    return detail::for_each<D>(std::forward<F>(f), seq);
  } else {
    static_assert(detail::always_false_v<D>, "magic_enum::enum_for_each requires invocable of all enum value.");
  }
}

} // namespace magic_enum

template <>
struct std::common_type<magic_enum::detail::nonesuch, magic_enum::detail::nonesuch> : magic_enum::detail::identity<magic_enum::detail::nonesuch> {};

template <typename T>
struct std::common_type<T, magic_enum::detail::nonesuch> : magic_enum::detail::identity<T> {};

template <typename T>
struct std::common_type<magic_enum::detail::nonesuch, T> : magic_enum::detail::identity<T> {};

#endif // NEARGYE_MAGIC_ENUM_UTILITY_HPP
