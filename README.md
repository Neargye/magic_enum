[![Github releases](https://img.shields.io/github/release/Neargye/magic_enum.svg)](https://github.com/Neargye/magic_enum/releases)
[![Conan package](https://img.shields.io/badge/Conan-package-blueviolet)](https://conan.io/center/recipes/magic_enum)
[![Vcpkg package](https://img.shields.io/badge/Vcpkg-package-blueviolet)](https://github.com/microsoft/vcpkg/tree/master/ports/magic-enum)
[![Build2 package](https://img.shields.io/badge/Build2-package-blueviolet)](https://www.cppget.org/magic_enum?q=magic_enum)
[![Meson wrap](https://img.shields.io/badge/Meson-wrap-blueviolet)](https://github.com/mesonbuild/wrapdb/blob/master/subprojects/magic_enum.wrap)
[![License](https://img.shields.io/github/license/Neargye/magic_enum.svg)](LICENSE)
[![Compiler explorer](https://img.shields.io/badge/compiler_explorer-online-blue.svg)](https://godbolt.org/z/feqcPa5G6)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/Neargye/magic_enum/badge)](https://securityscorecards.dev/viewer/?uri=github.com/Neargye/magic_enum)

# Magic Enum C++

`magic_enum` is a header-only C++17 library for static enum reflection. It converts enum values to and from strings, provides compile-time enum sequences, and supports flags, formatting, and enum-indexed containers without macros or boilerplate.

## Documentation

* [Reference](doc/reference.md)
* [Limitations](doc/limitations.md)
* [Examples](example/)
* [Integration](#integration)

## Quick start

```cpp
#include <magic_enum/magic_enum.hpp>
#include <iostream>

enum class Color { RED = -10, BLUE = 0, GREEN = 10 };

int main() {
  Color color = Color::RED;
  std::cout << magic_enum::enum_name(color) << '\n'; // RED

  if (auto green = magic_enum::enum_cast<Color>("GREEN")) {
    color = *green;
  }

  constexpr auto values = magic_enum::enum_values<Color>();
  static_assert(values[2] == Color::GREEN);
}
```

## API

* Core reflection ([`magic_enum.hpp`](include/magic_enum/magic_enum.hpp)): `enum_name`, `enum_cast`, `enum_value`, `enum_values`, `enum_count`, `enum_integer`, `enum_underlying`, `enum_names`, `enum_entries`, `enum_contains`, and `enum_index`.
* Type and range API ([`magic_enum.hpp`](include/magic_enum/magic_enum.hpp)): `enum_type_name`, `enum_reflected`, `case_insensitive`, `as_flags`, and `as_common`.
* Traits ([`magic_enum.hpp`](include/magic_enum/magic_enum.hpp)): `is_magic_enum_supported`, `is_unscoped_enum`, `is_unscoped_enum_v`, `is_scoped_enum`, `is_scoped_enum_v`, `is_flags_enum`, `is_flags_v`, `underlying_type`, and `underlying_type_t`.
* Helpers ([`magic_enum.hpp`](include/magic_enum/magic_enum.hpp)): `Enum`, `enum_constant`, and operators from `bitwise_operators`.
* Customization ([`magic_enum.hpp`](include/magic_enum/magic_enum.hpp)): `customize::enum_range`, `customize::adl_info`, `customize::enum_name`, `customize::enum_type_name`, `customize_t`, `default_tag`, and `invalid_tag`.
* Flags ([`magic_enum_flags.hpp`](include/magic_enum/magic_enum_flags.hpp)): `enum_flags_name`, `enum_flags_cast`, `enum_flags_contains`, `enum_flags_test`, and `enum_flags_test_any`.
* Enum utilities ([`magic_enum_utility.hpp`](include/magic_enum/magic_enum_utility.hpp)): `enum_for_each`, `enum_next_value`, `enum_prev_value`, `enum_next_value_circular`, and `enum_prev_value_circular`.
* Runtime dispatch ([`magic_enum_switch.hpp`](include/magic_enum/magic_enum_switch.hpp)): `enum_switch`.
* Enum fusion ([`magic_enum_fuse.hpp`](include/magic_enum/magic_enum_fuse.hpp)): `enum_fuse`.
* Streams ([`magic_enum_iostream.hpp`](include/magic_enum/magic_enum_iostream.hpp)): operators from `istream_operators`, `ostream_operators`, and `iostream_operators`.
* Formatting ([`magic_enum_format.hpp`](include/magic_enum/magic_enum_format.hpp)): `std::format` and `{fmt}` formatter support.
* Containers ([`magic_enum_containers.hpp`](include/magic_enum/magic_enum_containers.hpp)): `containers::array`, `containers::bitset`, `containers::set`, `get`, `make_array`, `to_array`, and `raw_access`.
* Container customization ([`magic_enum_containers.hpp`](include/magic_enum/magic_enum_containers.hpp)): `default_indexing`, `comparator_indexing`, `name_less`, `name_greater`, `name_less_case_insensitive`, and `name_greater_case_insensitive`.
* All headers ([`magic_enum_all.hpp`](include/magic_enum/magic_enum_all.hpp)): includes all public headers.

See [reference](doc/reference.md) for overloads, constraints, and customization details.

## Flags

```cpp
#include <magic_enum/magic_enum_flags.hpp>

enum class Directions { None = 0, Left = 1, Down = 2, Up = 4, Right = 8 };

template <>
struct magic_enum::customize::enum_range<Directions> {
  static constexpr bool is_flags = true;
};

using namespace magic_enum::bitwise_operators; // Imports operators for all enums; keep scope narrow.

auto directions = Directions::Up | Directions::Right;
auto name = magic_enum::enum_flags_name(directions); // "Up|Right"
auto value = magic_enum::enum_flags_cast<Directions>("Left|Down");
```

## Utilities

```cpp
#include <magic_enum/magic_enum_utility.hpp>

void use_color() {
  auto next = magic_enum::enum_next_value(Color::RED); // Color::BLUE
  auto wrapped = magic_enum::enum_next_value_circular(Color::GREEN); // Color::RED

  magic_enum::enum_for_each<Color>([](auto color) {
    constexpr Color value = color;
  });
}
```

## Formatting and streams

```cpp
#include <format>
#include <magic_enum/magic_enum_format.hpp>

auto text = std::format("{}", Color::RED); // "RED"
```

Include `{fmt}` before `magic_enum_format.hpp` to enable `{fmt}` formatter support. For streams, include `magic_enum_iostream.hpp` and import only required operator namespace.

## Containers

```cpp
#include <magic_enum/magic_enum_containers.hpp>

void use_containers() {
  magic_enum::containers::array<Color, int> priority{};
  priority[Color::GREEN] = 1;

  magic_enum::containers::bitset<Color> selected;
  selected.set(Color::BLUE);

  magic_enum::containers::set<Color> colors{Color::RED, Color::GREEN};
}
```

## Remarks

* Enum value aliases are not supported.
* Reflection scans configured enum range and is designed for small enums. Read [limitations](doc/limitations.md) before use.

## Integration

* Copy required headers from [`include/magic_enum`](include/magic_enum) or use [release archive](https://github.com/Neargye/magic_enum/releases/latest).
* Use CMake with `add_subdirectory` or `find_package(magic_enum CONFIG REQUIRED)`, then link `magic_enum::magic_enum`.
* Use [vcpkg](https://github.com/microsoft/vcpkg/tree/master/ports/magic-enum), [Conan](https://conan.io/center/recipes/magic_enum), [Build2](https://cppget.org/magic_enum?q=magic_enum), or [Meson WrapDB](https://github.com/mesonbuild/wrapdb/blob/master/subprojects/magic_enum.wrap).
* Fetch sources with CMake [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html) or [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake). Release tags use `vx.y.z` format.
* Use Bazel with `MODULE.bazel` or `http_archive`; target is `@magic_enum//:magic_enum`.
* Use ROS with `<depend>magic_enum</depend>` in `package.xml`, then link `magic_enum::magic_enum`.
* Use C++20 module with CMake 3.28+: configure with `MAGIC_ENUM_USE_MODULES=ON`, link `magic_enum::magic_enum`, and write `import magic_enum;`. Do not mix module import and `magic_enum` headers in same link unit.

## Compiler compatibility

* Clang/LLVM >= 5
* MSVC++ >= 15.3 / Visual Studio >= 2017
* Xcode >= 10
* GCC >= 9

## Licensed under the [MIT License](LICENSE)

Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.
