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

#ifndef NEARGYE_MAGIC_ENUM_FLAGS_HPP
#define NEARGYE_MAGIC_ENUM_FLAGS_HPP

#include "magic_enum.hpp"

namespace magic_enum {

namespace detail {

template <typename E>
MAGIC_ENUM_CONSTEVAL auto values_ors() noexcept {
  using U = make_unsigned_t<std::underlying_type_t<E>>;
  auto ors = U{0};
  for (const auto value : values_v<E, enum_subtype::flags>) {
    ors |= static_cast<U>(value);
  }

  return ors;
}

} // namespace magic_enum::detail

// Returns flag names joined by sep, the name of a named zero, or an empty string for unknown bits.
template <typename E>
[[nodiscard]] auto enum_flags_name(E value, char_type sep = char_type{'|'}) -> detail::enable_if_t<E, string> {
  using D = std::decay_t<E>;
  using U = detail::make_unsigned_t<underlying_type_t<D>>;
  constexpr auto S = detail::enum_subtype::flags;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum requires enum implementation and valid max and min.");

  const auto flag_value = static_cast<U>(value);
  string name;
  if (flag_value == U{0}) {
    // The zero name never joins a nonzero mask.
    const auto n = enum_name<S>(value);
    name.append(n.data(), n.size());
    return name;
  }

  auto check_value = U{0};
  for (std::size_t i = 0; i < detail::count_v<D, S>; ++i) {
    if (const auto v = static_cast<U>(detail::values_v<D, S>[i]); (flag_value & v) != U{0}) {
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

  if (check_value == flag_value) {
    return name;
  }
  return {}; // Invalid value or out of range.
}

// Returns the flag combination matching the integer, or an empty optional.
template <typename E, typename I, std::enable_if_t<detail::is_integer_input<I>::value, int> = 0>
[[nodiscard]] constexpr auto enum_flags_cast(I value) noexcept -> detail::enable_if_t<E, optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  using V = detail::make_unsigned_t<underlying_type_t<D>>;
  constexpr auto S = detail::enum_subtype::flags;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum requires enum implementation and valid max and min.");

  if constexpr (detail::count_v<D, S> == 0) {
    static_cast<void>(value);
    return {}; // Empty enum.
  } else {
    if (!detail::in_underlying_range<D>(value)) {
      return {}; // Value does not fit the underlying type.
    }
    const auto u = static_cast<underlying_type_t<D>>(value);
    const auto flag_value = static_cast<V>(u);
    constexpr auto mask = detail::values_ors<D>();
    // Zero is valid only when it is named.
    if ((flag_value & static_cast<V>(~mask)) == V{0} && (flag_value != V{0} || detail::index_of<D, S>(u))) {
      return static_cast<D>(u);
    }
    return {}; // Invalid value or out of range.
  }
}

// Returns the flag combination matching the separated names, or an empty optional.
template <typename E, typename BinaryPredicate = std::equal_to<>, typename String = string_view, std::enable_if_t<detail::is_name_input_v<String>, int> = 0>
[[nodiscard]] constexpr auto enum_flags_cast(String&& name, [[maybe_unused]] char_type sep = char_type{'|'}, [[maybe_unused]] BinaryPredicate p = {}) noexcept(detail::is_nothrow_invocable_v<BinaryPredicate> && detail::is_nothrow_name_input_v<String>) -> detail::enable_if_t<E, optional<std::decay_t<E>>, BinaryPredicate> {
  string_view value(std::forward<String>(name));
  using D = std::decay_t<E>;
  using U = detail::make_unsigned_t<underlying_type_t<D>>;
  constexpr auto S = detail::enum_subtype::flags;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum requires enum implementation and valid max and min.");

  if constexpr (detail::count_v<D, S> == 0) {
    static_cast<void>(value);
    return {}; // Empty enum.
  } else if (value.empty()) {
    return {}; // Empty string. A named zero parses to zero, so the result cannot mark it.
  } else {
    auto result = U{0};
    // Avoid GCC C++26 wrong-code with find/remove_prefix, see #467.
    for (std::size_t first = 0; first < value.size();) {
      auto last = first;
      while (last < value.size() && value[last] != sep) {
        ++last;
      }
      const auto s = value.substr(first, last - first);
      if (const auto flag = enum_cast<D, S, BinaryPredicate&>(s, p)) {
        result |= static_cast<U>(*flag);
      } else {
        return {}; // Invalid value or out of range.
      }
      first = (last < value.size()) ? last + 1 : last;
    }

    return static_cast<D>(result);
  }
}

// Returns true if the value is a valid flag combination. Zero is valid only when it is named.
template <typename E>
[[nodiscard]] constexpr auto enum_flags_contains(E value) noexcept -> detail::enable_if_t<E, bool> {
  return static_cast<bool>(enum_flags_cast<E>(static_cast<underlying_type_t<E>>(value)));
}

// Returns true if the integer is a valid flag combination. Zero is valid only when it is named.
// The enum value overload handles arguments of type E.
template <typename E, typename I, std::enable_if_t<detail::is_integer_input<I>::value && !std::is_same_v<I, std::decay_t<E>>, int> = 0>
[[nodiscard]] constexpr auto enum_flags_contains(I value) noexcept -> detail::enable_if_t<E, bool> {
  return static_cast<bool>(enum_flags_cast<E>(value));
}

// Returns true if the names form a valid flag combination.
template <typename E, typename BinaryPredicate = std::equal_to<>, typename String = string_view, std::enable_if_t<detail::is_name_input_v<String>, int> = 0>
[[nodiscard]] constexpr auto enum_flags_contains(String&& value, char_type sep = char_type{'|'}, BinaryPredicate p = {}) noexcept(detail::is_nothrow_invocable_v<BinaryPredicate> && detail::is_nothrow_name_input_v<String>) -> detail::enable_if_t<E, bool, BinaryPredicate> {
  return static_cast<bool>(enum_flags_cast<E, BinaryPredicate&>(std::forward<String>(value), sep, p));
}

// Returns true if flag is nonzero and all its bits are set in flags.
template <typename E>
[[nodiscard]] constexpr auto enum_flags_test(E flags, E flag) noexcept -> detail::enable_if_t<E, bool> {
  using U = detail::make_unsigned_t<underlying_type_t<E>>;

  const auto flag_value = static_cast<U>(flag);
  return flag_value != U{0} && (static_cast<U>(flags) & flag_value) == flag_value;
}

// Returns true if lhs and rhs share at least one set bit.
template <typename E>
[[nodiscard]] constexpr auto enum_flags_test_any(E lhs, E rhs) noexcept -> detail::enable_if_t<E, bool> {
  using U = detail::make_unsigned_t<underlying_type_t<E>>;

  return (static_cast<U>(lhs) & static_cast<U>(rhs)) != U{0};
}

} // namespace magic_enum

#endif // NEARGYE_MAGIC_ENUM_FLAGS_HPP
