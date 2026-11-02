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

#ifndef NEARGYE_MAGIC_ENUM_HPP
#define NEARGYE_MAGIC_ENUM_HPP

#define MAGIC_ENUM_VERSION_MAJOR 0
#define MAGIC_ENUM_VERSION_MINOR 9
#define MAGIC_ENUM_VERSION_PATCH 8

#ifndef MAGIC_ENUM_USE_STD_MODULE
#  include <array>
#  include <cstddef>
#  include <cstdint>
#  include <functional>
#  include <limits>
#  include <type_traits>
#  include <utility>
#endif

#if defined(MAGIC_ENUM_CONFIG_FILE)
#  include MAGIC_ENUM_CONFIG_FILE
#endif

// MAGIC_ENUM_USE_STD_MODULE imports the standard library before this header.
#if !defined(MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION) && defined(__cpp_impl_reflection) && __cpp_impl_reflection >= 202506L && defined(__cpp_expansion_statements) && __cpp_expansion_statements >= 202506L
#  if !defined(MAGIC_ENUM_USE_STD_MODULE) && __has_include(<meta>)
#    include <meta>
#  endif
#  if defined(__cpp_lib_reflection) && __cpp_lib_reflection >= 202506L && defined(__cpp_lib_define_static) && __cpp_lib_define_static >= 202506L
#    define MAGIC_ENUM_DETAIL_USE_STD_REFLECTION 1
#  endif
#endif

#ifndef MAGIC_ENUM_USE_STD_MODULE
#  if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
#    include <algorithm>
#  endif
#  if !defined(MAGIC_ENUM_USING_ALIAS_OPTIONAL)
#    include <optional>
#  endif
#  if !defined(MAGIC_ENUM_USING_ALIAS_STRING)
#    include <string>
#  endif
#  if !defined(MAGIC_ENUM_USING_ALIAS_STRING_VIEW)
#    include <string_view>
#  endif
#endif

#if defined(MAGIC_ENUM_NO_ASSERT) && defined(MAGIC_ENUM_ASSERT)
#  error MAGIC_ENUM_NO_ASSERT and MAGIC_ENUM_ASSERT cannot be used together.
#elif defined(MAGIC_ENUM_NO_ASSERT)
#  define MAGIC_ENUM_ASSERT(...) static_cast<void>(0)
#elif !defined(MAGIC_ENUM_ASSERT)
#  include <cassert>
#  define MAGIC_ENUM_ASSERT(...) assert((__VA_ARGS__))
#endif

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunknown-warning-option"
#  pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wconversion" // Scanning casts candidates outside the value range of unscoped enums without a fixed underlying type.
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 28020) // MSVC analyzer loses constexpr array bounds in template instantiations.
#  pragma warning(disable : 4514) // Unreferenced inline function has been removed.
#endif

// Checks magic_enum compiler compatibility.
#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION) || defined(__clang__) && __clang_major__ >= 9 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1920
#  define MAGIC_ENUM_SUPPORTED 1
#elif defined(__RESHARPER__)
#  define MAGIC_ENUM_SUPPORTED 1
#endif

// Enum value must be greater or equals than MAGIC_ENUM_RANGE_MIN. By default MAGIC_ENUM_RANGE_MIN = -128.
// If need another min range for all enum types by default, redefine the macro MAGIC_ENUM_RANGE_MIN.
#if !defined(MAGIC_ENUM_RANGE_MIN)
#  define MAGIC_ENUM_RANGE_MIN -128
#endif

// Enum value must be less or equals than MAGIC_ENUM_RANGE_MAX. By default MAGIC_ENUM_RANGE_MAX = 127.
// If need another max range for all enum types by default, redefine the macro MAGIC_ENUM_RANGE_MAX.
#if !defined(MAGIC_ENUM_RANGE_MAX)
#  define MAGIC_ENUM_RANGE_MAX 127
#endif

// Improve ReSharper C++ intellisense performance with builtins, avoiding unnecessary template instantiations.
#if defined(__RESHARPER__)
#  undef MAGIC_ENUM_GET_ENUM_NAME_BUILTIN
#  undef MAGIC_ENUM_GET_TYPE_NAME_BUILTIN
#  if __RESHARPER__ >= 20230100
#    define MAGIC_ENUM_GET_ENUM_NAME_BUILTIN(V) __rscpp_enumerator_name(V)
#    define MAGIC_ENUM_GET_TYPE_NAME_BUILTIN(T) __rscpp_type_name<T>()
#  else
#    define MAGIC_ENUM_GET_ENUM_NAME_BUILTIN(V) nullptr
#    define MAGIC_ENUM_GET_TYPE_NAME_BUILTIN(T) nullptr
#  endif
#endif

// Specify calling convention for compilers that need it to produce reliable mangled names under different compiler flags. In particular, MSVC allows changing default calling convention on x86.
#if defined(_MSC_VER) && !defined(__clang__)
#  define MAGIC_ENUM_CALLING_CONVENTION __cdecl
#else
#  define MAGIC_ENUM_CALLING_CONVENTION
#endif

// Signature of the enclosing function. n() reads type and enum names from it.
#if defined(_MSC_VER) && !defined(__clang__)
#  define MAGIC_ENUM_SIGNATURE __FUNCSIG__
#elif defined(__clang__) || defined(__GNUC__)
#  if defined(__PRETTY_FUNCTION__) && !defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
#    error magic_enum requires the builtin __PRETTY_FUNCTION__, not a macro.
#  endif
#  define MAGIC_ENUM_SIGNATURE __PRETTY_FUNCTION__
#else
#  define MAGIC_ENUM_SIGNATURE ""
#endif

#if defined(__cpp_consteval) && __cpp_consteval >= 201811L
#  define MAGIC_ENUM_CONSTEVAL consteval
#else
#  define MAGIC_ENUM_CONSTEVAL constexpr
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

using char_type = string_view::value_type;
static_assert(std::is_same_v<string_view::value_type, string::value_type>, "magic_enum::customize requires same string_view::value_type and string::value_type");
#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
static_assert(std::is_same_v<char_type, char> || std::is_same_v<char_type, wchar_t> || std::is_same_v<char_type, char8_t> || std::is_same_v<char_type, char16_t> || std::is_same_v<char_type, char32_t>,
              "magic_enum standard reflection requires string_view::value_type to be a standard character type; define MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION before including magic_enum.hpp for other character types.");
#endif
static_assert([] {
  if constexpr (std::is_same_v<char_type, wchar_t>) {
    constexpr const char     c[] =  "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789|";
    constexpr const wchar_t wc[] = L"abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789|";
    static_assert(std::size(c) == std::size(wc), "magic_enum::customize identifier characters are multichars in wchar_t.");

    for (std::size_t i = 0; i < std::size(c); ++i) {
      if (c[i] != wc[i]) {
        return false;
      }
    }
  }
  return true;
}(), "magic_enum::customize wchar_t is not compatible with ASCII.");

namespace customize {

template <bool IsFlags = false, int Min = MAGIC_ENUM_RANGE_MIN, int Max = MAGIC_ENUM_RANGE_MAX, std::size_t PrefixLength = 0>
struct adl_info_holder {
  static constexpr int min = Min;
  static constexpr int max = Max;
  static constexpr bool is_flags = IsFlags;
  static constexpr std::size_t prefix_length = PrefixLength;

  template <int NewMin, int NewMax>
  static constexpr adl_info_holder<IsFlags, NewMin, NewMax, PrefixLength> minmax() { return {}; }

  template <bool NewIsFlags>
  static constexpr adl_info_holder<NewIsFlags, Min, Max, PrefixLength> flag() { return {}; }

  template <std::size_t NewPrefixLength>
  static constexpr adl_info_holder<IsFlags, Min, Max, NewPrefixLength> prefix() { return {}; }
};

constexpr adl_info_holder<> adl_info() { return {}; }

// Compiler-specific reflection scans common enum values in [min, max]. Redefine MAGIC_ENUM_RANGE_MIN/MAX globally or specialize enum_range for a specific enum.
template <typename E, typename = void>
struct enum_range {
  static constexpr int min = MAGIC_ENUM_RANGE_MIN;
  static constexpr int max = MAGIC_ENUM_RANGE_MAX;
};

template <typename E>
struct enum_range<E, decltype(void(magic_enum_define_range_adl(E{})))> : decltype(magic_enum_define_range_adl(E{})) {};

namespace detail {

enum class customize_tag {
  default_tag,
  invalid_tag,
  custom_tag
};

} // namespace magic_enum::customize::detail

class customize_t : public std::pair<detail::customize_tag, string_view> {
 public:
  constexpr customize_t(string_view srt) : std::pair<detail::customize_tag, string_view>{detail::customize_tag::custom_tag, srt} {}
  constexpr customize_t(const char_type* srt) : customize_t{string_view{srt}} {}
  constexpr customize_t(detail::customize_tag tag) : std::pair<detail::customize_tag, string_view>{tag, string_view{}} {
    MAGIC_ENUM_ASSERT(tag != detail::customize_tag::custom_tag);
  }
};

// Default customize.
inline constexpr auto default_tag = customize_t{detail::customize_tag::default_tag};
// Invalid customize.
inline constexpr auto invalid_tag = customize_t{detail::customize_tag::invalid_tag};

// If need custom names for enum, add specialization enum_name for necessary enum type.
template <typename E>
constexpr customize_t enum_name(E) noexcept {
  return default_tag;
}

// If need custom type name for enum, add specialization enum_type_name for necessary enum type.
template <typename E>
constexpr customize_t enum_type_name() noexcept {
  return default_tag;
}

} // namespace magic_enum::customize

