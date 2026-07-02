#pragma once

#include <doctest/doctest.h>

#include <cstddef>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

namespace magic_enum_tests {

template <typename View>
void require_null_terminated(View value) {
  using char_type = typename View::value_type;

  REQUIRE(value.data() != nullptr);
  REQUIRE(value.data()[value.size()] == char_type{});
}

template <typename View>
void require_null_terminated(View value, const typename View::value_type* expected) {
  using char_type = typename View::value_type;

  require_null_terminated(value);

  const auto expected_size = std::char_traits<char_type>::length(expected);
  REQUIRE(value.size() == expected_size);
  for (std::size_t i = 0; i < expected_size; ++i) {
    REQUIRE(value[i] == expected[i]);
  }
}

template <typename String>
void require_c_str_null_terminated(const String& value) {
  using char_type = typename String::value_type;

  REQUIRE(value.c_str() != nullptr);
  REQUIRE(value.c_str()[value.size()] == char_type{});
}

template <typename String>
void require_c_str_null_terminated(const String& value, const typename String::value_type* expected) {
  using char_type = typename String::value_type;

  require_c_str_null_terminated(value);

  const auto expected_size = std::char_traits<char_type>::length(expected);
  REQUIRE(value.size() == expected_size);
  for (std::size_t i = 0; i < expected_size; ++i) {
    REQUIRE(value[i] == expected[i]);
  }
}

template <typename Value, typename Char>
void require_ostream(Value&& value, const Char* expected) {
  using namespace magic_enum::ostream_operators;

  std::basic_stringstream<Char> stream;
  stream << std::forward<Value>(value);
  REQUIRE(stream);
  REQUIRE(stream.str() == expected);
}

template <typename Value, typename Char>
void require_istream(Value expected, const Char* name) {
  using namespace magic_enum::istream_operators;

  std::basic_istringstream<Char> stream{name};
  std::decay_t<Value> value;
  stream >> value;
  REQUIRE(stream);
  REQUIRE(value == expected);
}

} // namespace magic_enum_tests
