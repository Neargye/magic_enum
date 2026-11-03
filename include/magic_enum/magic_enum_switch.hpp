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

// Avoid std::invoke_result instantiations for ordinary callables.
template <typename F, typename V, bool = std::is_member_pointer_v<std::decay_t<F>>, typename = void>
struct invoke_result : identity<nonesuch> {};

template <typename F, typename V>
struct invoke_result<F, V, true, std::enable_if_t<std::is_invocable_v<F, V>>> : std::invoke_result<F, V> {};

template <typename F, typename V>
struct invoke_result<F, V, false, std::void_t<decltype(std::declval<F>()(std::declval<V>()))>> : identity<decltype(std::declval<F>()(std::declval<V>()))> {};

template <typename F, typename V>
using invoke_result_t = typename invoke_result<F, V>::type;

template <typename...>
struct type_list {};

template <typename... Ts>
struct common_type : std::common_type<Ts...> {};

// Chunk the left fold to limit template depth without reordering common_type specializations.
template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7,
          typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15,
          typename Next, typename... Ts>
struct common_type<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, Next, Ts...>
    : common_type<typename std::common_type<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::type, Next, Ts...> {};

template <typename E, enum_subtype S, typename F, std::size_t... J>
constexpr auto common_invocable(std::index_sequence<J...>) noexcept {
  if constexpr (count_v<E, S> == 0) {
    return identity<nonesuch>{};
  } else {
    using R = invoke_result_t<F, enum_constant<values_v<E, S>[0]>>;
    // Preserve user specializations of common_type<R, R> when skipping the fold.
    if constexpr (std::is_same_v<type_list<R, invoke_result_t<F, enum_constant<values_v<E, S>[J]>>...>, type_list<invoke_result_t<F, enum_constant<values_v<E, S>[J]>>..., R>>) {
      if constexpr (std::is_same_v<typename std::common_type<R, R>::type, std::decay_t<R>>) {
        return identity<std::decay_t<R>>{};
      } else {
        return common_type<invoke_result_t<F, enum_constant<values_v<E, S>[J]>>...>{};
      }
    } else {
      return common_type<invoke_result_t<F, enum_constant<values_v<E, S>[J]>>...>{};
    }
  }
}

template <typename E, enum_subtype S, typename Result, typename F, bool HasResult>
constexpr auto result_type() noexcept {
  constexpr auto seq = std::make_index_sequence<count_v<E, S>>{};
  using R = std::decay_t<typename decltype(common_invocable<E, S, F>(seq))::type>;
  using D = std::decay_t<Result>;
  if constexpr (std::is_same_v<Result, default_result_type>) {
    if constexpr (std::is_same_v<R, nonesuch>) {
      return identity<void>{};
    } else {
      return identity<R>{};
    }
  } else {
    if constexpr (count_v<E, S> == 0) {
      // An empty enum has no cases, so only Result determines the result type.
      return identity<D>{};
    } else if constexpr (std::is_convertible_v<R, D> && (!HasResult || std::is_convertible_v<Result, D>)) {
      return identity<D>{};
    } else if constexpr (std::is_convertible_v<Result, R>) {
      return identity<R>{};
    } else {
      return identity<nonesuch>{};
    }
  }
}

template <typename E, enum_subtype S, typename Result, typename F, bool HasResult = false, typename D = enable_if_t<E, std::decay_t<E>>, typename R = typename decltype(result_type<D, S, Result, F, HasResult>())::type>
using result_t = std::enable_if_t<!std::is_same_v<R, nonesuch>, R>;

template <typename E, enum_subtype S, typename Result, typename F>
using result_with_fallback_t = result_t<E, S, Result, F, true>;

#define MAGIC_ENUM_SWITCH_CASE(J)                                                                                                 \
  case cases[(J)]:                                                                                                                \
    if constexpr ((J) + Page < count_v<E, S>) {                                                                                   \
      using V = enum_constant<values_v<E, S>[(J) + Page]>;                                                                        \
      using T = invoke_result_t<F, V>;                                                                                            \
      if constexpr (std::is_same_v<T, nonesuch> || !(std::is_void_v<R> || std::is_same_v<T, R> || std::is_convertible_v<T, R>)) { \
        return def();                                                                                                             \
      } else if constexpr (std::is_member_pointer_v<std::decay_t<F>>) {                                                           \
        return static_cast<R>(detail::invoke_constant(std::forward<F>(f), V{}));                                                  \
      } else {                                                                                                                    \
        return static_cast<R>(std::forward<F>(f)(V{}));                                                                           \
      }                                                                                                                           \
    } else {                                                                                                                      \
      break;                                                                                                                      \
    }
