# Migration

Upgrade steps per release, newest first. Find your current version and apply every section newer than it. For example, from 0.9.5 apply 0.9.7 and 0.10.0.

## Upgrading to 0.10.0

From 0.9.7 or 0.9.8. Review the behavior changes, then fix the compile errors.

Minimum versions: C++17, GCC 9, Clang 9, Xcode 11, MSVC 19.20 (Visual Studio 2019), CMake 3.22.

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

* **Named zero in flags.** `None = 0` is a regular flag value. It is counted and iterated, `enum_flags_name(E{0})` returns `"None"` instead of `""`, and `enum_flags_cast("None")` succeeds. Define `MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS` for the whole build to leave it out.

* **Signed flags** also reflect the sign bit, first in value order.

* **C++26 reflection** is selected automatically when available. It reflects only declared values, ignores the scan range and names aliases by their first declaration. Define `MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION` to keep the previous backend.

* **Checked integers.** Casts and membership checks reject integers that do not fit the underlying type. For a `uint8_t` enum, `enum_cast<E>(257)` is empty instead of `E{1}`. Cast intentional narrowing explicitly, and pass `unsigned char` instead of `char` for bytes above `0x7F`.

* **Customization hooks** are found only through ADL. A global `magic_enum_define_range_adl` for an enum in a namespace is ignored. Move it to the enum's namespace.

* **Custom names** rename or hide declared values only. A custom name for an undeclared value other than zero is ignored.

* **`enum_switch`** returns by value, even when the explicit result type is a reference.

* **Stream output** writes unnamed values of enums with a character underlying type as numbers. `static_cast<E>(65)` prints `65` instead of `A`.

* **Container comparators.** Keys that compare equivalent keep `enum_values` order, so their indices can change. `containers::set` compares lexicographically.

### Compile errors

* **A named value has more than one bit set**, for example `All = A | B` in an enum with ADL operators. Set `is_flags` explicitly. With `is_flags = true` combinations are not values, see [`enum_flags`](reference.md#enum_flags).

* **A named value is just outside the scan range.** Extend the [range](limitations.md#enum-range). To hide a sentinel such as `Count`, return `invalid_tag` from `customize::enum_name`. `MAGIC_ENUM_NO_CHECK_OUT_OF_RANGE` disables only the check.

* **Bitwise operators are missing.** Include `<magic_enum/magic_enum_bitwise.hpp>`. Prefer `MAGIC_ENUM_DEFINE_BITWISE_OPERATORS(E)` next to the enum, and keep `using namespace magic_enum::bitwise_operators` inside functions.

* **`customize_t`.** Replace `first` and `second` with `tag` and `name`. It no longer inherits `std::pair`.

* **A custom name for an undeclared zero.** Declare the value, for example `None = 0`.

* **`containers::set<E>(mask)`.** Use `set<E>::from_mask(mask)`. An invalid key throws `std::invalid_argument`.

* **`containers::bitset` string constructor.** The order is now `(sv, sep, cmp)`. Zero and combinations are masks, not positions.

* **`enum_type_name` for an unnamed enum.** Give the enum a typedef name or specialize `customize::enum_type_name`.

* **Removed macros.** Drop `MAGIC_ENUM_SUPPORTED_ALIASES` checks and `MAGIC_ENUM_ENABLE_HASH_SWITCH`.

### Modules

Link `magic_enum::magic_enum_module`. `magic_enum::magic_enum` stays header-only. Modules need C++20 and CMake 3.28, see [compiler issues](limitations.md#other-compiler-issues).

## Upgrading to 0.9.7

From 0.9.4, 0.9.5 or 0.9.6.

* Include `<magic_enum/magic_enum.hpp>` instead of `<magic_enum.hpp>`. The CMake, Meson and Bazel targets add `include`, not `include/magic_enum`, to the include path.

## Upgrading to 0.9.4

From 0.8.x or an earlier 0.9.x. Headers were split and moved. `<magic_enum/magic_enum_all.hpp>` includes all of them.

* 0.9.4: headers moved from `include/` to `include/magic_enum/`.
* 0.9.3: `enum_for_each` moved to `magic_enum_utility.hpp`.
* 0.9.2: `enum_flags_*` functions moved to `magic_enum_flags.hpp`.
* 0.9.0: stream operators moved to `magic_enum_iostream.hpp`.
