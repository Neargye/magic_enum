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
* `magic_enum::enum_to_string` obtains string enum name from enum variable.
* `magic_enum::enum_from_string` obtains enum value from enum string name.
* `magic_enum::enum_to_sequence` obtains value enum sequence.
* `magic_enum::enum_to_string_sequence` obtains string enum name sequence.

## Features

* C++17
* Header-only
* Dependency-free
* Compile-time
* Enum to string
* String to enum

## [Examples](example/example.cpp)

* Enum variable to string enum name
  ```cpp
  auto color = Color::RED;
  auto color_name = magic_enum::enum_to_string(color);
  if (color_name.has_value()) {
    // color_name.value() -> "RED"
  }
  ```

* Static storage enum variable to string enum name
  ```cpp
  constexpr auto color = Color::BLUE;
  constexpr auto color_name = magic_enum::enum_to_string<color>();
  if (color_name.has_value()) {
    // color_name.value() -> "BLUE"
  }
  ```

* String enum name to enum value
  ```cpp
  constexpr auto color = magic_enum::enum_from_string<Color>("GREEN");
  if (color.has_value()) {
    // color.value() -> Color::GREEN
  }
  ```

* Enum to value sequence
  ```cpp
  constexpr auto colors = magic_enum::enum_to_sequence<Color>();
  // colors -> {Color::RED, Color::BLUE, Color::GREEN}
  ```

* Enum to string enum name sequence
  ```cpp
  constexpr auto color_names = magic_enum::enum_to_string_sequence<Color>();
  // color_names -> {"RED", "BLUE", "GREEN"}
  ```

## Remarks

* `magic_enum::enum_to_string` returns `std::optional<std::string_view>`, using `has_value()` to check contains enum name and `value()` to get the enum name. If enum value does not have name or out of range `MAGIC_ENUM_RANGE`, returns `std::nullopt`.

* `magic_enum::enum_from_string` returns `std::optional<E>`, using `has_value()` to check contains enum value and `value()` to get the enum value. If enum value does not have name or out of range `MAGIC_ENUM_RANGE`, returns `std::nullopt`.

* `magic_enum::enum_to_sequence` returns `std::array<E, N>` with all value enum, sorted by enum value.

* `magic_enum::enum_to_string_sequence` returns `std::array<std::string_view, N>` with all string enum name, sorted by enum value.

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
