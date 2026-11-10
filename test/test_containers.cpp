// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.
// Copyright (c) 2022 - 2023 Bela Schaum <schaumb@gmail.com>.

#include <doctest/doctest.h>

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_bitwise.hpp>
#define max(a, b) MAGIC_ENUM_TEST_MAX_MACRO
#include <magic_enum/magic_enum_containers.hpp>
#undef max

#if defined(MAGIC_ENUM_THROW) || defined(MAGIC_ENUM_CONTAINERS_THROW)
#  error Internal containers macro leaked from magic_enum_containers.hpp.
#endif

#include <magic_enum/magic_enum_iostream.hpp>

#include "test_fixed_string.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

enum class Color { RED = 1, GREEN = 2, BLUE = 4 };
template <>
struct magic_enum::customize::enum_range<Color> {
  static constexpr bool is_flags = true;
};

enum class BoolFlags : bool { None = false, Enabled = true };
template <>
struct magic_enum::customize::enum_range<BoolFlags> {
  static constexpr bool is_flags = true;
};

enum class ZeroFlags : std::uint8_t { None = 0, A = 1, B = 2 };
template <>
struct magic_enum::customize::enum_range<ZeroFlags> {
  static constexpr bool is_flags = true;
};

enum class NoEqualFlags : std::uint8_t { None = 0, A = 1, B = 2 };
template <>
struct magic_enum::customize::enum_range<NoEqualFlags> {
  static constexpr bool is_flags = true;
};
constexpr bool operator==(NoEqualFlags, NoEqualFlags) = delete;

enum class OnlyZeroFlags : std::uint8_t { None = 0 };
template <>
struct magic_enum::customize::enum_range<OnlyZeroFlags> {
  static constexpr bool is_flags = true;
};

enum class SignedHighFlags : std::int8_t { High = -128, None = 0, Low = 1 };
template <>
struct magic_enum::customize::enum_range<SignedHighFlags> {
  static constexpr bool is_flags = true;
};

enum class ZeroFlags8 : std::uint8_t {
  None = 0,
  B0 = std::uint8_t{1} << 0, B1 = std::uint8_t{1} << 1, B2 = std::uint8_t{1} << 2, B3 = std::uint8_t{1} << 3,
  B4 = std::uint8_t{1} << 4, B5 = std::uint8_t{1} << 5, B6 = std::uint8_t{1} << 6, B7 = std::uint8_t{1} << 7,
};
template <>
struct magic_enum::customize::enum_range<ZeroFlags8> {
  static constexpr bool is_flags = true;
};

enum class ZeroFlags64 : std::uint64_t {
  None = 0,
  B0 = std::uint64_t{1} << 0, B1 = std::uint64_t{1} << 1, B2 = std::uint64_t{1} << 2, B3 = std::uint64_t{1} << 3,
  B4 = std::uint64_t{1} << 4, B5 = std::uint64_t{1} << 5, B6 = std::uint64_t{1} << 6, B7 = std::uint64_t{1} << 7,
  B8 = std::uint64_t{1} << 8, B9 = std::uint64_t{1} << 9, B10 = std::uint64_t{1} << 10, B11 = std::uint64_t{1} << 11,
  B12 = std::uint64_t{1} << 12, B13 = std::uint64_t{1} << 13, B14 = std::uint64_t{1} << 14, B15 = std::uint64_t{1} << 15,
  B16 = std::uint64_t{1} << 16, B17 = std::uint64_t{1} << 17, B18 = std::uint64_t{1} << 18, B19 = std::uint64_t{1} << 19,
  B20 = std::uint64_t{1} << 20, B21 = std::uint64_t{1} << 21, B22 = std::uint64_t{1} << 22, B23 = std::uint64_t{1} << 23,
  B24 = std::uint64_t{1} << 24, B25 = std::uint64_t{1} << 25, B26 = std::uint64_t{1} << 26, B27 = std::uint64_t{1} << 27,
  B28 = std::uint64_t{1} << 28, B29 = std::uint64_t{1} << 29, B30 = std::uint64_t{1} << 30, B31 = std::uint64_t{1} << 31,
  B32 = std::uint64_t{1} << 32, B33 = std::uint64_t{1} << 33, B34 = std::uint64_t{1} << 34, B35 = std::uint64_t{1} << 35,
  B36 = std::uint64_t{1} << 36, B37 = std::uint64_t{1} << 37, B38 = std::uint64_t{1} << 38, B39 = std::uint64_t{1} << 39,
  B40 = std::uint64_t{1} << 40, B41 = std::uint64_t{1} << 41, B42 = std::uint64_t{1} << 42, B43 = std::uint64_t{1} << 43,
  B44 = std::uint64_t{1} << 44, B45 = std::uint64_t{1} << 45, B46 = std::uint64_t{1} << 46, B47 = std::uint64_t{1} << 47,
  B48 = std::uint64_t{1} << 48, B49 = std::uint64_t{1} << 49, B50 = std::uint64_t{1} << 50, B51 = std::uint64_t{1} << 51,
  B52 = std::uint64_t{1} << 52, B53 = std::uint64_t{1} << 53, B54 = std::uint64_t{1} << 54, B55 = std::uint64_t{1} << 55,
  B56 = std::uint64_t{1} << 56, B57 = std::uint64_t{1} << 57, B58 = std::uint64_t{1} << 58, B59 = std::uint64_t{1} << 59,
  B60 = std::uint64_t{1} << 60, B61 = std::uint64_t{1} << 61, B62 = std::uint64_t{1} << 62, B63 = std::uint64_t{1} << 63,
};
template <>
struct magic_enum::customize::enum_range<ZeroFlags64> {
  static constexpr bool is_flags = true;
};

// Places the named zero between the bits.
struct ZeroInsideIndex {
  static constexpr ZeroFlags order[] = {ZeroFlags::A, ZeroFlags::None, ZeroFlags::B};

  [[nodiscard]] static constexpr const ZeroFlags* it(std::size_t i) noexcept { return order + i; }

  [[nodiscard]] static constexpr magic_enum::optional<std::size_t> at(ZeroFlags value) noexcept {
    for (std::size_t i = 0; i < 3; ++i) {
      if (order[i] == value) {
        return i;
      }
    }
    return {};
  }
};

// Maps the unnamed zero onto the position of RED.
struct ZeroCollidingIndex {
  [[nodiscard]] static constexpr const Color* it(std::size_t i) noexcept { return magic_enum::enum_values<Color>().data() + i; }

  [[nodiscard]] static constexpr magic_enum::optional<std::size_t> at(Color value) noexcept { return static_cast<std::size_t>(value) / 2; }
};

// Iterates through std::reverse_iterator instead of a pointer.
struct ReverseIteratorIndex {
  [[nodiscard]] static constexpr auto it(std::size_t i) noexcept { return std::make_reverse_iterator(magic_enum::enum_values<Color>().data() + 3 - i); }

  [[nodiscard]] static constexpr magic_enum::optional<std::size_t> at(Color value) noexcept {
    if (const auto i = magic_enum::enum_index(value)) {
      return 2 - *i;
    }
    return {};
  }
};

enum class Empty {};

enum class ReverseOrder { First, Second, Third };

constexpr bool operator<(ReverseOrder lhs, ReverseOrder rhs) noexcept {
  return static_cast<int>(lhs) > static_cast<int>(rhs);
}

// Enums used by containers_bitset_iterator tests.
enum class Numbers { ONE, TWO, THREE, FOUR };           // 4 values, uint8_t base_type, unused_bits=4
enum class Nibble  { B0, B1, B2, B3, B4, B5, B6, B7 }; // 8 values, uint8_t base_type, unused_bits=0 (edge case)
enum class Flags9  { A, B, C, D, E, F, G, H, I };      // 9 values, uint16_t base_type
enum class Bits32 { B00, B01, B02, B03, B04, B05, B06, B07, B08, B09, B10, B11, B12, B13, B14, B15, B16, B17, B18, B19, B20, B21, B22, B23, B24, B25, B26, B27, B28, B29, B30, B31 };
enum class Bits65 {
  B00, B01, B02, B03, B04, B05, B06, B07,
  B08, B09, B10, B11, B12, B13, B14, B15,
  B16, B17, B18, B19, B20, B21, B22, B23,
  B24, B25, B26, B27, B28, B29, B30, B31,
  B32, B33, B34, B35, B36, B37, B38, B39,
  B40, B41, B42, B43, B44, B45, B46, B47,
  B48, B49, B50, B51, B52, B53, B54, B55,
  B56, B57, B58, B59, B60, B61, B62, B63,
  B64,
};

static_assert(magic_enum::enum_count<Bits32>() == 32);
static_assert(magic_enum::enum_count<Bits65>() == 65);

enum class Wide256 : std::uint8_t {
  W000, W001, W002, W003, W004, W005, W006, W007, W008, W009, W010, W011, W012, W013, W014, W015,
  W016, W017, W018, W019, W020, W021, W022, W023, W024, W025, W026, W027, W028, W029, W030, W031,
  W032, W033, W034, W035, W036, W037, W038, W039, W040, W041, W042, W043, W044, W045, W046, W047,
  W048, W049, W050, W051, W052, W053, W054, W055, W056, W057, W058, W059, W060, W061, W062, W063,
  W064, W065, W066, W067, W068, W069, W070, W071, W072, W073, W074, W075, W076, W077, W078, W079,
  W080, W081, W082, W083, W084, W085, W086, W087, W088, W089, W090, W091, W092, W093, W094, W095,
  W096, W097, W098, W099, W100, W101, W102, W103, W104, W105, W106, W107, W108, W109, W110, W111,
  W112, W113, W114, W115, W116, W117, W118, W119, W120, W121, W122, W123, W124, W125, W126, W127,
  W128, W129, W130, W131, W132, W133, W134, W135, W136, W137, W138, W139, W140, W141, W142, W143,
  W144, W145, W146, W147, W148, W149, W150, W151, W152, W153, W154, W155, W156, W157, W158, W159,
  W160, W161, W162, W163, W164, W165, W166, W167, W168, W169, W170, W171, W172, W173, W174, W175,
  W176, W177, W178, W179, W180, W181, W182, W183, W184, W185, W186, W187, W188, W189, W190, W191,
  W192, W193, W194, W195, W196, W197, W198, W199, W200, W201, W202, W203, W204, W205, W206, W207,
  W208, W209, W210, W211, W212, W213, W214, W215, W216, W217, W218, W219, W220, W221, W222, W223,
  W224, W225, W226, W227, W228, W229, W230, W231, W232, W233, W234, W235, W236, W237, W238, W239,
  W240, W241, W242, W243, W244, W245, W246, W247, W248, W249, W250, W251, W252, W253, W254, W255,
};

template <>
struct magic_enum::customize::enum_range<Wide256> {
  static constexpr int min = 0;
  static constexpr int max = 255;
};

static_assert(magic_enum::enum_count<Wide256>() == 256);

struct StaticIndex : magic_enum::containers::default_indexing<Color> {
  StaticIndex() = delete;
};

struct InvalidIndex : magic_enum::containers::default_indexing<Color> {
  [[nodiscard]] static constexpr magic_enum::optional<std::size_t> at(Color value) noexcept {
    return value == Color::RED ? 0 : 99;
  }
};

struct MissingIndex : magic_enum::containers::default_indexing<Color> {
  [[nodiscard]] static constexpr magic_enum::optional<std::size_t> at(Color value) noexcept {
    return value == Color::RED ? magic_enum::optional<std::size_t>{0} : magic_enum::optional<std::size_t>{};
  }
};

struct DuplicateIndex : magic_enum::containers::default_indexing<Color> {
  [[nodiscard]] static constexpr magic_enum::optional<std::size_t> at(Color) noexcept {
    return 0;
  }
};

struct OutOfRangeIndex : magic_enum::containers::default_indexing<Color> {
  [[nodiscard]] static constexpr magic_enum::optional<std::size_t> at(Color value) noexcept {
    if (auto index = magic_enum::containers::default_indexing<Color>::at(value)) {
      return index;
    }
    return 99;
  }
};

static_assert(!magic_enum::containers::detail::valid_indexing<Color, InvalidIndex>());
static_assert(!magic_enum::containers::detail::valid_indexing<Color, MissingIndex>());
static_assert(!magic_enum::containers::detail::valid_indexing<Color, DuplicateIndex>());
static_assert(!magic_enum::containers::detail::valid_indexing<Empty, magic_enum::containers::default_indexing<Empty>>());
static_assert(std::is_default_constructible_v<magic_enum::containers::array<Color, int, StaticIndex>>);
static_assert(std::is_default_constructible_v<magic_enum::containers::bitset<Color, StaticIndex>>);

template <auto I, typename T, typename = void>
inline constexpr bool has_array_get = false;

template <auto I, typename T>
inline constexpr bool has_array_get<I, T, std::void_t<decltype(magic_enum::containers::get<I>(std::declval<T>()))>> = true;

using ColorArray = magic_enum::containers::array<Color, int>;
static_assert(std::is_aggregate_v<ColorArray>);
static_assert([] {
  ColorArray values{{1, 2, 3}};
  values.at(Color::GREEN) = 7;
  const auto& const_values = values;
  return &values.at(Color::GREEN) == &values.a[1] && &const_values.at(Color::GREEN) == &values.a[1] && const_values.at(Color::GREEN) == 7;
}());
static_assert(!has_array_get<-1LL, ColorArray&>);
static_assert(!has_array_get<-1LL, const ColorArray&>);
static_assert(!has_array_get<-1LL, ColorArray&&>);
static_assert(!has_array_get<-1LL, const ColorArray&&>);
static_assert(!has_array_get<3, ColorArray&>);
static_assert(has_array_get<0LL, ColorArray&>);
static_assert(has_array_get<2ULL, const ColorArray&>);
static_assert(has_array_get<Color::RED, ColorArray&>);

