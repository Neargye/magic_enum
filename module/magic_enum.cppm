module;

#include <version>
#if __has_include(<fmt/format.h>)
#include <fmt/format.h>
#endif
#ifndef MAGIC_ENUM_USE_STD_MODULE
#if defined(__cpp_lib_format)
#include <format>
#endif

#define MAGIC_ENUM_INTERFACE_UNIT

#include <magic_enum/magic_enum.hpp>
#ifndef MAGIC_ENUM_USING_ALIAS_STRING
#include <magic_enum/magic_enum_containers.hpp>
#endif
#include <magic_enum/magic_enum_flags.hpp>
#include <magic_enum/magic_enum_format.hpp>
#include <magic_enum/magic_enum_fuse.hpp>
#include <magic_enum/magic_enum_iostream.hpp>
#include <magic_enum/magic_enum_switch.hpp>
#include <magic_enum/magic_enum_utility.hpp>

#endif

export module magic_enum;

#if defined(MAGIC_ENUM_USE_STD_MODULE)
import std;

extern "C++" {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 5244)
#endif

#include <magic_enum/magic_enum_all.hpp>

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif
}
#endif

export namespace magic_enum {
namespace customize {
    using customize::adl_info;
    using customize::customize_t;
    using customize::enum_name;
    using customize::enum_range;
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
    using containers::get;
    using containers::set;
    using containers::to_array;
}
#endif

    using magic_enum::is_magic_enum_supported;
    using magic_enum::Enum;
    using magic_enum::enum_name;
    using magic_enum::enum_cast;
    using magic_enum::enum_value;
    using magic_enum::enum_values;
    using magic_enum::enum_count;
    using magic_enum::enum_integer;
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
    using magic_enum::underlying_type;
    using magic_enum::underlying_type_t;

#ifdef MAGIC_ENUM_EXPORT_DETAIL
namespace detail {
    MAGIC_ENUM_EXPORT_DETAIL
}
#endif
}

#if defined(__cpp_lib_format)
export namespace std {
    using std::formatter;
}
#endif

#if defined(FMT_VERSION)
export namespace fmt {
    using fmt::formatter;
}
#endif
