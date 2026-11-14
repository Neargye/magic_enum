// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <array>
#include <cstdint>
#include <limits>
#include <string_view>
#include <type_traits>

namespace ide {

enum class Range {
  Last = 1023, First = -2, BeforeBlock = 509, AfterBlock = 510, Alias = 510,
  Hidden = 7, Renamed = 8, Below = -11, Above = 1032, Near = 1027
};
enum class Empty {};
enum Unfixed { Zero, One };
enum class Binary : bool { False, True };
enum class SignedFlags : std::int8_t { None = 0, A = 1, B = 2, AB = 3, High = -128, Alias = High };
enum class WideFlags : std::uint64_t { None = 0, A = 1, High = std::uint64_t{1} << 63, All = (std::numeric_limits<std::uint64_t>::max)() };

template <typename E>
struct data {
  inline static constexpr std::array<E, 0> values{};
  inline static constexpr std::array<const char*, 0> names{};
};

template <>
struct data<Range> {
  inline static constexpr std::array values{
    Range::Last, Range::First, Range::BeforeBlock, Range::AfterBlock, Range::Alias,
    Range::Hidden, Range::Renamed, Range::Below, Range::Above, Range::Near
  };
  inline static constexpr std::array names{
    "ide::Range::P_Last", "ide::Range::P_First", "ide::Range::P_BeforeBlock", "ide::Range::P_FirstAlias", "ide::Range::P_SecondAlias",
    "ide::Range::P_Hidden", "ide::Range::P_Renamed", "ide::Range::P_Below", "ide::Range::P_Above", "ide::Range::P_Near"
  };
};

template <>
struct data<Unfixed> {
  inline static constexpr std::array values{One, Zero};
  inline static constexpr std::array names{"ide::One", "ide::Zero"};
};

template <>
struct data<Binary> {
  inline static constexpr std::array values{Binary::True, Binary::False};
  inline static constexpr std::array names{"True", "False"};
};

template <>
struct data<SignedFlags> {
  inline static constexpr std::array values{SignedFlags::High, SignedFlags::Alias, SignedFlags::AB, SignedFlags::B, SignedFlags::A, SignedFlags::None};
  inline static constexpr std::array names{"High", "Alias", "AB", "B", "A", "None"};
};

template <>
struct data<WideFlags> {
  inline static constexpr std::array values{WideFlags::All, WideFlags::High, WideFlags::A, WideFlags::None};
  inline static constexpr std::array names{"All", "High", "A", "None"};
};

template <typename E>
constexpr const char* enumerator_name(E value) {
  static_assert(std::is_enum_v<E>);
  using U = std::underlying_type_t<E>;
  for (std::size_t i = 0; i < data<E>::values.size(); ++i) {
    if (static_cast<U>(data<E>::values[i]) == static_cast<U>(value)) {
      return data<E>::names[i];
    }
  }
  return nullptr;
}

template <typename E>
constexpr const char* type_name() { return "ide::Simulated"; }

} // namespace ide

// These stubs exercise the adapter, they do not validate the IDE's intrinsic implementation.
#define __rscpp_enumerator_name ide::enumerator_name
#define __rscpp_type_name ide::type_name
#define __RESHARPER__ 20250300
#define MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_flags.hpp>

#undef __RESHARPER__
#undef __rscpp_enumerator_name
#undef __rscpp_type_name

template <>
struct magic_enum::customize::enum_range<ide::Range> {
  static constexpr int min = -2;
  static constexpr int max = 1023;
  static constexpr std::size_t prefix_length = 2;
};

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<ide::Range>(ide::Range value) noexcept {
  if (value == ide::Range::Hidden || value == ide::Range::Near) {
    return invalid_tag;
  }
  if (value == ide::Range::Renamed || value == static_cast<ide::Range>(9)) {
    return "custom";
  }
  return default_tag;
}

template <>
struct magic_enum::customize::enum_range<ide::SignedFlags> {
  static constexpr bool is_flags = true;
};

template <>
struct magic_enum::customize::enum_range<ide::WideFlags> {
  static constexpr bool is_flags = true;
};

using namespace magic_enum;

static_assert(enum_type_name<ide::Range>() == "Simulated");
static_assert(enum_count<ide::Range>() == 5);
static_assert(enum_value<ide::Range, 0>() == ide::Range::First);
static_assert(enum_value<ide::Range, 1>() == ide::Range::Renamed);
static_assert(enum_value<ide::Range, 2>() == ide::Range::BeforeBlock);
static_assert(enum_value<ide::Range, 3>() == ide::Range::AfterBlock);
static_assert(enum_value<ide::Range, 4>() == ide::Range::Last);
static_assert(enum_name(ide::Range::BeforeBlock) == "BeforeBlock");
static_assert(enum_name(ide::Range::AfterBlock) == "FirstAlias");
static_assert(enum_name<ide::Range::Alias>() == "FirstAlias");
static_assert(detail::n<static_cast<ide::Range>(6)>().size_ == 0);
static_assert(enum_name(ide::Range::Renamed) == "custom");
static_assert(enum_name<ide::Range::Above>() == "Above");
static_assert(!enum_contains<ide::Range>(ide::Range::Below));
static_assert(!enum_contains<ide::Range>(ide::Range::Above));
static_assert(!enum_contains<ide::Range>(ide::Range::Hidden));
static_assert(!enum_contains<ide::Range>(9));
static_assert(enum_cast<ide::Range>("FirstAlias") == ide::Range::AfterBlock);
static_assert(!enum_cast<ide::Range>("SecondAlias"));
static_assert(!detail::named_probes<ide::Range, detail::range_probes<ide::Range, -2, 1023>>(std::make_index_sequence<8>{}));
static_assert(detail::named_probes<ide::Range, detail::range_probes<ide::Range, -2, 1022>>(std::make_index_sequence<8>{}));
static_assert(detail::has_named_multibit<ide::SignedFlags>());
static_assert(detail::has_named_multibit<ide::WideFlags>());
static_assert(!detail::has_named_multibit<ide::Unfixed>());
static_assert(!detail::has_named_multibit<ide::Binary>());
static_assert(enum_count<ide::Empty>() == 0);
static_assert(enum_count<ide::Unfixed>() == 2);
static_assert(enum_name(ide::Zero) == "Zero");
static_assert(enum_count<ide::Binary>() == 2);
static_assert(enum_value<ide::Binary, 0>() == ide::Binary::False);
static_assert(enum_value<ide::Binary, 1>() == ide::Binary::True);
static_assert(enum_count<ide::SignedFlags>() == 4);
static_assert(enum_value<ide::SignedFlags, 0>() == ide::SignedFlags::High);
static_assert(enum_value<ide::SignedFlags, 1>() == ide::SignedFlags::None);
static_assert(enum_name(ide::SignedFlags::High) == "High");
static_assert(enum_contains<ide::SignedFlags>(0));
static_assert(!enum_contains<ide::SignedFlags>(3));
static_assert(enum_flags_contains<ide::SignedFlags>(3));
static_assert(enum_count<ide::WideFlags>() == 3);
static_assert(enum_value<ide::WideFlags, 0>() == ide::WideFlags::None);
static_assert(enum_value<ide::WideFlags, 2>() == ide::WideFlags::High);
static_assert(enum_name(ide::WideFlags::High) == "High");

int main() {}
