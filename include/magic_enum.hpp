//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.7.3
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2021 Daniil Goncharov <neargye@gmail.com>.
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
#define MAGIC_ENUM_VERSION_MINOR 7
#define MAGIC_ENUM_VERSION_PATCH 3

#include <array>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <limits>
#include <type_traits>
#include <utility>

#if defined(MAGIC_ENUM_CONFIG_FILE)
#include MAGIC_ENUM_CONFIG_FILE
#endif

#if !defined(MAGIC_ENUM_USING_ALIAS_OPTIONAL)
#include <optional>
#endif
#if !defined(MAGIC_ENUM_USING_ALIAS_STRING)
#include <string>
#endif
#if !defined(MAGIC_ENUM_USING_ALIAS_STRING_VIEW)
#include <string_view>
#endif

#if defined(__clang__)
#  pragma clang diagnostic push
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wmaybe-uninitialized" // May be used uninitialized 'return {};'.
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 26495) // Variable 'static_string<N>::chars_' is uninitialized.
#  pragma warning(disable : 28020) // Arithmetic overflow: Using operator '-' on a 4 byte value and then casting the result to a 8 byte value.
#  pragma warning(disable : 26451) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call.
#endif

// Checks magic_enum compiler compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1910
#  undef  MAGIC_ENUM_SUPPORTED
#  define MAGIC_ENUM_SUPPORTED 1
#endif

// Checks magic_enum compiler aliases compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1920
#  undef  MAGIC_ENUM_SUPPORTED_ALIASES
#  define MAGIC_ENUM_SUPPORTED_ALIASES 1
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

// If need another optional type, define the macro MAGIC_ENUM_USING_ALIAS_OPTIONAL.
#if defined(MAGIC_ENUM_USING_ALIAS_OPTIONAL)
MAGIC_ENUM_USING_ALIAS_OPTIONAL
#else
using std::optional;
#endif

// If need another string_view type, define the macro MAGIC_ENUM_USING_ALIAS_STRING_VIEW.
#if defined(MAGIC_ENUM_USING_ALIAS_STRING_VIEW)
MAGIC_ENUM_USING_ALIAS_STRING_VIEW
#else
using std::string_view;
#endif

// If need another string type, define the macro MAGIC_ENUM_USING_ALIAS_STRING.
#if defined(MAGIC_ENUM_USING_ALIAS_STRING)
MAGIC_ENUM_USING_ALIAS_STRING
#else
using std::string;
#endif

namespace customize {

// Enum value must be in range [MAGIC_ENUM_RANGE_MIN, MAGIC_ENUM_RANGE_MAX]. By default MAGIC_ENUM_RANGE_MIN = -128, MAGIC_ENUM_RANGE_MAX = 128.
// If need another range for all enum types by default, redefine the macro MAGIC_ENUM_RANGE_MIN and MAGIC_ENUM_RANGE_MAX.
// If need another range for specific enum type, add specialization enum_range for necessary enum type.
template <typename E>
struct enum_range {
  static_assert(std::is_enum_v<E>, "magic_enum::customize::enum_range requires enum type.");
  static constexpr int min = MAGIC_ENUM_RANGE_MIN;
  static constexpr int max = MAGIC_ENUM_RANGE_MAX;
  static_assert(max > min, "magic_enum::customize::enum_range requires max > min.");
};

static_assert(MAGIC_ENUM_RANGE_MAX > MAGIC_ENUM_RANGE_MIN, "MAGIC_ENUM_RANGE_MAX must be greater than MAGIC_ENUM_RANGE_MIN.");
static_assert((MAGIC_ENUM_RANGE_MAX - MAGIC_ENUM_RANGE_MIN) < (std::numeric_limits<std::uint16_t>::max)(), "MAGIC_ENUM_RANGE must be less than UINT16_MAX.");

// If need custom names for enum, add specialization enum_name for necessary enum type.
template <typename E>
constexpr string_view enum_name(E) noexcept {
  static_assert(std::is_enum_v<E>, "magic_enum::customize::enum_name requires enum type.");

  return {};
}

} // namespace magic_enum::customize

namespace detail {

template <typename T>
struct supported
#if defined(MAGIC_ENUM_SUPPORTED) && MAGIC_ENUM_SUPPORTED || defined(MAGIC_ENUM_NO_CHECK_SUPPORT)
    : std::true_type {};
#else
    : std::false_type {};
#endif

template <typename T, typename = void>
struct has_is_flags : std::false_type {};

template <typename T>
struct has_is_flags<T, std::void_t<decltype(customize::enum_range<T>::is_flags)>> : std::bool_constant<std::is_same_v<bool, std::decay_t<decltype(customize::enum_range<T>::is_flags)>>> {};

template <typename T, typename = void>
struct range_min : std::integral_constant<int, MAGIC_ENUM_RANGE_MIN> {};

template <typename T>
struct range_min<T, std::void_t<decltype(customize::enum_range<T>::min)>> : std::integral_constant<decltype(customize::enum_range<T>::min), customize::enum_range<T>::min> {};

template <typename T, typename = void>
struct range_max : std::integral_constant<int, MAGIC_ENUM_RANGE_MAX> {};

template <typename T>
struct range_max<T, std::void_t<decltype(customize::enum_range<T>::max)>> : std::integral_constant<decltype(customize::enum_range<T>::max), customize::enum_range<T>::max> {};

template <std::size_t N>
class static_string {
 public:
  constexpr explicit static_string(string_view str) noexcept : static_string{str, std::make_index_sequence<N>{}} {
    assert(str.size() == N);
  }

