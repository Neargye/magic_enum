// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include "test_shared_config.hpp"

// #231: every reflecting TU sees the same enum definition and customization.
int main() {
  using shared_config::Value;
  if (magic_enum::enum_count<Value>() != 2 || shared_config::count_from_other_tu() != 2) {
    return 1;
  }
  const auto& values = magic_enum::enum_values<Value>();
  const auto& names = magic_enum::enum_names<Value>();
  if (values[0] != Value::Low || values[1] != Value::High || names[0] != "Low" || names[1] != "upper") {
    return 2;
  }
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (magic_enum::enum_name(values[i]) != names[i] || shared_config::name_from_other_tu(values[i]) != names[i]) {
      return 3;
    }
    if (magic_enum::enum_cast<Value>(names[i]) != values[i]) {
      return 4;
    }
  }
  return 0;
}
