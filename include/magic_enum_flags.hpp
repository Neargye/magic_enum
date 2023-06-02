//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.9.1
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2023 Daniil Goncharov <neargye@gmail.com>.
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

#ifndef NEARGYE_MAGIC_ENUM_FLAGS_HPP
#define NEARGYE_MAGIC_ENUM_FLAGS_HPP

#include "magic_enum.hpp"

namespace magic_enum {

// Checks whether `flags set` contains `flag`.
// Note: If `flag` equals 0, it returns false, as 0 is not a flag.
template <typename E>
constexpr auto enum_flags_test(E flags, E flag) noexcept -> detail::enable_if_t<E, bool> {
  using U = underlying_type_t<E>;

  return static_cast<bool>(static_cast<U>(flags) & static_cast<U>(flag));
}

// Checks whether `lhs flags set` and `rhs flags set` have common flags.
// Note: If `lhs flags set` or `rhs flags set` equals 0, it returns false, as 0 is not a flag, and therfore cannot have any matching flag.
template <typename E>
constexpr auto enum_flags_test_any(E lhs, E rhs) noexcept -> detail::enable_if_t<E, bool> {
  using U = underlying_type_t<E>;

  return (static_cast<U>(lhs) & static_cast<U>(rhs)) != 0;
}

} // namespace magic_enum

#endif // NEARGYE_MAGIC_ENUM_FLAGS_HPP
