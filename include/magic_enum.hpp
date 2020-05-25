//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.6.5
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2020 Daniil Goncharov <neargye@gmail.com>.
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

#ifndef NEARGYE_MAGIC_ENUM_HPP
#define NEARGYE_MAGIC_ENUM_HPP

#include <array>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <limits>
#include <string_view>
#include <optional>
#include <type_traits>
#include <utility>

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 26495) // Variable 'magic_enum::detail::static_string<N>::chars' is uninitialized.
#endif

// Checks magic_enum compiler compatibility.
#if defined(__clang__) || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER)
#  undef  MAGIC_ENUM_SUPPORTED
#  define MAGIC_ENUM_SUPPORTED 1
#endif

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
struct enum_range {
  static_assert(std::is_enum_v<E>, "magic_enum::enum_range requires enum type.");
  inline static constexpr int min = MAGIC_ENUM_RANGE_MIN;
  inline static constexpr int max = MAGIC_ENUM_RANGE_MAX;
  static_assert(max > min, "magic_enum::enum_range requires max > min.");
};

static_assert(MAGIC_ENUM_RANGE_MIN <= 0, "MAGIC_ENUM_RANGE_MIN must be less or equals than 0.");
static_assert(MAGIC_ENUM_RANGE_MIN > (std::numeric_limits<std::int16_t>::min)(), "MAGIC_ENUM_RANGE_MIN must be greater than INT16_MIN.");

static_assert(MAGIC_ENUM_RANGE_MAX > 0, "MAGIC_ENUM_RANGE_MAX must be greater than 0.");
static_assert(MAGIC_ENUM_RANGE_MAX < (std::numeric_limits<std::int16_t>::max)(), "MAGIC_ENUM_RANGE_MAX must be less than INT16_MAX.");

static_assert(MAGIC_ENUM_RANGE_MAX > MAGIC_ENUM_RANGE_MIN, "MAGIC_ENUM_RANGE_MAX must be greater than MAGIC_ENUM_RANGE_MIN.");

namespace detail {

template <typename T>
struct supported
#if defined(MAGIC_ENUM_SUPPORTED) && MAGIC_ENUM_SUPPORTED || defined(MAGIC_ENUM_NO_CHECK_SUPPORT)
    : std::true_type {};
#else
    : std::false_type {};
#endif

template <typename T>
inline constexpr bool is_enum_v = std::is_enum_v<T> && std::is_same_v<T, std::decay_t<T>>;

template <std::size_t N>
struct static_string {
  constexpr explicit static_string(std::string_view str) noexcept : static_string{str, std::make_index_sequence<N>{}} {
    assert(str.size() == N);
  }

  constexpr const char* data() const noexcept { return chars.data(); }

  constexpr std::size_t size() const noexcept { return N; }

  constexpr operator std::string_view() const noexcept { return {data(), size()}; }

 private:
  template <std::size_t... I>
  constexpr static_string(std::string_view str, std::index_sequence<I...>) noexcept : chars{{str[I]..., '\0'}} {}

  const std::array<char, N + 1> chars;
};

template <>
struct static_string<0> {
  constexpr explicit static_string(std::string_view) noexcept {}

  constexpr const char* data() const noexcept { return nullptr; }

  constexpr std::size_t size() const noexcept { return 0; }