namespace detail {

template <typename E, typename = void>
inline constexpr std::size_t prefix_length_v = 0;

template <typename E>
inline constexpr auto prefix_length_v<E, std::void_t<decltype(customize::enum_range<E>::prefix_length)>> = std::size_t{customize::enum_range<E>::prefix_length};

MAGIC_ENUM_CONSTEVAL bool valid_prefix_length(std::size_t prefix, std::size_t name_size) noexcept {
  return prefix == 0 || prefix < name_size;
}

template <typename T>
struct supported
#if defined(MAGIC_ENUM_SUPPORTED) || defined(MAGIC_ENUM_NO_CHECK_SUPPORT)
  : std::true_type {};
#else
  : std::false_type {};
#endif

template <auto V, typename E = std::decay_t<decltype(V)>, std::enable_if_t<std::is_enum_v<E>, int> = 0>
using enum_constant = std::integral_constant<E, V>;

template <typename... T>
inline constexpr bool always_false_v = false;

template <typename T, typename = void>
struct has_is_flags : std::false_type {};

template <typename T>
struct has_is_flags<T, std::void_t<decltype(customize::enum_range<T>::is_flags)>> : std::true_type {};

template <typename T, typename = void>
inline constexpr bool valid_is_flags_v = true;

template <typename T>
inline constexpr bool valid_is_flags_v<T, std::void_t<decltype(customize::enum_range<T>::is_flags)>> = std::is_same_v<bool, std::decay_t<decltype(customize::enum_range<T>::is_flags)>>;

template <typename T, typename = void>
struct range_min : std::integral_constant<int, MAGIC_ENUM_RANGE_MIN> {};

template <typename T>
struct range_min<T, std::void_t<decltype(customize::enum_range<T>::min)>> : std::integral_constant<decltype(customize::enum_range<T>::min), customize::enum_range<T>::min> {};

template <typename T, typename = void>
struct range_max : std::integral_constant<int, MAGIC_ENUM_RANGE_MAX> {};

template <typename T>
struct range_max<T, std::void_t<decltype(customize::enum_range<T>::max)>> : std::integral_constant<decltype(customize::enum_range<T>::max), customize::enum_range<T>::max> {};

struct cname_ref {
  const char* str_ = nullptr;
  std::size_t size_ = 0;
};

template <typename E>
constexpr bool enum_value_equal(E lhs, E rhs) noexcept {
  using U = std::underlying_type_t<E>;
  return static_cast<U>(lhs) == static_cast<U>(rhs);
}

template <typename E>
constexpr bool enum_value_equal(E lhs, std::underlying_type_t<E> rhs) noexcept {
  return static_cast<std::underlying_type_t<E>>(lhs) == rhs;
}

#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)

namespace reflection {

template <typename E>
consteval auto enumerators() noexcept {
  if constexpr (std::meta::is_enumerable_type(^^E)) {
    return std::define_static_array(std::meta::enumerators_of(^^E));
  } else {
    static_assert(always_false_v<E>, "magic_enum requires a complete enum definition.");
    return std::array<std::meta::info, 0>{};
  }
}

template <typename E>
inline constexpr auto enumerators_v = enumerators<E>();

template <typename E>
consteval auto type_name() noexcept {
  if constexpr (std::meta::has_identifier(^^E)) {
    constexpr auto identifier = std::meta::identifier_of(^^E);
    return cname_ref{identifier.data(), identifier.size()};
  } else {
    return cname_ref{};
  }
}

template <auto V, typename E = std::decay_t<decltype(V)>>
consteval auto enum_name() noexcept {
  template for (constexpr auto enumerator : enumerators_v<E>) {
    if constexpr (enum_value_equal([:enumerator:], V)) {
      constexpr auto identifier = std::meta::identifier_of(enumerator);
      return cname_ref{identifier.data(), identifier.size()}; // Preserve one-name-per-value: first declaration wins.
    }
  }
  return cname_ref{};
}

template <typename E>
constexpr bool contains_underlying([[maybe_unused]] std::underlying_type_t<E> value) noexcept {
  template for (constexpr auto enumerator : enumerators_v<E>) {
    if (enum_value_equal([:enumerator:], value)) {
      return true;
    }
  }
  return false;
}

template <typename E>
MAGIC_ENUM_CONSTEVAL bool contains(E value) noexcept {
  return contains_underlying<E>(static_cast<std::underlying_type_t<E>>(value));
}

} // namespace reflection

#endif

// Always null-terminated. The empty string points to static storage.
template <std::uint16_t N>
class static_str {
 public:
  constexpr static_str() noexcept : chars_{} {}

  constexpr explicit static_str(cname_ref str) noexcept : static_str{str.str_, std::make_integer_sequence<std::uint16_t, N>{}} {}

  constexpr explicit static_str(const char* const str) noexcept : static_str{str, std::make_integer_sequence<std::uint16_t, N>{}} {}

  constexpr explicit static_str(string_view str) noexcept : static_str{str.data(), std::make_integer_sequence<std::uint16_t, N>{}} {}

  constexpr const char_type* data() const noexcept { return N == 0 ? empty_ : chars_; }

  constexpr std::uint16_t size() const noexcept { return N; }

  constexpr string_view str() const noexcept { return string_view(data(), size()); }

  // Public because GCC 13-15 wrongly checks access to them when a constexpr name is used in a templated lambda, see #452.
  char_type chars_[static_cast<std::size_t>(N) + 1];

  static constexpr char_type empty_[1] = {};

 private:
  template <typename Char, std::uint16_t... J>
  constexpr static_str([[maybe_unused]] const Char* str, std::integer_sequence<std::uint16_t, J...>) noexcept : chars_{static_cast<char_type>(str[J])..., char_type{}} {}
};

template <typename Op = std::equal_to<>>
class case_insensitive {
  static constexpr char_type to_lower(char_type c) noexcept {
    return (c >= char_type{'A'} && c <= char_type{'Z'}) ? static_cast<char_type>(c + (char_type{'a'} - char_type{'A'})) : c;
  }

