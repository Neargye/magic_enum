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

Branch | Linux/OSX | Windows | License | Codacy
-------|-----------|---------|---------|-------
master |-----------|---------|---------|-------

## What is Magic Enum?

Header-only C++17 library provides Enum-to-String and String-to-Enum functions.
* `magic_enum::enum_to_string` obtain string enum name from enum variable.
* `magic_enum::enum_from_string` obtain enum variable from enum string name.

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
// Enum variable to string enum name
auto color_name = magic_enum::enum_to_string(c);
if (color_name.has_value()) {
  // color_name.value() -> "RED"
}

constexpr auto cx_color = Color::BLUE;
// Static storage enum variable to string enum name
auto cx_color_name = magic_enum::enum_to_string<cx_color>();
if (color_name.has_value()) {
  // cx_color_name.value() -> "BLUE"
}
```

* String to enum
```cpp
// String enum name to enum variable
auto color = magic_enum::enum_from_string<Color>("GREEN");
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
