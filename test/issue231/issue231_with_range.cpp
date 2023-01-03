#include "enum.h"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <magic_enum.hpp>

TEST_CASE("enum_name_via_proxy") {
  REQUIRE("low"  == to_string(flag::low));
  REQUIRE("high" == to_string(flag::high));
}

void unused_function_that_triggers_the_issue() {
  const auto values = magic_enum::enum_values<flag>();
  (void)values;
}