struct NoexceptSwapThrowingMove {
  NoexceptSwapThrowingMove() = default;
  NoexceptSwapThrowingMove(const NoexceptSwapThrowingMove&) = delete;
  NoexceptSwapThrowingMove& operator=(const NoexceptSwapThrowingMove&) = delete;
  NoexceptSwapThrowingMove(NoexceptSwapThrowingMove&&) noexcept(false) {}
  NoexceptSwapThrowingMove& operator=(NoexceptSwapThrowingMove&&) noexcept(false) { return *this; }

  friend void swap(NoexceptSwapThrowingMove&, NoexceptSwapThrowingMove&) noexcept {}
};

using ThrowingMoveArray = magic_enum::containers::array<Color, NoexceptSwapThrowingMove>;
static_assert(std::is_nothrow_swappable_v<NoexceptSwapThrowingMove>);
static_assert(!noexcept(std::declval<ThrowingMoveArray&>().swap(std::declval<ThrowingMoveArray&>())));
static_assert([] {
  magic_enum::containers::array<Color, int> lhs{{1, 2, 3}};
  magic_enum::containers::array<Color, int> rhs{{4, 5, 6}};
  lhs.swap(rhs);
  return lhs[Color::RED] == 4 && rhs[Color::RED] == 1;
}());

// Volatile keeps MSVC from proving the compare loops unreachable, which is C4702 at /O1.
volatile bool throw_on_less = true;

struct ThrowingLess {
  friend bool operator<(ThrowingLess, ThrowingLess) {
    if (throw_on_less) {
      throw 0;
    }
    return false;
  }
};

enum class ThrowingEnum { A, B };

bool operator<(ThrowingEnum, ThrowingEnum) {
  if (throw_on_less) {
    throw 0;
  }
  return false;
}

struct UnderlyingLess {
  template <typename E>
  constexpr bool operator()(E lhs, E rhs) const noexcept {
    return magic_enum::enum_underlying(lhs) < magic_enum::enum_underlying(rhs);
  }
};

enum class EquivalentNames { A, a };

struct NonCopyablePredicate {
  NonCopyablePredicate() = default;
  NonCopyablePredicate(const NonCopyablePredicate&) = delete;

  constexpr bool operator()(char lhs, char rhs) & noexcept { return lhs == rhs; }
};

volatile bool throw_conversion = true;

struct ThrowingConversion {
  ThrowingConversion() = default;
  ThrowingConversion(const ThrowingConversion&) = delete;

  operator magic_enum::string_view() const {
    if (throw_conversion) {
      throw 0;
    }
    return {};
  }

  operator Color() const {
    if (throw_conversion) {
      throw 0;
    }
    return Color::RED;
  }
};

// Reads the given characters, then fails like a broken device.
class FailingReadBuf : public std::streambuf {
  std::string chars;

 public:
  explicit FailingReadBuf(std::string s) : chars(std::move(s)) { setg(chars.data(), chars.data(), chars.data() + chars.size()); }

 protected:
  int_type underflow() override { throw std::runtime_error("read error"); }
};

using ColorBitset = magic_enum::containers::bitset<Color>;
using ColorSet = magic_enum::containers::set<Color>;
using ThrowingSet = magic_enum::containers::set<ThrowingEnum, UnderlyingLess>;
static_assert(std::is_same_v<typename std::iterator_traits<ColorBitset::iterator>::value_type, Color>);
static_assert(std::is_same_v<typename std::iterator_traits<ColorSet::iterator>::value_type, Color>);
static_assert(!noexcept(std::declval<ColorSet&>().insert(static_cast<const Color*>(nullptr), static_cast<const Color*>(nullptr))));
static_assert([] {
  constexpr char bits[] = {'1', '0', '1'};
  constexpr magic_enum::containers::bitset<Color> value{magic_enum::containers::raw_access, bits, sizeof(bits)};
  return value.test(Color::RED) && !value.test(Color::GREEN) && value.test(Color::BLUE);
}());
static_assert([] {
  constexpr auto max = (std::numeric_limits<unsigned long long>::max)();
  constexpr magic_enum::containers::bitset<Bits65> value{magic_enum::containers::raw_access, max};
  return value.count() == 64 && value.test(Bits65::B00) && value.test(Bits65::B63) && !value.test(Bits65::B64) && value.to_ullong(magic_enum::containers::raw_access) == max;
}());
static_assert([] {
  magic_enum::containers::bitset<Bits65> values;
  values[Bits65::B63] = true;
  values[Bits65::B64] = values[Bits65::B63];
  values[Bits65::B63].flip();
  const auto& const_values = values;
  return !const_values[Bits65::B63] && const_values[Bits65::B64] && values.count() == 1;
}());
// Bitset iteration must stay constexpr on MSVC in C++17, where _BitScan* intrinsics are not.
static_assert([] {
  constexpr magic_enum::containers::bitset<Bits65> value{magic_enum::containers::raw_access, (1ULL << 5) | (1ULL << 40)};
  auto it = value.begin();
  const bool forward = *it == Bits65::B05 && *++it == Bits65::B40 && ++it == value.end();
  auto rit = value.end();
  return forward && *--rit == Bits65::B40 && *--rit == Bits65::B05;
}());
#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
static_assert(std::bidirectional_iterator<ColorBitset::iterator>);
static_assert(std::bidirectional_iterator<ColorSet::iterator>);
#endif

struct RGB {

  std::uint8_t r {};
  std::uint8_t g {};
  std::uint8_t b {};

  [[nodiscard]] constexpr bool empty() { return std::equal_to{}(r, g) && std::equal_to{}(g, b) && std::equal_to{}(b, 0); }

  [[nodiscard]] constexpr bool operator==(RGB rgb) const noexcept { return std::equal_to{}(r, rgb.r) && std::equal_to{}(g, rgb.g) && std::equal_to{}(b, rgb.b); }

  friend std::ostream& operator<<(std::ostream& ostream, RGB rgb) {

    ostream << "R=" << static_cast<std::uint32_t>(rgb.r) << " G=" << static_cast<std::uint32_t>(rgb.g) << " B=" << static_cast<std::uint32_t>(rgb.b);
    return ostream;
  }
};

template <typename T> bool check_const([[maybe_unused]]T& element) { return false; }
template <typename T> bool check_const([[maybe_unused]]T const& element) { return true; }

constexpr std::uint8_t color_max = std::numeric_limits<std::uint8_t>::max();

struct ConstLvalueStringLess {
  constexpr bool operator()(magic_enum::string_view lhs, magic_enum::string_view rhs) const & { return lhs < rhs; }
  bool operator()(magic_enum::string_view, magic_enum::string_view) const && = delete;
};

struct RvalueStringLess {
  constexpr bool operator()(magic_enum::string_view lhs, magic_enum::string_view rhs) && { return lhs < rhs; }
};

struct LvalueCharLess {
  constexpr bool operator()(const char& lhs, const char& rhs) & { return lhs < rhs; }
  bool operator()(const char&, const char&) && = delete;
  bool operator()(char&&, char&&) & = delete;
};

struct RvalueCharLess {
  constexpr bool operator()(char lhs, char rhs) && { return lhs < rhs; }
};

TEST_CASE_TEMPLATE("bit counts preserve width in constant and runtime evaluation", T, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
  using magic_enum::containers::detail::countr_zero;
  using magic_enum::containers::detail::countl_zero;
  constexpr int digits = std::numeric_limits<T>::digits;
  static_assert(countr_zero(T{0}) == digits);
  static_assert(countl_zero(T{0}) == digits);
  static_assert(countr_zero((std::numeric_limits<T>::max)()) == 0);
  static_assert(countl_zero((std::numeric_limits<T>::max)()) == 0);
  // MSVC 19.29 rejects the implicit use of a constexpr local in this lambda (C3493).
  static_assert([] {
    constexpr int width = std::numeric_limits<T>::digits;
    for (int i = 0; i < width; ++i) {
      const auto bit = static_cast<T>(T{1} << i);
      if (countr_zero(bit) != i || countl_zero(bit) != width - i - 1) {
        return false;
      }
    }
    return true;
  }());

  // Volatile inputs exercise runtime intrinsics as well as the constexpr paths above.
  volatile T value = 0;
  REQUIRE(countr_zero(static_cast<T>(value)) == digits);
  REQUIRE(countl_zero(static_cast<T>(value)) == digits);
  value = (std::numeric_limits<T>::max)();
  REQUIRE(countr_zero(static_cast<T>(value)) == 0);
  REQUIRE(countl_zero(static_cast<T>(value)) == 0);
  for (int i = 0; i < digits; ++i) {
    value = static_cast<T>(T{1} << i);
    REQUIRE(countr_zero(static_cast<T>(value)) == i);
    REQUIRE(countl_zero(static_cast<T>(value)) == digits - i - 1);
  }
}

TEST_CASE("name comparator categories") {
  using StringLess = magic_enum::containers::detail::name_sort_impl<void, ConstLvalueStringLess>;
  using CharLess = magic_enum::containers::detail::name_sort_impl<void, LvalueCharLess>;
  using InvalidStringLess = magic_enum::containers::detail::name_sort_impl<void, RvalueStringLess>;
  using InvalidCharLess = magic_enum::containers::detail::name_sort_impl<void, RvalueCharLess>;

  static_assert(std::is_invocable_r_v<bool, StringLess, Color, Color>);
  static_assert(std::is_invocable_r_v<bool, CharLess, Color, Color>);
  static_assert(!std::is_invocable_v<InvalidStringLess, Color, Color>);
  static_assert(!std::is_invocable_v<InvalidCharLess, Color, Color>);
  static_assert(StringLess{}(Color::BLUE, Color::GREEN));
  static_assert(CharLess{}(Color::BLUE, Color::GREEN));
  REQUIRE(StringLess{}(Color::GREEN, "RED"));
  REQUIRE(CharLess{}("BLUE", Color::RED));
  REQUIRE_FALSE(StringLess{}(Color::RED, Color::GREEN));
  REQUIRE_FALSE(CharLess{}(Color::RED, Color::GREEN));
}

