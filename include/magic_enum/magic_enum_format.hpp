//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.9.7
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2024 Daniil Goncharov <neargye@gmail.com>.
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

#include "magic_enum.hpp"
#include "magic_enum_flags.hpp"

namespace magic_enum::detail {

template <typename E, std::enable_if_t<std::is_enum_v<std::decay_t<E>>, int> = 0>
std::string format_as(E e) {
  using D = std::decay_t<E>;
  static_assert(std::is_same_v<char, magic_enum::string_view::value_type>, "magic_enum::formatter requires string_view::value_type type same as char.");
  if constexpr (magic_enum::detail::supported<D>::value) {
    if constexpr (magic_enum::detail::subtype_v<D> == magic_enum::detail::enum_subtype::flags) {
      if (const auto name = magic_enum::enum_flags_name<D>(e); !name.empty()) {
        return {name.data(), name.size()};
      }
    } else {
      if (const auto name = magic_enum::enum_name<D>(e); !name.empty()) {
        return {name.data(), name.size()};
      }
    }
  }
  return std::to_string(magic_enum::enum_integer<D>(e));
}

} // namespace magic_enum::format

#if defined(__cpp_lib_format)

template <typename E>
struct std::formatter<E, std::enable_if_t<std::is_enum_v<std::decay_t<E>>, char>> : std::formatter<std::string_view, char> {
  template <class FormatContext>
  auto format(E e, FormatContext& ctx) const {
    return std::formatter<std::string_view, char>::format(magic_enum::detail::format_as<E>(e), ctx);
  }
};

#endif

#if defined(FMT_VERSION)

template <typename E>
struct fmt::formatter<E, std::enable_if_t<std::is_enum_v<std::decay_t<E>>, char>> : fmt::formatter<std::string_view, char> {
  template <class FormatContext>
  auto format(E e, FormatContext& ctx) const {
    return fmt::formatter<std::string_view, char>::format(magic_enum::detail::format_as<E>(e), ctx);
  }
};

#endif

#endif // NEARGYE_MAGIC_ENUM_FORMAT_HPP
