#pragma once

#include <string>
#include <string_view>

// Configuration header for the wchar_t module test build.
// Redefines string types to their wide-character equivalents.
// Used via MAGIC_ENUM_CONFIG_FILE to avoid CMake semicolon-escaping in compile definitions.
#define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = std::wstring_view;
#define MAGIC_ENUM_USING_ALIAS_STRING using string = std::wstring;