TEST_CASE("containers_array") {

  using namespace magic_enum::bitwise_operators;
  using namespace magic_enum::ostream_operators;

  constexpr magic_enum::containers::array<Color, RGB> color_rgb_initializer {{{{color_max, 0, 0}, {0, color_max, 0}, {0, 0, color_max}}}};
  REQUIRE(color_rgb_initializer.at(Color::RED) == RGB{color_max, 0, 0});
  REQUIRE(color_rgb_initializer.at(Color::GREEN) == RGB{0, color_max, 0});
  REQUIRE(color_rgb_initializer.at(Color::BLUE) == RGB{0, 0, color_max});

  /* BUG: a sort will not survive the data integration */
  magic_enum::containers::array<Color, std::uint8_t> color_rgb_container_int{{1U, 4U, 2U}};

  // ENC: Direct convert to std::array
  // std::array compare_before {1U, 4U, 2U};
  constexpr magic_enum::containers::array<Color, std::uint8_t> compare_before{{{1U, 4U, 2U}}};
  REQUIRE(color_rgb_container_int == compare_before);

  constexpr auto colors = magic_enum::enum_values<Color>();

  std::ignore = magic_enum::containers::get<0>(compare_before);
  std::ignore = magic_enum::containers::get<1>(compare_before);
  std::ignore = magic_enum::containers::get<2>(compare_before);

  std::ignore = magic_enum::containers::get<Color::RED>(compare_before);
  std::ignore = magic_enum::containers::get<Color::GREEN>(compare_before);
  std::ignore = magic_enum::containers::get<Color::BLUE>(compare_before);

  REQUIRE(std::make_pair(colors[0], color_rgb_container_int[colors[0]]) == std::make_pair<Color, std::uint8_t>(Color::RED, 1U));
  REQUIRE(std::make_pair(colors[1], color_rgb_container_int[colors[1]]) == std::make_pair<Color, std::uint8_t>(Color::GREEN, 4U));
  REQUIRE(std::make_pair(colors[2], color_rgb_container_int[colors[2]]) == std::make_pair<Color, std::uint8_t>(Color::BLUE, 2U));

  std::sort(std::begin(color_rgb_container_int), std::end(color_rgb_container_int));

  // Missing: Direct convert to std::array
  // std::array compare_after {1U, 2U, 4U};
  constexpr magic_enum::containers::array<Color, std::uint8_t> compare_after{{{1U, 2U, 4U}}};
  REQUIRE(color_rgb_container_int == compare_after);

  std::ignore = magic_enum::containers::get<0>(compare_after);
  std::ignore = magic_enum::containers::get<1>(compare_after);
  std::ignore = magic_enum::containers::get<2>(compare_after);

  std::ignore = magic_enum::containers::get<Color::RED>(compare_after);
  std::ignore = magic_enum::containers::get<Color::GREEN>(compare_after);
  std::ignore = magic_enum::containers::get<Color::BLUE>(compare_after);

  REQUIRE(std::make_pair(colors[0], color_rgb_container_int[colors[0]]) == std::make_pair<Color, std::uint8_t>(Color::RED, 1U));
  REQUIRE(std::make_pair(colors[1], color_rgb_container_int[colors[1]]) == std::make_pair<Color, std::uint8_t>(Color::GREEN, 2U));
  REQUIRE(std::make_pair(colors[2], color_rgb_container_int[colors[2]]) == std::make_pair<Color, std::uint8_t>(Color::BLUE, 4U));

  auto color_rgb_container = magic_enum::containers::array<Color, RGB>();
  REQUIRE_FALSE(color_rgb_container.empty());
  REQUIRE(color_rgb_container.size() == 3);
  REQUIRE(magic_enum::enum_count<Color>() == color_rgb_container.size());

  REQUIRE(color_rgb_container.at(Color::RED).empty());
  REQUIRE(color_rgb_container.at(Color::GREEN).empty());
  REQUIRE(color_rgb_container.at(Color::BLUE).empty());
  REQUIRE_THROWS(color_rgb_container.at(Color::BLUE|Color::GREEN));

  color_rgb_container[Color::RED] = {color_max, 0, 0};
  color_rgb_container[Color::GREEN] = {0, color_max, 0};
  color_rgb_container[Color::BLUE] = {0, 0, color_max};

  REQUIRE(color_rgb_container.at(Color::RED) == RGB{color_max, 0, 0});
  REQUIRE(color_rgb_container.at(Color::GREEN) == RGB{0, color_max, 0});
  REQUIRE(color_rgb_container.at(Color::BLUE) == RGB{0, 0, color_max});

  REQUIRE(color_rgb_container.front() == RGB{color_max, 0, 0});
  REQUIRE(color_rgb_container.back() == RGB{0, 0, color_max});

  REQUIRE(magic_enum::containers::get<Color::RED>(color_rgb_container) == RGB{color_max, 0, 0});
  REQUIRE(magic_enum::containers::get<Color::GREEN>(color_rgb_container) == RGB{0, color_max, 0});
  REQUIRE(magic_enum::containers::get<Color::BLUE>(color_rgb_container) == RGB{0, 0, color_max});

  auto iterator = color_rgb_container.begin();
  REQUIRE_FALSE(check_const(iterator));
  REQUIRE(check_const(color_rgb_container.begin()));
  REQUIRE(check_const(color_rgb_container.cbegin()));

  auto color_rgb_container_compare = magic_enum::containers::array<Color, RGB>();
  color_rgb_container_compare.fill({color_max, color_max, color_max});
  REQUIRE_FALSE(color_rgb_container == color_rgb_container_compare);

  color_rgb_container_compare[Color::RED] = {color_max, 0, 0};
  color_rgb_container_compare[Color::GREEN] = {0, color_max, 0};
  color_rgb_container_compare[Color::BLUE] = {0, 0, color_max};
  REQUIRE(color_rgb_container == color_rgb_container_compare);

  constexpr auto from_to_array = magic_enum::containers::to_array<Color, RGB>({{color_max, 0, 0}, {0, color_max, 0}, {0, 0, color_max}});
  REQUIRE(from_to_array.at(Color::RED) == RGB{color_max, 0, 0});
  REQUIRE(from_to_array.at(Color::GREEN) == RGB{0, color_max, 0});
  REQUIRE(from_to_array.at(Color::BLUE) == RGB{0, 0, color_max});

  constexpr auto from_make_array = magic_enum::containers::make_array<Color>(RGB{color_max, 0, 0}, RGB{0, color_max, 0}, RGB{0, 0, color_max});
  REQUIRE(from_make_array.at(Color::RED) == RGB{color_max, 0, 0});
  REQUIRE(from_make_array.at(Color::GREEN) == RGB{0, color_max, 0});
  REQUIRE(from_make_array.at(Color::BLUE) == RGB{0, 0, color_max});

  using NameIndex = magic_enum::containers::comparator_indexing<magic_enum::containers::name_less<>>;
  constexpr magic_enum::containers::array<Color, int, NameIndex> by_name{{10, 20, 30}};
  REQUIRE(by_name[Color::BLUE] == 10);
  REQUIRE(by_name[Color::GREEN] == 20);
  REQUIRE(by_name[Color::RED] == 30);

  const magic_enum::containers::array<Color, ThrowingLess> throwing_lhs{};
  const magic_enum::containers::array<Color, ThrowingLess> throwing_rhs{};
  REQUIRE_THROWS(static_cast<void>(throwing_lhs < throwing_rhs));

  ColorArray checked_array{{1, 2, 3}};
  REQUIRE_THROWS_WITH_AS(checked_array.at(static_cast<Color>(8)), "magic_enum::containers::array::at: Unrecognized position", std::out_of_range);
  REQUIRE_THROWS_WITH_AS(std::as_const(checked_array).at(static_cast<Color>(8)), "magic_enum::containers::array::at: Unrecognized position", std::out_of_range);
  magic_enum::containers::array<Color, int, OutOfRangeIndex> custom_index_array{};
  REQUIRE_THROWS_WITH_AS(custom_index_array.at(static_cast<Color>(8)), "magic_enum::containers::array::at: Unrecognized position", std::out_of_range);
  REQUIRE_THROWS_WITH_AS(std::as_const(custom_index_array).at(static_cast<Color>(8)), "magic_enum::containers::array::at: Unrecognized position", std::out_of_range);
}

TEST_CASE("containers_bitset") {

  using namespace magic_enum::bitwise_operators;

  auto color_bitset = magic_enum::containers::bitset<Color>();
  REQUIRE(color_bitset.to_string().empty());
  REQUIRE(color_bitset.size() == 3);
  REQUIRE(magic_enum::enum_count<Color>() == color_bitset.size());
  REQUIRE_FALSE(color_bitset.all());
  REQUIRE_FALSE(color_bitset.any());
  REQUIRE(color_bitset.none());
  REQUIRE(color_bitset.count() == 0);

  color_bitset.set(Color::GREEN);
  REQUIRE_FALSE(color_bitset.all());
  REQUIRE(color_bitset.any());
  REQUIRE_FALSE(color_bitset.none());
  REQUIRE(color_bitset.count() == 1);
  REQUIRE_FALSE(color_bitset.test(Color::RED));
  REQUIRE(color_bitset.test(Color::GREEN));
  REQUIRE_FALSE(color_bitset.test(Color::BLUE));

  color_bitset.set(Color::BLUE);
  REQUIRE_FALSE(color_bitset.all());
  REQUIRE(color_bitset.any());
  REQUIRE_FALSE(color_bitset.none());
  REQUIRE(color_bitset.count() == 2);
  REQUIRE_FALSE(color_bitset.test(Color::RED));
  REQUIRE(color_bitset.test(Color::GREEN));
  REQUIRE(color_bitset.test(Color::BLUE));

  color_bitset.set(Color::RED);
  REQUIRE(color_bitset.all());
  REQUIRE(color_bitset.any());
  REQUIRE_FALSE(color_bitset.none());
  REQUIRE(color_bitset.count() == 3);
  REQUIRE(color_bitset.test(Color::RED));
  REQUIRE(color_bitset.test(Color::GREEN));
  REQUIRE(color_bitset.test(Color::BLUE));

  color_bitset.reset();
  REQUIRE_FALSE(color_bitset.all());
  REQUIRE_FALSE(color_bitset.any());
  REQUIRE(color_bitset.none());
  REQUIRE(color_bitset.count() == 0);
  REQUIRE_FALSE(color_bitset.test(Color::RED));
  REQUIRE_FALSE(color_bitset.test(Color::GREEN));
  REQUIRE_FALSE(color_bitset.test(Color::BLUE));

  color_bitset.set(Color::RED);
  REQUIRE(color_bitset.test(Color::RED));
  REQUIRE_FALSE(color_bitset.test(Color::GREEN));
  REQUIRE_FALSE(color_bitset.test(Color::BLUE));

  color_bitset.flip();
  REQUIRE_FALSE(color_bitset.test(Color::RED));
  REQUIRE(color_bitset.test(Color::GREEN));
  REQUIRE(color_bitset.test(Color::BLUE));

  constexpr magic_enum::containers::bitset<Color> color_bitset_all {Color::RED|Color::GREEN|Color::BLUE};
  REQUIRE(static_cast<Color>(color_bitset_all) == (Color::RED | Color::GREEN | Color::BLUE));
  REQUIRE(color_bitset_all.to_string() == "RED|GREEN|BLUE");
  REQUIRE(color_bitset_all.to_string( {}, '0', '1' ) == "111");
  REQUIRE(color_bitset_all.to_ulong( {} ) == 7);
  REQUIRE(color_bitset_all.to_ullong( {} ) == 7);
  REQUIRE(color_bitset_all.all());
  REQUIRE(color_bitset_all.any());
  REQUIRE_FALSE(color_bitset_all.none());

  constexpr magic_enum::containers::bitset<Color> color_bitset_red_green {Color::RED|Color::GREEN};
  REQUIRE(color_bitset_red_green.to_string() == "RED|GREEN");
  REQUIRE(color_bitset_red_green.to_string( {}, '0', '1' ) == "110");
  REQUIRE(color_bitset_red_green.to_ulong( {} ) == 3);
  REQUIRE(color_bitset_red_green.to_ullong( {} ) == 3);
  REQUIRE_FALSE(color_bitset_red_green.all());
  REQUIRE(color_bitset_red_green.any());
  REQUIRE_FALSE(color_bitset_red_green.none());

  constexpr magic_enum::containers::bitset<Color> color_bitset_raw_string {magic_enum::containers::raw_access, "101"};
  REQUIRE(color_bitset_raw_string.to_string( {}, '0', '1' ) == "101");
  REQUIRE(color_bitset_raw_string.to_ulong( {} ) == 5);
  REQUIRE(color_bitset_raw_string.test(Color::RED));
  REQUIRE_FALSE(color_bitset_raw_string.test(Color::GREEN));
  REQUIRE(color_bitset_raw_string.test(Color::BLUE));

  constexpr magic_enum::containers::bitset<Color> color_bitset_raw_number {magic_enum::containers::raw_access, 5};
  REQUIRE(color_bitset_raw_number == color_bitset_raw_string);
  REQUIRE_THROWS((magic_enum::containers::bitset<Color>{magic_enum::containers::raw_access, 8}));

  magic_enum::containers::bitset<Bits65> too_wide;
  too_wide.set(Bits65::B64);
  REQUIRE_THROWS(static_cast<void>(too_wide.to_ullong(magic_enum::containers::raw_access)));
  if constexpr (std::numeric_limits<unsigned long>::digits < magic_enum::enum_count<Bits65>()) {
    too_wide.reset().set(static_cast<Bits65>(std::numeric_limits<unsigned long>::digits));
    REQUIRE_THROWS(static_cast<void>(too_wide.to_ulong(magic_enum::containers::raw_access)));
  }

  NonCopyablePredicate predicate;
  const magic_enum::containers::bitset<Color> color_bitset_names {"RED|BLUE", '|', predicate};
  REQUIRE(color_bitset_names.test(Color::RED));
  REQUIRE_FALSE(color_bitset_names.test(Color::GREEN));
  REQUIRE(color_bitset_names.test(Color::BLUE));

  constexpr magic_enum::containers::bitset<Color> constexpr_repeated_names {"RED|GREEN|GREEN"};
  static_assert(constexpr_repeated_names.test(Color::RED));
  static_assert(constexpr_repeated_names.test(Color::GREEN));
  static_assert(!constexpr_repeated_names.test(Color::BLUE));
  const magic_enum::containers::bitset<Color> runtime_repeated_names {"RED|GREEN|GREEN"};
  REQUIRE(runtime_repeated_names == constexpr_repeated_names);

  constexpr magic_enum::containers::bitset<Color> constexpr_empty_names {""};
  static_assert(constexpr_empty_names.none());
  constexpr magic_enum::containers::bitset<Color> constexpr_trailing_separator {"RED|"};
  static_assert(constexpr_trailing_separator == magic_enum::containers::bitset<Color>{Color::RED});
  constexpr magic_enum::containers::bitset<Color> constexpr_custom_separator {"GREEN,BLUE", ','};
  static_assert(constexpr_custom_separator == magic_enum::containers::bitset<Color>{Color::GREEN | Color::BLUE});
  REQUIRE(magic_enum::containers::bitset<Color>{""}.none());
  REQUIRE(magic_enum::containers::bitset<Color>{"RED|"} == constexpr_trailing_separator);
  REQUIRE(magic_enum::containers::bitset<Color>{"GREEN,BLUE", ','} == constexpr_custom_separator);
  REQUIRE_THROWS((magic_enum::containers::bitset<Color>{"|RED"}));
  REQUIRE_THROWS((magic_enum::containers::bitset<Color>{"RED||BLUE"}));
  REQUIRE_THROWS((magic_enum::containers::bitset<Color>{"RED|PURPLE"}));
  REQUIRE_THROWS((magic_enum::containers::bitset<Color>{"RED,BLUE"}));
  REQUIRE_THROWS_AS((magic_enum::containers::bitset<Color>{magic_enum::containers::raw_access, magic_enum::string_view{"1"}, 2}), std::out_of_range);

  constexpr magic_enum::containers::bitset<Color> copy_list_init = {};
  static_assert(copy_list_init.none());

  magic_enum::containers::bitset<Color> streamed{Color::RED};
  std::istringstream valid_stream{"GREEN,BLUE GREEN|BLUE"};
  std::istringstream invalid_stream{"GREEN|PURPLE"};
  REQUIRE_FALSE(static_cast<bool>(valid_stream >> streamed));
  REQUIRE(streamed == magic_enum::containers::bitset<Color>{Color::RED});
  valid_stream.clear();
  REQUIRE(static_cast<bool>(valid_stream >> streamed));
  REQUIRE(streamed == constexpr_custom_separator);
  REQUIRE_FALSE(static_cast<bool>(invalid_stream >> streamed));
  REQUIRE(streamed == constexpr_custom_separator);

  std::istringstream throwing_stream{"RED|PURPLE"};
  throwing_stream.exceptions(std::ios_base::failbit);
  REQUIRE_THROWS_AS(throwing_stream >> streamed, std::ios_base::failure);
  REQUIRE(throwing_stream.fail());
  REQUIRE(streamed == constexpr_custom_separator);

  // A read error after a valid prefix keeps the value.
  FailingReadBuf bad_buf{"RED"};
  std::istream bad_stream{&bad_buf};
  REQUIRE_FALSE(static_cast<bool>(bad_stream >> streamed));
  REQUIRE(bad_stream.bad());
  REQUIRE(streamed == constexpr_custom_separator);
  FailingReadBuf throwing_bad_buf{"RED"};
  std::istream throwing_bad_stream{&throwing_bad_buf};
  throwing_bad_stream.exceptions(std::ios_base::badbit);
  REQUIRE_THROWS_AS(throwing_bad_stream >> streamed, std::runtime_error);
  REQUIRE(streamed == constexpr_custom_separator);

  magic_enum::containers::bitset<Color, OutOfRangeIndex> custom_index_bitset;
  REQUIRE(custom_index_bitset.find(static_cast<Color>(8)) == custom_index_bitset.end());
  REQUIRE_THROWS(static_cast<void>(custom_index_bitset.test(static_cast<Color>(8))));
  REQUIRE_THROWS(custom_index_bitset.set(static_cast<Color>(8)));
  REQUIRE_THROWS(custom_index_bitset.reset(static_cast<Color>(8)));
  REQUIRE_THROWS(custom_index_bitset.flip(static_cast<Color>(8)));

  static_assert(magic_enum::containers::bitset<Color>{}.flip(Color::GREEN) == magic_enum::containers::bitset<Color>{Color::GREEN});
  magic_enum::containers::bitset<Color> flipped{Color::RED};
  flipped.flip(Color::RED).flip(Color::BLUE);
  REQUIRE(flipped == magic_enum::containers::bitset<Color>{Color::BLUE});
  REQUIRE_THROWS_AS(flipped.flip(static_cast<Color>(8)), std::out_of_range);
  REQUIRE(flipped == magic_enum::containers::bitset<Color>{Color::BLUE});
}

