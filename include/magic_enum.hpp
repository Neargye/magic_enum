//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.6.6
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

#define MAGIC_ENUM_VERSION_MAJOR 0
#define MAGIC_ENUM_VERSION_MINOR 6
#define MAGIC_ENUM_VERSION_PATCH 6

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

#if defined(__clang__)
#  pragma clang diagnostic push
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 26495) // Variable 'static_string<N>::chars' is uninitialized.
#endif

// Checks magic_enum compiler compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1910
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

template <typename E, typename = void>
struct has_min : std::false_type {};

template <typename E>
struct has_min<E, std::void_t<decltype(enum_range<E>::min)>> : std::true_type {};

template <typename E, typename = void>
struct has_max : std::false_type {};

template <typename E>
struct has_max<E, std::void_t<decltype(enum_range<E>::max)>> : std::true_type {};

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
constexpr bool cmp_equal(std::string_view lhs, std::string_view rhs, BinaryPredicate&& p) noexcept(std::is_nothrow_invocable_r_v<bool, BinaryPredicate, char, char>) {
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

template <typename T>
constexpr std::uint8_t log2(T value) noexcept {
  if constexpr (std::is_enum_v<T>) {
    using U = std::underlying_type_t<T>;

    return log2<U>(static_cast<U>(value));
  } else {
    auto ret = std::uint8_t{0};
    for (; value > static_cast<T>(1); value >>= static_cast<T>(1), ++ret) {};

    return ret;
  }
}

template <typename T>
constexpr bool is_pow2(T x) noexcept {
  if constexpr (std::is_enum_v<T>) {
    using U = std::underlying_type_t<T>;

    return is_pow2<U>(static_cast<U>(x));
  } else {
    return x != 0 && (x & (x - 1)) == 0;
  }
}

#if defined(NEARGYE_NAMEOF_HPP)
using ::nameof::detail::type_name_v;
#else
template <typename... T>
constexpr auto n() noexcept {
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 7 || defined(_MSC_VER) && _MSC_VER >= 1910
#  if defined(__clang__)
  constexpr std::string_view name{__PRETTY_FUNCTION__ + 35, sizeof(__PRETTY_FUNCTION__) - 38};
#  elif defined(__GNUC__)
  constexpr std::string_view name{__PRETTY_FUNCTION__ + 50, sizeof(__PRETTY_FUNCTION__) - 53};
#  elif defined(_MSC_VER)
  constexpr std::string_view name{__FUNCSIG__ + 40, sizeof(__FUNCSIG__) - 57};
#  endif
  return static_string<name.size()>{name};
#else
  return std::string_view{}; // Unsupported compiler.
#endif
}

template <typename... T>
inline constexpr auto type_name_v = n<T...>();
#endif

#if defined(NEARGYE_NAMEOF_HPP)
using ::nameof::detail::enum_name_v;
#else
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
inline constexpr auto enum_name_v = n<E, V>();
#endif

template <typename E, auto V>
constexpr bool is_valid() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::is_valid requires enum type.");

  return n<E, static_cast<E>(V)>().size() != 0;
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr int reflected_min() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::reflected_min requires enum type.");

  if constexpr (IsFlags) {
    return 0;
  } else {
    static_assert(has_min<E>::value, "magic_enum::enum_range requires min.");
    constexpr auto lhs = enum_range<E>::min;
    static_assert(lhs > (std::numeric_limits<std::int16_t>::min)(), "magic_enum::enum_range requires min must be greater than INT16_MIN.");
    constexpr auto rhs = (std::numeric_limits<U>::min)();

    if constexpr (cmp_less(lhs, rhs)) {
      return rhs;
    } else {
      return lhs;
    }
  }
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr int reflected_max() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::reflected_max requires enum type.");

  if constexpr (IsFlags) {
    return (std::numeric_limits<U>::max)();
  } else {
    static_assert(has_max<E>::value, "magic_enum::enum_range requires max.");
    constexpr auto lhs = enum_range<E>::max;
    static_assert(lhs < (std::numeric_limits<std::int16_t>::max)(), "magic_enum::enum_range requires max must be less than INT16_MAX.");
    constexpr auto rhs = (std::numeric_limits<U>::max)();

    if constexpr (cmp_less(lhs, rhs)) {
      return lhs;
    } else {
      return rhs;
    }
  }
}

