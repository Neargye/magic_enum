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
#if MAGIC_ENUM_VERSION_MAJOR != 0 || MAGIC_ENUM_VERSION_MINOR != 9 || MAGIC_ENUM_VERSION_PATCH != 8
#  error "magic_enum headers of different versions are mixed. Check the include paths."
#endif

#if defined(__GNUC__) && !defined(__clang__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wmaybe-uninitialized" // False positive on `return {};` for optional.
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4459 6244) // Local names can match global names in user code.
#endif

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

template <typename E>
MAGIC_ENUM_CONSTEVAL std::size_t zero_index() noexcept {
  constexpr auto& values = values_v<E, enum_subtype::flags>;
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (enum_value_equal(values[i], 0)) {
      return i;
    }
  }

  return values.size();
}

// Returns true if the integer is a valid flag combination. Zero is valid only when it is named.
template <typename E, typename I>
constexpr bool is_flags_mask([[maybe_unused]] I value) noexcept {
  using U = std::underlying_type_t<E>;
  using V = make_unsigned_t<U>;
  if constexpr (count_v<E, enum_subtype::flags> == 0) {
    return false;
  } else {
    const auto flag_value = static_cast<V>(static_cast<U>(value));
    constexpr auto mask = values_ors<E>();
    constexpr bool named_zero = zero_index<E>() < count_v<E, enum_subtype::flags>;
    return in_underlying_range<E>(value) && (flag_value & static_cast<V>(~mask)) == V{0} && (flag_value != V{0} || named_zero);
  }
}

// Parses separated names into result. Returns false for an empty string or an unknown name.
template <typename E, typename BinaryPredicate>
constexpr bool parse_flags([[maybe_unused]] string_view value, [[maybe_unused]] char_type sep, [[maybe_unused]] BinaryPredicate& p, [[maybe_unused]] make_unsigned_t<std::underlying_type_t<E>>& result) {
  using V = make_unsigned_t<std::underlying_type_t<E>>;
  constexpr auto S = enum_subtype::flags;
  if constexpr (count_v<E, S> == 0) {
    return false;
  } else {
    if (value.empty()) {
      return false; // A named zero parses to zero, so the result cannot mark an empty string.
    }
    // Avoid GCC C++26 wrong-code with find/remove_prefix, see #467.
    for (std::size_t first = 0; first < value.size();) {
      auto last = first;
      while (last < value.size() && value[last] != sep) {
        ++last;
      }
      const auto s = value.substr(first, last - first);
#if defined(MAGIC_ENUM_ENABLE_HASH)
      if (const auto flag = enum_cast<E, S, BinaryPredicate&>(s, p)) {
        result |= static_cast<V>(*flag);
      } else {
        return false;
      }
#else
      if (const auto i = name_index<E, S>(s, p); i < count_v<E, S>) {
        result |= static_cast<V>(values_v<E, S>[i]);
      } else {
        return false;
      }
#endif
      first = (last < value.size()) ? last + 1 : last;
    }
    return true;
  }
}

} // namespace magic_enum::detail

// Returns flag names joined by sep, the name of a named zero, or an empty string for unknown bits.
template <typename E, typename String = string>
[[nodiscard]] constexpr auto enum_flags_name(E value, char_type sep = char_type{'|'}) -> detail::enable_if_t<E, String> {
  using D = std::decay_t<E>;
  using U = detail::make_unsigned_t<underlying_type_t<D>>;
  constexpr auto S = detail::enum_subtype::flags;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_flags_name requires a non-empty reflected enum.");
  static_assert(std::is_same_v<typename String::value_type, char_type>, "magic_enum::enum_flags_name requires String::value_type to be magic_enum::char_type.");

  const auto flag_value = static_cast<U>(value);
  String name;
  if (flag_value == U{0}) {
    // The zero name never joins a nonzero mask.
    constexpr auto z = detail::zero_index<D>();
    if constexpr (z < detail::count_v<D, S>) {
      const auto n = detail::names_v<D, S>[z];
      name.append(n.data(), n.size());
    }
    return name;
  }

  auto check_value = U{0};
  for (std::size_t i = 0; i < detail::count_v<D, S>; ++i) {
    if (const auto v = static_cast<U>(detail::values_v<D, S>[i]); (flag_value & v) != U{0}) {
      check_value |= v;
      if (!name.empty()) {
        name.append(1, sep);
      }
      const auto n = detail::names_v<D, S>[i];
      name.append(n.data(), n.size());
    }
  }

  if (check_value != flag_value) {
    name.clear(); // Invalid value or out of range.
  }
  return name;
}

