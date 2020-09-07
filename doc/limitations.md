# Limitations

* To check is magic_enum supported compiler use macro `MAGIC_ENUM_SUPPORTED` or constexpr constant `magic_enum::is_magic_enum_supported`.

* This library uses a compiler-specific hack (based on `__PRETTY_FUNCTION__` / `__FUNCSIG__`), which works on Clang >= 5, MSVC >= 15.3 and GCC >= 9.

* Enum can't reflect if the enum is a forward declaration.

* Enum value must be in range `[MAGIC_ENUM_RANGE_MIN, MAGIC_ENUM_RANGE_MAX]`.

  * By default `MAGIC_ENUM_RANGE_MIN = -128`, `MAGIC_ENUM_RANGE_MAX = 128`.

  * `MAGIC_ENUM_RANGE_MIN` must be less or equals than `0` and must be greater than `INT16_MIN`.

  * `MAGIC_ENUM_RANGE_MAX` must be greater than `0` and must be less than `INT16_MAX`.

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

    namespace magic_enum::customize {
    template <>
    struct enum_range<number> {
      static constexpr int min = 100; // Must be greater than `INT16_MIN`.
      static constexpr int max = 300; // Must be less than `INT16_MAX`.
    };
    } // namespace magic_enum
    ```

* `magic_enum` won't work if a value is aliased. Work with enum-aliases is compiler-implementation-defined.

  ```cpp
  enum ShapeKind {
    ConvexBegin = 0,
    Box = 0, // Won't work.
    Sphere = 1,
    ConvexEnd = 2,
    Donut = 2, // Won't work too.
    Banana = 3,
    COUNT = 4,
  };
  // magic_enum::enum_cast<ShapeKind>("Box") -> std::nullopt or ShapeKind::Box
  // magic_enum::enum_name(ShapeKind::Box) -> "ConvexBegin" or ""
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
    ConvexEnd = Sphere + 1,
  };
  // magic_enum::enum_cast<ShapeKind>("Box") -> ShapeKind::Box
  // magic_enum::enum_name(ShapeKind::Box) -> "Box"

  // Non-reflected aliases.
  // magic_enum::enum_cast<ShapeKind>("ConvexBegin") -> std::nullopt
  // magic_enum::enum_name(ShapeKind::ConvexBegin) -> "Box"
  ```

  On some compiler enum-aliases not supported, [for example Visual Studio 2017](https://github.com/Neargye/magic_enum/issues/36).

  It is possible to check whether enum-aliases supported using a macro `MAGIC_ENUM_SUPPORTED_ALIASES`.

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
