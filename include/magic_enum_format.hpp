//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.8.2
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2022 Daniil Goncharov <neargye@gmail.com>.
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

#ifndef NEARGYE_MAGIC_ENUM_FORMAT_HPP
#define NEARGYE_MAGIC_ENUM_FORMAT_HPP

#if !defined(__cpp_lib_format)
#  error "Format is not supported"
#endif

#include "magic_enum.hpp"

#if !defined(MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT)
#  define MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT true
#  define MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT_AUTO_DEFINE
#endif // MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT

namespace magic_enum::customize {
  // customize enum to enable/disable automatic std::format
  template <typename E>
  constexpr bool enum_format_enabled() noexcept {
    return MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT;
  }
} // magic_enum::customize

#include <format>

template <typename E>
struct std::formatter<E, std::enable_if_t<std::is_enum_v<E> && magic_enum::customize::enum_format_enabled<E>(), char>> : std::formatter<std::string_view, char> {
  auto format(E e, format_context& ctx) {
    static_assert(std::is_same_v<char, string_view::value_type>, "formatter requires string_view::value_type type same as char.");
    using D = std::decay_t<E>;

    if constexpr (magic_enum::detail::supported<D>::value) {
      if (const auto name = magic_enum::enum_name<D, magic_enum::as_flags<magic_enum::detail::is_flags_v<D>>>(e); !name.empty()) {
        return std::formatter<std::string_view, char>::format(std::string_view{name.data(), name.size()}, ctx);
      }
    }
    return std::formatter<std::string_view, char>::format(std::to_string(magic_enum::enum_integer<D>(e)), ctx);
  }
};

#if defined(MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT_AUTO_DEFINE)
#  undef MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT
#  undef MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT_AUTO_DEFINE
#endif // MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT_AUTO_DEFINE

#endif // NEARGYE_MAGIC_ENUM_FORMAT_HPP
