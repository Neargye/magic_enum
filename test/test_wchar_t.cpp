#define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = std::wstring_view;
#define MAGIC_ENUM_USING_ALIAS_STRING      using string      = std::wstring;

#if defined(MAGIC_ENUM_ENABLE_NONASCII)
#undef MAGIC_ENUM_ENABLE_NONASCII
#endif

#define MAKE_STR(v) L##v

#include "test.cpp"