  constexpr operator std::string_view() const noexcept { return {}; }
};

constexpr std::string_view pretty_name(std::string_view name) noexcept {
  for (std::size_t i = name.size(); i > 0; --i) {
    if (!((name[i - 1] >= '0' && name[i - 1] <= '9') ||
          (name[i - 1] >= 'a' && name[i - 1] <= 'z') ||
          (name[i - 1] >= 'A' && name[i - 1] <= 'Z') ||
          (name[i - 1] == '_'))) {
      name.remove_prefix(i);
      break;
    }
  }

  if (name.size() > 0 && ((name.front() >= 'a' && name.front() <= 'z') ||
                          (name.front() >= 'A' && name.front() <= 'Z') ||
                          (name.front() == '_'))) {
    return name;
  }

  return {}; // Invalid name.
}

template <typename BinaryPredicate>
constexpr bool cmp_equal(std::string_view lhs, std::string_view rhs, BinaryPredicate p) noexcept(std::is_nothrow_invocable_r_v<bool, BinaryPredicate, char, char>) {
  if (lhs.size() != rhs.size()) {
    return false;
  }

  const auto size = lhs.size();
  for (std::size_t i = 0; i < size; ++i) {
    if (!p(lhs[i], rhs[i])) {
      return false;
    }
  }

  return true;
}

template <typename L, typename R>
constexpr bool cmp_less(L lhs, R rhs) noexcept {
  static_assert(std::is_integral_v<L> && std::is_integral_v<R>, "magic_enum::detail::cmp_less requires integral type.");

  if constexpr (std::is_signed_v<L> == std::is_signed_v<R>) {
    // If same signedness (both signed or both unsigned).
    return lhs < rhs;
  } else if constexpr (std::is_signed_v<R>) {
    // If 'right' is negative, then result is 'false', otherwise cast & compare.
    return rhs > 0 && lhs < static_cast<std::make_unsigned_t<R>>(rhs);
  } else {
    // If 'left' is negative, then result is 'true', otherwise cast & compare.
    return lhs < 0 || static_cast<std::make_unsigned_t<L>>(lhs) < rhs;
  }
}

template <typename E>
constexpr auto n() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::n requires enum type.");
#if defined(MAGIC_ENUM_SUPPORTED) && MAGIC_ENUM_SUPPORTED
#  if defined(__clang__)
  constexpr std::string_view name{__PRETTY_FUNCTION__ + 34, sizeof(__PRETTY_FUNCTION__) - 36};
#  elif defined(__GNUC__)
  constexpr std::string_view name{__PRETTY_FUNCTION__ + 49, sizeof(__PRETTY_FUNCTION__) - 51};
#  elif defined(_MSC_VER)
  constexpr std::string_view name{__FUNCSIG__ + 40, sizeof(__FUNCSIG__) - 57};
#  endif
  return static_string<name.size()>{name};
#else
  return std::string_view{}; // Unsupported compiler.
#endif
}

template <typename E>
inline constexpr auto type_name_v = n<E>();

template <typename E, E V>
constexpr auto n() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::n requires enum type.");
#if defined(MAGIC_ENUM_SUPPORTED) && MAGIC_ENUM_SUPPORTED
#  if defined(__clang__) || defined(__GNUC__)
  constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#  elif defined(_MSC_VER)
  constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#  endif
  return static_string<name.size()>{name};
#else
  return std::string_view{}; // Unsupported compiler.
#endif
}

template <typename E, E V>
inline constexpr auto name_v = n<E, V>();

template <typename E, int Min, int Max>
constexpr std::size_t range_size() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::range_size requires enum type.");
  constexpr auto size = Max - Min + 1;
  static_assert(size > 0, "magic_enum::enum_range requires valid size.");
  static_assert(size < (std::numeric_limits<std::uint16_t>::max)(), "magic_enum::enum_range requires valid size.");

  return static_cast<std::size_t>(size);
}

template <typename E>
constexpr int reflected_min() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::reflected_min requires enum type.");
  constexpr auto lhs = enum_range<E>::min;
  static_assert(lhs > (std::numeric_limits<std::int16_t>::min)(), "magic_enum::enum_range requires min must be greater than INT16_MIN.");
  constexpr auto rhs = (std::numeric_limits<std::underlying_type_t<E>>::min)();

  return cmp_less(lhs, rhs) ? rhs : lhs;
}

template <typename E>
constexpr int reflected_max() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::reflected_max requires enum type.");
  constexpr auto lhs = enum_range<E>::max;
  static_assert(lhs < (std::numeric_limits<std::int16_t>::max)(), "magic_enum::enum_range requires max must be less than INT16_MAX.");
  constexpr auto rhs = (std::numeric_limits<std::underlying_type_t<E>>::max)();

  return cmp_less(lhs, rhs) ? lhs : rhs;
}

template <typename E>
inline constexpr int reflected_min_v = reflected_min<E>();

template <typename E>
inline constexpr int reflected_max_v = reflected_max<E>();

template <typename E, int... I>
constexpr auto values(std::integer_sequence<int, I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::values requires enum type.");
  constexpr std::array<bool, sizeof...(I)> valid{{(n<E, static_cast<E>(I + reflected_min_v<E>)>().size() != 0)...}};
  constexpr int count = ((valid[I] ? 1 : 0) + ...);

  std::array<E, count> values{};
  for (int i = 0, v = 0; v < count; ++i) {
    if (valid[i]) {
      values[v++] = static_cast<E>(i + reflected_min_v<E>);
    }
  }

  return values;
}

template <typename E>
inline constexpr auto values_v = values<E>(std::make_integer_sequence<int, range_size<E, reflected_min_v<E>, reflected_max_v<E>>()>{});

template <typename E>
inline constexpr std::size_t count_v = values_v<E>.size();

template <typename E>
inline constexpr int min_v = values_v<E>.empty() ? 0 : static_cast<int>(values_v<E>.front());