template <typename E, bool IsFlags = false>
inline constexpr auto reflected_min_v = reflected_min<E, IsFlags>();

template <typename E, bool IsFlags = false>
inline constexpr auto reflected_max_v = reflected_max<E, IsFlags>();

template <typename E, int O, bool IsFlags = false, typename U = std::underlying_type_t<E>>
constexpr E value(std::size_t i) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::value requires enum type.");

  if constexpr (IsFlags) {
    return static_cast<E>(static_cast<U>(1) << static_cast<U>(static_cast<int>(i) + O));
  } else {
    return static_cast<E>(static_cast<int>(i) + O);
  }
}

template <typename E, bool IsFlags, int Min, int... I>
constexpr auto values(std::integer_sequence<int, I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::values requires enum type.");
  constexpr std::array<bool, sizeof...(I)> valid{{is_valid<E, value<E, Min, IsFlags>(I)>()...}};
  constexpr std::size_t count = ((valid[I] ? 1 : 0) + ...);

  std::array<E, count> values{};
  for (std::size_t i = 0, v = 0; v < count; ++i) {
    if (valid[i]) {
      values[v++] = value<E, Min, IsFlags>(i);
    }
  }

  return values;
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr auto values() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::values requires enum type.");

  if constexpr (IsFlags) {
    return values<E, true, 0>(std::make_integer_sequence<int, std::numeric_limits<U>::digits>{});
  } else {
    constexpr auto range_size = reflected_max_v<E> - reflected_min_v<E> + 1;
    static_assert(range_size > 0, "magic_enum::enum_range requires valid size.");
    static_assert(range_size < (std::numeric_limits<std::uint16_t>::max)(), "magic_enum::enum_range requires valid size.");
    return values<E, false, reflected_min_v<E>>(std::make_integer_sequence<int, range_size>{});
  }
}

template <typename E, bool IsFlags = false>
inline constexpr auto values_v = values<E, IsFlags>();

template <typename E, bool IsFlags = false, typename D = std::decay_t<E>>
using values_t = decltype((values_v<D, IsFlags>));

template <typename E, bool IsFlags = false>
inline constexpr auto count_v = values_v<E, IsFlags>.size();

template <typename E, bool IsFlags = false, typename U = std::underlying_type_t<E>>
inline constexpr auto min_v = static_cast<U>(values_v<E, IsFlags>.front());

template <typename E, bool IsFlags = false, typename U = std::underlying_type_t<E>>
inline constexpr auto max_v = static_cast<U>(values_v<E, IsFlags>.back());

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr std::size_t range_size() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::range_size requires enum type.");

  if constexpr (IsFlags) {
    return std::numeric_limits<U>::digits;
  } else {
    constexpr auto range_size = max_v<E> - min_v<E> + 1;
    static_assert(range_size > 0, "magic_enum::enum_range requires valid size.");
    static_assert(range_size < (std::numeric_limits<std::uint16_t>::max)(), "magic_enum::enum_range requires valid size.");
    return static_cast<std::size_t>(range_size);
  }
}

template <typename E, bool IsFlags = false>
inline constexpr auto range_size_v = range_size<E, IsFlags>();

template <typename E>
using index_t = std::conditional_t<range_size_v<E> < (std::numeric_limits<std::uint8_t>::max)(), std::uint8_t, std::uint16_t>;

template <typename E>
inline constexpr auto invalid_index_v = (std::numeric_limits<index_t<E>>::max)();

template <typename E, int... I>
constexpr auto indexes(std::integer_sequence<int, I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::indexes requires enum type.");
  [[maybe_unused]] auto i = index_t<E>{0};

  return std::array<index_t<E>, sizeof...(I)>{{(is_valid<E, I + min_v<E>>() ? i++ : invalid_index_v<E>)...}};
}

template <typename E>
inline constexpr auto indexes_v = indexes<E>(std::make_integer_sequence<int, range_size_v<E>>{});

template <typename E, bool IsFlags, std::size_t... I>
constexpr auto names(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::names requires enum type.");

  return std::array<std::string_view, sizeof...(I)>{{enum_name_v<E, values_v<E, IsFlags>[I]>...}};
}

template <typename E, bool IsFlags = false>
inline constexpr auto names_v = names<E, IsFlags>(std::make_index_sequence<count_v<E, IsFlags>>{});

template <typename E, bool IsFlags = false, typename D = std::decay_t<E>>
using names_t = decltype((names_v<D, IsFlags>));

