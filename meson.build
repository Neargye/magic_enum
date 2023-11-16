project(
    'magic_enum', ['cpp'],
    default_options: ['cpp_std=c++17'],
    version: '0.9.5',
)

magic_enum_include = include_directories('include/magic_enum')

magic_enum_args = []

if get_option('hash')
    magic_enum_args += '-DMAGIC_ENUM_ENABLE_HASH'
endif

magic_enum_dep = declare_dependency(
    include_directories: magic_enum_include,
    compile_args: magic_enum_args,
)

if get_option('test')
    subdir('test')
endif
