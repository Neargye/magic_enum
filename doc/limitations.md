# Limitations

* To check is magic_enum supported compiler use macro `MAGIC_ENUM_SUPPORTED` or constexpr constant `magic_enum::is_magic_enum_supported`.

* This library uses a compiler-specific hack (based on `__PRETTY_FUNCTION__` / `__FUNCSIG__`), which works on Clang >= 5, MSVC >= 15.3 and GCC >= 9.

* Enum can't reflect if the enum is a forward declaration.

* Enum value must be in range `[MAGIC_ENUM_RANGE_MIN, MAGIC_ENUM_RANGE_MAX]`. By default `MAGIC_ENUM_RANGE_MIN = -128`, `MAGIC_ENUM_RANGE_MAX = 128`.

  If need another range for all enum types by default, redefine the macro `MAGIC_ENUM_RANGE_MIN` and `MAGIC_ENUM_RANGE_MAX`.
  ```cpp
  #define MAGIC_ENUM_RANGE_MIN 0
  #define MAGIC_ENUM_RANGE_MAX 256
  #include <magic_enum.hpp>
  ```

  If need another range for specific enum type, add specialization `enum_range` for necessary enum type.
  ```cpp
  #include <magic_enum.hpp>

  enum number { one = 100, two = 200, three = 300 };

  namespace magic_enum {
  template <>
  struct enum_range<number> {
    static constexpr int min = 100;
    static constexpr int max = 300;
  };
  }
  ```

* `magic_enum` obtains the first defined value enums, and won't work if value are aliased.
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
  // magic_enum::enum_cast<ShapeKind>("Box") -> std::nullopt
  // magic_enum::enum_name(ShapeKind::Box) -> "ConvexBegin"
  ```
  Work around the issue:
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
