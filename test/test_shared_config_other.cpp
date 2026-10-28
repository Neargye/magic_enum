// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include "test_shared_config.hpp"

magic_enum::string_view shared_config::name_from_other_tu(Value value) {
  return magic_enum::enum_name(value);
}

std::size_t shared_config::count_from_other_tu() {
  return magic_enum::enum_count<Value>();
}