  constexpr const char* data() const noexcept { return chars_; }

  constexpr std::size_t size() const noexcept { return N; }

  constexpr operator string_view() const noexcept { return {data(), size()}; }

 private:
  template <std::size_t... I>
  constexpr static_string(string_view str, std::index_sequence<I...>) noexcept : chars_{str[I]..., '\0'} {}

  char chars_[N + 1];
};

template <>
class static_string<0> {
 public:
  constexpr explicit static_string(string_view) noexcept {}

  constexpr const char* data() const noexcept { return nullptr; }

  constexpr std::size_t size() const noexcept { return 0; }

  constexpr operator string_view() const noexcept { return {}; }
};

constexpr string_view pretty_name(string_view name) noexcept {
  for (std::size_t i = name.size(); i > 0; --i) {
    if (!((name[i - 1] >= '0' && name[i - 1] <= '9') ||
          (name[i - 1] >= 'a' && name[i - 1] <= 'z') ||
          (name[i - 1] >= 'A' && name[i - 1] <= 'Z') ||
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
          (name[i - 1] & 0x80) ||
#endif
          (name[i - 1] == '_'))) {
      name.remove_prefix(i);
      break;
    }
  }

  if (name.size() > 0 && ((name.front() >= 'a' && name.front() <= 'z') ||
                          (name.front() >= 'A' && name.front() <= 'Z') ||
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
                          (name.front() & 0x80) ||
#endif
                          (name.front() == '_'))) {
    return name;
  }

  return {}; // Invalid name.
}

template<typename CharType>
constexpr auto to_lower([[maybe_unused]] CharType ch) noexcept -> std::enable_if_t<std::is_same_v<std::decay_t<CharType>, char>, char> {
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  static_assert(!std::is_same_v<CharType, CharType>, "magic_enum::detail::to_lower not supported Non-ASCII feature.");
  return {};
#else
  return 'A' <= ch && ch <= 'Z' ? ch - 'A' + 'a' : ch;
#endif
}

constexpr std::size_t find(string_view str, char c) noexcept {
#if defined(__clang__) && __clang_major__ < 9 && defined(__GLIBCXX__) || defined(_MSC_VER) && _MSC_VER < 1920 && !defined(__clang__)
// https://stackoverflow.com/questions/56484834/constexpr-stdstring-viewfind-last-of-doesnt-work-on-clang-8-with-libstdc
// https://developercommunity.visualstudio.com/content/problem/360432/vs20178-regression-c-failed-in-test.html
  constexpr bool workaround = true;
#else
  constexpr bool workaround = false;
#endif

  if constexpr (workaround) {
    for (std::size_t i = 0; i < str.size(); ++i) {
      if (str[i] == c) {
        return i;
      }
    }

    return string_view::npos;
  } else {
    return str.find_first_of(c);
  }
}

template <typename T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N], std::index_sequence<I...>) {
  return {{a[I]...}};
}

template <typename BinaryPredicate>
constexpr bool cmp_equal(string_view lhs, string_view rhs, [[maybe_unused]] BinaryPredicate&& p) noexcept(std::is_nothrow_invocable_r_v<bool, BinaryPredicate, char, char>) {
#if defined(_MSC_VER) && _MSC_VER < 1920 && !defined(__clang__)
  // https://developercommunity.visualstudio.com/content/problem/360432/vs20178-regression-c-failed-in-test.html
  // https://developercommunity.visualstudio.com/content/problem/232218/c-constexpr-string-view.html
  constexpr bool workaround = true;
#else
  constexpr bool workaround = false;
#endif
  constexpr bool custom_predicate =
    !std::is_same_v<std::decay_t<BinaryPredicate>, std::equal_to<char>> &&
    !std::is_same_v<std::decay_t<BinaryPredicate>, std::equal_to<>>;

  if constexpr (custom_predicate || workaround) {
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
  } else {
    return lhs == rhs;
  }
}

template <typename L, typename R>
constexpr bool cmp_less(L lhs, R rhs) noexcept {
  static_assert(std::is_integral_v<L> && std::is_integral_v<R>, "magic_enum::detail::cmp_less requires integral type.");

  if constexpr (std::is_signed_v<L> == std::is_signed_v<R>) {
    // If same signedness (both signed or both unsigned).
    return lhs < rhs;
  } else if constexpr (std::is_same_v<L, bool>) { // bool special case
      return static_cast<R>(lhs) < rhs;
  } else if constexpr (std::is_same_v<R, bool>) { // bool special case
      return lhs < static_cast<L>(rhs);
  } else if constexpr (std::is_signed_v<R>) {
    // If 'right' is negative, then result is 'false', otherwise cast & compare.
    return rhs > 0 && lhs < static_cast<std::make_unsigned_t<R>>(rhs);
  } else {
    // If 'left' is negative, then result is 'true', otherwise cast & compare.
    return lhs < 0 || static_cast<std::make_unsigned_t<L>>(lhs) < rhs;
  }
}

