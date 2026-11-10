// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT

#ifndef MAGIC_ENUM_TEST_AUTO_FLAGS_HPP
#define MAGIC_ENUM_TEST_AUTO_FLAGS_HPP

// Shared by header and module consumers. No reflection-dependent operator constraints.
namespace auto_flags_test {

#ifdef MAGIC_ENUM_TEST_NO_AUTO_IS_FLAGS
inline constexpr bool enabled = false;
#else
inline constexpr bool enabled = true;
#endif

enum class Bits : unsigned { None = 0, A = 1, B = 2, All = 3, High = 1024, ForceDword = 0xffffffff };
constexpr Bits operator|(Bits a, Bits b) { return static_cast<Bits>(static_cast<unsigned>(a) | static_cast<unsigned>(b)); }
constexpr Bits operator&(Bits a, Bits b) { return static_cast<Bits>(static_cast<unsigned>(a) & static_cast<unsigned>(b)); }

enum class Common { None = 0, A = 1, B = 2, All = 3 };
Common operator|(Common, Common);
Common operator&(Common, Common);
inline constexpr auto magic_enum_define_range_adl(Common) { return magic_enum::customize::adl_info().flag<false>(); }

enum class Explicit { A = 1, B = 2, All = 3 };
inline constexpr auto magic_enum_define_range_adl(Explicit) { return magic_enum::customize::adl_info().flag<true>(); }

enum class Range { rNone = 0, rA = 1, rB = 2, rAll = 3 };
Range operator|(Range, Range);
Range operator&(Range, Range);
inline constexpr auto magic_enum_define_range_adl(Range) { return magic_enum::customize::adl_info().prefix<1>().minmax<0, 3>(); }

enum class Specialized { A = 1, B = 2, All = 3 };
Specialized operator|(Specialized, Specialized);
Specialized operator&(Specialized, Specialized);
inline constexpr auto magic_enum_define_range_adl(Specialized) { return magic_enum::customize::adl_info().flag<false>().prefix<1>(); }

enum class Override { A = 1, B = 2, All = 3 };
inline constexpr auto magic_enum_define_range_adl(Override) { return magic_enum::customize::adl_info().flag<true>(); }

enum class OrOnly { A = 1, B = 2 };
OrOnly operator|(OrOnly, OrOnly);
enum class AndOnly { A = 1, B = 2 };
AndOnly operator&(AndOnly, AndOnly);
enum class BoolAnd { A = 1, B = 2 };
BoolAnd operator|(BoolAnd, BoolAnd);
bool operator&(BoolAnd, BoolAnd);
enum class Ref { A = 1, B = 2 };
Ref& operator|(Ref, Ref);
Ref& operator&(Ref, Ref);
enum class Lvalue { A = 1, B = 2 };
Lvalue operator|(Lvalue&, Lvalue&);
Lvalue operator&(Lvalue&, Lvalue&);
enum class Deleted { A = 1, B = 2 };
Deleted operator|(Deleted, Deleted) = delete;
Deleted operator&(Deleted, Deleted) = delete;
enum class Ambiguous { A = 1, B = 2 };
Ambiguous operator|(Ambiguous, const Ambiguous&);
Ambiguous operator|(const Ambiguous&, Ambiguous);
Ambiguous operator&(Ambiguous, Ambiguous);
enum class Wrapped { A = 1, B = 2 };
struct Wrapper { operator Wrapped() const; };
Wrapper operator|(Wrapped, Wrapped);
Wrapper operator&(Wrapped, Wrapped);
enum class Integer { A = 1, B = 2 };
int operator|(Integer, Integer);
int operator&(Integer, Integer);
enum Unscoped { UA = 1, UB = 2, UAll = 3 };
enum class StreamOnly { A = 1, B = 2 };
template <typename T> T& operator<<(T&, StreamOnly);

namespace operators {
template <typename E, typename = decltype(E::Largest)> E operator|(E, E);
template <typename E, typename = decltype(E::Largest)> E operator&(E, E);
}
namespace imported {
enum class Bits { A = 1, B = 2, Largest = B };
using operators::operator|;
using operators::operator&;
}

namespace lazy {

template <typename T>
struct Poison {
  static_assert(!std::is_same_v<T, T>, "The auto detector must not be instantiated.");
  using type = T;
};
template <typename T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
typename Poison<T>::type operator|(T, T);
template <typename T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
typename Poison<T>::type operator&(T, T);

enum class Lazy { A = 1, B = 2, All = 3 };

enum class LazyExplicit { A = 1, B = 2, All = 3 };
inline constexpr auto magic_enum_define_range_adl(LazyExplicit) { return magic_enum::customize::adl_info().flag<false>(); }

enum class LazyFlags { A = 1, B = 2, All = 3 };
inline constexpr auto magic_enum_define_range_adl(LazyFlags) { return magic_enum::customize::adl_info().flag<true>(); }

} // namespace lazy
using lazy::Lazy;
using lazy::LazyExplicit;

// The detector must not instantiate generic pipe operators with deduced return types.
namespace piped {
template <typename T, typename F>
constexpr auto operator|(T&& value, F&& f) { return f(value); }

enum class Color { Red, Green };
} // namespace piped

// 0.9 hooks may name the holder type explicitly.
enum class Legacy { A = 10, B = 11 };
inline constexpr magic_enum::customize::adl_info_holder<false, 10, 11> magic_enum_define_range_adl(Legacy) { return magic_enum::customize::adl_info().minmax<10, 11>(); }

} // namespace auto_flags_test

