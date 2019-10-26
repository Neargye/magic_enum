# Reference

* [`enum_cast` obtains enum value from string or integer.](#enum_cast)
* [`enum_value` returns enum value at specified index.](#enum_value)
* [`enum_values` obtains enum value sequence.](#enum_values)
* [`enum_count` returns number of enum values.](#enum_count)
* [`enum_integer` obtains integer value from enum value.](#enum_integer)
* [`enum_name` returns string name from enum value.](#enum_name)
* [`enum_names` obtains string enum name sequence.](#enum_names)
* [`enum_entries` obtains pair (value enum, string enum name) sequence.](#enum_entries)
* [`enum_index` obtains index in enum value sequence from enum value.](#enum_index)
* [`is_unscoped_enum` checks whether type is an Unscoped enumeration.](#is_unscoped_enum)
* [`is_scoped_enum` checks whether type is an Scoped enumeration.](#is_scoped_enum)
* [`underlying_type` improved UB-free "SFINAE-friendly" std::underlying_type.](#underlying_type)
* [`ostream_operators` ostream operators for enums.](#ostream_operators)
* [`bitwise_operators` bitwise operators for enums.](#bitwise_operators)

# Synopsis

* Before use, read the [limitations](limitations.md) of functionality.

* To check is magic_enum supported compiler use macro `MAGIC_ENUM_SUPPORTED` or constexpr constant `magic_enum::is_magic_enum_supported`.
If magic_enum used on unsupported compiler, occurs the compilation error. To suppress error define macro `MAGIC_ENUM_NO_CHECK_SUPPORT`.

## `enum_cast`
```cpp
template <typename E>
constexpr optional<E> enum_cast(string_view value) noexcept;

template <typename E>
constexpr optional<E> enum_cast(underlying_type_t<E> value) noexcept;
```

* Obtains enum value from string or integer.

* Returns `std::optional<E>`, using `has_value()` to check contains enum value and `value()` to get the enum value.

* If argument does not enum value, returns empty `std::optional`.

* Examples
    * String to enum value.
    ```cpp
    std::string color_name{"GREEN"};
    auto color = magic_enum::enum_cast<Color>(color_name);
    if (color.has_value()) {
        // color.value() -> Color::GREEN
    }
    ```
    * Integer to enum value.
    ```cpp
    int color_integer = 2;
    auto color = magic_enum::enum_cast<Color>(color_integer);
    if (colo.has_value()) {
        // color.value() -> Color::RED
    }
    ```

## `enum_value`
```cpp
template <typename E>
constexpr E enum_value(size_t index) noexcept;
```

* Returns enum value at specified index.

* No bounds checking is performed: the behavior is undefined if `index >= number of enum values`.

* Examples
  ```cpp
  int i = 1;
  Color color = magic_enum::enum_value<Color>(i);
  // color -> Color::BLUE
  ````

## `enum_values`
```cpp
template <typename E>
constexpr array<E, N> enum_values() noexcept;
```

* Returns `std::array<E, N>` with all enum value where `N = number of enum values`, sorted by enum value.

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
  constexpr std::size_t color_count = magic_enum::enum_count<Color>();
  // color_count -> 3
  ```

## `enum_integer`
```cpp
template <typename E>
constexpr underlying_type_t<E> enum_integer(E value) noexcept;
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

* Returns `std::string_view`. If enum value does not have name, returns empty string.

* Examples
    * Enum value to string.
    ```cpp
    Color color = Color::RED;
    auto color_name = magic_enum::enum_name(color);
    // color_name -> "RED"
    ```
    * Static storage enum variable to string.
    This version is much lighter on the compile times and is not restricted to the enum_range [limitation](doc/limitations.md).
    ```cpp
    constexpr Color color = Color::BLUE;
    constexpr auto color_name = magic_enum::enum_name<color>();
    // color_name -> "BLUE"
    ```

* `magic_enum::enum_name<value>()` is much lighter on the compile times and is not restricted to the enum_range [limitation](limitation.md).


## `enum_names`
```cpp
template <typename E>
constexpr array<string_view, N> enum_names() noexcept;
```

* Returns `std::array<std::string_view, N>` with all string enum name where `N = number of enum values`, sorted by enum value.

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

* Returns `std::array<std::pair<E, std::string_view>, N>` with all `std::pair` (value enum, string enum name) where `N = number of enum values`, sorted by enum value.

* Examples
  ```cpp
  constexpr auto color_entries = magic_enum::enum_entries<Color>();
  // color_entries -> {{Color::RED, "RED"}, {Color::BLUE, "BLUE"}, {Color::GREEN, "GREEN"}}
  // color_entries[0].first -> Color::RED
  // color_entries[0].second -> "RED"
  ```

## `is_unscoped_enum`
```cpp
template <typename T>
struct is_unscoped_enum;

template <typename T>
inline constexpr bool is_unscoped_enum_v = is_unscoped_enum<T>::value;
```

* Checks whether type is an [Unscoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Unscoped_enumeration).

* Provides the member constant value which is equal to true, if T is an [Unscoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Unscoped_enumeration) type. Otherwise, value is equal to false.

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

* Provides the member constant value which is equal to true, if T is an [Scoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations) type. Otherwise, value is equal to false.

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

* Improved UB-free "SFINAE-friendly" [std::underlying_type](https://en.cppreference.com/w/cpp/types/underlying_type).

* If T is a complete enumeration type, provides a member typedef type that names the underlying type of T. Otherwise, if T is not an enumeration type, there is no member type. Otherwise (T is an incomplete enumeration type), the program is ill-formed.

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
