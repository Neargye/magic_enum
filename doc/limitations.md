# Limitations

Without standard reflection, magic_enum reads compiler signatures to discover enum names.

With both backends, customization can rename or hide declared values, but cannot add new ones. A custom name for an undeclared zero fails to compile. A custom name for another undeclared value is ignored, and `enum_name<V>()` for such a value fails to compile.

## C++26 Standard Reflection

Standard reflection is selected automatically when available. Otherwise magic_enum uses compiler-specific reflection.

* Standard reflection ignores scan ranges and finds every declared value. Counts, indices and names can differ from compiler-specific reflection.

* Each value has one name, taken from its first declaration.

* Define `MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION` before including magic_enum to keep compiler-specific reflection.

* Reflecting values requires the enum definition. See [empty enums](#empty-enums) for opaque declarations.

* Use the same enum definitions and configuration in every translation unit and in the module build.

* `enum_reflected` checks whether a value can be reflected, not whether it has a name. See [`enum_reflected`](reference.md#enum_reflected).

## Enum Flags

* Flags are selected automatically when ADL finds both `E | E` and `E & E` for rvalues returning exactly `E`. Operator macros placed next to the enum satisfy this rule. `using namespace`, built-in integer operators and operators returning a wrapper do not.

* Detection changes results without a compile error for enums that already have operators. Counts and indices can change, and named combinations are no longer values. See the migration note below.

* Set `enum_range<E>::is_flags` explicitly to override detection. Use `true` for flags without matching operators, or `false` for ordinary reflection. `enum_flags_*` APIs always use flags.
  ```cpp
  enum class Directions { Up = 1 << 0, Down = 1 << 1, Right = 1 << 2, Left = 1 << 3 };
  template <>
  struct magic_enum::customize::enum_range<Directions> {
    static constexpr bool is_flags = true;
  };
  ```

* Define `MAGIC_ENUM_NO_AUTO_IS_FLAGS` for the whole build to disable automatic detection. Explicit `is_flags = true`, `.flag<true>()` and `as_flags<>` still work. For modules, configure the module build, defining the macro after `import` has no effect.

* Declare operators and customization before the first reflection use, consistently across translation units. Operators whose constraints depend on reflection need an explicit mode.

* Detection checks `E | E` only when `E & E` returns `E`. An unconstrained `operator&` or `operator|` template with a deduced return type that is selected for `E` is instantiated and can fail with a hard error. Constrain such templates, for example pipe operators, to non-enum types or set an explicit mode.

* An operator template constrained only by `std::is_enum` selects flags for every enum in its namespace. Constrain it to flag enums or set `is_flags = false` for the others.

* Detection follows the operators, not the meaning of the values. Enums containing codes or multibit fields may need `is_flags = false`. Named single bits, including implementation-specific values, are reflected unless excluded through name customization.

* `MAGIC_ENUM_RANGE_MIN` / `MAGIC_ENUM_RANGE_MAX` do not control flag reflection.

* Only a named zero and named single bits are reflected.

* For signed enums, a named sign bit is reflected too. It comes first, since it is negative.

* Combinations such as `Both = A | B` are not flag values. They are formatted as `"A|B"`, and `enum_flags_cast` rejects `"Both"`. To look up a combination by name, use `as_common<>` with `enum_name` or `enum_cast`. With compiler-specific reflection, the combination must be within the common range.

* Names round-trip only if they are unique and do not contain the separator. Otherwise parsing takes the first match in value order.

* When migrating, check names, counts, indices and container sizes for enums that now select flags. For example, `std::chars_format::general` has a common name but no single-bit flag name. `enum_name` returns empty in flags mode, while `enum_flags_name` returns `"scientific|fixed"`. Use `as_common<>` for an individual lookup or disable automatic detection.

## Containers

* `array` and `set` use reflected values as keys. For flags, a named zero is a key and combinations are not. Use `set<E>{value}` for a key and `set<E>::from_mask(mask)` for a mask.

* A flag `bitset` has a position only for each nonzero bit. Zero and combinations work as masks, but `test`, `set`, `reset` and `flip` reject them. Use `none()` to check for emptiness. An enum with only a named zero cannot be used with `bitset`.

* Invalid keys, masks and positions throw, or call `std::abort()` when exceptions are disabled. See [container errors](reference.md#container-helpers).

## Operators

* Put operator macros in the enum's namespace, or the `MAGIC_ENUM_DEFINE_FRIEND_*` variants inside the class that declares the enum. For function-local enums, use the operator namespace inside the function.

* For enums without a fixed underlying type, `~` is undefined behavior if the result is outside the enum's value range. Use `enum class` or an explicit underlying type, for example `enum Permission : unsigned { Read = 1, Write = 2 }`.

* The module exports the operator namespaces, but not the macros. Do not include `magic_enum_bitwise.hpp` or `magic_enum_iostream.hpp` after `import magic_enum`.

* A `using` of an operator namespace affects every enum in scope. Keep it inside a function. See [namespace risks](reference.md#risks-of-operator-namespaces).

## Enum Range

* The scan range applies only to non-flag enums with compiler-specific reflection. Flags and standard reflection ignore it.

* The default range is `MAGIC_ENUM_RANGE_MIN = -128` to `MAGIC_ENUM_RANGE_MAX = 127`.

* A named value at some points just outside the range is a compilation error, since it usually means that the range is too small. Only a few values near the bounds are checked, other values outside the range are silently not reflected. To exclude a value on purpose, return `invalid_tag` for it from `customize::enum_name`. The check is skipped for `as_common<>` with a flags enum.

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
    // Bounds are clamped to the underlying type. Requires min <= max and max - min < 65535.
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

* APIs that read reflected values, such as `enum_values`, `enum_name` and `enum_cast`, fail to compile if no values are reflected.

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

Aliases share one reflected name. Standard reflection uses the first declaration. With compiler-specific reflection it [depends on the compiler](https://github.com/Neargye/magic_enum/issues/68). The examples below assume the first declaration:

```cpp
enum ShapeKind {
  ConvexBegin = 0,
  Box = 0, // Won't work.
  Sphere = 1,
  ConvexEnd = 2,
  Donut = 2, // Won't work either.
  Banana = 3,
  COUNT = 4
};
// magic_enum::enum_cast<ShapeKind>("Box") -> nullopt
// magic_enum::enum_name(ShapeKind::Box) -> "ConvexBegin"
```

Place the desired name before its aliases:

```cpp
enum ShapeKind {
  // Convex shapes, see ConvexBegin and ConvexEnd below.
  Box = 0,
  Sphere = 1,

  // Non-convex shapes.
  Donut = 2,
  Banana = 3,

  COUNT = Banana + 1,

  // Non-reflected aliases.
  ConvexBegin = Box,
  ConvexEnd = Sphere + 1
};
// magic_enum::enum_cast<ShapeKind>("Box") -> ShapeKind::Box
// magic_enum::enum_name(ShapeKind::Box) -> "Box"

// Non-reflected aliases.
// magic_enum::enum_cast<ShapeKind>("ConvexBegin") -> nullopt
// magic_enum::enum_name(ShapeKind::ConvexBegin) -> "Box"
```

## NVCC & NVC++

* Enums declared inside a function or nested in a class template are not reflected.

* With a Clang 20+ host compiler, `nvcc` does not support unscoped enums without a fixed underlying type.

## Other Compiler Issues

* If compilation hits the constexpr evaluation limit:

  ```text
  [...]
  note: constexpr evaluation hit maximum step limit
  ```

  Increase the compiler's constexpr limit:

  * MSVC: `/constexpr:depthN`, `/constexpr:stepsN` <https://learn.microsoft.com/en-us/cpp/build/reference/constexpr-control-constexpr-evaluation>
  * Clang: `-fconstexpr-depth=N`, `-fconstexpr-steps=N` <https://clang.llvm.org/docs/UsersManual.html#controlling-implementation-limits>
  * GCC: `-fconstexpr-depth=N`, `-fconstexpr-loop-limit=N`, `-fconstexpr-ops-limit=N` <https://gcc.gnu.org/onlinedocs/gcc-9.2.0/gcc/C_002b_002b-Dialect-Options.html>

* Visual Studio's IntelliSense may have problems analyzing magic_enum.

* Some compilers have problems reflecting enums inside templates. See [#164](https://github.com/Neargye/magic_enum/issues/164) and [#65](https://github.com/Neargye/magic_enum/issues/65).
