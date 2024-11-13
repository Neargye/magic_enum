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

#ifndef NEARGYE_MAGIC_ENUM_FUSE_HPP
#define NEARGYE_MAGIC_ENUM_FUSE_HPP

#include "magic_enum.hpp"

namespace magic_enum {

namespace detail {

template <typename E>
constexpr optional<std::uintmax_t> fuse_one_enum(optional<std::uintmax_t> hash, E value) noexcept {
  if (hash) {
    if (const auto index = enum_index(value)) {
      return (*hash << log2((enum_count<E>() << 1) - 1)) | *index;
    }
  }
  return {};
}

template <typename E>
constexpr optional<std::uintmax_t> fuse_enum(E value) noexcept {
  return fuse_one_enum(0, value);
}

template <typename E, typename... Es>
constexpr optional<std::uintmax_t> fuse_enum(E head, Es... tail) noexcept {
  return fuse_one_enum(fuse_enum(tail...), head);
}

template <typename... Es>
constexpr auto typesafe_fuse_enum(Es... values) noexcept {
  enum class enum_fuse_t : std::uintmax_t;
  const auto fuse = fuse_enum(values...);
  if (fuse) {
    return optional<enum_fuse_t>{static_cast<enum_fuse_t>(*fuse)};
  }
  return optional<enum_fuse_t>{};
}

} // namespace magic_enum::detail

// Returns a bijective mix of several enum values. This can be used to emulate 2D switch/case statements.
template <typename... Es>
[[nodiscard]] constexpr auto enum_fuse(Es... values) noexcept {
  static_assert((std::is_enum_v<std::decay_t<Es>> && ...), "magic_enum::enum_fuse requires enum type.");
  static_assert(sizeof...(Es) >= 2, "magic_enum::enum_fuse requires at least 2 values.");
  static_assert((detail::log2(enum_count<std::decay_t<Es>>() + 1) + ...) <= (sizeof(std::uintmax_t) * 8), "magic_enum::enum_fuse does not work for large enums");
#if defined(MAGIC_ENUM_NO_TYPESAFE_ENUM_FUSE)
  const auto fuse = detail::fuse_enum<std::decay_t<Es>...>(values...);
#else
  const auto fuse = detail::typesafe_fuse_enum<std::decay_t<Es>...>(values...);
#endif
  return MAGIC_ENUM_ASSERT(fuse), fuse;
}

} // namespace magic_enum

#endif // NEARGYE_MAGIC_ENUM_FUSE_HPP