constexpr std::size_t hash_string(string_view to_hash) {
  auto result = static_cast<std::size_t>(0xcbf29ce484222325); // FNV offset basis

  for (char c : to_hash) {
    result ^= c;
    result *= static_cast<std::size_t>(1099511628211); // FNV prime
  }

  return result;
}

template <typename I>
constexpr I log2(I value) noexcept {
  static_assert(std::is_integral_v<I>, "magic_enum::detail::log2 requires integral type.");

  auto ret = I{0};
  for (; value > I{1}; value >>= I{1}, ++ret) {}

  return ret;
}

template <typename T>
inline constexpr bool is_enum_v = std::is_enum_v<T> && std::is_same_v<T, std::decay_t<T>>;

template <typename E>
constexpr auto n() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::n requires enum type.");

  if constexpr (supported<E>::value) {
#if defined(__clang__) || defined(__GNUC__)
    constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER)
    constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#else
    constexpr auto name = string_view{};
#endif
    return static_string<name.size()>{name};
  } else {
    return string_view{}; // Unsupported compiler.
  }
}

template <typename E>
inline constexpr auto type_name_v = n<E>();

template <typename E, E V>
constexpr auto n() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::n requires enum type.");
  [[maybe_unused]] constexpr auto custom_name = customize::enum_name<E>(V);

  if constexpr (custom_name.empty()) {
    if constexpr (supported<E>::value) {
#if defined(__clang__) || defined(__GNUC__)
      constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER)
      constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#else
      constexpr auto name = string_view{};
#endif
      return static_string<name.size()>{name};
    } else {
      return string_view{}; // Unsupported compiler.
    }
  } else {
    return static_string<custom_name.size()>{custom_name};
  }
}

template <typename E, E V>
inline constexpr auto enum_name_v = n<E, V>();

template <typename E, auto V>
constexpr bool is_valid() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::is_valid requires enum type.");

  return n<E, static_cast<E>(V)>().size() != 0;
}

template <typename E, int O, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr E value(std::size_t i) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::value requires enum type.");

  if constexpr (std::is_same_v<U, bool>) { // bool special case
    static_assert(O == 0, "magic_enum::detail::value requires valid offset.");

    return static_cast<E>(i);
  } else if constexpr (IsFlags) {
    return static_cast<E>(U{1} << static_cast<U>(static_cast<int>(i) + O));
  } else {
    return static_cast<E>(static_cast<int>(i) + O);
  }
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr int reflected_min() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::reflected_min requires enum type.");

  if constexpr (IsFlags) {
    return 0;
  } else {
    constexpr auto lhs = range_min<E>::value;
    constexpr auto rhs = (std::numeric_limits<U>::min)();

    if constexpr (cmp_less(rhs, lhs)) {
      return lhs;
    } else {
      return rhs;
    }
  }
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr int reflected_max() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::reflected_max requires enum type.");

  if constexpr (IsFlags) {
    return std::numeric_limits<U>::digits - 1;
  } else {
    constexpr auto lhs = range_max<E>::value;
    constexpr auto rhs = (std::numeric_limits<U>::max)();

    if constexpr (cmp_less(lhs, rhs)) {
      return lhs;
    } else {
      return rhs;
    }
  }
}

template <typename E, bool IsFlags>
inline constexpr auto reflected_min_v = reflected_min<E, IsFlags>();

template <typename E, bool IsFlags>
inline constexpr auto reflected_max_v = reflected_max<E, IsFlags>();

template <std::size_t N>
constexpr std::size_t values_count(const bool (&valid)[N]) noexcept {
  auto count = std::size_t{0};
  for (std::size_t i = 0; i < N; ++i) {
    if (valid[i]) {
      ++count;
    }
  }

  return count;
}

template <typename E, bool IsFlags, int Min, std::size_t... I>
constexpr auto values(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::values requires enum type.");
  constexpr bool valid[sizeof...(I)] = {is_valid<E, value<E, Min, IsFlags>(I)>()...};
  constexpr std::size_t count = values_count(valid);

  if constexpr (count > 0) {
    E values[count] = {};
    for (std::size_t i = 0, v = 0; v < count; ++i) {
      if (valid[i]) {
        values[v++] = value<E, Min, IsFlags>(i);
      }
    }

    return to_array(values, std::make_index_sequence<count>{});
  } else {
    return std::array<E, 0>{};
  }
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr auto values() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::values requires enum type.");
  constexpr auto min = reflected_min_v<E, IsFlags>;
  constexpr auto max = reflected_max_v<E, IsFlags>;
  constexpr auto range_size = max - min + 1;
  static_assert(range_size > 0, "magic_enum::enum_range requires valid size.");
  static_assert(range_size < (std::numeric_limits<std::uint16_t>::max)(), "magic_enum::enum_range requires valid size.");

  return values<E, IsFlags, reflected_min_v<E, IsFlags>>(std::make_index_sequence<range_size>{});
}