template <typename E>
inline constexpr int max_v = values_v<E>.empty() ? 0 : static_cast<int>(values_v<E>.back());

template <typename E>
inline constexpr std::size_t range_size_v = range_size<E, min_v<E>, max_v<E>>();

template <typename E>
using index_t = std::conditional_t<range_size_v<E> < (std::numeric_limits<std::uint8_t>::max)(), std::uint8_t, std::uint16_t>;

template <typename E>
inline constexpr auto invalid_index_v = (std::numeric_limits<index_t<E>>::max)();

template <typename E, int... I>
constexpr auto indexes(std::integer_sequence<int, I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::indexes requires enum type.");
  index_t<E> i = 0;

  return std::array<index_t<E>, sizeof...(I)>{{((n<E, static_cast<E>(I + min_v<E>)>().size() != 0) ? i++ : invalid_index_v<E>)...}};
}

template <typename E>
inline constexpr auto indexes_v = indexes<E>(std::make_integer_sequence<int, range_size_v<E>>{});

template <typename E, std::size_t... I>
constexpr auto names(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::names requires enum type.");

  return std::array<std::string_view, sizeof...(I)>{{name_v<E, values_v<E>[I]>...}};
}

template <typename E>
inline constexpr auto names_v = names<E>(std::make_index_sequence<count_v<E>>{});

template <typename E, std::size_t... I>
constexpr auto entries(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::entries requires enum type.");

  return std::array<std::pair<E, std::string_view>, sizeof...(I)>{{{values_v<E>[I], name_v<E, values_v<E>[I]>}...}};
}

template <typename E>
inline constexpr auto entries_v = entries<E>(std::make_index_sequence<count_v<E>>{});

template <typename E>
inline constexpr bool is_sparse_v = range_size_v<E> != count_v<E>;

template <typename E, typename U = std::underlying_type_t<E>>
constexpr int undex(U value) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::undex requires enum type.");

  if (const auto i = static_cast<int>(value) - min_v<E>; value >= static_cast<U>(min_v<E>) && value <= static_cast<U>(max_v<E>)) {
    if constexpr (is_sparse_v<E>) {
      if (const auto idx = indexes_v<E>[i]; idx != invalid_index_v<E>) {
        return idx;
      }
    } else {
      return i;
    }
  }

  return -1; // Value out of range.
}

template <typename E>
constexpr int endex(E value) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::endex requires enum type.");

  return undex<E>(static_cast<std::underlying_type_t<E>>(value));
}

template <typename T, typename R>
using enable_if_enum_t = std::enable_if_t<std::is_enum_v<std::decay_t<T>>, R>;

template <typename T, typename Enable = std::enable_if_t<std::is_enum_v<std::decay_t<T>>>>
using enum_concept = T;

template <typename T, bool = std::is_enum_v<T>>
struct is_scoped_enum : std::false_type {};

template <typename T>
struct is_scoped_enum<T, true> : std::bool_constant<!std::is_convertible_v<T, std::underlying_type_t<T>>> {};

template <typename T, bool = std::is_enum_v<T>>
struct is_unscoped_enum : std::false_type {};

template <typename T>
struct is_unscoped_enum<T, true> : std::bool_constant<std::is_convertible_v<T, std::underlying_type_t<T>>> {};

template <typename T, bool = std::is_enum_v<std::decay_t<T>>>
struct underlying_type {};

template <typename T>
struct underlying_type<T, true> : std::underlying_type<std::decay_t<T>> {};

} // namespace magic_enum::detail

// Checks is magic_enum supported compiler.
inline constexpr bool is_magic_enum_supported = detail::supported<void>::value;

template <typename T>
using Enum = detail::enum_concept<T>;

// Checks whether T is an Unscoped enumeration type.
// Provides the member constant value which is equal to true, if T is an [Unscoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Unscoped_enumeration) type. Otherwise, value is equal to false.
template <typename T>
struct is_unscoped_enum : detail::is_unscoped_enum<T> {};

template <typename T>
inline constexpr bool is_unscoped_enum_v = is_unscoped_enum<T>::value;

// Checks whether T is an Scoped enumeration type.
// Provides the member constant value which is equal to true, if T is an [Scoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations) type. Otherwise, value is equal to false.
template <typename T>
struct is_scoped_enum : detail::is_scoped_enum<T> {};

template <typename T>
inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;

// If T is a complete enumeration type, provides a member typedef type that names the underlying type of T.
// Otherwise, if T is not an enumeration type, there is no member type. Otherwise (T is an incomplete enumeration type), the program is ill-formed.
template <typename T>
struct underlying_type : detail::underlying_type<T> {};

