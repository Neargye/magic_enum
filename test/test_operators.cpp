// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include "test_adl.hpp"
#include "test_enums.hpp"
#include "test_flags_enums.hpp"

#if defined(NEARGYE_MAGIC_ENUM_BITWISE_HPP)
#  error magic_enum.hpp must not provide bitwise operators.
#endif

#include <magic_enum/magic_enum_all.hpp>

#if !defined(NEARGYE_MAGIC_ENUM_BITWISE_HPP)
#  error magic_enum_all.hpp must provide bitwise operators.
#endif

#include "test_operators.hpp"

#include "test_helpers.hpp"

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include "test_auto_flags.hpp"
#include <charconv>
#include <filesystem>

using operators_test::Bits;

// Reads the given characters, then fails like a broken device.
class FailingReadBuf : public std::streambuf {
  std::string chars;

 public:
  explicit FailingReadBuf(std::string s) : chars(std::move(s)) { setg(chars.data(), chars.data(), chars.data() + chars.size()); }

 protected:
  int_type underflow() override { throw std::runtime_error("read error"); }
};

namespace ops {

enum class Scoped : unsigned { A = 1, B = 2 };

enum Unscoped : int { UA = 1, UB = 2 };

enum UnscopedMacro { UMA = 1, UMB = 2 };
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(UnscopedMacro);
MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS(UnscopedMacro);

enum Unfixed { Minus = -1, Zero = 0, One = 1 };
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(Unfixed);

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

