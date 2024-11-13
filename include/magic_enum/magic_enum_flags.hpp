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

#ifndef NEARGYE_MAGIC_ENUM_FLAGS_HPP
#define NEARGYE_MAGIC_ENUM_FLAGS_HPP

#include "magic_enum.hpp"

#if defined(__clang__)
#  pragma clang diagnostic push
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wmaybe-uninitialized" // May be used uninitialized 'return {};'.
#elif defined(_MSC_VER)
#  pragma warning(push)
#endif

namespace magic_enum {

namespace detail {

template <typename E, enum_subtype S, typename U = std::underlying_type_t<E>>
constexpr U values_ors() noexcept {
  static_assert(S == enum_subtype::flags, "magic_enum::detail::values_ors requires valid subtype.");

  auto ors = U{0};
  for (std::size_t i = 0; i < count_v<E, S>; ++i) {
    ors |= static_cast<U>(values_v<E, S>[i]);
  }

  return ors;
}

} // namespace magic_enum::detail

// Returns name from enum-flags value.
// If enum-flags value does not have name or value out of range, returns empty string.
template <typename E>
[[nodiscard]] auto enum_flags_name(E value, char_type sep = static_cast<char_type>('|')) -> detail::enable_if_t<E, string> {
  using D = std::decay_t<E>;
  using U = underlying_type_t<D>;
  constexpr auto S = detail::enum_subtype::flags;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum requires enum implementation and valid max and min.");

  string name;
  auto check_value = U{0};
  for (std::size_t i = 0; i < detail::count_v<D, S>; ++i) {
    if (const auto v = static_cast<U>(enum_value<D, S>(i)); (static_cast<U>(value) & v) != 0) {
      if (const auto n = detail::names_v<D, S>[i]; !n.empty()) {
        check_value |= v;
        if (!name.empty()) {
          name.append(1, sep);
        }
        name.append(n.data(), n.size());
      } else {
        return {}; // Value out of range.
      }
    }
  }

  if (check_value != 0 && check_value == static_cast<U>(value)) {
    return name;
  }
  return {}; // Invalid value or out of range.
}

// Obtains enum-flags value from integer value.
// Returns optional with enum-flags value.
template <typename E>
[[nodiscard]] constexpr auto enum_flags_cast(underlying_type_t<E> value) noexcept -> detail::enable_if_t<E, optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  using U = underlying_type_t<D>;
  constexpr auto S = detail::enum_subtype::flags;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum requires enum implementation and valid max and min.");

  if constexpr (detail::count_v<D, S> == 0) {
    static_cast<void>(value);
    return {}; // Empty enum.
  } else {
    if constexpr (detail::is_sparse_v<D, S>) {
      auto check_value = U{0};
      for (std::size_t i = 0; i < detail::count_v<D, S>; ++i) {
        if (const auto v = static_cast<U>(enum_value<D, S>(i)); (value & v) != 0) {
          check_value |= v;
        }
      }

      if (check_value != 0 && check_value == value) {
        return static_cast<D>(value);
      }
    } else {
      constexpr auto min = detail::min_v<D, S>;
      constexpr auto max = detail::values_ors<D, S>();

      if (value >= min && value <= max) {
        return static_cast<D>(value);
      }
    }
    return {}; // Invalid value or out of range.
  }
}

// Obtains enum-flags value from name.
// Returns optional with enum-flags value.
template <typename E, typename BinaryPredicate = std::equal_to<>>
[[nodiscard]] constexpr auto enum_flags_cast(string_view value, [[maybe_unused]] BinaryPredicate p = {}) noexcept(detail::is_nothrow_invocable<BinaryPredicate>()) -> detail::enable_if_t<E, optional<std::decay_t<E>>, BinaryPredicate> {
  using D = std::decay_t<E>;
  using U = underlying_type_t<D>;
  constexpr auto S = detail::enum_subtype::flags;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum requires enum implementation and valid max and min.");

  if constexpr (detail::count_v<D, S> == 0) {
    static_cast<void>(value);
    return {}; // Empty enum.
  } else {
    auto result = U{0};
    while (!value.empty()) {
      const auto d = detail::find(value, '|');
      const auto s = (d == string_view::npos) ? value : value.substr(0, d);
      auto f = U{0};
      for (std::size_t i = 0; i < detail::count_v<D, S>; ++i) {
        if (detail::cmp_equal(s, detail::names_v<D, S>[i], p)) {
          f = static_cast<U>(enum_value<D, S>(i));
          result |= f;
          break;
        }
      }
      if (f == U{0}) {
        return {}; // Invalid value or out of range.
      }
      value.remove_prefix((d == string_view::npos) ? value.size() : d + 1);
    }

    if (result != U{0}) {
      return static_cast<D>(result);
    }
    return {}; // Invalid value or out of range.
  }
}

// Checks whether enum-flags contains value with such value.
template <typename E>
[[nodiscard]] constexpr auto enum_flags_contains(E value) noexcept -> detail::enable_if_t<E, bool> {
  using D = std::decay_t<E>;
  using U = underlying_type_t<D>;

  return static_cast<bool>(enum_flags_cast<D>(static_cast<U>(value)));
}

// Checks whether enum-flags contains value with such integer value.
template <typename E>
[[nodiscard]] constexpr auto enum_flags_contains(underlying_type_t<E> value) noexcept -> detail::enable_if_t<E, bool> {
  using D = std::decay_t<E>;

  return static_cast<bool>(enum_flags_cast<D>(value));
}

// Checks whether enum-flags contains enumerator with such name.
template <typename E, typename BinaryPredicate = std::equal_to<>>
[[nodiscard]] constexpr auto enum_flags_contains(string_view value, BinaryPredicate p = {}) noexcept(detail::is_nothrow_invocable<BinaryPredicate>()) -> detail::enable_if_t<E, bool, BinaryPredicate> {
  using D = std::decay_t<E>;

  return static_cast<bool>(enum_flags_cast<D>(value, std::move(p)));
}

// Checks whether `flags set` contains `flag`.
// Note: If `flag` equals 0, it returns false, as 0 is not a flag.
template <typename E>
constexpr auto enum_flags_test(E flags, E flag) noexcept -> detail::enable_if_t<E, bool> {
  using U = underlying_type_t<E>;

  return static_cast<U>(flag) && ((static_cast<U>(flags) & static_cast<U>(flag)) == static_cast<U>(flag));
}

// Checks whether `lhs flags set` and `rhs flags set` have common flags.
// Note: If `lhs flags set` or `rhs flags set` equals 0, it returns false, as 0 is not a flag, and therfore cannot have any matching flag.
template <typename E>
constexpr auto enum_flags_test_any(E lhs, E rhs) noexcept -> detail::enable_if_t<E, bool> {
  using U = underlying_type_t<E>;

  return (static_cast<U>(lhs) & static_cast<U>(rhs)) != 0;
}

} // namespace magic_enum

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // NEARGYE_MAGIC_ENUM_FLAGS_HPP