template <typename E, typename U = std::underlying_type_t<E>>
constexpr bool is_flags_enum() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::is_flags_enum requires enum type.");

  if constexpr (has_is_flags<E>::value) {
    return customize::enum_range<E>::is_flags;
  } else if constexpr (std::is_same_v<U, bool>) { // bool special case
    return false;
  } else {
#if defined(MAGIC_ENUM_NO_CHECK_FLAGS)
    return false;
#else
    constexpr auto flags_values = values<E, true>();
    constexpr auto default_values = values<E, false>();
    if (flags_values.size() == 0 || default_values.size() > flags_values.size()) {
      return false;
    }
    for (std::size_t i = 0; i < default_values.size(); ++i) {
      const auto v = static_cast<U>(default_values[i]);
      if (v != 0 && (v & (v - 1)) != 0) {
        return false;
      }
    }
    return flags_values.size() > 0;
#endif
  }
}

template <typename E>
inline constexpr bool is_flags_v = is_flags_enum<E>();

template <typename E>
inline constexpr std::array values_v = values<E, is_flags_v<E>>();

template <typename E, typename D = std::decay_t<E>>
using values_t = decltype((values_v<D>));

template <typename E>
inline constexpr auto count_v = values_v<E>.size();

template <typename E, typename U = std::underlying_type_t<E>>
inline constexpr auto min_v = (count_v<E> > 0) ? static_cast<U>(values_v<E>.front()) : U{0};

template <typename E, typename U = std::underlying_type_t<E>>
inline constexpr auto max_v = (count_v<E> > 0) ? static_cast<U>(values_v<E>.back()) : U{0};

template <typename E, std::size_t... I>
constexpr auto names(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::names requires enum type.");

  return std::array<string_view, sizeof...(I)>{{enum_name_v<E, values_v<E>[I]>...}};
}

template <typename E>
inline constexpr std::array names_v = names<E>(std::make_index_sequence<count_v<E>>{});

template <typename E, typename D = std::decay_t<E>>
using names_t = decltype((names_v<D>));

template <typename E, std::size_t... I>
constexpr auto name_hashes(std::index_sequence<I...>) noexcept {
    static_assert(is_enum_v<E>, "magic_enum::detail::names requires enum type.");

    return std::array<std::size_t, sizeof...(I)>{{ hash_string(enum_name_v<E, values_v<E>[I]>)... }};
}

template <typename E>
inline constexpr std::array name_hashes_v = name_hashes<E>(std::make_index_sequence<count_v<E>>{});

template <typename E, typename D = std::decay_t<E>>
using name_hashes_t = decltype((name_hashes_v<D>));

template <typename E, std::size_t... I>
constexpr auto entries(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::entries requires enum type.");

  return std::array<std::pair<E, string_view>, sizeof...(I)>{{{values_v<E>[I], enum_name_v<E, values_v<E>[I]>}...}};
}

template <typename E>
inline constexpr std::array entries_v = entries<E>(std::make_index_sequence<count_v<E>>{});

template <typename E, typename D = std::decay_t<E>>
using entries_t = decltype((entries_v<D>));

template <typename E, typename U = std::underlying_type_t<E>>
constexpr bool is_sparse() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::is_sparse requires enum type.");
  constexpr auto max = is_flags_v<E> ? log2(max_v<E>) : max_v<E>;
  constexpr auto min = is_flags_v<E> ? log2(min_v<E>) : min_v<E>;
  constexpr auto range_size = max - min + 1;

  return range_size != count_v<E>;
}

template <typename E>
inline constexpr bool is_sparse_v = is_sparse<E>();

template <typename E, typename U = std::underlying_type_t<E>>
constexpr U values_ors() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::values_ors requires enum type.");

  auto ors = U{0};
  for (std::size_t i = 0; i < count_v<E>; ++i) {
    ors |= static_cast<U>(values_v<E>[i]);
  }

  return ors;
}

template <bool, typename T, typename R>
struct enable_if_enum {};

