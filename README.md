[![Github releases](https://img.shields.io/github/release/Neargye/magic_enum.svg)](https://github.com/Neargye/magic_enum/releases)
[![Conan package](https://img.shields.io/badge/Conan-package-blueviolet)](https://conan.io/center/recipes/magic_enum)
[![Vcpkg package](https://img.shields.io/badge/Vcpkg-package-blueviolet)](https://github.com/microsoft/vcpkg/tree/master/ports/magic-enum)
[![Build2 package](https://img.shields.io/badge/Build2-package-blueviolet)](https://www.cppget.org/magic_enum?q=magic_enum)
[![Meson wrap](https://img.shields.io/badge/Meson-wrap-blueviolet)](https://github.com/mesonbuild/wrapdb/blob/master/subprojects/magic_enum.wrap)
[![License](https://img.shields.io/github/license/Neargye/magic_enum.svg)](LICENSE)
[![Compiler explorer](https://img.shields.io/badge/compiler_explorer-online-blue.svg)](https://godbolt.org/z/feqcPa5G6)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/Neargye/magic_enum/badge)](https://securityscorecards.dev/viewer/?uri=github.com/Neargye/magic_enum)

# Magic Enum C++

Header-only C++17 library provides static reflection for enums, work with any enum type without any macro or boilerplate code.

## Documentation

* [Reference](doc/reference.md)
* [Limitations](doc/limitations.md)
* [Integration](#integration)

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
  auto color_case_insensitive = magic_enum::enum_cast<Color>(color_name, magic_enum::case_insensitive);

  // enum_cast with BinaryPredicate
  auto color_with_predicate = magic_enum::enum_cast<Color>(color_name, [](char lhs, char rhs) { return std::tolower(static_cast<unsigned char>(lhs)) == std::tolower(static_cast<unsigned char>(rhs)); });

  // enum_cast with default
  auto color_or_default = magic_enum::enum_cast<Color>(color_name).value_or(Color::RED);
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
  auto color_integer = magic_enum::enum_integer(color); // or magic_enum::enum_underlying(color);
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
    case magic_enum::enum_fuse(Color::BLUE, Directions::Down).value(): // ...
  // ...
  }
  ```

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

* Check whether value is in reflection range

  ```cpp
  magic_enum::enum_reflected<Color>(123); // -> true
  magic_enum::enum_contains<Color>(123);  // -> false
  magic_enum::enum_reflected<Color>(128); // -> false
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
  using namespace magic_enum::bitwise_operators; // Use with care; operators are enabled for all enums.

  magic_enum::enum_flags_name(Directions::Up | Directions::Right); // -> "Up|Right"
  magic_enum::enum_flags_name(Directions::Up | Directions::Right, ','); // -> "Up,Right"
  magic_enum::enum_flags_contains(Directions::Up | Directions::Right); // -> true
  magic_enum::enum_flags_cast<Directions>(3).value(); // -> Directions::Left|Directions::Down
  magic_enum::enum_flags_cast<Directions>("Left,Down", ',').value(); // -> Directions::Left|Directions::Down
  magic_enum::enum_flags_test(Directions::Up | Directions::Right, Directions::Up); // -> true
  magic_enum::enum_flags_test_any(Directions::Left | Directions::Down, Directions::Down | Directions::Right); // -> true
  magic_enum::is_flags_v<Directions>; // -> true
  ```

* Enum type name

  ```cpp
  Color color = Color::RED;
  auto type_name = magic_enum::enum_type_name<decltype(color)>();
  // type_name -> "Color"
  ```

* I/O stream operators for enums

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

* Bitwise operators for enums

  ```cpp
  enum class Flags { A = 1 << 0, B = 1 << 1, C = 1 << 2, D = 1 << 3 };
  using namespace magic_enum::bitwise_operators; // Use with care; operators are enabled for all enums.
  // Support operators: ~, |, &, ^, |=, &=, ^=.
  Flags flags = Flags::A | (Flags::B & ~Flags::C);
  ```

* Formatting

  ```cpp
  #include <format>
  #include <magic_enum/magic_enum_format.hpp>

  std::format("{}", Color::RED); // -> "RED"
  std::format("{}", Color{42});  // -> "42"
  ```

  Include `{fmt}` before `magic_enum_format.hpp` to enable `{fmt}` formatter support.

* Identifies [unscoped enum types](https://en.cppreference.com/w/cpp/language/enum#Unscoped_enumeration).

  ```cpp
  enum color { red, green, blue };
  enum class direction { left, right };

  magic_enum::is_unscoped_enum<color>::value -> true
  magic_enum::is_unscoped_enum<direction>::value -> false
  magic_enum::is_unscoped_enum<int>::value -> false

  // Helper variable template.
  magic_enum::is_unscoped_enum_v<color> -> true
  ```

* Identifies [scoped enum types](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations).

  ```cpp
  enum color { red, green, blue };
  enum class direction { left, right };

  magic_enum::is_scoped_enum<color>::value -> false
  magic_enum::is_scoped_enum<direction>::value -> true
  magic_enum::is_scoped_enum<int>::value -> false

  // Helper variable template.
  magic_enum::is_scoped_enum_v<direction> -> true
  ```

* Compile-time enum value to string
  This overload compiles faster and is not restricted by `enum_range` [limitation](doc/limitations.md).

  ```cpp
  constexpr Color color = Color::BLUE;
  constexpr auto color_name = magic_enum::enum_name<color>();
  // color_name -> "BLUE"
  ```

* `containers::array` array container for enums.

  ```cpp
  constexpr auto color_rgb_values = magic_enum::containers::make_array<Color>(RGB{255, 0, 0}, RGB{0, 255, 0}, RGB{0, 0, 255});

  magic_enum::containers::array<Color, RGB> color_rgb_array {};
  color_rgb_array[Color::RED] = {255, 0, 0};
  color_rgb_array[Color::GREEN] = {0, 255, 0};
  color_rgb_array[Color::BLUE] = {0, 0, 255};
  magic_enum::containers::get<Color::BLUE>(color_rgb_array); // -> RGB{0, 0, 255}
  ```

* `containers::bitset` bitset container for enums.

  ```cpp
  constexpr magic_enum::containers::bitset<Color> color_bitset {Color::RED, Color::GREEN};
  color_bitset.test(Color::RED);  // -> true
  color_bitset.test(Color::BLUE); // -> false

  std::uint8_t incoming = 0b00000011;
  auto raw_bitset = magic_enum::containers::bitset<Color> {magic_enum::containers::raw_access, incoming};
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

  using color_name_set = magic_enum::containers::set<Color, magic_enum::containers::name_less<>>;
  color_name_set colors_by_name {Color::RED, Color::GREEN, Color::BLUE};
  ```

* Improved UB-free, SFINAE-friendly [underlying type trait](https://en.cppreference.com/w/cpp/types/underlying_type).

  ```cpp
  magic_enum::underlying_type<color>::type -> int

  // Helper types.
  magic_enum::underlying_type_t<Direction> -> int
  ```

## Remarks

* `magic_enum` is not a silver bullet for enum reflection and was originally designed for small enums.

* Before use, read [limitations](doc/limitations.md).

## Integration

* Copy required headers from [`include/magic_enum`](include/magic_enum) or use [release archive](https://github.com/Neargye/magic_enum/releases/latest).
* Use CMake with `add_subdirectory` or `find_package(magic_enum CONFIG REQUIRED)`, then link `magic_enum::magic_enum`.
* Use [vcpkg](https://github.com/microsoft/vcpkg/tree/master/ports/magic-enum), [Conan](https://conan.io/center/recipes/magic_enum), [Build2](https://cppget.org/magic_enum?q=magic_enum), or [Meson](https://github.com/mesonbuild/wrapdb/blob/master/subprojects/magic_enum.wrap).
* Fetch sources with CMake [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html) or [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake). Release tags use `vx.y.z` format.
* Use Bazel with `MODULE.bazel` or `http_archive`; target is `@magic_enum//:magic_enum`.
* Use ROS with `<depend>magic_enum</depend>` in `package.xml`, then link `magic_enum::magic_enum`.
* Use C++20 module with CMake 3.28+: configure with `MAGIC_ENUM_USE_MODULES=ON`, link `magic_enum::magic_enum`, and write `import magic_enum;`. Do not mix module import and `magic_enum` headers in same program.

## Compiler compatibility

* Clang/LLVM >= 5
* MSVC++ >= 15.3 / Visual Studio >= 2017
* Xcode >= 10
* GCC >= 9

## Licensed under the [MIT License](LICENSE)