// Returns the flag combination matching the integer, or an empty optional.
template <typename E, typename I = underlying_type_t<std::decay_t<E>>, std::enable_if_t<detail::is_integer_input_v<I>, int> = 0>
[[nodiscard]] constexpr auto enum_flags_cast(I value) noexcept -> detail::enable_if_t<E, optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, detail::enum_subtype::flags>, "magic_enum::enum_flags_cast requires a non-empty reflected enum.");

  if (detail::is_flags_mask<D>(value)) {
    return static_cast<D>(static_cast<underlying_type_t<D>>(value));
  }
  return {}; // Invalid value or out of range.
}

// Returns the flag combination matching the separated names, or an empty optional.
template <typename E, typename BinaryPredicate = std::equal_to<>>
[[nodiscard]] constexpr auto enum_flags_cast(string_view value, char_type sep = char_type{'|'}, BinaryPredicate p = {}) noexcept(detail::is_nothrow_invocable_v<BinaryPredicate>) -> detail::enable_if_t<E, optional<std::decay_t<E>>, BinaryPredicate> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, detail::enum_subtype::flags>, "magic_enum::enum_flags_cast requires a non-empty reflected enum.");

  if (auto result = detail::make_unsigned_t<underlying_type_t<D>>{0}; detail::parse_flags<D>(value, sep, p, result)) {
    return static_cast<D>(result);
  }
  return {}; // Invalid value or out of range.
}

// Returns true if the value is a valid flag combination. Zero is valid only when it is named.
template <typename E>
[[nodiscard]] constexpr auto enum_flags_contains(E value) noexcept -> detail::enable_if_t<E, bool> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, detail::enum_subtype::flags>, "magic_enum::enum_flags_contains requires a non-empty reflected enum.");

  return detail::is_flags_mask<D>(static_cast<underlying_type_t<D>>(value));
}

// Returns true if the integer is a valid flag combination. Zero is valid only when it is named.
// The enum value overload handles arguments of type E.
template <typename E, typename I, std::enable_if_t<detail::is_integer_input_v<I> && !std::is_same_v<I, std::decay_t<E>>, int> = 0>
[[nodiscard]] constexpr auto enum_flags_contains(I value) noexcept -> detail::enable_if_t<E, bool> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, detail::enum_subtype::flags>, "magic_enum::enum_flags_contains requires a non-empty reflected enum.");

  return detail::is_flags_mask<D>(value);
}

// Returns true if the names form a valid flag combination.
template <typename E, typename BinaryPredicate = std::equal_to<>>
[[nodiscard]] constexpr auto enum_flags_contains(string_view value, char_type sep = char_type{'|'}, BinaryPredicate p = {}) noexcept(detail::is_nothrow_invocable_v<BinaryPredicate>) -> detail::enable_if_t<E, bool, BinaryPredicate> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, detail::enum_subtype::flags>, "magic_enum::enum_flags_contains requires a non-empty reflected enum.");

  auto result = detail::make_unsigned_t<underlying_type_t<D>>{0};
  return detail::parse_flags<D>(value, sep, p, result);
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

#if defined(__GNUC__) && !defined(__clang__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // NEARGYE_MAGIC_ENUM_FLAGS_HPP
