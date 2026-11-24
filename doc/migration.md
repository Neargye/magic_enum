# Migration

Apply every section newer than your version. For example, from 0.9.5 apply 0.9.7, 0.9.8 and 0.10.0.

## Upgrading to 0.10.0

From 0.9.8. Review the behavior changes, then fix the compile errors.

Minimum versions: C++17, GCC 9, Clang 9, Xcode 11, MSVC 19.20 (Visual Studio 2019), CMake 3.22, ReSharper C++ and CLion Nova 2025.3.

Options such as `MAGIC_ENUM_NO_AUTO_IS_FLAGS` and the range macros must match in every translation unit and in the module. Put them in one header and pass `MAGIC_ENUM_CONFIG_FILE` to the whole build.

### Behavior changes

These compile but can change counts, indices and names. Review stored indices, loops over `enum_values` and serialized names.

* **Automatic flags.** An enum with `operator|` and `operator&` found by ADL and returning the enum is now reflected as [flags](limitations.md#enum-flags). Only a named zero and named single bits are reflected. Streams, formatting and containers use flags too.

  To keep common reflection, set `is_flags = false`, or define `MAGIC_ENUM_NO_AUTO_IS_FLAGS` for the whole build. Use `as_common<>` for a single call.

  ```cpp
  template <>
  struct magic_enum::customize::enum_range<E> {
    static constexpr bool is_flags = false;
  };

  static_assert(!magic_enum::is_flags_enum_v<E>); // Guards against a silent switch.
  ```

* **Named zero in flags.** `None = 0` is a regular flag value. It is counted and iterated, `enum_flags_name(E{0})` returns `"None"` instead of `""`, and `enum_flags_cast("None")` succeeds. Containers include it too, so a flags `containers::array` has one more element. Define `MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS` for the whole build to leave it out.

* **Signed flags** also reflect the sign bit, first in value order.

* **C++26 standard reflection** is selected automatically when available. It reflects only declared values, ignores the scan range and names aliases by their first declaration. Define `MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION` to keep the previous backend.

* **Checked integers.** Casts and membership checks reject integers that do not fit the underlying type. For a `uint8_t` enum, `enum_cast<E>(257)` is empty instead of `E{1}`. Cast intentional narrowing explicitly, and pass `unsigned char` instead of `char` for bytes above `0x7F`.

* **Customization hooks** are found only through ADL. A global `magic_enum_define_range_adl` for an enum in a namespace is ignored. Move it to the enum's namespace.

* **Custom names** rename or hide declared values only. A custom name for an undeclared value other than zero is ignored by tables, and `enum_name<V>()` rejects it.

* **`enum_switch`** returns by value, even when the explicit result type is a reference.

* **`enum_reflected`** accepts only zero and single bits for flags, and any representable value of a common enum with standard reflection. The result no longer depends on whether values are reflected.

* **Stream output** writes unnamed values of enums with a character underlying type as numbers. `static_cast<E>(65)` prints `65` instead of `A`.

* **fmt and `format_as`.** An enum with its own `format_as` overload is formatted by it instead of by name, see [formatting](reference.md#formatting).

* **Container comparators.** Keys that compare equivalent keep `enum_values` order, so their indices can change.

* **`containers::set`** compares lexicographically. Inserting an unknown value throws `std::invalid_argument` instead of being ignored.

* **`containers::bitset`.** In strings, zero and combinations are masks, not positions. `(sv, {})` now passes a null character as the separator. Use `(sv)` for the default `|` separator. `to_ulong` returns `unsigned long`. Invalid input sets `failbit` instead of throwing.

### Compile errors

* **A named value has more than one bit set**, for example `All = A | B` in an enum with ADL operators. Set `is_flags = false` to keep 0.9 behavior and names, or `is_flags = true` to reflect single bits only, see [`enum_flags`](reference.md#enum_flags). If a namespace defines operator templates for all its enums, define `MAGIC_ENUM_NO_AUTO_IS_FLAGS` for the whole build. `MAGIC_ENUM_NO_CHECK_MULTIBIT_FLAGS` disables only the check.

* **A named value is just outside the scan range.** Extend the [range](limitations.md#enum-range). To hide a sentinel such as `Count`, return `invalid_tag` from `customize::enum_name`. `MAGIC_ENUM_NO_CHECK_OUT_OF_RANGE` disables only the check.

* **Bitwise operators are missing.** Include `<magic_enum/magic_enum_bitwise.hpp>`. Prefer `MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(E)` next to the enum, and keep `using namespace magic_enum::bitwise_operators` inside functions. The macro makes `E` a flags enum, see automatic flags above.

* **`enum_range<E>::is_flags` is not set.** Read the mode with `is_flags_enum_v<E>`.

* **`is_flags` is not `bool`.** Declare it as `static constexpr bool is_flags`.

* **Explicit `E&`** in `enum_contains`, `enum_reflected` and `enum_flags_contains`. Drop `&` or pass an lvalue.

* **Non-integer arguments** such as `1.0` or `std::integral_constant<int, 1>{}` in casts and membership checks. Pass an integer, for example `::value`, or cast explicitly.

* **`std::format` of `std::byte`** with `MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM`. magic_enum no longer formats it. Use `std::to_integer<unsigned>(value)`.

* **An API requires a non-empty reflected enum.** Check the [scan range](limitations.md#enum-range), the enum definition and customization, see [empty enums](limitations.md#empty-enums).

* **`MAGIC_ENUM_NO_ASSERT` together with `MAGIC_ENUM_ASSERT`.** Define only one of them.

* **`customize_t`.** Replace `first` and `second` with `tag` and `name`. It no longer inherits `std::pair`, so compare the fields and build a `std::pair` explicitly if needed.

* **A custom name for an undeclared zero.** Declare the value, for example `None = 0`.

* **`containers::set<E>(mask)`.** Use `set<E>::from_mask(mask)`. A mask with unknown bits throws `std::invalid_argument`.

* **`containers::bitset` string constructor.** The order is now `(sv, sep, cmp)`.

* **`enum_type_name` for an unnamed enum.** Give the enum a typedef name or specialize `customize::enum_type_name`.

* **Removed macros.** Drop `MAGIC_ENUM_SUPPORTED_ALIASES` checks and `MAGIC_ENUM_ENABLE_HASH_SWITCH`.

### Modules

Set `MAGIC_ENUM_USE_MODULES=ON` and link `magic_enum::magic_enum_module`. `magic_enum::magic_enum` stays header-only. Modules need C++20 and CMake 3.28, see [compiler issues](limitations.md#other-compiler-issues).

`{fmt}` support is no longer detected automatically. Set `MAGIC_ENUM_MODULE_WITH_FMT=ON`, or define `MAGIC_ENUM_USE_FMT_HEADER` when building `module/magic_enum.cppm` without CMake.

## Upgrading to 0.9.8

From 0.9.7.

* **`MAGIC_ENUM_AUTO_IS_FLAGS`** is removed and ignored. Set `is_flags = true` for each enum that relied on it.
* **`enum_flags_cast` and `enum_flags_contains` with a predicate** take the separator first: `(sv, '|', p)`.
* **`MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT` and `customize::enum_format_enabled`** are removed. Fully specialize `std::formatter` or `fmt::formatter` for one enum.

## Upgrading to 0.9.7

From 0.9.4, 0.9.5 or 0.9.6.

* Include `<magic_enum/magic_enum.hpp>` instead of `<magic_enum.hpp>`. The CMake, Meson and Bazel targets add `include`, not `include/magic_enum`, to the include path.

## Upgrading to 0.9.4

From 0.8.x or an earlier 0.9.x. Headers were split and moved. `<magic_enum/magic_enum_all.hpp>` includes all of them.

* 0.9.4: headers moved from `include/` to `include/magic_enum/`.
* 0.9.3: `enum_for_each` moved to `magic_enum_utility.hpp`.
* 0.9.2: `enum_flags_*` functions moved to `magic_enum_flags.hpp`.
* 0.9.0: stream operators moved to `magic_enum_iostream.hpp`.
