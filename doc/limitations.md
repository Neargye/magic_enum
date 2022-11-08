# Limitations

* This library uses a compiler-specific hack (based on `__PRETTY_FUNCTION__` / `__FUNCSIG__`).

* To check is magic_enum supported compiler use macro `MAGIC_ENUM_SUPPORTED` or constexpr constant `magic_enum::is_magic_enum_supported`.</br>
  If magic_enum used on unsupported compiler, occurs the compilation error. To suppress error define macro `MAGIC_ENUM_NO_CHECK_SUPPORT`.

* Enum can't reflect if the enum is a forward declaration.

* For enum-flags add `is_flags` to specialization `enum_range` for necessary enum type. Specialization of `enum_range` must be injected in `namespace magic_enum::customize`.
  ```cpp
  enum class Directions { Up = 1 << 1, Down = 1 << 2, Right = 1 << 3, Left = 1 << 4 };
  template <>
  struct magic_enum::customize::enum_range<Directions> {
    static constexpr bool is_flags = true;
  };
  ```

* Enum value must be in range `[MAGIC_ENUM_RANGE_MIN, MAGIC_ENUM_RANGE_MAX]`.

  * By default `MAGIC_ENUM_RANGE_MIN = -128`, `MAGIC_ENUM_RANGE_MAX = 128`.

  * `MAGIC_ENUM_RANGE = (MAGIC_ENUM_RANGE_MAX - MAGIC_ENUM_RANGE_MIN)` must be less than `UINT16_MAX`.

  * If need another range for all enum types by default, redefine the macro `MAGIC_ENUM_RANGE_MIN` and `MAGIC_ENUM_RANGE_MAX`.

    ```cpp
    #define MAGIC_ENUM_RANGE_MIN 0
    #define MAGIC_ENUM_RANGE_MAX 256
    #include <magic_enum.hpp>
    ```

  * If need another range for specific enum type, add specialization `enum_range` for necessary enum type. Specialization of `enum_range` must be injected in `namespace magic_enum::customize`.

    ```cpp
    #include <magic_enum.hpp>

    enum class number { one = 100, two = 200, three = 300 };

    template <>
    struct magic_enum::customize::enum_range<number> {
      static constexpr int min = 100;
      static constexpr int max = 300;
      // (max - min) must be less than UINT16_MAX.
    };
    ```

* `magic_enum` [won't work if a value is aliased](https://github.com/Neargye/magic_enum/issues/68). Work with enum-aliases is compiler-implementation-defined.

  ```cpp
  enum ShapeKind {
    ConvexBegin = 0,
    Box = 0, // Won't work.
    Sphere = 1,
    ConvexEnd = 2,
    Donut = 2, // Won't work too.
    Banana = 3,
    COUNT = 4
  };
  // magic_enum::enum_cast<ShapeKind>("Box") -> nullopt
  // magic_enum::enum_name(ShapeKind::Box) -> "ConvexBegin"
  ```

  One of the possible workaround the issue:

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

  On some compiler enum-aliases not supported, [for example Visual Studio 2017](https://github.com/Neargye/magic_enum/issues/36), macro `MAGIC_ENUM_SUPPORTED_ALIASES` will be undefined.

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

* If you hit a message like this:

  ```text
  [...]
  note: constexpr evaluation hit maximum step limit; possible infinite loop?
  ```

  Change the limit for the number of constexpr evaluated:
  * MSVC `/constexpr:depthN`, `/constexpr:stepsN` <https://docs.microsoft.com/en-us/cpp/build/reference/constexpr-control-constexpr-evaluation>
  * Clang `-fconstexpr-depth=N`, `-fconstexpr-steps=N` <https://clang.llvm.org/docs/UsersManual.html#controlling-implementation-limits>
  * GCC `-fconstexpr-depth=N`, `-fconstexpr-loop-limit=N`, `-fconstexpr-ops-limit=N` <https://gcc.gnu.org/onlinedocs/gcc-9.2.0/gcc/C_002b_002b-Dialect-Options.html>

* Intellisense Visual Studio may have some problems analyzing `magic_enum`.

* Enums in template may work incorrectly (especially on Сlang)
  See (#164)[https://github.com/Neargye/magic_enum/issues/164], (#65)[https://github.com/Neargye/magic_enum/issues/65]