 public:
  template <typename L, typename R>
  constexpr auto operator()(L lhs, R rhs) const noexcept -> std::enable_if_t<std::is_same_v<std::decay_t<L>, char_type> && std::is_same_v<std::decay_t<R>, char_type>, bool> {
    return Op{}(to_lower(lhs), to_lower(rhs));
  }
};

template <typename T>
inline constexpr bool is_name_input_v = std::is_convertible_v<T, string_view>;

// MSVC 19.29 reports std::is_nothrow_constructible_v as true for a throwing conversion operator.
template <typename T>
inline constexpr bool is_nothrow_name_input_v = noexcept(static_cast<string_view>(std::declval<T>()));

template <typename BinaryPredicate>
inline constexpr bool is_default_predicate_v = std::is_same_v<std::decay_t<BinaryPredicate>, std::equal_to<string_view::value_type>> || std::is_same_v<std::decay_t<BinaryPredicate>, std::equal_to<>>;

template <typename BinaryPredicate>
inline constexpr bool is_nothrow_invocable_v = is_default_predicate_v<BinaryPredicate> || std::is_nothrow_invocable_r_v<bool, BinaryPredicate&, const char_type&, const char_type&>;

template <typename BinaryPredicate>
constexpr bool cmp_equal(string_view lhs, string_view rhs, [[maybe_unused]] BinaryPredicate&& p) noexcept(is_nothrow_invocable_v<BinaryPredicate>) {
  if constexpr (!is_default_predicate_v<BinaryPredicate>) {
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

// std::cmp_less is C++20 and rejects bool.
template <typename L, typename R>
constexpr bool cmp_less(L lhs, R rhs) noexcept {
  static_assert(std::is_integral_v<L> && std::is_integral_v<R>, "magic_enum::detail::cmp_less requires integral type.");

  if constexpr (std::is_same_v<L, bool>) {
    return cmp_less(static_cast<int>(lhs), rhs);
  } else if constexpr (std::is_same_v<R, bool>) {
    return cmp_less(lhs, static_cast<int>(rhs));
  } else if constexpr (std::is_signed_v<L> == std::is_signed_v<R>) {
    return lhs < rhs;
  } else if constexpr (std::is_signed_v<L>) {
    return lhs < 0 || static_cast<std::make_unsigned_t<L>>(lhs) < rhs;
  } else {
    return rhs > 0 && lhs < static_cast<std::make_unsigned_t<R>>(rhs);
  }
}

template <typename T>
using make_unsigned_t = std::make_unsigned_t<std::conditional_t<std::is_same_v<T, bool>, unsigned char, T>>;

template <typename T>
constexpr T log2(T value) noexcept {
  static_assert(std::is_integral_v<T>, "magic_enum::detail::log2 requires integral type.");

  auto ret = T{0};
  for (; value > T{1}; value >>= T{1}, ++ret) {}

  return ret;
}

template <typename T>
constexpr bool has_single_bit(T value) noexcept {
  using V = make_unsigned_t<T>;
  const auto v = static_cast<V>(value);
  return v != V{0} && (v & (v - V{1})) == V{0};
}

// Flag candidates need not have reflected names.
template <typename T>
constexpr bool is_flag_candidate(T value) noexcept {
  return value == T{0} || has_single_bit(value);
}

// Drops everything up to the last ':' of a qualified name.
MAGIC_ENUM_CONSTEVAL cname_ref unqualified(cname_ref name) noexcept {
  for (std::size_t i = name.size_; i > 0; --i) {
    if (name.str_[i] == ':') {
      return {name.str_ + i + 1, name.size_ - i - 1};
    }
  }
  return name;
}

// Parse signatures outside n() to keep each instantiation small.
MAGIC_ENUM_CONSTEVAL cname_ref type_signature_name([[maybe_unused]] const char* str, [[maybe_unused]] std::size_t size) noexcept {
#if defined(__clang__)
  return {str + 34, size - 35};
#elif defined(__GNUC__)
  return str[size - 1] == ']' ? cname_ref{str + 49, size - 50} : cname_ref{str + 37, size - 40};
#elif defined(_MSC_VER)
  // CLI/C++ workaround, see #284.
  return {str + 40, size - 56};
#else
  return {};
#endif
}

MAGIC_ENUM_CONSTEVAL cname_ref enum_signature_name([[maybe_unused]] const char* str, [[maybe_unused]] std::size_t size) noexcept {
#if defined(__clang__)
  cname_ref name{str + 34, size - 35};
  if (name.size_ > 22 && name.str_[0] == '(' && name.str_[1] == 'a' && name.str_[10] == ' ' && name.str_[22] == ':') {
    name.size_ -= 23;
    name.str_ += 23;
  }
  if (name.str_[0] == '(' || name.str_[0] == '-' || (name.str_[0] >= '0' && name.str_[0] <= '9')) {
    return {};
  }
  return name;
#elif defined(__GNUC__)
  if (str[size - 1] == ']') {
    return str[54] == '(' ? cname_ref{} : cname_ref{str + 54, size - 55};
  }
  return str[37] == '(' ? cname_ref{} : cname_ref{str + 37, size - 40};
#elif defined(_MSC_VER)
  // CLI/C++ workaround, see #284.
  if ((str[5] == '_' && str[35] != '(') || (str[5] == 'c' && str[41] != '(')) {
    return {str + 35, size - 51};
  }
  return {};
#else
  return {};
#endif
}

template <typename E>
MAGIC_ENUM_CONSTEVAL auto MAGIC_ENUM_CALLING_CONVENTION n() noexcept {
  if constexpr (supported<E>::value) {
#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
    return reflection::type_name<E>();
#elif defined(MAGIC_ENUM_GET_TYPE_NAME_BUILTIN)
    constexpr auto name_ptr = MAGIC_ENUM_GET_TYPE_NAME_BUILTIN(E);
    return unqualified(name_ptr ? cname_ref{name_ptr, std::char_traits<char>::length(name_ptr)} : cname_ref{});
#else
    return unqualified(type_signature_name(MAGIC_ENUM_SIGNATURE, sizeof(MAGIC_ENUM_SIGNATURE) - 1));
#endif
  } else {
    return cname_ref{}; // Unsupported compiler or Invalid customize.
  }
}

template <typename E>
MAGIC_ENUM_CONSTEVAL auto type_name() noexcept {
  [[maybe_unused]] constexpr auto custom = customize::enum_type_name<E>();
  static_assert(std::is_same_v<std::decay_t<decltype(custom)>, customize::customize_t>, "magic_enum::customize requires customize_t type.");
  if constexpr (custom.first == customize::detail::customize_tag::custom_tag) {
    constexpr auto name = custom.second;
    static_assert(!name.empty(), "magic_enum::customize requires not empty string.");
    return static_str<name.size()>{name};
  } else if constexpr (custom.first == customize::detail::customize_tag::invalid_tag) {
    return static_str<0>{};
  } else if constexpr (custom.first == customize::detail::customize_tag::default_tag) {
    constexpr auto name = n<E>();
    return static_str<name.size_>{name};
  } else {
    static_assert(always_false_v<E>, "magic_enum::customize invalid.");
  }
}

template <typename E>
inline constexpr auto type_name_v = type_name<E>();

template <auto V>
MAGIC_ENUM_CONSTEVAL auto MAGIC_ENUM_CALLING_CONVENTION n() noexcept {
  if constexpr (supported<decltype(V)>::value) {
#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
    return reflection::enum_name<V>();
#elif defined(MAGIC_ENUM_GET_ENUM_NAME_BUILTIN)
    constexpr auto name_ptr = MAGIC_ENUM_GET_ENUM_NAME_BUILTIN(V);
    return unqualified(name_ptr ? cname_ref{name_ptr, std::char_traits<char>::length(name_ptr)} : cname_ref{});
#else
    return unqualified(enum_signature_name(MAGIC_ENUM_SIGNATURE, sizeof(MAGIC_ENUM_SIGNATURE) - 1));
#endif
  } else {
    return cname_ref{}; // Unsupported compiler or Invalid customize.
  }
}

template <typename E, E V>
MAGIC_ENUM_CONSTEVAL auto enum_name() noexcept {
  [[maybe_unused]] constexpr auto custom = customize::enum_name<E>(V);
  static_assert(std::is_same_v<std::decay_t<decltype(custom)>, customize::customize_t>, "magic_enum::customize requires customize_t type.");
  if constexpr (custom.first == customize::detail::customize_tag::custom_tag) {
#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
    static_assert(reflection::contains<E>(V), "magic_enum::customize::enum_name cannot add an undeclared enum value when standard reflection is enabled.");
#endif
    constexpr auto name = custom.second;
    static_assert(!name.empty(), "magic_enum::customize requires not empty string.");
    return static_str<name.size()>{name};
  } else if constexpr (custom.first == customize::detail::customize_tag::invalid_tag) {
    return static_str<0>{};
  } else if constexpr (custom.first == customize::detail::customize_tag::default_tag) {
    constexpr auto name = n<V>();
    constexpr auto prefix = prefix_length_v<E>;
    // An unnamed value has no prefix to strip.
    constexpr bool valid_prefix = name.size_ == 0 || valid_prefix_length(prefix, name.size_);
    static_assert(valid_prefix, "magic_enum::customize requires valid prefix length.");
    if constexpr (name.size_ != 0 && valid_prefix) {
      return static_str<name.size_ - prefix>{name.str_ + prefix};
    } else {
      return static_str<0>{};
    }
  } else {
    static_assert(always_false_v<E>, "magic_enum::customize invalid.");
  }
}

template <typename E, E V>
inline constexpr auto enum_name_v = enum_name<E, V>();

// Reject custom values that bit_cast accepts but constant static_cast rejects.
template <typename E, auto V, typename = void>
inline constexpr bool is_valid_custom_value_v = false;

template <typename E, auto V>
inline constexpr bool is_valid_custom_value_v<E, V, std::void_t<std::integral_constant<E, static_cast<E>(V)>>> = true;

enum class enum_subtype {
  common,
  flags
};

#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)

namespace reflection {

struct name_ref {
  const char_type* data = nullptr;
  std::size_t size = 0;

  [[nodiscard]] constexpr string_view view() const noexcept { return {data, size}; }
};

template <typename E>
struct entry {
  E value{};
  name_ref name{};
  std::size_t declaration_index = 0;
};

template <typename E, std::size_t N>
struct table {
  std::array<entry<E>, N> entries{};
  std::size_t size = 0;
  bool prefixes_valid = true;
};

template <typename E, enum_subtype S, typename U = std::underlying_type_t<E>>
consteval auto canonical_data() noexcept {
  table<E, enumerators_v<E>.size()> result{};
  constexpr auto prefix = prefix_length_v<E>;
  [[maybe_unused]] std::size_t declaration_index = 0;

  template for (constexpr auto enumerator : enumerators_v<E>) {
    constexpr E value = [:enumerator:];
    if constexpr (S == enum_subtype::common || is_flag_candidate(static_cast<U>(value))) {
      constexpr auto custom = customize::enum_name<E>(value);
      static_assert(std::is_same_v<std::decay_t<decltype(custom)>, customize::customize_t>, "magic_enum::customize requires customize_t type.");

      if constexpr (custom.first == customize::detail::customize_tag::custom_tag) {
        constexpr auto name = custom.second;
        static_assert(!name.empty(), "magic_enum::customize requires not empty string.");
        constexpr auto persistent_name = std::define_static_string(std::span{name.data(), name.size()});
        result.entries[result.size++] = entry<E>{value, {persistent_name, name.size()}, declaration_index};
      } else if constexpr (custom.first == customize::detail::customize_tag::invalid_tag) {
        // An invalid customization removes this declared enumerator from the public tables.
      } else if constexpr (custom.first == customize::detail::customize_tag::default_tag) {
        constexpr auto identifier = std::meta::identifier_of(enumerator);
        if constexpr (valid_prefix_length(prefix, identifier.size())) {
          if constexpr (std::is_same_v<char_type, char>) {
            constexpr auto persistent_name = std::define_static_string(identifier.substr(prefix));
            result.entries[result.size++] = entry<E>{value, {persistent_name, identifier.size() - prefix}, declaration_index};
          } else {
            constexpr auto converted_name = [=] {
              std::array<char_type, identifier.size() - prefix> name{};
              for (std::size_t i = 0; i < name.size(); ++i) {
                name[i] = static_cast<char_type>(identifier[i + prefix]);
              }
              return name;
            }();
            constexpr auto persistent_name = std::define_static_string(converted_name);
            result.entries[result.size++] = entry<E>{value, {persistent_name, converted_name.size()}, declaration_index};
          }
        } else {
          result.entries[result.size++] = entry<E>{value, {}, declaration_index};
        }
      } else {
        static_assert(always_false_v<E>, "magic_enum::customize invalid.");
      }
    }
    ++declaration_index;
  }

  const auto less = [](const auto& lhs, const auto& rhs) {
    const auto lhs_value = static_cast<U>(lhs.value);
    const auto rhs_value = static_cast<U>(rhs.value);
    if (lhs_value != rhs_value) {
      return lhs_value < rhs_value;
    }
    return lhs.declaration_index < rhs.declaration_index;
  };
  const auto first = result.entries.begin();
  const auto last = first + result.size;
  if (!std::is_sorted(first, last, less)) {
    std::sort(first, last, less);
  }

  // Valid reflected names are non-empty, so an empty name marks an invalid prefix on the retained declaration.
  std::size_t unique_size = 0;
  for (std::size_t i = 0; i < result.size; ++i) {
    if (unique_size == 0 || !enum_value_equal(result.entries[unique_size - 1].value, result.entries[i].value)) {
      result.prefixes_valid = result.prefixes_valid && result.entries[i].name.size != 0;
      if (unique_size != i) {
        result.entries[unique_size] = result.entries[i];
      }
      ++unique_size;
    }
  }
  result.size = unique_size;
  return result;
}

template <typename E, enum_subtype S>
inline constexpr auto table_v = canonical_data<E, S>();

template <typename E, enum_subtype S>
consteval std::size_t table_size() noexcept {
  static_assert(table_v<E, S>.prefixes_valid, "magic_enum::customize requires valid prefix length.");
  return table_v<E, S>.size;
}

} // namespace reflection

#endif

template <typename E, int O, enum_subtype S, typename U = std::underlying_type_t<E>>
constexpr U ualue(std::size_t i) noexcept {
  if constexpr (S == enum_subtype::flags) {
    using V = make_unsigned_t<U>;
    // Position -1 is zero, so the scan finds a named zero like a bit.
    const auto bit = static_cast<int>(i) + O;
    return bit < 0 ? U{0} : static_cast<U>(V{1} << static_cast<V>(bit));
  } else {
    return static_cast<U>(static_cast<int>(i) + O);
  }
}

template <typename E, int O, enum_subtype S>
constexpr E value(std::size_t i) noexcept {
#if defined(__clang__) && __clang_major__ >= 16
  // CWG1766: Clang 16+ rejects a constant static_cast to a value outside the range of the values of an enumeration.
  // https://reviews.llvm.org/D130058, https://reviews.llvm.org/D131307
  return __builtin_bit_cast(E, ualue<E, O, S>(i));
#else
  return static_cast<E>(ualue<E, O, S>(i));
#endif
}

// Keep the type of a custom bound until values() checks that it fits in int.
template <typename E, enum_subtype S, typename U = std::underlying_type_t<E>>
MAGIC_ENUM_CONSTEVAL auto reflected_min() noexcept {
  if constexpr (S == enum_subtype::flags) {
    // Zero before bit 0, see ualue.
    return -1;
  } else if constexpr (cmp_less((std::numeric_limits<U>::min)(), range_min<E>::value)) {
    return range_min<E>::value;
  } else {
    return (std::numeric_limits<U>::min)();
  }
}

template <typename E, enum_subtype S, typename U = std::underlying_type_t<E>>
MAGIC_ENUM_CONSTEVAL auto reflected_max() noexcept {
  if constexpr (S == enum_subtype::flags) {
    // Include the sign bit of signed types.
    return std::numeric_limits<U>::digits + std::is_signed_v<U> - 1;
  } else if constexpr (cmp_less(range_max<E>::value, (std::numeric_limits<U>::max)())) {
    return range_max<E>::value;
  } else {
    return (std::numeric_limits<U>::max)();
  }
}

#define MAGIC_ENUM_FOR_EACH_256(T)                                                                                                                                                                 \
  T(  0)T(  1)T(  2)T(  3)T(  4)T(  5)T(  6)T(  7)T(  8)T(  9)T( 10)T( 11)T( 12)T( 13)T( 14)T( 15)T( 16)T( 17)T( 18)T( 19)T( 20)T( 21)T( 22)T( 23)T( 24)T( 25)T( 26)T( 27)T( 28)T( 29)T( 30)T( 31) \
  T( 32)T( 33)T( 34)T( 35)T( 36)T( 37)T( 38)T( 39)T( 40)T( 41)T( 42)T( 43)T( 44)T( 45)T( 46)T( 47)T( 48)T( 49)T( 50)T( 51)T( 52)T( 53)T( 54)T( 55)T( 56)T( 57)T( 58)T( 59)T( 60)T( 61)T( 62)T( 63) \
  T( 64)T( 65)T( 66)T( 67)T( 68)T( 69)T( 70)T( 71)T( 72)T( 73)T( 74)T( 75)T( 76)T( 77)T( 78)T( 79)T( 80)T( 81)T( 82)T( 83)T( 84)T( 85)T( 86)T( 87)T( 88)T( 89)T( 90)T( 91)T( 92)T( 93)T( 94)T( 95) \
  T( 96)T( 97)T( 98)T( 99)T(100)T(101)T(102)T(103)T(104)T(105)T(106)T(107)T(108)T(109)T(110)T(111)T(112)T(113)T(114)T(115)T(116)T(117)T(118)T(119)T(120)T(121)T(122)T(123)T(124)T(125)T(126)T(127) \
  T(128)T(129)T(130)T(131)T(132)T(133)T(134)T(135)T(136)T(137)T(138)T(139)T(140)T(141)T(142)T(143)T(144)T(145)T(146)T(147)T(148)T(149)T(150)T(151)T(152)T(153)T(154)T(155)T(156)T(157)T(158)T(159) \
  T(160)T(161)T(162)T(163)T(164)T(165)T(166)T(167)T(168)T(169)T(170)T(171)T(172)T(173)T(174)T(175)T(176)T(177)T(178)T(179)T(180)T(181)T(182)T(183)T(184)T(185)T(186)T(187)T(188)T(189)T(190)T(191) \
  T(192)T(193)T(194)T(195)T(196)T(197)T(198)T(199)T(200)T(201)T(202)T(203)T(204)T(205)T(206)T(207)T(208)T(209)T(210)T(211)T(212)T(213)T(214)T(215)T(216)T(217)T(218)T(219)T(220)T(221)T(222)T(223) \
  T(224)T(225)T(226)T(227)T(228)T(229)T(230)T(231)T(232)T(233)T(234)T(235)T(236)T(237)T(238)T(239)T(240)T(241)T(242)T(243)T(244)T(245)T(246)T(247)T(248)T(249)T(250)T(251)T(252)T(253)T(254)T(255)

template <std::size_t N>
struct valid_offsets_t {
  std::uint16_t count = 0;
  std::uint16_t offsets[N] = {};

  constexpr void append(std::size_t i) noexcept {
    offsets[count++] = static_cast<std::uint16_t>(i);
  }
};

// Avoid a helper template instantiation per scanned value.
template <typename E, enum_subtype S, std::size_t Size, int Min, std::size_t J>
MAGIC_ENUM_CONSTEVAL void valid_offsets(valid_offsets_t<Size>& vc) noexcept {
#define MAGIC_ENUM_V(O)                                                                   \
  if constexpr ((J + O) < Size) {                                                         \
    constexpr auto v = value<E, Min, S>(J + O);                                           \
    constexpr auto custom = customize::enum_name<E>(v);                                   \
    if constexpr (custom.first == customize::detail::customize_tag::default_tag) {        \
      if constexpr (n<v>().size_ != 0) {                                                  \
        vc.append(J + O);                                                                 \
      }                                                                                   \
    } else if constexpr (custom.first == customize::detail::customize_tag::custom_tag) {  \
      if constexpr (is_valid_custom_value_v<E, ualue<E, Min, S>(J + O)>) {                \
        static_assert(!custom.second.empty(),                                             \
                      "magic_enum::customize requires not empty string.");                \
        vc.append(J + O);                                                                 \
      }                                                                                   \
    }                                                                                     \
  }

  MAGIC_ENUM_FOR_EACH_256(MAGIC_ENUM_V)

  if constexpr ((J + 256) < Size) {
    valid_offsets<E, S, Size, Min, J + 256>(vc);
  }
#undef MAGIC_ENUM_V
}

template <typename E, enum_subtype S, std::size_t Size, int Min>
MAGIC_ENUM_CONSTEVAL auto valid_offsets() noexcept {
  valid_offsets_t<Size> vc;
  valid_offsets<E, S, Size, Min, 0>(vc);
  return vc;
}

template <typename E, enum_subtype S, std::size_t Size, int Min>
MAGIC_ENUM_CONSTEVAL auto values() noexcept {
  constexpr auto vc = valid_offsets<E, S, Size, Min>();
  if constexpr (vc.count > 0) {
    // A negative sign bit flag goes first, so values ascend as in the standard reflection table.
    constexpr std::size_t first = (S == enum_subtype::flags && cmp_less(ualue<E, Min, S>(vc.offsets[vc.count - 1]), 0)) ? 1 : 0;
    std::array<E, vc.count> values = {};
    for (std::size_t i = 0; i < vc.count; ++i) {
      values[(i + first) % vc.count] = value<E, Min, S>(vc.offsets[i]);
    }
    return values;
  } else {
    return std::array<E, 0>{};
  }
}

template <typename E, enum_subtype S, typename U = std::underlying_type_t<E>>
MAGIC_ENUM_CONSTEVAL auto values() noexcept {
#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
  std::array<E, reflection::table_size<E, S>()> values{};
  for (std::size_t i = 0; i < values.size(); ++i) {
    values[i] = reflection::table_v<E, S>.entries[i].value;
  }
  return values;
#else
  constexpr auto min = reflected_min<E, S>();
  constexpr auto max = reflected_max<E, S>();
  constexpr bool valid_range = !cmp_less(max, min);
  constexpr bool valid_bounds = !cmp_less(min, (std::numeric_limits<int>::min)()) && !cmp_less((std::numeric_limits<int>::max)(), max);
  // Unsigned subtraction avoids signed overflow.
  constexpr auto span = static_cast<unsigned int>(max) - static_cast<unsigned int>(min);
  constexpr bool valid_size = span < (std::numeric_limits<std::uint16_t>::max)();
  if constexpr (!valid_range) {
    static_assert(valid_range, "magic_enum::enum_range requires min <= max.");
    return std::array<E, 0>{};
  } else if constexpr (!valid_bounds) {
    static_assert(valid_bounds, "magic_enum::enum_range requires min and max to fit in int.");
    return std::array<E, 0>{};
  } else if constexpr (!valid_size) {
    static_assert(valid_size, "magic_enum::enum_range requires valid size.");
    return std::array<E, 0>{};
  } else {
    return values<E, S, std::size_t{span} + 1, static_cast<int>(min)>();
  }
#endif
}

template <typename E>
MAGIC_ENUM_CONSTEVAL enum_subtype subtype() noexcept {
  if constexpr (std::is_enum_v<E>) {
    static_assert(valid_is_flags_v<E>, "magic_enum::customize::enum_range requires is_flags to have bool type.");
    if constexpr (has_is_flags<E>::value) {
      return customize::enum_range<E>::is_flags ? enum_subtype::flags : enum_subtype::common;
    }
  }
  return enum_subtype::common;
}

template <typename E, typename D = std::decay_t<E>>
inline constexpr enum_subtype subtype_v = subtype<D>();

template <typename E, enum_subtype S>
inline constexpr auto values_v = values<E, S>();

template <typename E, enum_subtype S, typename D = std::decay_t<E>>
using values_t = decltype((values_v<D, S>));

// Reflection reads the size from the table so enum_count alone does not build values_v.
#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
template <typename E, enum_subtype S>
inline constexpr auto count_v = reflection::table_size<E, S>();
#else
template <typename E, enum_subtype S>
inline constexpr auto count_v = values_v<E, S>.size();
#endif

template <typename E, enum_subtype S, typename U = std::underlying_type_t<E>>
inline constexpr auto min_v = (count_v<E, S> > 0) ? static_cast<U>(values_v<E, S>.front()) : U{0};

template <typename E, enum_subtype S, typename U = std::underlying_type_t<E>>
inline constexpr auto max_v = (count_v<E, S> > 0) ? static_cast<U>(values_v<E, S>.back()) : U{0};

#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)

template <typename E, enum_subtype S>
MAGIC_ENUM_CONSTEVAL auto names() noexcept {
  std::array<string_view, count_v<E, S>> names{};
  for (std::size_t i = 0; i < names.size(); ++i) {
    names[i] = reflection::table_v<E, S>.entries[i].name.view();
  }
  return names;
}

template <typename E, enum_subtype S>
inline constexpr auto names_v = names<E, S>();

#else

template <typename E, enum_subtype S, std::size_t... J>
MAGIC_ENUM_CONSTEVAL auto names(std::index_sequence<J...>) noexcept {
  constexpr auto names = std::array<string_view, sizeof...(J)>{{enum_name_v<E, values_v<E, S>[J]>.str()...}};
  return names;
}

template <typename E, enum_subtype S>
inline constexpr auto names_v = names<E, S>(std::make_index_sequence<count_v<E, S>>{});

#endif

template <typename E, enum_subtype S, typename D = std::decay_t<E>>
using names_t = decltype((names_v<D, S>));

template <typename E, enum_subtype S, std::size_t... J>
MAGIC_ENUM_CONSTEVAL auto entries(std::index_sequence<J...>) noexcept {
  return std::array<std::pair<E, string_view>, sizeof...(J)>{{{values_v<E, S>[J], names_v<E, S>[J]}...}};
}

template <typename E, enum_subtype S>
inline constexpr auto entries_v = entries<E, S>(std::make_index_sequence<count_v<E, S>>{});

template <typename E, enum_subtype S, typename D = std::decay_t<E>>
using entries_t = decltype((entries_v<D, S>));

template <typename E, enum_subtype S, typename U = std::underlying_type_t<E>>
MAGIC_ENUM_CONSTEVAL bool is_sparse() noexcept {
  using V = make_unsigned_t<U>;
  if constexpr (count_v<E, S> == 0) {
    return false;
  } else if constexpr (S == enum_subtype::flags) {
    constexpr auto max = log2(static_cast<V>(max_v<E, S>));
    constexpr auto min = log2(static_cast<V>(min_v<E, S>));

    // A named zero has no bit position. A negative sign bit flag goes first, so min can exceed max.
    return static_cast<V>(min_v<E, S>) == V{0} || max < min || static_cast<std::size_t>(max - min) + 1 != count_v<E, S>;
  } else {
    // Dense lookup passes min to value() as an int. The unsigned span does not overflow on wide ranges.
    constexpr auto span = static_cast<V>(static_cast<V>(max_v<E, S>) - static_cast<V>(min_v<E, S>));

    return cmp_less(min_v<E, S>, (std::numeric_limits<int>::min)()) || cmp_less((std::numeric_limits<int>::max)(), max_v<E, S>) || static_cast<std::size_t>(span) != count_v<E, S> - 1;
  }
}

template <typename E, enum_subtype S = subtype_v<E>>
inline constexpr bool is_sparse_v = is_sparse<E, S>();

#if defined(MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM)
template <typename, enum_subtype>
struct is_reflected : std::true_type {};
#else
template <typename E, enum_subtype S, bool = std::is_enum_v<std::decay_t<E>>>
struct is_reflected : std::false_type {};

template <typename E, enum_subtype S>
struct is_reflected<E, S, true> : std::bool_constant<count_v<std::decay_t<E>, S> != 0> {};
#endif

template <typename E, enum_subtype S>
inline constexpr bool is_reflected_v = is_reflected<E, S>::value;

template <bool, typename R, typename E>
struct enable_if_enum {};

template <typename R, typename E>
struct enable_if_enum<true, R, E> {
  using type = R;
  static_assert(supported<R>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
  // An explicit subtype does not read subtype_v, so check is_flags here.
  static_assert(valid_is_flags_v<E>, "magic_enum::customize::enum_range requires is_flags to have bool type.");
};

template <typename T, typename R, typename BinaryPredicate = std::equal_to<>, typename D = std::decay_t<T>>
using enable_if_t = typename enable_if_enum<std::is_enum_v<D> && std::is_invocable_r_v<bool, BinaryPredicate&, const char_type&, const char_type&>, R, D>::type;

template <typename T, std::enable_if_t<std::is_enum_v<std::decay_t<T>>, int> = 0>
using enum_concept = T;

template <typename T, bool = std::is_enum_v<T>>
struct is_scoped_enum : std::false_type {};

template <typename T>
struct is_scoped_enum<T, true> : std::bool_constant<!std::is_convertible_v<T, std::underlying_type_t<T>>> {};

template <typename T>
struct is_unscoped_enum : std::bool_constant<std::is_enum_v<T> && !is_scoped_enum<T>::value> {};

template <typename T, bool = std::is_enum_v<T>>
struct is_flags_enum : std::false_type {};

template <typename T>
struct is_flags_enum<T, true> : std::bool_constant<subtype_v<T> == enum_subtype::flags> {};

template <typename T, bool = std::is_enum_v<std::decay_t<T>>>
struct underlying_type {};

template <typename T>
struct underlying_type<T, true> : std::underlying_type<std::decay_t<T>> {};

// Integer overloads take integral types and unscoped enums, which convert implicitly like integers.
template <typename T, bool = std::is_enum_v<T>>
struct is_integer_input : std::is_integral<T> {};

template <typename T>
struct is_integer_input<T, true> : std::is_convertible<T, std::underlying_type_t<T>> {};

// Integer arguments keep their own type, so a value outside the underlying type is rejected before narrowing.
template <typename E, typename I, typename U = typename underlying_type<E>::type>
constexpr bool in_underlying_range(I value) noexcept {
  if constexpr (std::is_enum_v<I>) {
    return in_underlying_range<E>(static_cast<std::underlying_type_t<I>>(value));
  } else {
    return !cmp_less(value, (std::numeric_limits<U>::min)()) && !cmp_less((std::numeric_limits<U>::max)(), value);
  }
}

template <typename F, typename T, T V>
constexpr decltype(auto) invoke_constant(F&& f, std::integral_constant<T, V>&& value) noexcept(std::is_nothrow_invocable_v<F, std::integral_constant<T, V>>) {
  if constexpr (std::is_member_function_pointer_v<std::decay_t<F>>) {
    return (std::move(value).*f)();
  } else if constexpr (std::is_member_object_pointer_v<std::decay_t<F>>) {
    return std::move(value).*f;
  } else {
    return std::forward<F>(f)(std::move(value));
  }
}

template <typename E>
using switch_t = std::conditional_t<(sizeof(std::underlying_type_t<E>) < sizeof(int)), int, std::underlying_type_t<E>>;

// Padding labels must be distinct from real cases and from each other.
// Only the last page needs padding, so labels may reuse values from earlier pages.
template <typename T, std::size_t Size>
MAGIC_ENUM_CONSTEVAL void fill_cases(std::array<T, Size>& cases, std::size_t count) noexcept {
  if (count == Size) {
    return;
  }
  T min = cases[0];
  T max = min;
  for (std::size_t j = 1; j < count; ++j) {
    min = cases[j] < min ? cases[j] : min;
    max = cases[j] > max ? cases[j] : max;
  }
  std::size_t i = count;
  for (; i < Size && max < (std::numeric_limits<T>::max)(); ++i) {
    cases[i] = ++max;
  }
  for (; i < Size && min > (std::numeric_limits<T>::min)(); ++i) {
    cases[i] = --min;
  }
  // Only reachable when the page has values at both limits of the type.
  for (T v = (std::numeric_limits<T>::min)(); i < Size; ++v) {
    bool used = false;
    for (std::size_t j = 0; j < count; ++j) {
      used = used || cases[j] == v;
    }
    if (!used) {
      cases[i++] = v;
    }
  }
}

template <typename E, enum_subtype S>
struct value_cases {
  using type = switch_t<E>;
  static constexpr std::size_t count = count_v<E, S>;

  template <std::size_t Page, std::size_t Size>
  static MAGIC_ENUM_CONSTEVAL auto page() noexcept {
    std::array<type, Size> cases{};
    std::size_t i = 0;
    for (; i < Size && Page + i < count; ++i) {
      cases[i] = static_cast<type>(values_v<E, S>[Page + i]);
    }
    fill_cases(cases, i);
    return cases;
  }
};

MAGIC_ENUM_CONSTEVAL std::size_t switch_page_size(std::size_t n) noexcept {
  return n <= 8 ? 8 : n <= 16 ? 16 : n <= 32 ? 32 : n <= 64 ? 64 : n <= 128 ? 128 : 256;
}

#define MAGIC_ENUM_INDEX_CASE(J)               \
  case cases[(J)]:                             \
    if constexpr ((J) + Page < Cases::count) { \
      return (J) + Page;                       \
    } else {                                   \
      break;                                   \
    }
#define MAGIC_ENUM_CASES_8(C, J) C(J) C(J + 1) C(J + 2) C(J + 3) C(J + 4) C(J + 5) C(J + 6) C(J + 7)
#define MAGIC_ENUM_CASES_16(C, J) MAGIC_ENUM_CASES_8(C, J) MAGIC_ENUM_CASES_8(C, J + 8)
#define MAGIC_ENUM_CASES_32(C, J) MAGIC_ENUM_CASES_16(C, J) MAGIC_ENUM_CASES_16(C, J + 16)
#define MAGIC_ENUM_CASES_64(C, J) MAGIC_ENUM_CASES_32(C, J) MAGIC_ENUM_CASES_32(C, J + 32)
#define MAGIC_ENUM_CASES_128(C, J) MAGIC_ENUM_CASES_64(C, J) MAGIC_ENUM_CASES_64(C, J + 64)
#define MAGIC_ENUM_CASES_256(C, J) MAGIC_ENUM_CASES_128(C, J) MAGIC_ENUM_CASES_128(C, J + 128)
#define MAGIC_ENUM_PAGE_SWITCH(C, V, N) \
  if constexpr (Size == (N)) {          \
    switch (V) {                        \
      MAGIC_ENUM_CASES_##N(C, 0)        \
      default:                          \
        break;                          \
    }                                   \
  }
#define MAGIC_ENUM_PAGE_SWITCHES(C, V) \
  MAGIC_ENUM_PAGE_SWITCH(C, V, 8)      \
  MAGIC_ENUM_PAGE_SWITCH(C, V, 16)     \
  MAGIC_ENUM_PAGE_SWITCH(C, V, 32)     \
  MAGIC_ENUM_PAGE_SWITCH(C, V, 64)     \
  MAGIC_ENUM_PAGE_SWITCH(C, V, 128)    \
  MAGIC_ENUM_PAGE_SWITCH(C, V, 256)

template <typename Cases, std::size_t Page = 0>
constexpr optional<std::size_t> index_switch(typename Cases::type value) noexcept {
  constexpr std::size_t n = Cases::count - Page;
  constexpr std::size_t Size = switch_page_size(n);
  constexpr auto cases = Cases::template page<Page, Size>();
  MAGIC_ENUM_PAGE_SWITCHES(MAGIC_ENUM_INDEX_CASE, value)
  if constexpr (n > Size) {
    return index_switch<Cases, Page + Size>(value);
  } else {
    return {};
  }
}

template <typename E, enum_subtype S>
constexpr optional<std::size_t> index_of(std::underlying_type_t<E> value) noexcept {
  if constexpr (count_v<E, S> == 0) {
    return {};
  } else if constexpr (is_sparse_v<E, S> || (S == enum_subtype::flags)) {
    return index_switch<value_cases<E, S>>(static_cast<switch_t<E>>(value));
  } else if (value >= min_v<E, S> && value <= max_v<E, S>) {
    return static_cast<std::size_t>(value - min_v<E, S>);
  } else {
    return {};
  }
}

#undef MAGIC_ENUM_INDEX_CASE

template <typename T = void>
inline constexpr auto default_result_type_lambda = []() noexcept(std::is_nothrow_default_constructible_v<T>) { return T{}; };

template <>
inline constexpr auto default_result_type_lambda<void> = []() noexcept {};

#if defined(MAGIC_ENUM_ENABLE_HASH)

struct constexpr_hash_t {
  // FNV-1a. Faster than CRC32 at run time on short names.
  constexpr std::uint32_t operator()(string_view value) const noexcept {
    std::uint32_t acc = 2166136261U;
    for (const auto c : value) {
      acc = (acc ^ static_cast<std::uint32_t>(c)) * 16777619U;
    }
    return acc;
  }

  // CRC32, used when FNV-1a hashes of the names collide.
  struct secondary_hash {
    static constexpr std::uint32_t crc_table[256] {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
    0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
    0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL, 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
    0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
    0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
    0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
    0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L, 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
    0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
    0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
    0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
    0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
    0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
    0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
    0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
    0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L, 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
    0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
    0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
    0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
    0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L, 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
    0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
    0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
    0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
    0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL, 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
    0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
    0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
    0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
    };
    constexpr std::uint32_t operator()(string_view value) const noexcept {
      auto crc = static_cast<std::uint32_t>(0xffffffffL);
      for (const auto c : value) {
        crc = (crc >> 8) ^ crc_table[(crc ^ static_cast<std::uint32_t>(c)) & 0xff];
      }
      return crc ^ 0xffffffffL;
    }
  };
};

template <typename Hash>
inline constexpr Hash hash_v{};

template <auto* GlobValues, typename Hash>
struct hash_cases {
  using type = std::invoke_result_t<Hash, typename std::decay_t<decltype(*GlobValues)>::value_type>;
  static_assert(std::is_integral_v<type> && !std::is_same_v<type, bool>);
  static constexpr std::size_t count = GlobValues->size();

  template <std::size_t Page, std::size_t Size>
  static MAGIC_ENUM_CONSTEVAL auto page() noexcept {
    std::array<type, Size> cases{};
    std::size_t i = 0;
    for (; i < Size && Page + i < count; ++i) {
      cases[i] = hash_v<Hash>((*GlobValues)[Page + i]);
    }
    fill_cases(cases, i);
    return cases;
  }
};

template <typename T>
MAGIC_ENUM_CONSTEVAL void hash_heap_sift_down(T* values, std::size_t root, std::size_t end) noexcept {
  while (root < end / 2) {
    auto child = root * 2 + 1;
    if (child + 1 < end && values[child] < values[child + 1]) {
      ++child;
    }
    if (!(values[root] < values[child])) {
      return;
    }
    auto tmp = values[root];
    values[root] = values[child];
    values[child] = tmp;
    root = child;
  }
}

template <typename T>
MAGIC_ENUM_CONSTEVAL void hash_heap_sort(T* values, std::size_t size) noexcept {
  for (auto root = size / 2; root > 0; --root) {
    hash_heap_sift_down(values, root - 1, size);
  }
  for (auto end = size; end > 1; --end) {
    auto tmp = values[0];
    values[0] = values[end - 1];
    values[end - 1] = tmp;
    hash_heap_sift_down(values, 0, end - 1);
  }
}

template <auto* Arr, typename Hash>
MAGIC_ENUM_CONSTEVAL bool has_unique_hashes() noexcept {
  std::array<std::invoke_result_t<Hash, std::decay_t<decltype((*Arr)[0])>>, Arr->size()> hashes{};
  for (std::size_t i = 0; i < hashes.size(); ++i) {
    hashes[i] = hash_v<Hash>((*Arr)[i]);
  }
  hash_heap_sort(hashes.data(), hashes.size());
  for (std::size_t i = 1; i < hashes.size(); ++i) {
    if (hashes[i - 1] == hashes[i]) {
      return false;
    }
  }
  return true;
}

template <auto* Arr, typename Hash>
inline constexpr bool has_unique_hashes_v = has_unique_hashes<Arr, Hash>();

template <auto* Arr, typename Hash, bool Enabled = true>
MAGIC_ENUM_CONSTEVAL bool has_usable_hash() noexcept {
  if constexpr (!Enabled) {
    return false;
  } else if constexpr (has_unique_hashes_v<Arr, Hash>) {
    return true;
  } else {
    return has_unique_hashes_v<Arr, typename Hash::secondary_hash>;
  }
}

// Keep name comparison in each case so its length and characters remain compile-time constants.
#define MAGIC_ENUM_HASH_CASE(J)                                                              \
  case cases[(J)]:                                                                           \
    if constexpr ((J) + Page < size) {                                                       \
      if (pred(values[(J) + Page], searched)) {                                              \
        return static_cast<result_t>(std::forward<Lambda>(lambda)(std::size_t{(J) + Page})); \
      }                                                                                      \
    }                                                                                        \
    break;

template <auto* GlobValues, std::size_t Page, typename Hash, typename BinaryPredicate, typename Lambda, typename Searched, typename ResultGetterType>
constexpr decltype(auto) hash_switch_page(Lambda&& lambda, Searched searched, typename hash_cases<GlobValues, Hash>::type searched_hash, ResultGetterType&& def, BinaryPredicate&& pred) {
  using result_t = std::invoke_result_t<ResultGetterType>;
  constexpr const auto& values = *GlobValues;
  constexpr std::size_t size = values.size();
  constexpr std::size_t Size = switch_page_size(size - Page);
  constexpr auto cases = hash_cases<GlobValues, Hash>::template page<Page, Size>();
  MAGIC_ENUM_PAGE_SWITCHES(MAGIC_ENUM_HASH_CASE, searched_hash)
  if constexpr (size > Page + Size) {
    return hash_switch_page<GlobValues, Page + Size, Hash>(std::forward<Lambda>(lambda), searched, searched_hash, std::forward<ResultGetterType>(def), std::forward<BinaryPredicate>(pred));
  } else {
    return def();
  }
}

#undef MAGIC_ENUM_HASH_CASE

template <auto* GlobValues,
          typename Hash = constexpr_hash_t,
          typename BinaryPredicate = std::equal_to<>,
          typename Lambda,
          typename Searched,
          typename ResultGetterType>
constexpr decltype(auto) hash_switch(
    Lambda&& lambda,
    Searched searched,
    ResultGetterType&& def,
    BinaryPredicate&& pred = {}) {
  using hash_t = std::conditional_t<has_unique_hashes_v<GlobValues, Hash>, Hash, typename Hash::secondary_hash>;
  static_assert(has_unique_hashes_v<GlobValues, hash_t>, "magic_enum::detail::hash_switch duplicated hash found, please report it: https://github.com/Neargye/magic_enum/issues.");
  const auto searched_hash = hash_v<hash_t>(searched);
  return hash_switch_page<GlobValues, 0, hash_t>(std::forward<Lambda>(lambda), searched, searched_hash, std::forward<ResultGetterType>(def), std::forward<BinaryPredicate>(pred));
}

#endif

#undef MAGIC_ENUM_PAGE_SWITCHES
#undef MAGIC_ENUM_PAGE_SWITCH
#undef MAGIC_ENUM_CASES_256
#undef MAGIC_ENUM_CASES_128
#undef MAGIC_ENUM_CASES_64
#undef MAGIC_ENUM_CASES_32
#undef MAGIC_ENUM_CASES_16
#undef MAGIC_ENUM_CASES_8

} // namespace magic_enum::detail

// Checks is magic_enum supported compiler.
inline constexpr bool is_magic_enum_supported = detail::supported<void>::value;

template <typename T>
using Enum = detail::enum_concept<T>;

// Identifies unscoped enum types.
template <typename T>
struct is_unscoped_enum : detail::is_unscoped_enum<T> {};

template <typename T>
inline constexpr bool is_unscoped_enum_v = is_unscoped_enum<T>::value;

// Identifies scoped enum types.
template <typename T>
struct is_scoped_enum : detail::is_scoped_enum<T> {};

template <typename T>
inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;

// Identifies flag enum types (i.e., enum_range<T>::is_flags == true).
template <typename T>
struct is_flags_enum : detail::is_flags_enum<T> {};

template <typename T>
inline constexpr bool is_flags_v = is_flags_enum<T>::value;

// If T is a complete enumeration type, provides a member typedef type that names the underlying type of T.
// Otherwise, if T is not an enumeration type, there is no member type. Otherwise (T is an incomplete enumeration type), the program is ill-formed.
template <typename T>
struct underlying_type : detail::underlying_type<T> {};

template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

template <auto V>
using enum_constant = detail::enum_constant<V>;

// Returns type name of enum.
template <typename E>
[[nodiscard]] constexpr auto enum_type_name() noexcept -> detail::enable_if_t<E, string_view> {
  constexpr string_view name = detail::type_name_v<std::decay_t<E>>.str();
  static_assert(!name.empty(), "magic_enum::enum_type_name enum type does not have a name.");

  return name;
}

// Returns the number of reflected enum values.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_count() noexcept -> detail::enable_if_t<E, std::size_t> {
  return detail::count_v<std::decay_t<E>, S>;
}

// Returns minimum reflected enum value.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_min() noexcept -> detail::enable_if_t<E, std::decay_t<E>> {
  using D = std::decay_t<E>;
  static_assert(detail::count_v<D, S> > 0, "magic_enum::enum_min requires at least one reflected enum value.");

  return detail::values_v<D, S>.front();
}

// Returns maximum reflected enum value.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_max() noexcept -> detail::enable_if_t<E, std::decay_t<E>> {
  using D = std::decay_t<E>;
  static_assert(detail::count_v<D, S> > 0, "magic_enum::enum_max requires at least one reflected enum value.");

