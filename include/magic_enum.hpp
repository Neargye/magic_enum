//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  vesion 0.5.0
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

#ifndef MAGIC_ENUM_HPP
#define MAGIC_ENUM_HPP

#include <array>
#include <cassert>
#include <cstddef>
#include <iosfwd>
#include <limits>
#include <string_view>
#include <optional>
#include <type_traits>
#include <utility>

// Enum value must be greater or equals than MAGIC_ENUM_RANGE_MIN. By default MAGIC_ENUM_RANGE_MIN = -128.
// If need another min range for all enum types by default, redefine the macro MAGIC_ENUM_RANGE_MIN.
#if !defined(MAGIC_ENUM_RANGE_MIN)
#  define MAGIC_ENUM_RANGE_MIN -128
#endif

// Enum value must be less or equals than MAGIC_ENUM_RANGE_MAX. By default MAGIC_ENUM_RANGE_MAX = 128.
// If need another max range for all enum types by default, redefine the macro MAGIC_ENUM_RANGE_MAX.
#if !defined(MAGIC_ENUM_RANGE_MAX)
#  define MAGIC_ENUM_RANGE_MAX 128
#endif

namespace magic_enum {

// Enum value must be in range [MAGIC_ENUM_RANGE_MIN, MAGIC_ENUM_RANGE_MAX]. By default MAGIC_ENUM_RANGE_MIN = -128, MAGIC_ENUM_RANGE_MAX = 128.
// If need another range for all enum types by default, redefine the macro MAGIC_ENUM_RANGE_MIN and MAGIC_ENUM_RANGE_MAX.
// If need another range for specific enum type, add specialization enum_range for necessary enum type.
template <typename E>
struct enum_range final {
  static_assert(std::is_enum_v<E>, "magic_enum::enum_range requires enum type.");
  static constexpr int min = std::is_signed_v<std::underlying_type_t<E>> ? MAGIC_ENUM_RANGE_MIN : 0;
  static constexpr int max = MAGIC_ENUM_RANGE_MAX;
  static_assert(max > min, "magic_enum::enum_range requires max > min.");
};

static_assert(MAGIC_ENUM_RANGE_MAX > 0,
              "MAGIC_ENUM_RANGE_MAX must be greater than 0.");
static_assert(MAGIC_ENUM_RANGE_MAX < std::numeric_limits<int>::max(),
              "MAGIC_ENUM_RANGE_MAX must be less than INT_MAX.");

static_assert(MAGIC_ENUM_RANGE_MIN <= 0,
              "MAGIC_ENUM_RANGE_MIN must be less or equals than 0.");
static_assert(MAGIC_ENUM_RANGE_MIN > std::numeric_limits<int>::min(),
              "MAGIC_ENUM_RANGE_MIN must be greater than INT_MIN.");

namespace detail {

template <typename E>
[[nodiscard]] constexpr int min_impl() {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::min_impl requires enum type.");
  using U = std::underlying_type_t<E>;
  constexpr int min = enum_range<E>::min > (std::numeric_limits<U>::min)() ? enum_range<E>::min : (std::numeric_limits<U>::min)();

  return min;
}

template <typename E>
[[nodiscard]] constexpr auto range_impl() {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::range_impl requires enum type.");
  static_assert(enum_range<E>::max > enum_range<E>::min, "magic_enum::enum_range requires max > min.");
  using U = std::underlying_type_t<E>;
  constexpr int max = enum_range<E>::max < (std::numeric_limits<U>::max)() ? enum_range<E>::max : (std::numeric_limits<U>::max)();
  constexpr auto range = std::make_integer_sequence<int, max - min_impl<E>() + 1>{};

  return range;
}

template <typename E, E V>
[[nodiscard]] constexpr std::string_view name_impl() noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::name_impl requires enum type.");
#if defined(__clang__)
  constexpr std::string_view name{__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2};
#elif defined(__GNUC__) && __GNUC__ >= 9
  constexpr std::string_view name{__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 51};
#elif defined(_MSC_VER)
  constexpr std::string_view name{__FUNCSIG__, sizeof(__FUNCSIG__) - 17};
#else
  return {}; // Unsupported compiler.
#endif

#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9) || defined(_MSC_VER)
  constexpr auto prefix = name.find_last_of(" :,-)") + 1;

