# -*- coding: utf-8 -*-

from conans import ConanFile

class MagicEnumConan(ConanFile):
    name = 'magic_enum'
    version = '0.6.0'
    description = 'Header-only C++17 library provides static reflection for enums, work with any enum type without any macro or boilerplate code.'
    topics = (
        'conan',
        'cplusplus',
        'enum-to-string',
        'string-to-enum'
        'serialization',
        'reflection',
        'header-only',
        'compile-time'
    )
    url = 'https://github.com/Neargye/magic_enum'
    homepage = 'https://github.com/Neargye/magic_enum'
    author = 'Daniil Goncharov <neargye@gmail.com>'
    license = 'MIT'
    exports_sources = ["include/*", "LICENCE"]
    exports = ["LICENSE"]
    no_copy_source = True

    def package(self):
        self.copy("include/*")
        self.copy("LICENSE", dst="licenses")

    def package_id(self):
        self.info.header_only()
