# GenPkgConfig.cmake

`GenPkgConfig.cmake` generates and installs pkg-config metadata for CMake
interface, object, static, and shared library targets. It requires CMake 3.22
or newer.

The module was added to nameof and magic_enum through
[Neargye/nameof#44](https://github.com/Neargye/nameof/pull/44) and
[Neargye/magic_enum#202](https://github.com/Neargye/magic_enum/pull/202).

## Usage

```cmake
configure_pkg_config_file(<target>
  NAME <package name>
  VERSION <package version>
  DESCRIPTION <package description>
  URL <package homepage>
  COMPONENT <install component>
  INSTALL_LIB_DIR <library install directory>
  INSTALL_INCLUDE_DIR <include install directory>
  REQUIRES <required pkg-config packages> ...
  CONFLICTS <conflicting pkg-config packages> ...
)
```

`NAME` defaults to the target name. `VERSION` and `DESCRIPTION` fall back to
project or CPack metadata and are required after fallback. Unknown arguments
and keywords without values are rejected.

Install directories must be relative. Generated paths are based on
`${pcfiledir}`, so install-time `cmake --install --prefix` overrides and
`DESTDIR` staging remain relocatable.

Call `configure_pkg_config_file()` after setting the target's public include
directories. Dependencies are not inferred from CMake target properties; pass
pkg-config dependencies explicitly with `REQUIRES`.

The caller must install static and shared targets into `INSTALL_LIB_DIR`.
Object files are installed by this module and listed in `Libs`; their basenames
must be unique, including case-insensitively on Windows.