  return detail::values_v<D, S>.back();
}

// Returns enum value at specified index.
// No bounds checking is performed: the behavior is undefined if index >= number of enum values.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_value(std::size_t index) noexcept -> detail::enable_if_t<E, std::decay_t<E>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_value requires a non-empty reflected enum.");

  if constexpr (detail::is_sparse_v<D, S>) {
    return MAGIC_ENUM_ASSERT(index < detail::count_v<D, S>), detail::values_v<D, S>[index];
  } else if constexpr (S == detail::enum_subtype::flags) {
    using V = detail::make_unsigned_t<underlying_type_t<D>>;
    constexpr auto min = detail::log2(static_cast<V>(detail::min_v<D, S>));

    return MAGIC_ENUM_ASSERT(index < detail::count_v<D, S>), detail::value<D, min, S>(index);
  } else {
    constexpr auto min = detail::min_v<D, S>;

    return MAGIC_ENUM_ASSERT(index < detail::count_v<D, S>), detail::value<D, min, S>(index);
  }
}

// Returns enum value at specified index.
template <typename E, std::size_t J, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_value() noexcept -> detail::enable_if_t<E, std::decay_t<E>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_value requires a non-empty reflected enum.");
  static_assert(J < detail::count_v<D, S>, "magic_enum::enum_value out of range.");

  return enum_value<D, S>(J);
}