template <typename E, bool IsFlags, std::size_t... I>
constexpr auto entries(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::entries requires enum type.");

  return std::array<std::pair<E, std::string_view>, sizeof...(I)>{{{values_v<E, IsFlags>[I], enum_name_v<E, values_v<E, IsFlags>[I]>}...}};
}

template <typename E, bool IsFlags = false>
inline constexpr auto entries_v = entries<E, IsFlags>(std::make_index_sequence<count_v<E, IsFlags>>{});

template <typename E, bool IsFlags = false, typename D = std::decay_t<E>>
using entries_t = decltype((entries_v<D, IsFlags>));

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr bool is_sparse() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::is_sparse requires enum type.");

  if constexpr (IsFlags) {
    auto range_count = std::size_t{0};
    for (auto i = max_v<E, true>; i != max_v<E, true>; i <<= static_cast<U>(1), ++range_count) {};

    return range_count != count_v<E, true>;
  } else {
    return range_size_v<E, false> != count_v<E, false>;
  }
}

template <typename E, bool IsFlags = false>
inline constexpr bool is_sparse_v = is_sparse<E, IsFlags>();

template <typename E, typename U = std::underlying_type_t<E>>
constexpr std::size_t undex(U value) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::undex requires enum type.");

  if (const auto i = static_cast<std::size_t>(value - min_v<E>); value >= min_v<E> && value <= max_v<E>) {
    if constexpr (is_sparse_v<E>) {
      if (const auto idx = indexes_v<E>[i]; idx != invalid_index_v<E>) {
        return idx;
      }
    } else {
      return i;
    }
  }

  return invalid_index_v<E>; // Value out of range.
}

template <typename E, typename U = std::underlying_type_t<E>>
constexpr std::size_t endex(E value) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::endex requires enum type.");

  return undex<E>(static_cast<U>(value));
}

template <typename E, typename U = std::underlying_type_t<E>>
constexpr U value_ors() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::endex requires enum type.");

  auto value = U{0};
  for (std::size_t i = 0; i < count_v<E, true>; ++i) {
    value |= static_cast<U>(values_v<E, true>[i]);
  }

  return value;
}

template <bool, bool, typename T, typename R>
struct enable_if_enum {};

template <typename T, typename R>
struct enable_if_enum<true, false, T, R> {
  using type = R;
  using D = std::decay_t<T>;
  static_assert(supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(count_v<D, false> > 0, "magic_enum requires enum implementation and valid max and min.");
};

template <typename T, typename R>
struct enable_if_enum<true, true, T, R> {
  using type = R;
  using D = std::decay_t<T>;
  static_assert(supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(count_v<D, true> > 0, "magic_enum::flags requires enum-flags implementation.");
};

template <typename T, typename R = void>
using enable_if_enum_t = typename enable_if_enum<std::is_enum_v<std::decay_t<T>>, false, T, R>::type;

template <typename T, typename R = void>
using enable_if_enum_flags_t = typename enable_if_enum<std::is_enum_v<std::decay_t<T>>, true, T, R>::type;

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
[[nodiscard]] constexpr std::string_view enum_type_name() noexcept {
  using D = std::decay_t<E>;
  static_assert(std::is_enum_v<D>, "Requires enum type.");
  constexpr std::string_view name = detail::type_name_v<D>;
  static_assert(name.size() > 0, "Enum type does not have a name.");

  return name;
}

// Returns number of enum values.
template <typename E>
[[nodiscard]] constexpr auto enum_count() noexcept -> detail::enable_if_enum_t<E, std::size_t> {
  using D = std::decay_t<E>;

  return detail::count_v<D>;
}

// Returns enum value at specified index.
// No bounds checking is performed: the behavior is undefined if index >= number of enum values.
template <typename E>
[[nodiscard]] constexpr auto enum_value(std::size_t index) noexcept -> detail::enable_if_enum_t<E, std::decay_t<E>> {
  using D = std::decay_t<E>;

  if constexpr (detail::is_sparse_v<D>) {
    return assert((index < detail::count_v<D>)), detail::values_v<D>[index];
  } else {
    return assert((index < detail::count_v<D>)), detail::value<D, detail::min_v<D>>(index);
  }
}

// Returns std::array with enum values, sorted by enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_values() noexcept -> detail::enable_if_enum_t<E, detail::values_t<E>> {
  using D = std::decay_t<E>;