TEST_CASE("containers_bitset_reference") {
  const auto check = [] {
    magic_enum::containers::bitset<Color> bits{Color::RED};
    auto red = bits[Color::RED];
    auto green = bits[Color::GREEN];
    // Copying a reference must not warn under -Wdeprecated-copy.
    auto copy = red;
    if (!copy) {
      return false;
    }
    const auto& same = red;
    red = same;
    if (!bits.test(Color::RED)) {
      return false;
    }
    green = red;
    red = false;
    red = same;
    if (bits.test(Color::RED)) {
      return false;
    }
    red = green;
    if (green.flip()) {
      return false;
    }
    return bits.test(Color::RED) && green.flip() && !bits.test(Color::BLUE);
  };
  static_assert(check());
  REQUIRE(check());
}

TEST_CASE("containers_bitset_ostream_field_width") {
  using Bitset = magic_enum::containers::bitset<Color>;
  const Bitset bits{Color::RED, Color::BLUE};

  for (const bool left : {false, true}) {
    std::ostringstream stream;
    stream.setf(left ? std::ios::left : std::ios::right, std::ios::adjustfield);
    stream.fill('_');
    stream.width(12);
    REQUIRE(&(stream << bits) == &stream);
    REQUIRE(stream.width() == 0);
    stream << '!';
    REQUIRE(stream.str() == (left ? "RED|BLUE____!" : "____RED|BLUE!"));
  }

  std::ostringstream short_width;
  short_width.width(3);
  short_width << bits << '!';
  REQUIRE(short_width.str() == "RED|BLUE!");

  std::ostringstream empty;
  empty.fill('_');
  empty.width(3);
  empty << Bitset{} << '!';
  REQUIRE(empty.str() == "___!");

  struct CustomTraits : std::char_traits<char> {};
  std::basic_ostringstream<char, CustomTraits> custom;
  custom.fill('_');
  custom.width(6);
  custom << Bitset{Color::RED} << '!';
  const auto result = custom.str();
  REQUIRE(std::string(result.data(), result.size()) == "___RED!");
}

TEST_CASE("containers_bitset_hash") {
  using namespace magic_enum::bitwise_operators;
  using magic_enum::containers::bitset;
  using magic_enum::containers::raw_access;

  std::hash<bitset<Color>> hasher;

  // empty bitset has a stable hash
  bitset<Color> empty;
  REQUIRE(hasher(empty) == hasher(empty));

  // equal bitsets produce the same hash
  bitset<Color> a;
  a.set(Color::RED);
  a.set(Color::GREEN);
  bitset<Color> b;
  b.set(Color::RED);
  b.set(Color::GREEN);
  REQUIRE(hasher(a) == hasher(b));

  // hash matches std::hash<unsigned long long> applied to the same bit pattern
  REQUIRE(hasher(a) == std::hash<unsigned long long>{}(a.to_ullong(raw_access)));

  bitset<Bits32> dense_32;
  dense_32.set();
  REQUIRE(std::hash<bitset<Bits32>>{}(dense_32) == std::hash<unsigned long long>{}(dense_32.to_ullong(raw_access)));

  // usable as key in unordered_set
  std::unordered_set<bitset<Color>> s;
  s.insert(bitset<Color>{Color::RED});
  s.insert(bitset<Color>{Color::GREEN});
  s.insert(bitset<Color>{Color::RED});   // duplicate
  REQUIRE(s.size() == 2);
  REQUIRE(s.count(bitset<Color>{Color::RED}) == 1);
  REQUIRE(s.count(bitset<Color>{Color::BLUE}) == 0);

  // Hashing must include storage beyond the first 64 bits and must not throw.
  std::hash<bitset<Bits65>> wide_hasher;
  bitset<Bits65> low_bit;
  low_bit.set(Bits65::B00);
  REQUIRE(wide_hasher(low_bit) == std::hash<unsigned long long>{}(1));

  bitset<Bits65> high_bit;
  high_bit.set(Bits65::B64);
  const auto high_hash = wide_hasher(high_bit);

  bitset<Bits65> high_bit_copy;
  high_bit_copy.set(Bits65::B64);
  REQUIRE(high_hash == wide_hasher(high_bit_copy));

  std::unordered_set<bitset<Bits65>> wide_set;
  wide_set.insert(high_bit);
  REQUIRE(wide_set.count(high_bit_copy) == 1);
}

TEST_CASE("containers_bitset_all_full_storage") {
  constexpr auto full_nibble = [] {
    magic_enum::containers::bitset<Nibble> bs;
    bs.set();
    return bs;
  }();
  static_assert(full_nibble.all());
  REQUIRE(full_nibble.all());

  constexpr auto full_32 = [] {
    magic_enum::containers::bitset<Bits32> bs;
    bs.set();
    return bs;
  }();
  static_assert(full_32.all());
  REQUIRE(full_32.all());

  auto partial_32 = full_32;
  partial_32.reset(Bits32::B31);
  REQUIRE_FALSE(partial_32.all());

  static_assert([] {
    magic_enum::containers::bitset<Bits65> bs;
    const auto flipped = ~bs;
    return flipped.all() && bs.none() && bs.set().all() && bs.flip().none();
  }());
}

TEST_CASE("containers_set overloaded ordering") {
  using namespace magic_enum::containers;
  constexpr set<ReverseOrder> all{ReverseOrder::First, ReverseOrder::Second, ReverseOrder::Third};
  static_assert(*all.begin() == ReverseOrder::Third);
  static_assert(*all.lower_bound(ReverseOrder::Second) == ReverseOrder::Second);
  static_assert(*all.upper_bound(ReverseOrder::Second) == ReverseOrder::First);

  const auto check = [](auto actual, unsigned mask) {
    std::set<ReverseOrder> expected;
    for (unsigned i = 0; i < 3; ++i) {
      if (mask & (1U << i)) {
        actual.insert(static_cast<ReverseOrder>(i));
        expected.insert(static_cast<ReverseOrder>(i));
      }
    }
    REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    REQUIRE(std::equal(actual.rbegin(), actual.rend(), expected.rbegin(), expected.rend()));
    for (int i = -1; i <= 3; ++i) {
      const auto key = static_cast<ReverseOrder>(i);
      const auto [first, last] = actual.equal_range(key);
      const auto [expected_first, expected_last] = expected.equal_range(key);
      REQUIRE(std::distance(actual.begin(), first) == std::distance(expected.begin(), expected_first));
      REQUIRE(std::distance(actual.begin(), last) == std::distance(expected.begin(), expected_last));
      REQUIRE((actual.find(key) != actual.end()) == (expected.find(key) != expected.end()));
    }
    for (const auto key : magic_enum::enum_values<ReverseOrder>()) {
      REQUIRE(actual.erase(key) == expected.erase(key));
      REQUIRE(actual.size() == expected.size());
    }
  };
  for (unsigned mask = 0; mask < 8; ++mask) {
    check(set<ReverseOrder>{}, mask);
    check(set<ReverseOrder, std::less<>>{}, mask);
  }
}

TEST_CASE("container indexing preserves the requested order") {
  using namespace magic_enum::containers;
  constexpr array<ReverseOrder, int> natural{{10, 20, 30}};
  static_assert(natural[ReverseOrder::First] == 10);
  static_assert(natural[ReverseOrder::Third] == 30);
  constexpr bitset<ReverseOrder> bits{ReverseOrder::First, ReverseOrder::Third};
  REQUIRE(*bits.begin() == ReverseOrder::First);
  static_assert(bits.to_ullong(raw_access) == 5);

  using SortedIndex = comparator_indexing<std::less<>>;
  constexpr array<ReverseOrder, int, SortedIndex> sorted{{10, 20, 30}};
  static_assert(sorted[ReverseOrder::First] == 30);
  static_assert(sorted[ReverseOrder::Third] == 10);
  REQUIRE(default_indexing<>::at(ReverseOrder::First) == 0);
  REQUIRE(SortedIndex::at(ReverseOrder::First) == 2);

  constexpr array<ThrowingEnum, int> throwing{{10, 20}};
  static_assert(throwing[ThrowingEnum::A] == 10);
  constexpr bitset<ThrowingEnum> throwing_bits{ThrowingEnum::A};
  REQUIRE(*throwing_bits.begin() == ThrowingEnum::A);
}

struct HalfGreater {
  template <typename E>
  constexpr bool operator()(E lhs, E rhs) const noexcept {
    return magic_enum::enum_underlying(lhs) / 2 > magic_enum::enum_underlying(rhs) / 2;
  }
};

enum class StableKey { A, B, C };

struct GroupLess {
  constexpr bool operator()(StableKey lhs, StableKey rhs) const noexcept {
    return (lhs == StableKey::C ? 0 : 1) < (rhs == StableKey::C ? 0 : 1);
  }
};

TEST_CASE("equivalent comparator keys keep enum_values order") {
  using namespace magic_enum::containers;
  using Index = comparator_indexing<GroupLess>;
  // C moves first, A and B keep their relative order.
  static_assert(Index::at(StableKey::C) == 0);
  static_assert(Index::at(StableKey::A) == 1);
  static_assert(Index::at(StableKey::B) == 2);
  constexpr array<StableKey, int, Index> values{{10, 20, 30}};
  static_assert(values[StableKey::C] == 10);
  static_assert(values[StableKey::A] == 20);
  static_assert(values[StableKey::B] == 30);
  constexpr bitset<StableKey, Index> bits{StableKey::A};
  static_assert(bits.to_ullong(raw_access) == 2);
  constexpr bitset<StableKey, Index> restored{raw_access, 2};
  static_assert(restored.test(StableKey::A));
  static_assert(!restored.test(StableKey::B));
  constexpr set<StableKey, GroupLess> keys{StableKey::B, StableKey::A, StableKey::C};
  static_assert(*keys.begin() == StableKey::C);
  static_assert(*std::next(keys.begin()) == StableKey::A);
  static_assert(*std::next(keys.begin(), 2) == StableKey::B);
  CHECK(*keys.rbegin() == StableKey::B);
  CHECK(*std::next(keys.rbegin()) == StableKey::A);
  CHECK(*keys.find(StableKey::B) == StableKey::B);
  const auto range = keys.equal_range(StableKey::A);
  CHECK(std::distance(range.first, range.second) == 2);
  CHECK(*range.first == StableKey::A);
}

TEST_CASE("comparator indexing sorts wide enums and keeps the order of equivalent keys") {
  using namespace magic_enum::containers;
  // The index table is sorted in constant evaluation, so a quadratic sort exceeded Clang's step limit here.
  constexpr set<Wide256, std::greater<>> reversed{Wide256::W000, Wide256::W128, Wide256::W255};
  static_assert(*reversed.begin() == Wide256::W255);
  static_assert(*std::next(reversed.begin()) == Wide256::W128);
  static_assert(comparator_indexing<std::greater<>>::at(Wide256::W000) == 255);
  static_assert(comparator_indexing<std::greater<>>::at(Wide256::W200) == 55);

  constexpr set<Wide256, HalfGreater> grouped{Wide256::W000, Wide256::W001, Wide256::W254, Wide256::W255};
  static_assert(*grouped.begin() == Wide256::W254);
  static_assert(*std::next(grouped.begin()) == Wide256::W255);
  static_assert(*std::next(grouped.begin(), 2) == Wide256::W000);
  static_assert(*std::next(grouped.begin(), 3) == Wide256::W001);

  using HalfIndex = comparator_indexing<HalfGreater>;
  static_assert(HalfIndex::at(Nibble::B0) == 6);
  static_assert(HalfIndex::at(Nibble::B1) == 7);
  static_assert(HalfIndex::at(Nibble::B2) == 4);
  static_assert(HalfIndex::at(Nibble::B3) == 5);
  static_assert(HalfIndex::at(Nibble::B6) == 0);
  static_assert(HalfIndex::at(Nibble::B7) == 1);
}

