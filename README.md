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

[![Github releases](https://img.shields.io/github/release/Neargye/magic_enum.svg)](https://github.com/Neargye/magic_enum/releases)
[![Conan package](https://img.shields.io/badge/Conan-package-blueviolet)](https://bintray.com/neargye/conan-packages/magic_enum:neargye)
[![Vcpkg package](https://img.shields.io/badge/Vcpkg-package-blueviolet)](https://github.com/microsoft/vcpkg/tree/master/ports/magic-enum)
[![License](https://img.shields.io/github/license/Neargye/magic_enum.svg)](LICENSE)
[![Build status](https://travis-ci.org/Neargye/magic_enum.svg?branch=master)](https://travis-ci.org/Neargye/magic_enum)
[![Build status](https://ci.appveyor.com/api/projects/status/0rpr966p9ssrvwu3/branch/master?svg=true)](https://ci.appveyor.com/project/Neargye/magic-enum-hf8vk/branch/master)
[![Codacy badge](https://api.codacy.com/project/badge/Grade/64d04f150af14c3e8bd1090057b68538)](https://www.codacy.com/app/Neargye/magic_enum?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Neargye/magic_enum&amp;utm_campaign=Badge_Grade)
[![Try online](https://img.shields.io/badge/try-online-blue.svg)](https://wandbox.org/permlink/2exUJizXSEFgKCvo)
[![Compiler explorer](https://img.shields.io/badge/compiler_explorer-online-blue.svg)](https://godbolt.org/z/BxfmsH)

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
* `enum_index` obtains index in enum value sequence from enum value.
* `enum_contains` checks whether enum contains enumerator with such value.
* `enum_type_name` returns string name of enum type.
* `is_unscoped_enum` checks whether type is an [Unscoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Unscoped_enumeration).
* `is_scoped_enum` checks whether type is an [Scoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations).
* `underlying_type` improved UB-free "SFINAE-friendly" [std::underlying_type](https://en.cppreference.com/w/cpp/types/underlying_type).
* `ostream_operators` ostream operators for enums.
* `bitwise_operators` bitwise operators for enums.

## Documentation

* [Reference](doc/reference.md)
* [Limitations](doc/limitations.md)
* [Integration](#Integration)

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
  Color color = magic_enum::enum_value<Color>(i);
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

* Ostream operator for enum

  ```cpp
  using namespace magic_enum::ostream_operators; // out-of-the-box ostream operators for enums.
  Color color = Color::BLUE;
  std::cout << color << std::endl; // "BLUE"
  ```

* Bitwise operator for enum

  ```cpp
  enum class Flags { A = 1 << 0, B = 1 << 1, C = 1 << 2, D = 1 << 3 };
  using namespace magic_enum::bitwise_operators; // out-of-the-box bitwise operators for enums.
  // Support operators: ~, |, &, ^, |=, &=, ^=.
  Flags flags = Flags::A | Flags::B & ~Flags::C;
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
  This version is much lighter on the compile times and is not restricted to the enum_range [limitation](doc/limitations.md).

  ```cpp
  constexpr Color color = Color::BLUE;
  constexpr auto color_name = magic_enum::enum_name<color>();
  // color_name -> "BLUE"
  ```

## Remarks

* `magic_enum` does not pretend to be a silver bullet for reflection for enums, it was originally designed for small enum.

* Before use, read the [limitations](doc/limitations.md) of functionality.

## Integration

You should add the required file [magic_enum.hpp](include/magic_enum.hpp).

If you are using [vcpkg](https://github.com/Microsoft/vcpkg/) on your project for external dependencies, then you can use the [magic_enum package](https://github.com/microsoft/vcpkg/tree/master/ports/magic-enum).

If you are using [Conan](https://www.conan.io/) to manage your dependencies, merely add `magic_enum/x.y.z@neargye/stable` to your conan's requires, where `x.y.z` is the release version you want to use.

Alternatively, you can use something like [CPM](https://github.com/TheLartians/CPM) which is based on CMake's `Fetch_Content` module.

```cmake
CPMAddPackage(
    NAME magic_enum
    GITHUB_REPOSITORY Neargye/magic_enum
    GIT_TAG x.y.z # Where `x.y.z` is the release version you want to use.
)
```

## Compiler compatibility

* Clang/LLVM >= 5
* MSVC++ >= 14.11 / Visual Studio >= 2017
* Xcode >= 10
* GCC >= 9

## Licensed under the [MIT License](LICENSE)
