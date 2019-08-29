//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  vesion 0.6.0
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

#ifndef NEARGYE_MAGIC_ENUM_HPP
#define NEARGYE_MAGIC_ENUM_HPP

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
  static constexpr int min = MAGIC_ENUM_RANGE_MIN;
  static constexpr int max = MAGIC_ENUM_RANGE_MAX;
  static_assert(max > min, "magic_enum::enum_range requires max > min.");
};

static_assert(MAGIC_ENUM_RANGE_MIN <= 0,
              "MAGIC_ENUM_RANGE_MIN must be less or equals than 0.");
static_assert(MAGIC_ENUM_RANGE_MIN > (std::numeric_limits<int>::min)(),
              "MAGIC_ENUM_RANGE_MIN must be greater than INT_MIN.");

static_assert(MAGIC_ENUM_RANGE_MAX > 0,
              "MAGIC_ENUM_RANGE_MAX must be greater than 0.");
static_assert(MAGIC_ENUM_RANGE_MAX < (std::numeric_limits<int>::max)(),
              "MAGIC_ENUM_RANGE_MAX must be less than INT_MAX.");

static_assert(MAGIC_ENUM_RANGE_MAX > MAGIC_ENUM_RANGE_MIN,
              "MAGIC_ENUM_RANGE_MAX must be greater than MAGIC_ENUM_RANGE_MIN.");

namespace detail {

template <typename T>
struct magic_enum_supported final
#if defined(__clang__) || defined(__GNUC__) && __GNUC__>= 9 || defined(_MSC_VER) || defined(MAGIC_ENUM_NO_CHECK_SUPPORT)
    : std::true_type {};
#else
    : std::false_type {};
#endif

template <std::size_t N>
struct static_string final {
  constexpr static_string(std::string_view str) noexcept : static_string(str, std::make_index_sequence<N>{}) {}

  constexpr const char* data() const noexcept { return chars.data(); }

  constexpr std::size_t size() const noexcept { return chars.size(); }

  constexpr operator std::string_view() const noexcept { return {data(), size()}; }

 private:
  template <std::size_t... I>
  constexpr static_string(std::string_view str, std::index_sequence<I...>) noexcept : chars{{str[I]...}} {}

  const std::array<char, N> chars;
};

template <>
struct static_string<0> final {
  constexpr static_string(std::string_view) noexcept {}

  constexpr const char* data() const noexcept { return nullptr; }

  constexpr std::size_t size() const noexcept { return 0; }

  constexpr operator std::string_view() const noexcept { return {}; }
};

template <typename E>
inline constexpr int min_v = static_cast<int>(enum_range<E>::min > (std::numeric_limits<std::underlying_type_t<E>>::min)()
                                                  ? enum_range<E>::min
                                                  : (std::numeric_limits<std::underlying_type_t<E>>::min)());

template <typename E>
inline constexpr int max_v = static_cast<int>(enum_range<E>::max < (std::numeric_limits<std::underlying_type_t<E>>::max)()
                                                  ? enum_range<E>::max
                                                  : (std::numeric_limits<std::underlying_type_t<E>>::max)());

template <typename E>
[[nodiscard]] constexpr auto range() {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::range requires enum type.");
  static_assert(enum_range<E>::min > (std::numeric_limits<int>::min)(), "magic_enum::enum_range requires min must be greater than INT_MIN.");
  static_assert(enum_range<E>::max < (std::numeric_limits<int>::max)(), "magic_enum::enum_range requires max must be less than INT_MAX.");
  static_assert(enum_range<E>::max > enum_range<E>::min, "magic_enum::enum_range requires max > min.");

  return std::make_integer_sequence<int, max_v<E> - min_v<E> + 1>{};
}

template <typename E>
inline constexpr auto range_v = range<E>();

[[nodiscard]] constexpr std::string_view pretty_name(std::string_view name) noexcept {
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

template <typename E, E V>
[[nodiscard]] constexpr auto n() noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::n requires enum type.");
#if defined(__clang__) || defined(__GNUC__) && __GNUC__ >= 9
  constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER)
  constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#else
  return std::string_view{}; // Unsupported compiler.
#endif

#if defined(__clang__) || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER)
  return static_string<name.size()>{name};
#endif
}