TEST_CASE("containers bool flags") {
  constexpr auto enabled = magic_enum::containers::set<BoolFlags>::from_mask(BoolFlags::Enabled);
  static_assert(enabled.size() == 1);
  REQUIRE(enabled.contains(BoolFlags::Enabled));

  constexpr auto empty = magic_enum::containers::set<BoolFlags>::from_mask(BoolFlags::None);
  static_assert(empty.empty());
  REQUIRE_FALSE(empty.contains(BoolFlags::Enabled));

  const magic_enum::containers::bitset<BoolFlags> enabled_bits(BoolFlags::Enabled);
  REQUIRE(enabled_bits.count() == 1);
  REQUIRE(enabled_bits.test(BoolFlags::Enabled));
  REQUIRE(magic_enum::containers::bitset<BoolFlags>(BoolFlags::None).none());
}

TEST_CASE("containers keep a named zero as a key but not as a bit") {
  using namespace magic_enum::bitwise_operators;
  using magic_enum::containers::array;
  using magic_enum::containers::bitset;
  using magic_enum::containers::set;

  constexpr array<ZeroFlags, int> values{{{0, 1, 2}}};
  static_assert(values.size() == 3);
  static_assert(values[ZeroFlags::None] == 0);
  static_assert(values[ZeroFlags::B] == 2);
  static_assert(array<OnlyZeroFlags, int>{}.size() == 1);

  constexpr set<ZeroFlags> key{ZeroFlags::None};
  static_assert(key.size() == 1);
  static_assert(key.contains(ZeroFlags::None));
  static_assert(key.max_size() == 3);
  static_assert(set<ZeroFlags>::from_mask(ZeroFlags::None).empty());
  constexpr auto bits = set<ZeroFlags>::from_mask(ZeroFlags::A | ZeroFlags::B);
  static_assert(bits.size() == 2);
  static_assert(!bits.contains(ZeroFlags::None));
  static_assert(set<OnlyZeroFlags>{}.empty());
  static_assert(set<OnlyZeroFlags>{OnlyZeroFlags::None}.size() == 1);
  static_assert(set<OnlyZeroFlags>::from_mask(OnlyZeroFlags::None).empty());

  set<ZeroFlags> keys;
  REQUIRE(keys.insert(ZeroFlags::None).second);
  REQUIRE(keys.insert(ZeroFlags::B).second);
  REQUIRE_THROWS_AS(keys.insert(ZeroFlags::A | ZeroFlags::B), std::invalid_argument);
  REQUIRE(keys.size() == 2);
  REQUIRE(*keys.begin() == ZeroFlags::None);
  REQUIRE(keys.erase(ZeroFlags::None) == 1);
  REQUIRE(keys.size() == 1);

  set<ZeroFlags, std::greater<>> reversed{ZeroFlags::None, ZeroFlags::B};
  REQUIRE(reversed.size() == 2);
  REQUIRE(*reversed.begin() == ZeroFlags::B);
  REQUIRE(*std::next(reversed.begin()) == ZeroFlags::None);

  constexpr bitset<ZeroFlags> zero_bits{ZeroFlags::None};
  static_assert(zero_bits.none());
  static_assert(zero_bits.size() == 2);
  static_assert(zero_bits.max_size() == 2);
  static_assert(bitset<ZeroFlags>(ZeroFlags::None).none());
  static_assert(bitset<ZeroFlags>(ZeroFlags::A | ZeroFlags::B).all());

  static_assert(bitset<ZeroFlags>{ZeroFlags::None, ZeroFlags::A}.count() == 1);

  bitset<ZeroFlags> flags;
  REQUIRE_THROWS_AS((void)flags.test(ZeroFlags::None), std::out_of_range);
  REQUIRE_THROWS_AS(flags.set(ZeroFlags::None), std::out_of_range);
  REQUIRE_THROWS_AS(flags.set(ZeroFlags::None, false), std::out_of_range);
  REQUIRE_THROWS_AS(flags.reset(ZeroFlags::None), std::out_of_range);
  REQUIRE_THROWS_AS(flags.flip(ZeroFlags::None), std::out_of_range);
  REQUIRE_THROWS_AS(flags.set(ZeroFlags::A | ZeroFlags::B), std::out_of_range);
  REQUIRE(flags.none());
  flags.set(ZeroFlags::B);
  REQUIRE(flags.count() == 1);
  REQUIRE(flags.find(ZeroFlags::None) == flags.end());
  REQUIRE(*flags.begin() == ZeroFlags::B);
  REQUIRE(static_cast<ZeroFlags>(flags) == ZeroFlags::B);
  REQUIRE(flags.to_string() == "B");
  REQUIRE(flags.to_string(magic_enum::containers::raw_access) == "01");
  flags.flip();
  REQUIRE(flags.to_string() == "A");
  REQUIRE(bitset<ZeroFlags>().to_string() == "None");
  REQUIRE(bitset<NoEqualFlags>().to_string() == "None");
  REQUIRE(bitset<NoEqualFlags>{NoEqualFlags::B}.to_string() == "B");
  REQUIRE(bitset<ZeroFlags>("None|A").to_string() == "A");
  REQUIRE(bitset<ZeroFlags>("None").none());
  constexpr bitset<ZeroFlags, ZeroInsideIndex> parsed{"None,B,A,None", ','};
  static_assert(parsed.all());
  REQUIRE(parsed.to_string() == "A|B");
  REQUIRE(parsed.to_string(magic_enum::containers::raw_access) == "11");
  constexpr bitset<Numbers> common_zero{"ONE"};
  static_assert(common_zero.count() == 1 && common_zero.test(Numbers::ONE));
  REQUIRE(std::hash<bitset<ZeroFlags>>{}(bitset<ZeroFlags>{ZeroFlags::None}) == std::hash<bitset<ZeroFlags>>{}(bitset<ZeroFlags>{}));

  bitset<ZeroFlags, ZeroInsideIndex> inside;
  inside.set(ZeroFlags::B);
  REQUIRE_THROWS_AS(inside.set(ZeroFlags::None), std::out_of_range);
  REQUIRE(inside.size() == 2);
  REQUIRE(inside.to_string(magic_enum::containers::raw_access) == "01");
  REQUIRE(*inside.begin() == ZeroFlags::B);
  inside.set(ZeroFlags::A);
  REQUIRE(std::vector<ZeroFlags>(inside.begin(), inside.end()) == std::vector<ZeroFlags>{ZeroFlags::A, ZeroFlags::B});
}

TEST_CASE("bitset to_string with a custom String") {
  using magic_enum::containers::bitset;
  using magic_enum::containers::raw_access;
  using magic_enum_tests::FixedString;
  using Name = FixedString<16>;

  constexpr bitset<Color> colors{Color::RED, Color::BLUE};
  static_assert(std::is_same_v<decltype(colors.to_string()), std::string>);
  static_assert(std::is_same_v<decltype(colors.to_string(raw_access)), std::string>);
  static_assert(colors.to_string<Name>() == "RED|BLUE");
  static_assert(colors.to_string<Name>(',') == "RED,BLUE");
  static_assert(colors.to_string<Name>(raw_access) == "101");
  static_assert(colors.to_string<Name>(raw_access, '-', '+') == "+-+");
  static_assert(colors.to_string<Name>(raw_access).reserved == 3);
  static_assert(bitset<Color>{}.to_string<Name>().empty());
  static_assert(bitset<ZeroFlags>{}.to_string<Name>() == "None");

  constexpr auto truncated = colors.to_string<FixedString<2>>(raw_access);
  static_assert(truncated == "10" && truncated.overflow);

  bitset<Color> runtime;
  runtime.set(Color::GREEN);
  REQUIRE(runtime.to_string<Name>() == "GREEN");
  REQUIRE(runtime.to_string<std::string>(raw_access) == "010");
}

TEST_CASE("bitset string overflow does not select the zero name") {
  struct Name : magic_enum_tests::FixedString<4> {
    using FixedString::append;

    constexpr void append(const char* s, size_type count) noexcept {
      if (count > 4 - size()) {
        overflow = true;
      } else {
        FixedString::append(s, count);
      }
    }
  };

  using magic_enum::containers::bitset;
  constexpr auto enabled = bitset<BoolFlags>{BoolFlags::Enabled}.to_string<Name>();
  static_assert(enabled.empty() && enabled.overflow);
  constexpr auto zero = bitset<BoolFlags>{}.to_string<Name>();
  static_assert(zero == "None" && !zero.overflow);
}

template <typename S, typename E, typename = void>
inline constexpr bool has_from_mask_v = false;

template <typename S, typename E>
inline constexpr bool has_from_mask_v<S, E, std::void_t<decltype(S::from_mask(std::declval<E>()))>> = true;

template <typename S, typename V, typename = void>
inline constexpr bool has_explicit_from_mask_v = false;

template <typename S, typename V>
inline constexpr bool has_explicit_from_mask_v<S, V, std::void_t<decltype(S::template from_mask<V>(std::declval<typename S::value_type>()))>> = true;

template <typename S, typename T, typename = void>
inline constexpr bool has_emplace_v = false;

template <typename S, typename T>
inline constexpr bool has_emplace_v<S, T, std::void_t<decltype(std::declval<S&>().emplace(std::declval<T>()))>> = true;

template <typename S, typename T, typename = void>
inline constexpr bool has_emplace_hint_v = false;

template <typename S, typename T>
inline constexpr bool has_emplace_hint_v<S, T, std::void_t<decltype(std::declval<S&>().emplace_hint(std::declval<typename S::const_iterator>(), std::declval<T>()))>> = true;

template <typename T, typename = void>
inline constexpr bool has_bitset_guide_v = false;

template <typename T>
inline constexpr bool has_bitset_guide_v<T, std::void_t<decltype(magic_enum::containers::bitset(std::declval<T>()))>> = true;

TEST_CASE("set takes keys, masks go through from_mask") {
  using namespace magic_enum::bitwise_operators;
  using magic_enum::containers::set;
  constexpr auto unknown = static_cast<ZeroFlags>(4);

  static_assert(!std::is_constructible_v<set<ZeroFlags>, ZeroFlags>);
  static_assert(!std::is_constructible_v<set<ZeroFlags>, ZeroFlags, ZeroFlags>);
  static_assert(!std::is_constructible_v<set<Numbers>, Numbers>);
  static_assert(std::is_same_v<decltype(set{ZeroFlags::A}), set<ZeroFlags>>);
  static_assert(has_from_mask_v<set<ZeroFlags>, ZeroFlags>);
  static_assert(!has_from_mask_v<set<Numbers>, Numbers>);
  static_assert(!has_from_mask_v<set<ZeroFlags>, int>);
  static_assert(has_explicit_from_mask_v<set<ZeroFlags>, ZeroFlags>);
  static_assert(!has_explicit_from_mask_v<set<Numbers>, ZeroFlags>);
  static_assert(!has_explicit_from_mask_v<set<ZeroFlags>, Color>);
  // An integer is not a key, as in std::set.
  static_assert(has_emplace_v<set<Numbers>, Numbers>);
  static_assert(!has_emplace_v<set<Numbers>, int>);
  static_assert(!has_emplace_v<set<ZeroFlags>, int>);
  static_assert(has_emplace_hint_v<set<Numbers>, Numbers>);
  static_assert(!has_emplace_hint_v<set<Numbers>, int>);
  static_assert(has_bitset_guide_v<ZeroFlags>);
  static_assert(!has_bitset_guide_v<int>);
  static_assert(std::is_same_v<decltype(set<ZeroFlags, std::greater<>>::from_mask(ZeroFlags::A)), set<ZeroFlags, std::greater<>>>);
  static_assert(*set<ZeroFlags, std::greater<>>::from_mask(ZeroFlags::A | ZeroFlags::B).begin() == ZeroFlags::B);
  static_assert(!noexcept(std::declval<set<ZeroFlags>&>().insert(ZeroFlags::A)));
  static_assert(!noexcept(std::declval<set<ZeroFlags>&>().insert(std::declval<std::initializer_list<ZeroFlags>>())));

  REQUIRE_THROWS_AS((void)set<ZeroFlags>{ZeroFlags::A | ZeroFlags::B}, std::invalid_argument);
  REQUIRE_THROWS_AS((void)set<Numbers>{static_cast<Numbers>(7)}, std::invalid_argument);
  REQUIRE_THROWS_AS((void)set<ZeroFlags>::from_mask(unknown), std::invalid_argument);

  set<ZeroFlags> keys;
  const ZeroFlags input[] = {ZeroFlags::A, ZeroFlags::A | ZeroFlags::B, ZeroFlags::B};
  REQUIRE_THROWS_AS(keys.insert(std::begin(input), std::end(input)), std::invalid_argument);
  // Range insertion keeps the keys added before the error.
  REQUIRE(keys.size() == 1);
  REQUIRE(std::distance(keys.begin(), keys.end()) == 1);
  REQUIRE(keys.contains(ZeroFlags::A));

  // A failed list assignment keeps the old contents.
  REQUIRE_THROWS_AS((keys = {ZeroFlags::None, ZeroFlags::A | ZeroFlags::B}), std::invalid_argument);
  REQUIRE(keys.size() == 1);
  REQUIRE(keys.contains(ZeroFlags::A));
  keys = {ZeroFlags::None, ZeroFlags::B};
  REQUIRE(keys.size() == 2);
  REQUIRE_FALSE(keys.contains(ZeroFlags::A));

  REQUIRE_THROWS_AS((void)keys.emplace(unknown), std::invalid_argument);
  REQUIRE_THROWS_AS((void)keys.insert(keys.begin(), unknown), std::invalid_argument);
  REQUIRE_FALSE(keys.insert(ZeroFlags::B).second);
  REQUIRE(keys.size() == 2);
  REQUIRE_FALSE(keys.contains(unknown));
  REQUIRE(keys.find(unknown) == keys.end());
  REQUIRE(keys.erase(unknown) == 0);
}

