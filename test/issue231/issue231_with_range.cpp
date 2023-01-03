#include "enum.h"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <magic_enum.hpp>

TEST_CASE("enum_name_via_proxy") {
  for(auto v: magic_enum::enum_values<flag>()) {
    (void)v;
  }
  REQUIRE("low"  == to_string(flag::low));
  REQUIRE("high" == to_string(flag::high));
}
