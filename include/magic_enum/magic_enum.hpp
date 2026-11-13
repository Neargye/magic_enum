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
#if !defined(MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION) && defined(__cpp_impl_reflection) && __cpp_impl_reflection >= 202506L
#  if !defined(MAGIC_ENUM_USE_STD_MODULE) && __has_include(<meta>)
#    include <meta>
#  endif
#  if defined(__cpp_lib_reflection) && __cpp_lib_reflection >= 202506L && defined(__cpp_lib_define_static) && __cpp_lib_define_static >= 202506L
#    define MAGIC_ENUM_DETAIL_USE_STD_REFLECTION 1
#  endif
#endif

#if defined(MAGIC_ENUM_REQUIRE_STD_REFLECTION)
#  if defined(MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION)
#    error "MAGIC_ENUM_REQUIRE_STD_REFLECTION and MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION cannot be used together."
#  elif !defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
#    error "MAGIC_ENUM_REQUIRE_STD_REFLECTION requires C++26 standard reflection support."
#  endif
#endif

#ifndef MAGIC_ENUM_USE_STD_MODULE
#  if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
#    include <algorithm>
#    include <vector>
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
#  if __has_warning("-Wenum-constexpr-conversion")
#    pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
#  endif
#  if __has_warning("-Wnrvo")
#    pragma clang diagnostic ignored "-Wnrvo" // Tables are built at compile time, where copy elision does not matter.
#  endif
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wconversion" // Scanning casts candidates outside the value range of unscoped enums without a fixed underlying type.
#  pragma GCC diagnostic ignored "-Wmaybe-uninitialized" // False positive on `return {};` for optional.
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 28020) // MSVC analyzer loses constexpr array bounds in template instantiations.
#  pragma warning(disable : 4514) // Unreferenced inline function has been removed.
#  pragma warning(disable : 4459 6244) // Local names can match global names in user code.
#  pragma warning(disable : 6262) // Large scan buffers are only used during constant evaluation.
#  pragma warning(disable : 6326) // Compile-time metadata intentionally compares constants.
#endif

#if defined(__RESHARPER__) && __RESHARPER__ >= 20250300
#  define MAGIC_ENUM_SUPPORTED 1
#  define MAGIC_ENUM_DETAIL_RESHARPER_ENUMS 1
#elif defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION) || defined(__clang__) && __clang_major__ >= 9 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1920
#  define MAGIC_ENUM_SUPPORTED 1
#endif

#if !defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION) && (defined(__NVCOMPILER) || defined(__NVCC__))
#  define MAGIC_ENUM_DETAIL_NVCC 1
#endif

// Lower bound of the default scan range, inclusive. Redefine it to change the default for all enums.
#if !defined(MAGIC_ENUM_RANGE_MIN)
#  define MAGIC_ENUM_RANGE_MIN -128
#endif

// Upper bound of the default scan range, inclusive. Redefine it to change the default for all enums.
#if !defined(MAGIC_ENUM_RANGE_MAX)
#  define MAGIC_ENUM_RANGE_MAX 127
#endif

// MSVC x86 can change the default calling convention, which would change __FUNCSIG__.
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

// Define MAGIC_ENUM_USING_ALIAS_OPTIONAL to use another optional type.
#if defined(MAGIC_ENUM_USING_ALIAS_OPTIONAL)
MAGIC_ENUM_USING_ALIAS_OPTIONAL
#else
using std::optional;
#endif

// Define MAGIC_ENUM_USING_ALIAS_STRING_VIEW to use another string_view type.
#if defined(MAGIC_ENUM_USING_ALIAS_STRING_VIEW)
MAGIC_ENUM_USING_ALIAS_STRING_VIEW
#else
using std::string_view;
#endif

// Define MAGIC_ENUM_USING_ALIAS_STRING to use another string type.
#if defined(MAGIC_ENUM_USING_ALIAS_STRING)
MAGIC_ENUM_USING_ALIAS_STRING
#else
using std::string;
#endif

using char_type = string_view::value_type;
static_assert(std::is_same_v<string_view::value_type, string::value_type>, "magic_enum requires MAGIC_ENUM_USING_ALIAS_STRING and MAGIC_ENUM_USING_ALIAS_STRING_VIEW to use the same character type.");
#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
static_assert(std::is_same_v<char_type, char> || std::is_same_v<char_type, wchar_t> || std::is_same_v<char_type, char8_t> || std::is_same_v<char_type, char16_t> || std::is_same_v<char_type, char32_t>,
              "magic_enum standard reflection requires string_view::value_type to be a standard character type; define MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION before including magic_enum.hpp for other character types.");
#endif
static_assert([] {
  if constexpr (std::is_same_v<char_type, wchar_t>) {
    constexpr const char     c[] =  "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789|";
    constexpr const wchar_t wc[] = L"abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789|";
    for (std::size_t i = 0; i < std::size(c); ++i) {
      if (c[i] != wc[i]) {
        return false;
      }
    }
  }
  return true;
}(), "magic_enum::customize wchar_t is not compatible with ASCII.");

namespace customize {

namespace detail {

template <typename Flags>
struct flag_setting {};

template <bool IsFlags>
struct flag_setting<std::bool_constant<IsFlags>> {
  static constexpr bool is_flags = IsFlags;
};

template <typename Flags = void, int Min = MAGIC_ENUM_RANGE_MIN, int Max = MAGIC_ENUM_RANGE_MAX, std::size_t PrefixLength = 0>
struct adl_info : flag_setting<Flags> {
  static constexpr int min = Min;
  static constexpr int max = Max;
  static constexpr std::size_t prefix_length = PrefixLength;

  constexpr adl_info() noexcept = default;

  // v0.9.* hooks may spell `adl_info_holder<false, ...>` as the return type of an `adl_info()` chain.
  template <typename F, std::enable_if_t<std::is_void_v<F> && std::is_same_v<Flags, std::false_type>, int> = 0>
  constexpr adl_info(adl_info<F, Min, Max, PrefixLength>) noexcept {}

  template <int NewMin, int NewMax>
  static constexpr adl_info<Flags, NewMin, NewMax, PrefixLength> minmax() { return {}; }

  template <bool IsFlags>
  static constexpr adl_info<std::bool_constant<IsFlags>, Min, Max, PrefixLength> flag() { return {}; }

  template <std::size_t NewPrefixLength>
  static constexpr adl_info<Flags, Min, Max, NewPrefixLength> prefix() { return {}; }
};

// Block ordinary lookup, including declarations preceding this header.
void magic_enum_define_range_adl();

template <typename E>
using adl_range_t = decltype(magic_enum_define_range_adl(E{}));

template <typename T, bool = std::is_class_v<T> && !std::is_final_v<T>>
struct adl_range : T {};

template <typename T>
struct adl_range<T, false> {
  static_assert(!std::is_same_v<T, T>, "magic_enum customization hook must return a non-final class by value.");
};

} // namespace magic_enum::customize::detail

template <bool IsFlags = false, int Min = MAGIC_ENUM_RANGE_MIN, int Max = MAGIC_ENUM_RANGE_MAX, std::size_t PrefixLength = 0>
using adl_info_holder = detail::adl_info<std::bool_constant<IsFlags>, Min, Max, PrefixLength>;

constexpr detail::adl_info<> adl_info() { return {}; }

// Compiler-specific reflection scans common enum values in [min, max]. Redefine MAGIC_ENUM_RANGE_MIN/MAX globally or specialize enum_range for a specific enum.
template <typename E, typename = void>
struct enum_range {
  static constexpr int min = MAGIC_ENUM_RANGE_MIN;
  static constexpr int max = MAGIC_ENUM_RANGE_MAX;
};

template <typename E>
struct enum_range<E, std::void_t<detail::adl_range_t<E>>> : detail::adl_range<detail::adl_range_t<E>> {};

namespace detail {

enum class customize_tag {
  default_tag,
  invalid_tag,
  custom_tag
};

} // namespace magic_enum::customize::detail

class customize_t {
 public:
  detail::customize_tag tag;
  string_view name;