template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

// Returns string name of enum type.
template <typename E>
[[nodiscard]] constexpr auto enum_type_name() noexcept -> detail::enable_if_enum_t<E, std::string_view> {
  using D = std::decay_t<E>;
  constexpr std::string_view name = detail::type_name_v<D>;
  static_assert(name.size() > 0, "Enum type does not have a name.");

  return name;
}

// Returns number of enum values.
template <typename E>
[[nodiscard]] constexpr auto enum_count() noexcept -> detail::enable_if_enum_t<E, std::size_t> {
  using D = std::decay_t<E>;
  static_assert(detail::supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "magic_enum requires enum implementation and valid max and min.");

  return detail::count_v<D>;
}

// Returns enum value at specified index.
// No bounds checking is performed: the behavior is undefined if index >= number of enum values.
template <typename E>
[[nodiscard]] constexpr auto enum_value(std::size_t index) noexcept -> detail::enable_if_enum_t<E, std::decay_t<E>> {
  using D = std::decay_t<E>;
  static_assert(detail::supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "magic_enum requires enum implementation and valid max and min.");

  if constexpr (detail::is_sparse_v<D>) {
    return assert(index < detail::count_v<D>), detail::values_v<D>[index];
  } else {
    return assert(index < detail::count_v<D>), static_cast<D>(index + detail::min_v<D>);
  }
}

// Obtains value enum sequence.
// Returns std::array with enum values, sorted by enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_values() noexcept -> detail::enable_if_enum_t<E, decltype(detail::values_v<std::decay_t<E>>)&> {
  using D = std::decay_t<E>;
  static_assert(detail::supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "magic_enum requires enum implementation and valid max and min.");

  return detail::values_v<D>;
}

// Returns string enum name from static storage enum variable.
// This version is much lighter on the compile times and is not restricted to the enum_range limitation.
template <auto V>
[[nodiscard]] constexpr auto enum_name() noexcept -> detail::enable_if_enum_t<decltype(V), std::string_view> {
  constexpr std::string_view name = detail::name_v<std::decay_t<decltype(V)>, V>;
  static_assert(name.size() > 0, "Enum value does not have a name.");

  return name;
}

// Returns string enum name from enum value.
// If enum value does not have name or value out of range, returns empty string.
template <typename E>
[[nodiscard]] constexpr auto enum_name(E value) noexcept -> detail::enable_if_enum_t<E, std::string_view> {
  using D = std::decay_t<E>;
  static_assert(detail::supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "magic_enum requires enum implementation and valid max and min.");

  if (const auto i = detail::endex<D>(value); i != -1) {
    return detail::names_v<D>[i];
  }

  return {}; // Value out of range.
}

// Obtains string enum name sequence.
// Returns std::array with string enum names, sorted by enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_names() noexcept -> detail::enable_if_enum_t<E, decltype(detail::names_v<std::decay_t<E>>)&> {
  using D = std::decay_t<E>;
  static_assert(detail::supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "magic_enum requires enum implementation and valid max and min.");

  return detail::names_v<D>;
}

// Obtains pair (value enum, string enum name) sequence.
// Returns std::array with std::pair (value enum, string enum name), sorted by enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_entries() noexcept -> detail::enable_if_enum_t<E, decltype(detail::entries_v<std::decay_t<E>>)&> {
  using D = std::decay_t<E>;
  static_assert(detail::supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "magic_enum requires enum implementation and valid max and min.");

  return detail::entries_v<D>;
}

// Obtains enum value from enum string name.
// Returns std::optional with enum value.
template <typename E, typename BinaryPredicate>
[[nodiscard]] constexpr auto enum_cast(std::string_view value, BinaryPredicate p) noexcept(std::is_nothrow_invocable_r_v<bool, BinaryPredicate, char, char>) -> detail::enable_if_enum_t<E, std::optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  static_assert(detail::supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "magic_enum requires enum implementation and valid max and min.");
  static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>, "magic_enum::enum_cast requires bool(char, char) invocable predicate.");

  for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
    if (detail::cmp_equal(value, detail::names_v<D>[i], std::move(p))) {
      return enum_value<D>(i);
    }
  }

  return std::nullopt; // Invalid value or out of range.
}

template <typename E>
[[nodiscard]] constexpr auto enum_cast(std::string_view value) noexcept -> detail::enable_if_enum_t<E, std::optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  static_assert(detail::supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "magic_enum requires enum implementation and valid max and min.");

  for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
    if (value == detail::names_v<D>[i]) {
      return enum_value<D>(i);
    }
  }

  return std::nullopt; // Invalid value or out of range.
}

