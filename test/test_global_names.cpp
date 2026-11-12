// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

// Globals of user code can share names with locals of the library.
// MSVC checks them when templates are instantiated, GCC and Clang when headers are parsed.
int result = 0;
int count = 0;
int prefix = 0;
int value = 0;
int name = 0;
int size = 0;
int first = 0;
int last = 0;

#if defined(_MSC_VER) && !defined(__clang__)
// C4365 is off by default, but user code can enable it.
#  pragma warning(4 : 4365)
#endif

// Standard headers may use min and max, so they are included before the macros.
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iosfwd>
#include <iterator>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#if (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L
#  if __has_include(<bit>)
#    include <bit>
#  endif
#  if __has_include(<format>)
#    include <format>
#  endif
#endif
#if defined(__cpp_impl_reflection) && __has_include(<meta>)
#  include <meta>
#endif
#if defined(_MSC_VER) && !defined(__clang__)
#  include <intrin.h>
#endif

// Windows headers define function-like min and max macros unless NOMINMAX is defined.
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

#include "magic_enum_test_config.hpp"
#include <magic_enum/magic_enum_all.hpp>

#undef min
#undef max

namespace global_names {

enum class Mode { Read, Write };

enum class Bits : unsigned { A = 1, B = 2 };
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(Bits);

enum class Prefixed { Prefix_A, Prefix_B };

} // namespace global_names

template <>
struct magic_enum::customize::enum_range<global_names::Prefixed> {
  static constexpr std::size_t prefix_length = sizeof("Prefix_") - 1;
};

int global_names_use() {
  using namespace global_names;
  static_assert(magic_enum::enum_count<Mode>() == 2);
  static_assert(magic_enum::enum_name<Prefixed::Prefix_A>() == "A");
  static_assert(magic_enum::enum_flags_cast<Bits>("A|B") == (Bits::A | Bits::B));
  static_assert(magic_enum::enum_cast<Mode>("Write") == Mode::Write);

  magic_enum::containers::array<Mode, int> array{};
  magic_enum::containers::bitset<Mode> bitset{};
  magic_enum::containers::set<Mode> set{Mode::Read};
  int sum = array[Mode::Read] + static_cast<int>(bitset.count() + set.size());
  magic_enum::enum_for_each<Mode>([&](auto) { ++sum; });
  sum += magic_enum::enum_switch([](auto) { return 1; }, Mode::Write, 0);
  sum += static_cast<int>(magic_enum::enum_fuse(Mode::Read, Bits::A).has_value());
  sum += magic_enum::enum_next_value_circular(Mode::Write) == Mode::Read ? 1 : 0;
  sum += magic_enum::enum_flags_name(Bits::A | Bits::B) == "A|B" ? 1 : 0;
  sum += magic_enum::enum_flags_contains<Bits>(3) ? 1 : 0;

  std::ostringstream os;
  using magic_enum::iostream_operators::operator<<;
  os << Mode::Read << Bits::A;
  return sum + static_cast<int>(os.str().size()) + result + count + prefix + value + name + size + first + last;
}