  return detail::values_v<D>;
}

// Returns string name from static storage enum variable.
// This version is much lighter on the compile times and is not restricted to the enum_range limitation.
template <auto V>
[[nodiscard]] constexpr std::string_view enum_name() noexcept {
  using D = std::decay_t<decltype(V)>;
  static_assert(std::is_enum_v<D>, "Requires enum type.");
  constexpr std::string_view name = detail::enum_name_v<D, V>;
  static_assert(name.size() > 0, "Enum value does not have a name.");

  return name;
}

// Returns string name from enum value.
// If enum value does not have name or value out of range, returns empty string.
template <typename E>
[[nodiscard]] constexpr auto enum_name(E value) noexcept -> detail::enable_if_enum_t<E, std::string_view> {
  using D = std::decay_t<E>;

  if (const auto i = detail::endex<D>(value); i != detail::invalid_index_v<D>) {
    return detail::names_v<D>[i];
  }

  return {}; // Value out of range.
}

// Returns std::array with string names, sorted by enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_names() noexcept -> detail::enable_if_enum_t<E, detail::names_t<E>> {
  using D = std::decay_t<E>;

  return detail::names_v<D>;
}

// Returns std::array with pairs (enum value, string name), sorted by enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_entries() noexcept -> detail::enable_if_enum_t<E, detail::entries_t<E>> {
  using D = std::decay_t<E>;

  return detail::entries_v<D>;
}

// Obtains enum value from string name.
// Returns std::optional with enum value.
template <typename E, typename BinaryPredicate>
[[nodiscard]] constexpr auto enum_cast(std::string_view value, BinaryPredicate p) noexcept(std::is_nothrow_invocable_r_v<bool, BinaryPredicate, char, char>) -> detail::enable_if_enum_t<E, std::optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>, "magic_enum::enum_cast requires bool(char, char) invocable predicate.");

  for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
    if (detail::cmp_equal(value, detail::names_v<D>[i], p)) {
      return enum_value<D>(i);
    }
  }

  return std::nullopt; // Invalid value or out of range.
}

// Obtains enum value from string name.
// Returns std::optional with enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_cast(std::string_view value) noexcept -> detail::enable_if_enum_t<E, std::optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;

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

  if (detail::undex<D>(value) != detail::invalid_index_v<D>) {
    return static_cast<D>(value);
  }

  return std::nullopt; // Invalid value or out of range.
}

// Returns integer value from enum value.
template <typename E>
[[nodiscard]] constexpr underlying_type_t<E> enum_integer(E value) noexcept {
  return static_cast<underlying_type_t<E>>(value);
}

// Obtains index in enum values from enum value.
// Returns std::optional with index.
template <typename E>
[[nodiscard]] constexpr auto enum_index(E value) noexcept -> detail::enable_if_enum_t<E, std::optional<std::size_t>> {
  using D = std::decay_t<E>;

  if (const auto i = detail::endex<D>(value); i != detail::invalid_index_v<D>) {
    return i;
  }

  return std::nullopt; // Value out of range.
}

// Checks whether enum contains enumerator with such enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_contains(E value) noexcept -> detail::enable_if_enum_t<E, bool> {
  using D = std::decay_t<E>;

  return detail::endex<D>(value) != detail::invalid_index_v<D>;
}

// Checks whether enum contains enumerator with such integer value.
template <typename E>
[[nodiscard]] constexpr auto enum_contains(underlying_type_t<E> value) noexcept -> detail::enable_if_enum_t<E, bool> {
  using D = std::decay_t<E>;

  return detail::undex<D>(value) != detail::invalid_index_v<D>;
}

// Checks whether enum contains enumerator with such string name.
template <typename E>
[[nodiscard]] constexpr auto enum_contains(std::string_view value) noexcept -> detail::enable_if_enum_t<E, bool> {
  using D = std::decay_t<E>;

  return enum_cast<D>(value).has_value();
}

namespace ostream_operators {

template <typename Char, typename Traits, typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
auto& operator<<(std::basic_ostream<Char, Traits>& os, E value) {
#if defined(MAGIC_ENUM_SUPPORTED) && MAGIC_ENUM_SUPPORTED
  using D = std::decay_t<E>;
  using namespace magic_enum;

  if (const auto name = enum_name<D>(value); !name.empty()) {
    for (const auto c : name) {
      os.put(c);
    }
  } else {
    os << enum_integer<D>(value);
  }
#else
  os << static_cast<std::underlying_type_t<E>>(value);
#endif
  return os;
}

template <typename Char, typename Traits, typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
auto& operator<<(std::basic_ostream<Char, Traits>& os, std::optional<E> value) {
  if (value.has_value()) {
    os << value.value();
  }

  return os;
}

} // namespace magic_enum::ostream_operators