template <typename E, E V>
inline constexpr auto name_v = n<E, V>();

template <typename E, int... I>
[[nodiscard]] constexpr auto strings(std::integer_sequence<int, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::strings requires enum type.");

  return std::array<std::string_view, sizeof...(I)>{{name_v<E, static_cast<E>(I + min_v<E>)>...}};
}

template <typename E>
[[nodiscard]] constexpr std::string_view name(E value) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::name requires enum type.");
  using U = std::underlying_type_t<E>;
  constexpr auto names = strings<E>(range_v<E>);

  if (static_cast<U>(value) > static_cast<U>(max_v<E>) || static_cast<U>(value) < static_cast<U>(min_v<E>)) {
    return {}; // Value out of range.
  }

  if (auto i = static_cast<std::size_t>(static_cast<U>(value) - min_v<E>); i < names.size()) {
    return names[i];
  }

  return {}; // Value out of range.
}

template <typename E, int... I>
[[nodiscard]] constexpr auto values(std::integer_sequence<int, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::values requires enum type.");
  constexpr std::array<bool, sizeof...(I)> valid{{(name_v<E, static_cast<E>(I + min_v<E>)>.size() != 0)...}};
  constexpr auto num_valid = ((valid[I] ? 1 : 0) + ...);

  std::array<E, num_valid> values{};
  for (std::size_t i = 0, v = 0; i < valid.size() && v < num_valid; ++i) {
    if (valid[i]) {
      values[v++] = static_cast<E>(static_cast<int>(i) + min_v<E>);
    }
  }

  return values;
}

template <typename E>
inline constexpr auto values_v = values<E>(range_v<E>);

template <typename E>
inline constexpr std::size_t count_v = values_v<E>.size();

template <typename E, std::size_t... I>
[[nodiscard]] constexpr auto names(std::integer_sequence<std::size_t, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::names requires enum type.");

  return std::array<std::string_view, sizeof...(I)>{{name_v<E, values_v<E>[I]>...}};
}

template <typename E>
inline constexpr auto names_v = names<E>(std::make_index_sequence<count_v<E>>{});

template <typename E, std::size_t... I>
[[nodiscard]] constexpr auto entries(std::integer_sequence<std::size_t, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::detail::entries requires enum type.");

  return std::array<std::pair<E, std::string_view>, sizeof...(I)>{{{values_v<E>[I], names_v<E>[I]}...}};
}

template <typename E>
inline constexpr auto entries_v = entries<E>(std::make_index_sequence<count_v<E>>{});

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename T>
using enable_if_enum_t = std::enable_if_t<std::is_enum_v<remove_cvref_t<T>>, remove_cvref_t<T>>;

template <typename E, typename D>
inline constexpr bool check_enum_v = std::is_same_v<remove_cvref_t<E>, D> && std::is_enum_v<D>;

template <typename T, bool = std::is_enum_v<T>>
struct is_scoped_enum : std::false_type {};

template <typename T>
struct is_scoped_enum<T, true> : std::bool_constant<!std::is_convertible_v<T, std::underlying_type_t<T>>> {};

template <typename T, bool = std::is_enum_v<T>>
struct is_unscoped_enum : std::false_type {};

template <typename T>
struct is_unscoped_enum<T, true> : std::bool_constant<std::is_convertible_v<T, std::underlying_type_t<T>>> {};

template <typename T, typename = T>
struct is_fixed_enum : std::false_type {};

template <typename T>
struct is_fixed_enum<T, decltype(T{0})> : std::is_enum<T> {};

template <typename T, bool = std::is_enum_v<T>>
struct underlying_type {};

template <typename T>
struct underlying_type<T, true> : std::underlying_type<T> {};

} // namespace magic_enum::detail

// Checks is magic_enum supported compiler.
inline constexpr bool is_magic_enum_supported = detail::magic_enum_supported<void>::value;

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

