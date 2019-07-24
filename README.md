```text
 __  __             _        ______                          _____
|  \/  |           (_)      |  ____|                        / ____|_     _
| \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
| |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
| |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
|_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
               __/ |
              |___/
```

[![Github Releases](https://img.shields.io/github/release/Neargye/magic_enum.svg)](https://github.com/Neargye/magic_enum/releases)
[![License](https://img.shields.io/github/license/Neargye/magic_enum.svg)](LICENSE)
[![Build Status](https://travis-ci.org/Neargye/magic_enum.svg?branch=master)](https://travis-ci.org/Neargye/magic_enum)
[![Build status](https://ci.appveyor.com/api/projects/status/0rpr966p9ssrvwu3/branch/master?svg=true)](https://ci.appveyor.com/project/Neargye/magic-enum-hf8vk/branch/master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/64d04f150af14c3e8bd1090057b68538)](https://www.codacy.com/app/Neargye/magic_enum?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Neargye/magic_enum&amp;utm_campaign=Badge_Grade)
[![Try online](https://img.shields.io/badge/try-online-blue.svg)](https://wandbox.org/permlink/EzMNDcSTYFqAcQfj)

# Magic Enum C++

Header-only C++17 library provides static reflection for enums, work with any enum type without any macro or boilerplate code.
* `enum_cast` obtains enum value from string or integer.
* `enum_value` returns enum value at specified index.
* `enum_values` obtains enum value sequence.
* `enum_count` returns number of enum values.
* `enum_integer` obtains integer value from enum value.
* `enum_name` returns string name from enum value.
* `enum_names` obtains string enum name sequence.
* `enum_entries` obtains pair (value enum, string enum name) sequence.
* `is_unscoped_enum` checks whether type is an [Unscoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Unscoped_enumeration).
* `is_scoped_enum` checks whether type is an [Scoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations).
* `is_fixed_enum` checks whether type is an [Fixed enumeration](https://en.cppreference.com/w/cpp/language/enum).
* `underlying_type` port of C++20 improved UB-free "SFINAE-friendly" [std::underlying_type](https://en.cppreference.com/w/cpp/types/underlying_type).

## Features

* C++17
* Header-only
* Dependency-free
* Compile-time
* Enum to string
* String to enum
* Iterating over enum

## [Examples](example/example.cpp)

```cpp
// For example color enum.
enum Color { RED = 2, BLUE = 4, GREEN = 8 };
```

* Enum value to string
  ```cpp
  Color color = Color::RED;
  auto color_name = magic_enum::enum_name(color);
  // color_name -> "RED"
  ```

* String to enum value
  ```cpp
  std::string color_name{"GREEN"};
  auto color = magic_enum::enum_cast<Color>(color_name);
  if (color.has_value()) {
    // color.value() -> Color::GREEN
  }
  ```

* Integer to enum value
  ```cpp
  int color_integer = 2;
  auto color = magic_enum::enum_cast<Color>(color_integer);
  if (colo.has_value()) {
    // color.value() -> Color::RED
  }
  ```

* Indexed access to enum value
  ```cpp
  int i = 1;
  Color colo = magic_enum::enum_value<Color>(i);
  // color -> Color::BLUE
  ```

* Enum value sequence
  ```cpp
  constexpr auto colors = magic_enum::enum_values<Color>();
  // colors -> {Color::RED, Color::BLUE, Color::GREEN}
  // colors[0] -> Color::RED
  ```

* Number of enum elements
  ```cpp
  constexpr std::size_t color_count = magic_enum::enum_count<Color>();
  // color_count -> 3
  ```

* Enum value to integer
  ```cpp
  Color color = Color::RED;
  auto color_integer = magic_enum::enum_integer(color);
  // color -> 2
  ```

* Enum names sequence
  ```cpp
  constexpr auto color_names = magic_enum::enum_names<Color>();
  // color_names -> {"RED", "BLUE", "GREEN"}
  // color_names[0] -> "RED"
  ```

* Enum entries sequence
  ```cpp
  constexpr auto color_entries = magic_enum::enum_entries<Color>();
  // color_entries -> {{Color::RED, "RED"}, {Color::BLUE, "BLUE"}, {Color::GREEN, "GREEN"}}
  // color_entries[0].first -> Color::RED
  // color_entries[0].second -> "RED"
  ```

* Stream operator for enum
  ```cpp
  using namespace magic_enum::ostream_operators; // out-of-the-box ostream operators for enums.
  Color color = Color::BLUE;
  std::cout << color << std::endl; // "BLUE"
  ```

* Checks whether type is an [Unscoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Unscoped_enumeration).
  ```cpp
  enum color { red, green, blue };
  enum class direction { left, right };

  magic_enum::is_unscoped_enum<color>::value -> true
  magic_enum::is_unscoped_enum<direction>::value -> false
  magic_enum::is_unscoped_enum<int>::value -> false

  // Helper variable template.
  magic_enum::is_unscoped_enum_v<color> -> true
  ```

* Checks whether type is an [Scoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations).
  ```cpp
  enum color { red, green, blue };
  enum class direction { left, right };

  magic_enum::is_scoped_enum<color>::value -> false
  magic_enum::is_scoped_enum<direction>::value -> true
  magic_enum::is_scoped_enum<int>::value -> false

  // Helper variable template.
  magic_enum::is_scoped_enum_v<direction> -> true
  ```

* Static storage enum variable to string
  ```cpp
  constexpr Color color = Color::BLUE;
  constexpr auto color_name = magic_enum::enum_name<color>();
  // color_name -> "BLUE"
  ```

## Remarks

* `magic_enum::enum_cast(value)` returns `std::optional<E>`, using `has_value()` to check contains enum value and `value()` to get the enum value.

* `magic_enum::enum_value(index)` no bounds checking is performed: the behavior is undefined if `index >= number of enum values`.

* `magic_enum::enum_values<E>()` returns `std::array<E, N>` with all enum value where `N = number of enum values`, sorted by enum value.

* `magic_enum::enum_name(value)` returns `std::string_view`. If enum value does not have name, returns empty string.

* `magic_enum::enum_name<value>()` is much lighter on the compile times and is not restricted to the enum_range limitation.

* `magic_enum::enum_names<E>()` returns `std::array<std::string_view, N>` with all string enum name where `N = number of enum values`, sorted by enum value.

* `magic_enum::enum_entries<E>()` returns `std::array<std::pair<E, std::string_view>, N>` with all std::pair (value enum, string enum name) where `N = number of enum values`, sorted by enum value.

* Enum value must be in range `[MAGIC_ENUM_RANGE_MIN, MAGIC_ENUM_RANGE_MAX]`. By default `MAGIC_ENUM_RANGE_MIN = -128`, `MAGIC_ENUM_RANGE_MAX = 128`.

  If need another range for all enum types by default, redefine the macro `MAGIC_ENUM_RANGE_MIN` and `MAGIC_ENUM_RANGE_MAX`.
  ```cpp
  #define MAGIC_ENUM_RANGE_MIN 0
  #define MAGIC_ENUM_RANGE_MAX 256
  #include <magic_enum.hpp>
  ```

  If need another range for specific enum type, add specialization `enum_range` for necessary enum type.
  ```cpp
  #include <magic_enum.hpp>

  enum number { one = 100, two = 200, three = 300 };

  namespace magic_enum {
  template <>
  struct enum_range<number> {
    static constexpr int min = 100;
    static constexpr int max = 300;
  };
  }
  ```

* `magic_enum` obtains the first defined value enums, and won't work if value are aliased.
  ```cpp
  enum ShapeKind {
    ConvexBegin = 0,
    Box = 0, // Won't work.
    Sphere = 1,
    ConvexEnd = 2,
    Donut = 2, // Won't work too.
    Banana = 3,
    COUNT = 4,
  };
  // magic_enum::enum_cast<ShapeKind>("Box") -> std::nullopt
  // magic_enum::enum_name(ShapeKind::Box) -> "ConvexBegin"
  ```
  Work around the issue:
  ```cpp
  enum ShapeKind {
    // Convex shapes, see ConvexBegin and ConvexEnd below.
    Box = 0,
    Sphere = 1,

    // Non-convex shapes.
    Donut = 2,
    Banana = 3,

    COUNT = Banana + 1,

    // Non-reflected aliases.
    ConvexBegin = Box,
    ConvexEnd = Sphere + 1,
  };
  // magic_enum::enum_cast<ShapeKind>("Box") -> ShapeKind::Box
  // magic_enum::enum_name(ShapeKind::Box) -> "Box"

  // Non-reflected aliases.
  // magic_enum::enum_cast<ShapeKind>("ConvexBegin") -> std::nullopt
  // magic_enum::enum_name(ShapeKind::ConvexBegin) -> "Box"
  ```

## Integration

You should add the required file [magic_enum.hpp](include/magic_enum.hpp).

## Compiler compatibility

* Clang/LLVM >= 5
* Visual C++ >= 15.3 / Visual Studio >= 2017
* Xcode >= 10.2
* GCC >= 9

## Licensed under the [MIT License](LICENSE)
