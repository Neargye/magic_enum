# Limitations

* This library uses a compiler-specific hack based on `__PRETTY_FUNCTION__` / `__FUNCSIG__`.

* Use `MAGIC_ENUM_SUPPORTED` or `magic_enum::is_magic_enum_supported` to check compiler support. Unsupported compilers cause compilation errors unless `MAGIC_ENUM_NO_CHECK_SUPPORT` is defined.

* magic_enum cannot reflect forward-declared enums.

## Enum Flags

* Set `enum_range<E>::is_flags` to `true` to use flag semantics for `E` by default. `enum_flags_*` APIs always use flag semantics.
  ```cpp
  enum class Directions { Up = 1 << 0, Down = 1 << 1, Right = 1 << 2, Left = 1 << 3 };
  template <>
  struct magic_enum::customize::enum_range<Directions> {
    static constexpr bool is_flags = true;
  };
  ```

* `MAGIC_ENUM_RANGE_MIN` / `MAGIC_ENUM_RANGE_MAX` do not control flag reflection. Flag reflection scans bit positions available in `E`'s underlying type.

* Zero is not reflected for flag enums.

## Enum Range

* For non-flag enums, range-based reflection only considers values in `[MAGIC_ENUM_RANGE_MIN, MAGIC_ENUM_RANGE_MAX]`.

* By default, `MAGIC_ENUM_RANGE_MIN = -128`, `MAGIC_ENUM_RANGE_MAX = 127`.

* To change default range for all enum types, redefine `MAGIC_ENUM_RANGE_MIN` and `MAGIC_ENUM_RANGE_MAX`:

    ```cpp
    #define MAGIC_ENUM_RANGE_MIN 0
    #define MAGIC_ENUM_RANGE_MAX 256
    #include <magic_enum/magic_enum.hpp>
    ```

* To change range for one enum type, specialize `enum_range` for that type. Specializations must be declared in `namespace magic_enum::customize`.

  ```cpp
  #include <magic_enum/magic_enum.hpp>

  enum class number { one = 100, two = 200, three = 300 };

  template <>
  struct magic_enum::customize::enum_range<number> {
    static constexpr int min = 100;
    static constexpr int max = 300;
    // (max - min) must be less than UINT16_MAX.
  };
  ```

* For ADL customization, define `magic_enum_define_range_adl(my_enum_type)` in associated namespace or as friend of associated class. Return `magic_enum::customize::adl_info()`:

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

  For flag enums, add `.flag<true>()`; `.minmax<...>()` is ignored.

## Aliasing

magic_enum [cannot reliably distinguish aliased enumerators](https://github.com/Neargye/magic_enum/issues/68). Its behavior with aliases is compiler-dependent.

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

One possible workaround for the issue is to define the enum values you want reflected before their aliases:

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

On compilers without enum alias support, [such as Visual Studio 2017](https://github.com/Neargye/magic_enum/issues/36), `MAGIC_ENUM_SUPPORTED_ALIASES` is not defined.

```cpp
enum Number {
  one = 1,
  ONE = 1
};
// magic_enum::enum_cast<Number>("one") -> nullopt
// magic_enum::enum_name(Number::one) -> ""
// magic_enum::enum_cast<Number>("ONE") -> nullopt
// magic_enum::enum_name(Number::ONE) -> ""
```

## Other Compiler Issues

* If you hit a message like this:

  ```text
  [...]
  note: constexpr evaluation hit maximum step limit; possible infinite loop?
  ```

  Increase constexpr evaluation limit:

  * MSVC: `/constexpr:depthN`, `/constexpr:stepsN` <https://docs.microsoft.com/en-us/cpp/build/reference/constexpr-control-constexpr-evaluation>
  * Clang: `-fconstexpr-depth=N`, `-fconstexpr-steps=N` <https://clang.llvm.org/docs/UsersManual.html#controlling-implementation-limits>
  * GCC: `-fconstexpr-depth=N`, `-fconstexpr-loop-limit=N`, `-fconstexpr-ops-limit=N` <https://gcc.gnu.org/onlinedocs/gcc-9.2.0/gcc/C_002b_002b-Dialect-Options.html>

* Visual Studio's IntelliSense may have problems analyzing magic_enum.

* Enums in templates may not work correctly, especially on Clang. See [#164](https://github.com/Neargye/magic_enum/issues/164) and [#65](https://github.com/Neargye/magic_enum/issues/65).
