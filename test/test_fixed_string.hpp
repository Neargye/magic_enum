// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#pragma once

// No includes, so module tests can use it after import.
namespace magic_enum_tests {

// Constexpr String for enum_flags_name and bitset::to_string. Overflow drops characters and sets a flag.
template <decltype(sizeof(0)) N, typename Char = char>
struct FixedString {
  using value_type = Char;
  using size_type = decltype(sizeof(0));

  constexpr bool empty() const noexcept { return length == 0; }

  constexpr size_type size() const noexcept { return length; }

  constexpr const Char* data() const noexcept { return chars; }

  constexpr void reserve(size_type capacity) noexcept { reserved = capacity; }

  constexpr void clear() noexcept { length = 0; }

  constexpr void append(size_type count, Char c) noexcept {
    for (size_type i = 0; i < count; ++i) {
      push(c);
    }
  }

  constexpr void append(const Char* s, size_type count) noexcept {
    for (size_type i = 0; i < count; ++i) {
      push(s[i]);
    }
  }

  constexpr void push(Char c) noexcept {
    if (length < N) {
      chars[length++] = c;
    } else {
      overflow = true;
    }
  }

  // Compares with a null-terminated literal.
  constexpr bool operator==(const Char* s) const noexcept {
    size_type i = 0;
    for (; i < length; ++i) {
      if (s[i] != chars[i]) {
        return false;
      }
    }
    return s[i] == Char{};
  }

  Char chars[N + 1]{};
  size_type length = 0;
  size_type reserved = 0;
  bool overflow = false;
};

} // namespace magic_enum_tests