// Returns std::array with enum values, sorted by enum value.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_values() noexcept -> detail::enable_if_t<E, detail::values_t<E, S>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_values requires a non-empty reflected enum.");

  return detail::values_v<D, S>;
}

// Returns integer value from enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_integer(E value) noexcept -> underlying_type_t<E> {
  return static_cast<underlying_type_t<E>>(value);
}

// Returns underlying value from enum value.
template <typename E>
[[nodiscard]] constexpr auto enum_underlying(E value) noexcept -> underlying_type_t<E> {
  return static_cast<underlying_type_t<E>>(value);
}

// Returns the index of the enum value in enum_values, or an empty optional.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_index(E value) noexcept -> detail::enable_if_t<E, optional<std::size_t>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_index requires a non-empty reflected enum.");

  return detail::index_of<D, S>(static_cast<underlying_type_t<D>>(value));
}

// Returns the index of the enum value in enum_values, or an empty optional.
template <detail::enum_subtype S, typename E>
[[nodiscard]] constexpr auto enum_index(E value) noexcept -> detail::enable_if_t<E, optional<std::size_t>> {
  return enum_index<std::decay_t<E>, S>(value);
}

// Returns index in enum values from compile-time enum value.
template <auto V, detail::enum_subtype S = detail::subtype_v<decltype(V)>>
[[nodiscard]] constexpr auto enum_index() noexcept -> detail::enable_if_t<decltype(V), std::size_t> {
  using D = std::decay_t<decltype(V)>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_index requires a non-empty reflected enum.");
  constexpr auto index = enum_index<D, S>(V);
  static_assert(index, "magic_enum::enum_index enum value has no index.");

  return *index;
}