TEST_CASE("set emplace constructs enum keys") {
  using magic_enum::containers::set;
  struct Key {
    ZeroFlags value;
    constexpr explicit operator ZeroFlags() && noexcept { return value; }
  };
  static_assert(has_emplace_v<set<ZeroFlags>, Key>);
  static_assert(!has_emplace_v<set<ZeroFlags>, Key&>);
  static_assert(has_emplace_hint_v<set<ZeroFlags>, Key>);
  static_assert(!has_emplace_hint_v<set<ZeroFlags>, Key&>);

  constexpr auto constructed = [] {
    set<ZeroFlags> keys;
    keys.emplace();
    keys.emplace(Key{ZeroFlags::A});
    keys.emplace_hint(keys.end(), Key{ZeroFlags::B});
    return keys;
  }();
  static_assert(constructed.size() == 3);

  set<ZeroFlags> keys;
  const auto zero = keys.emplace();
  REQUIRE(zero.second);
  REQUIRE(*zero.first == ZeroFlags::None);
  REQUIRE(*keys.emplace_hint(keys.end()) == ZeroFlags::None);
  REQUIRE(keys.emplace(Key{ZeroFlags::A}).second);
  REQUIRE(*keys.emplace_hint(keys.end(), Key{ZeroFlags::B}) == ZeroFlags::B);
  REQUIRE_FALSE(keys.emplace(Key{ZeroFlags::A}).second);
  const auto unknown = static_cast<ZeroFlags>(4);
  REQUIRE_THROWS_AS(keys.emplace(Key{unknown}), std::invalid_argument);
  REQUIRE_THROWS_AS(keys.emplace_hint(keys.end(), Key{unknown}), std::invalid_argument);
  REQUIRE(keys == constructed);

  set<Color> unnamed_zero;
  REQUIRE_THROWS_AS(unnamed_zero.emplace(), std::invalid_argument);
  REQUIRE(unnamed_zero.empty());
}

TEST_CASE_TEMPLATE("named zero preserves full-width flag storage boundaries", E, ZeroFlags8, ZeroFlags64) {
  using magic_enum::containers::bitset;
  using magic_enum::containers::raw_access;
  using magic_enum::containers::set;
  using U = magic_enum::underlying_type_t<E>;
  constexpr auto width = static_cast<std::size_t>(std::numeric_limits<U>::digits);
  constexpr auto full_mask = static_cast<unsigned long long>((std::numeric_limits<U>::max)());
  constexpr auto high_mask = 1ULL << (width - 1);
  constexpr auto high = static_cast<E>(high_mask);
  constexpr auto values = magic_enum::enum_values<E>();

  static_assert(values.size() == width + 1);
  static_assert(bitset<E>{}.size() == width);
  static_assert(set<E>{}.max_size() == width + 1);
  constexpr bitset<E> full{raw_access, full_mask};
  static_assert(full.all() && full.count() == width);
  static_assert(full.to_ullong(raw_access) == full_mask);
  static_assert(static_cast<E>(full) == static_cast<E>(full_mask));
  static_assert(full.test(high));

  bitset<E> bits;
  bits.set();
  REQUIRE(bits == full);
  REQUIRE(bitset<E>(bits.to_string()) == full);
  REQUIRE(bits.to_string(raw_access) == std::string(width, '1'));
  REQUIRE(bits.find(E::None) == bits.end());
  REQUIRE(std::hash<bitset<E>>{}(bits) == std::hash<unsigned long long>{}(full_mask));

  std::size_t index = 1;
  for (const auto value : bits) {
    REQUIRE(index < values.size());
    REQUIRE(value == values.at(index++));
  }
  REQUIRE(index == values.size());
  auto it = bits.end();
  while (index > 1) {
    REQUIRE(*--it == values[--index]);
  }
  REQUIRE(it == bits.begin());

  bits.flip();
  REQUIRE(bits.none());
  REQUIRE(bits.count() == 0);
  REQUIRE(bits.begin() == bits.end());
  REQUIRE(bits.to_ullong(raw_access) == 0);
  REQUIRE(bits.to_string() == "None");
  bits.flip();
  REQUIRE(bits == full);

  bits.reset().set(high);
  REQUIRE(bits.count() == 1);
  REQUIRE(bits.to_ullong(raw_access) == high_mask);
  REQUIRE(*bits.begin() == high);
  REQUIRE(std::next(bits.begin()) == bits.end());
  REQUIRE(*std::prev(bits.end()) == high);
  REQUIRE(std::hash<bitset<E>>{}(bits) == std::hash<unsigned long long>{}(high_mask));

  set<E> keys(values.begin(), values.end());
  REQUIRE(keys.size() == width + 1);
  REQUIRE(keys.contains(E::None));
  REQUIRE(keys.contains(high));
  REQUIRE(std::equal(keys.begin(), keys.end(), values.begin(), values.end()));
  REQUIRE(*std::prev(keys.end()) == high);
  REQUIRE(keys.erase(E::None) == 1);
  REQUIRE(keys.size() == width);
  REQUIRE_FALSE(keys.contains(E::None));
  REQUIRE(keys.contains(E::B0));
  REQUIRE(keys.contains(high));
  REQUIRE(std::equal(keys.begin(), keys.end(), std::next(values.begin()), values.end()));
  REQUIRE(keys.erase(high) == 1);
  REQUIRE(keys.size() == width - 1);
  REQUIRE_FALSE(keys.contains(high));
  REQUIRE(keys.insert(E::None).second);
  REQUIRE(keys.insert(high).second);
  REQUIRE(std::equal(keys.begin(), keys.end(), values.begin(), values.end()));
}

TEST_CASE("bitset custom index keeps its contract") {
  magic_enum::containers::bitset<Color, ZeroCollidingIndex> colliding;
  REQUIRE(colliding.size() == 3);
  colliding.set(Color::RED);
  colliding.set(Color::BLUE);
  REQUIRE(colliding.test(Color::RED));
  REQUIRE(colliding.to_string() == "RED|BLUE");

  const magic_enum::containers::bitset<Color, ReverseIteratorIndex> reversed{Color::RED, Color::BLUE};
  REQUIRE(reversed.size() == 3);
  REQUIRE(std::vector<Color>(reversed.begin(), reversed.end()) == std::vector<Color>{Color::BLUE, Color::RED});
}

TEST_CASE("containers_set") {

  using namespace magic_enum::bitwise_operators;
  using namespace magic_enum::ostream_operators;

  auto color_set = magic_enum::containers::set<Color>();
  REQUIRE(color_set.empty());
  REQUIRE(color_set.size() == 0);
  REQUIRE_FALSE(magic_enum::enum_count<Color>() == color_set.size());

  color_set.insert(Color::RED);
  std::ignore = color_set.insert(Color::RED);
  color_set.insert(Color::GREEN);
  color_set.insert(Color::BLUE);
  color_set.insert(Color::RED);
  REQUIRE_THROWS_AS(color_set.insert(Color::RED|Color::GREEN), std::invalid_argument);
  REQUIRE_THROWS_AS(color_set.insert(Color::RED|Color::GREEN|Color::BLUE), std::invalid_argument);

  REQUIRE_FALSE(color_set.empty());
  REQUIRE(color_set.size() == 3);
  REQUIRE(magic_enum::enum_count<Color>() == color_set.size());
  color_set.erase(Color::RED);
  color_set.erase(Color::GREEN);
  REQUIRE(magic_enum::enum_count<Color>() - 2 == color_set.size());
  REQUIRE(color_set.count(Color::RED) == 0);
  REQUIRE_FALSE(color_set.contains(Color::GREEN));
  REQUIRE(color_set.contains(Color::BLUE));

  auto color_set_compare = magic_enum::containers::set<Color>();
  color_set_compare.insert(Color::BLUE);
  color_set_compare.insert(Color::RED);
  color_set_compare.insert(Color::GREEN);

  REQUIRE_THROWS(static_cast<void>(ThrowingSet{ThrowingEnum::A} < ThrowingSet{ThrowingEnum::B}));

  constexpr magic_enum::containers::set color_set_filled = {Color::RED, Color::GREEN, Color::BLUE};
  REQUIRE_FALSE(color_set_filled.empty());
  REQUIRE(color_set_filled.size() == 3);
  REQUIRE(magic_enum::enum_count<Color>() == color_set_filled.size());

  magic_enum::containers::set color_set_not_const = {Color::RED, Color::GREEN, Color::BLUE};
  REQUIRE_FALSE(color_set_not_const.empty());
  REQUIRE(color_set_not_const.size() == 3);
  REQUIRE(magic_enum::enum_count<Color>() == color_set_not_const.size());
  color_set_not_const.clear();
  REQUIRE(color_set_not_const.empty());
  REQUIRE(color_set_not_const.size() == 0);
  REQUIRE_FALSE(magic_enum::enum_count<Color>() == color_set_not_const.size());

  magic_enum::containers::set<Color> color_set_assign {Color::RED, Color::BLUE};
  color_set_assign = {Color::GREEN};
  REQUIRE(color_set_assign.size() == 1);
  REQUIRE(color_set_assign.contains(Color::GREEN));
  REQUIRE_FALSE(color_set_assign.contains(Color::RED));
  REQUIRE_FALSE(color_set_assign.contains(Color::BLUE));

  const auto color_set_mask = magic_enum::containers::set<Color>::from_mask(Color::RED | Color::BLUE);
  REQUIRE(color_set_mask.size() == 2);
  REQUIRE(color_set_mask.contains(Color::RED));
  REQUIRE(color_set_mask.contains(Color::BLUE));
  REQUIRE(magic_enum::containers::set<Color>::from_mask(static_cast<Color>(0)).empty());
  REQUIRE_THROWS_AS((void)magic_enum::containers::set<Color>::from_mask(Color::RED | static_cast<Color>(8)), std::invalid_argument);

  magic_enum::containers::set<Color, magic_enum::containers::name_less<>> color_name_set {Color::RED, Color::GREEN, Color::BLUE};
  constexpr magic_enum::string_view green_name = "GREEN";
  constexpr magic_enum::string_view blue_name = "BLUE";
  constexpr magic_enum::string_view purple_name = "PURPLE";
  const std::string blue_name_string = "BLUE";
  REQUIRE(color_name_set.contains(green_name));
  REQUIRE(color_name_set.contains(blue_name_string));
  REQUIRE(color_name_set.count(purple_name) == 0);
  REQUIRE(color_name_set.key_comp()(Color::BLUE, "GREEN"));
  REQUIRE(color_name_set.key_comp()("GREEN", Color::RED));
  REQUIRE_FALSE(color_name_set.key_comp()("GREEN", Color::BLUE));
  auto blue_by_literal = color_name_set.find("BLUE");
  REQUIRE(blue_by_literal != color_name_set.end());
  REQUIRE(*blue_by_literal == Color::BLUE);
  auto blue_by_string_view = color_name_set.find(blue_name);
  REQUIRE(blue_by_string_view != color_name_set.end());
  REQUIRE(*blue_by_string_view == Color::BLUE);
  auto blue_by_string = color_name_set.find(blue_name_string);
  REQUIRE(blue_by_string != color_name_set.end());
  REQUIRE(*blue_by_string == Color::BLUE);
  REQUIRE_THROWS(static_cast<void>(color_name_set.find(ThrowingConversion{})));
  REQUIRE(color_name_set.erase("GREEN") == 1);
  REQUIRE_FALSE(color_name_set.contains(Color::GREEN));
  REQUIRE_THROWS(static_cast<void>(color_name_set.emplace(ThrowingConversion{})));
  REQUIRE_THROWS(static_cast<void>(color_name_set.emplace_hint(color_name_set.end(), ThrowingConversion{})));

  static_assert(!noexcept(color_name_set.contains(green_name)));

  magic_enum::containers::set<Color> erase_set {Color::RED, Color::GREEN, Color::BLUE};
  auto empty_first = erase_set.find(Color::GREEN);
  auto empty_result = erase_set.erase(empty_first, empty_first);
  REQUIRE(empty_result == empty_first);
  REQUIRE(erase_set.size() == 3);
  REQUIRE(erase_set.erase(static_cast<Color>(8)) == 0);
  REQUIRE(erase_set.size() == 3);

  auto erase_first = erase_set.find(Color::RED);
  auto erase_last = erase_set.find(Color::BLUE);
  REQUIRE(erase_set.erase(erase_first, erase_last) == erase_last);
  REQUIRE_FALSE(erase_set.contains(Color::RED));
  REQUIRE_FALSE(erase_set.contains(Color::GREEN));
  REQUIRE(erase_set.contains(Color::BLUE));

  magic_enum::containers::set<Color> bounds_set {Color::GREEN, Color::BLUE};
  REQUIRE(*bounds_set.lower_bound(Color::RED) == Color::GREEN);
  REQUIRE(*bounds_set.lower_bound(Color::RED | Color::GREEN) == Color::BLUE);
  REQUIRE(*bounds_set.upper_bound(Color::GREEN) == Color::BLUE);
  REQUIRE(bounds_set.upper_bound(Color::BLUE) == bounds_set.end());

  magic_enum::containers::set<Color> compare_lhs {Color::BLUE};
  magic_enum::containers::set<Color> compare_rhs {Color::RED, Color::GREEN};
  REQUIRE_FALSE(compare_lhs < compare_rhs);
  REQUIRE(compare_rhs < compare_lhs);

  magic_enum::containers::set<Color, magic_enum::containers::name_less<>> name_bounds_set {Color::BLUE, Color::RED};
  REQUIRE(*name_bounds_set.lower_bound("BLACK") == Color::BLUE);
  REQUIRE(*name_bounds_set.lower_bound("GREEN") == Color::RED);
  REQUIRE(*name_bounds_set.upper_bound("BLUE") == Color::RED);
  const auto [missing_first, missing_last] = name_bounds_set.equal_range("GREEN");
  REQUIRE(missing_first == missing_last);
  REQUIRE(*missing_first == Color::RED);

  magic_enum::containers::set<Color, magic_enum::containers::name_less_case_insensitive> color_ci_name_set {Color::RED, Color::GREEN, Color::BLUE};
  constexpr magic_enum::string_view green_name_lower = "green";
  constexpr magic_enum::string_view blue_name_lower = "blue";
  const std::string blue_name_lower_string = "blue";
  REQUIRE(color_ci_name_set.contains(green_name_lower));
  REQUIRE(color_ci_name_set.contains(blue_name_lower_string));
  auto blue_ci_by_literal = color_ci_name_set.find("blue");
  REQUIRE(blue_ci_by_literal != color_ci_name_set.end());
  REQUIRE(*blue_ci_by_literal == Color::BLUE);
  auto blue_ci_by_string_view = color_ci_name_set.find(blue_name_lower);
  REQUIRE(blue_ci_by_string_view != color_ci_name_set.end());
  REQUIRE(*blue_ci_by_string_view == Color::BLUE);
  auto blue_ci_by_string = color_ci_name_set.find(blue_name_lower_string);
  REQUIRE(blue_ci_by_string != color_ci_name_set.end());
  REQUIRE(*blue_ci_by_string == Color::BLUE);

  magic_enum::containers::set<EquivalentNames, magic_enum::containers::name_less_case_insensitive> equivalent_name_set {EquivalentNames::A, EquivalentNames::a};
  REQUIRE(equivalent_name_set.size() == 2);
  REQUIRE(equivalent_name_set.count("a") == 2);
  auto exact = EquivalentNames::A;
  REQUIRE(equivalent_name_set.erase(exact) == 1);
  REQUIRE_FALSE(equivalent_name_set.contains(EquivalentNames::A));
  REQUIRE(equivalent_name_set.contains(EquivalentNames::a));
  REQUIRE(equivalent_name_set.insert(EquivalentNames::A).second);
  REQUIRE(equivalent_name_set.erase("a") == 2);
  REQUIRE(equivalent_name_set.empty());
}