  constexpr customize_t(string_view custom_name) : tag{detail::customize_tag::custom_tag}, name{custom_name} {}
  constexpr customize_t(const char_type* custom_name) : customize_t{string_view{custom_name}} {}
  constexpr customize_t(detail::customize_tag tag_value) : tag{tag_value}, name{} {}
};

inline constexpr auto default_tag = customize_t{detail::customize_tag::default_tag};
inline constexpr auto invalid_tag = customize_t{detail::customize_tag::invalid_tag};

// Specialize enum_name to rename or hide values of an enum.
template <typename E>
constexpr customize_t enum_name(E) noexcept {
  return default_tag;
}

// Specialize enum_type_name to rename an enum type.
template <typename E>
constexpr customize_t enum_type_name() noexcept {
  return default_tag;
}

} // namespace magic_enum::customize

namespace detail {

using customize::detail::customize_tag;

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

#if defined(__clang__) || defined(__NVCOMPILER) || defined(__NVCC__)
// These compilers define scoped enums of class templates lazily, see #164. Member lookup instantiates them before reflection.
template <typename E, typename = void>
struct enum_definition { using type = E; };

// Preserve E if the lookup happens to name an enumerator.
template <typename E>
struct enum_definition<E, std::void_t<decltype(E::magic_enum_detail_enum_definition)>> { using type = E; };

template <typename E>
using enum_definition_t = typename enum_definition<E>::type;
#else
template <typename E>
using enum_definition_t = E;
#endif

template <typename T, typename = void>
inline constexpr bool has_is_flags_v = false;

template <typename T>
inline constexpr bool has_is_flags_v<T, std::void_t<decltype(customize::enum_range<T>::is_flags)>> = true;

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
  for (const auto enumerator : enumerators_v<E>) {
    if (enum_value_equal(std::meta::extract<E>(enumerator), V)) {
      const auto identifier = std::meta::identifier_of(enumerator);
      return cname_ref{identifier.data(), identifier.size()}; // One name per value, the first declaration wins.
    }
  }
  return cname_ref{};
}

} // namespace magic_enum::detail::reflection

#endif

// Always null-terminated. The empty string points to static storage.
template <std::uint16_t N>
class static_str {
 public:
  constexpr static_str() noexcept : chars_{} {}

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
  auto ret = T{0};
  for (; value > T{1}; value >>= T{1}, ++ret) {}

  return ret;
}

// Zero or a single bit. Flag candidates need not have reflected names.
template <typename T>
constexpr bool is_flag_candidate(T value) noexcept {
  using V = make_unsigned_t<T>;
  const auto v = static_cast<V>(value);
  return (v & static_cast<V>(v - V{1})) == V{0};
}

// Drops everything up to the last ':' of a qualified name.
MAGIC_ENUM_CONSTEVAL cname_ref unqualified(cname_ref name) noexcept {
  for (std::size_t i = name.size_; i > 0; --i) {
    if (name.str_[i - 1] == ':') {
      return {name.str_ + i, name.size_ - i};
    }
  }
  return name;
}

#if defined(MAGIC_ENUM_DETAIL_RESHARPER_ENUMS)
MAGIC_ENUM_CONSTEVAL cname_ref resharper_name(const char* name) noexcept {
  return name != nullptr ? unqualified({name, std::char_traits<char>::length(name)}) : cname_ref{};
}
#endif

#if defined(MAGIC_ENUM_DETAIL_NVCC)
MAGIC_ENUM_CONSTEVAL cname_ref nvcc_signature_name(const char* str, std::size_t size) noexcept {
#  if defined(_MSC_VER)
  // The name is the only template argument, between the first '<' and `>(void)`.
  std::size_t i = 0;
  while (str[i++] != '<') {
  }
  std::size_t e = size - 1;
  while (str[e] != '(') {
    --e;
  }
  return {str + i, e - 1 - i};
#  else
  // The name follows the last '='.
  std::size_t i = size - 1;
  while (str[i - 1] != '=') {
    --i;
  }
  return {str + i + 1, size - i - 2};
#  endif
}
#endif

// Parsed outside n() to keep each instantiation small.
MAGIC_ENUM_CONSTEVAL cname_ref type_signature_name([[maybe_unused]] const char* str, [[maybe_unused]] std::size_t size) noexcept {
#if defined(MAGIC_ENUM_DETAIL_NVCC) && !defined(_MSC_VER)
  return nvcc_signature_name(str, size);
#elif defined(__clang__)
  return {str + 34, size - 35};
#elif defined(__GNUC__)
  return str[size - 1] == ']' ? cname_ref{str + 49, size - 50} : cname_ref{str + 37, size - 40};
#elif defined(_MSC_VER)
  // CLI/C++ workaround, see #284. A typedef name of an unnamed enum has no `enum ` prefix.
  const std::size_t i = str[35] == 'e' && str[36] == 'n' && str[37] == 'u' && str[38] == 'm' && str[39] == ' ' ? 40u : 35u;
  return {str + i, size - 16 - i};
#else
  return {};
#endif
}

// Unnamed enums print as `(unnamed enum at ...)`, `<unnamed enum>` or `<unnamed-type-x>` and have no name.
MAGIC_ENUM_CONSTEVAL cname_ref type_identifier(cname_ref name) noexcept {
  if (name.size_ == 0 || name.str_[name.size_ - 1] == ')' || name.str_[name.size_ - 1] == '>') {
    return {};
  }
  return unqualified(name);
}

MAGIC_ENUM_CONSTEVAL cname_ref enum_signature_name([[maybe_unused]] const char* str, [[maybe_unused]] std::size_t size) noexcept {
#if defined(MAGIC_ENUM_DETAIL_NVCC)
  const auto name = nvcc_signature_name(str, size);
  return name.str_[0] == '(' ? cname_ref{} : name;
#elif defined(__clang__)
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
#elif defined(MAGIC_ENUM_DETAIL_RESHARPER_ENUMS)
    constexpr auto name = __rscpp_type_name<E>();
    return resharper_name(name);
#else
    return type_identifier(type_signature_name(MAGIC_ENUM_SIGNATURE, sizeof(MAGIC_ENUM_SIGNATURE) - 1));
#endif
  } else {
    return cname_ref{}; // Unsupported compiler.
  }
}

template <typename E>
MAGIC_ENUM_CONSTEVAL auto type_name() noexcept {
  [[maybe_unused]] constexpr auto custom = customize::enum_type_name<E>();
  if constexpr (custom.tag == customize_tag::custom_tag) {
    constexpr auto name = custom.name;
    static_assert(!name.empty(), "magic_enum::customize::enum_type_name requires a non-empty name.");
    return static_str<name.size()>{name};
  } else if constexpr (custom.tag == customize_tag::invalid_tag) {
    return static_str<0>{};
  } else {
    constexpr auto name = n<E>();
    return static_str<name.size_>{name.str_};
  }
}

template <typename E>
inline constexpr auto type_name_v = type_name<E>();

#if defined(MAGIC_ENUM_DETAIL_NVCC)
// nvcc prints a value as a cast when its type is named through an alias. A deduced return type drops the alias.
template <typename E>
constexpr auto nvcc_unaliased() noexcept {
  return E{};
}

template <auto V>
MAGIC_ENUM_CONSTEVAL cname_ref nvcc_name() noexcept {
  return enum_signature_name(MAGIC_ENUM_SIGNATURE, sizeof(MAGIC_ENUM_SIGNATURE) - 1);
}
#endif