// Returns name from compile-time enum value.
// Compiles faster than enum_name(value) and is not restricted by enum_range.
template <auto V>
[[nodiscard]] constexpr auto enum_name() noexcept -> detail::enable_if_t<decltype(V), string_view> {
  constexpr string_view name = detail::enum_name_v<std::decay_t<decltype(V)>, V>.str();
  static_assert(!name.empty(), "magic_enum::enum_name enum value does not have a name.");

  return name;
}

// Returns the reflected name of the enum value, or an empty string.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_name(E value) noexcept -> detail::enable_if_t<E, string_view> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_name requires a non-empty reflected enum.");

  if (const auto i = enum_index<D, S>(value)) {
    return detail::names_v<D, S>[*i];
  }
  return detail::static_str<0>{}.str();
}

// Returns the reflected name of the enum value, or an empty string.
template <detail::enum_subtype S, typename E>
[[nodiscard]] constexpr auto enum_name(E value) noexcept -> detail::enable_if_t<E, string_view> {
  return enum_name<std::decay_t<E>, S>(value);
}

// Returns std::array with names, sorted by enum value.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_names() noexcept -> detail::enable_if_t<E, detail::names_t<E, S>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_names requires a non-empty reflected enum.");

  return detail::names_v<D, S>;
}

// Returns std::array with pairs (value, name), sorted by enum value.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_entries() noexcept -> detail::enable_if_t<E, detail::entries_t<E, S>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_entries requires a non-empty reflected enum.");

  return detail::entries_v<D, S>;
}

