# Limitations

Without standard reflection, magic_enum reads compiler signatures to discover enum names.

With both backends, customization can rename or hide declared values, but cannot add new ones. A custom name for an undeclared zero fails to compile. A custom name for another undeclared value is ignored, and `enum_name<V>()` for such a value fails to compile.

## C++26 Standard Reflection

Standard reflection is selected automatically when available. Otherwise magic_enum uses compiler-specific reflection.

* Standard reflection finds declared values without a scan range. Customization and flags filtering still apply. Counts, indices and names can differ from compiler-specific reflection.

* Each value has one name, taken from its first declaration.

* Define `MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION` to keep compiler-specific reflection, or `MAGIC_ENUM_REQUIRE_STD_REFLECTION` to fail compilation when standard reflection is unavailable. Do not define both.

* Reflecting values requires the enum definition. See [empty enums](#empty-enums) for opaque declarations.

* Use the same enum definitions and configuration in every translation unit and in the module build.

## Enum Flags

* Flags are selected automatically when ADL finds both `E | E` and `E & E` for rvalues returning exactly `E`. Operator macros placed next to the enum satisfy this rule. `using namespace`, built-in integer operators and operators returning a wrapper do not.

* Existing operators can silently switch an enum to flags, changing its names, counts and indices.

* Set `enum_range<E>::is_flags` explicitly to override detection. Use `true` for flags without matching operators, or `false` for ordinary reflection. `enum_flags_*` APIs always use flags.
  ```cpp
  enum class Directions { Up = 1 << 0, Down = 1 << 1, Right = 1 << 2, Left = 1 << 3 };
  template <>
  struct magic_enum::customize::enum_range<Directions> {
    static constexpr bool is_flags = true;
  };
  ```

* Define `MAGIC_ENUM_NO_AUTO_IS_FLAGS` for the whole build to disable automatic detection. Explicit `is_flags = true`, `.flag<true>()` and `as_flags<>` still work. Set it in `MAGIC_ENUM_CONFIG_FILE` to keep it consistent. For modules, set it when building the module.

* Declare operators and customization before the first reflection use, consistently across translation units. Operators whose constraints depend on reflection need an explicit mode.

* Operator templates with deduced return types can fail to compile during detection. Constrain unrelated templates to exclude enums, or set an explicit mode.

* Matching operator templates constrained only by `std::is_enum` select flags for every enum in their namespace. Restrict them to flag enums or set `is_flags = false` for the others.

* Detection does not inspect the values. Enums containing codes or multibit fields may need `is_flags = false`.

* Only a named zero and named single bits are reflected. For signed enums, the sign bit comes first in value order. Use name customization to hide unwanted values.

* Define `MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS` for the whole build to leave a named zero out of flags. Zero still represents an empty mask, but `enum_flags_contains` and `enum_flags_cast` reject it. `as_common<>` still reflects its name. Set it in `MAGIC_ENUM_CONFIG_FILE` to keep it consistent. For modules, set it when building the module.

* Combinations such as `Both = A | B` are not flag values. They are formatted as `"A|B"`, and `enum_flags_cast` rejects `"Both"`. To look up a combination by name, use `as_common<>` with `enum_name` or `enum_cast`. With compiler-specific reflection, the combination must be within the common range.

* Names round-trip only if they are unique and contain no separator. For duplicate names, parsing takes the first match in value order.

## Containers

* `array` and `set` use reflected values as keys. For flags, a named zero is a key and combinations are not. Use `set<E>{value}` for a key and `set<E>::from_mask(mask)` for a mask.

* A flag `bitset` has a position only for each nonzero bit. Zero and combinations work as masks, but `test`, `set`, `reset` and `flip` reject them. Use `none()` to check for emptiness. An enum with only a named zero cannot be used with `bitset`.

* Checked operations throw on invalid input, or call `std::abort()` when exceptions are disabled. `operator[]` requires a valid key or position. See [container errors](reference.md#container-helpers).

## Operators

* Put operator macros in the enum's namespace, or the `MAGIC_ENUM_DEFINE_FRIEND_*` variants inside the class that declares the enum. For function-local enums, use the operator namespace inside the function.

* For enums without a fixed underlying type, `~` is undefined behavior if the result is outside the enum's value range. Use `enum class` or an explicit underlying type, for example `enum Permission : unsigned { Read = 1, Write = 2 }`.

* The module exports the operator namespaces, but not the macros. Do not include `magic_enum_bitwise.hpp` or `magic_enum_iostream.hpp` after `import magic_enum`.

* A `using` of an operator namespace affects every enum in scope. Keep it inside a function. See [namespace risks](reference.md#risks-of-operator-namespaces).

## Enum Range

* The scan range applies only to non-flag enums with compiler-specific reflection. Flags and standard reflection ignore it.

* The default range is `MAGIC_ENUM_RANGE_MIN = -128` to `MAGIC_ENUM_RANGE_MAX = 127`. After clamping to the underlying type, bounds must fit `int`, with `min <= max` and `max - min < 65535`.

* Named values at a few points just outside the range cause a compile-time error. This check is not exhaustive. Extend the range, or return `invalid_tag` from `customize::enum_name` to exclude a value. The check is skipped for `as_common<>` with a flags enum. Define `MAGIC_ENUM_NO_CHECK_OUT_OF_RANGE` for the whole build to disable it.

* To change the default range for all enums, define these macros before including magic_enum:

  ```cpp
  #define MAGIC_ENUM_RANGE_MIN 0
  #define MAGIC_ENUM_RANGE_MAX 256
  #include <magic_enum/magic_enum.hpp>
  ```

* To change one enum's range, specialize `magic_enum::customize::enum_range`:

  ```cpp
  #include <magic_enum/magic_enum.hpp>

  enum class number { one = 100, two = 200, three = 300 };

  template <>
  struct magic_enum::customize::enum_range<number> {
    static constexpr int min = 100;
    static constexpr int max = 300;
  };
  ```

* Alternatively, define `magic_enum_define_range_adl(E)` in the enum's namespace or as a friend in its class:

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

  `.flag<true>()` selects flags and `.flag<false>()` selects common reflection. Without `.flag`, the mode is automatic, including after `.minmax<...>()` or `.prefix<...>()`. Flags ignore `.minmax<...>()`.

## Empty Enums

* APIs that read reflected values, such as `enum_values`, `enum_name(value)` and `enum_cast`, fail to compile if no values are reflected.

  With compiler-specific reflection, no values can mean an empty enum, a missing definition, values outside the range or values hidden by customization.

  Standard reflection requires the full enum definition, even for `enum_count`. An opaque declaration is a compile-time error.

* `enum_count` returns 0 when no values are reflected. Use it as a guard:

  ```cpp
  template <typename E>
  std::size_t count_name_chars() {
    std::size_t n = 0;
    if constexpr (magic_enum::enum_count<E>() != 0) {
      for (auto name : magic_enum::enum_names<E>()) {
        n += name.size();
      }
    }
    return n;
  }
  ```

* `MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM` allows enums with no reflected values. Arrays are empty, names are empty, casts and `enum_index` return empty optionals, and membership checks return `false`. `enum_for_each` and `enum_switch` call nothing, and `enum_switch` returns its fallback or a default value. `enum_value(index)` and circular navigation have no valid input.

* `enum_min`, `enum_max`, `enum_fuse` and containers still require reflected values.

* Prefer the `enum_count` guard. Disabling the check can hide a missing definition or a wrong range. Use the same setting in every translation unit and in the module build.

## Aliasing

Aliases share one reflected name. Standard reflection uses the first declaration. With compiler-specific reflection, the choice [depends on the compiler](https://github.com/Neargye/magic_enum/issues/68).

On compilers that follow declaration order, put the desired name first:

```cpp
enum ShapeKind { Box = 0, ConvexBegin = Box, Sphere = 1 };

// magic_enum::enum_cast<ShapeKind>("Box") -> ShapeKind::Box
// magic_enum::enum_cast<ShapeKind>("ConvexBegin") -> nullopt
// magic_enum::enum_name(ShapeKind::ConvexBegin) -> "Box"
```

## NVCC & NVC++

* Enums declared inside a function or nested in a class template are not reflected.

* String views returned by `magic_enum` refer to host storage and are not generally usable in CUDA device code, even with `--expt-relaxed-constexpr`.

## Other Compiler Issues

* If compilation hits the constexpr evaluation limit, increase it with:

  * MSVC: `/constexpr:depthN`, `/constexpr:stepsN` <https://learn.microsoft.com/en-us/cpp/build/reference/constexpr-control-constexpr-evaluation>
  * Clang: `-fconstexpr-depth=N`, `-fconstexpr-steps=N` <https://clang.llvm.org/docs/UsersManual.html#controlling-implementation-limits>
  * GCC: `-fconstexpr-depth=N`, `-fconstexpr-loop-limit=N`, `-fconstexpr-ops-limit=N` <https://gcc.gnu.org/onlinedocs/gcc-9.2.0/gcc/C_002b_002b-Dialect-Options.html>

* Visual Studio's IntelliSense may have problems analyzing magic_enum.

* ReSharper C++ and CLion Nova are supported from version 2025.3.

* Some compilers have problems reflecting enums inside templates. See [#164](https://github.com/Neargye/magic_enum/issues/164) and [#65](https://github.com/Neargye/magic_enum/issues/65).

* With Clang 18 and `import magic_enum`, an enum without a fixed underlying type fails to compile. Use a fixed type, for example `enum E : int`, or `-Wno-enum-constexpr-conversion`.
