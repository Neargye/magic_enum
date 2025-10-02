project(
    'magic_enum', ['cpp'],
    default_options: ['cpp_std=c++17'],
    version: '0.9.7',
)

magic_enum_include = include_directories('include')

magic_enum_args = []

if get_option('hash')
    magic_enum_args += '-DMAGIC_ENUM_ENABLE_HASH'
endif

magic_enum_dep = declare_dependency(
    include_directories: magic_enum_include,
    compile_args: magic_enum_args,
)

# install header and pkg-config file
install_subdir('include/magic_enum', install_dir: get_option('includedir'))
pkg = import('pkgconfig')
pkg.generate(
  name: 'magic_enum',
  description: 'A library that provides static reflection for enums, work with any enum type without any macro or boilerplate code.',
  url: 'https://github.com/Neargye/magic_enum',
  extra_cflags: magic_enum_args,
)

subdir('test')
