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
constexpr auto values_ors() noexcept {
  using U = make_unsigned_t<std::underlying_type_t<E>>;
  auto ors = U{0};
  for (const auto value : values_v<E, enum_subtype::flags>) {
    ors |= static_cast<U>(value);
  }

  return ors;
}

} // namespace magic_enum::detail

// Returns name from flag enum value.
// If flag enum value has no name or is out of range, returns empty string.
template <typename E>
[[nodiscard]] auto enum_flags_name(E value, char_type sep = char_type{'|'}) -> detail::enable_if_t<E, string> {
  using D = std::decay_t<E>;
  using U = detail::make_unsigned_t<underlying_type_t<D>>;
  constexpr auto S = detail::enum_subtype::flags;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum requires enum implementation and valid max and min.");

  const auto flag_value = static_cast<U>(value);
  string name;
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

  if (check_value != U{0} && check_value == flag_value) {
    return name;
  }
  return {}; // Invalid value or out of range.
}

// Returns flag enum value from integer value.
// Returns optional containing flag enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_flags_cast(underlying_type_t<E> value) noexcept -> detail::enable_if_t<E, optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  using U = underlying_type_t<D>;
  using V = detail::make_unsigned_t<U>;
  constexpr auto S = detail::enum_subtype::flags;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum requires enum implementation and valid max and min.");

  if constexpr (detail::count_v<D, S> == 0) {
    static_cast<void>(value);
    return {}; // Empty enum.
  } else {
    const auto flag_value = static_cast<V>(value);
    constexpr auto mask = detail::values_ors<D>();
    if (flag_value != V{0} && (flag_value & static_cast<V>(~mask)) == V{0}) {
      return static_cast<D>(value);
    }
    return {}; // Invalid value or out of range.
  }
}

// Returns flag enum value from name.
// Returns optional containing flag enum value.
template <typename E, typename BinaryPredicate = std::equal_to<>>
[[nodiscard]] constexpr auto enum_flags_cast(string_view value, [[maybe_unused]] char_type sep = char_type{'|'}, [[maybe_unused]] BinaryPredicate p = {}) noexcept(detail::is_nothrow_invocable_v<BinaryPredicate>) -> detail::enable_if_t<E, optional<std::decay_t<E>>, BinaryPredicate> {
  using D = std::decay_t<E>;
  using U = detail::make_unsigned_t<underlying_type_t<D>>;
  constexpr auto S = detail::enum_subtype::flags;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum requires enum implementation and valid max and min.");

  if constexpr (detail::count_v<D, S> == 0) {
    static_cast<void>(value);
    return {}; // Empty enum.
  } else {
    auto result = U{0};
    // Avoid GCC C++26 wrong-code with find/remove_prefix; see https://github.com/Neargye/magic_enum/issues/467.
    for (std::size_t first = 0; first < value.size();) {
      auto last = first;
      while (last < value.size() && value[last] != sep) {
        ++last;
      }
      const auto s = value.substr(first, last - first);
      auto flag = U{0};
      for (std::size_t i = 0; i < detail::count_v<D, S>; ++i) {
        if (detail::cmp_equal(s, detail::names_v<D, S>[i], p)) {
          flag = static_cast<U>(detail::values_v<D, S>[i]);
          result |= flag;
          break;
        }
      }
      if (flag == U{0}) {
        return {}; // Invalid value or out of range.
      }
      first = (last < value.size()) ? last + 1 : last;
    }

    if (result != U{0}) {
      return static_cast<D>(result);
    }
    return {}; // Invalid value or out of range.
  }
}

// Returns true if flag enum contains specified value.
template <typename E>
[[nodiscard]] constexpr auto enum_flags_contains(E value) noexcept -> detail::enable_if_t<E, bool> {
  using D = std::decay_t<E>;
  using U = underlying_type_t<D>;

  return static_cast<bool>(enum_flags_cast<D>(static_cast<U>(value)));
}

// Returns true if flag enum contains specified integer value.
template <typename E>
[[nodiscard]] constexpr auto enum_flags_contains(underlying_type_t<E> value) noexcept -> detail::enable_if_t<E, bool> {
  using D = std::decay_t<E>;

  return static_cast<bool>(enum_flags_cast<D>(value));
}

// Returns true if flag enum contains enumerator with specified name.
template <typename E, typename BinaryPredicate = std::equal_to<>>
[[nodiscard]] constexpr auto enum_flags_contains(string_view value, char_type sep = char_type{'|'}, BinaryPredicate p = {}) noexcept(detail::is_nothrow_invocable_v<BinaryPredicate>) -> detail::enable_if_t<E, bool, BinaryPredicate> {
  using D = std::decay_t<E>;

  return static_cast<bool>(enum_flags_cast<D, BinaryPredicate&>(value, sep, p));
}

// Returns true if `flags` contains `flag`.
// Returns false if `flag` equals 0 because 0 is not a flag.
template <typename E>
constexpr auto enum_flags_test(E flags, E flag) noexcept -> detail::enable_if_t<E, bool> {
  using U = detail::make_unsigned_t<underlying_type_t<E>>;

  const auto flag_value = static_cast<U>(flag);
  return flag_value != U{0} && (static_cast<U>(flags) & flag_value) == flag_value;
}

// Returns true if `lhs` and `rhs` share any flags.
// Returns false if either value equals 0 because 0 is not a flag.
template <typename E>
constexpr auto enum_flags_test_any(E lhs, E rhs) noexcept -> detail::enable_if_t<E, bool> {
  using U = detail::make_unsigned_t<underlying_type_t<E>>;

  return (static_cast<U>(lhs) & static_cast<U>(rhs)) != U{0};
}

} // namespace magic_enum

#endif // NEARGYE_MAGIC_ENUM_FLAGS_HPP
