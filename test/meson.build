doctest = static_library(
    'doctest', 'test_main.cpp',
    include_directories: '3rdparty',
    build_by_default: false,
)
doctest_dep = declare_dependency(include_directories: '3rdparty', link_with: doctest)

test_files = {
    'basic test': files('test.cpp'),
    'aliases test': files('test_aliases.cpp'),
    'containers test': files('test_containers.cpp'),
    'flags test': files('test_flags.cpp'),
    'single range test': files('test_range.cpp'),
    'wchar_t test': files('test_wchar_t.cpp'),
}

foreach test_name, test_src : test_files
    test_exe = executable(
        test_name.underscorify(),
        test_src,

        build_by_default: false,
        dependencies: [magic_enum_dep, doctest_dep],
    )

    test(test_name, test_exe)
endforeach
