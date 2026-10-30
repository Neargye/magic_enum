// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 0
#define MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION
#include <magic_enum/magic_enum.hpp>

#include <cstdint>
#include <limits>

enum class Single { value };

TEST_CASE("single-value reflection range") {
  static_assert(magic_enum::enum_count<Single>() == 1);
  static_assert(magic_enum::enum_value<Single, 0>() == Single::value);
}

enum class FullByte : std::uint8_t { Zero = 0, Last = 255 };
enum class HighByte : std::uint8_t { Sentinel = 0, First = 248, Last = 255 };
enum class LowSignedByte : std::int8_t { First = -128, Last = -127, Sentinel = 127 };
enum class Binary : bool { Zero, One };
enum Unfixed { Zero, One };
enum class NearIntMin : std::int64_t {
  First = (std::numeric_limits<int>::min)(),
  Last = static_cast<std::int64_t>((std::numeric_limits<int>::min)()) + 1,
};
enum class NearIntMax : std::uint64_t {
  First = (std::numeric_limits<int>::max)() - 1,
  Last = (std::numeric_limits<int>::max)(),
};
enum class WideUnsigned : std::uint64_t { First = 1, Sentinel = (std::numeric_limits<std::uint64_t>::max)() };
enum class Ignored { Keep = 0, Drop = 1 };
enum class Expanded { First = 5, Last = 133 };
enum class Flags : std::uint64_t { First = 1, OutsideCommonRange = 128, Last = std::uint64_t{1} << 63 };
enum class Unprobed { Keep = 0, Gap = 3, Far = 1000 };
enum class Synthetic { Declared = 0 };
enum class WideBounds : std::int8_t { First = -128, Last = 127 };

template <>
struct magic_enum::customize::enum_range<FullByte> {
  static constexpr int min = 0;
  static constexpr int max = 255;
};

template <>
struct magic_enum::customize::enum_range<HighByte> {
  static constexpr int min = 248;
  static constexpr int max = 255;
};

template <>
struct magic_enum::customize::enum_range<LowSignedByte> {
  static constexpr int min = -128;
  static constexpr int max = -127;
};

template <>
struct magic_enum::customize::enum_range<Binary> {
  static constexpr int min = 0;
  static constexpr int max = 1;
};

template <>
struct magic_enum::customize::enum_range<Unfixed> {
  static constexpr int min = 0;
  static constexpr int max = 1;
};

#if defined(__clang__) && __clang_major__ >= 16
enum UnfixedCustom { OneCustom = 1 };

template <>
struct magic_enum::customize::enum_range<UnfixedCustom> {
  static constexpr int min = 0;
  static constexpr int max = 3;
};

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<UnfixedCustom>(UnfixedCustom value) noexcept {
  return static_cast<int>(value) == 3 ? customize_t{"Outside"} : default_tag;
}

// Some Clang versions reject the extra value, but declared values must remain usable.
static_assert(magic_enum::enum_name(UnfixedCustom::OneCustom) == "OneCustom");
static_assert(magic_enum::enum_cast<UnfixedCustom>("Outside") ==
              magic_enum::enum_cast<UnfixedCustom>(3));
#endif

template <>
struct magic_enum::customize::enum_range<NearIntMin> {
  static constexpr int min = (std::numeric_limits<int>::min)();
  static constexpr int max = (std::numeric_limits<int>::min)() + 1;
};

template <>
struct magic_enum::customize::enum_range<NearIntMax> {
  static constexpr int min = (std::numeric_limits<int>::max)() - 1;
  static constexpr int max = (std::numeric_limits<int>::max)();
};

template <>
struct magic_enum::customize::enum_range<WideUnsigned> {
  static constexpr int min = 0;
  static constexpr int max = 127;
};

template <>
struct magic_enum::customize::enum_range<Ignored> {
  static constexpr int min = 0;
  static constexpr int max = 1;
};

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<Ignored>(Ignored value) noexcept {
  return value == Ignored::Drop ? invalid_tag : default_tag;
}

template <>
struct magic_enum::customize::enum_range<Expanded> {
  static constexpr int min = 5;
  static constexpr int max = 133;
};

template <>
struct magic_enum::customize::enum_range<Flags> {
  static constexpr bool is_flags = true;
  static constexpr int min = 0;
  static constexpr int max = 0;
};

template <>
struct magic_enum::customize::enum_range<Unprobed> {
  static constexpr int min = 0;
  static constexpr int max = 0;
};

// Bounds wider than int are clamped to the underlying type before the int check.
template <>
struct magic_enum::customize::enum_range<WideBounds> {
  static constexpr long long min = (std::numeric_limits<long long>::min)();
  static constexpr unsigned long long max = (std::numeric_limits<unsigned long long>::max)();
};

template <>
struct magic_enum::customize::enum_range<Synthetic> {
  static constexpr int min = 0;
  static constexpr int max = 1;
};

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<Synthetic>(Synthetic value) noexcept {
  return static_cast<int>(value) == 1 ? customize_t{"Named"} : default_tag;
}

// Custom and excluded values must not instantiate compiler-name lookup.
template <>
MAGIC_ENUM_CONSTEVAL auto magic_enum::detail::n<static_cast<Synthetic>(1)>() noexcept {}
template <>
MAGIC_ENUM_CONSTEVAL auto magic_enum::detail::n<Ignored::Drop>() noexcept {}

static_assert(magic_enum::enum_count<FullByte>() == 2);
static_assert(magic_enum::enum_count<HighByte>() == 2);
static_assert(magic_enum::enum_count<LowSignedByte>() == 2);
static_assert(magic_enum::enum_count<Binary>() == 2);
static_assert(magic_enum::enum_count<Unfixed>() == 2);
static_assert(magic_enum::enum_count<NearIntMin>() == 2);
static_assert(magic_enum::enum_count<NearIntMax>() == 2);
static_assert(magic_enum::enum_count<WideUnsigned>() == 1);
static_assert(magic_enum::enum_count<Ignored>() == 1);
static_assert(magic_enum::enum_count<Expanded>() == 2);
static_assert(magic_enum::enum_contains(Expanded::Last));
static_assert(magic_enum::enum_count<Flags>() == 3);
// Values outside the range are not reflected.
static_assert(magic_enum::enum_count<Unprobed>() == 1);
static_assert(!magic_enum::enum_contains(Unprobed::Gap));
static_assert(!magic_enum::enum_contains(Unprobed::Far));
// A custom name makes an undeclared value reflected, the compiler name of that value is empty.
static_assert(magic_enum::enum_count<Synthetic>() == 2);
static_assert(magic_enum::enum_count<WideBounds>() == 2);
static_assert(magic_enum::enum_name(Synthetic{1}) == "Named");
static_assert(magic_enum::enum_cast<Synthetic>("Named") == Synthetic{1});