  if constexpr (name[prefix] >= '0' && name[prefix] <= '9') {
    return {}; // Value does not have name.
  } else {
    return name.substr(prefix, name.length() - prefix);
  }
#endif
}

template <typename E, int... I>
[[nodiscard]] constexpr auto strings_impl(std::integer_sequence<int, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::strings_impl requires enum type.");
  constexpr std::array<std::string_view, sizeof...(I)> names{{name_impl<E, static_cast<E>(I + min_impl<E>())>()...}};

  return names;
}

template <typename E>
[[nodiscard]] constexpr std::string_view name_impl(int value) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::name_impl requires enum type.");
  constexpr auto names = strings_impl<E>(range_impl<E>());

  if (int i = value - min_impl<E>(); i >= 0 && static_cast<std::size_t>(i) < names.size()) {
    return names[i];
  } else {
    return {}; // Value out of range.
  }
}

template <typename E, int... I>
[[nodiscard]] constexpr auto values_impl(std::integer_sequence<int, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::values_impl requires enum type.");
  constexpr int n = sizeof...(I);
  constexpr std::array<bool, n> valid{{!name_impl<E, static_cast<E>(I + min_impl<E>())>().empty()...}};
  constexpr int num_valid = ((valid[I] ? 1 : 0) + ...);

  std::array<E, num_valid> values{};
  for (int i = 0, v = 0; i < n && v < num_valid; ++i) {
    if (valid[i]) {
      values[v++] = static_cast<E>(i + min_impl<E>());
    }
  }

  return values;
}

template <typename E, std::size_t... I>
[[nodiscard]] constexpr auto names_impl(std::integer_sequence<std::size_t, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::names_impl requires enum type.");
  constexpr auto values = values_impl<E>(range_impl<E>());
  constexpr std::array<std::string_view, sizeof...(I)> names{{name_impl<E, values[I]>()...}};

  return names;
}

template <typename E, std::size_t... I>
[[nodiscard]] constexpr auto entries_impl(std::integer_sequence<std::size_t, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::entries_impl requires enum type.");
  constexpr auto values = values_impl<E>(range_impl<E>());
  constexpr std::array<std::pair<E, std::string_view>, sizeof...(I)> entries{{{values[I], name_impl<E, values[I]>()}...}};

  return entries;
}

template<typename T>
using enable_if_enum_t = std::enable_if_t<std::is_enum_v<std::decay_t<T>>>;

template<typename T, bool = std::is_enum_v<T>>
struct is_scoped_enum_impl : std::false_type {};

template<typename T>
struct is_scoped_enum_impl<T, true> : std::bool_constant<!std::is_convertible_v<T, std::underlying_type_t<T>>> {};

template<typename T, bool = std::is_enum_v<T>>
struct is_unscoped_enum_impl : std::false_type {};

template<typename T>
struct is_unscoped_enum_impl<T, true> : std::bool_constant<std::is_convertible_v<T, std::underlying_type_t<T>>> {};

} // namespace magic_enum::detail

// Checks whether T is an Unscoped enumeration type.
// Provides the member constant value which is equal to true, if T is an [Unscoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Unscoped_enumeration) type. Otherwise, value is equal to false.
template <typename T>
struct is_unscoped_enum : detail::is_unscoped_enum_impl<T> {};

template <typename T>
inline constexpr bool is_unscoped_enum_v = is_unscoped_enum<T>::value;

// Checks whether T is an Scoped enumeration type.
// Provides the member constant value which is equal to true, if T is an [Scoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations) type. Otherwise, value is equal to false.
template <typename T>
struct is_scoped_enum : detail::is_scoped_enum_impl<T> {};

template <typename T>
inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;

