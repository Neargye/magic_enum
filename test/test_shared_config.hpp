// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#pragma once

#include <magic_enum/magic_enum.hpp>

#include <cstddef>

namespace shared_config {
enum class Value { Low = 0, High = 500 };
magic_enum::string_view name_from_other_tu(Value value);
std::size_t count_from_other_tu();
} // namespace shared_config

template <>
struct magic_enum::customize::enum_range<shared_config::Value> {
  static constexpr int min = 0;
  static constexpr int max = 500;
};

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<shared_config::Value>(shared_config::Value value) noexcept {
  return value == shared_config::Value::High ? magic_enum::customize::customize_t{"upper"} : default_tag;
}
