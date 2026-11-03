// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT

export module magic_enum_test_types;

export namespace exported_flags {
enum class Bits { None = 0, A = 1, B = 2, All = 3 };
constexpr Bits operator|(Bits a, Bits b) { return static_cast<Bits>(static_cast<int>(a) | static_cast<int>(b)); }
constexpr Bits operator&(Bits a, Bits b) { return static_cast<Bits>(static_cast<int>(a) & static_cast<int>(b)); }
}
