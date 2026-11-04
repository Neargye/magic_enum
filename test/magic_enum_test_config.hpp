// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#pragma once

// Shared configuration for the core test TUs and their module variant.

#define MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM
#define MAGIC_ENUM_RANGE_MIN -120
#define MAGIC_ENUM_RANGE_MAX 120

// Detail symbols exported by the test module.
#define MAGIC_ENUM_EXPORT_DETAIL \
  using detail::reflected_min; \
  using detail::reflected_max; \
  using detail::min_v; \
  using detail::max_v; \
  using detail::is_reflected_v; \
  using detail::cmp_less;