// Obtains enum value from enum string name.
// Returns std::optional with enum value.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr std::optional<std::decay_t<E>> enum_cast(std::string_view value) noexcept {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_cast requires enum type.");
  constexpr auto values = detail::values_impl<D>(detail::range_impl<D>());
  constexpr auto count = values.size();
  constexpr auto names = detail::names_impl<D>(std::make_index_sequence<count>{});

  for (std::size_t i = 0; i < count; ++i) {
    if (names[i] == value) {
      return values[i];
    }
  }

  return std::nullopt; // Invalid value or out of range.
}

// Obtains enum value from integer value.
// Returns std::optional with enum value.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr std::optional<std::decay_t<E>> enum_cast(std::underlying_type_t<std::decay_t<E>> value) noexcept {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_cast requires enum type.");

  if (detail::name_impl<D>(static_cast<int>(value)).empty()) {
    return std::nullopt; // Invalid value or out of range.
  } else {
    return static_cast<D>(value);
  }
}

// Returns integer value from enum value.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr auto enum_integer(E value) noexcept {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_integer requires enum type.");

  return static_cast<std::underlying_type_t<D>>(value);
}

// Returns enum value at specified index.
// No bounds checking is performed: the behavior is undefined if index >= number of enum values.
template<typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr auto enum_value(std::size_t index) {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_value requires enum type.");
  constexpr auto values = detail::values_impl<D>(detail::range_impl<D>());

  return assert(index < values.size()), values[index];
}

// Obtains value enum sequence.
// Returns std::array with enum values, sorted by enum value.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr auto enum_values() noexcept {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_values requires enum type.");
  constexpr auto values = detail::values_impl<D>(detail::range_impl<D>());

  return values;
}

// Returns number of enum values.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr auto enum_count() noexcept {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_count requires enum type.");
  constexpr auto count = detail::values_impl<D>(detail::range_impl<D>()).size();

  return count;
}

// Returns string enum name from static storage enum variable.
// This version is much lighter on the compile times and is not restricted to the enum_range limitation.
template <auto V, typename = detail::enable_if_enum_t<decltype(V)>>
[[nodiscard]] constexpr std::string_view enum_name() noexcept {
  using D = std::decay_t<decltype(V)>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_name requires enum type.");

  return detail::name_impl<D, V>();
}

// Returns string enum name from enum value.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr std::string_view enum_name(E value) noexcept {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_name requires enum type.");

  return detail::name_impl<D>(static_cast<int>(value));
}

// Obtains string enum name sequence.
// Returns std::array with string enum names, sorted by enum value.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr auto enum_names() noexcept {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_names requires enum type.");
  constexpr auto count = detail::values_impl<D>(detail::range_impl<D>()).size();
  constexpr auto names = detail::names_impl<D>(std::make_index_sequence<count>{});

  return names;
}

// Obtains pair (value enum, string enum name) sequence.
// Returns std::array with std::pair (value enum, string enum name), sorted by enum value.
template <typename E, typename = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr auto enum_entries() noexcept {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::enum_entries requires enum type.");
  constexpr auto count = detail::values_impl<D>(detail::range_impl<D>()).size();
  constexpr auto entries = detail::entries_impl<D>(std::make_index_sequence<count>{});

  return entries;
}

namespace ops {

template <class Char, class Traits, typename E, typename = detail::enable_if_enum_t<E>>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, E value) {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::ops::operator<< requires enum type.");

  if (auto name = detail::name_impl<D>(static_cast<int>(value)); !name.empty()) {
    for (auto c : name) {
      os.put(c);
    }
  }

  return os;
}

template <class Char, class Traits, typename E, typename = detail::enable_if_enum_t<E>>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, std::optional<E> value) {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "magic_enum::ops::operator<< requires enum type.");

  if (value.has_value()) {
    if (auto name = detail::name_impl<D>(static_cast<int>(value.value())); !name.empty()) {
      for (auto c : name) {
        os.put(c);
      }
    }
  }

  return os;
}

} // namespace magic_enum::ops

} // namespace magic_enum

#endif // MAGIC_ENUM_HPP
