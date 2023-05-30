catch2_dep = declare_dependency(include_directories: '3rdparty/Catch2/include')

test_files = {
    'basic test': files('test.cpp'),
    'flags test': files('test_flags.cpp'),
}

foreach test_name, test_src : test_files
    test_exe = executable(
        test_name.underscorify(),
        test_src,

        dependencies: [magic_enum_dep, catch2_dep],
    )

    test(test_name, test_exe)
endforeach
