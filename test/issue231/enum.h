#ifndef EXAMPLE_ENUM_H
#define EXAMPLE_ENUM_H

#include <string_view>

enum class flag
{
    low = 0,
    high = 500,
};

std::string_view to_string(flag);

#endif // EXAMPLE_ENUM_H