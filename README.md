[![Github releases](https://img.shields.io/github/release/Neargye/magic_enum.svg)](https://github.com/Neargye/magic_enum/releases)
[![Conan package](https://img.shields.io/badge/Conan-package-blueviolet)](https://conan.io/center/recipes/magic_enum)
[![Vcpkg package](https://img.shields.io/badge/Vcpkg-package-blueviolet)](https://github.com/microsoft/vcpkg/tree/master/ports/magic-enum)
[![Build2 package](https://img.shields.io/badge/Build2-package-blueviolet)](https://www.cppget.org/magic_enum?q=magic_enum)
[![Meson wrap](https://img.shields.io/badge/Meson-wrap-blueviolet)](https://github.com/mesonbuild/wrapdb/blob/master/subprojects/magic_enum.wrap)
[![License](https://img.shields.io/github/license/Neargye/magic_enum.svg)](LICENSE)
[![Compiler explorer](https://img.shields.io/badge/compiler_explorer-online-blue.svg)](https://godbolt.org/z/feqcPa5G6)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/Neargye/magic_enum/badge)](https://securityscorecards.dev/viewer/?uri=github.com/Neargye/magic_enum)
[![Stand With Ukraine](https://raw.githubusercontent.com/vshymanskyy/StandWithUkraine/main/badges/StandWithUkraine.svg)](https://stand-with-ukraine.pp.ua)

# Magic Enum C++

Header-only C++17 library provides static reflection for enums, work with any enum type without any macro or boilerplate code.

If you like this project, please consider donating to one of the funds that help victims of the war in Ukraine: https://u24.gov.ua.

## Documentation

* [Reference](doc/reference.md)
* [Limitations](doc/limitations.md)
* [Integration](#Integration)

## [Features & Examples](example/)

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

  // case insensitive enum_cast
  auto color = magic_enum::enum_cast<Color>(value, magic_enum::case_insensitive);

  // enum_cast with BinaryPredicate
  auto color = magic_enum::enum_cast<Color>(value, [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); });

  // enum_cast with default
  auto color_or_default = magic_enum::enum_cast<Color>(value).value_or(Color::RED);
  ```

* Integer to enum value

  ```cpp
  int color_integer = 0;
  auto color = magic_enum::enum_cast<Color>(color_integer);
  if (color.has_value()) {
    // color.value() -> Color::BLUE
  }

  auto color_or_default = magic_enum::enum_cast<Color>(value).value_or(Color::RED);
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

* Number of enum elements

  ```cpp
  constexpr std::size_t color_count = magic_enum::enum_count<Color>();
  // color_count -> 3
  ```

* Enum value to integer

  ```cpp
  Color color = Color::RED;
  auto color_integer = magic_enum::enum_integer(color); // or magic_enum::enum_underlying(color);
  // color_integer -> -10
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

* Enum fusion for multi-level switch/case statements

  ```cpp
  switch (magic_enum::enum_fuse(color, direction).value()) {
    case magic_enum::enum_fuse(Color::RED, Directions::Up).value(): // ...
    case magic_enum::enum_fuse(Color::BLUE, Directions::Down).value(): // ...
  // ...
  }
  ```

* Enum switch runtime value as constexpr constant
  ```cpp
  Color color = Color::RED;
  magic_enum::enum_switch([] (auto val) {
    constexpr Color c_color = val;
    // ...
  }, color);
  ```

* Enum iterate for each enum as constexpr constant
  ```cpp
  magic_enum::enum_for_each<Color>([] (auto val) {
    constexpr Color c_color = val;
    // ...
  });
  ```

* Check if enum contains

  ```cpp
  magic_enum::enum_contains(Color::GREEN); // -> true
  magic_enum::enum_contains<Color>(0); // -> true
  magic_enum::enum_contains<Color>(123); // -> false
  magic_enum::enum_contains<Color>("GREEN"); // -> true
  magic_enum::enum_contains<Color>("fda"); // -> false
  ```

* Enum index in sequence

  ```cpp
  constexpr auto color_index = magic_enum::enum_index(Color::BLUE);
  // color_index.value() -> 1
  // color_index.has_value() -> true
  ```

* Functions for flags

  ```cpp
  enum Directions : std::uint64_t {
    Left = 1,
    Down = 2,
    Up = 4,
    Right = 8,
  };
  template <>
  struct magic_enum::customize::enum_range<Directions> {
    static constexpr bool is_flags = true;
  };
  using namespace magic_enum::bitwise_operators;

  magic_enum::enum_flags_name(Directions::Up | Directions::Right); // -> "Up|Right"
  magic_enum::enum_flags_name(Directions::Up | Directions::Right, ','); // -> "Up,Right"
  magic_enum::enum_flags_contains(Directions::Up | Directions::Right); // -> true
  magic_enum::enum_flags_cast<Directions>(3).value(); // -> Directions::Left|Directions::Down
  magic_enum::enum_flags_cast<Directions>("Left,Down", ',').value(); // -> Directions::Left|Directions::Down
  ```

* Enum type name

  ```cpp
  Color color = Color::RED;
  auto type_name = magic_enum::enum_type_name<decltype(color)>();
  // type_name -> "Color"
  ```

* IOstream operator for enum

  ```cpp
  using magic_enum::iostream_operators::operator<<; // out-of-the-box ostream operators for enums.
  Color color = Color::BLUE;
  std::cout << color << std::endl; // "BLUE"
  ```

  ```cpp
  using magic_enum::iostream_operators::operator>>; // out-of-the-box istream operators for enums.
  Color color;
  std::cin >> color;
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

* Checks whether type is a [Scoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations).

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

* `containers::array` array container for enums.

  ```cpp
  magic_enum::containers::array<Color, RGB> color_rgb_array {};
  color_rgb_array[Color::RED] = {255, 0, 0};
  color_rgb_array[Color::GREEN] = {0, 255, 0};
  color_rgb_array[Color::BLUE] = {0, 0, 255};
  magic_enum::containers::get<Color::BLUE>(color_rgb_array) // -> RGB{0, 0, 255}
  ```

* `containers::bitset` bitset container for enums.

  ```cpp
  std::uint8_t incoming = 0b00000011;
  auto color_bitset = magic_enum::containers::bitset<Color> {magic_enum::containers::raw_access, incoming};
  color_bitset.set(Color::BLUE);
  auto raw_value = color_bitset.to_ulong(magic_enum::containers::raw_access);
  ```

* `containers::set` set container for enums.

  ```cpp
  auto color_set = magic_enum::containers::set<Color>();
  bool empty = color_set.empty();
  // empty -> true
  color_set.insert(Color::GREEN);
  color_set.insert(Color::BLUE);
  color_set.insert(Color::RED);
  std::size_t size = color_set.size();
  // size -> 3
  ```

* Improved UB-free "SFINAE-friendly" [underlying_type](https://en.cppreference.com/w/cpp/types/underlying_type).

  ```cpp
  magic_enum::underlying_type<color>::type -> int

  // Helper types.
  magic_enum::underlying_type_t<Direction> -> int
  ```
## Remarks

* `magic_enum` does not pretend to be a silver bullet for reflection for enums, it was originally designed for small enum.

* Before use, read the [limitations](doc/limitations.md) of functionality.

## Integration

* You should add the required file [magic_enum.hpp](include/magic_enum/magic_enum.hpp), and optionally other headers from [include dir](include/) or [release archive](https://github.com/Neargye/magic_enum/releases/latest). Alternatively, you can build the library with CMake.

* If you are using [vcpkg](https://github.com/Microsoft/vcpkg/) on your project for external dependencies, then you can use the [magic-enum package](https://github.com/microsoft/vcpkg/tree/master/ports/magic-enum).

* If you are using [Conan](https://www.conan.io/) to manage your dependencies, merely add `magic_enum/x.y.z` to your conan's requires, where `x.y.z` is the release version you want to use.

* If you are using [Build2](https://build2.org/) to build and manage your dependencies, add `depends: magic_enum ^x.y.z` to the manifest file where `x.y.z` is the release version you want to use. You can then import the target using `magic_enum%lib{magic_enum}`.

* Alternatively, you can use something like [CPM](https://github.com/TheLartians/CPM) which is based on CMake's `Fetch_Content` module.

  ```cmake
  CPMAddPackage(
      NAME magic_enum
      GITHUB_REPOSITORY Neargye/magic_enum
      GIT_TAG vx.y.z # Where `x.y.z` is the release version you want to use.
  )
  ```

* Bazel is also supported, simply add to your WORKSPACE file:

  ```
  http_archive(
      name = "magic_enum",
      strip_prefix = "magic_enum-<commit>",
      urls = ["https://github.com/Neargye/magic_enum/archive/<commit>.zip"],
  )
  ```

  To use bazel inside the repository it's possible to do:

  ```
  bazel build //...
  bazel test //...
  bazel run //example
  ```

  (Note that you must use a supported compiler or specify it with `export CC= <compiler>`.)

* If you are using [Ros](https://www.ros.org/), you can include this package by adding `<depend>magic_enum</depend>` to your package.xml and include this package in your workspace. In your CMakeLists.txt add the following:
  ```cmake
  find_package(magic_enum CONFIG REQUIRED)
  ...
  target_link_libraries(your_executable magic_enum::magic_enum)
  ```

* **CMake targets**:
  - `magic_enum::magic_enum` is always provided for the header-only library;
  - `magic_enum::magic_enum_module` is provided for the C++20 module library when `MAGIC_ENUM_USE_MODULES=ON`.

  Use the header-only target with `#include <magic_enum/magic_enum.hpp>`, or substitute the module target and use `import magic_enum;`.

  Maintainers using CMake 3.24+ can enable `MAGIC_ENUM_OPT_VERIFY_INTERFACE_HEADER_SETS=ON` to verify that every public header can be included independently. The option is enabled by default when `MAGIC_ENUM_OPT_BUILD_TESTS` is enabled on supported CMake versions.

* **C++20 modules** require CMake 3.28+ and are explicitly enabled when building magic_enum.

  Configure with CMake:
  ```sh
  cmake -S . -B build -G Ninja -DMAGIC_ENUM_USE_MODULES=ON
  ```

  Link the module target instead of the header-only target:
  ```cmake
  target_link_libraries(your_executable PRIVATE magic_enum::magic_enum_module)
  ```

  Then use `import` instead of `#include`:
  ```cpp
  import magic_enum;

  enum class Color { RED, GREEN, BLUE };
  auto name = magic_enum::enum_name(Color::RED); // "RED"
  ```

  Caveats:
  - Do not mix `#include <magic_enum/...>` and `import magic_enum;` within the same program; this is an ODR violation.
  - An installed module package must be consumed with a compatible compiler, standard library, C++ language standard, and compiler-extension mode. The module target propagates the language standard used to build it, but CMake cannot validate every toolchain option that affects module compatibility.
  - An installed package provides `magic_enum::magic_enum_module` only when magic_enum was configured with `MAGIC_ENUM_USE_MODULES=ON` before installation.
  - Module configuration macros are fixed when `magic_enum::magic_enum_module` is built; they are not applied independently by each consumer translation unit.
  - `import std;` is opt-in and experimental (requires CMake 3.30+). Set `CMAKE_EXPERIMENTAL_CXX_IMPORT_STD` to the UUID documented by the selected CMake version before `project()`, choose a supported `CMAKE_CXX_STANDARD`, and enable `MAGIC_ENUM_MODULE_IMPORT_STD=ON`. The experimental UUID is version-specific and is intentionally not selected by magic_enum. If a consumer source also uses `import std;`, enable it on that consumer target with `set_property(TARGET your_executable PROPERTY CXX_MODULE_STD ON)`; this target property is not propagated by linking `magic_enum::magic_enum_module`.
  - `{fmt}` integration is explicit for the compiled module. Enable it with `MAGIC_ENUM_MODULE_WITH_FMT=ON`; this requires the `fmt` CMake package and makes `fmt::fmt` a public dependency of `magic_enum::magic_enum_module`. The installed magic_enum package will resolve the dependency with `find_dependency(fmt CONFIG)`.

## Compiler compatibility

* Clang/LLVM >= 5
* MSVC++ >= 14.11 / Visual Studio >= 2017
* Xcode >= 10
* GCC >= 9
* MinGW >= 9

## Licensed under the [MIT License](LICENSE)
