//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.8.2
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

#ifndef NEARGYE_MAGIC_ENUM_SWITCH_HPP
#define NEARGYE_MAGIC_ENUM_SWITCH_HPP

#include "magic_enum.hpp"

namespace magic_enum {

namespace detail {

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

template <typename E, typename Result, typename F>
constexpr auto result_type() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::result_type requires enum type.");

  constexpr auto seq = std::make_index_sequence<detail::count_v<E>>{};
  using R = typename decltype(common_invocable<E, F>(seq))::type;
  if constexpr (std::is_same_v<Result, default_result_type>) {
    if constexpr (std::is_same_v<R, nonesuch>) {
      return identity<void>{};
    } else {
      return identity<R>{};
    }
  } else {
    if constexpr (std::is_convertible_v<R, Result>) {
      return identity<Result>{};
    } else if constexpr (std::is_convertible_v<Result, R>) {
      return identity<R>{};
    } else {
      return identity<nonesuch>{};
    }
  }
}

template <typename T, typename Result, typename F, typename D = std::decay_t<T>, typename R = typename decltype(result_type<D, Result, F>())::type>
using result_t = std::enable_if_t<std::is_enum_v<D> && !std::is_same_v<R, nonesuch>, R>;

#if !defined(MAGIC_ENUM_ENABLE_HASH)

template <typename T = void>
inline constexpr auto default_result_type_lambda = []() noexcept(std::is_nothrow_default_constructible_v<T>) { return T{}; };

template <>
inline constexpr auto default_result_type_lambda<void> = []() noexcept {};

template <typename R, typename F, typename... Args>
constexpr R invoke_r(F&& f, Args&&... args) noexcept(std::is_nothrow_invocable_r_v<R, F, Args...>) {
  if constexpr (std::is_void_v<R>) {
    std::forward<F>(f)(std::forward<Args>(args)...);
  } else {
    return static_cast<R>(std::forward<F>(f)(std::forward<Args>(args)...));
  }
}

template <std::size_t I, std::size_t End, typename R, typename E, typename F, typename Def>
constexpr decltype(auto) constexpr_switch_impl(F&& f, E value, Def&& def) {
  if constexpr(I < End) {
    constexpr auto v = enum_constant<enum_value<E, I>()>{};
    if (value == v) {
      if constexpr (std::is_invocable_r_v<R, F, decltype(v)>) {
        return invoke_r<R>(std::forward<F>(f), v);
      } else {
        return def();
      }
    } else {
      return constexpr_switch_impl<I + 1, End, R>(std::forward<F>(f), value, std::forward<Def>(def));
    }
  } else {
    return def();
  }
}

template <typename R, typename E, typename F, typename Def>
constexpr decltype(auto) constexpr_switch(F&& f, E value, Def&& def) {
  static_assert(is_enum_v<E>, "magic_enum::detail::constexpr_switch requires enum type.");

  if constexpr (count_v<E> == 0) {
    return def();
  } else {
    return constexpr_switch_impl<0, count_v<E>, R>(std::forward<F>(f), value, std::forward<Def>(def));
  }
}
#endif

} // namespace magic_enum::detail

template <typename Result = detail::default_result_type, typename E, typename F, typename R = detail::result_t<E, Result, F>>
constexpr decltype(auto) enum_switch(F&& f, E value) {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_switch requires enum type.");

#if defined(MAGIC_ENUM_ENABLE_HASH)
  return detail::constexpr_switch<&detail::values_v<D>, detail::case_call_t::value>(
      std::forward<F>(f),
      value,
      detail::default_result_type_lambda<R>);
#else
  return detail::constexpr_switch<R, D>(
      std::forward<F>(f),
      value,
      detail::default_result_type_lambda<R>);
#endif
}

template <typename Result, typename E, typename F, typename R = detail::result_t<E, Result, F>>
constexpr decltype(auto) enum_switch(F&& f, E value, Result&& result) {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_switch requires enum type.");

#if defined(MAGIC_ENUM_ENABLE_HASH)
  return detail::constexpr_switch<&detail::values_v<D>, detail::case_call_t::value>(
      std::forward<F>(f),
      value,
      [&result]() -> R { return std::forward<Result>(result); });
#else
  return detail::constexpr_switch<R, D>(
      std::forward<F>(f),
      value,
      [&result]() -> R { return std::forward<Result>(result); });
#endif
}

} // namespace magic_enum

template <>
struct std::common_type<magic_enum::detail::nonesuch, magic_enum::detail::nonesuch> : magic_enum::detail::identity<magic_enum::detail::nonesuch> {};

template <typename T>
struct std::common_type<T, magic_enum::detail::nonesuch> : magic_enum::detail::identity<T> {};

template <typename T>
struct std::common_type<magic_enum::detail::nonesuch, T> : magic_enum::detail::identity<T> {};

#endif // NEARGYE_MAGIC_ENUM_SWITCH_HPP