template <typename T, typename R>
struct enable_if_enum<true, T, R> {
  using type = R;
  using D = std::decay_t<T>;
  static_assert(supported<D>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
};

template <typename T, typename R = void>
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

template <typename EnumType>
using switch_type_t = std::conditional_t<std::is_same_v<bool, typename underlying_type<EnumType>::type>, std::uint8_t, typename underlying_type<EnumType>::type>;

template<typename ResultType, typename EnumType, std::size_t size>
constexpr auto calculate_cases(std::size_t page, const std::array<EnumType, size>& values) {
  const std::size_t values_to = (std::min)(static_cast<std::size_t>(256), size - page);

  std::array<ResultType, 256> result{};
  for (std::size_t i = 0; i < values_to; ++i) {
    result[i] = static_cast<ResultType>(values[i + page]);
  }

  // dead cases, try to avoid case collisions
  ResultType last_value = result[values_to-1];
  bool reached_max = false;
  auto it = result.begin();
  for (std::size_t i = values_to; i < 256; ++i) {
    if (last_value == (std::numeric_limits<ResultType>::max)()) {
      last_value = (std::numeric_limits<ResultType>::min)();
      reached_max = true;
    } else if (!reached_max) {
      result[i] = ++last_value;
      continue;
    } else {
      ++last_value;
    }
    // find next suitable value
    while (last_value == *it) {
      ++last_value;
      ++it;
    }
    result[i] = last_value;
  }
  return result;
}

template< class R, class F, class... Args >
constexpr R invoke_r( F&& f, Args&&... args ) noexcept(std::is_nothrow_invocable_r_v<R, F, Args...>) {
  if constexpr (std::is_void_v<R>) {
    std::forward<F>(f)(std::forward<Args>(args)...);
  } else {
    return static_cast<R>(std::forward<F>(f)(std::forward<Args>(args)...));
  }
}

enum class try_index_invoke_t : bool {
    no = false, yes = true
};

template<typename DefaultResultType = void>
constexpr auto default_result_type_lambda = [] { return DefaultResultType{}; };
template<>
constexpr auto default_result_type_lambda<void> = [] {};

#define MAGIC_ENUM_FOR_EACH_256(T) T(0)T(1)T(2)T(3)T(4)T(5)T(6)T(7)T(8)T(9)T(10)T(11)T(12)T(13)T(14)T(15)T(16)T(17)T(18)T(19)T(20)T(21)T(22)T(23)T(24)T(25)T(26)T(27)T(28)T(29)T(30)T(31)          \
  T(32)T(33)T(34)T(35)T(36)T(37)T(38)T(39)T(40)T(41)T(42)T(43)T(44)T(45)T(46)T(47)T(48)T(49)T(50)T(51)T(52)T(53)T(54)T(55)T(56)T(57)T(58)T(59)T(60)T(61)T(62)T(63)                                 \
  T(64)T(65)T(66)T(67)T(68)T(69)T(70)T(71)T(72)T(73)T(74)T(75)T(76)T(77)T(78)T(79)T(80)T(81)T(82)T(83)T(84)T(85)T(86)T(87)T(88)T(89)T(90)T(91)T(92)T(93)T(94)T(95)                                 \
  T(96)T(97)T(98)T(99)T(100)T(101)T(102)T(103)T(104)T(105)T(106)T(107)T(108)T(109)T(110)T(111)T(112)T(113)T(114)T(115)T(116)T(117)T(118)T(119)T(120)T(121)T(122)T(123)T(124)T(125)T(126)T(127)     \
  T(128)T(129)T(130)T(131)T(132)T(133)T(134)T(135)T(136)T(137)T(138)T(139)T(140)T(141)T(142)T(143)T(144)T(145)T(146)T(147)T(148)T(149)T(150)T(151)T(152)T(153)T(154)T(155)T(156)T(157)T(158)T(159) \
  T(160)T(161)T(162)T(163)T(164)T(165)T(166)T(167)T(168)T(169)T(170)T(171)T(172)T(173)T(174)T(175)T(176)T(177)T(178)T(179)T(180)T(181)T(182)T(183)T(184)T(185)T(186)T(187)T(188)T(189)T(190)T(191) \
  T(192)T(193)T(194)T(195)T(196)T(197)T(198)T(199)T(200)T(201)T(202)T(203)T(204)T(205)T(206)T(207)T(208)T(209)T(210)T(211)T(212)T(213)T(214)T(215)T(216)T(217)T(218)T(219)T(220)T(221)T(222)T(223) \
  T(224)T(225)T(226)T(227)T(228)T(229)T(230)T(231)T(232)T(233)T(234)T(235)T(236)T(237)T(238)T(239)T(240)T(241)T(242)T(243)T(244)T(245)T(246)T(247)T(248)T(249)T(250)T(251)T(252)T(253)T(254)T(255)

#define MAGIC_ENUM_CASE(val)                                                                                            \
  case cases[val]:                                                                                                      \
    if constexpr ((val) + page < size) {                                                                                \
      if constexpr (try_index_invoke == try_index_invoke_t::yes &&                                                      \
          std::is_invocable_r_v<result_t, Lambda, std::integral_constant<std::size_t, (val) + page>>)                   \
        return invoke_r<result_t>(std::forward<Lambda>(lambda), std::integral_constant<std::size_t, (val) + page>{});   \
      else if constexpr (std::is_invocable_r_v<result_t, Lambda,                                                        \
          std::integral_constant<std::decay_t<decltype(values[(val) + page])>, values[(val) + page]>>)                  \
        return invoke_r<result_t>(std::forward<Lambda>(lambda),                                                         \
          std::integral_constant<std::decay_t<decltype(values[(val) + page])>, values[(val) + page]>{});                \
      break;                                                                                                            \
    } else {                                                                                                            \
      [[fallthrough]];                                                                                                  \
    }

template<try_index_invoke_t try_index_invoke = try_index_invoke_t::no, std::size_t page = 0, typename Lambda, typename EnumType, typename ResultGetterType = decltype(default_result_type_lambda<>)>
static constexpr auto constexpr_switch(Lambda&& lambda, EnumType searched, ResultGetterType&& def = default_result_type_lambda<>)
  -> std::invoke_result_t<ResultGetterType> {
  using switch_t = switch_type_t<EnumType>;
  using result_t = std::invoke_result_t<ResultGetterType>;
  constexpr std::array values = values_v<EnumType>;
  constexpr std::size_t size = values.size();
  constexpr std::array cases = calculate_cases<switch_t>(page, values);

  switch (static_cast<switch_t>(searched)) {
    MAGIC_ENUM_FOR_EACH_256(MAGIC_ENUM_CASE)
  default:
    if constexpr (size > 256 + page) {
      return constexpr_switch<try_index_invoke, page + 256>(std::forward<Lambda>(lambda), searched, std::forward<ResultGetterType>(def));
    }
  }
  return def();
}

template<typename EnumType, std::size_t page = 0,
        typename Lambda, typename ResultGetterType = decltype(default_result_type_lambda<>)>
static constexpr auto constexpr_switch(Lambda&& lambda, string_view searched, ResultGetterType&& def = default_result_type_lambda<>)
  -> std::invoke_result_t<ResultGetterType> {
  constexpr try_index_invoke_t try_index_invoke = try_index_invoke_t::yes;
  using result_t = std::invoke_result_t<ResultGetterType>;
  constexpr std::array values = name_hashes_v<EnumType>;
  constexpr std::size_t size = values.size();
  constexpr std::array cases = calculate_cases<std::size_t>(page, values);

  switch (hash_string(searched)) {
    MAGIC_ENUM_FOR_EACH_256(MAGIC_ENUM_CASE)
  default:
    if constexpr (size > 256 + page) {
      return constexpr_switch<EnumType, page + 256>(std::forward<Lambda>(lambda), searched, std::forward<ResultGetterType>(def));
    }
  }
  return def();
}

#undef MAGIC_ENUM_FOR_EACH_256
#undef MAGIC_ENUM_CASE

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

// Returns type name of enum.
template <typename E>
[[nodiscard]] constexpr auto enum_type_name() noexcept -> detail::enable_if_enum_t<E, string_view> {
  constexpr string_view name = detail::type_name_v<std::decay_t<E>>;
  static_assert(name.size() > 0, "Enum type does not have a name.");

  return name;
}

// Returns number of enum values.
template <typename E>
[[nodiscard]] constexpr auto enum_count() noexcept -> detail::enable_if_enum_t<E, std::size_t> {
  return detail::count_v<std::decay_t<E>>;
}

// Returns enum value at specified index.
// No bounds checking is performed: the behavior is undefined if index >= number of enum values.
template <typename E>
[[nodiscard]] constexpr auto enum_value(std::size_t index) noexcept -> detail::enable_if_enum_t<E, std::decay_t<E>> {
  using D = std::decay_t<E>;

  if constexpr (detail::is_sparse_v<D>) {
    return assert((index < detail::count_v<D>)), detail::values_v<D>[index];
  } else {
    constexpr bool is_flag = detail::is_flags_v<D>;
    constexpr auto min = is_flag ? detail::log2(detail::min_v<D>) : detail::min_v<D>;

    return assert((index < detail::count_v<D>)), detail::value<D, min, is_flag>(index);
  }
}

// Returns enum value at specified index.
template <typename E, std::size_t I>
[[nodiscard]] constexpr auto enum_value() noexcept -> detail::enable_if_enum_t<E, std::decay_t<E>> {
  return enum_value<std::decay_t<E>>(I);
}

// Returns std::array with enum values, sorted by enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_values() noexcept -> detail::enable_if_enum_t<E, detail::values_t<E>> {
  return detail::values_v<std::decay_t<E>>;
}