namespace bitwise_operators {

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E operator~(E rhs) noexcept {
  return static_cast<E>(~static_cast<underlying_type_t<E>>(rhs));
}

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E operator|(E lhs, E rhs) noexcept {
  return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) | static_cast<underlying_type_t<E>>(rhs));
}

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E operator&(E lhs, E rhs) noexcept {
  return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) & static_cast<underlying_type_t<E>>(rhs));
}

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E operator^(E lhs, E rhs) noexcept {
  return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) ^ static_cast<underlying_type_t<E>>(rhs));
}

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E& operator|=(E& lhs, E rhs) noexcept {
  return lhs = lhs | rhs;
}

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E& operator&=(E& lhs, E rhs) noexcept {
  return lhs = lhs & rhs;
}

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E& operator^=(E& lhs, E rhs) noexcept {
  return lhs = lhs ^ rhs;
}

} // namespace magic_enum::bitwise_operators

namespace flags {

// Returns number of enum-flags values.
template <typename E>
[[nodiscard]] constexpr auto enum_count() noexcept -> detail::enable_if_enum_flags_t<E, std::size_t> {
  using D = std::decay_t<E>;

  return detail::count_v<D, true>;
}

// Returns enum-flags value at specified index.
// No bounds checking is performed: the behavior is undefined if index >= number of enum-flags values.
template <typename E>
[[nodiscard]] constexpr auto enum_value(std::size_t index) noexcept -> detail::enable_if_enum_flags_t<E, std::decay_t<E>> {
  using D = std::decay_t<E>;

  if constexpr (detail::is_sparse_v<D, true>) {
    return assert((index < detail::count_v<D, true>)), detail::values_v<D, true>[index];
  } else {
    constexpr auto min = detail::log2(detail::min_v<D, true>) - 1;

    return assert((index < detail::count_v<D, true>)), detail::value<D, min, true>(index);
  }
}

// Returns std::array with enum-flags values, sorted by enum-flags value.
template <typename E>
[[nodiscard]] constexpr auto enum_values() noexcept -> detail::enable_if_enum_flags_t<E, detail::values_t<E, true>> {
  using D = std::decay_t<E>;

  return detail::values_v<D, true>;
}

// Returns string name from enum-flags value.
// If enum-flags value does not have name or value out of range, returns empty string.
template <typename E>
[[nodiscard]] auto enum_name(E value) -> detail::enable_if_enum_flags_t<E, std::string> {
  using D = std::decay_t<E>;
  using U = std::underlying_type_t<D>;

  std::string name;
  for (std::size_t i = 0; i < detail::count_v<D, true>; ++i) {
    if (const auto v = enum_value<D>(i); (static_cast<U>(value) & static_cast<U>(v)) != 0) {
      const auto n = detail::names_v<D, true>[i];
      if (!name.empty()) {
        name.append(1, '|');
      }
      name.append(n.data(), n.size());
    }
  }

  return name;
}

// Returns std::array with string names, sorted by enum-flags value.
template <typename E>
[[nodiscard]] constexpr auto enum_names() noexcept -> detail::enable_if_enum_flags_t<E, detail::names_t<E, true>> {
  using D = std::decay_t<E>;

  return detail::names_v<D, true>;
}

// Returns std::array with pairs (enum-flags value, string name), sorted by enum-flags value.
template <typename E>
[[nodiscard]] constexpr auto enum_entries() noexcept -> detail::enable_if_enum_flags_t<E, detail::entries_t<E, true>> {
  using D = std::decay_t<E>;

  return detail::entries_v<D, true>;
}

// Checks whether enum-flags contains enumerator with such integer value.
template <typename E>
[[nodiscard]] constexpr auto enum_contains(underlying_type_t<E> value) noexcept -> detail::enable_if_enum_flags_t<E, bool> {
  using D = std::decay_t<E>;
  using U = std::underlying_type_t<D>;

  if constexpr (detail::is_sparse_v<D, true>) {
    constexpr auto min = detail::min_v<D, true>;
    constexpr auto max = detail::value_ors<D>();

    return value >= min && value <= max;
  } else {
    auto check_value = U{0};
    for (std::size_t i = 0; i < detail::count_v<D, true>; ++i) {
      if (const auto v = static_cast<U>(enum_value<D>(i)); (static_cast<U>(value) & v) != 0) {
        check_value |= v;
      }
    }

    return check_value == value;
  }
}

// Checks whether enum-flags contains enumerator with such enum-flags value.
template <typename E>
[[nodiscard]] constexpr auto enum_contains(E value) noexcept -> detail::enable_if_enum_flags_t<E, bool> {
  using D = std::decay_t<E>;
  using U = std::underlying_type_t<D>;

  return enum_contains<D>(static_cast<U>(value));
}

// Checks whether enum-flags contains enumerator with such string name.
template <typename E>
[[nodiscard]] constexpr auto enum_contains(std::string_view value) noexcept -> detail::enable_if_enum_flags_t<E, bool> {
  // TODO: impl
  static_assert(sizeof(decltype(value)) == sizeof(E) * 0, "not implemented");
  return {};
}

// Obtains enum-flags value from integer value.
// Returns std::optional with enum-flags value.
template <typename E>
[[nodiscard]] constexpr auto enum_cast(underlying_type_t<E> value) noexcept -> detail::enable_if_enum_flags_t<E, std::optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;