// Allows you to write magic_enum::enum_cast<foo>("bar", magic_enum::case_insensitive);
inline constexpr auto case_insensitive = detail::case_insensitive<>{};

// Returns the enum value matching the integer, or an empty optional.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>, typename I, std::enable_if_t<detail::is_integer_input<I>::value, int> = 0>
[[nodiscard]] constexpr auto enum_cast(I value) noexcept -> detail::enable_if_t<E, optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_cast requires a non-empty reflected enum.");

  if (detail::in_underlying_range<D>(value)) {
    if (const auto u = static_cast<underlying_type_t<D>>(value); detail::index_of<D, S>(u)) {
      return static_cast<D>(u);
    }
  }
  return {}; // Invalid value or out of range.
}

// Returns the enum value matching the name, or an empty optional.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>, typename BinaryPredicate = std::equal_to<>, typename String = string_view, std::enable_if_t<detail::is_name_input_v<String>, int> = 0>
[[nodiscard]] constexpr auto enum_cast(String&& name, [[maybe_unused]] BinaryPredicate p = {}) noexcept(detail::is_nothrow_invocable_v<BinaryPredicate> && detail::is_nothrow_name_input_v<String>) -> detail::enable_if_t<E, optional<std::decay_t<E>>, BinaryPredicate> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_cast requires a non-empty reflected enum.");
  string_view value(std::forward<String>(name));