template <>
struct magic_enum::customize::enum_range<auto_flags_test::Specialized> {
  static constexpr int min = 0;
  static constexpr int max = 3;
};
template <>
struct magic_enum::customize::enum_range<auto_flags_test::Override> {
  static constexpr bool is_flags = false;
};

namespace auto_flags_test {

static_assert(magic_enum::is_flags_enum_v<Bits> == enabled);
static_assert(magic_enum::is_flags_enum_v<const Bits> == enabled);
static_assert(!magic_enum::is_flags_enum_v<Common>);
static_assert(magic_enum::is_flags_enum_v<Explicit>);
static_assert(magic_enum::is_flags_enum_v<Range> == enabled);
static_assert(magic_enum::enum_name(Range::rA) == "A");
static_assert(magic_enum::is_flags_enum_v<Specialized> == enabled);
static_assert(magic_enum::enum_name(Specialized::A) == "A");
static_assert(!magic_enum::is_flags_enum_v<Override>);
static_assert(!magic_enum::is_flags_enum_v<OrOnly> && !magic_enum::is_flags_enum_v<AndOnly>);
static_assert(!magic_enum::is_flags_enum_v<BoolAnd> && !magic_enum::is_flags_enum_v<Ref>);
static_assert(!magic_enum::is_flags_enum_v<Lvalue> && !magic_enum::is_flags_enum_v<Deleted>);
static_assert(!magic_enum::is_flags_enum_v<Ambiguous> && !magic_enum::is_flags_enum_v<Wrapped>);
static_assert(!magic_enum::is_flags_enum_v<Integer> && !magic_enum::is_flags_enum_v<Unscoped>);
static_assert(!magic_enum::is_flags_enum_v<StreamOnly>);
static_assert(!magic_enum::is_flags_enum_v<piped::Color> && magic_enum::enum_name(piped::Color::Green) == "Green");
static_assert(!magic_enum::is_flags_enum_v<Legacy> && magic_enum::enum_count<Legacy>() == 2);
static_assert(magic_enum::is_flags_enum_v<imported::Bits> == enabled);
static_assert(magic_enum::enum_count<imported::Bits>() == 2);
static_assert(magic_enum::enum_cast<imported::Bits>(magic_enum::enum_name(imported::Bits::B)).value() == imported::Bits::B);
#ifdef MAGIC_ENUM_TEST_STD_REFLECTION
static_assert(magic_enum::enum_name(imported::Bits::B) == "B");
#endif
static_assert(!magic_enum::is_flags_enum_v<LazyExplicit>);
static_assert(magic_enum::is_flags_enum_v<lazy::LazyFlags>);
static_assert(magic_enum::enum_count<Lazy, magic_enum::as_common<>>() == 3);
static_assert(magic_enum::enum_count<Lazy, magic_enum::as_flags<>>() == 2);
#ifdef MAGIC_ENUM_TEST_NO_AUTO_IS_FLAGS
static_assert(!magic_enum::is_flags_enum_v<Lazy>);
#endif

static_assert(magic_enum::enum_contains<Bits>(Bits::All) == !enabled);
static_assert(magic_enum::enum_name(Bits::None) == "None");
static_assert(magic_enum::enum_contains<Bits, magic_enum::as_flags<>>(Bits::High));
#ifndef MAGIC_ENUM_TEST_STD_REFLECTION
static_assert(magic_enum::enum_contains<Bits>(Bits::High) == enabled);
#endif
static_assert(magic_enum::enum_cast<Bits>("All").has_value() == !enabled);
static_assert(magic_enum::enum_contains<Bits, magic_enum::as_common<>>(Bits::All));
static_assert(!magic_enum::enum_contains<Bits, magic_enum::as_flags<>>(Bits::All));
static_assert(magic_enum::enum_flags_contains(Bits::All));
static_assert(magic_enum::enum_flags_cast<Bits>("A|B").value() == Bits::All);
static_assert(magic_enum::enum_count<Range>() == (enabled ? 3 : 4));
static_assert(magic_enum::enum_reflected<Range>(3) == !enabled);
static_assert(magic_enum::containers::array<Range, int>{}.size() == (enabled ? 3 : 4));
static_assert(magic_enum::containers::bitset<Range>{}.size() == (enabled ? 2 : 4));
#ifndef MAGIC_ENUM_TEST_NO_AUTO_IS_FLAGS
static_assert(magic_enum::containers::set<Range>::from_mask(Range::rAll).size() == 2);
#else
static_assert(magic_enum::containers::set<Range>{Range::rAll}.size() == 1);
#endif

struct ReturnsOne {
  template <typename T>
  constexpr int operator()(T) const { return 1; }
};
static_assert(magic_enum::enum_switch<int>(ReturnsOne{}, Bits::All, -1) == (enabled ? -1 : 1));
static_assert(magic_enum::enum_next_value(Range::rB).has_value() == !enabled);
static_assert(magic_enum::enum_fuse(Range::rB, Explicit::A).has_value());

template <typename T, typename = void> struct has_flag : std::false_type {};
template <typename T> struct has_flag<T, std::void_t<decltype(T::is_flags)>> : std::true_type {};
using Auto = decltype(magic_enum::customize::adl_info().minmax<1, 4>().prefix<2>());
static_assert(!has_flag<Auto>::value && Auto::min == 1 && Auto::max == 4 && Auto::prefix_length == 2);
using Selected = decltype(Auto::flag<true>().minmax<0, 7>().prefix<1>().flag<false>());
static_assert(!Selected::is_flags && Selected::min == 0 && Selected::max == 7 && Selected::prefix_length == 1);
static_assert(std::is_same_v<Selected, magic_enum::customize::adl_info_holder<false, 0, 7, 1>>);
static_assert(!magic_enum::customize::adl_info_holder<>::is_flags);
using FromHolder = decltype(magic_enum::customize::adl_info_holder<>::minmax<-2, 8>().flag<true>().prefix<2>());
static_assert(std::is_same_v<FromHolder, magic_enum::customize::adl_info_holder<true, -2, 8, 2>>);
static_assert(std::is_same_v<decltype(Auto::minmax<1, 4>().prefix<2>()), Auto>);
using Automatic = decltype(magic_enum::customize::adl_info());
static_assert(!has_flag<Automatic>::value);
static_assert(!has_flag<decltype(Automatic::prefix<2>().minmax<-2, 8>())>::value);
static_assert(std::is_same_v<decltype(Automatic::flag<false>().flag<true>().prefix<2>().minmax<-2, 8>()), FromHolder>);

} // namespace auto_flags_test
#endif
