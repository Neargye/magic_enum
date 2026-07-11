module;

#include <version>
#ifdef MAGIC_ENUM_MODULE_WITH_FMT
#  include <fmt/format.h>
#endif

#ifndef MAGIC_ENUM_USE_STD_MODULE

#  include <compare>
#  include <cstdlib>
#  if __has_include(<format>)
#    include <format>
#  endif
#  include <stdexcept>
#  if __has_include(<bit>)
#    include <bit>
#  endif
#  if defined(_MSC_VER) && !defined(__clang__)
#    include <intrin.h>
#  endif

#  include <magic_enum/magic_enum.hpp>
#  include <magic_enum/magic_enum_flags.hpp>
#  include <magic_enum/magic_enum_fuse.hpp>
#  include <magic_enum/magic_enum_iostream.hpp>
#  include <magic_enum/magic_enum_utility.hpp>

#else // MAGIC_ENUM_USE_STD_MODULE

// MAGIC_ENUM_CONFIG_FILE may include STL headers. Keep them in the global module
// fragment to avoid conflicting declarations with import std.
#  ifdef MAGIC_ENUM_CONFIG_FILE
#    include MAGIC_ENUM_CONFIG_FILE
#  endif // MAGIC_ENUM_CONFIG_FILE

#  if !defined(MAGIC_ENUM_NO_ASSERT) && !defined(MAGIC_ENUM_ASSERT)
#    include <cassert>
#  endif

#endif // MAGIC_ENUM_USE_STD_MODULE

export module magic_enum;

#ifdef MAGIC_ENUM_USE_STD_MODULE
import std;

extern "C++" {
#  if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"
#  elif defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable : 5244)
#  endif

#  include <magic_enum/magic_enum.hpp>
#  ifndef MAGIC_ENUM_USING_ALIAS_STRING
#    include <magic_enum/magic_enum_containers.hpp>
#  endif
#  include <magic_enum/magic_enum_flags.hpp>
#  include <magic_enum/magic_enum_format.hpp>
#  include <magic_enum/magic_enum_fuse.hpp>
#  include <magic_enum/magic_enum_iostream.hpp>
#  include <magic_enum/magic_enum_switch.hpp>
#  include <magic_enum/magic_enum_utility.hpp>

#  if defined(__clang__)
#    pragma clang diagnostic pop
#  elif defined(_MSC_VER)
#    pragma warning(pop)
#  endif
}
#else
extern "C++" {
#  if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"
#  elif defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable : 5244)
#  endif

// Keep standard-library specializations attached to the named module. Exporting
// the primary template does not make specializations from the global module
// fragment reachable to importers.
#  ifndef MAGIC_ENUM_USING_ALIAS_STRING
#    include <magic_enum/magic_enum_containers.hpp>
#  endif
#  include <magic_enum/magic_enum_format.hpp>
#  include <magic_enum/magic_enum_switch.hpp>

#  if defined(__clang__)
#    pragma clang diagnostic pop
#  elif defined(_MSC_VER)
#    pragma warning(pop)
#  endif
}
#endif

export namespace magic_enum {
namespace customize {
  using customize::adl_info_holder;
  using customize::adl_info;
  using customize::customize_t;
  using customize::enum_name;
  using customize::enum_range;
  using customize::enum_type_name;
  using customize::default_tag;
  using customize::invalid_tag;
}

namespace istream_operators {
  using istream_operators::operator>>;
}

namespace ostream_operators {
  using ostream_operators::operator<<;
}

namespace iostream_operators {
  using iostream_operators::operator<<;
  using iostream_operators::operator>>;
}

namespace bitwise_operators {
  using bitwise_operators::operator~;
  using bitwise_operators::operator|;
  using bitwise_operators::operator&;
  using bitwise_operators::operator^;
  using bitwise_operators::operator|=;
  using bitwise_operators::operator&=;
  using bitwise_operators::operator^=;
}

#ifndef MAGIC_ENUM_USING_ALIAS_STRING
namespace containers {
  using containers::array;
  using containers::bitset;
  using containers::comparator_indexing;
  using containers::default_indexing;
  using containers::get;
  using containers::make_array;
  using containers::name_greater;
  using containers::name_greater_case_insensitive;
  using containers::name_less;
  using containers::name_less_case_insensitive;
  using containers::raw_access;
  using containers::set;
  using containers::to_array;
}
#endif

using magic_enum::optional;
using magic_enum::string_view;
using magic_enum::string;
using magic_enum::char_type;
using magic_enum::is_magic_enum_supported;
using magic_enum::Enum;
using magic_enum::enum_name;
using magic_enum::enum_cast;
using magic_enum::enum_value;
using magic_enum::enum_values;
using magic_enum::enum_count;
using magic_enum::enum_integer;
using magic_enum::enum_underlying;
using magic_enum::enum_names;
using magic_enum::enum_entries;
using magic_enum::enum_fuse;
using magic_enum::enum_switch;
using magic_enum::enum_for_each;
using magic_enum::enum_contains;
using magic_enum::enum_index;
using magic_enum::enum_prev_value;
using magic_enum::enum_prev_value_circular;
using magic_enum::enum_next_value;
using magic_enum::enum_next_value_circular;
using magic_enum::enum_constant;
using magic_enum::enum_flags_name;
using magic_enum::enum_flags_contains;
using magic_enum::enum_flags_test;
using magic_enum::enum_flags_test_any;
using magic_enum::enum_flags_cast;
using magic_enum::enum_type_name;
using magic_enum::enum_reflected;
using magic_enum::as_flags;
using magic_enum::as_common;
using magic_enum::case_insensitive;
using magic_enum::is_unscoped_enum;
using magic_enum::is_unscoped_enum_v;
using magic_enum::is_scoped_enum;
using magic_enum::is_scoped_enum_v;
using magic_enum::is_flags_enum;
using magic_enum::is_flags_v;
using magic_enum::underlying_type;
using magic_enum::underlying_type_t;

#ifdef MAGIC_ENUM_EXPORT_DETAIL
namespace detail {
  MAGIC_ENUM_EXPORT_DETAIL
}
#endif
}

#if defined(__cpp_lib_three_way_comparison) && __cpp_lib_three_way_comparison >= 201907L
export namespace std {
  using std::partial_ordering;
  using std::strong_ordering;
  using std::weak_ordering;
}
#endif

#ifndef MAGIC_ENUM_USING_ALIAS_STRING
export namespace std {
  using std::hash;
}
#endif

#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
export namespace std {
  using std::formatter;
}
#endif

#ifdef MAGIC_ENUM_MODULE_WITH_FMT
export namespace fmt {
  using fmt::formatter;
}
#endif
