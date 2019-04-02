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
[![Try online](https://img.shields.io/badge/try-online-blue.svg)](https://wandbox.org/permlink/vvYmXey2yclPdeKM)

## What is Magic Enum?

Header-only C++17 library provides Enum-to-String and String-to-Enum functions.
* `magic_enum::enum_to_string` used to obtain string enum name from enum variable.
* `magic_enum::enum_from_string` used to obtain enum variable from enum string name.

## Features

* C++17
* Header-only
* Dependency-free
* Compile-time
* Enum to string
* String to enum

## [Examples](example/example.cpp)

* Enum to string
```cpp
auto color = Color::RED;
// Enum variable to string enum name.
auto color_name = magic_enum::enum_to_string(c);
if (color_name.has_value()) {
  // color_name.value() -> "RED"
}

constexpr auto cx_color = Color::BLUE;
// Static storage enum variable to string enum name.
constexpr auto color_name = magic_enum::enum_to_string<cx_color>();
if (color_name.has_value()) {
  // color_name.value() -> "BLUE"
}
```

* String to enum
```cpp
// String enum name to enum variable.
constexpr auto color = magic_enum::enum_from_string<Color>("GREEN");
if (color.has_value()) {
  // color.value() -> Color::GREEN
}
```

## Integration

You have to add required file [magic_enum.hpp](include/magic_enum.hpp).

## Compiler compatibility

* Clang/LLVM >= 5
* Visual C++ >= 15.3 / Visual Studio >= 2017
* Xcode >= 10.2
* GCC >= 9

## Licensed under the [MIT License](LICENSE)
