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
#include <string>

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
enum class WideRange : std::int16_t { First = -8192, Middle = 0, Last = 8191 };

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

// The value has no name, so the custom name does not add it.
static_assert(magic_enum::enum_name(UnfixedCustom::OneCustom) == "OneCustom");
static_assert(magic_enum::enum_count<UnfixedCustom>() == 1);
static_assert(!magic_enum::enum_cast<UnfixedCustom>("Outside").has_value());
#endif

// Clang before 20 scans and probes this enum with n() per value.
enum UnfixedProbe { UnfixedProbeZero = 0, UnfixedProbeHidden = 8 };

template <>
struct magic_enum::customize::enum_range<UnfixedProbe> {
  static constexpr int min = 0;
  static constexpr int max = 0;
};

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<UnfixedProbe>(UnfixedProbe value) noexcept {
  return value == UnfixedProbeHidden ? invalid_tag : default_tag;
}

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

// Each block of the scan is evaluated separately, so a wide range stays within the default constexpr step limits.
template <>
struct magic_enum::customize::enum_range<WideRange> {
  static constexpr int min = -8192;
  static constexpr int max = 8191;
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

// Undeclared custom names and hidden values do not trigger range probes.
enum class ProbeCustom { Zero = 0, Renamed = 1, Hidden = 5 };

template <>
struct magic_enum::customize::enum_range<ProbeCustom> {
  static constexpr int min = 0;
  static constexpr int max = 1;
};

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<ProbeCustom>(ProbeCustom value) noexcept {
  switch (static_cast<int>(value)) {
    case -8: return "BelowEight";
    case -1: return "BelowOne";
    case 1: return "One";
    case 2: return "AboveOne";
    case 5: return invalid_tag;
    case 9: return "AboveEight";
    default: return default_tag;
  }
}

// Excluded values must not instantiate compiler-name lookup.
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
// Customization renames or hides declared values, but cannot add one.
static_assert(magic_enum::enum_count<Synthetic>() == 1);
static_assert(magic_enum::enum_name(Synthetic{1}).empty());
static_assert(!magic_enum::enum_cast<Synthetic>("Named").has_value());
static_assert(magic_enum::enum_count<ProbeCustom>() == 2);
static_assert(magic_enum::enum_name(ProbeCustom::Renamed) == "One");
static_assert(magic_enum::enum_count<UnfixedProbe>() == 1);
static_assert(magic_enum::enum_count<WideBounds>() == 2);
static_assert(magic_enum::enum_count<WideRange>() == 3);
static_assert(magic_enum::enum_name(WideRange::Last) == "Last");

// Regressions for the signature parser of nb().
namespace parser {

// Commas inside template arguments of the qualifier do not separate values.
template <typename T, int N>
struct Outer;

template <>
struct Outer<std::int8_t, 1> {
  enum class Nested : std::int8_t { Low = -3, High = 2 };
};

namespace {
enum class Anonymous { First = 1, Third = 3 };
} // namespace

enum class Long {
  ThisEnumeratorNameIsLongEnoughToCrossAnyFixedSizeBufferThatTheSignatureParserCouldUseForOneValueInTheBlock = 1,
  Short = 2,
};

// Values next to the edges of 256 and 512 value blocks.
enum class Boundaries : std::uint16_t { B0 = 0, B254 = 254, B255 = 255, B256 = 256, B257 = 257, B510 = 510, B511 = 511, B512 = 512, B513 = 513, B1023 = 1023 };

} // namespace parser

using NestedParse = parser::Outer<std::int8_t, 1>::Nested;

template <>
struct magic_enum::customize::enum_range<NestedParse> {
  static constexpr int min = -4;
  static constexpr int max = 4;
};

template <>
struct magic_enum::customize::enum_range<parser::Anonymous> {
  static constexpr int min = 0;
  static constexpr int max = 4;
};

template <>
struct magic_enum::customize::enum_range<parser::Long> {
  static constexpr int min = 0;
  static constexpr int max = 2;
};

template <>
struct magic_enum::customize::enum_range<parser::Boundaries> {
  static constexpr int min = 0;
  static constexpr int max = 1023;
};

TEST_CASE("signature parser regressions") {
  static_assert(magic_enum::enum_count<NestedParse>() == 2);
  static_assert(magic_enum::enum_name(NestedParse::Low) == "Low");
  static_assert(magic_enum::enum_name(NestedParse::High) == "High");
  static_assert(magic_enum::enum_count<parser::Anonymous>() == 2);
  static_assert(magic_enum::enum_name(parser::Anonymous::Third) == "Third");
  static_assert(magic_enum::enum_count<parser::Long>() == 2);
  static_assert(magic_enum::enum_name(parser::Long::Short) == "Short");
  static_assert(magic_enum::enum_name(parser::Long::ThisEnumeratorNameIsLongEnoughToCrossAnyFixedSizeBufferThatTheSignatureParserCouldUseForOneValueInTheBlock).size() == 106);

  constexpr auto boundaries = magic_enum::enum_entries<parser::Boundaries>();
  static_assert(boundaries.size() == 10);
  for (const auto& [value, name] : boundaries) {
    CHECK(name.substr(1) == std::to_string(static_cast<int>(value)));
  }

  // The qualifier of a local enum holds the name of the enclosing function.
  enum class LocalParse { Zero };
  static_assert(magic_enum::enum_name(LocalParse::Zero) == "Zero");
  const auto lambda_name = [] {
    enum class LambdaParse { Zero };
    return magic_enum::enum_name(LambdaParse::Zero);
  };
  CHECK(lambda_name() == "Zero");
}


enum class FallbackNames { Default, Renamed, Hidden };

template <>
struct magic_enum::customize::enum_range<FallbackNames> {
  static constexpr int min = 0;
  static constexpr int max = 3;
};

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<FallbackNames>(FallbackNames value) noexcept {
  switch (value) {
    case FallbackNames::Default: return default_tag;
    case FallbackNames::Renamed: return "Changed";
    case FallbackNames::Hidden: return invalid_tag;
    default: return "Synthetic";
  }
}

// Exercise the per-value scanner on every compiler.
template <>
constexpr bool magic_enum::detail::batch_supported<FallbackNames>() noexcept {
  return false;
}

TEST_CASE("fallback scanner customization") {
  constexpr auto entries = magic_enum::enum_entries<FallbackNames>();
  static_assert(entries.size() == 2);
  static_assert(entries[0].first == FallbackNames::Default && entries[0].second == "Default");
  static_assert(entries[1].first == FallbackNames::Renamed && entries[1].second == "Changed");
  static_assert(!magic_enum::enum_contains(FallbackNames::Hidden));
  static_assert(!magic_enum::enum_contains<FallbackNames>(3));
}
