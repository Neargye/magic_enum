# Magic Enum C++

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
[![Try online](https://img.shields.io/badge/try-online-blue.svg)](https://wandbox.org/permlink/uCF1Op6ZgSI6cDJS)

## What is Magic Enum?

Header-only C++17 library provides Enum-to-String and String-to-Enum functions.
* `magic_enum::enum_cast` obtains enum value from string or integer.
* `magic_enum::enum_value` obtains indexed access to enum value.
* `magic_enum::enum_values` obtains enum value sequence.
* `magic_enum::enum_count` obtains number of enum values.
* `magic_enum::enum_name` obtains string name from enum value.
* `magic_enum::enum_names` obtains string enum name sequence.

## Features

* C++17
* Header-only
* Dependency-free
* Compile-time
* Enum to string
* String to enum
* Works with any enum type

## [Examples](example/example.cpp)

* Enum value to string
  ```cpp
  Color color = Color::RED;
  auto color_name = magic_enum::enum_name(color);
  if (color_name.has_value()) {
    // color_name.value() -> "RED"
  }
  ```

* Static storage enum variable to string
  ```cpp
  constexpr Color color = Color::BLUE;
  constexpr auto color_name = magic_enum::enum_name(color);
  if (color_name.has_value()) {
    // color_name.value() -> "BLUE"
  }
  ```

* String to enum value
  ```cpp
  constexpr auto color = magic_enum::enum_cast<Color>("GREEN");
  if (color.has_value()) {
    // color.value() -> Color::GREEN
  }
  ```

* Integer to enum value
  ```cpp
  constexpr auto color = magic_enum::enum_cast<Color>(0);
  if (color.has_value()) {
    // color.value() -> Color::RED
  }
  ```

* Indexed access to enum value
  ```cpp
  constexpr Color color_value = magic_enum::enum_value<Color>(0);
  // color_element -> Color::RED
  ```

* Enum value sequence
  ```cpp
  constexpr auto colors = magic_enum::enum_values<Color>();
  // colors -> {Color::RED, Color::BLUE, Color::GREEN}
  ```

* Number of enum elements
  ```cpp
  constexpr std::size_t color_size = magic_enum::enum_count<Color>();
  // color_size -> 3
  ```

* Enum string sequence
  ```cpp
  constexpr auto color_names = magic_enum::enum_names<Color>();
  // color_names -> {"RED", "BLUE", "GREEN"}
  ```

* Stream operator for enum
  ```cpp
  using namespace magic_enum::ops; // out-of-the-box stream operator for enums.
  Color color = Color::BLUE;
  std::cout << color << std::endl; // "BLUE"
  ```

## Remarks

* `magic_enum::enum_cast` returns `std::optional<E>`, using `has_value()` to check contains enum value and `value()` to get the enum value.

* `magic_enum::enum_values` returns `std::array<E, N>` with all enum value, sorted by enum value.

* `magic_enum::enum_name` returns `std::optional<std::string_view>`, using `has_value()` to check contains enum name and `value()` to get the enum name.

* `magic_enum::enum_names` returns `std::array<std::string_view, N>` with all string enum name, sorted by enum value.

* Enum value must be in range `(-MAGIC_ENUM_RANGE, MAGIC_ENUM_RANGE)`. By default `MAGIC_ENUM_RANGE = 128`. If you need larger range, redefine the macro `MAGIC_ENUM_RANGE`.
  ```cpp
  #define MAGIC_ENUM_RANGE 1028 // Redefine MAGIC_ENUM_RANGE for larger range.
  #include <magic_enum.hpp>
  ```

## Integration

You should add the required file [magic_enum.hpp](include/magic_enum.hpp).

## Compiler compatibility

* Clang/LLVM >= 5
* Visual C++ >= 15.3 / Visual Studio >= 2017
* Xcode >= 10.2
* GCC >= 9

## Licensed under the [MIT License](LICENSE)