  if (enum_contains<D>(value)) {
    return static_cast<D>(value);
  }

  return std::nullopt; // Invalid value or out of range.
}

// Obtains enum-flags value from string name.
// Returns std::optional with enum-flags value.
template <typename E, typename BinaryPredicate>
[[nodiscard]] constexpr auto enum_cast(std::string_view value, BinaryPredicate p) noexcept(std::is_nothrow_invocable_r_v<bool, BinaryPredicate, char, char>) -> detail::enable_if_enum_flags_t<E, std::optional<std::decay_t<E>>> {
  static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>, "magic_enum::flags::enum_cast requires bool(char, char) invocable predicate.");
  // TODO: impl
  static_assert(sizeof(decltype(value)) + sizeof(decltype(p)) == sizeof(E) * 0, "not implemented");
  return {};
}

// Obtains enum-flags value from string name.
// Returns std::optional with enum-flags value.
template <typename E>
[[nodiscard]] constexpr auto enum_cast(std::string_view value) noexcept -> detail::enable_if_enum_flags_t<E, std::optional<std::decay_t<E>>> {
  // TODO: impl
  static_assert(sizeof(decltype(value)) == sizeof(E) * 0, "not implemented");
  return {};
}

// Obtains index in enum-flags values from enum-flags value.
// Returns std::optional with index.
template <typename E>
[[nodiscard]] constexpr auto enum_index(E value) noexcept -> detail::enable_if_enum_flags_t<E, std::optional<std::size_t>> {
  using D = std::decay_t<E>;

  if (detail::is_pow2<D>(value)) {
    for (std::size_t i = 0; i < detail::count_v<D, true>; ++i) {
      if (enum_value<D>(i) == value) {
        return i;
      }
    }
  }

  return std::nullopt; // Value out of range.
}

using magic_enum::enum_type_name; // TODO: impl
using magic_enum::enum_integer; // TODO: impl

namespace ostream_operators {

template <typename Char, typename Traits, typename E, detail::enable_if_enum_flags_t<E, int> = 0>
auto& operator<<(std::basic_ostream<Char, Traits>& os, E value) {
  using namespace magic_enum::flags;
  using D = std::decay_t<E>;

  if (const auto name = enum_name<D>(value); !name.empty()) {
    for (const auto c : name) {
      os.put(c);
    }
  } else {
    os << enum_integer<D>(value);
  }

  return os;
}

template <typename Char, typename Traits, typename E, detail::enable_if_enum_flags_t<E, int> = 0>
auto& operator<<(std::basic_ostream<Char, Traits>& os, std::optional<E> value) {
  if (value.has_value()) {
    os << value.value();
  }

  return os;
}

} // namespace magic_enum::flags::ostream_operators

namespace bitwise_operators {

using namespace magic_enum::bitwise_operators;

} // namespace magic_enum::flags::bitwise_operators

} // namespace magic_enum::flags

} // namespace magic_enum

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // NEARGYE_MAGIC_ENUM_HPP