template <auto V>
MAGIC_ENUM_CONSTEVAL auto MAGIC_ENUM_CALLING_CONVENTION n() noexcept {
  if constexpr (supported<decltype(V)>::value) {
#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
    return reflection::enum_name<V>();
#elif defined(MAGIC_ENUM_DETAIL_RESHARPER_ENUMS)
    constexpr auto name = __rscpp_enumerator_name(V);
    return resharper_name(name);
#elif defined(MAGIC_ENUM_DETAIL_NVCC)
    return unqualified(nvcc_name<static_cast<decltype(nvcc_unaliased<decltype(V)>())>(static_cast<std::underlying_type_t<decltype(V)>>(V))>());
#else
    return unqualified(enum_signature_name(MAGIC_ENUM_SIGNATURE, sizeof(MAGIC_ENUM_SIGNATURE) - 1));
#endif
  } else {
    return cname_ref{}; // Unsupported compiler.
  }
}

// One signature lists a whole block of values, so the scan instantiates one function per block instead of one per value.
template <auto... V>
MAGIC_ENUM_CONSTEVAL auto MAGIC_ENUM_CALLING_CONVENTION nb() noexcept {
  return cname_ref{MAGIC_ENUM_SIGNATURE, sizeof(MAGIC_ENUM_SIGNATURE) - 1};
}

template <std::size_t N>
struct signature_names_t {
  cname_ref names[N] = {};
  // Number of named values.
  std::size_t count = 0;
  bool valid = false;
};

MAGIC_ENUM_CONSTEVAL bool same_prefix(const char* str, std::size_t size, std::size_t a, std::size_t b, std::size_t n) noexcept {
  if (b + n > size) {
    return false;
  }
#if defined(__clang__) || defined(_MSC_VER) || defined(__GNUC__) && __GNUC__ >= 13
  return __builtin_memcmp(str + a, str + b, n) == 0;
#else
  for (std::size_t i = 0; i < n; ++i) {
    if (str[a + i] != str[b + i]) {
      return false;
    }
  }
  return true;
#endif
}

// GCC prints the values of nb() as `{A, (E)1}`, Clang as `<A, (E)1>`, MSVC as `<A,(enum E)0x1>`.
// Qualifiers can contain templates, lambdas and anonymous namespaces, so only top-level commas separate values.
// Any other format leaves the result invalid, and the scan falls back to n() per value.
template <std::size_t N>
MAGIC_ENUM_CONSTEVAL auto signature_names(cname_ref sig) noexcept {
  signature_names_t<N> result;
  const char* const str = sig.str_;
  const std::size_t size = sig.size_;
  std::size_t i = 0;
  while (i < size && str[i] != '<' && str[i] != '{') {
    ++i;
  }
  // Values of one enum repeat the same cast and qualifier, so a known prefix is compared instead of parsed.
  std::size_t cast = 0, cast_size = 0, qualifier = 0, qualifier_size = 0;
  for (std::size_t k = 0; k < N; ++k) {
    auto begin = ++i;
    if (begin < size && str[begin] == ' ') {
      begin = ++i;
    }
    // An unnamed value is the cast followed by a number.
#if defined(__clang__)
    if (cast_size != 0 && k + 1 < N && begin + cast_size < size && str[begin] == '(' && str[begin + cast_size - 1] == ')' &&
        (str[begin + cast_size] == '-' || (str[begin + cast_size] >= '0' && str[begin + cast_size] <= '9'))) {
#else
    // GCC and MSVC do not start a name with a parenthesis.
    if (cast_size != 0 && k + 1 < N && begin + cast_size < size && str[begin] == '(') {
#endif
#if defined(__clang__) || defined(_MSC_VER)
      const char* const comma = __builtin_char_memchr(str + begin + cast_size, ',', size - begin - cast_size);
      if (comma == nullptr) {
        return result;
      }
      i = static_cast<std::size_t>(comma - str);
#else
      for (i = begin + cast_size; i < size && str[i] != ','; ++i) {
      }
      if (i >= size) {
        return result;
      }
#endif
      continue;
    }
    auto name = begin;
    std::size_t known = 0;
    if (cast_size != 0 && same_prefix(str, size, cast, begin, cast_size)) {
      known = cast_size;
    } else if (qualifier_size != 0 && same_prefix(str, size, qualifier, begin, qualifier_size)) {
      known = qualifier_size;
      name = begin + known;
    }
    if (known != 0) {
      i = begin + known;
#if defined(__clang__) || defined(_MSC_VER)
      if (k + 1 < N) {
        const char* const comma = __builtin_char_memchr(str + i, ',', size - i);
        i = comma ? static_cast<std::size_t>(comma - str) : size;
      } else
#endif
      while (i < size && str[i] != ',' && str[i] != '>' && str[i] != '}') {
        ++i;
      }
    } else {
      std::size_t close = 0;
      for (int depth = 0; i < size; ++i) {
        const char c = str[i];
        if (c == '(' || c == '<' || c == '{' || c == '[') {
          ++depth;
        } else if (c == ')' || c == '>' || c == '}' || c == ']') {
          if (depth == 0) {
            break;
          }
          if (--depth == 0 && close == 0) {
            close = i + 1;
          }
        } else if (depth == 0 && c == ',') {
          break;
        } else if (depth == 0 && c == ':') {
          name = i + 1;
        }
      }
      if (name != begin) {
        qualifier = begin;
        qualifier_size = name - begin;
      } else if (str[begin] == '(' && close != 0) {
        cast = begin;
        cast_size = close - begin;
      }
    }
    if (i >= size || name >= i || (str[i] == ',') != (k + 1 < N)) {
      return result;
    }
    // Unnamed values are casts like `(E)1`, `(enum E)0x1` or `((anonymous namespace)::E)1`.
    // A top-level `::` means a name. MSVC prints a value of an enum local to `operator-` as `-::E::A`.
    const char c = str[begin];
    if (name != begin || (c != '(' && c != '-' && (c < '0' || c > '9'))) {
      result.names[k] = {str + name, i - name};
      ++result.count;
    }
  }
  result.valid = true;
  return result;
}

template <typename E, E V>
MAGIC_ENUM_CONSTEVAL auto enum_name() noexcept {
  [[maybe_unused]] constexpr auto custom = customize::enum_name<E>(V);
  if constexpr (custom.tag == customize_tag::custom_tag) {
    static_assert(n<V>().size_ != 0, "magic_enum::customize::enum_name cannot add an undeclared enum value.");
    constexpr auto name = custom.name;
    static_assert(!name.empty(), "magic_enum::customize::enum_name requires a non-empty name. Return invalid_tag to exclude a value.");
    return static_str<name.size()>{name};
  } else if constexpr (custom.tag == customize_tag::invalid_tag) {
    return static_str<0>{};
  } else {
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
  }
}

template <typename E, E V>
inline constexpr auto enum_name_v = enum_name<enum_definition_t<E>, V>();

// Tables skip undeclared values, so only a custom name for zero is checked, as the most common mistake.
template <typename E>
MAGIC_ENUM_CONSTEVAL bool valid_custom_zero() noexcept {
  if constexpr (customize::enum_name<E>(E{}).tag == customize_tag::custom_tag) {
    return n<E{}>().size_ != 0;
  } else {
    return true;
  }
}

// Reject custom values that bit_cast accepts but constant static_cast rejects.
template <typename E, auto V, typename = void>
inline constexpr bool is_valid_custom_value_v = false;

template <typename E, auto V>
inline constexpr bool is_valid_custom_value_v<E, V, std::void_t<std::integral_constant<E, static_cast<E>(V)>>> = true;

enum class enum_subtype {
  common,
  flags
};

namespace adl_flags {

struct lookup_blocker {};
void operator|(lookup_blocker, lookup_blocker);
void operator&(lookup_blocker, lookup_blocker);

// Direct calls avoid MSVC's deleted-operator expression bug and exclude built-in operators.
template <typename E, typename = void>
struct has_and : std::false_type {};

template <typename E>
struct has_and<E, std::enable_if_t<std::is_same_v<decltype(operator&(E{}, E{})), E>>> : std::true_type {};

template <typename E, typename = void>
struct has_or : std::false_type {};

template <typename E>
struct has_or<E, std::enable_if_t<std::is_same_v<decltype(operator|(E{}, E{})), E>>> : std::true_type {};

// `|` is checked only after `&`, so generic pipe operators with deduced return types are not instantiated for enums without `&`.
template <typename E>
struct has_operators : std::conjunction<has_and<E>, has_or<E>> {};

} // namespace magic_enum::detail::adl_flags

template <typename E>
MAGIC_ENUM_CONSTEVAL enum_subtype subtype() noexcept {
  if constexpr (std::is_enum_v<E>) {
    static_assert(valid_is_flags_v<E>, "magic_enum::customize::enum_range requires is_flags to have bool type.");
    if constexpr (has_is_flags_v<E>) {
      return customize::enum_range<E>::is_flags ? enum_subtype::flags : enum_subtype::common;
#if !defined(MAGIC_ENUM_NO_AUTO_IS_FLAGS)
    } else if constexpr (adl_flags::has_operators<E>::value) {
      return enum_subtype::flags;
#endif
    }
  }
  return enum_subtype::common;
}

template <typename E, typename D = std::decay_t<E>>
inline constexpr enum_subtype subtype_v = subtype<D>();

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
  bool names_valid = true;
  bool sorted = true;
};

