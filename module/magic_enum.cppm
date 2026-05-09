module;

#include <version>

#if __has_include(<fmt/format.h>)
#include <fmt/format.h>
#endif

#ifndef MAGIC_ENUM_USE_STD_MODULE

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <type_traits>
#include <utility>

#if defined(__cpp_lib_format)
#include <format>
#endif

#endif // MAGIC_ENUM_USE_STD_MODULE

#define MAGIC_ENUM_INTERFACE_UNIT

export module magic_enum;

#if defined(MAGIC_ENUM_USE_STD_MODULE)
import std;
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 5244)
#endif

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
