// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT

#if MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 1

#  define MAGIC_ENUM_NO_ASSERT
#  define MAGIC_ENUM_ASSERT(...) static_cast<void>(0)
#  include <magic_enum/magic_enum.hpp>

#elif MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE == 2

#  include <magic_enum/magic_enum.hpp>

enum class InvalidFlagsSetting { value };

template <>
struct magic_enum::customize::enum_range<InvalidFlagsSetting> {
  static constexpr int is_flags = 1;
};

static_assert(magic_enum::enum_count<InvalidFlagsSetting>() == 1);

#else
#  error Unknown MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE.
#endif

int main() {}
