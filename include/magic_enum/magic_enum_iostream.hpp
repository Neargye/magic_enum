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

#ifndef NEARGYE_MAGIC_ENUM_IOSTREAM_HPP
#define NEARGYE_MAGIC_ENUM_IOSTREAM_HPP

#include "magic_enum.hpp"
#if MAGIC_ENUM_VERSION_MAJOR != 0 || MAGIC_ENUM_VERSION_MINOR != 9 || MAGIC_ENUM_VERSION_PATCH != 8
#  error "magic_enum headers of different versions are mixed. Check the include paths."
#endif
#include "magic_enum_flags.hpp"

#ifndef MAGIC_ENUM_USE_STD_MODULE
#  include <iosfwd>
#  include <string>
#  include <string_view>
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4459 6244) // Local names can match global names in user code.
#endif

namespace magic_enum {

namespace detail {

// Formatted insertion, so width, fill and adjustfield apply to the name as a whole.
template <typename Char, typename Traits>
std::basic_ostream<Char, Traits>& write_name(std::basic_ostream<Char, Traits>& os, string_view name) {
  if constexpr (std::is_same_v<Char, char_type>) {
    return os << std::basic_string_view<Char, Traits>{name.data(), name.size()};
  } else {
    std::basic_string<Char, Traits> s;
    s.reserve(name.size());
    for (const auto c : name) {
      s.push_back(static_cast<Char>(c));
    }
    return os << s;
  }
}

} // namespace magic_enum::detail

// Warning: a using directive enables stream operators for every enum in scope, including enums from other libraries.
// An unscoped enum can then print its name instead of its integer value.
// Prefer MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS(E), or its FRIEND variant inside the declaring class.
// For a local enum, put the using directive inside its function.
// Do not put it in a header or at namespace scope, where it can affect code included later.
// Different operator lookup for the same inline code across translation units violates the ODR.
namespace ostream_operators {

template <typename Char, typename Traits, typename E, detail::enable_if_t<E, int> = 0>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, E value) {
  using D = std::decay_t<E>;

  // Enums without reflected values, such as std::byte, are written as integers.
  if constexpr (detail::count_v<D, detail::subtype_v<D>> > 0) {
    if constexpr (detail::subtype_v<D> == detail::enum_subtype::flags) {
      if (const auto name = enum_flags_name<D>(value); !name.empty()) {
        return detail::write_name(os, string_view{name.data(), name.size()});
      }
    } else {
      if (const auto name = enum_name<D>(value); !name.empty()) {
        return detail::write_name(os, name);
      }
    }
  }
  return os.operator<<(static_cast<underlying_type_t<D>>(value));
}

template <typename Char, typename Traits, typename E, detail::enable_if_t<E, int> = 0>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, optional<E> value) {
  return value ? (os << *value) : os;
}

} // namespace magic_enum::ostream_operators

namespace istream_operators {

template <typename Char, typename Traits, typename E, detail::enable_if_t<E, int> = 0>
std::basic_istream<Char, Traits>& operator>>(std::basic_istream<Char, Traits>& is, E& value) {
  using D = std::decay_t<E>;

  std::basic_string<Char, Traits> s;
  if (!(is >> s)) {
    return is;
  }
  const auto v = [&] {
    if constexpr (detail::count_v<D, detail::subtype_v<D>> == 0) {
      return optional<D>{};
    } else if constexpr (detail::subtype_v<D> == detail::enum_subtype::flags) {
      return enum_flags_cast<D>(string_view{s.data(), s.size()});
    } else {
      return enum_cast<D>(string_view{s.data(), s.size()});
    }
  }();
  if (v) {
    value = *v;
  } else {
    is.setstate(std::basic_ios<Char>::failbit);
  }
  return is;
}

} // namespace magic_enum::istream_operators

namespace iostream_operators {

using magic_enum::ostream_operators::operator<<;
using magic_enum::istream_operators::operator>>;

} // namespace magic_enum::iostream_operators

} // namespace magic_enum

// Defines stream operators for E only, found by ADL without a using directive.
// Use in the namespace of E, or in the namespace of the enclosing class after its definition.
#define MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS(E) MAGIC_ENUM_DETAIL_DEFINE_IOSTREAM_OPERATORS(, E)

// Defines hidden friend stream operators for E. Use inside the class that declares E, for private enums and class templates.
#define MAGIC_ENUM_DEFINE_FRIEND_IOSTREAM_OPERATORS(E) MAGIC_ENUM_DETAIL_DEFINE_IOSTREAM_OPERATORS(friend, E)

// Fixing E makes these templates more specialized than the namespace ones, so a using directive in the same scope is not ambiguous.
// Prefixed names avoid -Wshadow with members and template parameters of the enclosing class.
#define MAGIC_ENUM_DETAIL_DEFINE_IOSTREAM_OPERATORS(SPEC, E)                                                                                                         \
  static_assert(::std::is_enum_v<E>, "magic_enum iostream operators require an enum type.");                                                                         \
  template <typename MagicEnumChar, typename MagicEnumTraits>                                                                                                        \
  SPEC ::std::basic_ostream<MagicEnumChar, MagicEnumTraits>& operator<<(::std::basic_ostream<MagicEnumChar, MagicEnumTraits>& magic_enum_os, E magic_enum_value) {   \
    return ::magic_enum::ostream_operators::operator<<(magic_enum_os, magic_enum_value);                                                                             \
  }                                                                                                                                                                  \
  template <typename MagicEnumChar, typename MagicEnumTraits>                                                                                                        \
  SPEC ::std::basic_istream<MagicEnumChar, MagicEnumTraits>& operator>>(::std::basic_istream<MagicEnumChar, MagicEnumTraits>& magic_enum_is, E& magic_enum_value) {  \
    return ::magic_enum::istream_operators::operator>>(magic_enum_is, magic_enum_value);                                                                             \
  }                                                                                                                                                                  \
  static_assert(true, "")

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif

#endif // NEARGYE_MAGIC_ENUM_IOSTREAM_HPP
