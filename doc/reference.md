# Reference

* [`enum_cast` returns enum value from string or integer.](#enum_cast)
* [`enum_value` returns enum value at specified index.](#enum_value)
* [`enum_values` returns enum value sequence.](#enum_values)
* [`enum_count` returns number of enum values.](#enum_count)
* [`enum_integer` and `enum_underlying` return underlying enum value.](#enum_integer-and-enum_underlying)
* [`enum_name` returns name from enum value.](#enum_name)
* [`enum_names` returns enum name sequence.](#enum_names)
* [`enum_entries` returns enum value and name sequence.](#enum_entries)
* [`customize::enum_range` customizes enum reflection.](#customizeenum_range)
* [`enum_index` returns index from enum value.](#enum_index)
* [`enum_contains` checks whether enum contains value or name.](#enum_contains)
* [`enum_reflected` checks whether enum value can be reflected.](#enum_reflected)
* [`enum_type_name` returns enum type name.](#enum_type_name)
* [`enum_fuse` combines enum values for switch/case statements.](#enum_fuse)
* [`enum_switch` transforms runtime enum value into constexpr constant.](#enum_switch)
* [`enum_for_each` calls function for each enum value as constexpr constant.](#enum_for_each)
* [`enum_next_value` and `enum_prev_value` move through enum values.](#enum_next_value-and-enum_prev_value)
* [`enum_flags_*` functions operate on flags.](#enum_flags)
* [`is_flags_enum` checks whether enum uses flag semantics.](#is_flags_enum)
* [`is_unscoped_enum` identifies unscoped enum types.](#is_unscoped_enum)
* [`is_scoped_enum` identifies scoped enum types.](#is_scoped_enum)
* [`underlying_type` provides underlying enum type.](#underlying_type)
* [`ostream_operators` provides stream insertion operators.](#ostream_operators)
* [`istream_operators` provides stream extraction operators.](#istream_operators)
* [`bitwise_operators` provides bitwise operators.](#bitwise_operators)
* [`std::format` and `fmt::format` support for enums.](#formatting)
* [Container comparators and indexing helpers.](#container-helpers)
* [`containers::array` array container for enums.](#containersarray)
* [`containers::bitset` bitset container for enums.](#containersbitset)
* [`containers::set` set container for enums.](#containersset)

## Synopsis

* Before use, read [limitations](limitations.md).

* Use `MAGIC_ENUM_SUPPORTED` or `magic_enum::is_magic_enum_supported` to check compiler support. Unsupported compilers cause compilation errors unless `MAGIC_ENUM_NO_CHECK_SUPPORT` is defined.

* C++26 reflection is selected automatically when available. See [limitations](limitations.md#c26-standard-reflection).

* `Enum<T>` constrains C++17 function parameters to enum types.

* To add custom enum or type names, see [example](../example/example_custom_name.cpp).

* To customize string and optional types, define these macros before including `magic_enum.hpp`:

  ```cpp
  #include <my_lib/string.hpp>
  #include <my_lib/string_view.hpp>
  #define MAGIC_ENUM_USING_ALIAS_STRING using string = my_lib::String;
  #define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = my_lib::StringView;
  #define MAGIC_ENUM_USING_ALIAS_OPTIONAL template <typename T> using optional = my_lib::Optional<T>;
  #include <magic_enum/magic_enum.hpp>
  ```

  The string aliases must have matching `value_type`s. `MAGIC_ENUM_USING_ALIAS_STRING` must name a default- and move-constructible owning string supporting `empty()`, `size()`, `data()`, `reserve()`, `append(count, character)`, and `append(data, size)`; return values are ignored.

* To keep configuration in separate header, define `MAGIC_ENUM_CONFIG_FILE`:

  ```cpp
  #define MAGIC_ENUM_CONFIG_FILE "my_magic_enum_cfg.hpp"
  #include <magic_enum/magic_enum.hpp>
  ```

  Configuration header can contain these aliases and range macros.

* Define all configuration macros consistently before the first magic_enum include in every translation unit and module BMI.

## `enum_cast`

```cpp
template <typename E>
constexpr optional<E> enum_cast(underlying_type_t<E> value) noexcept;

template <typename E>
constexpr optional<E> enum_cast(string_view value) noexcept;

template <typename E, typename BinaryPredicate>
constexpr optional<E> enum_cast(string_view value, BinaryPredicate p);
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns enum value from string or integer.

* Returns empty `optional<E>` if no value matches.

* `case_insensitive` provides ASCII case-insensitive matching for `enum_cast`, `enum_contains`, `enum_flags_cast`, and `enum_flags_contains`.

* Examples

  * String to enum value.

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

  * Integer to enum value.

    ```cpp
    int color_integer = 0;
    auto color = magic_enum::enum_cast<Color>(color_integer);
    if (color.has_value()) {
        // color.value() -> Color::BLUE
    }

    auto color_or_default = magic_enum::enum_cast<Color>(123).value_or(Color::RED);
    ```

## `enum_value`

```cpp
template <typename E>
constexpr E enum_value(size_t index) noexcept;

template <typename E, size_t I>
constexpr E enum_value() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns enum value at specified index.
  * `enum_value<E>(index)` performs no bounds checking: behavior is undefined if `index >= enum_count<E>()`.
  * `enum_value<E, I>()` produces compilation error if `I >= enum_count<E>()`.

* Examples

  ```cpp
  int i = 1;
  Color color = magic_enum::enum_value<Color>(i);
  // color -> Color::BLUE
  ```

  ```cpp
  Color color = magic_enum::enum_value<Color, 1>();
  // color -> Color::BLUE
  ```

## `enum_values`

```cpp
template <typename E>
constexpr array<E, N> enum_values() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns `array<E, N>` containing all enum values sorted by value, where `N = enum_count<E>()`.

* Examples

  ```cpp
  constexpr auto colors = magic_enum::enum_values<Color>();
  // colors -> {Color::RED, Color::BLUE, Color::GREEN}
  // colors[0] -> Color::RED
  ```

## `enum_count`

```cpp
template <typename E>
constexpr size_t enum_count() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns number of enum values.

* Examples

  ```cpp
  constexpr auto color_count = magic_enum::enum_count<Color>();
  // color_count -> 3
  ```

## `enum_integer` and `enum_underlying`

```cpp
template <typename E>
constexpr underlying_type_t<E> enum_integer(E value) noexcept;

template <typename E>
constexpr underlying_type_t<E> enum_underlying(E value) noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns underlying integer value of enum value.

* Examples

  ```cpp
  Color color = Color::RED;
  auto color_integer = magic_enum::enum_integer(color);
  // color_integer -> -10
  ```

## `enum_name`

```cpp
template <typename E>
constexpr string_view enum_name(E value) noexcept;

template <auto V>
constexpr string_view enum_name() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns enum name as null-terminated `string_view`.

* `enum_name(value)` returns empty string for unnamed or [out-of-range](limitations.md) value. `enum_name<value>()` produces compilation error for unnamed value.

* `enum_name<value>()` compiles faster than `enum_name(value)` and is not restricted by `enum_range` [limitation](limitations.md).

* Examples

  ```cpp
  Color color = Color::RED;
  auto color_name = magic_enum::enum_name(color);
  // color_name -> "RED"
  ```

  ```cpp
  constexpr Color color = Color::BLUE;
  constexpr auto color_name = magic_enum::enum_name<color>();
  // color_name -> "BLUE"
  ```

## `enum_names`

```cpp
template <typename E>
constexpr array<string_view, N> enum_names() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns `array<string_view, N>` containing all enum names sorted by value, where `N = enum_count<E>()`.

* Examples

  ```cpp
  constexpr auto color_names = magic_enum::enum_names<Color>();
  // color_names -> {"RED", "BLUE", "GREEN"}
  // color_names[0] -> "RED"
  ```

## `enum_entries`

```cpp
template <typename E>
constexpr array<pair<E, string_view>, N> enum_entries() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns `array<pair<E, string_view>, N>` containing all enum value and name pairs sorted by value, where `N = enum_count<E>()`.

* Examples

  ```cpp
  constexpr auto color_entries = magic_enum::enum_entries<Color>();
  // color_entries -> {{Color::RED, "RED"}, {Color::BLUE, "BLUE"}, {Color::GREEN, "GREEN"}}
  // color_entries[0].first -> Color::RED
  // color_entries[0].second -> "RED"
  ```

## `customize::enum_range`

```cpp
namespace magic_enum::customize {
template <typename E, typename = void>
struct enum_range {
  static constexpr int min = MAGIC_ENUM_RANGE_MIN;
  static constexpr int max = MAGIC_ENUM_RANGE_MAX;
};
} // namespace magic_enum::customize

```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Customization point for controlling `magic_enum` defaults.

* `is_flags` enables flag semantics. If omitted, defaults to `false`.

* `prefix_length` sets number of characters removed from start of each reflected enumerator name. If omitted, defaults to `0`.

* `min` and `max` set compiler-specific scan bounds for non-flag enums. Standard reflection and enum flags ignore them.

* `as_flags<>` and `as_common<>` force subtype for individual API calls without changing `enum_range`.

* Examples

  * Controlling prefix length

    ```cpp
    enum CStyleAnimals {
      CStyleAnimals_Giraffe,
      CStyleAnimals_Elephant,
      CStyleAnimals_Lion,
    };

    template <>
    struct magic_enum::customize::enum_range<CStyleAnimals> {
      static constexpr auto prefix_length = sizeof("CStyleAnimals_") - 1;
      static constexpr int min = CStyleAnimals_Giraffe;
      static constexpr int max = CStyleAnimals_Lion;
    };

    CStyleAnimals animal = CStyleAnimals_Giraffe;
    auto animal_name = magic_enum::enum_name(animal);
    // animal_name => "Giraffe"
    auto animal_from_string = magic_enum::enum_cast<CStyleAnimals>(animal_name);
    // animal_from_string.value() == CStyleAnimals_Giraffe
    ```

  * ADL customization

    For ADL customization, define `magic_enum_define_range_adl(my_enum_type)` in associated namespace or as friend of associated class. Return `magic_enum::customize::adl_info()`:

    ```cpp
    namespace Deeply::Nested::Namespace {
      enum class my_enum_type { my_enum_value1 = 10, my_enum_value2 = 11 };

      auto magic_enum_define_range_adl(my_enum_type) {
        return magic_enum::customize::adl_info()
            .minmax<10, 11>()
            .prefix<sizeof("my_enum_") - 1>();
      }
    }
    ```

    If both an explicit `customize::enum_range<E>` specialization and an ADL `magic_enum_define_range_adl(E)` function are present, the explicit specialization is used and the ADL customization is ignored.

    For flag enums, add `.flag<true>()`; `.minmax<...>()` is ignored.

## `enum_index`

```cpp
template <typename E>
constexpr optional<size_t> enum_index(E value) noexcept;

template <auto V>
constexpr size_t enum_index() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns enum index in `enum_values<E>()`. Runtime overload returns empty `optional` for invalid value; compile-time overload produces compilation error.

* Examples

  ```cpp
  constexpr auto color_index = magic_enum::enum_index(Color::BLUE);
  // color_index.value() -> 1
  // color_index.has_value() -> true
  ```

  ```cpp
  constexpr auto color_index = magic_enum::enum_index<Color::BLUE>();
  // color_index -> 1
  ```

## `enum_contains`

```cpp
template <typename E>
constexpr bool enum_contains(E value) noexcept;

template <typename E>
constexpr bool enum_contains(underlying_type_t<E> value) noexcept;

template <typename E>
constexpr bool enum_contains(string_view value) noexcept;

template <typename E, typename BinaryPredicate>
constexpr bool enum_contains(string_view value, BinaryPredicate p);
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns `true` if enum contains specified value or name.

* Examples

  ```cpp
  magic_enum::enum_contains(Color::GREEN); // -> true
  magic_enum::enum_contains<Color>(0); // -> true
  magic_enum::enum_contains<Color>(123); // -> false
  magic_enum::enum_contains<Color>("GREEN"); // -> true
  magic_enum::enum_contains<Color>("fda"); // -> false
  ```

## `enum_reflected`

```cpp
template <typename E>
constexpr bool enum_reflected(E value) noexcept;

template <typename E>
constexpr bool enum_reflected(underlying_type_t<E> value) noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* With standard reflection, returns `true` for a declared enumerator.

* With compiler-specific reflection, returns `true` for a value in the configured range.

* For flags, the value must be non-zero and single-bit.

## `enum_type_name`

```cpp
template <typename E>
constexpr string_view enum_type_name() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns enum type name as null-terminated `string_view`.

* Examples

  ```cpp
  Color color = Color::RED;
  auto type_name = magic_enum::enum_type_name<decltype(color)>();
  // type_name -> "Color"
  ```

## `enum_fuse`

```cpp
template <typename... Es>
constexpr optional<enum_fuse_t> enum_fuse(Es... values) noexcept;
```

* Defined in header `<magic_enum/magic_enum_fuse.hpp>`

* Combines several enum values for multidimensional switch/case statements.

* On MSVC, suppress warning C4064 with `/wd4064` or define `MAGIC_ENUM_NO_TYPESAFE_ENUM_FUSE` to use `uintmax_t` instead.

* Examples

  ```cpp
  switch (magic_enum::enum_fuse(color, direction).value()) {
    case magic_enum::enum_fuse(Color::RED, Directions::Up).value(): // ...
    case magic_enum::enum_fuse(Color::BLUE, Directions::Down).value(): // ...
    case magic_enum::enum_fuse(Directions::Up, Color::RED).value(): // Compilation error
  // ...
  }
  ```

## `enum_switch`

```cpp
template <typename E, typename Lambda>
constexpr decltype(auto) enum_switch(Lambda&& lambda, E value);

template <typename Result, typename E, typename Lambda>
constexpr decltype(auto) enum_switch(Lambda&& lambda, E value);

template <typename Result, typename E, typename Lambda>
constexpr decltype(auto) enum_switch(Lambda&& lambda, E value, Result&& result);
```

* Defined in header `<magic_enum/magic_enum_switch.hpp>`

* Calls callable with `enum_constant<V>` for matching reflected value.

* If no value matches, returns default-constructed result or supplied `result`.

* Examples

  ```cpp
  Color color = Color::RED;

  magic_enum::enum_switch([](auto val) {
    constexpr Color c_color = val;
    // ...
  }, color);
  ```

## `enum_for_each`

```cpp
template <typename E, typename Lambda>
constexpr auto enum_for_each(Lambda&& lambda);
```

* Defined in header `<magic_enum/magic_enum_utility.hpp>`

* Calls callable with `enum_constant<V>` for each reflected value.

* Returns `void`, array for equal result types, or tuple for different result types.

* Examples

  ```cpp
  magic_enum::underlying_type_t<Color> sum{};
  magic_enum::enum_for_each<Color>([&sum](auto val) {
    constexpr auto v = magic_enum::enum_integer(val());
    sum += v;
  });
  ```

## `enum_next_value` and `enum_prev_value`

```cpp
template <typename E>
constexpr optional<E> enum_next_value(E value, std::ptrdiff_t n = 1) noexcept;

template <typename E>
constexpr E enum_next_value_circular(E value, std::ptrdiff_t n = 1) noexcept;

template <typename E>
constexpr optional<E> enum_prev_value(E value, std::ptrdiff_t n = 1) noexcept;

template <typename E>
constexpr E enum_prev_value_circular(E value, std::ptrdiff_t n = 1) noexcept;
```

* Defined in header `<magic_enum/magic_enum_utility.hpp>`

* Moves by `n` positions in `enum_values<E>()` order; negative `n` reverses direction.

* Non-circular functions return empty `optional` for invalid input or out-of-range result. Circular functions wrap and require reflected input value.

  ```cpp
  magic_enum::enum_next_value(Color::RED);           // -> optional containing Color::BLUE
  magic_enum::enum_prev_value_circular(Color::RED);  // -> Color::GREEN
  ```

## `enum_flags`

```cpp
template <typename E>
string enum_flags_name(E value, char_type sep = '|');

template <typename E>
constexpr optional<E> enum_flags_cast(underlying_type_t<E> value) noexcept;

template <typename E>
constexpr optional<E> enum_flags_cast(string_view value, char_type sep = '|');

template <typename E, typename BinaryPredicate>
constexpr optional<E> enum_flags_cast(string_view value, char_type sep, BinaryPredicate p);

template <typename E>
constexpr bool enum_flags_contains(E value) noexcept;

template <typename E>
constexpr bool enum_flags_contains(underlying_type_t<E> value) noexcept;

template <typename E>
constexpr bool enum_flags_contains(string_view value, char_type sep = '|');

template <typename E, typename BinaryPredicate>
constexpr bool enum_flags_contains(string_view value, char_type sep, BinaryPredicate p);

template <typename E>
constexpr bool enum_flags_test(E flags, E flag) noexcept;

template <typename E>
constexpr bool enum_flags_test_any(E lhs, E rhs) noexcept;
```

* Defined in header `<magic_enum/magic_enum_flags.hpp>`

* `enum_flags_name` - Returns name of flag enum value with custom separator (default `'|'`).

* `enum_flags_cast` - Returns flag enum value from integer or string.

* `enum_flags_contains` - Checks whether flag enum contains specified value or name.

* `enum_flags_test` - Checks whether `flags` contains `flag`. Returns `false` if `flag` equals `0`.

* `enum_flags_test_any` - Checks whether `lhs` and `rhs` share any flags. Returns `false` if either value equals `0`.

* Set `enum_range<E>::is_flags` to `true` to use flag semantics for `E` by default. `enum_flags_*` APIs always use flag semantics.
  ```cpp
  enum class Directions { Up = 1 << 1, Down = 1 << 2, Right = 1 << 3, Left = 1 << 4 };
  template <>
  struct magic_enum::customize::enum_range<Directions> {
    static constexpr bool is_flags = true;
  };
  ```

  * `MAGIC_ENUM_RANGE_MIN` / `MAGIC_ENUM_RANGE_MAX` do not control flag reflection.

  * Zero is not reflected for flag enums.

* Examples

  ```cpp
  enum Directions : std::uint64_t {
    Left = 1,
    Down = 2,
    Up = 4,
    Right = 8,
    LeftAndDown = 3
  };
  template <>
  struct magic_enum::customize::enum_range<Directions> {
    static constexpr bool is_flags = true;
  };
  using namespace magic_enum::bitwise_operators; // Use with care; operators are enabled for all enums.

  magic_enum::enum_flags_name(Directions::Up | Directions::Right); // -> "Up|Right"
  magic_enum::enum_flags_name(Directions::LeftAndDown); // -> "Left|Down"
  magic_enum::enum_flags_name(Directions::Up | Directions::Right, ','); // -> "Up,Right"

  magic_enum::enum_flags_contains(Directions::Up | Directions::Right); // -> true
  magic_enum::enum_flags_contains(Directions::LeftAndDown); // -> true

  magic_enum::enum_flags_cast<Directions>(3).value(); // -> Directions::Left|Directions::Down
  magic_enum::enum_flags_cast<Directions>("Left|Down").value(); // -> Directions::Left|Directions::Down
  magic_enum::enum_flags_cast<Directions>("Left,Down", ',').value(); // -> Directions::Left|Directions::Down

  magic_enum::enum_flags_test(Directions::Left | Directions::Down, Directions::Down); // -> true
  magic_enum::enum_flags_test(Directions::Left | Directions::Down, Directions::Right); // -> false

  magic_enum::enum_flags_test_any(Directions::Left | Directions::Down | Directions::Right, Directions::Down | Directions::Right); // -> true
  ```

## `is_flags_enum`

```cpp
template <typename T>
struct is_flags_enum;

template <typename T>
inline constexpr bool is_flags_v = is_flags_enum<T>::value;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Checks whether `T` is configured to use flag semantics by default.

## `is_unscoped_enum`

```cpp
template <typename T>
struct is_unscoped_enum;

template <typename T>
inline constexpr bool is_unscoped_enum_v = is_unscoped_enum<T>::value;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* `value` is `true` for [unscoped enum](https://en.cppreference.com/w/cpp/language/enum#Unscoped_enumeration) types and `false` otherwise.

* Examples

  ```cpp
  magic_enum::is_unscoped_enum<color>::value -> true
  magic_enum::is_unscoped_enum<Direction>::value -> false

  // Helper variable template.
  magic_enum::is_unscoped_enum_v<color> -> true
  ```

## `is_scoped_enum`

```cpp
template <typename T>
struct is_scoped_enum;

template <typename T>
inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* `value` is `true` for [scoped enum](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations) types and `false` otherwise.

* Examples

  ```cpp
  magic_enum::is_scoped_enum<color>::value -> false
  magic_enum::is_scoped_enum<Direction>::value -> true

  // Helper variable template.
  magic_enum::is_scoped_enum_v<Direction> -> true
  ```

## `underlying_type`

```cpp
template <typename T>
struct underlying_type;

template <typename T>
using underlying_type_t = typename underlying_type<T>::type;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Provides underlying type for complete enums and no `type` for non-enums. Incomplete enums are ill-formed.

* Examples

  ```cpp
  magic_enum::underlying_type<color>::type -> int

  // Helper types.
  magic_enum::underlying_type_t<Direction> -> int
  ```

## `ostream_operators`

```cpp
template <typename Char, typename Traits, typename E>
basic_ostream<Char, Traits>& operator<<(basic_ostream<Char, Traits>& os, E value);

template <typename Char, typename Traits, typename E>
basic_ostream<Char, Traits>& operator<<(basic_ostream<Char, Traits>& os, optional<E> value);
```

* Defined in header `<magic_enum/magic_enum_iostream.hpp>`

* Provides stream insertion operators for all enums.

* Examples

  ```cpp
  using magic_enum::iostream_operators::operator<<; // out-of-the-box ostream operators for enums.
  Color color = Color::BLUE;
  std::cout << color << std::endl; // "BLUE"
  ```

## `istream_operators`

```cpp
template <typename Char, typename Traits, typename E>
basic_istream<Char, Traits>& operator>>(basic_istream<Char, Traits>& is, E& value);
```

* Defined in header `<magic_enum/magic_enum_iostream.hpp>`

* Provides stream extraction operators for all enums.

* Examples

  ```cpp
  using magic_enum::iostream_operators::operator>>; // out-of-the-box istream operators for enums.
  Color color;
  std::cin >> color;
  ```

## `bitwise_operators`

```cpp
template <typename E>
constexpr E operator~(E rhs) noexcept;

template <typename E>
constexpr E operator|(E lhs, E rhs) noexcept;

template <typename E>
constexpr E operator&(E lhs, E rhs) noexcept;

template <typename E>
constexpr E operator^(E lhs, E rhs) noexcept;

template <typename E>
constexpr E& operator|=(E& lhs, E rhs) noexcept;

template <typename E>
constexpr E& operator&=(E& lhs, E rhs) noexcept;

template <typename E>
constexpr E& operator^=(E& lhs, E rhs) noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Provides bitwise operators for all enums.

* Examples

  ```cpp
  enum class Flags { A = 1 << 0, B = 1 << 1, C = 1 << 2, D = 1 << 3 };
  using namespace magic_enum::bitwise_operators; // Use with care; operators are enabled for all enums.
  // Support operators: ~, |, &, ^, |=, &=, ^=.
  Flags flags = Flags::A | (Flags::B & ~Flags::C);
  ```

## Formatting

* Defined in header `<magic_enum/magic_enum_format.hpp>`

* Provides `std::formatter` when `std::format` is supported, and `fmt::formatter` when `{fmt}` is included before `magic_enum_format.hpp`.

* Formats reflected enums by name, flag combinations with `|`, and unreflected values by their underlying integer values.

  ```cpp
  #include <format>
  #include <magic_enum/magic_enum_format.hpp>

  std::format("{}", Color::RED); // -> "RED"
  std::format("{}", Color{42});  // -> "42"
  ```

  ```cpp
  #include <fmt/format.h>
  #include <magic_enum/magic_enum_format.hpp>

  fmt::format("{}", Color::BLUE); // -> "BLUE"
  ```

## Container helpers

* Defined in header `<magic_enum/magic_enum_containers.hpp>`

* `name_less<>`, `name_greater<>`, `name_less_case_insensitive`, and `name_greater_case_insensitive` order enum values by name and support lookup by name.

* `default_indexing<E>` uses `enum_values<E>()` order. `comparator_indexing<Cmp>` provides comparator-based positions for `containers::array`.

* Custom `Index` must provide non-throwing `constexpr` `at(E)` that maps each reflected value to a unique index in `[0, enum_count<E>())`.

* For `containers::bitset`, custom `Index` must also provide non-throwing `constexpr` `it(std::size_t)` that maps each index back to its value.

## `containers::array`

```cpp
template <typename E, typename V, typename Index = default_indexing<E>>
struct array {

  constexpr reference at(E pos);

  constexpr const_reference at(E pos) const;

  constexpr reference operator[](E pos);

  constexpr const_reference operator[](E pos) const;

  constexpr reference front() noexcept;

  constexpr const_reference front() const noexcept;

  constexpr reference back() noexcept;

  constexpr const_reference back() const noexcept;

  constexpr pointer data() noexcept;

  constexpr const_pointer data() const noexcept;

  constexpr iterator begin() noexcept;

  constexpr const_iterator begin() const noexcept;

  constexpr const_iterator cbegin() const noexcept;

  constexpr iterator end() noexcept;

  constexpr const_iterator end() const noexcept;

  constexpr const_iterator cend() const noexcept;

  constexpr reverse_iterator rbegin() noexcept;

  constexpr const_reverse_iterator rbegin() const noexcept;

  constexpr const_reverse_iterator crbegin() const noexcept;

  constexpr reverse_iterator rend() noexcept;

  constexpr const_reverse_iterator rend() const noexcept;

  constexpr const_reverse_iterator crend() const noexcept;

  constexpr bool empty() const noexcept;

  constexpr size_type size() const noexcept;

  constexpr size_type max_size() const noexcept;

  constexpr void fill(const V& value);

  constexpr void swap(array& other);

  friend constexpr bool operator==(const array& a1, const array& a2);

  friend constexpr bool operator!=(const array& a1, const array& a2);

  friend constexpr bool operator<(const array& a1, const array& a2);

  friend constexpr bool operator<=(const array& a1, const array& a2);

  friend constexpr bool operator>(const array& a1, const array& a2);

  friend constexpr bool operator>=(const array& a1, const array& a2);
};
```

* Defined in header `<magic_enum/magic_enum_containers.hpp>`

* Fixed-size array indexed by reflected enum values.

* `make_array<E>(values...)` deduces common element type and assigns arguments in `enum_values<E>()` order. Number of arguments must equal `enum_count<E>()`.

* `to_array<E>(array)` converts built-in array whose size equals `enum_count<E>()`.

* Examples

  ```cpp
  constexpr auto color_rgb_array = magic_enum::containers::make_array<Color>(RGB{255, 0, 0}, RGB{0, 255, 0}, RGB{0, 0, 255});
  ```

  ```cpp
  magic_enum::containers::array<Color, RGB> color_rgb_array {};
  color_rgb_array[Color::RED] = {255, 0, 0};
  color_rgb_array[Color::GREEN] = {0, 255, 0};
  color_rgb_array[Color::BLUE] = {0, 0, 255};
  magic_enum::containers::get<Color::BLUE>(color_rgb_array); // -> RGB{0, 0, 255}
  ```

## `containers::bitset`

```cpp
template <typename E, typename Index = default_indexing<E>>
class bitset {

  constexpr explicit bitset(detail::raw_access_t = raw_access) noexcept;

  constexpr explicit bitset(detail::raw_access_t, unsigned long long val);

  constexpr explicit bitset(detail::raw_access_t, string_view sv, string_view::size_type pos = 0, string_view::size_type n = string_view::npos, char_type zero = '0', char_type one = '1');

  constexpr explicit bitset(detail::raw_access_t, const char_type* str, std::size_t n = ~std::size_t{}, char_type zero = '0', char_type one = '1');

  constexpr bitset(std::initializer_list<E> starters);

  constexpr explicit bitset(E starter);

  template <typename Cmp = std::equal_to<>>
  constexpr explicit bitset(string_view sv, Cmp&& cmp = {}, char_type sep = '|');

  friend constexpr bool operator==(const bitset& lhs, const bitset& rhs) noexcept;

  friend constexpr bool operator!=(const bitset& lhs, const bitset& rhs) noexcept;

  constexpr bool operator[](E pos) const;

  constexpr reference operator[](E pos);

  constexpr iterator begin() noexcept;

  constexpr const_iterator begin() const noexcept;

  constexpr const_iterator cbegin() const noexcept;

  constexpr iterator end() noexcept;

  constexpr const_iterator end() const noexcept;

  constexpr const_iterator cend() const noexcept;

  constexpr iterator find(E pos) noexcept;

  constexpr const_iterator find(E pos) const noexcept;

  constexpr bool test(E pos) const;

  constexpr bool all() const noexcept;

  constexpr bool any() const noexcept;

  constexpr bool none() const noexcept;

  constexpr std::size_t count() const noexcept;

  constexpr std::size_t size() const noexcept;

  constexpr std::size_t max_size() const noexcept;

  constexpr bitset& operator&=(const bitset& other) noexcept;

  constexpr bitset& operator|=(const bitset& other) noexcept;

  constexpr bitset& operator^=(const bitset& other) noexcept;

  constexpr bitset operator~() const noexcept;

  constexpr bitset& set() noexcept;

  constexpr bitset& set(E pos, bool value = true);

  constexpr bitset& reset() noexcept;

  constexpr bitset& reset(E pos);

  constexpr bitset& flip() noexcept;

  friend constexpr bitset operator&(const bitset& lhs, const bitset& rhs) noexcept;

  friend constexpr bitset operator|(const bitset& lhs, const bitset& rhs) noexcept;

  friend constexpr bitset operator^(const bitset& lhs, const bitset& rhs) noexcept;

  constexpr explicit operator E() const;

  string to_string(char_type sep = '|') const;

  string to_string(detail::raw_access_t, char_type zero = '0', char_type one = '1') const;

  constexpr unsigned long long to_ullong(detail::raw_access_t raw) const;

  constexpr unsigned long to_ulong(detail::raw_access_t raw) const;

  template <typename Traits>
  friend std::basic_ostream<char_type, Traits>& operator<<(std::basic_ostream<char_type, Traits>& o, const bitset& bs);

  template <typename Traits>
  friend std::basic_istream<char_type, Traits>& operator>>(std::basic_istream<char_type, Traits>& i, bitset& bs);
};
```

* Defined in header `<magic_enum/magic_enum_containers.hpp>`

* Fixed-size bitset indexed by reflected enum values.

* With `raw_access`, bit `I` corresponds to `enum_values<E>()[I]` for default indexing and to `*Index::it(I)` for custom indexing; this mapping is independent of underlying enum values.

* Construction from single `E` value and conversion to `E` are available only for flag enums.

* Provides `std::hash` specialization for use in unordered containers.

* Examples

  ```cpp
  constexpr magic_enum::containers::bitset<Color> color_bitset_red_green {Color::RED, Color::GREEN};
  bool all = color_bitset_red_green.all();
  // all -> false
  // Color::BLUE is missing
  bool test = color_bitset_red_green.test(Color::RED);
  // test -> true
  ```

  ```cpp
  auto color_bitset = magic_enum::containers::bitset<Color>();
  color_bitset.set(Color::GREEN);
  color_bitset.set(Color::BLUE);
  std::string to_string = color_bitset.to_string();
  // to_string -> "GREEN|BLUE"
  ```

  ```cpp
  std::uint8_t incoming = 0b00000011;
  auto color_bitset = magic_enum::containers::bitset<Color> {magic_enum::containers::raw_access, incoming};
  bool has_red = color_bitset.test(Color::RED);
  bool has_green = color_bitset.test(Color::GREEN);
  bool has_blue = color_bitset.test(Color::BLUE);

  color_bitset.set(Color::BLUE);
  auto raw_value = color_bitset.to_ulong(magic_enum::containers::raw_access);
  ```

## `containers::set`

```cpp
template <typename E, typename Cmp = std::less<E>>
class set {

  constexpr set() noexcept = default;

  template <typename InputIt>
  constexpr set(InputIt first, InputIt last);

  constexpr set(std::initializer_list<E> ilist);

  constexpr explicit set(E starter);

  constexpr set(const set&) noexcept = default;

  constexpr set(set&&) noexcept = default;

  constexpr set& operator=(const set&) noexcept = default;

  constexpr set& operator=(set&&) noexcept = default;

  constexpr set& operator=(std::initializer_list<E> ilist);

  constexpr const_iterator begin() const noexcept;

  constexpr const_iterator end() const noexcept;

  constexpr const_iterator cbegin() const noexcept;

  constexpr const_iterator cend() const noexcept;

  constexpr const_reverse_iterator rbegin() const noexcept;

  constexpr const_reverse_iterator rend() const noexcept;

  constexpr const_reverse_iterator crbegin() const noexcept;

  constexpr const_reverse_iterator crend() const noexcept;

  constexpr bool empty() const noexcept;

  constexpr size_type size() const noexcept;

  constexpr size_type max_size() const noexcept;

  constexpr void clear() noexcept;

  constexpr std::pair<iterator, bool> insert(const value_type& value) noexcept;

  constexpr std::pair<iterator, bool> insert(value_type&& value) noexcept;

  constexpr iterator insert(const_iterator, const value_type& value) noexcept;

  constexpr iterator insert(const_iterator hint, value_type&& value) noexcept;

  template <typename InputIt>
  constexpr void insert(InputIt first, InputIt last);

  constexpr void insert(std::initializer_list<value_type> ilist) noexcept;

  template <typename... Args>
  constexpr std::pair<iterator, bool> emplace(Args&&... args);

  template <typename... Args>
  constexpr iterator emplace_hint(const_iterator, Args&&... args);

  constexpr iterator erase(const_iterator pos) noexcept;

  constexpr iterator erase(const_iterator first, const_iterator last) noexcept;

  constexpr size_type erase(const key_type& key) noexcept;

  template <typename K, typename KC = key_compare>
  constexpr std::enable_if_t<detail::is_transparent_v<KC>, size_type> erase(K&& x);

  void swap(set& other) noexcept;

  constexpr size_type count(const key_type& key) const noexcept;

  template <typename K, typename KC = key_compare>
  constexpr std::enable_if_t<detail::is_transparent_v<KC>, size_type> count(const K& x) const;

  constexpr const_iterator find(const key_type& key) const noexcept;

  template <typename K, typename KC = key_compare>
  constexpr std::enable_if_t<detail::is_transparent_v<KC>, const_iterator> find(const K& x) const;

  constexpr bool contains(const key_type& key) const noexcept;

  template <typename K, typename KC = key_compare>
  constexpr std::enable_if_t<detail::is_transparent_v<KC>, bool> contains(const K& x) const;

  constexpr std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const;

  template <typename K, typename KC = key_compare>
  constexpr std::enable_if_t<detail::is_transparent_v<KC>, std::pair<const_iterator, const_iterator>> equal_range(const K& x) const;

  constexpr const_iterator lower_bound(const key_type& key) const;

  template <typename K, typename KC = key_compare>
  constexpr std::enable_if_t<detail::is_transparent_v<KC>, const_iterator> lower_bound(const K& x) const;

  constexpr const_iterator upper_bound(const key_type& key) const;

  template <typename K, typename KC = key_compare>
  constexpr std::enable_if_t<detail::is_transparent_v<KC>, const_iterator> upper_bound(const K& x) const;

  constexpr key_compare key_comp() const;

  constexpr value_compare value_comp() const;

  constexpr friend bool operator==(const set& lhs, const set& rhs) noexcept;

  constexpr friend bool operator!=(const set& lhs, const set& rhs) noexcept;

  constexpr friend bool operator<(const set& lhs, const set& rhs);

  constexpr friend bool operator<=(const set& lhs, const set& rhs);

  constexpr friend bool operator>(const set& lhs, const set& rhs);

  constexpr friend bool operator>=(const set& lhs, const set& rhs);

  template <typename Pred>
  size_type erase_if(Pred pred);
};
```

* Defined in header `<magic_enum/magic_enum_containers.hpp>`

* Fixed-capacity set of reflected enum values. Comparator is default-constructed and must work in constant expressions.

* Distinct enum values remain distinct even if comparator treats them as equivalent. Lookup with another key type can match multiple values.

* Construction from single `E` value is available only for flag enums.

* Examples

  ```cpp
  constexpr magic_enum::containers::set color_set_filled = {Color::RED, Color::GREEN, Color::BLUE};
  ```

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

  ```cpp
  using name_set = magic_enum::containers::set<Color, magic_enum::containers::name_less<>>;
  name_set color_set {Color::RED, Color::GREEN, Color::BLUE};

  bool has_green = color_set.contains(std::string_view{"GREEN"});
  auto it = color_set.find(std::string_view{"GREEN"});
  auto range = color_set.equal_range(std::string_view{"GREEN"});
  ```
