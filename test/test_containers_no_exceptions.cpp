// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

// Built without exceptions. Each invalid case runs in its own process and must abort.

#include <magic_enum/magic_enum_containers.hpp>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>

enum class Flags : unsigned { None = 0, A = 1, B = 2 };
template <>
struct magic_enum::customize::enum_range<Flags> {
  static constexpr bool is_flags = true;
};

enum class Plain { X, Y };

namespace {

// The abort cases pass by this output. A nonzero code keeps an abort in the valid case a failure.
void on_abort(int) {
  std::fputs("magic_enum::containers aborted\n", stdout);
  std::fflush(stdout);
  std::_Exit(3);
}

constexpr auto both = static_cast<Flags>(3);
constexpr auto unknown = static_cast<Flags>(4);

bool valid() {
  using namespace magic_enum::containers;
  set<Flags> keys{Flags::None, Flags::A};
  keys = {Flags::B};
  keys.insert(Flags::A);
  const auto mask = set<Flags>::from_mask(both);
  bitset<Flags> bits{Flags::None, both};
  bits.set(Flags::A, false);
  const set<Plain> plain{Plain::X};
  return keys.size() == 2 && mask.size() == 2 && bits.count() == 1 && bits.test(Flags::B) && bitset<Flags>("None|A").count() == 1 && plain.size() == 1;
}

} // namespace

int main(int argc, char** argv) {
#if defined(_MSC_VER)
  // A debug CRT would show a dialog before raising SIGABRT.
  _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif
  std::signal(SIGABRT, on_abort);

  using namespace magic_enum::containers;
  const char* name = argc > 1 ? argv[1] : "valid";
  if (std::strcmp(name, "valid") == 0) {
    return valid() ? 0 : 1;
  } else if (std::strcmp(name, "set-key") == 0) {
    set<Flags> keys;
    keys.insert(both);
  } else if (std::strcmp(name, "set-plain-key") == 0) {
    static_cast<void>(set<Plain>{static_cast<Plain>(5)});
  } else if (std::strcmp(name, "from-mask") == 0) {
    static_cast<void>(set<Flags>::from_mask(unknown));
  } else if (std::strcmp(name, "bitset-mask") == 0) {
    static_cast<void>(bitset<Flags>(unknown));
  } else if (std::strcmp(name, "bitset-name") == 0) {
    static_cast<void>(bitset<Flags>("None|C"));
  } else if (std::strcmp(name, "bitset-raw-number") == 0) {
    static_cast<void>(bitset<Flags>(raw_access, 4ULL));
  } else if (std::strcmp(name, "bitset-raw-string") == 0) {
    static_cast<void>(bitset<Flags>(raw_access, "2"));
  } else if (std::strcmp(name, "array-at") == 0) {
    array<Flags, int> values{};
    static_cast<void>(values.at(both));
  } else if (std::strcmp(name, "bitset-zero") == 0) {
    const bitset<Flags> bits;
    static_cast<void>(bits.test(Flags::None));
  } else if (std::strcmp(name, "bitset-combination") == 0) {
    bitset<Flags> bits;
    bits.set(both);
  }
  std::fputs("no abort\n", stdout);
  return 1;
}
