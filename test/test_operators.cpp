// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include <magic_enum/magic_enum.hpp>

#if defined(NEARGYE_MAGIC_ENUM_BITWISE_HPP)
#  error magic_enum.hpp must not provide bitwise operators.
#endif

#include <magic_enum/magic_enum_all.hpp>

#if !defined(NEARGYE_MAGIC_ENUM_BITWISE_HPP)
#  error magic_enum_all.hpp must provide bitwise operators.
#endif

#include "test_operators.hpp"

#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

using operators_test::Bits;

namespace ops {

enum class Scoped : unsigned { A = 1, B = 2 };

enum Unscoped : int { UA = 1, UB = 2 };

enum UnscopedMacro : int { UMA = 1, UMB = 2 };
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(UnscopedMacro);
MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS(UnscopedMacro);

enum class Signed : std::int8_t { Low = 1, High = std::numeric_limits<std::int8_t>::min() };
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(Signed);

enum class BoolEnum : bool { Off = false, On = true };
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(BoolEnum);

struct Public {
  enum class Nested : unsigned { A = 1, B = 2 };
};
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(Public::Nested);
MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS(Public::Nested);

// Members named like ordinary operator parameters check -Wshadow in the friend macros.
class Private {
  enum class Hidden : unsigned { A = 1, B = 2 };
  MAGIC_ENUM_DEFINE_FRIEND_BITWISE_OPERATORS(Hidden);
  MAGIC_ENUM_DEFINE_FRIEND_IOSTREAM_OPERATORS(Hidden);

 public:
  int lhs = 0;
  int rhs = 0;
  int value = 0;
  int os = 0;
  int is = 0;

  static constexpr unsigned combined() noexcept {
    return static_cast<unsigned>(Hidden::A | Hidden::B);
  }

  static std::string printed() {
    std::ostringstream stream;
    stream << Hidden::B;
    return stream.str();
  }

  static bool parsed() {
    std::istringstream stream{"A"};
    auto v = Hidden::B;
    stream >> v;
    return !stream.fail() && v == Hidden::A;
  }
};

template <typename Char, typename Traits>
struct Holder {
  enum class Flags : unsigned { A = 1, B = 2 };
  MAGIC_ENUM_DEFINE_FRIEND_BITWISE_OPERATORS(Flags);
  MAGIC_ENUM_DEFINE_FRIEND_IOSTREAM_OPERATORS(Flags);
};

using HolderFlags = Holder<char, std::char_traits<char>>::Flags;

namespace foreign {

enum class Own : unsigned { A = 1 };

constexpr int operator|(Own, Own) noexcept {
  return 42;
}

} // namespace foreign

} // namespace ops

namespace {

template <typename E, typename = void>
struct has_or : std::false_type {};

template <typename E>
struct has_or<E, std::void_t<decltype(std::declval<E>() | std::declval<E>())>> : std::true_type {};

template <typename E, typename = void>
struct has_compl : std::false_type {};

template <typename E>
struct has_compl<E, std::void_t<decltype(~std::declval<E>())>> : std::true_type {};

template <typename E, typename = void>
struct has_or_assign : std::false_type {};

template <typename E>
struct has_or_assign<E, std::void_t<decltype(std::declval<E&>() |= std::declval<E>())>> : std::true_type {};

template <typename E, typename = void>
struct has_insert : std::false_type {};

template <typename E>
struct has_insert<E, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<E>())>> : std::true_type {};

template <typename E, typename = void>
struct has_extract : std::false_type {};

template <typename E>
struct has_extract<E, std::void_t<decltype(std::declval<std::istream&>() >> std::declval<E&>())>> : std::true_type {};

constexpr Bits compound() noexcept {
  auto v = Bits::A;
  v |= Bits::B;
  v &= Bits::B;
  v ^= Bits::C;
  return v;
}

template <typename E>
std::string print(E value) {
  std::ostringstream stream;
  stream << value;
  return stream.str();
}

} // namespace

TEST_CASE("bitwise macros") {
  static_assert(std::is_same_v<decltype(Bits::A | Bits::B), Bits>);
  static_assert(std::is_same_v<decltype(std::declval<Bits&>() |= Bits::A), Bits&>);
  static_assert(std::is_same_v<decltype(std::declval<Bits&>() &= Bits::A), Bits&>);
  static_assert(std::is_same_v<decltype(std::declval<Bits&>() ^= Bits::A), Bits&>);
  static_assert(noexcept(~Bits::A) && noexcept(Bits::A | Bits::B) && noexcept(std::declval<Bits&>() |= Bits::A));

  static_assert((Bits::A | Bits::B) == static_cast<Bits>(3));
  static_assert((Bits::A & Bits::B) == static_cast<Bits>(0));
  static_assert((Bits::A ^ Bits::C) == static_cast<Bits>(5));
  static_assert(~Bits::A == static_cast<Bits>(~1u));
  static_assert(compound() == (Bits::B | Bits::C));

  static_assert(~ops::BoolEnum::Off == ops::BoolEnum::On);
  static_assert(~ops::BoolEnum::On == ops::BoolEnum::Off);
  static_assert((ops::BoolEnum::Off | ops::BoolEnum::On) == ops::BoolEnum::On);
  static_assert((ops::BoolEnum::Off ^ ops::BoolEnum::On) == ops::BoolEnum::On);

  static_assert((ops::Signed::Low | ops::Signed::High) == static_cast<ops::Signed>(-127));
  static_assert(~ops::Signed::High == static_cast<ops::Signed>(127));
  static_assert((ops::Signed::High & ops::Signed::High) == ops::Signed::High);

  static_assert((ops::Public::Nested::A | ops::Public::Nested::B) == static_cast<ops::Public::Nested>(3));
  static_assert(ops::Private::combined() == 3);
  static_assert((ops::HolderFlags::A | ops::HolderFlags::B) == static_cast<ops::HolderFlags>(3));

  REQUIRE(operators_test::combine_in_other_tu(Bits::A, Bits::C) == (Bits::A | Bits::C));
}

