#pragma once

// Configuration header used when building the magic_enum module target for tests.
// Defines test-specific range overrides and exports detail symbols required by test.cpp.

#define MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM
#define MAGIC_ENUM_RANGE_MIN -120
#define MAGIC_ENUM_RANGE_MAX 120

// Export detail symbols required by test.cpp internals.
// Only meaningful when building as a module (MAGIC_ENUM_EXPORT_DETAIL is consumed by
// magic_enum.cppm's "export namespace magic_enum { namespace detail { ... } }" block).
#define MAGIC_ENUM_EXPORT_DETAIL \
  using detail::reflected_min; \
  using detail::reflected_max; \
  using detail::min_v; \
  using detail::max_v; \
  using detail::is_reflected_v; \
  using detail::cmp_less;
