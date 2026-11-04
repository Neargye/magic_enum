# Reference

* [`enum_cast` returns enum value from string or integer.](#enum_cast)
* [`enum_value` returns enum value at specified index.](#enum_value)
* [`enum_values` returns enum value sequence.](#enum_values)
* [`enum_count` returns number of enum values.](#enum_count)
* [`enum_min` and `enum_max` return the minimum and maximum reflected enum values.](#enum_min-and-enum_max)
* [`enum_integer` and `enum_underlying` return underlying enum value.](#enum_integer-and-enum_underlying)
* [`enum_name` returns name from enum value.](#enum_name)
* [`enum_names` returns enum name sequence.](#enum_names)
* [`enum_entries` returns enum value and name sequence.](#enum_entries)
* [`customize::enum_range` customizes enum reflection.](#customizeenum_range)
* [`enum_index` returns index from enum value.](#enum_index)
* [`enum_contains` checks whether enum contains value or name.](#enum_contains)
* [`enum_reflected` checks the potential reflection domain.](#enum_reflected)
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
* [`MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS` defines stream operators for one enum.](#magic_enum_define_iostream_operators)
* [`ostream_operators` provides stream insertion operators for all enums.](#ostream_operators)
* [`istream_operators` provides stream extraction operators for all enums.](#istream_operators)
* [`MAGIC_ENUM_DEFINE_BITWISE_OPERATORS` defines bitwise operators for one enum.](#magic_enum_define_bitwise_operators)
* [`bitwise_operators` provides bitwise operators for all enums.](#bitwise_operators)
* [Risks of operator namespaces.](#risks-of-operator-namespaces)
* [`std::format` and `fmt::format` support for enums.](#formatting)
* [Container comparators and indexing helpers.](#container-helpers)
* [`containers::array` array container for enums.](#containersarray)
* [`containers::bitset` bitset container for enums.](#containersbitset)
* [`containers::set` set container for enums.](#containersset)

## Synopsis

* Read the [limitations](limitations.md) before use.

* Signatures are simplified. Name arguments take `string_view` by value and accept implicit conversions.

* A subtype goes after the enum type, as in `enum_values<E, as_flags<>>()`. `enum_name`, `enum_index`, `enum_contains` and `enum_reflected` also take it first, as in `enum_name<as_common<>>(value)`. `enum_switch` takes it after the result type, as in `enum_switch<R, as_flags<>>(f, value)`. `enum_flags_*` functions always use flags.

* Color examples use:

  ```cpp
  enum class Color { RED = -10, BLUE = 0, GREEN = 10 };
  ```

* `MAGIC_ENUM_SUPPORTED` and `magic_enum::is_magic_enum_supported` report compiler support. An unsupported compiler fails to compile unless `MAGIC_ENUM_NO_CHECK_SUPPORT` is defined.

* C++26 reflection is selected automatically when available. See [limitations](limitations.md#c26-standard-reflection).

* `Enum<T>` restricts C++17 function parameters to enum types.

* `enum_constant<V>` is `std::integral_constant<decltype(V), V>`. `enum_switch` and `enum_for_each` pass it to the callable.

* `MAGIC_ENUM_VERSION_MAJOR`, `MAGIC_ENUM_VERSION_MINOR` and `MAGIC_ENUM_VERSION_PATCH` give the library version. The module does not export them.

* `customize::customize_t` stores the customization `tag` and `name`. Construct it from a name, `default_tag`, or `invalid_tag`.

* Specialize `customize::enum_name` or `customize::enum_type_name` to change names. Return a non-empty name, `default_tag` to keep the reflected name, or `invalid_tag` to hide it. A hidden value is not reflected. Only declared values can be renamed. A custom name for an undeclared zero fails to compile. Other undeclared values have no reflected name. A hidden type name makes `enum_type_name` fail to compile. See the [example](../example/example_custom_name.cpp).

  ```cpp
  template <>
  constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<Color>(Color value) noexcept {
    switch (value) {
      case Color::RED:
        return "the red color";
      case Color::GREEN:
        return invalid_tag;
      default:
        return default_tag;
    }
  }

  template <>
  constexpr magic_enum::customize::customize_t magic_enum::customize::enum_type_name<Color>() noexcept {
    return "MyColor";
  }
  ```

* `MAGIC_ENUM_ASSERT` checks preconditions, such as the index passed to `enum_value`. It defaults to `assert`. Define your own `MAGIC_ENUM_ASSERT(...)` or disable it with `MAGIC_ENUM_NO_ASSERT`, but not both.

* `MAGIC_ENUM_ENABLE_HASH` enables hash-based name lookup with the default predicate. Custom predicates still use linear search.

* To customize string and optional types, define these macros before including `magic_enum.hpp`:

  ```cpp
  #include <my_lib/string.hpp>
  #include <my_lib/string_view.hpp>
  #define MAGIC_ENUM_USING_ALIAS_STRING using string = my_lib::String;
  #define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = my_lib::StringView;
  #define MAGIC_ENUM_USING_ALIAS_OPTIONAL template <typename T> using optional = my_lib::Optional<T>;
  #include <magic_enum/magic_enum.hpp>
  ```

  The types should behave like `std::string`, `std::string_view` and `std::optional`. Both string types must use the same character type.

  Reflected names are not transcoded. With a wide character type, use custom names for non-ASCII text.

* Use `MAGIC_ENUM_CONFIG_FILE` to keep configuration in a separate header:

  ```cpp
  #define MAGIC_ENUM_CONFIG_FILE "my_magic_enum_cfg.hpp"
  #include <magic_enum/magic_enum.hpp>
  ```

  The header can define type aliases and range macros.

* Use the same configuration in every translation unit and in the module build.

## `enum_cast`

```cpp
template <typename E, typename I>
constexpr optional<E> enum_cast(I value) noexcept;

template <typename E>
constexpr optional<E> enum_cast(string_view value) noexcept;

template <typename E, typename BinaryPredicate>
constexpr optional<E> enum_cast(string_view value, BinaryPredicate p);
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns the matching enum value as `optional<E>`, or an empty optional if no value matches.

* Integer input can be any integral type or unscoped enum. Values that do not fit the underlying type are rejected. Other types, such as floating point, need an explicit cast. The same rules apply to `enum_contains`, `enum_flags_cast`, `enum_flags_contains` and `enum_reflected`.

* `case_insensitive` gives ASCII case-insensitive matching in `enum_cast`, `enum_contains`, `enum_flags_cast` and `enum_flags_contains`.

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

* Cast explicitly if you want narrowing:

  ```cpp
  enum class Byte : std::uint8_t { One = 1 };
  static_assert(!magic_enum::enum_cast<Byte>(257).has_value());
  static_assert(magic_enum::enum_cast<Byte>(static_cast<std::uint8_t>(257)) == Byte::One);
  ```

## `enum_value`

```cpp
template <typename E>
constexpr E enum_value(size_t index) noexcept;

template <typename E, size_t I>
constexpr E enum_value() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns the enum value at the given index.
  * `enum_value<E>(index)` is undefined behavior if `index >= enum_count<E>()`. Only `MAGIC_ENUM_ASSERT` checks it, in debug builds by default.
  * `enum_value<E, I>()` fails to compile if `I >= enum_count<E>()`.

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
constexpr const array<E, N>& enum_values() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns a reference to a static `array<E, N>` of all reflected values sorted by value, where `N = enum_count<E>()`. The reference stays valid for the whole program.

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

* Returns the number of reflected values, or 0 if none are reflected.

* Use it to guard APIs that require reflected values. See [empty enums](limitations.md#empty-enums).

* Examples

  ```cpp
  constexpr auto color_count = magic_enum::enum_count<Color>();
  // color_count -> 3
  ```

## `enum_min` and `enum_max`

```cpp
template <typename E>
constexpr E enum_min() noexcept;

template <typename E>
constexpr E enum_max() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Return the minimum and maximum reflected values.

* Fail to compile if no values are reflected.

* For flags, the minimum can be a named zero or a negative sign bit.

* Examples

  ```cpp
  constexpr auto color_min = magic_enum::enum_min<Color>();
  // color_min -> Color::RED

  constexpr auto color_max = magic_enum::enum_max<Color>();
  // color_max -> Color::GREEN

  constexpr auto max_integer = magic_enum::enum_underlying(color_max);
  // max_integer -> 10
  ```

## `enum_integer` and `enum_underlying`

```cpp
template <typename E>
constexpr underlying_type_t<E> enum_integer(E value) noexcept;

template <typename E>
constexpr underlying_type_t<E> enum_underlying(E value) noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns the underlying integer value.

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

* Returns the enum name as a null-terminated `string_view`.

* `enum_name(value)` returns an empty string for an unnamed or [out-of-range](limitations.md#enum-range) value. `enum_name<value>()` fails to compile for an unnamed value.

* `enum_name<value>()` ignores the [scan range](limitations.md#enum-range) and flags filtering, so it can name declared flag combinations. Name customization still applies.

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
constexpr const array<string_view, N>& enum_names() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns a reference to the reflected names in `enum_values<E>()` order. The array and names remain valid for the whole program.

* Examples

  ```cpp
  constexpr auto color_names = magic_enum::enum_names<Color>();
  // color_names -> {"RED", "BLUE", "GREEN"}
  // color_names[0] -> "RED"
  ```

## `enum_entries`

```cpp
template <typename E>
constexpr const array<pair<E, string_view>, N>& enum_entries() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns a reference to the reflected value-name pairs in `enum_values<E>()` order. The array and names remain valid for the whole program.

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

* Sets reflection options for an enum.

* An explicit `is_flags` must be `bool`. `true` selects flags, `false` selects common reflection. Without it, flags are selected when ADL finds both `E | E` and `E & E` for rvalues with result exactly `E`. Otherwise reflection is common.

* Define `MAGIC_ENUM_NO_AUTO_IS_FLAGS` before including magic_enum to disable this automatic choice. Explicit settings and `as_flags<>` still work. Use the same definition across translation units and when building the module.

* `prefix_length` strips a prefix from reflected names and defaults to `0`. It must leave at least one character. Custom names are not affected.

* `min` and `max` set the scan range for compiler-specific reflection. Flags and standard reflection ignore them. See [range limits](limitations.md#enum-range).

* `as_flags<>` and `as_common<>` override the default mode for one call. Invalid customization still produces an error.

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

    Define `magic_enum_define_range_adl(E)` in the enum's namespace or as a friend in its class, returning `magic_enum::customize::adl_info()`:

    ```cpp
    namespace Deeply::Nested::Namespace {
      enum class my_enum_type { my_enum_value1 = 10, my_enum_value2 = 11 };

      inline constexpr auto magic_enum_define_range_adl(my_enum_type) {
        return magic_enum::customize::adl_info()
            .minmax<10, 11>()
            .prefix<sizeof("my_enum_") - 1>();
      }
    }
    ```

    A `customize::enum_range<E>` specialization replaces the hook settings entirely. The hook must return a non-final class by value. Declare it and any ADL operators before the first reflection use, consistently across translation units.

    `.flag<true>()` selects flags and `.flag<false>()` selects common reflection. Without `.flag`, the mode is automatic, including after `.minmax<...>()` or `.prefix<...>()`. Flags ignore `.minmax<...>()`.

    Use `is_flags_v<E>` to check the selected mode. Prefer `adl_info()` when defining hooks.

## `enum_index`

```cpp
template <typename E>
constexpr optional<size_t> enum_index(E value) noexcept;

template <auto V>
constexpr size_t enum_index() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns the index in `enum_values<E>()`. For a value that is not reflected, `enum_index(value)` returns an empty optional and `enum_index<V>()` fails to compile.

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

template <typename E, typename I>
constexpr bool enum_contains(I value) noexcept;

template <typename E>
constexpr bool enum_contains(string_view value) noexcept;

template <typename E, typename BinaryPredicate>
constexpr bool enum_contains(string_view value, BinaryPredicate p);
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns `true` for a reflected value or name.

* For a flag mask, use `enum_flags_contains`. [`enum_reflected`](#enum_reflected) checks whether a value can be reflected at all.

  ```cpp
  enum class Small : std::uint8_t { A = 1, B = 2 };
  static_assert(!magic_enum::enum_contains<Small>(3)); // Not an enumerator.
  static_assert(magic_enum::enum_reflected<Small>(3)); // Within the scan range.
  ```

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

template <typename E, typename I>
constexpr bool enum_reflected(I value) noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Checks whether a value can be reflected. It does not check whether the value has a name or is a valid flag mask.

* Integer input follows the [`enum_cast`](#enum_cast) rules. Values that do not fit the underlying type return `false`.

| Backend | Common enum | Flags enum |
|---|---|---|
| Compiler-specific | Value is within the configured `enum_range` | Zero or a single bit of the underlying type |
| Standard reflection | Any representable value | Zero or a single bit of the underlying type |

* For signed types, the sign bit counts as a single bit. Names, empty or opaque enums and `MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM` do not affect the result.

* Examples

  ```cpp
  enum class DomainFlags : std::uint8_t { None = 0, A = 1, B = 2, Both = 3 };
  static_assert(!magic_enum::enum_reflected<DomainFlags, magic_enum::as_flags<>>(257));
  static_assert(magic_enum::enum_reflected<DomainFlags, magic_enum::as_flags<>>(4));
  static_assert(!magic_enum::enum_contains<DomainFlags, magic_enum::as_flags<>>(4));
  static_assert(!magic_enum::enum_reflected<DomainFlags, magic_enum::as_flags<>>(3));
  static_assert(magic_enum::enum_flags_contains<DomainFlags>(3));
  ```

## `enum_type_name`

```cpp
template <typename E>
constexpr string_view enum_type_name() noexcept;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Returns the enum type name as a null-terminated `string_view`.

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

* Combines several enum values into one for nested switch/case statements. A value that is not reflected fails `MAGIC_ENUM_ASSERT`, or gives an empty optional when asserts are disabled.

* `enum_fuse_t` is a distinct type for each ordered list of enum types, so mixing orders fails to compile.

* Requires at least two values. All combinations of the enums must fit in `std::uintmax_t`.

* On MSVC, suppress warning C4064 with `/wd4064`, or define `MAGIC_ENUM_NO_TYPESAFE_ENUM_FUSE` to use `uintmax_t`.

* Examples

  ```cpp
  switch (magic_enum::enum_fuse(color, direction).value()) {
    case magic_enum::enum_fuse(Color::RED, Directions::Up).value(): // ...
      break;
    case magic_enum::enum_fuse(Color::BLUE, Directions::Down).value(): // ...
      break;
    case magic_enum::enum_fuse(Directions::Up, Color::RED).value(): // Compilation error
      break;
    default:
      break;
  }
  ```

## `enum_switch`

```cpp
template <typename Result = default_result_type, typename E, typename Lambda>
constexpr decltype(auto) enum_switch(Lambda&& lambda, E value);

template <typename Result, typename E, typename Lambda>
constexpr decltype(auto) enum_switch(Lambda&& lambda, E value, Result&& result);
```

* Defined in header `<magic_enum/magic_enum_switch.hpp>`

* Calls the callable with `enum_constant<V>` for the matching reflected value.

* The first template argument sets the result type, as in `enum_switch<int>(f, value)`. By default it is the decayed common type of the callable results.

* If no value matches, returns the fallback or a default-constructed result.

* Always returns by value, even when `Result` is a reference.

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

* Calls the callable with `enum_constant<V>` for each reflected value.

* Returns `void` if any call returns `void`, an array if all results have the same type, or a tuple otherwise. Results are stored by value.

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

* Moves by `n` positions in `enum_values<E>()` order. A negative `n` moves the other way.

* The non-circular functions return an empty optional if the input is not reflected or the result is out of range. The circular functions wrap around and require a reflected input.

  ```cpp
  magic_enum::enum_next_value(Color::RED);           // -> optional containing Color::BLUE
  magic_enum::enum_prev_value_circular(Color::RED);  // -> Color::GREEN
  ```

## `enum_flags`

```cpp
template <typename E, typename String = string>
constexpr String enum_flags_name(E value, char_type sep = '|');

template <typename E, typename I>
constexpr optional<E> enum_flags_cast(I value) noexcept;

template <typename E>
constexpr optional<E> enum_flags_cast(string_view value, char_type sep = '|') noexcept;

template <typename E, typename BinaryPredicate>
constexpr optional<E> enum_flags_cast(string_view value, char_type sep, BinaryPredicate p);

template <typename E>
constexpr bool enum_flags_contains(E value) noexcept;

template <typename E, typename I>
constexpr bool enum_flags_contains(I value) noexcept;

template <typename E>
constexpr bool enum_flags_contains(string_view value, char_type sep = '|') noexcept;

template <typename E, typename BinaryPredicate>
constexpr bool enum_flags_contains(string_view value, char_type sep, BinaryPredicate p);

template <typename E>
constexpr bool enum_flags_test(E flags, E flag) noexcept;

template <typename E>
constexpr bool enum_flags_test_any(E lhs, E rhs) noexcept;
```

* Defined in header `<magic_enum/magic_enum_flags.hpp>`

* `enum_flags_name` returns flag names joined by the separator, `'|'` by default. For zero it returns the zero name, if one is declared. Unknown bits and an unnamed zero give an empty string.

* A custom `String` must be default constructible, have `char_type` as its `value_type`, and provide `empty()`, `append(count, ch)` and `append(data, size)`. For constant evaluation it must be usable in constant expressions.

* `enum_flags_cast` returns the mask as `optional<E>`, or an empty optional for unknown bits, an unnamed zero or invalid names.

* Names are split at each separator without trimming spaces. Repeated names are allowed, and one trailing separator is ignored, so `"Left|"` parses as `Left`. An empty string, a leading separator or two separators in a row are rejected.

* If several values match a name, parsing takes the first in value order. Names round-trip only if they are unique and do not contain the separator.

* `enum_flags_contains` checks whether the value or names form a valid mask. Zero is valid only if it is named.

* `enum_flags_test` checks that `flag` is nonzero and all its bits are set in `flags`. Use `flags == E{}` to check for an empty mask.

* `enum_flags_test_any` checks whether `lhs` and `rhs` share any bit. It returns `false` if either is zero.

* ADL bitwise operators enable flags automatically. For enums without matching operators, set `enum_range<E>::is_flags` to `true`. `enum_flags_*` functions always use flags, even with an explicit opt-out.
  ```cpp
  enum class Directions { Up = 1 << 1, Down = 1 << 2, Right = 1 << 3, Left = 1 << 4 };
  template <>
  struct magic_enum::customize::enum_range<Directions> {
    static constexpr bool is_flags = true;
  };
  ```

  * Flags ignore `MAGIC_ENUM_RANGE_MIN` and `MAGIC_ENUM_RANGE_MAX`.

  * A named zero, such as `None = 0`, is reflected. It is never added to nonzero output, and `"None|A"` parses as `A`. Returning `invalid_tag` for zero hides it everywhere.

  * Zero is a regular value for `enum_count`, `enum_index`, `enum_switch`, `enum_for_each`, `array` and `set`. A flag `bitset` has no position for it.

    ```cpp
    enum class Access : std::uint8_t { None = 0, Read = 1, Write = 2 };
    template <>
    struct magic_enum::customize::enum_range<Access> {
      static constexpr bool is_flags = true;
    };
    static_assert(magic_enum::enum_count<Access>() == 3);
    static_assert(magic_enum::enum_index(Access::Read) == std::size_t{1});
    static_assert(magic_enum::enum_flags_cast<Access>("None") == Access::None);
    ```

  * For signed enums, a named sign bit is reflected too. It comes first, since it is negative.

  * Named combinations such as `Both = 3` are formatted as their bits, and `enum_flags_cast` rejects their names. To look up a combination by name, use `as_common<>` with `enum_name` or `enum_cast`. With compiler-specific reflection, it must be within the common range.

* Examples

  ```cpp
  enum Directions : std::uint64_t {
    Left = 1,
    Down = 2,
    Up = 4,
    Right = 8,
    LeftAndDown = 3
  };
  MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(Directions); // From <magic_enum/magic_enum_bitwise.hpp>.

  magic_enum::enum_flags_name(Directions::Up | Directions::Right); // -> "Up|Right"
  magic_enum::enum_flags_name(Directions::LeftAndDown); // -> "Left|Down"
  magic_enum::enum_flags_name(Directions::Up | Directions::Right, ','); // -> "Up,Right"

  magic_enum::enum_flags_contains(Directions::Up | Directions::Right); // -> true
  magic_enum::enum_flags_contains(Directions::LeftAndDown); // -> true

  magic_enum::enum_flags_cast<Directions>(3).value(); // -> Directions::Left|Directions::Down
  magic_enum::enum_flags_cast<Directions>("Left|Down").value(); // -> Directions::Left|Directions::Down
  magic_enum::enum_flags_cast<Directions>("Left,Down", ',').value(); // -> Directions::Left|Directions::Down
  magic_enum::enum_flags_cast<Directions>("LeftAndDown"); // -> nullopt
  magic_enum::enum_cast<Directions>("LeftAndDown"); // -> nullopt, default subtype is flags
  magic_enum::enum_cast<Directions, magic_enum::as_common<>>("LeftAndDown").value(); // -> Directions::LeftAndDown
  magic_enum::enum_name<magic_enum::as_common<>>(Directions::LeftAndDown); // -> "LeftAndDown"

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

* Checks whether `T` uses flag semantics by default, including automatic detection and explicit settings.

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
  enum color { red, green, blue };
  enum class Direction { Left, Right };

  static_assert(magic_enum::is_unscoped_enum<color>::value);
  static_assert(!magic_enum::is_unscoped_enum<Direction>::value);

  // Helper variable template.
  static_assert(magic_enum::is_unscoped_enum_v<color>);
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
  enum color { red, green, blue };
  enum class Direction { Left, Right };

  static_assert(!magic_enum::is_scoped_enum<color>::value);
  static_assert(magic_enum::is_scoped_enum<Direction>::value);

  // Helper variable template.
  static_assert(magic_enum::is_scoped_enum_v<Direction>);
  ```

## `underlying_type`

```cpp
template <typename T>
struct underlying_type;

template <typename T>
using underlying_type_t = typename underlying_type<T>::type;
```

* Defined in header `<magic_enum/magic_enum.hpp>`

* Gives the underlying type of an enum. For a non-enum type there is no `type` member.

* Examples

  ```cpp
  enum color { red, green, blue };
  enum class Direction { Left, Right };

  static_assert(std::is_same_v<magic_enum::underlying_type<Direction>::type, int>);
  static_assert(std::is_integral_v<magic_enum::underlying_type<color>::type>); // Implementation-defined for an unscoped enum without a fixed type.

  // Helper types.
  static_assert(std::is_same_v<magic_enum::underlying_type_t<Direction>, int>);
  ```

## `MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS`

```cpp
#define MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS(E)
#define MAGIC_ENUM_DEFINE_FRIEND_IOSTREAM_OPERATORS(E)
```

* Defined in header `<magic_enum/magic_enum_iostream.hpp>`

* Defines `operator<<` and `operator>>` for `E` only. No `using` is needed. They work like [`ostream_operators`](#ostream_operators) and [`istream_operators`](#istream_operators), without the `optional<E>` overload.

* Put `MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS(E)` in the enum's namespace. For a public nested enum, put it after the class and use `C::E`.

* Put `MAGIC_ENUM_DEFINE_FRIEND_IOSTREAM_OPERATORS(E)` inside the class that declares the enum. Use it for private enums and class templates.

* End the macro call with a semicolon. These operators take precedence over the operator namespaces.

* Modules do not export macros. Module users put a `using` of the operator namespace inside functions.

* Examples

  ```cpp
  namespace app {
  enum class Color { RED, GREEN, BLUE };
  MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS(Color);
  } // namespace app

  std::cout << app::Color::BLUE << std::endl; // "BLUE"

  app::Color color;
  std::cin >> color;

  class Widget {
    enum class State { Idle, Busy };
    MAGIC_ENUM_DEFINE_FRIEND_IOSTREAM_OPERATORS(State);
  };
  ```

## `ostream_operators`

```cpp
template <typename Char, typename Traits, typename E>
basic_ostream<Char, Traits>& operator<<(basic_ostream<Char, Traits>& os, E value);

template <typename Char, typename Traits, typename E>
basic_ostream<Char, Traits>& operator<<(basic_ostream<Char, Traits>& os, optional<E> value);
```

* Defined in header `<magic_enum/magic_enum_iostream.hpp>`

* Writes the name, flag names joined by `|`, or the integer for an unnamed value. An empty `optional<E>` writes nothing.

* `iostream_operators` contains both `ostream_operators` and `istream_operators`.

* Prefer [`MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS`](#magic_enum_define_iostream_operators). Otherwise keep the `using` inside a function. See [namespace risks](#risks-of-operator-namespaces).

* Examples

  ```cpp
  void print(Color color) {
    using magic_enum::iostream_operators::operator<<; // Enables operator<< for all enums in this function.
    std::cout << color << std::endl; // Writes the name, such as "BLUE".
  }
  ```

## `istream_operators`

```cpp
template <typename Char, typename Traits, typename E>
basic_istream<Char, Traits>& operator>>(basic_istream<Char, Traits>& is, E& value);
```

* Defined in header `<magic_enum/magic_enum_iostream.hpp>`

* Reads one word with `enum_cast`, or `enum_flags_cast` for flags. An invalid name sets `failbit` and leaves the value unchanged. The stream character type must be `magic_enum::char_type`.

* Prefer [`MAGIC_ENUM_DEFINE_IOSTREAM_OPERATORS`](#magic_enum_define_iostream_operators). Otherwise keep the `using` inside a function. See [namespace risks](#risks-of-operator-namespaces).

* Examples

  ```cpp
  Color read() {
    using magic_enum::iostream_operators::operator>>; // Enables operator>> for all enums in this function.
    Color color = Color::RED;
    std::cin >> color;
    return color;
  }
  ```

## `MAGIC_ENUM_DEFINE_BITWISE_OPERATORS`

```cpp
#define MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(E)
#define MAGIC_ENUM_DEFINE_FRIEND_BITWISE_OPERATORS(E)
```

* Defined in header `<magic_enum/magic_enum_bitwise.hpp>`

* Defines `~`, `|`, `&`, `^`, `|=`, `&=`, `^=` for `E` only. No `using` is needed. `~` inverts the underlying value, or applies logical not for a `bool` underlying type.

* `~` also sets bits without a name, so `enum_flags_name(~E::A)` returns `""`. Use it to clear bits, for example `mask & ~E::A`.

* Placement follows the [iostream macro rules](#magic_enum_define_iostream_operators). These operators take precedence over the operator namespace.

* These operators enable automatic flag reflection unless `is_flags = false` or `MAGIC_ENUM_NO_AUTO_IS_FLAGS` disables it. The macro itself does not depend on reflection.

* Without a fixed underlying type, `~` is undefined behavior if the result is outside the enum's value range. Use `enum class` or an explicit underlying type.

* Examples

  ```cpp
  enum class Flags { A = 1 << 0, B = 1 << 1, C = 1 << 2, D = 1 << 3 };
  MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(Flags);

  constexpr Flags flags = Flags::A | (Flags::B & ~Flags::C);

  template <typename T>
  class Buffer {
    enum class Mode { Read = 1, Write = 2 };
    MAGIC_ENUM_DEFINE_FRIEND_BITWISE_OPERATORS(Mode);
  };
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

* Defined in header `<magic_enum/magic_enum_bitwise.hpp>`. `<magic_enum/magic_enum.hpp>` does not provide it.

* Provides bitwise operators for all enums. Use it for function-local enums or with the module. A `using namespace` directive does not enable automatic flag reflection.

* The same `~` rule applies as for [`MAGIC_ENUM_DEFINE_BITWISE_OPERATORS`](#magic_enum_define_bitwise_operators).

* Prefer [`MAGIC_ENUM_DEFINE_BITWISE_OPERATORS`](#magic_enum_define_bitwise_operators). Otherwise keep the `using` inside a function. See [namespace risks](#risks-of-operator-namespaces).

* Examples

  ```cpp
  void f() {
    enum class Flags { A = 1 << 0, B = 1 << 1, C = 1 << 2, D = 1 << 3 };
    using namespace magic_enum::bitwise_operators; // Enables operators for all enums in this function.
    Flags flags = Flags::A | (Flags::B & ~Flags::C);
  }
  ```

## Risks of operator namespaces

* A `using` of an operator namespace enables its operators for every enum in scope, including enums from other libraries.

* For unscoped enums, `a | b` can return the enum instead of an integer, and stream output can write a name instead of a number.

* If the same inline function or template picks a different operator in different translation units, the program violates the one definition rule. This can crash, see [#434](https://github.com/Neargye/magic_enum/issues/434).

* Prefer the per-enum macros. Keep any `using` of an operator namespace inside a function, never in a header or at namespace scope.

## Formatting

* Defined in header `<magic_enum/magic_enum_format.hpp>`

* Provides `std::formatter` when `std::format` is available, and `fmt::formatter` when `{fmt}` is included first.

* Formats a value by name, flags joined by `|`, and other values as integers.

* Requires `magic_enum::char_type` to be `char`.

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

* `name_less<>`, `name_greater<>`, `name_less_case_insensitive` and `name_greater_case_insensitive` order values by name and allow lookup by name.

* `default_indexing<E>` uses `enum_values<E>()` order. `comparator_indexing<Cmp>` orders `containers::array` by a comparator. Comparator-based container indexing preserves `enum_values<E>()` order for equivalent keys.

* A custom `Index` needs a `static constexpr` non-throwing `at(E)` that returns `optional<std::size_t>`. It maps each reflected value to a unique index in `[0, enum_count<E>())` and returns an empty optional for any other value, including an unnamed zero and combinations for flags.

* For `containers::bitset`, a custom `Index` also needs a `static constexpr` non-throwing `it(std::size_t)` that returns a `const E*` to the value at that index.

* Container errors:
  * `std::out_of_range` for an invalid position in `array::at`, `bitset::test/set/reset/flip` and raw `bitset` constructors.
  * `std::invalid_argument` for unknown names, masks with unknown bits, invalid `set` keys, and raw string characters other than `zero` and `one`.
  * `std::overflow_error` from `bitset::to_ulong` and `to_ullong` if a set position does not fit the result.
  * With `MAGIC_ENUM_NO_EXCEPTION` or with exceptions disabled, `std::abort()` is called instead.

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

  constexpr void swap(array& other) noexcept(std::is_nothrow_move_constructible_v<V> && std::is_nothrow_move_assignable_v<V>);

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

* `make_array<E>(values...)` deduces the element type and assigns the arguments in `enum_values<E>()` order. It takes exactly `enum_count<E>()` arguments.

* `to_array<E>(array)` converts a built-in array of `enum_count<E>()` elements.

* `get<Color::BLUE>(array)` accesses an element by value, and `get<1>(array)` by index in the indexing order. Both are checked at compile time.

* Examples

  ```cpp
  // Arguments follow enum_values<Color>() order: RED, BLUE, GREEN.
  constexpr auto color_rgb_array = magic_enum::containers::make_array<Color>(RGB{255, 0, 0}, RGB{0, 0, 255}, RGB{0, 255, 0});
  ```

  ```cpp
  magic_enum::containers::array<Color, RGB> color_rgb_array {};
  color_rgb_array[Color::RED] = {255, 0, 0};
  color_rgb_array[Color::GREEN] = {0, 255, 0};
  color_rgb_array[Color::BLUE] = {0, 0, 255};
  magic_enum::containers::get<Color::BLUE>(color_rgb_array); // -> RGB{0, 0, 255}
  magic_enum::containers::get<1>(color_rgb_array); // -> RGB{0, 0, 255}, BLUE is at index 1
  ```

## `containers::bitset`

```cpp
template <typename E, typename Index = default_indexing<E>>
class bitset {

  constexpr bitset() noexcept;

  constexpr explicit bitset(detail::raw_access_t) noexcept;

  constexpr explicit bitset(detail::raw_access_t, unsigned long long val);

  constexpr explicit bitset(detail::raw_access_t, string_view sv, string_view::size_type pos = 0, string_view::size_type n = string_view::npos, char_type zero = '0', char_type one = '1');

  constexpr explicit bitset(detail::raw_access_t, const char_type* str, std::size_t n = ~std::size_t{}, char_type zero = '0', char_type one = '1');

  constexpr bitset(std::initializer_list<E> starters);

  constexpr explicit bitset(E starter);

  template <typename Cmp = std::equal_to<>>
  constexpr explicit bitset(string_view sv, char_type sep = '|', Cmp&& cmp = {});

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

  constexpr bitset& flip(E pos);

  friend constexpr bitset operator&(const bitset& lhs, const bitset& rhs) noexcept;

  friend constexpr bitset operator|(const bitset& lhs, const bitset& rhs) noexcept;

  friend constexpr bitset operator^(const bitset& lhs, const bitset& rhs) noexcept;

  constexpr explicit operator E() const;

  template <typename String = string>
  constexpr String to_string(char_type sep = '|') const;

  template <typename String = string>
  constexpr String to_string(detail::raw_access_t, char_type zero = '0', char_type one = '1') const;

  constexpr unsigned long long to_ullong(detail::raw_access_t raw) const;

  constexpr unsigned long to_ulong(detail::raw_access_t raw) const;

  template <typename Traits>
  friend std::basic_ostream<char_type, Traits>& operator<<(std::basic_ostream<char_type, Traits>& o, const bitset& bs);

  template <typename Traits>
  friend std::basic_istream<char_type, Traits>& operator>>(std::basic_istream<char_type, Traits>& i, bitset& bs);
};

template <typename E> // Only for flag enums.
explicit bitset(E starter) -> bitset<E>;
```

* Defined in header `<magic_enum/magic_enum_containers.hpp>`

* Fixed-size bitset indexed by reflected enum values.

* Default construction creates an empty bitset.

* Pass `magic_enum::containers::raw_access` to use the raw constructors and conversions, which work with positions instead of names.

* Raw positions follow the indexing order, `enum_values<E>()` by default. For flags, a named zero has no position.

* In a raw string, the first character is position 0, the opposite of `std::bitset`. In a raw integer, bit 0 is position 0.

  ```cpp
  auto bits = magic_enum::containers::bitset<Color>(magic_enum::containers::raw_access, "100");
  // bits.test(Color::RED) -> true, RED is position 0
  ```

* For flags, only nonzero bits have positions. An empty bitset prints the zero name if one is declared, otherwise an empty string. A nonempty bitset never prints zero. Names follow `enum_values<E>()` order, raw output follows the indexing order.

* `test`, `set`, `reset` and `flip` with an `E` argument throw `std::out_of_range` for an invalid position, including zero and combinations for flags. `operator[]` requires a valid position. `find` returns `end()` if there is no such position. Use `none()` to check for emptiness. For non-flag enums, a reflected zero is a normal position.

* A flag enum with only a named zero cannot be used with `bitset`.

* For flags, the single-value and initializer-list constructors take masks. Unknown bits throw `std::invalid_argument`, and zero adds nothing. Convert back with `static_cast<E>(bits)`. To apply a mask, use `bits |= bitset<E>(mask)`, `&=` or `^=`.

* For non-flag enums, construct from a list of values. Values that are not reflected throw `std::out_of_range`. The single-value constructor and the conversion to `E` exist only for flags.

* String construction follows the [flags grammar](#enum_flags), but an empty string gives an empty bitset. Unknown names throw `std::invalid_argument`. For flags, `"None|A"` sets only `A`. For non-flag enums, a zero name sets its position.

* `to_string<String>()` has the same [`String` requirements](#enum_flags) as `enum_flags_name`. The raw form also needs `reserve(size())`. Streams use the default `string`.

* Stream input reads one word with `'|'` as the separator. An invalid name sets `failbit` and leaves the bitset unchanged. The stream's exception mask applies.

* Specializes `std::hash`, so it works in unordered containers.

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
  // to_string -> "BLUE|GREEN"
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

  static constexpr set from_mask(E mask);

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

  constexpr std::pair<iterator, bool> insert(const value_type& value);

  constexpr std::pair<iterator, bool> insert(value_type&& value);

  constexpr iterator insert(const_iterator, const value_type& value);

  constexpr iterator insert(const_iterator hint, value_type&& value);

  template <typename InputIt>
  constexpr void insert(InputIt first, InputIt last);

  constexpr void insert(std::initializer_list<value_type> ilist);

  template <typename... Args>
  constexpr std::pair<iterator, bool> emplace(Args&&... args);

  template <typename... Args>
  constexpr iterator emplace_hint(const_iterator, Args&&... args);

  constexpr iterator erase(const_iterator pos) noexcept;

  constexpr iterator erase(const_iterator first, const_iterator last) noexcept;

  constexpr size_type erase(const key_type& key) noexcept;

  template <typename K>
  constexpr size_type erase(K&& x);

  void swap(set& other) noexcept;

  constexpr size_type count(const key_type& key) const noexcept;

  template <typename K>
  constexpr size_type count(const K& x) const;

  constexpr const_iterator find(const key_type& key) const noexcept;

  template <typename K>
  constexpr const_iterator find(const K& x) const;

  constexpr bool contains(const key_type& key) const noexcept;

  template <typename K>
  constexpr bool contains(const K& x) const;

  constexpr std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const;

  template <typename K>
  constexpr std::pair<const_iterator, const_iterator> equal_range(const K& x) const;

  constexpr const_iterator lower_bound(const key_type& key) const;

  template <typename K>
  constexpr const_iterator lower_bound(const K& x) const;

  constexpr const_iterator upper_bound(const key_type& key) const;

  template <typename K>
  constexpr const_iterator upper_bound(const K& x) const;

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

* Fixed-capacity set of reflected enum values. The comparator must be default constructible and usable in constant expressions.

* Distinct values stay separate keys even if the comparator treats them as equal. Lookup by another key type needs a transparent comparator and can match several values.

* Use `set<E>{value}` for one key and `set<E>{}` for an empty set.

* For flags, `set<E, Cmp>::from_mask(mask)` returns the set of its nonzero bits. Unknown bits throw `std::invalid_argument`. A zero mask gives an empty set. A named zero is a key, so `set<E>{E::None}` contains it, while `from_mask(E::None)` is empty. Combinations are not keys.

* Constructors, insertion and list assignment throw `std::invalid_argument` for values that are not reflected. Duplicates are ignored, and `insert(value)` returns `{iterator, false}` for an existing key. Lookup and erasure of a missing key return `end()`, `false` or `0`.

* `emplace` and `emplace_hint` construct `E` from their arguments, including explicit conversions. Integers are rejected. With no arguments they insert `E{}`, which must be reflected.

* Range and list insertion keep the keys inserted before an error. List assignment changes nothing if any key is invalid.

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
