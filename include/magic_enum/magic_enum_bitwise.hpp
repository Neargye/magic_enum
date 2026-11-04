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

#ifndef NEARGYE_MAGIC_ENUM_BITWISE_HPP
#define NEARGYE_MAGIC_ENUM_BITWISE_HPP

// Standalone header, it does not include magic_enum.hpp.

#ifndef MAGIC_ENUM_USE_STD_MODULE
#  include <type_traits>
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4459) // Local names can match global names in user code.
#endif

namespace magic_enum {

// Warning: a using directive enables these operators for every enum in scope, including enums from other libraries.
// For an unscoped enum, this can change a | b from an integer result to an enum result.
// Prefer MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(E), or its FRIEND variant inside the declaring class.
// For a local enum, put the using directive inside its function.
// Do not put it in a header or at namespace scope, where it can affect code included later.
// Different operator lookup for the same inline code across translation units violates the ODR.
namespace bitwise_operators {

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E operator~(E rhs) noexcept {
  using U = std::underlying_type_t<E>;

  if constexpr (std::is_same_v<U, bool>) {
    return static_cast<E>(!static_cast<U>(rhs));
  } else {
    return static_cast<E>(~static_cast<U>(rhs));
  }
}

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E operator|(E lhs, E rhs) noexcept {
  using U = std::underlying_type_t<E>;

  return static_cast<E>(static_cast<U>(lhs) | static_cast<U>(rhs));
}

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E operator&(E lhs, E rhs) noexcept {
  using U = std::underlying_type_t<E>;

  return static_cast<E>(static_cast<U>(lhs) & static_cast<U>(rhs));
}

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E operator^(E lhs, E rhs) noexcept {
  using U = std::underlying_type_t<E>;

  return static_cast<E>(static_cast<U>(lhs) ^ static_cast<U>(rhs));
}

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E& operator|=(E& lhs, E rhs) noexcept {
  return lhs = (lhs | rhs);
}

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E& operator&=(E& lhs, E rhs) noexcept {
  return lhs = (lhs & rhs);
}

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr E& operator^=(E& lhs, E rhs) noexcept {
  return lhs = (lhs ^ rhs);
}

} // namespace magic_enum::bitwise_operators

} // namespace magic_enum

// Defines operators for E only, found by ADL without a using directive.
// Use in the namespace of E, or in the namespace of the enclosing class after its definition.
#define MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(E) MAGIC_ENUM_DETAIL_DEFINE_BITWISE_OPERATORS(, E)

// Defines hidden friend operators for E. Use inside the class that declares E, for private enums and class templates.
#define MAGIC_ENUM_DEFINE_FRIEND_BITWISE_OPERATORS(E) MAGIC_ENUM_DETAIL_DEFINE_BITWISE_OPERATORS(friend, E)

// Keep bool-specific ~ in a template to avoid -Wbool-operation.
// Prefixed parameter names avoid -Wshadow with members of the enclosing class.
#define MAGIC_ENUM_DETAIL_DEFINE_BITWISE_OPERATORS(SPEC, E)                                             \
  static_assert(::std::is_enum_v<E>, "magic_enum bitwise operators require an enum type.");             \
  [[maybe_unused]] SPEC constexpr E operator~(E magic_enum_rhs) noexcept {                              \
    return ::magic_enum::bitwise_operators::operator~(magic_enum_rhs);                                  \
  }                                                                                                     \
  [[maybe_unused]] SPEC constexpr E operator|(E magic_enum_lhs, E magic_enum_rhs) noexcept {            \
    return ::magic_enum::bitwise_operators::operator|(magic_enum_lhs, magic_enum_rhs);                  \
  }                                                                                                     \
  [[maybe_unused]] SPEC constexpr E operator&(E magic_enum_lhs, E magic_enum_rhs) noexcept {            \
    return ::magic_enum::bitwise_operators::operator&(magic_enum_lhs, magic_enum_rhs);                  \
  }                                                                                                     \
  [[maybe_unused]] SPEC constexpr E operator^(E magic_enum_lhs, E magic_enum_rhs) noexcept {            \
    return ::magic_enum::bitwise_operators::operator^(magic_enum_lhs, magic_enum_rhs);                  \
  }                                                                                                     \
  [[maybe_unused]] SPEC constexpr E& operator|=(E& magic_enum_lhs, E magic_enum_rhs) noexcept {         \
    return magic_enum_lhs = ::magic_enum::bitwise_operators::operator|(magic_enum_lhs, magic_enum_rhs); \
  }                                                                                                     \
  [[maybe_unused]] SPEC constexpr E& operator&=(E& magic_enum_lhs, E magic_enum_rhs) noexcept {         \
    return magic_enum_lhs = ::magic_enum::bitwise_operators::operator&(magic_enum_lhs, magic_enum_rhs); \
  }                                                                                                     \
  [[maybe_unused]] SPEC constexpr E& operator^=(E& magic_enum_lhs, E magic_enum_rhs) noexcept {         \
    return magic_enum_lhs = ::magic_enum::bitwise_operators::operator^(magic_enum_lhs, magic_enum_rhs); \
  }                                                                                                     \
  static_assert(true, "")

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif

#endif // NEARGYE_MAGIC_ENUM_BITWISE_HPP