TEST_CASE("friend operators in class template specializations") {
  using First = ops::HolderFlags;
  using Second = ops::Holder<wchar_t, std::char_traits<wchar_t>>::Flags;

  static_assert(!std::is_same_v<First, Second>);
  static_assert(std::is_same_v<decltype(First::A | First::B), First>);
  static_assert(std::is_same_v<decltype(Second::A | Second::B), Second>);
  static_assert((First::A | First::B) == static_cast<First>(3));
  static_assert((Second::A | Second::B) == static_cast<Second>(3));

  std::stringstream stream;
  stream << First::A << ' ' << Second::B;
  REQUIRE(stream.str() == "A B");

  auto first = First::B;
  auto second = Second::A;
  stream >> first >> second;
  REQUIRE_FALSE(stream.fail());
  REQUIRE(first == First::A);
  REQUIRE(second == Second::B);
}

TEST_CASE("bitwise macros affect only their enum") {
  static_assert(!has_or<ops::Scoped>::value);
  static_assert(!has_compl<ops::Scoped>::value);
  static_assert(!has_or_assign<ops::Scoped>::value);

  // Built-in operators keep their integer result for unscoped enums without the macro.
  static_assert(std::is_same_v<decltype(ops::UA | ops::UB), int>);
  static_assert(!has_or_assign<ops::Unscoped>::value);
  static_assert(std::is_same_v<decltype(ops::UMA | ops::UMB), ops::UnscopedMacro>);
  static_assert(std::is_same_v<decltype(~ops::UMA), ops::UnscopedMacro>);
  static_assert(has_or_assign<ops::UnscopedMacro>::value);

  static_assert((ops::foreign::Own::A | ops::foreign::Own::A) == 42);
}

TEST_CASE("bitwise macros with the namespace") {
  using namespace magic_enum::bitwise_operators;

  static_assert(std::is_same_v<decltype(Bits::A | Bits::B), Bits>);
  static_assert((Bits::A | Bits::B) == static_cast<Bits>(3));
  static_assert(compound() == (Bits::B | Bits::C));
  static_assert(std::is_same_v<decltype(ops::UMA | ops::UMB), ops::UnscopedMacro>);

  static_assert((ops::Scoped::A | ops::Scoped::B) == static_cast<ops::Scoped>(3));
  static_assert(std::is_same_v<decltype(ops::UA | ops::UB), ops::Unscoped>);
  static_assert((ops::foreign::Own::A | ops::foreign::Own::A) == 42);

  auto v = Bits::A;
  v |= Bits::C;
  REQUIRE(v == static_cast<Bits>(5));
}

TEST_CASE("iostream macros") {
  REQUIRE(print(Bits::A) == "A");
  REQUIRE(print(Bits::A | Bits::C) == "A|C");
  REQUIRE(print(static_cast<Bits>(8)) == "8");
  REQUIRE(operators_test::print_in_other_tu(Bits::B) == "B");

  REQUIRE(print(ops::UMA) == "UMA");
  REQUIRE(print(ops::Public::Nested::B) == "B");
  REQUIRE(ops::Private::printed() == "B");
  REQUIRE(ops::Private::parsed());
  REQUIRE(print(ops::HolderFlags::A) == "A");

  {
    std::istringstream stream{"B"};
    auto v = Bits::A;
    stream >> v;
    REQUIRE_FALSE(stream.fail());
    REQUIRE(v == Bits::B);
  }
  {
    std::istringstream stream{"A|C"};
    auto v = Bits::B;
    stream >> v;
    REQUIRE_FALSE(stream.fail());
    REQUIRE(v == (Bits::A | Bits::C));
  }
  {
    std::istringstream stream{"Unknown"};
    auto v = Bits::B;
    stream >> v;
    REQUIRE(stream.fail());
    REQUIRE(v == Bits::B);
  }
  {
    std::wostringstream out;
    out << Bits::C;
    REQUIRE(out.str() == L"C");
  }
}

TEST_CASE("iostream macros affect only their enum") {
  static_assert(!has_insert<ops::Scoped>::value);
  static_assert(!has_extract<ops::Scoped>::value);
  static_assert(has_extract<Bits>::value);

  // Unscoped enums print as numbers until the macro is used.
  REQUIRE(print(ops::UA) == "1");
  REQUIRE(print(ops::UMA) == "UMA");
}

TEST_CASE("iostream macros with the namespace") {
  using namespace magic_enum::iostream_operators;

  std::ostringstream out;
  // The using directive also switches unscoped enums without the macro from numbers to names.
  out << Bits::A << ' ' << ops::UMB << ' ' << ops::Scoped::B << ' ' << ops::UA;
  REQUIRE(out.str() == "A UMB B UA");

  std::istringstream in{"C"};
  auto v = Bits::A;
  in >> v;
  REQUIRE(v == Bits::C);
}