TEST_CASE("set heterogeneous lookup skips absent equivalent keys") {
  constexpr auto check = [] {
    using namespace magic_enum::containers;
    set<EquivalentNames, name_less_case_insensitive> keys{EquivalentNames::a};
    if (keys.count("A") != 1 || keys.find("A") == keys.end() || *keys.find("A") != EquivalentNames::a) {
      return false;
    }
    if (keys.count("missing") != 0 || keys.find("missing") != keys.end() || keys.erase("missing") != 0) {
      return false;
    }
    if (keys.erase("A") != 1 || keys.erase("A") != 0 || !keys.empty() || keys.find("A") != keys.end()) {
      return false;
    }
    keys.insert(EquivalentNames::A);
    keys.insert(EquivalentNames::a);
    const auto next = keys.erase(keys.begin());
    return keys.size() == 1 && next != keys.end() && *next == EquivalentNames::a &&
           keys.count("a") == 1 && keys.erase("a") == 1 && keys.empty();
  };
  static_assert(check());
  CHECK(check());
}

TEST_CASE("set positional erase preserves reordered keys across words") {
  constexpr auto check = [] {
    using namespace magic_enum::containers;
    set<Bits65, name_greater<>> keys{Bits65::B00, Bits65::B01, Bits65::B63, Bits65::B64};
    auto next = keys.erase(keys.find("B64"));
    if (keys.size() != 3 || next == keys.end() || *next != Bits65::B63 || keys.count("B64") != 0) {
      return false;
    }
    if (keys.erase("B63") != 1 || keys.erase("B63") != 0 || keys.count("B00") != 1) {
      return false;
    }
    next = keys.erase(keys.begin(), keys.find("B00"));
    if (keys.size() != 1 || next == keys.end() || *next != Bits65::B00 || keys.erase(next) != keys.end() || !keys.empty()) {
      return false;
    }
    set<ZeroFlags, name_greater<>> flags{ZeroFlags::None, ZeroFlags::A, ZeroFlags::B};
    next = keys.erase(keys.begin(), keys.end());
    if (next != keys.end() || flags.count("None") != 1 || flags.erase("None") != 1 || flags.erase(ZeroFlags::None) != 0) {
      return false;
    }
    const auto last = flags.erase(flags.find("A"));
    return last == flags.end() && flags.size() == 1 && flags.count("B") == 1 && flags.erase(flags.begin()) == flags.end() && flags.empty();
  };
  static_assert(check());
  CHECK(check());
}

TEST_CASE("map_like_container") {

  using namespace magic_enum::ostream_operators;

  std::vector<std::pair<Color, RGB>> map {{Color::GREEN, {0, color_max, 0}}, {Color::BLUE, {0, 0, color_max}}, {Color::RED, {color_max, 0, 0}}};
  for (auto [key, value] : map) {

    std::cout << "Key=" << key << " Value=" << value << std::endl;
  }
  auto compare = [](std::pair<Color, RGB>& lhs,
                    std::pair<Color, RGB>& rhs) {
     return static_cast<std::int32_t>(lhs.first) < static_cast<std::int32_t>(rhs.first);
  };
  std::sort(std::begin(map), std::end(map), compare);
  for (auto [key, value] : map) {

    std::cout << "Key=" << key << " Value=" << value << std::endl;
  }
}

TEST_CASE("containers_set_reverse_iterator") {
  using Set = magic_enum::containers::set<Color>;
  const Set empty;
  REQUIRE(empty.rbegin() == empty.rend());
  REQUIRE(empty.crbegin() == empty.crend());

  const Set sparse{Color::RED, Color::BLUE};
  auto it = sparse.rbegin();
  REQUIRE(*it == Color::BLUE);
  REQUIRE(*it++ == Color::BLUE);
  REQUIRE(*it == Color::RED);
  REQUIRE(++it == sparse.rend());
  REQUIRE(*--it == Color::RED);
  REQUIRE(*--it == Color::BLUE);
  REQUIRE(it == sparse.crbegin());
  REQUIRE(std::distance(sparse.crbegin(), sparse.crend()) == 2);

  const Set singleton{Color::GREEN};
  REQUIRE(*singleton.rbegin() == Color::GREEN);
  REQUIRE(std::next(singleton.crbegin()) == singleton.crend());

  const magic_enum::containers::set<Color, magic_enum::containers::name_less<>> by_name{Color::RED, Color::BLUE};
  REQUIRE(*by_name.rbegin() == Color::RED);
  REQUIRE(*std::next(by_name.rbegin()) == Color::BLUE);
  REQUIRE(std::distance(by_name.rbegin(), by_name.rend()) == 2);
}

template <typename E, typename Cmp = std::less<>>
void check_set_bounds(std::initializer_list<E> keys) {
  const magic_enum::containers::set<E, Cmp> actual{keys};
  const std::set<E, Cmp> expected{keys};
  for (const E key : magic_enum::enum_values<E>()) {
    const auto lower = actual.lower_bound(key);
    const auto expected_lower = expected.lower_bound(key);
    REQUIRE((lower == actual.end()) == (expected_lower == expected.end()));
    if (expected_lower != expected.end()) {
      CHECK(*lower == *expected_lower);
    }
    const auto upper = actual.upper_bound(key);
    const auto expected_upper = expected.upper_bound(key);
    REQUIRE((upper == actual.end()) == (expected_upper == expected.end()));
    if (expected_upper != expected.end()) {
      CHECK(*upper == *expected_upper);
    }
  }
}

TEST_CASE("containers_set bounds skip empty words and stop at the end") {
  check_set_bounds<Bits65>({});
  check_set_bounds<Bits65>({Bits65::B64});
  check_set_bounds<Bits65>({Bits65::B00, Bits65::B63});
  check_set_bounds<Wide256>({Wide256::W000, Wide256::W127, Wide256::W128, Wide256::W255});
  check_set_bounds<Wide256, std::greater<>>({Wide256::W000, Wide256::W127, Wide256::W128, Wide256::W255});
  check_set_bounds<Wide256, std::greater<>>({});

  constexpr magic_enum::containers::set<Bits65> last{Bits65::B64};
  static_assert(*last.lower_bound(Bits65::B00) == Bits65::B64);
  static_assert(*last.lower_bound(Bits65::B64) == Bits65::B64);
  static_assert(last.upper_bound(Bits65::B64) == last.end());
  static_assert(last.lower_bound(static_cast<Bits65>(65)) == last.end());
  constexpr magic_enum::containers::set<Bits65> empty;
  static_assert(empty.lower_bound(Bits65::B00) == empty.end());
  static_assert(empty.upper_bound(Bits65::B64) == empty.end());
}

TEST_CASE("containers_set iterator preserves references and postfix results") {
  using Set = magic_enum::containers::set<Bits65>;
  static_assert(std::is_same_v<typename std::iterator_traits<Set::iterator>::reference, const Bits65&>);
  static_assert(std::is_same_v<typename std::iterator_traits<Set::iterator>::pointer, const Bits65*>);
  const Set values{Bits65::B01, Bits65::B64};
  auto it = values.begin();
  REQUIRE(it.operator->() == std::addressof(*it));
  const auto first = it++;
  REQUIRE(*first == Bits65::B01);
  REQUIRE(*it == Bits65::B64);
  const auto last = it++;
  REQUIRE(*last == Bits65::B64);
  REQUIRE(it == values.end());
  REQUIRE(*--it == Bits65::B64);
  const auto previous = it--;
  REQUIRE(*previous == Bits65::B64);
  REQUIRE(it == values.begin());
  REQUIRE(*it == Bits65::B01);
  REQUIRE(std::vector<Bits65>(values.rbegin(), values.rend()) == std::vector<Bits65>{Bits65::B64, Bits65::B01});
}

TEST_CASE("containers_set iterates across storage words") {
  using namespace magic_enum::containers;
  // Keys across word boundaries and empty words keep the order.
  const std::vector<Bits65> keys{Bits65::B00, Bits65::B07, Bits65::B08, Bits65::B63, Bits65::B64};
  const set<Bits65> wide(keys.begin(), keys.end());
  REQUIRE(std::vector<Bits65>(wide.begin(), wide.end()) == keys);
  REQUIRE(std::vector<Bits65>(wide.rbegin(), wide.rend()) == std::vector<Bits65>(keys.rbegin(), keys.rend()));

  const set<Bits65> last{Bits65::B64};
  REQUIRE(*last.begin() == Bits65::B64);
  REQUIRE(std::next(last.begin()) == last.end());

  set<Flags9> nine{Flags9::A, Flags9::H, Flags9::I};
  nine.erase(Flags9::H);
  REQUIRE(std::vector<Flags9>(nine.begin(), nine.end()) == std::vector<Flags9>{Flags9::A, Flags9::I});
  REQUIRE(*nine.lower_bound(Flags9::B) == Flags9::I);

  const set<Wide256, std::greater<>> reversed{Wide256::W000, Wide256::W127, Wide256::W128, Wide256::W255};
  REQUIRE(std::vector<Wide256>(reversed.begin(), reversed.end()) == std::vector<Wide256>{Wide256::W255, Wide256::W128, Wide256::W127, Wide256::W000});
  REQUIRE(std::vector<Wide256>(reversed.rbegin(), reversed.rend()) == std::vector<Wide256>{Wide256::W000, Wide256::W127, Wide256::W128, Wide256::W255});

  constexpr set<Wide256> separated{Wide256::W127, Wide256::W255};
  static_assert(*std::prev(separated.end()) == Wide256::W255);
  static_assert(std::prev(separated.end(), 2) == separated.begin());
  static_assert(*std::prev(separated.end(), 2) == Wide256::W127);


  constexpr set<Bits65> sparse{Bits65::B01, Bits65::B64};
  static_assert(*std::next(sparse.begin()) == Bits65::B64);
  static_assert(std::next(sparse.begin(), 2) == sparse.end());

  const set<Bits65> empty;
  REQUIRE(empty.begin() == empty.end());
  REQUIRE(empty.lower_bound(Bits65::B00) == empty.end());
  REQUIRE(empty.find(Bits65::B64) == empty.end());

  set<Bits65> erased{Bits65::B00, Bits65::B64};
  REQUIRE(*std::prev(erased.end()) == Bits65::B64);
  erased.erase(Bits65::B64);
  REQUIRE(*std::prev(erased.end()) == Bits65::B00);
  REQUIRE(std::next(erased.begin()) == erased.end());
  erased.erase(Bits65::B00);
  REQUIRE(erased.begin() == erased.end());

  // A named zero and a signed high bit keep the order of enum_values.
  const set<ZeroFlags> zero{ZeroFlags::B, ZeroFlags::None};
  REQUIRE(std::vector<ZeroFlags>(zero.begin(), zero.end()) == std::vector<ZeroFlags>{ZeroFlags::None, ZeroFlags::B});
  REQUIRE(*std::prev(zero.end()) == ZeroFlags::B);
  const set<SignedHighFlags> high{SignedHighFlags::Low, SignedHighFlags::High, SignedHighFlags::None};
  REQUIRE(std::vector<SignedHighFlags>(high.begin(), high.end()) == std::vector<SignedHighFlags>{SignedHighFlags::High, SignedHighFlags::None, SignedHighFlags::Low});
  REQUIRE(std::vector<SignedHighFlags>(high.rbegin(), high.rend()) == std::vector<SignedHighFlags>{SignedHighFlags::Low, SignedHighFlags::None, SignedHighFlags::High});
  const set<SignedHighFlags, std::greater<>> greater{SignedHighFlags::High, SignedHighFlags::Low, SignedHighFlags::None};
  REQUIRE(std::vector<SignedHighFlags>(greater.begin(), greater.end()) == std::vector<SignedHighFlags>{SignedHighFlags::Low, SignedHighFlags::None, SignedHighFlags::High});
}