// Obtains index in enum values from enum value.
// Returns optional with index.
template <typename E>
[[nodiscard]] constexpr auto enum_index(E value) noexcept -> detail::enable_if_enum_t<E, optional<std::size_t>> {
  using D = std::decay_t<E>;
  using U = underlying_type_t<D>;

  if constexpr (detail::is_sparse_v<D> || detail::is_flags_v<D>) {
    return detail::constexpr_switch<detail::try_index_invoke_t::yes>([](std::size_t index) { return optional<std::size_t>{index}; }, value,
                                                                     detail::default_result_type_lambda<optional<std::size_t>>);
  } else {
    const auto v = static_cast<U>(value);
    if (v >= detail::min_v<D> && v <= detail::max_v<D>) {
      return static_cast<std::size_t>(v - detail::min_v<D>);
    }
    return {}; // Invalid value or out of range.
  }
}

// Returns name from static storage enum variable.
// This version is much lighter on the compile times and is not restricted to the enum_range limitation.
template <auto V>
[[nodiscard]] constexpr auto enum_name() noexcept -> detail::enable_if_enum_t<decltype(V), string_view> {
  constexpr string_view name = detail::enum_name_v<std::decay_t<decltype(V)>, V>;
  static_assert(name.size() > 0, "Enum value does not have a name.");

  return name;
}

// Returns name from enum value.
// If enum value does not have name or value out of range, returns empty string.
template <typename E>
[[nodiscard]] constexpr auto enum_name(E value) noexcept -> detail::enable_if_enum_t<E, string_view> {
  using D = std::decay_t<E>;
  if (auto index = enum_index(value))
      return detail::names_v<D>[*index];
  return {};
}