// Values that flag tables hold. MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS leaves zero out, but it stays a valid mask.
template <typename T>
consteval bool is_flag_value(T value) noexcept {
#if defined(MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS)
  return value != T{0} && is_flag_candidate(value);
#else
  return is_flag_candidate(value);
#endif
}

template <typename E, enum_subtype S, typename U = std::underlying_type_t<E>>
consteval auto declared_data() noexcept {
  table<E, enumerators_v<E>.size()> result{};
  auto* const entries = result.entries.data();
  constexpr auto prefix = prefix_length_v<E>;
  [[maybe_unused]] std::size_t declaration_index = 0;

  // A regular loop avoids one template instantiation per enumerator.
  for (const auto enumerator : enumerators_v<E>) {
    const auto value = std::meta::extract<E>(enumerator);
    if (S == enum_subtype::common || is_flag_value(static_cast<U>(value))) {
      const auto custom = customize::enum_name<E>(value);
      if (custom.tag != customize_tag::invalid_tag) {
        name_ref name{};
        if (custom.tag == customize_tag::custom_tag) {
          result.names_valid = result.names_valid && !custom.name.empty();
          name = {std::define_static_string(std::span{custom.name.data(), custom.name.size()}), custom.name.size()};
        } else if (const auto identifier = std::meta::identifier_of(enumerator); valid_prefix_length(prefix, identifier.size())) {
          if constexpr (std::is_same_v<char_type, char>) {
            name = {std::define_static_string(identifier.substr(prefix)), identifier.size() - prefix};
          } else {
            const std::vector<char_type> chars(identifier.begin() + prefix, identifier.end());
            name = {std::define_static_string(chars), chars.size()};
          }
        }
        entries[result.size++] = entry<E>{value, name, declaration_index};
        if (result.size > 1) {
          result.sorted = result.sorted && static_cast<U>(entries[result.size - 2].value) <= static_cast<U>(entries[result.size - 1].value);
        }
      }
    }
    ++declaration_index;
  }

  return result;
}