// Checks whether T is an Fixed enumeration type.
// Provides the member constant value which is equal to true, if T is an [Fixed enumeration](https://en.cppreference.com/w/cpp/language/enum) type. Otherwise, value is equal to false.
template <typename T>
struct is_fixed_enum : detail::is_fixed_enum<T> {};

template <typename T>
inline constexpr bool is_fixed_enum_v = is_fixed_enum<T>::value;

// If T is a complete enumeration type, provides a member typedef type that names the underlying type of T.
// Otherwise, if T is not an enumeration type, there is no member type. Otherwise (T is an incomplete enumeration type), the program is ill-formed.
template <typename T>
struct underlying_type : detail::underlying_type<T> {};

template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

// Obtains enum value from enum string name.
// Returns std::optional with enum value.
template <typename E, typename D = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr std::optional<D> enum_cast(std::string_view value) noexcept {
  static_assert(detail::magic_enum_supported<D>::value, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::check_enum_v<E, D>, "magic_enum::enum_cast requires enum type.");
  constexpr auto values = detail::values_v<D>;
  constexpr auto count = detail::count_v<D>;
  constexpr auto names = detail::names_v<D>;

  for (std::size_t i = 0; i < count; ++i) {
    if (names[i] == value) {
      return values[i];
    }
  }

  return std::nullopt; // Invalid value or out of range.
}

// Obtains enum value from integer value.
// Returns std::optional with enum value.
template <typename E, typename D = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr std::optional<D> enum_cast(std::underlying_type_t<D> value) noexcept {
  static_assert(detail::magic_enum_supported<D>::value, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::check_enum_v<E, D>, "magic_enum::enum_cast requires enum type.");

  if (detail::name<D>(static_cast<D>(value)).empty()) {
    return std::nullopt; // Invalid value or out of range.
  }

  return static_cast<D>(value);
}

// Returns integer value from enum value.
template <typename E, typename D = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr std::underlying_type_t<D> enum_integer(E value) noexcept {
  static_assert(detail::magic_enum_supported<D>::value, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::check_enum_v<E, D>, "magic_enum::enum_integer requires enum type.");

  return static_cast<std::underlying_type_t<D>>(value);
}

// Returns enum value at specified index.
// No bounds checking is performed: the behavior is undefined if index >= number of enum values.
template<typename E, typename D = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr D enum_value(std::size_t index) {
  static_assert(detail::magic_enum_supported<D>::value, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::check_enum_v<E, D>, "magic_enum::enum_value requires enum type.");
  constexpr auto values = detail::values_v<D>;

  return assert(index < values.size()), values[index];
}

// Obtains value enum sequence.
// Returns std::array with enum values, sorted by enum value.
template <typename E, typename D = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr auto enum_values() noexcept {
  static_assert(detail::magic_enum_supported<D>::value, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::check_enum_v<E, D>, "magic_enum::enum_values requires enum type.");
  constexpr auto values = detail::values_v<D>;

  return values;
}

// Returns number of enum values.
template <typename E, typename D = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr std::size_t enum_count() noexcept {
  static_assert(detail::magic_enum_supported<D>::value, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::check_enum_v<E, D>, "magic_enum::enum_count requires enum type.");
  constexpr auto count = detail::count_v<D>;

  return count;
}

// Returns string enum name from static storage enum variable.
// This version is much lighter on the compile times and is not restricted to the enum_range limitation.
template <auto V, typename D = detail::enable_if_enum_t<decltype(V)>>
[[nodiscard]] constexpr std::string_view enum_name() noexcept {
  static_assert(detail::magic_enum_supported<D>::value, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::check_enum_v<decltype(V), D>, "magic_enum::enum_name requires enum type.");

  return detail::name_v<D, V>;
}

// Returns string enum name from enum value.
template <typename E, typename D = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr std::string_view enum_name(E value) noexcept {
  static_assert(detail::magic_enum_supported<D>::value, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::check_enum_v<E, D>, "magic_enum::enum_name requires enum type.");

  return detail::name<D>(value);
}

// Obtains string enum name sequence.
// Returns std::array with string enum names, sorted by enum value.
template <typename E, typename D = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr auto enum_names() noexcept {
  static_assert(detail::magic_enum_supported<D>::value, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::check_enum_v<E, D>, "magic_enum::enum_names requires enum type.");
  constexpr auto names = detail::names_v<D>;

  return names;
}