// Returns name from enum-flags value.
// If enum-flags value does not have name or value out of range, returns empty string.
template <typename E>
[[nodiscard]] auto enum_flags_name(E value) -> detail::enable_if_enum_t<E, string> {
  using D = std::decay_t<E>;
  using U = underlying_type_t<D>;

  if constexpr (detail::is_flags_v<D>) {
    string name;
    auto check_value = U{0};
    for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
      if (const auto v = static_cast<U>(enum_value<D>(i)); (static_cast<U>(value) & v) != 0) {
        check_value |= v;
        const auto n = detail::names_v<D>[i];
        if (!name.empty()) {
          name.append(1, '|');
        }
        name.append(n.data(), n.size());
      }
    }

    if (check_value != 0 && check_value == static_cast<U>(value)) {
      return name;
    }

    return {}; // Invalid value or out of range.
  } else {
    return string{enum_name(value)};
  }
}

// Returns std::array with names, sorted by enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_names() noexcept -> detail::enable_if_enum_t<E, detail::names_t<E>> {
  return detail::names_v<std::decay_t<E>>;
}

// Returns std::array with pairs (value, name), sorted by enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_entries() noexcept -> detail::enable_if_enum_t<E, detail::entries_t<E>> {
  return detail::entries_v<std::decay_t<E>>;
}

// Obtains enum value from integer value.
// Returns optional with enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_cast(underlying_type_t<std::decay_t<E>> value) noexcept -> detail::enable_if_enum_t<E, optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  using U = underlying_type_t<D>;

  if constexpr (detail::is_sparse_v<D>) {
    if constexpr (detail::is_flags_v<D>) {
      constexpr auto count = detail::count_v<D>;
      auto check_value = U{0};
      for (std::size_t i = 0; i < count; ++i) {
        if (const auto v = static_cast<U>(enum_value<D>(i)); (value & v) != 0) {
          check_value |= v;
        }
      }

      if (check_value != 0 && check_value == value) {
        return static_cast<D>(value);
      }
      return {}; // Invalid value or out of range.
    } else {
      return detail::constexpr_switch([](D value) { return optional<D>{value}; }, static_cast<D>(value),
                                      detail::default_result_type_lambda<optional<D>>);
    }
  } else {
    constexpr auto min = detail::min_v<D>;
    constexpr auto max = detail::is_flags_v<D> ? detail::values_ors<D>() : detail::max_v<D>;

    if (value >= min && value <= max) {
      return static_cast<D>(value);
    }
    return {}; // Invalid value or out of range.
  }
}

// allows you to write magic_enum::enum_cast<foo>("bar", magic_enum::case_insensitive);
inline constexpr auto case_insensitive = [](auto lhs, auto rhs) noexcept
        -> std::enable_if_t<std::is_same_v<std::decay_t<decltype(lhs)>, char> && std::is_same_v<std::decay_t<decltype(rhs)>, char>, bool> {
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
  static_assert(!std::is_same_v<decltype(lhs), decltype(lhs)>, "magic_enum::case_insensitive not supported Non-ASCII feature.");
  return {};
#else
  return detail::to_lower(lhs) == detail::to_lower(rhs);
#endif
};

// Obtains enum value from name.
// Returns optional with enum value.
template <typename E, typename BinaryPredicate = std::equal_to<char>>
[[nodiscard]] constexpr auto enum_cast(string_view value, [[maybe_unused]] BinaryPredicate p = {}) noexcept(std::is_nothrow_invocable_r_v<bool, BinaryPredicate, char, char>) -> detail::enable_if_enum_t<E, optional<std::decay_t<E>>> {
  static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>, "magic_enum::enum_cast requires bool(char, char) invocable predicate.");
  using D = std::decay_t<E>;
  using U = underlying_type_t<D>;

  if constexpr (detail::is_flags_v<D>) {
    auto result = U{0};
    while (!value.empty()) {
      const auto d = detail::find(value, '|');
      const auto s = (d == string_view::npos) ? value : value.substr(0, d);
      auto f = U{0};
      for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
        if (detail::cmp_equal(s, detail::names_v<D>[i], p)) {
          f = static_cast<U>(enum_value<D>(i));
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
  } else {
    constexpr bool default_predicate =
        std::is_same_v<std::decay_t<BinaryPredicate>, std::equal_to<char>> ||
        std::is_same_v<std::decay_t<BinaryPredicate>, std::equal_to<>>;
    if constexpr (default_predicate) {
      return detail::constexpr_switch<D>([](std::size_t index) { return optional<D>{detail::values_v<D>[index]}; }, value,
                                         detail::default_result_type_lambda<optional<D>>);
    } else {
      for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
        if (detail::cmp_equal(value, detail::names_v<D>[i], p)) {
          return enum_value<D>(i);
        }
      }
      return {}; // Invalid value or out of range.
    }
  }

  return {}; // Invalid value or out of range.
}

