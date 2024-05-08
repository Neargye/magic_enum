module;

#include <magic_enum_all.hpp>

export module magic_enum;

export namespace magic_enum {
namespace customize {
    using customize::enum_range;
}

namespace iostream_operators {
    using iostream_operators::operator<<;
    using iostream_operators::operator>>;
}

namespace bitwise_operators {
    using bitwise_operators::operator~;
    using bitwise_operators::operator|;
    using bitwise_operators::operator&;
    using bitwise_operators::operator^;
    using bitwise_operators::operator|=;
    using bitwise_operators::operator&=;
    using bitwise_operators::operator^=;
}

namespace containers {
    using containers::array;
    using containers::bitset;
    using containers::set;
}

    using magic_enum::enum_name;
    using magic_enum::enum_cast;
    using magic_enum::enum_value;
    using magic_enum::enum_values;
    using magic_enum::enum_count;
    using magic_enum::enum_integer;
    using magic_enum::enum_names;
    using magic_enum::enum_entries;
    using magic_enum::enum_fuse;
    using magic_enum::enum_switch;
    using magic_enum::enum_for_each;
    using magic_enum::enum_contains;
    using magic_enum::enum_index;
    using magic_enum::enum_flags_name;
    using magic_enum::enum_flags_contains;
    using magic_enum::enum_flags_cast;
    using magic_enum::enum_type_name;
    using magic_enum::is_unscoped_enum;
    using magic_enum::is_unscoped_enum_v;
    using magic_enum::is_scoped_enum;
    using magic_enum::is_scoped_enum_v;
    using magic_enum::underlying_type;
    using magic_enum::underlying_type_t;
}
