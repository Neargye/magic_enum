[![Github releases](https://img.shields.io/github/v/release/Neargye/magic_enum.svg)](https://github.com/Neargye/magic_enum/releases)
[![Conan package](https://img.shields.io/badge/Conan-package-blueviolet)](https://conan.io/center/recipes/magic_enum)
[![Vcpkg package](https://img.shields.io/badge/Vcpkg-package-blueviolet)](https://github.com/microsoft/vcpkg/tree/master/ports/magic-enum)
[![Build2 package](https://img.shields.io/badge/Build2-package-blueviolet)](https://cppget.org/magic_enum)
[![Meson wrap](https://img.shields.io/badge/Meson-wrap-blueviolet)](https://github.com/mesonbuild/wrapdb/blob/master/subprojects/magic_enum.wrap)
[![License](https://img.shields.io/github/license/Neargye/magic_enum.svg)](LICENSE)
[![Compiler explorer](https://img.shields.io/badge/compiler_explorer-online-blue.svg)](https://godbolt.org/z/feqcPa5G6)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/Neargye/magic_enum/badge)](https://securityscorecards.dev/viewer/?uri=github.com/Neargye/magic_enum)

# Magic Enum C++

A header-only C++17 library for enum reflection, without registration macros or boilerplate.

## Documentation

* [Reference](doc/reference.md)
* [Limitations](doc/limitations.md)
* [Integration](#integration)

## [Features & Examples](example/)

Flags, switch, fuse, utility and containers have their own headers, such as `magic_enum_switch.hpp`. `magic_enum_all.hpp` includes them all.

* Basic

  ```cpp
  #include <magic_enum/magic_enum.hpp>
  #include <iostream>

  enum class Color { RED = -10, BLUE = 0, GREEN = 10 };

  int main() {
    Color c1 = Color::RED;
    std::cout << magic_enum::enum_name(c1) << std::endl; // RED
    return 0;
  }
  ```

* String to enum value

  ```cpp
  std::string color_name{"GREEN"};
  auto color = magic_enum::enum_cast<Color>(color_name);
  if (color.has_value()) {
    // color.value() -> Color::GREEN
  }

  // case insensitive enum_cast
  auto color_case_insensitive = magic_enum::enum_cast<Color>(color_name, magic_enum::case_insensitive);

  // enum_cast with BinaryPredicate
  auto color_with_predicate = magic_enum::enum_cast<Color>(color_name, [](char lhs, char rhs) { return std::tolower(static_cast<unsigned char>(lhs)) == std::tolower(static_cast<unsigned char>(rhs)); });
  ```

* Integer to enum value

  ```cpp
  int color_integer = 0;
  auto color = magic_enum::enum_cast<Color>(color_integer);
  if (color.has_value()) {
    // color.value() -> Color::BLUE
  }

  auto color_or_default = magic_enum::enum_cast<Color>(123).value_or(Color::RED);
  ```

* Indexed access to enum value

  ```cpp
  std::size_t i = 0;
  Color color = magic_enum::enum_value<Color>(i);
  // color -> Color::RED
  ```

* Enum value sequence

  ```cpp
  constexpr auto colors = magic_enum::enum_values<Color>();
  // colors -> {Color::RED, Color::BLUE, Color::GREEN}
  // colors[0] -> Color::RED
  ```

* Number of enum values

  ```cpp
  constexpr std::size_t color_count = magic_enum::enum_count<Color>();
  // color_count -> 3
  ```

* Enum value to integer

  ```cpp
  Color color = Color::RED;
  auto color_integer = magic_enum::enum_integer(color); // or magic_enum::enum_underlying(color)
  // color_integer -> -10
  ```

* Enum name sequence

  ```cpp
  constexpr auto color_names = magic_enum::enum_names<Color>();
  // color_names -> {"RED", "BLUE", "GREEN"}
  // color_names[0] -> "RED"
  ```

* Enum entry sequence

  ```cpp
  constexpr auto color_entries = magic_enum::enum_entries<Color>();
  // color_entries -> {{Color::RED, "RED"}, {Color::BLUE, "BLUE"}, {Color::GREEN, "GREEN"}}
  // color_entries[0].first -> Color::RED
  // color_entries[0].second -> "RED"
  ```

* Enum fusion for multi-level switch/case statements

  ```cpp
  switch (magic_enum::enum_fuse(color, direction).value()) {
    case magic_enum::enum_fuse(Color::RED, Directions::Up).value(): // ...
      break;
    case magic_enum::enum_fuse(Color::BLUE, Directions::Down).value(): // ...
      break;
  // ...
  }
  ```

  On MSVC, this switch triggers warning C4064. See [enum_fuse](doc/reference.md#enum_fuse).

* Runtime enum value as constexpr constant

  ```cpp
  Color color = Color::RED;
  magic_enum::enum_switch([](auto val) {
    constexpr Color c_color = val;
    // ...
  }, color);
  ```

* Iterate over enum values as constexpr constants

  ```cpp
  magic_enum::enum_for_each<Color>([](auto val) {
    constexpr Color c_color = val;
    // ...
  });
  ```

* Move through enum values

  ```cpp
  magic_enum::enum_next_value(Color::RED);          // -> optional containing Color::BLUE
  magic_enum::enum_prev_value_circular(Color::RED); // -> Color::GREEN
  ```

* Check whether enum contains value

  ```cpp
  magic_enum::enum_contains(Color::GREEN); // -> true
  magic_enum::enum_contains<Color>(0); // -> true
  magic_enum::enum_contains<Color>(123); // -> false
  magic_enum::enum_contains<Color>("GREEN"); // -> true
  magic_enum::enum_contains<Color>("fda"); // -> false
  ```

* Check whether value can be reflected

  ```cpp
  magic_enum::enum_reflected(Color::GREEN); // -> true
  magic_enum::enum_reflected<Color>(1);     // -> true, but not an enumerator
  ```

* Enum index in sequence

  ```cpp
  constexpr auto color_index = magic_enum::enum_index(Color::BLUE);
  // color_index.value() -> 1
  // color_index.has_value() -> true
  ```

* Flag operations

  ```cpp
  enum Directions : std::uint64_t {
    Left = 1,
    Down = 2,
    Up = 4,
    Right = 8,
  };
  MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(Directions); // From <magic_enum/magic_enum_bitwise.hpp>.

  magic_enum::enum_flags_name(Directions::Up | Directions::Right); // -> "Up|Right"
  magic_enum::enum_flags_name(Directions::Up | Directions::Right, ','); // -> "Up,Right"
  magic_enum::enum_flags_contains(Directions::Up | Directions::Right); // -> true
  magic_enum::enum_flags_cast<Directions>(3).value(); // -> Directions::Left|Directions::Down
  magic_enum::enum_flags_cast<Directions>("Left,Down", ',').value(); // -> Directions::Left|Directions::Down
  magic_enum::enum_flags_test(Directions::Up | Directions::Right, Directions::Up); // -> true
  magic_enum::enum_flags_test_any(Directions::Left | Directions::Down, Directions::Down | Directions::Right); // -> true
  magic_enum::is_flags_enum_v<Directions>; // -> true
  ```

  **Note:** bitwise operators found through ADL automatically enable [flag reflection](doc/limitations.md#enum-flags). Only named zero and single bits are reflected. For combinations, `enum_flags_name` joins their bit names. Automatic detection fails to compile if a named value has more than one bit set, such as `All = A | B`. Set `is_flags` explicitly for such enums. To keep ordinary reflection, set `customize::enum_range<E>::is_flags = false` or define `MAGIC_ENUM_NO_AUTO_IS_FLAGS` for the whole build.

* Enum type name

  ```cpp
  Color color = Color::RED;
  auto type_name = magic_enum::enum_type_name<decltype(color)>();
  // type_name -> "Color"
  ```

* I/O stream operators for enums

  ```cpp
  #include <magic_enum/magic_enum_iostream.hpp>

  enum class Color { RED, GREEN, BLUE };
  MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS(Color); // Operators for Color only, no using needed.

  Color color = Color::BLUE;
  std::cout << color << std::endl; // "BLUE"
  std::cin >> color;
  ```

* Bitwise operators for enums

  ```cpp
  #include <magic_enum/magic_enum_bitwise.hpp>

  enum class Flags { A = 1 << 0, B = 1 << 1, C = 1 << 2, D = 1 << 3 };
  MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(Flags); // Defines ~, |, &, ^, |=, &=, ^= for Flags.

  Flags flags = Flags::A | (Flags::B & ~Flags::C);
  ```

  For a nested enum, use `MAGIC_ENUM_DEFINE_FRIEND_*` inside its class. Prefer these macros to operator namespaces, which affect every enum in scope. See [operator namespace risks](doc/reference.md#risks-of-operator-namespaces).

* Formatting

  ```cpp
  #include <format>
  #include <magic_enum/magic_enum_format.hpp>

  std::format("{}", Color::RED); // -> "RED"
  std::format("{}", Color{42});  // -> "42"
  ```

  For `{fmt}`, include it before `magic_enum_format.hpp`.

* Scoped and unscoped enum traits

  ```cpp
  enum color { red, green, blue };
  enum class direction { left, right };

  static_assert(magic_enum::is_unscoped_enum_v<color>);
  static_assert(magic_enum::is_scoped_enum_v<direction>);
  ```

* Compile-time enum value to string, without the `enum_range` [limit](doc/limitations.md).

  ```cpp
  constexpr Color color = Color::BLUE;
  constexpr auto color_name = magic_enum::enum_name<color>();
  // color_name -> "BLUE"
  ```

* `containers::array`

  ```cpp
  // In enum_values order: RED, BLUE, GREEN.
  constexpr auto color_rgb_values = magic_enum::containers::make_array<Color>(RGB{255, 0, 0}, RGB{0, 0, 255}, RGB{0, 255, 0});

  magic_enum::containers::array<Color, RGB> color_rgb_array {};
  color_rgb_array[Color::RED] = {255, 0, 0};
  color_rgb_array[Color::GREEN] = {0, 255, 0};
  color_rgb_array[Color::BLUE] = {0, 0, 255};
  magic_enum::containers::get<Color::BLUE>(color_rgb_array); // -> RGB{0, 0, 255}
  ```

* `containers::bitset`

  ```cpp
  constexpr magic_enum::containers::bitset<Color> color_bitset {Color::RED, Color::GREEN};
  color_bitset.test(Color::RED);  // -> true
  color_bitset.test(Color::BLUE); // -> false

  std::uint8_t incoming = 0b00000011;
  auto raw_bitset = magic_enum::containers::bitset<Color> {magic_enum::containers::raw_access, incoming};
  ```

* `containers::set`

  ```cpp
  auto color_set = magic_enum::containers::set<Color>();
  bool empty = color_set.empty();
  // empty -> true
  color_set.insert(Color::GREEN);
  color_set.insert(Color::BLUE);
  color_set.insert(Color::RED);
  std::size_t size = color_set.size();
  // size -> 3

  using color_name_set = magic_enum::containers::set<Color, magic_enum::containers::name_less<>>;
  color_name_set colors_by_name {Color::RED, Color::GREEN, Color::BLUE};
  ```

* [Underlying type](https://en.cppreference.com/w/cpp/types/underlying_type)

  ```cpp
  static_assert(std::is_same_v<magic_enum::underlying_type_t<Color>, int>);
  ```

## Integration

* Copy the required headers from [`include/magic_enum`](include/magic_enum) or a [release archive](https://github.com/Neargye/magic_enum/releases/latest). `magic_enum_all.hpp` includes all public headers.
* With CMake 3.22+, use `add_subdirectory` or `find_package(magic_enum CONFIG REQUIRED)`, then link `magic_enum::magic_enum`.
* Use [vcpkg](https://github.com/microsoft/vcpkg/tree/master/ports/magic-enum), [Conan](https://conan.io/center/recipes/magic_enum), [Build2](https://cppget.org/magic_enum), or [Meson](https://github.com/mesonbuild/wrapdb/blob/master/subprojects/magic_enum.wrap).
* Fetch sources with CMake [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html) or [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake). Release tags use `vx.y.z` format.
* Use Bazel with `MODULE.bazel` or `http_archive`. The target is `@magic_enum//:magic_enum`.
* Use ROS with `<depend>magic_enum</depend>` in `package.xml`, then link `magic_enum::magic_enum`.

* **CMake targets**:
  - `magic_enum::magic_enum` is the header-only target.
  - `magic_enum::magic_enum_module` is the C++20 module target. Enable it with `MAGIC_ENUM_USE_MODULES=ON`. CMake 3.28+ is required. On MSVC use Visual Studio 2026 or newer, and on GCC use GCC 16 or newer. Older versions have [module bugs](doc/limitations.md).

  Build the module target:
  ```sh
  cmake -S . -B build -G Ninja -DMAGIC_ENUM_USE_MODULES=ON
  cmake --build build
  ```

  Link the module target:
  ```cmake
  find_package(magic_enum CONFIG REQUIRED)
  target_link_libraries(your_executable PRIVATE magic_enum::magic_enum_module)
  set_target_properties(your_executable PROPERTIES CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES ON)
  ```

  Import the module:
  ```cpp
  import magic_enum;

  enum class Color { RED, GREEN, BLUE };
  auto name = magic_enum::enum_name(Color::RED); // "RED"
  ```

  Use either the headers or the module in the whole program. Build and use an installed module with the same compiler, standard library and C++ standard. pkg-config provides only the header-only target.

  Optional settings:
  - `MAGIC_ENUM_MODULE_WITH_FMT=ON` enables `{fmt}` through `fmt::fmt`. The `{fmt}` module is not supported.
  - `MAGIC_ENUM_MODULE_IMPORT_STD=ON` enables `import std`. It requires `CMAKE_CXX_STANDARD` 23 or later, CMake 3.31+ and a toolchain with `import std` support.

## Compiler compatibility

* Clang/LLVM >= 9
* MSVC >= 19.20 / Visual Studio >= 2019 (16.0)
* Xcode >= 11
* GCC >= 9

C++26 reflection is selected automatically when available. See [limitations](doc/limitations.md#c26-standard-reflection).

## Licensed under the [MIT License](LICENSE)