// Returns integer value from enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_integer(E value) noexcept -> detail::enable_if_enum_t<E, underlying_type_t<E>> {
  return static_cast<underlying_type_t<E>>(value);
}

// Checks whether enum contains enumerator with such enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_contains(E value) noexcept -> detail::enable_if_enum_t<E, bool> {
  using D = std::decay_t<E>;
  using U = underlying_type_t<D>;

  return enum_cast<D>(static_cast<U>(value)).has_value();
}

// Checks whether enum contains enumerator with such integer value.
template <typename E>
[[nodiscard]] constexpr auto enum_contains(underlying_type_t<E> value) noexcept -> detail::enable_if_enum_t<E, bool> {
  return enum_cast<std::decay_t<E>>(value).has_value();
}

// Checks whether enum contains enumerator with such name.
template <typename E, typename BinaryPredicate = std::equal_to<char>>
[[nodiscard]] constexpr auto enum_contains(string_view value, BinaryPredicate p = {}) noexcept(std::is_nothrow_invocable_r_v<bool, BinaryPredicate, char, char>) -> detail::enable_if_enum_t<E, bool> {
  static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>, "magic_enum::enum_contains requires bool(char, char) invocable predicate.");

  return enum_cast<std::decay_t<E>>(value, std::move_if_noexcept(p)).has_value();
}

namespace fusion_detail {

template <typename E>
constexpr std::optional<std::uintmax_t> fuse_one_enum(std::optional<std::uintmax_t> hash, E value) noexcept {
  if (hash.has_value()) {
    if (const auto index = enum_index(value); index.has_value()) {
      // Add 1 to prevent matching 2D fusions with 3D fusions etc.
      return (hash.value() << detail::log2(enum_count<E>() + 1)) | (index.value() + 1);
    }
  }
  return std::nullopt;
}

template <typename E>
constexpr std::optional<std::uintmax_t> fuse_enum(E value) noexcept {
  return fuse_one_enum(0, value);
}

template <typename E, typename... Es>
constexpr std::optional<std::uintmax_t> fuse_enum(E head, Es... tail) noexcept {
  return fuse_one_enum(fuse_enum(tail...), head);
}

} // namespace magic_enum::fusion_detail

// Returns a bijective mix of several enum values. This can be used to emulate 2D switch/case statements.
template <typename... Es>
[[nodiscard]] constexpr auto enum_fuse(Es... values) -> std::enable_if_t<(std::is_enum_v<std::decay_t<Es>> && ...), std::optional<std::uintmax_t>> {
  static_assert(sizeof...(Es) >= 2, "magic_enum::enum_fuse requires at least 2 enums");
  static_assert((detail::log2(enum_count<Es>() + 1) + ...) <= (sizeof(std::uintmax_t) * 8), "magic_enum::enum_fuse does not work for large enums");
  const auto fuse = fusion_detail::fuse_enum(values...);
  return assert(fuse.has_value()), fuse;
}

namespace ostream_operators {

template <typename Char, typename Traits, typename E, detail::enable_if_enum_t<E, int> = 0>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, E value) {
  using D = std::decay_t<E>;
  using U = underlying_type_t<D>;

  if constexpr (detail::supported<D>::value) {
    if (const auto name = magic_enum::enum_flags_name<D>(value); !name.empty()) {
      for (const auto c : name) {
        os.put(c);
      }
      return os;
    }
  }
  return (os << static_cast<U>(value));
}

template <typename Char, typename Traits, typename E, detail::enable_if_enum_t<E, int> = 0>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, optional<E> value) {
  return value.has_value() ? (os << value.value()) : os;
}

} // namespace magic_enum::ostream_operators

namespace bitwise_operators {

template <typename E, detail::enable_if_enum_t<E, int> = 0>
constexpr E operator~(E rhs) noexcept {
  return static_cast<E>(~static_cast<underlying_type_t<E>>(rhs));
}

template <typename E, detail::enable_if_enum_t<E, int> = 0>
constexpr E operator|(E lhs, E rhs) noexcept {
  return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) | static_cast<underlying_type_t<E>>(rhs));
}

template <typename E, detail::enable_if_enum_t<E, int> = 0>
constexpr E operator&(E lhs, E rhs) noexcept {
  return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) & static_cast<underlying_type_t<E>>(rhs));
}

template <typename E, detail::enable_if_enum_t<E, int> = 0>
constexpr E operator^(E lhs, E rhs) noexcept {
  return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) ^ static_cast<underlying_type_t<E>>(rhs));
}

template <typename E, detail::enable_if_enum_t<E, int> = 0>
constexpr E& operator|=(E& lhs, E rhs) noexcept {
  return lhs = (lhs | rhs);
}

template <typename E, detail::enable_if_enum_t<E, int> = 0>
constexpr E& operator&=(E& lhs, E rhs) noexcept {
  return lhs = (lhs & rhs);
}

template <typename E, detail::enable_if_enum_t<E, int> = 0>
constexpr E& operator^=(E& lhs, E rhs) noexcept {
  return lhs = (lhs ^ rhs);
}

} // namespace magic_enum::bitwise_operators

} // namespace magic_enum

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // NEARGYE_MAGIC_ENUM_HPP