// Obtains enum value from integer value.
// Returns std::optional with enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_cast(underlying_type_t<E> value) noexcept -> detail::enable_if_enum_t<E, std::optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  static_assert(detail::supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "magic_enum requires enum implementation and valid max and min.");

  if (detail::undex<D>(value) != -1) {
    return static_cast<D>(value);
  }

  return std::nullopt; // Invalid value or out of range.
}

// Returns integer value from enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_integer(E value) noexcept -> detail::enable_if_enum_t<E, underlying_type_t<E>> {
  return static_cast<underlying_type_t<E>>(value);
}

// Obtains index in enum value sequence from enum value.
// Returns std::optional with index.
template <typename E>
[[nodiscard]] constexpr auto enum_index(E value) noexcept -> detail::enable_if_enum_t<E, std::optional<std::size_t>> {
  using D = std::decay_t<E>;
  static_assert(detail::supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "magic_enum requires enum implementation and valid max and min.");

  if (const auto i = detail::endex<D>(value); i != -1) {
    return i;
  }

  return std::nullopt; // Value out of range.
}

// Checks whether enum contains enumerator with such value.
template <typename E>
[[nodiscard]] constexpr auto enum_contains(E value) noexcept -> detail::enable_if_enum_t<E, bool> {
  using D = std::decay_t<E>;
  static_assert(detail::supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "magic_enum requires enum implementation and valid max and min.");

  return detail::endex<D>(value) != -1;
}

// Checks whether enum contains enumerator with such integer value.
template <typename E>
[[nodiscard]] constexpr auto enum_contains(underlying_type_t<E> value) noexcept -> detail::enable_if_enum_t<E, bool> {
  using D = std::decay_t<E>;
  static_assert(detail::supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "magic_enum requires enum implementation and valid max and min.");

  return detail::undex<D>(value) != -1;
}

// Checks whether enum contains enumerator with such string enum name.
template <typename E>
[[nodiscard]] constexpr auto enum_contains(std::string_view value) noexcept -> detail::enable_if_enum_t<E, bool> {
  using D = std::decay_t<E>;
  static_assert(detail::supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "magic_enum requires enum implementation and valid max and min.");

  return enum_cast<D>(value).has_value();
}

namespace ostream_operators {

template <typename Char, typename Traits, typename E>
auto operator<<(std::basic_ostream<Char, Traits>& os, E value) -> detail::enable_if_enum_t<E, std::basic_ostream<Char, Traits>&> {
  if (const auto name = enum_name(value); !name.empty()) {
    for (const auto c : name) {
      os.put(c);
    }
  } else {
    os << enum_integer(value);
  }

  return os;
}

template <typename Char, typename Traits, typename E>
auto operator<<(std::basic_ostream<Char, Traits>& os, std::optional<E> value) -> detail::enable_if_enum_t<E, std::basic_ostream<Char, Traits>&> {
  if (value.has_value()) {
    os << value.value();
  }

  return os;
}

} // namespace magic_enum::ostream_operators

namespace bitwise_operators {

template <typename E>
constexpr auto operator~(E rhs) noexcept -> detail::enable_if_enum_t<E, E> {
  return static_cast<E>(~static_cast<underlying_type_t<E>>(rhs));
}

template <typename E>
constexpr auto operator|(E lhs, E rhs) noexcept -> detail::enable_if_enum_t<E, E> {
  return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) | static_cast<underlying_type_t<E>>(rhs));
}

template <typename E>
constexpr auto operator&(E lhs, E rhs) noexcept -> detail::enable_if_enum_t<E, E> {
  return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) & static_cast<underlying_type_t<E>>(rhs));
}

template <typename E>
constexpr auto operator^(E lhs, E rhs) noexcept -> detail::enable_if_enum_t<E, E> {
  return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) ^ static_cast<underlying_type_t<E>>(rhs));
}

template <typename E>
constexpr auto operator|=(E& lhs, E rhs) noexcept -> detail::enable_if_enum_t<E, E&> {
  return lhs = lhs | rhs;
}

template <typename E>
constexpr auto operator&=(E& lhs, E rhs) noexcept -> detail::enable_if_enum_t<E, E&> {
  return lhs = lhs & rhs;
}

template <typename E>
constexpr auto operator^=(E& lhs, E rhs) noexcept -> detail::enable_if_enum_t<E, E&> {
  return lhs = lhs ^ rhs;
}

} // namespace magic_enum::bitwise_operators

} // namespace magic_enum

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // NEARGYE_MAGIC_ENUM_HPP