template <typename E, enum_subtype S, typename U = std::underlying_type_t<E>>
consteval auto canonical_data() noexcept {
  constexpr auto declared = declared_data<E, S>();
  auto result = declared;
  auto* const entries = result.entries.data();
  // Avoid instantiating sort for enums already declared in value order.
  if constexpr (!declared.sorted) {
    const auto less = [](const auto& lhs, const auto& rhs) {
      const auto lhs_value = static_cast<U>(lhs.value);
      const auto rhs_value = static_cast<U>(rhs.value);
      return lhs_value != rhs_value ? lhs_value < rhs_value : lhs.declaration_index < rhs.declaration_index;
    };
    std::sort(entries, entries + result.size, less);
  }

  // Reflected names are not empty, so an empty name marks an invalid prefix on the retained first declaration.
  std::size_t unique_size = 0;
  for (std::size_t i = 0; i < result.size; ++i) {
    if (unique_size == 0 || !enum_value_equal(entries[unique_size - 1].value, entries[i].value)) {
      result.prefixes_valid = result.prefixes_valid && entries[i].name.size != 0;
      if (unique_size != i) {
        entries[unique_size] = entries[i];
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
  static_assert(table_v<E, S>.names_valid, "magic_enum::customize::enum_name requires a non-empty name. Return invalid_tag to exclude a value.");
  static_assert(valid_custom_zero<E>(), "magic_enum::customize::enum_name cannot add an undeclared enum value.");
  static_assert(table_v<E, S>.prefixes_valid, "magic_enum::customize requires valid prefix length.");
  return table_v<E, S>.size;
}

} // namespace magic_enum::detail::reflection

#endif

template <typename E, int O, enum_subtype S, typename U = std::underlying_type_t<E>>
constexpr U underlying_value(std::size_t i) noexcept {
  if constexpr (S == enum_subtype::flags) {
    using V = make_unsigned_t<U>;
    // Position -1 is zero, so the scan finds a named zero like a bit.
    const auto bit = static_cast<int>(i) + O;
    if constexpr (std::is_same_v<U, bool>) {
      return bit >= 0;
    } else {
      return bit < 0 ? U{0} : static_cast<U>(V{1} << static_cast<V>(bit));
    }
  } else {
    return static_cast<U>(static_cast<int>(i) + O);
  }
}

template <typename E, typename U = std::underlying_type_t<E>>
constexpr E to_enum(U u) noexcept {
#if defined(__clang__) && __clang_major__ >= 16
  // CWG1766: Clang 16+ rejects a constant static_cast to a value outside the range of the values of an enumeration.
  // https://reviews.llvm.org/D130058, https://reviews.llvm.org/D131307
  return __builtin_bit_cast(E, u);
#else
  return static_cast<E>(u);
#endif
}

template <typename E, int O, enum_subtype S>
constexpr E value(std::size_t i) noexcept {
  return to_enum<E>(underlying_value<E, O, S>(i));
}

template <typename E, typename = void>
inline constexpr bool has_fixed_underlying_v = false;

template <typename E>
inline constexpr bool has_fixed_underlying_v<E, std::void_t<decltype(E{0})>> = true;

// nvcc crashes on long batch signatures.
template <typename E>
MAGIC_ENUM_CONSTEVAL bool batch_supported() noexcept {
#if defined(MAGIC_ENUM_DETAIL_RESHARPER_ENUMS) || defined(MAGIC_ENUM_DETAIL_NVCC)
  return false;
#else
  return true;
#endif
}

// The range of an enumeration without a fixed underlying type is [0, 2^K - 1] or [-2^K, 2^K - 1].
template <typename E, int K = 1, typename U = std::underlying_type_t<E>>
MAGIC_ENUM_CONSTEVAL U unfixed_max() noexcept {
  if constexpr (K == std::numeric_limits<U>::digits) {
    return is_valid_custom_value_v<E, (std::numeric_limits<U>::max)()> ? (std::numeric_limits<U>::max)() : static_cast<U>((U{1} << (K - 1)) - 1);
  } else if constexpr (is_valid_custom_value_v<E, static_cast<U>((U{1} << K) - 1)>) {
    return unfixed_max<E, K + 1>();
  } else {
    return static_cast<U>((U{1} << (K - 1)) - 1);
  }
}

// Whether nb() accepts the value.
// Clang 20+ rejects a value outside the range of an enum without a fixed underlying type in nb(), see CWG1766.
// Older versions report -Wenum-constexpr-conversion, which is disabled above.
template <typename E, typename U = std::underlying_type_t<E>>
MAGIC_ENUM_CONSTEVAL bool batch_castable([[maybe_unused]] U u) noexcept {
#if defined(__clang__) && __clang_major__ >= 20
  if constexpr (!has_fixed_underlying_v<E>) {
    constexpr U max = unfixed_max<E>();
    if constexpr (std::is_signed_v<U> && is_valid_custom_value_v<E, static_cast<U>(-1)>) {
      return !cmp_less(u, -max - 1) && !cmp_less(max, u);
    } else {
      return !cmp_less(u, 0) && !cmp_less(max, u);
    }
  }
#endif
  return true;
}

// Zero is in the range of every enumeration, so it replaces candidates rejected by the batch scan.
template <typename E, int O, enum_subtype S>
MAGIC_ENUM_CONSTEVAL E batch_value(std::size_t i) noexcept {
  return batch_castable<E>(underlying_value<E, O, S>(i)) ? value<E, O, S>(i) : E{};
}

// Keep the type of a custom bound until values() checks that it fits in int.
template <typename E, enum_subtype S, typename U = std::underlying_type_t<E>>
MAGIC_ENUM_CONSTEVAL auto reflected_min() noexcept {
  if constexpr (S == enum_subtype::flags) {
#if defined(MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS)
    return 0;
#else
    // Zero before bit 0, see underlying_value.
    return -1;
#endif
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

#define MAGIC_ENUM_CASES_8(C, J) C(J) C(J + 1) C(J + 2) C(J + 3) C(J + 4) C(J + 5) C(J + 6) C(J + 7)
#define MAGIC_ENUM_CASES_16(C, J) MAGIC_ENUM_CASES_8(C, J) MAGIC_ENUM_CASES_8(C, J + 8)
#define MAGIC_ENUM_CASES_32(C, J) MAGIC_ENUM_CASES_16(C, J) MAGIC_ENUM_CASES_16(C, J + 16)
#define MAGIC_ENUM_CASES_64(C, J) MAGIC_ENUM_CASES_32(C, J) MAGIC_ENUM_CASES_32(C, J + 32)
#define MAGIC_ENUM_CASES_128(C, J) MAGIC_ENUM_CASES_64(C, J) MAGIC_ENUM_CASES_64(C, J + 64)
#define MAGIC_ENUM_CASES_256(C, J) MAGIC_ENUM_CASES_128(C, J) MAGIC_ENUM_CASES_128(C, J + 128)

template <std::size_t N>
struct valid_offsets_t {
  std::uint16_t count = 0;
  std::uint16_t offsets[N] = {};
  // Null for a custom name.
  cname_ref names[N] = {};
  bool names_valid = true;

  constexpr void append(std::size_t i, cname_ref name = {}) noexcept {
    names[count] = name;
    offsets[count++] = static_cast<std::uint16_t>(i);
  }
};

// Customization renames or hides a named value, but cannot add one.
template <typename E, typename N>
MAGIC_ENUM_CONSTEVAL void append_named(N& offsets, std::size_t i, E value, cname_ref name) noexcept {
  const auto custom = customize::enum_name<E>(value);
  if (custom.tag == customize_tag::custom_tag) {
    offsets.names_valid = offsets.names_valid && !custom.name.empty();
    offsets.append(i);
  } else if (custom.tag != customize_tag::invalid_tag) {
    offsets.append(i, name);
  }
}

// A fixed underlying type holds every common value in the range, so a plain cast is enough.
template <typename E, enum_subtype S>
inline constexpr bool plain_scan_v = S == enum_subtype::common && has_fixed_underlying_v<E>;

template <typename E, enum_subtype S, int Min, std::size_t J, std::size_t... I>
MAGIC_ENUM_CONSTEVAL auto block_names(std::index_sequence<I...>) noexcept {
#if defined(MAGIC_ENUM_DETAIL_RESHARPER_ENUMS)
  signature_names_t<sizeof...(I)> result{};
  result.valid = true;
  for (std::size_t i = 0; i < sizeof...(I); ++i) {
    result.names[i] = resharper_name(__rscpp_enumerator_name(batch_value<E, Min, S>(J + i)));
    result.count += result.names[i].size_ != 0;
  }
  return result;
#else
  if constexpr (!batch_supported<E>()) {
    return signature_names_t<sizeof...(I)>{};
  } else if constexpr (plain_scan_v<E, S>) {
    using U = std::underlying_type_t<E>;
    return signature_names<sizeof...(I)>(nb<static_cast<E>(static_cast<U>(static_cast<int>(J + I) + Min))...>());
  } else {
    return signature_names<sizeof...(I)>(nb<batch_value<E, Min, S>(J + I)...>());
  }
#endif
}

inline constexpr std::size_t scan_block_size = 512;

MAGIC_ENUM_CONSTEVAL std::size_t block_size(std::size_t size, std::size_t j) noexcept {
  return (size - j) < scan_block_size ? (size - j) : scan_block_size;
}

// Separate constants avoid repeated evaluation and constexpr step limits on Clang/MSVC.
// Copy blocks when reading them, references make MSVC emit blocks and signatures into the object file.
template <typename E, enum_subtype S, std::size_t Size, int Min, std::size_t J>
inline constexpr auto block_names_v = block_names<E, S, Min, J>(std::make_index_sequence<block_size(Size, J)>{});

// Scans 256 values with n() per value. Hidden values skip n().
template <typename E, enum_subtype S, std::size_t Size, int Min, std::size_t J, std::size_t N>
MAGIC_ENUM_CONSTEVAL void scan_fallback(valid_offsets_t<N>& offsets) noexcept {
#if defined(MAGIC_ENUM_DETAIL_NVCC)
  // A Clang 20+ host rejects n() for values outside the range of an enum without a fixed underlying type, see CWG1766.
#  define MAGIC_ENUM_CASTABLE(O) && batch_castable<E>(underlying_value<E, Min, S>(J + O))
#else
#  define MAGIC_ENUM_CASTABLE(O)
#endif
#define MAGIC_ENUM_V(O)                                                              \
  if constexpr ((J + O) < Size) {                                                    \
    constexpr auto v = value<E, Min, S>(J + O);                                      \
    constexpr auto custom = customize::enum_name<E>(v);                              \
    if constexpr (custom.tag != customize_tag::invalid_tag MAGIC_ENUM_CASTABLE(O)) { \
      if constexpr (n<v>().size_ != 0) {                                             \
        append_named(offsets, J + O, v, n<v>());                                     \
      }                                                                              \
    }                                                                                \
  }

  MAGIC_ENUM_CASES_256(MAGIC_ENUM_V, 0)
#undef MAGIC_ENUM_V
#undef MAGIC_ENUM_CASTABLE
}

// Scans a block with one nb(), or with n() per value if nb() fails.
template <typename E, enum_subtype S, std::size_t Size, int Min, std::size_t J>
MAGIC_ENUM_CONSTEVAL auto block_offsets() noexcept {
  constexpr std::size_t count = block_size(Size, J);
  constexpr auto block = block_names_v<E, S, Size, Min, J>;
  // Sized by the named values, since a block sized copy is slow in MSVC constant evaluation.
  constexpr std::size_t size = !block.valid ? count : block.count > 0 ? block.count : 1;
  valid_offsets_t<size> offsets;
  if constexpr (block.valid) {
    for (std::size_t i = 0; i < count; ++i) {
      // nb() takes zero in place of a value that it cannot cast.
      if (block.names[i].size_ != 0 && (plain_scan_v<E, S> || batch_castable<E>(underlying_value<E, Min, S>(J + i)))) {
        append_named(offsets, J + i, value<E, Min, S>(J + i), block.names[i]);
      }
    }
  } else {
    scan_fallback<E, S, Size, Min, J>(offsets);
    if constexpr (count > 256) {
      scan_fallback<E, S, Size, Min, J + 256>(offsets);
    }
  }
  return offsets;
}

template <typename E, enum_subtype S, std::size_t Size, int Min, std::size_t J>
inline constexpr auto block_offsets_v = block_offsets<E, S, Size, Min, J>();

template <typename E, enum_subtype S, std::size_t Size, int Min, std::size_t J = 0>
MAGIC_ENUM_CONSTEVAL std::size_t valid_count() noexcept {
  constexpr std::size_t count = block_offsets_v<E, S, Size, Min, J>.count;
  if constexpr ((J + scan_block_size) < Size) {
    return count + valid_count<E, S, Size, Min, J + scan_block_size>();
  } else {
    return count;
  }
}

template <typename E, enum_subtype S, std::size_t Size, int Min, std::size_t J, std::size_t N>
MAGIC_ENUM_CONSTEVAL void append_offsets(valid_offsets_t<N>& offsets) noexcept {
  constexpr auto block = block_offsets_v<E, S, Size, Min, J>;
  for (std::size_t i = 0; i < block.count; ++i) {
    offsets.append(block.offsets[i], block.names[i]);
  }
  offsets.names_valid = offsets.names_valid && block.names_valid;

  if constexpr ((J + scan_block_size) < Size) {
    append_offsets<E, S, Size, Min, J + scan_block_size>(offsets);
  }
}

// Sized by the found values, not by the range.
template <typename E, enum_subtype S, std::size_t Size, int Min>
MAGIC_ENUM_CONSTEVAL auto valid_offsets() noexcept {
  constexpr std::size_t count = valid_count<E, S, Size, Min>();
  valid_offsets_t<(count > 0 ? count : 1)> offsets;
  append_offsets<E, S, Size, Min, 0>(offsets);
  return offsets;
}

// Probe offsets 1, 2, 4, 8 outside each bound to catch a range that is too small, see #117.
template <int Min, int Max>
MAGIC_ENUM_CONSTEVAL long long probe(std::size_t i) noexcept {
  return i < 4 ? static_cast<long long>(Min) - (1LL << i) : static_cast<long long>(Max) + (1LL << (i - 4));
}

template <typename E, int Min, int Max, typename U = std::underlying_type_t<E>>
MAGIC_ENUM_CONSTEVAL bool probe_valid(std::size_t i) noexcept {
  const auto p = probe<Min, Max>(i);
  return !cmp_less(p, (std::numeric_limits<U>::min)()) && !cmp_less((std::numeric_limits<U>::max)(), p) && batch_castable<E>(static_cast<U>(p));
}

template <typename E, int Min, int Max, typename U = std::underlying_type_t<E>>
MAGIC_ENUM_CONSTEVAL E probe_value(std::size_t i) noexcept {
  return probe_valid<E, Min, Max>(i) ? to_enum<E>(static_cast<U>(probe<Min, Max>(i))) : E{};
}

template <typename E, int Min, int Max, std::size_t... I>
MAGIC_ENUM_CONSTEVAL auto probe_names(std::index_sequence<I...>) noexcept {
  if constexpr (batch_supported<E>()) {
    return signature_names<sizeof...(I)>(nb<probe_value<E, Min, Max>(I)...>());
  } else {
    // MSVC 19.29 rejects the temporary aggregate in consteval calls.
    constexpr signature_names_t<sizeof...(I)> empty{};
    return empty;
  }
}

// A probe counts if it has a name that customization does not hide.
template <typename E, int Min, int Max, std::size_t I>
MAGIC_ENUM_CONSTEVAL bool named_probe() noexcept {
  constexpr auto v = probe_value<E, Min, Max>(I);
  if constexpr (probe_valid<E, Min, Max>(I) && customize::enum_name<E>(v).tag != customize_tag::invalid_tag) {
    return n<v>().size_ != 0;
  } else {
    return false;
  }
}

template <typename E, int Min, int Max, std::size_t... I>
MAGIC_ENUM_CONSTEVAL bool named_near_range(std::index_sequence<I...> seq) noexcept {
  constexpr auto probes = probe_names<E, Min, Max>(seq);
  if constexpr (probes.valid) {
    for (std::size_t i = 0; i < sizeof...(I); ++i) {
      if (probes.names[i].size_ != 0 && probe_valid<E, Min, Max>(i) && customize::enum_name<E>(probe_value<E, Min, Max>(i)).tag != customize_tag::invalid_tag) {
        return true;
      }
    }
    return false;
  } else {
    return (named_probe<E, Min, Max, I>() || ...);
  }
}

template <typename E, std::size_t N, std::size_t C>
struct scan_t {
  std::array<E, N> values{};
  // Name i ends before chars[ends[i] - 1], which is null.
  std::array<std::size_t, N> ends{};
  std::array<char_type, C> chars{};
};

template <typename E, std::size_t N>
MAGIC_ENUM_CONSTEVAL bool valid_prefixes(const valid_offsets_t<N>& offsets) noexcept {
  for (std::size_t i = 0; i < offsets.count; ++i) {
    if (offsets.names[i].str_ != nullptr && !valid_prefix_length(prefix_length_v<E>, offsets.names[i].size_)) {
      return false;
    }
  }
  return true;
}

template <typename E, int Min, enum_subtype S, std::size_t N>
MAGIC_ENUM_CONSTEVAL std::size_t chars_size(const valid_offsets_t<N>& offsets) noexcept {
  std::size_t size = 0;
  for (std::size_t i = 0; i < offsets.count; ++i) {
    size += (offsets.names[i].str_ != nullptr
             ? offsets.names[i].size_ - prefix_length_v<E>
             : customize::enum_name<E>(value<E, Min, S>(offsets.offsets[i])).name.size()) + 1;
  }
  return size;
}

// All names are copied into one array instead of a static_str per value.
template <typename E, enum_subtype S, std::size_t Size, int Min>
MAGIC_ENUM_CONSTEVAL auto scan() noexcept {
#if !defined(MAGIC_ENUM_NO_CHECK_OUT_OF_RANGE)
  if constexpr (S == enum_subtype::common) {
    // Flags often have bits outside the common range, so as_common<> does not require them in it.
    if constexpr (named_near_range<E, Min, Min + static_cast<int>(Size - 1)>(std::make_index_sequence<8>{})) {
      static_assert(subtype_v<E> == enum_subtype::flags, "magic_enum::enum_range found a named value outside the range. Extend magic_enum::customize::enum_range<E>::min/max or MAGIC_ENUM_RANGE_MIN/MAX, or return invalid_tag from magic_enum::customize::enum_name to exclude the value.");
    }
  }
#endif
  constexpr auto offsets = valid_offsets<E, S, Size, Min>();
  static_assert(offsets.names_valid, "magic_enum::customize::enum_name requires a non-empty name. Return invalid_tag to exclude a value.");
  static_assert(valid_custom_zero<E>(), "magic_enum::customize::enum_name cannot add an undeclared enum value.");
  constexpr bool valid_prefix = valid_prefixes<E>(offsets);
  static_assert(valid_prefix, "magic_enum::customize requires valid prefix length.");
  scan_t<E, offsets.count, valid_prefix ? chars_size<E, Min, S>(offsets) : 0> result;
  if constexpr (offsets.count > 0 && valid_prefix) {
    // A negative sign bit flag goes first, so values ascend as in the standard reflection table.
    constexpr std::size_t first = (S == enum_subtype::flags && cmp_less(underlying_value<E, Min, S>(offsets.offsets[offsets.count - 1]), 0)) ? 1u : 0u;
    // Pointers avoid an operator[] call per element in constant evaluation.
    E* const values = result.values.data();
    std::size_t* const ends = result.ends.data();
    char_type* const chars = result.chars.data();
    std::size_t size = 0;
    for (std::size_t i = 0; i < offsets.count; ++i) {
      const auto j = (i + offsets.count - first) % offsets.count;
      const auto v = value<E, Min, S>(offsets.offsets[j]);
      values[i] = v;
      if (offsets.names[j].str_ != nullptr) {
#if defined(__clang__)
        // Faster than a loop in constant evaluation. GCC and MSVC do not allow it there.
        if constexpr (std::is_same_v<char_type, char>) {
          const auto n = offsets.names[j].size_ - prefix_length_v<E>;
          __builtin_memcpy(chars + size, offsets.names[j].str_ + prefix_length_v<E>, n);
          size += n;
        } else
#endif
        for (std::size_t k = prefix_length_v<E>; k < offsets.names[j].size_; ++k) {
          chars[size++] = static_cast<char_type>(offsets.names[j].str_[k]);
        }
      } else {
        const auto name = customize::enum_name<E>(v).name;
        const char_type* const str = name.data();
        for (std::size_t k = 0; k < name.size(); ++k) {
          chars[size++] = str[k];
        }
      }
      chars[size++] = char_type{};
      ends[i] = size;
    }
  }
  return result;
}

template <typename E, enum_subtype S>
MAGIC_ENUM_CONSTEVAL auto scan_span() noexcept {
  // Unsigned subtraction avoids signed overflow.
  return static_cast<unsigned int>(reflected_max<E, S>()) - static_cast<unsigned int>(reflected_min<E, S>());
}

template <typename E, enum_subtype S>
MAGIC_ENUM_CONSTEVAL bool check_range() noexcept {
  constexpr auto min = reflected_min<E, S>();
  constexpr auto max = reflected_max<E, S>();
  constexpr bool valid_range = !cmp_less(max, min);
  constexpr bool valid_bounds = !cmp_less(min, (std::numeric_limits<int>::min)()) && !cmp_less((std::numeric_limits<int>::max)(), max);
  constexpr bool valid_size = scan_span<E, S>() < (std::numeric_limits<std::uint16_t>::max)();
  if constexpr (!valid_range) {
    static_assert(valid_range, "magic_enum::enum_range requires min <= max.");
  } else if constexpr (!valid_bounds) {
    static_assert(valid_bounds, "magic_enum::enum_range requires min and max to fit in int.");
  } else {
    static_assert(valid_size, "magic_enum::enum_range requires max - min to be less than UINT16_MAX.");
  }
  return valid_range && valid_bounds && valid_size;
}

#if !defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)

template <typename E, enum_subtype S>
MAGIC_ENUM_CONSTEVAL auto scan() noexcept {
  if constexpr (check_range<E, S>()) {
    return scan<E, S, std::size_t{scan_span<E, S>()} + 1, static_cast<int>(reflected_min<E, S>())>();
  } else {
    return scan_t<E, 0, 0>{};
  }
}

template <typename E, enum_subtype S>
inline constexpr auto scan_v = scan<enum_definition_t<E>, S>();

#endif

template <typename E, enum_subtype S>
MAGIC_ENUM_CONSTEVAL auto values() noexcept {
#if defined(MAGIC_ENUM_DETAIL_USE_STD_REFLECTION)
  std::array<E, reflection::table_size<E, S>()> values{};
  auto* const out = values.data();
  const auto* const entries = reflection::table_v<E, S>.entries.data();
  for (std::size_t i = 0; i < values.size(); ++i) {
    out[i] = entries[i].value;
  }
  return values;
#else
  return scan_v<E, S>.values;
#endif
}

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
  auto* const out = names.data();
  const auto* const entries = reflection::table_v<E, S>.entries.data();
  for (std::size_t i = 0; i < names.size(); ++i) {
    out[i] = entries[i].name.view();
  }
  return names;
}

#else

template <typename E, enum_subtype S>
inline constexpr auto name_chars_v = scan_v<E, S>.chars;

template <typename E, enum_subtype S>
MAGIC_ENUM_CONSTEVAL auto names() noexcept {
  std::array<string_view, count_v<E, S>> names{};
  // Skip the loop for empty enums, otherwise MSVC /analyze warns that it never runs.
  if constexpr (count_v<E, S> != 0) {
    string_view* const out = names.data();
    const std::size_t* const ends = scan_v<E, S>.ends.data();
    const char_type* const chars = name_chars_v<E, S>.data();
    for (std::size_t i = 0, begin = 0; i < count_v<E, S>; ++i) {
      const auto end = ends[i];
      out[i] = string_view{chars + begin, end - begin - 1};
      begin = end;
    }
  }
  return names;
}

#endif

template <typename E, enum_subtype S>
inline constexpr auto names_v = names<E, S>();

struct name_lengths_t {
  std::size_t min;
  std::size_t max;
};

template <typename E, enum_subtype S>
MAGIC_ENUM_CONSTEVAL name_lengths_t name_lengths() noexcept {
  name_lengths_t lengths{~std::size_t{0}, 0};
  for (const auto name : names_v<E, S>) {
    lengths.min = name.size() < lengths.min ? name.size() : lengths.min;
    lengths.max = name.size() > lengths.max ? name.size() : lengths.max;
  }
  return lengths;
}

template <typename E, enum_subtype S>
inline constexpr auto name_lengths_v = name_lengths<E, S>();

// Returns count_v if no name matches.
template <typename E, enum_subtype S, typename BinaryPredicate>
constexpr std::size_t name_index([[maybe_unused]] string_view value, [[maybe_unused]] BinaryPredicate& p) {
  // Skip the loop for empty enums, otherwise MSVC /analyze warns that it never runs.
  if constexpr (count_v<E, S> > 0) {
    if constexpr (is_default_predicate_v<BinaryPredicate>) {
      if (value.size() < name_lengths_v<E, S>.min || value.size() > name_lengths_v<E, S>.max) {
        return count_v<E, S>;
      }
    }
    for (std::size_t i = 0; i < count_v<E, S>; ++i) {
      if (cmp_equal(value, names_v<E, S>[i], p)) {
        return i;
      }
    }
  }
  return count_v<E, S>;
}

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
  static_assert(supported<E>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
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
template <typename T>
inline constexpr bool is_integer_input_v = std::is_integral_v<T> || is_unscoped_enum<T>::value;

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
  // Only reachable when the labels reach both limits of the type, so skip real cases and placed labels.
  for (T v = (std::numeric_limits<T>::min)(); i < Size; ++v) {
    bool used = false;
    for (std::size_t j = 0; j < i; ++j) {
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
  return n <= 8 ? 8u : n <= 16 ? 16u : n <= 32 ? 32u : n <= 64 ? 64u : n <= 128 ? 128u : 256u;
}

#define MAGIC_ENUM_INDEX_CASE(J)               \
  case cases[(J)]:                             \
    if constexpr ((J) + Page < Cases::count) { \
      return (J) + Page;                       \
    } else {                                   \
      break;                                   \
    }
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
constexpr std::size_t index_switch(typename Cases::type value) noexcept {
  constexpr std::size_t n = Cases::count - Page;
  constexpr std::size_t Size = switch_page_size(n);
  constexpr auto cases = Cases::template page<Page, Size>();
  MAGIC_ENUM_PAGE_SWITCHES(MAGIC_ENUM_INDEX_CASE, value)
  if constexpr (n > Size) {
    return index_switch<Cases, Page + Size>(value);
  } else {
    return Cases::count;
  }
}

// De Bruijn multiplication maps every single bit to a distinct slot without a loop or a switch.
template <typename V>
constexpr std::size_t bit_slot(V value) noexcept {
  if constexpr (sizeof(V) <= sizeof(std::uint32_t)) {
    auto slot = static_cast<std::uint32_t>(value);
    slot *= std::uint32_t{0x077CB531};
    return slot >> 27;
  } else {
    auto slot = static_cast<std::uint64_t>(value);
    slot *= std::uint64_t{0x03F79D71B4CA8B09};
    return static_cast<unsigned>(slot >> 58);
  }
}

// Maps bit_slot of a flag to its index in values_v. The last slot holds the index of a named zero.
template <typename E, typename V = make_unsigned_t<std::underlying_type_t<E>>>
MAGIC_ENUM_CONSTEVAL auto flag_indices() noexcept {
  constexpr auto& values = values_v<E, enum_subtype::flags>;
  std::array<std::uint8_t, (sizeof(V) <= sizeof(std::uint32_t) ? 32u : 64u) + 1u> indices{};
  for (auto& i : indices) {
    i = static_cast<std::uint8_t>(values.size());
  }
  for (std::size_t i = 0; i < values.size(); ++i) {
    const auto v = static_cast<V>(values[i]);
    indices[v == V{0} ? indices.size() - 1 : bit_slot(v)] = static_cast<std::uint8_t>(i);
  }
  return indices;
}

template <typename E>
inline constexpr auto flag_indices_v = flag_indices<E>();

// Returns count_v if not found.
template <typename E, enum_subtype S>
constexpr std::size_t index_of([[maybe_unused]] std::underlying_type_t<E> value) noexcept {
  using V = make_unsigned_t<std::underlying_type_t<E>>;
  if constexpr (count_v<E, S> == 0) {
    return count_v<E, S>;
  } else if constexpr (S == enum_subtype::flags && sizeof(V) <= sizeof(std::uint64_t)) {
    // Flag tables hold only single bits and a named zero, so a slot table replaces a switch over every flag.
    if (!is_flag_candidate(value)) {
      return count_v<E, S>;
    }
    const auto v = static_cast<V>(value);
    constexpr auto& indices = flag_indices_v<E>;
    return indices[v == V{0} ? indices.size() - 1 : bit_slot(v)];
  } else if constexpr (is_sparse_v<E, S> || (S == enum_subtype::flags)) {
    return index_switch<value_cases<E, S>>(static_cast<switch_t<E>>(value));
  } else if (value >= min_v<E, S> && value <= max_v<E, S>) {
    if constexpr (std::is_same_v<std::underlying_type_t<E>, bool>) {
      return static_cast<std::size_t>(static_cast<unsigned>(value) - static_cast<unsigned>(min_v<E, S>));
    } else {
      return static_cast<std::size_t>(value - min_v<E, S>);
    }
  } else {
    return count_v<E, S>;
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

// Identifies enums whose default reflection mode is flags.
template <typename T>
struct is_flags_enum : detail::is_flags_enum<T> {};

template <typename T>
inline constexpr bool is_flags_enum_v = is_flags_enum<T>::value;

// Like std::underlying_type, but without a member type when T is not an enum.
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
  static_assert(!name.empty(), "magic_enum::enum_type_name enum type does not have a name. Use a typedef name or return a name from magic_enum::customize::enum_type_name.");

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
// Asserts index < count. An out-of-range index is undefined behavior when assertions are disabled.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_value(std::size_t index) noexcept -> detail::enable_if_t<E, std::decay_t<E>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_value requires a non-empty reflected enum.");
  MAGIC_ENUM_ASSERT(index < detail::count_v<D, S>);

  if constexpr (detail::is_sparse_v<D, S>) {
    return detail::values_v<D, S>[index];
  } else if constexpr (S == detail::enum_subtype::flags) {
    using V = detail::make_unsigned_t<underlying_type_t<D>>;
    constexpr auto min = detail::log2(static_cast<V>(detail::min_v<D, S>));

    return detail::value<D, min, S>(index);
  } else {
    constexpr auto min = detail::min_v<D, S>;

    return detail::value<D, min, S>(index);
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

  if (const auto i = detail::index_of<D, S>(static_cast<underlying_type_t<D>>(value)); i < detail::count_v<D, S>) {
    return i;
  }
  return {};
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

  if (const auto i = detail::index_of<D, S>(static_cast<underlying_type_t<D>>(value)); i < detail::count_v<D, S>) {
    return detail::names_v<D, S>[i];
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

// ASCII case-insensitive predicate for enum_cast and enum_contains.
inline constexpr auto case_insensitive = detail::case_insensitive<>{};

// Returns the enum value matching the integer, or an empty optional.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>, typename I = underlying_type_t<std::decay_t<E>>, std::enable_if_t<detail::is_integer_input_v<I>, int> = 0>
[[nodiscard]] constexpr auto enum_cast(I value) noexcept -> detail::enable_if_t<E, optional<std::decay_t<E>>> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_cast requires a non-empty reflected enum.");

  if (detail::in_underlying_range<D>(value) && detail::index_of<D, S>(static_cast<underlying_type_t<D>>(value)) < detail::count_v<D, S>) {
    return static_cast<D>(static_cast<underlying_type_t<D>>(value));
  }
  return {}; // Invalid value or out of range.
}

// Returns the enum value matching the name, or an empty optional.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>, typename BinaryPredicate = std::equal_to<>>
[[nodiscard]] constexpr auto enum_cast(string_view value, [[maybe_unused]] BinaryPredicate p = {}) noexcept(detail::is_nothrow_invocable_v<BinaryPredicate>) -> detail::enable_if_t<E, optional<std::decay_t<E>>, BinaryPredicate> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_cast requires a non-empty reflected enum.");

#if defined(MAGIC_ENUM_ENABLE_HASH)
  if constexpr (detail::has_usable_hash<&detail::names_v<D, S>, detail::constexpr_hash_t, (detail::count_v<D, S> > 0 && detail::is_default_predicate_v<BinaryPredicate>)>()) {
    return detail::hash_switch<&detail::names_v<D, S>>(
        [](std::size_t i) { return optional<D>{detail::values_v<D, S>[i]}; },
        value,
        detail::default_result_type_lambda<optional<D>>,
        [&p](string_view lhs, string_view rhs) { return detail::cmp_equal(lhs, rhs, p); });
  } else {
#endif
    if (const auto i = detail::name_index<D, S>(value, p); i < detail::count_v<D, S>) {
      return detail::values_v<D, S>[i];
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

  return detail::index_of<D, S>(static_cast<underlying_type_t<D>>(value)) < detail::count_v<D, S>;
}

// Returns true if enum contains specified value.
template <detail::enum_subtype S, typename E>
[[nodiscard]] constexpr auto enum_contains(E value) noexcept -> detail::enable_if_t<E, bool> {
  return enum_contains<std::decay_t<E>, S>(value);
}

// Returns true if enum contains specified integer value.
// The enum value overload handles arguments of type E.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>, typename I, std::enable_if_t<detail::is_integer_input_v<I> && !std::is_same_v<I, std::decay_t<E>>, int> = 0>
[[nodiscard]] constexpr auto enum_contains(I value) noexcept -> detail::enable_if_t<E, bool> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_contains requires a non-empty reflected enum.");

  return detail::in_underlying_range<D>(value) && detail::index_of<D, S>(static_cast<underlying_type_t<D>>(value)) < detail::count_v<D, S>;
}

// Returns true if enum contains enumerator with specified name.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>, typename BinaryPredicate = std::equal_to<>>
[[nodiscard]] constexpr auto enum_contains(string_view value, BinaryPredicate p = {}) noexcept(detail::is_nothrow_invocable_v<BinaryPredicate>) -> detail::enable_if_t<E, bool, BinaryPredicate> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum::enum_contains requires a non-empty reflected enum.");

#if defined(MAGIC_ENUM_ENABLE_HASH)
  if constexpr (detail::has_usable_hash<&detail::names_v<D, S>, detail::constexpr_hash_t, (detail::count_v<D, S> > 0 && detail::is_default_predicate_v<BinaryPredicate>)>()) {
    return static_cast<bool>(enum_cast<D, S, BinaryPredicate&>(value, p));
  } else {
#endif
    return detail::name_index<D, S>(value, p) < detail::count_v<D, S>;
#if defined(MAGIC_ENUM_ENABLE_HASH)
  }
#endif
}

// Checks the reflection domain, not enumerator membership.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>, typename I, std::enable_if_t<detail::is_integer_input_v<I> && !std::is_same_v<I, std::decay_t<E>>, int> = 0>
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
    return detail::check_range<D, S>() && !detail::cmp_less(v, min) && !detail::cmp_less(max, v);
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

#undef MAGIC_ENUM_DETAIL_RESHARPER_ENUMS
#undef MAGIC_ENUM_DETAIL_NVCC
#undef MAGIC_ENUM_CALLING_CONVENTION
#undef MAGIC_ENUM_SIGNATURE
#undef MAGIC_ENUM_DETAIL_USE_STD_REFLECTION

#endif // NEARGYE_MAGIC_ENUM_HPP
