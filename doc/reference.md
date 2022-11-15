# Reference

* [`enum_cast` obtains enum value from string or integer.](#enum_cast)
* [`enum_value` returns enum value at specified index.](#enum_value)
* [`enum_values` obtains enum value sequence.](#enum_values)
* [`enum_count` returns number of enum values.](#enum_count)
* [`enum_integer` obtains integer value from enum value.](#enum_integer)
* [`enum_name` returns name from enum value.](#enum_name)
* [`enum_names` obtains string enum name sequence.](#enum_names)
* [`enum_entries` obtains pair (value enum, string enum name) sequence.](#enum_entries)
* [`enum_index` obtains index in enum value sequence from enum value.](#enum_index)
* [`enum_contains` checks whether enum contains enumerator with such value.](#enum_contains)
* [`enum_type_name` returns type name of enum.](#enum_type_name)
* [`enum_fuse` returns a bijective mix of enum values.](#enum_fuse)
* [`enum_switch` allows runtime enum value transformation to constexpr context.](#enum_switch)
* [`enum_for_each` calls a function with all enum constexpr value.](#enum_for_each)
* [`enum_flags` API from enum-flags.](#enum_flags)
* [`is_unscoped_enum` checks whether type is an Unscoped enumeration.](#is_unscoped_enum)
* [`is_scoped_enum` checks whether type is an Scoped enumeration.](#is_scoped_enum)
* [`underlying_type` improved UB-free "SFINAE-friendly" underlying_type.](#underlying_type)
* [`ostream_operators` ostream operators for enums.](#ostream_operators)
* [`bitwise_operators` bitwise operators for enums.](#bitwise_operators)

## Synopsis

* Before use, read the [limitations](limitations.md) of functionality.

* To check is magic_enum supported compiler use macro `MAGIC_ENUM_SUPPORTED` or constexpr constant `magic_enum::is_magic_enum_supported`.</br>
  If magic_enum used on unsupported compiler, occurs the compilation error. To suppress error define macro `MAGIC_ENUM_NO_CHECK_SUPPORT`.

* To add custom enum or type names see the [example](../example/example_custom_name.cpp).

* To change the type of strings or ortional, use special macros:

  ```cpp
  #include <my_lib/string.hpp>
  #include <my_lib/string_view.hpp>
  #define MAGIC_ENUM_USING_ALIAS_STRING using string = my_lib::String;
  #define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = my_lib::StringView;
  #define MAGIC_ENUM_USING_ALIAS_OPTIONAL template <typename T> using optional = my_lib::Optional<T>;
  #include <magic_enum.hpp>
  ```

* Optionally define `MAGIC_ENUM_CONFIG_FILE` i.e., in your build system, with path to header file with defined
  macros or constants, for example:
  
  ```cpp
  #define MAGIC_ENUM_CONFIG_FILE "my_magic_enum_cfg.hpp"
  ```
  my_magic_enum_cfg.hpp:
  ```cpp
  #include <my_lib/string.hpp>
  #include <my_lib/string_view.hpp>
  #define MAGIC_ENUM_USING_ALIAS_STRING using string = my_lib::String;
  #define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = my_lib::StringView;
  #define MAGIC_ENUM_USING_ALIAS_OPTIONAL template <typename T> using optional = my_lib::Optional<T>;
  #define MAGIC_ENUM_RANGE_MIN 0
  #define MAGIC_ENUM_RANGE_MAX 255
  ```

* To add support for non-ASCII enumeration identifier, use special macros:
  ```cpp
  #define MAGIC_ENUM_ENABLE_NONASCII
  #include <magic_enum.hpp>
  ```

## `enum_cast`

```cpp
template <typename E>
constexpr optional<E> enum_cast(underlying_type_t<E> value) noexcept;

template <typename E>
constexpr optional<E> enum_cast(string_view value) noexcept;

template <typename E, typename BinaryPredicate>
constexpr optional<E> enum_cast(string_view value, BinaryPredicate p) noexcept(is_nothrow_invocable_v<BinaryPredicate>);
```

* Obtains enum value from string or integer.

* Returns `optional<E>`, using `has_value()` to check contains enum value and `value()` to get the enum value.

* If argument does not enum value, returns empty `optional`.

* Examples

  * String to enum value.

    ```cpp
    string color_name{"GREEN"};
    auto color = magic_enum::enum_cast<Color>(color_name);
    if (color.has_value()) {
        // color.value() -> Color::GREEN
    }

    auto color_or_default = magic_enum::enum_cast<Color>(value).value_or(Color::NONE);
    ```

  * Integer to enum value.

    ```cpp
    int color_integer = 2;
    auto color = magic_enum::enum_cast<Color>(color_integer);
    if (color.has_value()) {
        // color.value() -> Color::RED
    }

    auto color_or_default = magic_enum::enum_cast<Color>(value).value_or(Color::NONE);
    ```

## `enum_value`

```cpp
template <typename E>
constexpr E enum_value(size_t index) noexcept;

template <typename E, size_t I>
constexpr E enum_value() noexcept;
```

* Returns enum value at specified index.
  * `enum_value(value)` no bounds checking is performed: the behavior is undefined if `index >= number of enum values`.
  * `enum_value<value>()` check if `I >= number of enum values`, occurs the compilation error `magic_enum::enum_value out of range`.

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

* Returns `array<E, N>` with all enum values where `N = number of enum values`, sorted by enum value.

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

* Returns number of enum values.

* Examples

  ```cpp
  constexpr auto color_count = magic_enum::enum_count<Color>();
  // color_count -> 3
  ```

## `enum_integer`

```cpp
template <typename E>
constexpr underlying_type_t<E> enum_integer(E value) noexcept;

template <typename E>
constexpr underlying_type_t<E> enum_underlying(E value) noexcept;
```

* Returns integer value from enum value.

* Examples

  ```cpp
  Color color = Color::RED;
  auto color_integer = magic_enum::enum_integer(color);
  // color -> 2
  ```

## `enum_name`

```cpp
template <typename E>
constexpr string_view enum_name(E value) noexcept;

template <auto V>
constexpr string_view enum_name() noexcept;
```

* Returns name from enum value as `string_view` with null-terminated string.
  * If enum value does not have name or [out of range](limitations.md), `enum_name(value)` returns empty string.
  * If enum value does not have name, `enum_name<value>()` occurs the compilation error `magic_enum::enum_name enum value does not have a name`.

* `enum_name<value>()` is much lighter on the compile times and is not restricted to the enum_range [limitation](limitations.md).

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

* Returns `array<string_view, N>` with all names where `N = number of enum values`, sorted by enum value.

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

* Returns `array<pair<E, string_view>, N>` with all pairs (value, name) where `N = number of enum values`, sorted by enum value.

* Examples

  ```cpp
  constexpr auto color_entries = magic_enum::enum_entries<Color>();
  // color_entries -> {{Color::RED, "RED"}, {Color::BLUE, "BLUE"}, {Color::GREEN, "GREEN"}}
  // color_entries[0].first -> Color::RED
  // color_entries[0].second -> "RED"
  ```

## `enum_index`

```cpp
template <typename E>
constexpr optional<size_t> enum_index(E value) noexcept;

template <auto V>
constexpr size_t enum_index() noexcept;
```

* Obtains index in enum values from enum value.
  * `enum_index(value)` returns `optional<size_t>` with index.
  * `enum_index<value>()` returns index. If enum value does not have a index, occurs the compilation error `magic_enum::enum_index enum value does not have a index`.

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
constexpr optional<E> enum_contains(string_view value, BinaryPredicate p) noexcept(is_nothrow_invocable_v<BinaryPredicate>);
```

* Checks whether enum contains enumerator with such value.

* Returns true is enum contains value, otherwise false.

* Examples

  ```cpp
  magic_enum::enum_contains(Color::GREEN); // -> true
  magic_enum::enum_contains<Color>(2); // -> true
  magic_enum::enum_contains<Color>(123); // -> false
  magic_enum::enum_contains<Color>("GREEN"); // -> true
  magic_enum::enum_contains<Color>("fda"); // -> false
  ```

## `enum_type_name`

```cpp
template <typename E>
constexpr string_view enum_type_name() noexcept;
```

* Returns type name of enum as `string_view` null-terminated string.

* Examples

  ```cpp
  Color color = Color::RED;
  auto type_name = magic_enum::enum_type_name<decltype(color)>();
  // color_name -> "Color"
  ```

## `enum_fuse`

```cpp
template <typename... Es>
[[nodiscard]] constexpr optional<enum_fuse_t> enum_fuse(Es... values) noexcept;
```

* You should add the required file `<magic_enum_fuse.hpp>`.

* Returns a typesafe bijective mix of several enum values. This can be used to emulate 2D switch/case statements.

* Return type is `optional<enum_fuse_t>` where `enum_fuse_t` is an incomplete enum, it is unique for any given combination of `Es...`.

* Switch/case statement over an incomplete enum is a Visual Studio warning C4064
  * You have to silent (/wd4064) or ignore it.
  * Alternatively, define `MAGIC_ENUM_NO_TYPESAFE_ENUM_FUSE` to disable type-safety (`enum_fuse_t` equals `uintmax_t`).

* Examples

  ```cpp
  switch (magic_enum::enum_fuse(color, direction).value()) {
    case magic_enum::enum_fuse(Color::RED, Directions::Up).value(): // ...
    case magic_enum::enum_fuse(Color::BLUE, Directions::Down).value(): // ...
    case magic_enum::enum_fuse(Directions::BLUE, Color::Down).value(): // Compilation error
  // ...
  }
  ```

## `enum_switch`

```cpp
template <typename Result = void, typename E, typename Lambda>
constexpr Result enum_switch(Lambda&& lambda, E value);

template <typename Result, typename E, typename Lambda>
constexpr Result enum_switch(Lambda&& lambda, E value, Result&& result);
```

* You should add the required file `<magic_enum_switch.hpp>`.

* Examples

  ```cpp
  Color color = Color::RED;

  magic_enum::enum_switch([] (auto val) {
    constexpr Color c_color = val;
    // ...
  }, color);
  ```

## `enum_for_each`

```cpp
template <typename E, typename Lambda>
constexpr auto enum_for_each(Lambda&& lambda);
```

* Examples

  ```cpp
  magic_enum::enum_for_each<Color>([] (auto val) {
    constexpr Color c_color = val;
    // ...
  });
  ```

## `enum_flags`

```cpp
template <typename E>
string enum_flags_name(E value);

template <typename E>
constexpr optional<E> enum_flags_cast(underlying_type_t<E> value) noexcept;

template <typename E>
constexpr optional<E> enum_flags_cast(string_view value) noexcept;

template <typename E, typename BinaryPredicate>
constexpr optional<E> enum_flags_cast(string_view value, BinaryPredicate p) noexcept(is_nothrow_invocable_v<BinaryPredicate>);

template <typename E>
constexpr bool enum_flags_contains(E value) noexcept;

template <typename E>
constexpr bool enum_flags_contains(underlying_type_t<E> value) noexcept;

template <typename E>
constexpr bool enum_flags_contains(string_view value) noexcept;

template <typename E, typename BinaryPredicate>
constexpr optional<E> enum_flags_contains(string_view value, BinaryPredicate p) noexcept(is_nothrow_invocable_v<BinaryPredicate>);
```

* Examples

  ```cpp
  auto directions_name = magic_enum::enum_flags_name(Directions::Up | Directions::Right);
  // directions_name -> "Directions::Up | Directions::Right"
  ```


## `is_unscoped_enum`

```cpp
template <typename T>
struct is_unscoped_enum;

template <typename T>
inline constexpr bool is_unscoped_enum_v = is_unscoped_enum<T>::value;
```

* Checks whether type is an [Unscoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Unscoped_enumeration).

* Provides the member constant value which is equal to true, if T is an [Unscoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Unscoped_enumeration) type.</br>
  Otherwise, value is equal to false.

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

* Checks whether type is an [Scoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations).

* Provides the member constant value which is equal to true, if T is an [Scoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations) type.</br>
  Otherwise, value is equal to false.

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

* Improved UB-free "SFINAE-friendly" [underlying_type](https://en.cppreference.com/w/cpp/types/underlying_type).

* If T is a complete enumeration type, provides a member typedef type that names the underlying type of T.</br>
  Otherwise, if T is not an enumeration type, there is no member type.</br>
  Otherwise (T is an incomplete enumeration type), the program is ill-formed.

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

* Out-of-the-box ostream operators for all enums.

* Examples

  ```cpp
  using namespace magic_enum::ostream_operators; // out-of-the-box ostream operators for enums.
  Color color = Color::BLUE;
  std::cout << color << std::endl; // "BLUE"
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

* Out-of-the-box bitwise operators for all enums.

* Examples

  ```cpp
  enum class Flags { A = 1 << 0, B = 1 << 1, C = 1 << 2, D = 1 << 3 };
  using namespace magic_enum::bitwise_operators; // out-of-the-box bitwise operators for enums.
  // Support operators: ~, |, &, ^, |=, &=, ^=.
  Flags flags = Flags::A | Flags::B & ~Flags::C;
  ```
