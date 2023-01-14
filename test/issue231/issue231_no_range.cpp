#include "enum.h"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <magic_enum.hpp>

TEST_CASE("enum_name_via_proxy") {
  REQUIRE(magic_enum::enum_count<flag>() == 2);
  REQUIRE("low"  == to_string(flag::low));
  REQUIRE("high" == to_string(flag::high));
}
