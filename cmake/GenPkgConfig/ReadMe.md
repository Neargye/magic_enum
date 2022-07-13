GenPkgConfig.cmake
===================

A script generating pkg-config files.

WARNING: CMake [is currently merging own built-in pkgconfig generation implementation](https://gitlab.kitware.com/cmake/cmake/-/merge_requests/6363)!

If you require such a new version of CMake, you probably should use the built-in impl instead.

Syntax
------

```cmake
configure_pkg_config_file(<targetName>
	NAME <name of the pkgconfig package>
	VERSION <version to be written into the package>
	DESCRIPTION <description to be written into the package>
	URL <homepage URL to be written into the package>
	COMPONENT <install as the component>
	INSTALL_LIB_DIR <path to something like CMAKE_INSTALL_LIBDIR>
	INSTALL_INCLUDE_DIR <path to something like CMAKE_INSTALL_INCLUDEDIR>
	REQUIRES ... <list of pkg-config packages this one depends on> ...
	REQUIRES ... <list of pkg-config packages this one conflicts with> ...
)
```

Issuees
-------

1. For `OBJECT` targets we have run into big issues. CMake
	1. Doesn't allow to get the list of object files at configure time
	2. Allows to get a list of object files as a generator exression ...
	3. BUT ... the path to them is full, but we need only file name!
	4. CMake doesn't allow to strip directory path via generator expression
	5. ... neither it allows string editing within generator expressions ...

	so ... we have to create a custom target using a custom CMake script executed separately, but ...

	6. `file(GENERATE` doesn't properly register dependencies
	... so we have to use `add_custom_command` to say CMake that these files are generated
	
	7. And CMake `install(FILES` doesn't mean that the targets generating these files are automatically executed, 
	
	So we have to use `ALL` in `add_custom_target`.