  static constexpr bool reflected_flags() noexcept {
    return magic_enum::is_flags_enum_v<Hidden>;
  }

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

// Clang warns about unused functions with internal linkage.
enum class InternalBits : unsigned { A = 1, B = 2 };
MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(InternalBits);

struct InternalHolder {
  enum class Flags : unsigned { A = 1, B = 2 };
  MAGIC_ENUM_DEFINE_FRIEND_BITWISE_OPERATORS(Flags);
};

} // namespace

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

  static_assert(std::is_same_v<decltype(~ops::One), ops::Unfixed>);
  static_assert(static_cast<int>(~ops::One) == -2);
  static_assert(static_cast<int>(magic_enum::bitwise_operators::operator~(ops::One)) == -2);

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
    // A read error after a valid prefix keeps the value.
    FailingReadBuf buf{"A"};
    std::istream stream{&buf};
    auto v = Bits::B;
    stream >> v;
    REQUIRE(stream.bad());
    REQUIRE(v == Bits::B);
  }
  {
    FailingReadBuf buf{"A"};
    std::istream stream{&buf};
    stream.exceptions(std::ios_base::badbit);
    auto v = Bits::B;
    REQUIRE_THROWS_AS(stream >> v, std::runtime_error);
    REQUIRE(v == Bits::B);
  }
  {
    FailingReadBuf buf{"A"};
    std::istream stream{&buf};
    auto v = ops::Scoped::B;
    magic_enum::istream_operators::operator>>(stream, v);
    REQUIRE(stream.bad());
    REQUIRE(v == ops::Scoped::B);
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

namespace common_test {

using namespace magic_enum;
using namespace magic_enum_tests;

TEST_CASE("ostream_operators") {
  SUBCASE("character underlying types") {
    enum class Byte : unsigned char { Named = 1 };
    enum class SignedByte : signed char { Named = 1 };
    enum class Character : char { Named = 1 };
    enum class WideCharacter : wchar_t { Named = 1 };
    enum class Character16 : char16_t { Named = 1 };
    enum class Character32 : char32_t { Named = 1 };

    require_ostream(Byte::Named, "Named");
    require_ostream(static_cast<Byte>(255), "255");
    require_ostream(static_cast<SignedByte>(-1), "-1");
    require_ostream(static_cast<Character>(65), "65");
    require_ostream(static_cast<WideCharacter>(65), "65");
    require_ostream(Character16::Named, "Named");
    require_ostream(static_cast<Character16>(65), "65");
    require_ostream(static_cast<Character32>(65), "65");
    require_ostream(std::make_optional(static_cast<Byte>(255)), "255");
    require_ostream(static_cast<WideCharacter>(65), L"65");
#if defined(__cpp_char8_t)
    enum class Character8 : char8_t { Named = 1 };
    require_ostream(static_cast<Character8>(65), "65");
#endif
  }

  SUBCASE("field width applies to the name") {
    using namespace magic_enum::ostream_operators;

    std::ostringstream named;
    named << std::setfill('_') << std::setw(6) << Color::GREEN << '|' << std::left << std::setw(6) << Color::BLUE << '|' << Color::BLUE << '|';
    REQUIRE(named.str() == "_GREEN|BLUE__|BLUE|");

    std::ostringstream fallback;
    fallback << std::setfill('_') << std::setw(6) << static_cast<Color>(0) << '|' << std::setw(6) << std::make_optional(Color::BLUE) << '|';
    REQUIRE(fallback.str() == "_____0|__BLUE|");

    std::basic_ostringstream<char, CustomCharTraits<char>> custom;
    custom << std::setfill('_') << std::setw(6) << Color::BLUE << '|';
    const auto result = custom.str();
    REQUIRE(std::string(result.data(), result.size()) == "__BLUE|");

    std::wostringstream wide;
    wide << std::setfill(L'_') << std::setw(6) << Color::BLUE << L'|';
    REQUIRE(wide.str() == L"__BLUE|");
  }

  SUBCASE("short underlying type formatting") {
    using namespace magic_enum::ostream_operators;
    enum class Small : short { Named = 1 };

    for (const auto format : {std::dec, std::hex, std::oct}) {
      std::ostringstream actual, expected;
      actual << format << static_cast<Small>(-1);
      expected << format << short{-1};
      REQUIRE(actual.str() == expected.str());
    }
  }

  require_ostream(std::make_optional(Color::RED), "red");
  require_ostream(Color::GREEN, "GREEN");
  require_ostream(Color::BLUE, "BLUE");
  require_ostream(static_cast<Color>(0), "0");
  require_ostream(std::make_optional(static_cast<Color>(0)), "0");

  require_ostream(std::make_optional(Numbers::one), "one");
  require_ostream(Numbers::two, "two");
  require_ostream(Numbers::three, "three");
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  require_ostream(Numbers::many, "many");
#else
  require_ostream(Numbers::many, "127");
#endif
  require_ostream(static_cast<Numbers>(0), "0");
  require_ostream(std::make_optional(static_cast<Numbers>(0)), "0");

  require_ostream(std::make_optional(Directions::Up), "Up");
  require_ostream(Directions::Down, "Down");
  require_ostream(Directions::Right, "Right");
  require_ostream(Directions::Left, "Left");
  require_ostream(static_cast<Directions>(0), "0");
  require_ostream(std::make_optional(static_cast<Directions>(0)), "0");

  require_ostream(std::make_optional(number::one), "one");
  require_ostream(number::two, "two");
  require_ostream(number::three, "three");
#if defined(MAGIC_ENUM_TEST_STD_REFLECTION)
  require_ostream(number::four, "four");
#else
  require_ostream(number::four, "400");
#endif
  require_ostream(static_cast<number>(0), "0");
  require_ostream(std::make_optional(static_cast<number>(0)), "0");
}

enum class StreamNoValues {};

TEST_CASE("iostream_operators without reflected values") {
  static_assert(magic_enum::enum_count<StreamNoValues>() == 0);
  static_assert(magic_enum::enum_count<std::byte>() == 0);

  require_ostream(static_cast<StreamNoValues>(7), "7");
  require_ostream(std::make_optional(static_cast<StreamNoValues>(7)), "7");
  require_ostream(std::byte{5}, "5");
  require_ostream(std::byte{5}, L"5");

  using namespace magic_enum::istream_operators;
  std::istringstream empty{"7"};
  auto value = static_cast<StreamNoValues>(3);
  empty >> value;
  REQUIRE((empty.rdstate() & std::ios::failbit) != 0);
  REQUIRE(value == static_cast<StreamNoValues>(3));

  std::istringstream bytes{"5"};
  auto byte = std::byte{3};
  bytes >> byte;
  REQUIRE((bytes.rdstate() & std::ios::failbit) != 0);
  REQUIRE(byte == std::byte{3});
}

TEST_CASE("istream_operators") {
  require_istream(Color::GREEN, "GREEN");
  require_istream(Color::BLUE, "BLUE");

  require_istream(Numbers::two, "two");
  require_istream(Numbers::three, "three");

  require_istream(Directions::Down, "Down");
  require_istream(Directions::Right, "Right");
  require_istream(Directions::Left, "Left");

  require_istream(number::two, "two");
  require_istream(number::three, "three");
}

TEST_CASE("istream_operators invalid input with custom traits") {
  using namespace magic_enum::istream_operators;
  using String = std::basic_string<char, CustomCharTraits<char>>;
  constexpr char embedded_null[] = "GREEN\0UNKNOWN";

  for (const auto& input : {String{"UNKNOWN"}, String{embedded_null, sizeof(embedded_null) - 1}}) {
    std::basic_istringstream<char, CustomCharTraits<char>> stream{input};
    auto value = Color::BLUE;
    stream >> value;
    REQUIRE((stream.rdstate() & std::ios::failbit) != 0);
    REQUIRE(value == Color::BLUE);
  }
}

TEST_CASE("bitwise_operators") {
  using namespace magic_enum::bitwise_operators;

  SUBCASE("operator^") {
    REQUIRE(enum_integer(~Color::RED) == ~enum_integer(Color::RED));
    REQUIRE(enum_integer(~Numbers::one) == ~enum_integer(Numbers::one));
    REQUIRE(enum_integer(~number::one) == ~enum_integer(number::one));
  }

  SUBCASE("operator|") {
    REQUIRE(enum_integer(Color::RED | Color::BLUE) == (enum_integer(Color::RED) | enum_integer(Color::BLUE)));
    REQUIRE(enum_integer(Numbers::one | Numbers::two) == (enum_integer(Numbers::one) | enum_integer(Numbers::two)));
    REQUIRE(enum_integer(Directions::Up | Directions::Down) == (enum_integer(Directions::Up) | enum_integer(Directions::Down)));
    REQUIRE(enum_integer(number::one | number::two) == (enum_integer(number::one) | enum_integer(number::two)));
  }

  SUBCASE("operator&") {
    REQUIRE(enum_integer(Color::RED & Color::BLUE) == (enum_integer(Color::RED) & enum_integer(Color::BLUE)));
    REQUIRE(enum_integer(Numbers::one & Numbers::two) == (enum_integer(Numbers::one) & enum_integer(Numbers::two)));
    REQUIRE(enum_integer(Directions::Up & Directions::Down) == (enum_integer(Directions::Up) & enum_integer(Directions::Down)));
    REQUIRE(enum_integer(number::one & number::two) == (enum_integer(number::one) & enum_integer(number::two)));
  }

  SUBCASE("operator^") {
    REQUIRE(enum_integer(Color::RED ^ Color::BLUE) == (enum_integer(Color::RED) ^ enum_integer(Color::BLUE)));
    REQUIRE(enum_integer(Numbers::one ^ Numbers::two) == (enum_integer(Numbers::one) ^ enum_integer(Numbers::two)));
    REQUIRE(enum_integer(Directions::Up ^ Directions::Down) == (enum_integer(Directions::Up) ^ enum_integer(Directions::Down)));
    REQUIRE(enum_integer(number::one ^ number::two) == (enum_integer(number::one) ^ enum_integer(number::two)));
  }

  SUBCASE("operator|=") {
    Color x1 = Color::RED;
    x1 |= Color::BLUE;
    REQUIRE(enum_integer(x1) == (enum_integer(Color::RED) | enum_integer(Color::BLUE)));

    Numbers x2 = Numbers::one;
    x2 |= Numbers::two;
    REQUIRE(enum_integer(x2) == (enum_integer(Numbers::one) | enum_integer(Numbers::two)));

    Directions x3 = Directions::Up;
    x3 |= Directions::Down;
    REQUIRE(enum_integer(x3) == (enum_integer(Directions::Up) | enum_integer(Directions::Down)));

    number x4 = number::one;
    x4 |= number::two;
    REQUIRE(enum_integer(x4) == (enum_integer(number::one) | enum_integer(number::two)));
  }

  SUBCASE("operator&=") {
    Color x1 = Color::RED;
    x1 &= Color::BLUE;
    REQUIRE(enum_integer(x1) == (enum_integer(Color::RED) & enum_integer(Color::BLUE)));

    Numbers x2 = Numbers::one;
    x2 &= Numbers::two;
    REQUIRE(enum_integer(x2) == (enum_integer(Numbers::one) & enum_integer(Numbers::two)));

    Directions x3 = Directions::Up;
    x3 &= Directions::Down;
    REQUIRE(enum_integer(x3) == (enum_integer(Directions::Up) & enum_integer(Directions::Down)));

    number x4 = number::one;
    x4 &= number::two;
    REQUIRE(enum_integer(x4) == (enum_integer(number::one) & enum_integer(number::two)));
  }

  SUBCASE("operator^=") {
    Color x1 = Color::RED;
    x1 ^= Color::BLUE;
    REQUIRE(enum_integer(x1) == (enum_integer(Color::RED) ^ enum_integer(Color::BLUE)));

    Numbers x2 = Numbers::one;
    x2 ^= Numbers::two;
    REQUIRE(enum_integer(x2) == (enum_integer(Numbers::one) ^ enum_integer(Numbers::two)));

    Directions x3 = Directions::Up;
    x3 ^= Directions::Down;
    REQUIRE(enum_integer(x3) == (enum_integer(Directions::Up) ^ enum_integer(Directions::Down)));

    number x4 = number::one;
    x4 ^= number::two;
    REQUIRE(enum_integer(x4) == (enum_integer(number::one) ^ enum_integer(number::two)));
  }
}

} // namespace common_test

namespace flags_test {

using namespace magic_enum;
using namespace magic_enum::bitwise_operators;
using namespace magic_enum_tests;

TEST_CASE("flags ostream_operators") {
  SUBCASE("boolalpha fallback") {
    using namespace magic_enum::ostream_operators;
    std::ostringstream stream;
    stream << std::boolalpha << BoolFlags::Disabled << ' ' << static_cast<BoolEnabledFlag>(false);
    REQUIRE(stream.str() == "Disabled false");
  }

  SUBCASE("field width applies to the whole name") {
    using namespace magic_enum::ostream_operators;
    std::ostringstream stream;
    stream << std::setfill('_') << std::setw(10) << (Color::BLUE | Color::RED) << '|' << std::left << std::setw(10) << (Color::BLUE | Color::RED) << '|';
    REQUIRE(stream.str() == "__RED|BLUE|RED|BLUE__|");
  }

  require_ostream(std::make_optional(Color::RED), "RED");
  require_ostream(Color::GREEN, "GREEN");
  require_ostream(Color::BLUE, "BLUE");
  require_ostream(Color::BLUE | Color::RED, "RED|BLUE");
  require_ostream(Color::BLUE | Color::RED | Color::RED, "RED|BLUE");
  require_ostream(static_cast<Color>(0), "0");
  require_ostream(std::make_optional(static_cast<Color>(0)), "0");

  require_ostream(std::make_optional(Numbers::one), "one");
  require_ostream(Numbers::two, "two");
  require_ostream(Numbers::three, "three");
  require_ostream(Numbers::many, "many");
  require_ostream(static_cast<Numbers>(0), "none");
  require_ostream(std::make_optional(static_cast<Numbers>(0)), "none");

  require_ostream(std::make_optional(Directions::Up), "Up");
  require_ostream(Directions::Down, "Down");
  require_ostream(Directions::Right, "Right");
  require_ostream(Directions::Left, "Left");
  require_ostream(Directions::Right | Directions::Left, "Left|Right");
  require_ostream(static_cast<Directions>(0), "NoDirection");
  require_ostream(std::make_optional(static_cast<Directions>(0)), "NoDirection");

  require_ostream(std::make_optional(number::one), "one");
  require_ostream(number::two, "two");
  require_ostream(number::three, "three");
  require_ostream(number::four, "four");
  require_ostream(number::four | number::one, "one|four");
  require_ostream(static_cast<number>(0), "no_number");
  require_ostream(std::make_optional(static_cast<number>(0)), "no_number");
}

TEST_CASE("flags istream_operators") {
  require_istream(Color::GREEN, "GREEN");
  require_istream(Color::BLUE, "BLUE");
  require_istream(Color::BLUE | Color::RED, "RED|BLUE");
  require_istream(Color::BLUE | Color::RED | Color::RED, "RED|BLUE");
  require_istream(Color::GREEN | Color::RED, "GREEN|RED|RED");

  require_istream(Numbers::two, "two");
  require_istream(Numbers::three, "three");
  require_istream(Numbers::many, "many");

  require_istream(Directions::Down, "Down");
  require_istream(Directions::Right, "Right");
  require_istream(Directions::Left, "Left");
  require_istream(Directions::Right | Directions::Left, "Left|Right");

  require_istream(number::two, "two");
  require_istream(number::three, "three");
  require_istream(number::four, "four");
  require_istream(number::four | number::one, "one|four");
}

TEST_CASE("flags istream_operators invalid input with custom traits") {
  using namespace magic_enum::istream_operators;
  using String = std::basic_string<char, CustomCharTraits<char>>;
  constexpr char embedded_null[] = "RED\0|BLUE";

  for (const auto& input : {String{"UNKNOWN"}, String{"RED|UNKNOWN"}, String{embedded_null, sizeof(embedded_null) - 1}}) {
    std::basic_istringstream<char, CustomCharTraits<char>> stream{input};
    auto value = Color::BLUE;
    stream >> value;
    REQUIRE((stream.rdstate() & std::ios::failbit) != 0);
    REQUIRE(value == Color::BLUE);
  }
}

TEST_CASE("flags bitwise_operators") {
  SUBCASE("operator^") {
    REQUIRE(enum_integer(~Color::RED) == ~enum_integer(Color::RED));
    REQUIRE(enum_integer(~Numbers::one) == ~enum_integer(Numbers::one));
    REQUIRE(enum_integer(~Directions::Up) == ~enum_integer(Directions::Up));
    REQUIRE(enum_integer(~number::one) == ~enum_integer(number::one));
  }

  SUBCASE("operator|") {
    REQUIRE(enum_integer(Color::RED | Color::BLUE) == (enum_integer(Color::RED) | enum_integer(Color::BLUE)));
    REQUIRE(enum_integer(Numbers::one | Numbers::two) == (enum_integer(Numbers::one) | enum_integer(Numbers::two)));
    REQUIRE(enum_integer(Directions::Up | Directions::Down) == (enum_integer(Directions::Up) | enum_integer(Directions::Down)));
    REQUIRE(enum_integer(number::one | number::two) == (enum_integer(number::one) | enum_integer(number::two)));
  }

  SUBCASE("operator&") {
    REQUIRE(enum_integer(Color::RED & Color::BLUE) == (enum_integer(Color::RED) & enum_integer(Color::BLUE)));
    REQUIRE(enum_integer(Numbers::one & Numbers::two) == (enum_integer(Numbers::one) & enum_integer(Numbers::two)));
    REQUIRE(enum_integer(Directions::Up & Directions::Down) == (enum_integer(Directions::Up) & enum_integer(Directions::Down)));
    REQUIRE(enum_integer(number::one & number::two) == (enum_integer(number::one) & enum_integer(number::two)));
  }

  SUBCASE("operator^") {
    REQUIRE(enum_integer(Color::RED ^ Color::BLUE) == (enum_integer(Color::RED) ^ enum_integer(Color::BLUE)));
    REQUIRE(enum_integer(Numbers::one ^ Numbers::two) == (enum_integer(Numbers::one) ^ enum_integer(Numbers::two)));
    REQUIRE(enum_integer(Directions::Up ^ Directions::Down) == (enum_integer(Directions::Up) ^ enum_integer(Directions::Down)));
    REQUIRE(enum_integer(number::one ^ number::two) == (enum_integer(number::one) ^ enum_integer(number::two)));
  }

  SUBCASE("operator|=") {
    Color x1 = Color::RED;
    x1 |= Color::BLUE;
    REQUIRE(enum_integer(x1) == (enum_integer(Color::RED) | enum_integer(Color::BLUE)));

    Numbers x2 = Numbers::one;
    x2 |= Numbers::two;
    REQUIRE(enum_integer(x2) == (enum_integer(Numbers::one) | enum_integer(Numbers::two)));

    Directions x3 = Directions::Up;
    x3 |= Directions::Down;
    REQUIRE(enum_integer(x3) == (enum_integer(Directions::Up) | enum_integer(Directions::Down)));

    number x4 = number::one;
    x4 |= number::two;
    REQUIRE(enum_integer(x4) == (enum_integer(number::one) | enum_integer(number::two)));
  }

  SUBCASE("operator&=") {
    Color x1 = Color::RED;
    x1 &= Color::BLUE;
    REQUIRE(enum_integer(x1) == (enum_integer(Color::RED) & enum_integer(Color::BLUE)));

    Numbers x2 = Numbers::one;
    x2 &= Numbers::two;
    REQUIRE(enum_integer(x2) == (enum_integer(Numbers::one) & enum_integer(Numbers::two)));

    Directions x3 = Directions::Up;
    x3 &= Directions::Down;
    REQUIRE(enum_integer(x3) == (enum_integer(Directions::Up) & enum_integer(Directions::Down)));

    number x4 = number::one;
    x4 &= number::two;
    REQUIRE(enum_integer(x4) == (enum_integer(number::one) & enum_integer(number::two)));
  }

  SUBCASE("operator^=") {
    Color x1 = Color::RED;
    x1 ^= Color::BLUE;
    REQUIRE(enum_integer(x1) == (enum_integer(Color::RED) ^ enum_integer(Color::BLUE)));

    Numbers x2 = Numbers::one;
    x2 ^= Numbers::two;
    REQUIRE(enum_integer(x2) == (enum_integer(Numbers::one) ^ enum_integer(Numbers::two)));

    Directions x3 = Directions::Up;
    x3 ^= Directions::Down;
    REQUIRE(enum_integer(x3) == (enum_integer(Directions::Up) ^ enum_integer(Directions::Down)));

    number x4 = number::one;
    x4 ^= number::two;
    REQUIRE(enum_integer(x4) == (enum_integer(number::one) ^ enum_integer(number::two)));
  }
}

} // namespace flags_test

TEST_CASE("automatic flag reflection") {
  using namespace auto_flags_test;
  REQUIRE(adl_lookup_in_other_tu());
  REQUIRE(magic_enum::is_flags_enum_v<ops::UnscopedMacro> == enabled);
  REQUIRE(magic_enum::is_flags_enum_v<ops::Signed> == enabled);
  REQUIRE(magic_enum::is_flags_enum_v<ops::BoolEnum> == enabled);
  REQUIRE(magic_enum::is_flags_enum_v<ops::HolderFlags> == enabled);
  REQUIRE(ops::Private::reflected_flags() == enabled);
  // Automatic detection rejects these standard enums, since they name combinations.
#if !defined(_GLIBCXX_RELEASE) || _GLIBCXX_RELEASE >= 11 // libstdc++ declares chars_format since GCC 11.
#  ifdef MAGIC_ENUM_TEST_NO_AUTO_IS_FLAGS
  REQUIRE_FALSE(magic_enum::is_flags_enum_v<std::chars_format>);
  REQUIRE(magic_enum::enum_name(std::chars_format::general) == "general");
#  endif
  REQUIRE(magic_enum::enum_flags_name(std::chars_format::general) == "scientific|fixed");
#endif
#ifdef MAGIC_ENUM_TEST_NO_AUTO_IS_FLAGS
  REQUIRE_FALSE(magic_enum::is_flags_enum_v<std::filesystem::perms>);
#endif
  REQUIRE(magic_enum::enum_flags_name(std::filesystem::perms::unknown).empty());
  constexpr auto ab = static_cast<auto_flags_test::Bits>(3);
  REQUIRE(magic_enum::enum_flags_name(ab) == "A|B");
  REQUIRE_FALSE(magic_enum::enum_flags_contains(static_cast<auto_flags_test::Bits>(0xffffffffu)));
  using namespace magic_enum::iostream_operators;
  std::stringstream stream;
  stream << ab;
  REQUIRE(stream.str() == (enabled ? "A|B" : "3"));
  if constexpr (enabled) {
    auto parsed = auto_flags_test::Bits::None;
    stream >> parsed;
    REQUIRE(parsed == ab);
    REQUIRE_FALSE(stream.fail());
  }
  std::size_t calls = 0;
  magic_enum::enum_for_each<Range>([&](auto) { ++calls; });
  REQUIRE(calls == 4);
}

TEST_CASE("bitwise macros allow unused operators with internal linkage") {
  static_assert((InternalBits::A | InternalBits::B) == static_cast<InternalBits>(3));
  static_assert((InternalHolder::Flags::A | InternalHolder::Flags::B) == static_cast<InternalHolder::Flags>(3));
}
