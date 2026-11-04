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

#include "magic_enum_test_config.hpp"
#include <magic_enum/magic_enum_all.hpp>

#include <sstream>

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
  sum += magic_enum::enum_fuse(Mode::Read, Bits::A).has_value() ? 1 : 0;
  sum += magic_enum::enum_next_value_circular(Mode::Write) == Mode::Read ? 1 : 0;
  sum += magic_enum::enum_flags_name(Bits::A | Bits::B) == "A|B" ? 1 : 0;

  std::ostringstream os;
  using magic_enum::iostream_operators::operator<<;
  os << Mode::Read << Bits::A;
  return sum + static_cast<int>(os.str().size()) + result + count + prefix + value + name + size + first + last;
}
