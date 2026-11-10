// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT

#ifndef MAGIC_ENUM_TEST_ADL_HPP
#define MAGIC_ENUM_TEST_ADL_HPP

namespace adl_lookup_test {
enum class Wrong { A = 1, B = 2, All = 3 };
enum class Right { A = 1, B = 2, All = 3 };
struct Config { static constexpr bool is_flags = true; };
}
enum class GlobalAdl { A = 1, B = 2, All = 3 };

#ifdef MAGIC_ENUM_TEST_ADL_BEFORE_INCLUDE
adl_lookup_test::Config magic_enum_define_range_adl(adl_lookup_test::Wrong);
adl_lookup_test::Config magic_enum_define_range_adl(GlobalAdl);
namespace adl_lookup_test { Config magic_enum_define_range_adl(Right); }
namespace unrelated_operators {
template <typename E> E operator|(E, E);
template <typename E> E operator&(E, E);
}
using namespace unrelated_operators;
#endif

#include "test_enums.hpp"

#ifndef MAGIC_ENUM_TEST_ADL_BEFORE_INCLUDE
adl_lookup_test::Config magic_enum_define_range_adl(adl_lookup_test::Wrong);
adl_lookup_test::Config magic_enum_define_range_adl(GlobalAdl);
namespace adl_lookup_test { Config magic_enum_define_range_adl(Right); }
#endif

static_assert(!magic_enum::is_flags_enum_v<adl_lookup_test::Wrong>);
static_assert(magic_enum::is_flags_enum_v<adl_lookup_test::Right>);
static_assert(magic_enum::is_flags_enum_v<GlobalAdl>);
bool adl_lookup_in_other_tu();

#endif