#if defined(MAGIC_ENUM_ENABLE_HASH)
  if constexpr (detail::has_usable_hash<&detail::names_v<D, S>, detail::constexpr_hash_t, (detail::count_v<D, S> > 0 && detail::is_default_predicate_v<BinaryPredicate>)>()) {
    return detail::hash_switch<&detail::names_v<D, S>>(
        [](std::size_t i) { return optional<D>{detail::values_v<D, S>[i]}; },
        value,
        detail::default_result_type_lambda<optional<D>>,
        [&p](string_view lhs, string_view rhs) { return detail::cmp_equal(lhs, rhs, p); });
  } else {
#endif
    // Skip the loop for empty enums, otherwise MSVC /analyze warns that it never runs.
    if constexpr (detail::count_v<D, S> > 0) {
      for (std::size_t i = 0; i < detail::count_v<D, S>; ++i) {
        if (detail::cmp_equal(value, detail::names_v<D, S>[i], p)) {
          return detail::values_v<D, S>[i];
        }
      }
    }
    return {}; // Invalid value or out of range.
#if defined(MAGIC_ENUM_ENABLE_HASH)
  }
#endif
}

// Returns true if enum contains specified value.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_contains(E value) noexcept -> detail::enable_if_t<E, bool> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_contains requires a non-empty reflected enum.");

  return static_cast<bool>(enum_cast<D, S>(static_cast<underlying_type_t<D>>(value)));
}

// Returns true if enum contains specified value.
template <detail::enum_subtype S, typename E>
[[nodiscard]] constexpr auto enum_contains(E value) noexcept -> detail::enable_if_t<E, bool> {
  return enum_contains<std::decay_t<E>, S>(value);
}

// Returns true if enum contains specified integer value.
// The enum value overload handles arguments of type E.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>, typename I, std::enable_if_t<detail::is_integer_input<I>::value && !std::is_same_v<I, std::decay_t<E>>, int> = 0>
[[nodiscard]] constexpr auto enum_contains(I value) noexcept -> detail::enable_if_t<E, bool> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_contains requires a non-empty reflected enum.");

  return static_cast<bool>(enum_cast<D, S>(value));
}

// Returns true if enum contains enumerator with specified name.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>, typename BinaryPredicate = std::equal_to<>, typename String = string_view, std::enable_if_t<detail::is_name_input_v<String>, int> = 0>
[[nodiscard]] constexpr auto enum_contains(String&& value, BinaryPredicate p = {}) noexcept(detail::is_nothrow_invocable_v<BinaryPredicate> && detail::is_nothrow_name_input_v<String>) -> detail::enable_if_t<E, bool, BinaryPredicate> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_contains requires a non-empty reflected enum.");

  return static_cast<bool>(enum_cast<D, S, BinaryPredicate&>(std::forward<String>(value), p));
}

// Checks the reflection domain, not enumerator membership.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>, typename I, std::enable_if_t<detail::is_integer_input<I>::value && !std::is_same_v<I, std::decay_t<E>>, int> = 0>
[[nodiscard]] constexpr auto enum_reflected(I value) noexcept -> detail::enable_if_t<E, bool> {
  using D = std::decay_t<E>;
  using U = underlying_type_t<D>;

  if (!detail::in_underlying_range<D>(value)) {
    return false;
  }

  [[maybe_unused]] const auto v = static_cast<U>(value);
  if constexpr (S == detail::enum_subtype::flags) {
    return detail::is_flag_candidate(v);
  } else {
#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
    // Standard reflection has no scan range.
    return true;
#else
    constexpr auto min = detail::reflected_min<D, S>();
    constexpr auto max = detail::reflected_max<D, S>();
    return !detail::cmp_less(v, min) && !detail::cmp_less(max, v);
#endif
  }
}

// Checks the reflection domain, not enumerator membership.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_reflected(E value) noexcept -> detail::enable_if_t<E, bool> {
  using D = std::decay_t<E>;

  return enum_reflected<D, S>(static_cast<underlying_type_t<D>>(value));
}

// Checks the reflection domain, not enumerator membership.
template <detail::enum_subtype S, typename E>
[[nodiscard]] constexpr auto enum_reflected(E value) noexcept -> detail::enable_if_t<E, bool> {
  return enum_reflected<std::decay_t<E>, S>(value);
}

template <bool AsFlags = true>
inline constexpr auto as_flags = AsFlags ? detail::enum_subtype::flags : detail::enum_subtype::common;

template <bool AsCommon = true>
inline constexpr auto as_common = AsCommon ? detail::enum_subtype::common : detail::enum_subtype::flags;

} // namespace magic_enum

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#undef MAGIC_ENUM_GET_ENUM_NAME_BUILTIN
#undef MAGIC_ENUM_GET_TYPE_NAME_BUILTIN
#undef MAGIC_ENUM_CALLING_CONVENTION
#undef MAGIC_ENUM_SIGNATURE
#undef MAGIC_ENUM_FOR_EACH_256
#undef MAGIC_ENUM_DETAIL_USE_STD_REFLECTION

#endif // NEARGYE_MAGIC_ENUM_HPP
