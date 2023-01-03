#include "enum.h"
#include <magic_enum.hpp>

template <>
struct magic_enum::customize::enum_range<flag>
{
    static constexpr int min = static_cast<int>(flag::low);
    static constexpr int max = static_cast<int>(flag::high);

    static_assert(static_cast<uint64_t>(max) - static_cast<uint64_t>(min) < std::numeric_limits<uint16_t>::max(),
                  "magic_enum only supports names for the uint16_t range.");
};

std::string_view to_string(flag f){
    return magic_enum::enum_name(f);
}