#define MAGIC_ENUM_SWITCH_CASES_8(J) MAGIC_ENUM_SWITCH_CASE(J) MAGIC_ENUM_SWITCH_CASE(J + 1) MAGIC_ENUM_SWITCH_CASE(J + 2) MAGIC_ENUM_SWITCH_CASE(J + 3) MAGIC_ENUM_SWITCH_CASE(J + 4) MAGIC_ENUM_SWITCH_CASE(J + 5) MAGIC_ENUM_SWITCH_CASE(J + 6) MAGIC_ENUM_SWITCH_CASE(J + 7)
#define MAGIC_ENUM_SWITCH_CASES_16(J) MAGIC_ENUM_SWITCH_CASES_8(J) MAGIC_ENUM_SWITCH_CASES_8(J + 8)
#define MAGIC_ENUM_SWITCH_CASES_32(J) MAGIC_ENUM_SWITCH_CASES_16(J) MAGIC_ENUM_SWITCH_CASES_16(J + 16)
#define MAGIC_ENUM_SWITCH_CASES_64(J) MAGIC_ENUM_SWITCH_CASES_32(J) MAGIC_ENUM_SWITCH_CASES_32(J + 32)
#define MAGIC_ENUM_SWITCH_CASES_128(J) MAGIC_ENUM_SWITCH_CASES_64(J) MAGIC_ENUM_SWITCH_CASES_64(J + 64)
#define MAGIC_ENUM_SWITCH_CASES_256(J) MAGIC_ENUM_SWITCH_CASES_128(J) MAGIC_ENUM_SWITCH_CASES_128(J + 128)
#define MAGIC_ENUM_SWITCH(N)         \
  if constexpr (Size == (N)) {       \
    switch (value) {                 \
      MAGIC_ENUM_SWITCH_CASES_##N(0) \
      default:                       \
        break;                       \
    }                                \
  }

template <std::size_t Page, typename R, typename E, enum_subtype S, typename F, typename Def>
constexpr R switch_page(F&& f, switch_t<E> value, Def&& def) {
  constexpr std::size_t n = count_v<E, S> - Page;
  constexpr std::size_t Size = switch_page_size(n);
  constexpr auto cases = value_cases<E, S>::template page<Page, Size>();
  MAGIC_ENUM_SWITCH(8)
  MAGIC_ENUM_SWITCH(16)
  MAGIC_ENUM_SWITCH(32)
  MAGIC_ENUM_SWITCH(64)
  MAGIC_ENUM_SWITCH(128)
  MAGIC_ENUM_SWITCH(256)
  if constexpr (n > Size) {
    return switch_page<Page + Size, R, E, S>(std::forward<F>(f), value, std::forward<Def>(def));
  } else {
    return def();
  }
}

#undef MAGIC_ENUM_SWITCH
#undef MAGIC_ENUM_SWITCH_CASES_256
#undef MAGIC_ENUM_SWITCH_CASES_128
#undef MAGIC_ENUM_SWITCH_CASES_64
#undef MAGIC_ENUM_SWITCH_CASES_32
#undef MAGIC_ENUM_SWITCH_CASES_16
#undef MAGIC_ENUM_SWITCH_CASES_8
#undef MAGIC_ENUM_SWITCH_CASE

template <typename R, typename E, enum_subtype S, typename F, typename Def>
constexpr decltype(auto) value_switch(F&& f, E value, Def&& def) {
  if constexpr (count_v<E, S> == 0) {
    return def();
  } else {
    return switch_page<0, R, E, S>(std::forward<F>(f), static_cast<switch_t<E>>(static_cast<std::underlying_type_t<E>>(value)), std::forward<Def>(def));
  }
}

} // namespace magic_enum::detail

template <typename Result = detail::default_result_type, typename E, detail::enum_subtype S = detail::subtype_v<E>, typename F, typename R = detail::result_t<E, S, Result, F>>
constexpr decltype(auto) enum_switch(F&& f, E value) {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_switch requires a non-empty reflected enum.");

  return detail::value_switch<R, D, S>(
      std::forward<F>(f),
      value,
      detail::default_result_type_lambda<R>);
}

template <typename Result = detail::default_result_type, detail::enum_subtype S, typename E, typename F, typename R = detail::result_t<E, S, Result, F>>
constexpr decltype(auto) enum_switch(F&& f, E value) {
  return enum_switch<Result, E, S>(std::forward<F>(f), value);
}

template <typename Result, typename E, detail::enum_subtype S = detail::subtype_v<E>, typename F, typename R = detail::result_with_fallback_t<E, S, Result, F>>
constexpr decltype(auto) enum_switch(F&& f, E value, Result&& result) {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_switch requires a non-empty reflected enum.");

  return detail::value_switch<R, D, S>(
      std::forward<F>(f),
      value,
      [&result]() -> R { return std::forward<Result>(result); });
}

template <typename Result, detail::enum_subtype S, typename E, typename F, typename R = detail::result_with_fallback_t<E, S, Result, F>>
constexpr decltype(auto) enum_switch(F&& f, E value, Result&& result) {
  return enum_switch<Result, E, S>(std::forward<F>(f), value, std::forward<Result>(result));
}

} // namespace magic_enum

template <>
struct std::common_type<magic_enum::detail::nonesuch, magic_enum::detail::nonesuch> : magic_enum::detail::identity<magic_enum::detail::nonesuch> {};

template <typename T>
struct std::common_type<T, magic_enum::detail::nonesuch> : magic_enum::detail::identity<T> {};

template <typename T>
struct std::common_type<magic_enum::detail::nonesuch, T> : magic_enum::detail::identity<T> {};

#endif // NEARGYE_MAGIC_ENUM_SWITCH_HPP
