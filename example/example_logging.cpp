// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Daniil Goncharov <neargye@gmail.com>.
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

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_bitwise.hpp>
#include <magic_enum/magic_enum_flags.hpp>
#include <magic_enum/magic_enum_iostream.hpp>

#if defined(__has_include)
#  if __has_include(<version>)
#    include <version>
#  endif
#endif
#if !defined(MAGIC_ENUM_NO_STD_FORMAT) && defined(__cpp_lib_format) && __cpp_lib_format >= 202106L
#  include <format>
#  include <magic_enum/magic_enum_format.hpp>
#  define EXAMPLE_HAS_FORMAT
#endif

enum class Level { Debug, Info, Warning, Error };
MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS(Level);

// Operators next to the enum make magic_enum reflect it as flags.
enum class Permission : unsigned { None = 0, Read = 1, Write = 2, Execute = 4 };
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(Permission);
MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS(Permission);

// Name for a log line. A value without a name, such as data from a file or a newer protocol version, is written as an integer.
template <typename E>
std::string log_name(E value) {
  std::string name;
  if constexpr (magic_enum::is_flags_enum_v<E>) {
    name = magic_enum::enum_flags_name(value);
  } else {
    name = std::string{magic_enum::enum_name(value)};
  }
  return name.empty() ? std::to_string(magic_enum::enum_integer(value)) : name;
}

// Logger for C-style APIs that take strings.
template <typename E>
void log_printf(Level level, const char* what, E value) {
  std::printf("[%s] %s: %s\n", log_name(level).c_str(), what, log_name(value).c_str());
}

// Stream logger. The magic_enum stream operators already write unnamed values as integers.
template <typename... Args>
void log_stream(Level level, const Args&... args) {
  std::ostringstream line;
  line << '[' << level << ']';
  ((line << ' ' << args), ...);
  std::cout << line.str() << '\n';
}

#if defined(EXAMPLE_HAS_FORMAT)
// The magic_enum formatter also writes unnamed values as integers.
template <typename... Args>
void log_format(Level level, std::format_string<Args...> format, Args&&... args) {
  std::cout << std::format("[{}] ", level) << std::format(format, std::forward<Args>(args)...) << '\n';
}
#endif

int main() {
  const auto access = Permission::Read | Permission::Write;
  const auto unknown_level = static_cast<Level>(7);
  const auto unknown_bits = static_cast<Permission>(8);

  log_printf(Level::Info, "access", access); // [Info] access: Read|Write
  log_printf(Level::Warning, "level", unknown_level); // [Warning] level: 7
  log_printf(Level::Error, "bits", unknown_bits); // [Error] bits: 8

  log_stream(Level::Info, "user", 42, "access", access); // [Info] user 42 access Read|Write
  log_stream(unknown_level, "bits", unknown_bits); // [7] bits 8

#if defined(EXAMPLE_HAS_FORMAT)
  log_format(Level::Debug, "access {} bits {}", access, unknown_bits); // [Debug] access Read|Write bits 8
#endif

  return 0;
}