// Obtains pair (value enum, string enum name) sequence.
// Returns std::array with std::pair (value enum, string enum name), sorted by enum value.
template <typename E, typename D = detail::enable_if_enum_t<E>>
[[nodiscard]] constexpr auto enum_entries() noexcept {
  static_assert(detail::magic_enum_supported<D>::value, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::check_enum_v<E, D>, "magic_enum::enum_entries requires enum type.");
  constexpr auto entries = detail::entries_v<D>;

  return entries;
}

namespace ostream_operators {

template <class Char, class Traits, typename E, typename D = detail::enable_if_enum_t<E>>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, E value) {
  static_assert(detail::magic_enum_supported<D>::value, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::check_enum_v<E, D>, "magic_enum::ostream_operators::operator<< requires enum type.");

  if (auto name = detail::name<D>(value); !name.empty()) {
    for (auto c : name) {
      os.put(c);
    }
  } else {
    os << static_cast<std::underlying_type_t<D>>(value);
  }

  return os;
}

template <class Char, class Traits, typename E, typename D = detail::enable_if_enum_t<E>>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, std::optional<E> value) {
  static_assert(detail::magic_enum_supported<D>::value, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  static_assert(detail::check_enum_v<E, D>, "magic_enum::ostream_operators::operator<< requires enum type.");

  if (value.has_value()) {
    os << value.value();
  }

  return os;
}

} // namespace magic_enum::ostream_operators

namespace bitwise_operators {

template <typename E, typename D = detail::enable_if_enum_t<E>>
constexpr E operator~(E rhs) {
  static_assert(detail::check_enum_v<E, D>, "magic_enum::bitwise_operators::operator~ requires enum type.");
  using U = std::underlying_type_t<D>;

  return static_cast<E>(~static_cast<U>(rhs));
}

template <typename E, typename D = detail::enable_if_enum_t<E>>
constexpr E operator|(E lhs, E rhs) {
  static_assert(detail::check_enum_v<E, D>, "magic_enum::bitwise_operators::operator| requires enum type.");
  using U = std::underlying_type_t<D>;

  return static_cast<E>(static_cast<U>(lhs) | static_cast<U>(rhs));
}

template <typename E, typename D = detail::enable_if_enum_t<E>>
constexpr E operator&(E lhs, E rhs) {
  static_assert(detail::check_enum_v<E, D>, "magic_enum::bitwise_operators::operator& requires enum type.");
  using U = std::underlying_type_t<D>;

  return static_cast<E>(static_cast<U>(lhs) & static_cast<U>(rhs));
}

template <typename E, typename D = detail::enable_if_enum_t<E>>
constexpr E operator^(E lhs, E rhs) {
  static_assert(detail::check_enum_v<E, D>, "magic_enum::bitwise_operators::operator^ requires enum type.");
  using U = std::underlying_type_t<D>;

  return static_cast<E>(static_cast<U>(lhs) ^ static_cast<U>(rhs));
}

template <typename E, typename D = detail::enable_if_enum_t<E>>
constexpr E& operator|=(E& lhs, E rhs) {
  static_assert(detail::check_enum_v<E, D>, "magic_enum::bitwise_operators::operator|= requires enum type.");

  return lhs = lhs | rhs;
}

template <typename E, typename D = detail::enable_if_enum_t<E>>
constexpr E& operator&=(E& lhs, E rhs) {
  static_assert(detail::check_enum_v<E, D>, "magic_enum::bitwise_operators::operator%= requires enum type.");

  return lhs = lhs & rhs;
}

template <typename E, typename D = detail::enable_if_enum_t<E>>
constexpr E& operator^=(E& lhs, E rhs) {
  static_assert(detail::check_enum_v<E, D>, "magic_enum::bitwise_operators::operator^= requires enum type.");

  return lhs = lhs ^ rhs;
}

} // namespace magic_enum::bitwise_operators

} // namespace magic_enum

#endif // NEARGYE_MAGIC_ENUM_HPP