template <typename E>
constexpr bool bitset_reverse_constexpr() {
  magic_enum::containers::bitset<E> bits;
  constexpr auto values = magic_enum::enum_values<E>();
  bits.set(values.front());
  bits.set(values.back());
  auto it = bits.end();
  if (*--it != values.back() || *--it != values.front()) {
    return false;
  }
  const auto& const_bits = bits;
  auto cit = const_bits.end();
  return *--cit == values.back() && *--cit == values.front() && ++it == ++cit && ++it == bits.end();
}

#if !defined(_MSC_VER) || defined(__clang__) || (defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L)
static_assert(bitset_reverse_constexpr<Numbers>());
static_assert(bitset_reverse_constexpr<Nibble>());
static_assert(bitset_reverse_constexpr<Bits65>());
#endif

TEST_CASE("containers_bitset_iterator") {
  REQUIRE(bitset_reverse_constexpr<Numbers>());
  REQUIRE(bitset_reverse_constexpr<Nibble>());
  REQUIRE(bitset_reverse_constexpr<Bits65>());
  using namespace magic_enum::bitwise_operators;

  SUBCASE("empty - begin equals end") {
    magic_enum::containers::bitset<Color> bs;
    REQUIRE(bs.begin() == bs.end());
    REQUIRE(bs.cbegin() == bs.cend());
    std::size_t n = 0;
    for ([[maybe_unused]] Color e : bs) { ++n; }
    REQUIRE(n == 0);
  }

  SUBCASE("single element - first") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::RED);
    REQUIRE(bs.begin() != bs.end());
    REQUIRE(*bs.begin() == Color::RED);
    auto it = bs.begin();
    ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("single element - middle") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::GREEN);
    REQUIRE(bs.begin() != bs.end());
    REQUIRE(*bs.begin() == Color::GREEN);
    auto it = bs.begin();
    ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("single element - last") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::BLUE);
    REQUIRE(bs.begin() != bs.end());
    REQUIRE(*bs.begin() == Color::BLUE);
    auto it = bs.begin();
    ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("forward - order matches enum_values") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    constexpr auto expected = magic_enum::enum_values<Color>();
    std::size_t i = 0;
    for (Color e : bs) {
      REQUIRE(i < expected.size());
      REQUIRE(e == expected[i++]);
    }
    REQUIRE(i == expected.size());
  }

  SUBCASE("forward - partial - first and last only") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::RED);
    bs.set(Color::BLUE);
    auto it = bs.begin();
    REQUIRE(*it == Color::RED);  ++it;
    REQUIRE(*it == Color::BLUE); ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("forward - post-increment") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    auto it   = bs.begin();
    auto prev = it++;
    REQUIRE(*prev == Color::RED);
    REQUIRE(*it   == Color::GREEN);
  }

  SUBCASE("backward - all set") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    auto it = bs.end();
    --it; REQUIRE(*it == Color::BLUE);
    --it; REQUIRE(*it == Color::GREEN);
    --it; REQUIRE(*it == Color::RED);
  }

  SUBCASE("backward - partial - first and last only") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::RED);
    bs.set(Color::BLUE);
    auto it = bs.end();
    --it; REQUIRE(*it == Color::BLUE);
    --it; REQUIRE(*it == Color::RED);
  }

  SUBCASE("backward - post-decrement") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    auto it   = bs.end();
    --it;
    auto prev = it--;
    REQUIRE(*prev == Color::BLUE);
    REQUIRE(*it   == Color::GREEN);
  }

  SUBCASE("bidirectional - round-trip") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    auto it = bs.begin();
    REQUIRE(*it == Color::RED);   ++it;
    REQUIRE(*it == Color::GREEN); ++it;
    REQUIRE(*it == Color::BLUE);  ++it;
    REQUIRE(it == bs.end());
    --it; REQUIRE(*it == Color::BLUE);
    --it; REQUIRE(*it == Color::GREEN);
    --it; REQUIRE(*it == Color::RED);
  }

  SUBCASE("find - present values") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::GREEN);
    bs.set(Color::BLUE);
    auto it = bs.find(Color::GREEN);
    REQUIRE(it != bs.end());
    REQUIRE(*it == Color::GREEN);
    it = bs.find(Color::BLUE);
    REQUIRE(it != bs.end());
    REQUIRE(*it == Color::BLUE);
  }

  SUBCASE("find - absent values") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::GREEN);
    REQUIRE(bs.find(Color::RED)  == bs.end());
    REQUIRE(bs.find(Color::BLUE) == bs.end());
  }

  SUBCASE("find - empty bitset") {
    magic_enum::containers::bitset<Color> bs;
    for (Color e : magic_enum::enum_values<Color>())
      REQUIRE(bs.find(e) == bs.end());
  }

  SUBCASE("find - all set, const bitset") {
    const magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    for (Color e : magic_enum::enum_values<Color>()) {
      auto it = bs.find(e);
      REQUIRE(it != bs.end());
      REQUIRE(*it == e);
    }
  }

  SUBCASE("find - iterator can be incremented to next element") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    auto it = bs.find(Color::GREEN);
    REQUIRE(*it == Color::GREEN);
    ++it;
    REQUIRE(*it == Color::BLUE);
  }

  SUBCASE("non-const begin returns mutable iterator") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::RED);
    auto it = bs.begin();
    REQUIRE_FALSE(check_const(it));
  }

  SUBCASE("cbegin/cend usable on non-const bitset") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    magic_enum::containers::bitset<Color>::const_iterator cit = bs.cbegin();
    std::size_t n = 0;
    while (cit != bs.cend()) { ++n; ++cit; }
    REQUIRE(n == 3);
  }

  SUBCASE("iterator to const_iterator implicit conversion") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::RED);
    magic_enum::containers::bitset<Color>::iterator       it  = bs.begin();
    magic_enum::containers::bitset<Color>::const_iterator cit = it;
    REQUIRE(*it == *cit);
    REQUIRE(it == cit);
  }

  SUBCASE("const range-for") {
    const magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    std::size_t n = 0;
    for ([[maybe_unused]] Color e : bs) { ++n; }
    REQUIRE(n == 3);
  }

  SUBCASE("operator->") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::RED);
    auto it = bs.begin();
    REQUIRE(it.operator->() == &(*it));
  }

  // Regression check for the B7/MSB path used by reverse iteration.
  SUBCASE("Nibble - single MSB element B7 (countl_zero regression)") {
    magic_enum::containers::bitset<Nibble> bs;
    bs.set(Nibble::B7);
    REQUIRE(*bs.begin() == Nibble::B7);
    auto it = bs.end();
    --it;
    REQUIRE(*it == Nibble::B7);
  }

  SUBCASE("Nibble - first and last (B0 and B7)") {
    magic_enum::containers::bitset<Nibble> bs;
    bs.set(Nibble::B0);
    bs.set(Nibble::B7);
    auto it = bs.begin();
    REQUIRE(*it == Nibble::B0); ++it;
    REQUIRE(*it == Nibble::B7); ++it;
    REQUIRE(it == bs.end());
    --it; REQUIRE(*it == Nibble::B7);
    --it; REQUIRE(*it == Nibble::B0);
  }

  SUBCASE("Nibble - forward iteration all 8 values") {
    magic_enum::containers::bitset<Nibble> bs;
    for (Nibble e : magic_enum::enum_values<Nibble>()) { bs.set(e); }
    constexpr auto expected = magic_enum::enum_values<Nibble>();
    std::size_t i = 0;
    for (Nibble e : bs) { REQUIRE(e == expected[i++]); }
    REQUIRE(i == expected.size());
  }

  SUBCASE("Nibble - backward iteration all 8 values") {
    magic_enum::containers::bitset<Nibble> bs;
    for (Nibble e : magic_enum::enum_values<Nibble>()) { bs.set(e); }
    constexpr auto expected = magic_enum::enum_values<Nibble>();
    auto it = bs.end();
    for (std::size_t j = expected.size(); j > 0; --j) {
      --it;
      REQUIRE(*it == expected[j - 1]);
    }
  }

  SUBCASE("Nibble - partial middle elements") {
    magic_enum::containers::bitset<Nibble> bs;
    bs.set(Nibble::B2);
    bs.set(Nibble::B5);
    auto it = bs.begin();
    REQUIRE(*it == Nibble::B2); ++it;
    REQUIRE(*it == Nibble::B5); ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("Bits32 - backward from exact storage boundary") {
    magic_enum::containers::bitset<Bits32> bs;
    bs.set(Bits32::B00);
    bs.set(Bits32::B31);
    auto it = bs.end();
    --it;
    REQUIRE(*it == Bits32::B31);
    --it;
    REQUIRE(*it == Bits32::B00);
  }

  SUBCASE("Bits65 - backward across storage words") {
    magic_enum::containers::bitset<Bits65> bs;
    bs.set(Bits65::B00);
    bs.set(Bits65::B64);
    auto it = bs.end();
    --it;
    REQUIRE(*it == Bits65::B64);
    --it;
    REQUIRE(*it == Bits65::B00);
  }

  SUBCASE("Numbers - forward iteration partial") {
    magic_enum::containers::bitset<Numbers> bs;
    bs.set(Numbers::ONE);
    bs.set(Numbers::THREE);
    auto it = bs.begin();
    REQUIRE(*it == Numbers::ONE);   ++it;
    REQUIRE(*it == Numbers::THREE); ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("Numbers - backward iteration partial") {
    magic_enum::containers::bitset<Numbers> bs;
    bs.set(Numbers::ONE);
    bs.set(Numbers::THREE);
    auto it = bs.end();
    --it; REQUIRE(*it == Numbers::THREE);
    --it; REQUIRE(*it == Numbers::ONE);
  }

  SUBCASE("Numbers - all values forward and backward") {
    magic_enum::containers::bitset<Numbers> bs;
    for (Numbers e : magic_enum::enum_values<Numbers>()) { bs.set(e); }
    constexpr auto expected = magic_enum::enum_values<Numbers>();
    std::size_t i = 0;
    for (Numbers e : bs) { REQUIRE(e == expected[i++]); }
    REQUIRE(i == expected.size());
    auto it = bs.end();
    for (std::size_t j = expected.size(); j > 0; --j) {
      --it; REQUIRE(*it == expected[j - 1]);
    }
  }

  SUBCASE("Flags9 - forward iteration partial") {
    magic_enum::containers::bitset<Flags9> bs;
    bs.set(Flags9::A);
    bs.set(Flags9::E);
    bs.set(Flags9::I);
    auto it = bs.begin();
    REQUIRE(*it == Flags9::A); ++it;
    REQUIRE(*it == Flags9::E); ++it;
    REQUIRE(*it == Flags9::I); ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("Flags9 - backward from end") {
    magic_enum::containers::bitset<Flags9> bs;
    bs.set(Flags9::A);
    bs.set(Flags9::I);
    auto it = bs.end();
    --it; REQUIRE(*it == Flags9::I);
    --it; REQUIRE(*it == Flags9::A);
  }

  SUBCASE("Flags9 - all 9 values forward and backward") {
    magic_enum::containers::bitset<Flags9> bs;
    for (Flags9 e : magic_enum::enum_values<Flags9>()) { bs.set(e); }
    constexpr auto expected = magic_enum::enum_values<Flags9>();
    std::size_t i = 0;
    for (Flags9 e : bs) {
      REQUIRE(i < expected.size());
      REQUIRE(e == expected[i]);
      ++i;
    }
    REQUIRE(i == expected.size());
    auto it = bs.end();
    for (std::size_t j = expected.size(); j > 0; --j) {
      --it; REQUIRE(*it == expected[j - 1]);
    }
  }

  SUBCASE("Flags9 - empty bitset") {
    magic_enum::containers::bitset<Flags9> bs;
    REQUIRE(bs.begin() == bs.end());
    REQUIRE(bs.find(Flags9::A) == bs.end());
    REQUIRE(bs.find(Flags9::I) == bs.end());
  }
}
