// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#pragma once

#include <doctest/doctest.h>

#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace magic_enum_tests {

struct ConstReferencePredicate {
  constexpr bool operator()(const char& lhs, const char& rhs) & noexcept { return lhs == rhs; }
  bool operator()(char&&, char&&) & = delete;
};

struct ThrowingReferencePredicate {
  bool operator()(const char& lhs, const char& rhs) & {
    if (lhs == rhs) {
      throw 42;
    }
    return false;
  }
  bool operator()(char&& lhs, char&& rhs) & noexcept { return lhs == rhs; }
};

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

template <typename Char>
struct OutputBuffer : std::basic_streambuf<Char> {
  std::basic_string<Char> output;
  std::streamsize remaining = 20;
  int sync_count = 0;
  bool throw_on_write = false;

  std::streamsize xsputn(const Char* data, std::streamsize size) override {
    if (throw_on_write) {
      throw std::runtime_error("output buffer");
    }
    const auto written = size < remaining ? size : remaining;
    output.append(data, static_cast<std::size_t>(written));
    remaining -= written;
    return written;
  }

  typename std::basic_streambuf<Char>::int_type overflow(typename std::basic_streambuf<Char>::int_type value) override {
    if (throw_on_write) {
      throw std::runtime_error("output buffer");
    }
    if (remaining == 0) {
      return std::char_traits<Char>::eof();
    }
    --remaining;
    output.push_back(std::char_traits<Char>::to_char_type(value));
    return value;
  }

  int sync() override {
    ++sync_count;
    return 0;
  }
};

template <typename Value, typename Char>
void require_ostream_errors(Value value, const std::basic_string<Char>& name) {
  using namespace magic_enum::ostream_operators;
  for (const auto remaining : {0, 1, 2, 3, 5, 8, 20}) {
    for (const bool throw_on_write : {false, true}) {
      for (const bool exceptions : {false, true}) {
        for (const bool unitbuf : {false, true}) {
          for (const bool left : {false, true}) {
            OutputBuffer<Char> actual_buffer, expected_buffer, actual_tie, expected_tie;
            actual_buffer.remaining = expected_buffer.remaining = remaining;
            actual_buffer.throw_on_write = expected_buffer.throw_on_write = throw_on_write;
            std::basic_ostream<Char> actual(&actual_buffer), expected(&expected_buffer);
            std::basic_ostream<Char> actual_tied(&actual_tie), expected_tied(&expected_tie);
            actual.tie(&actual_tied);
            expected.tie(&expected_tied);
            for (auto* stream : {&actual, &expected}) {
              stream->width(8);
              stream->fill(Char{'_'});
              stream->setf(left ? std::ios::left : std::ios::right, std::ios::adjustfield);
              if (unitbuf) {
                stream->setf(std::ios::unitbuf);
              }
              if (exceptions) {
                stream->exceptions(std::ios::badbit | std::ios::failbit);
              }
            }
            const auto insert = [](auto& stream, const auto& item) {
              try {
                stream << item;
                return 0;
              } catch (const std::ios_base::failure&) {
                return 1;
              } catch (const std::runtime_error&) {
                return 2;
              }
            };
            REQUIRE(insert(actual, value) == insert(expected, name));
            REQUIRE(actual_buffer.output == expected_buffer.output);
            REQUIRE(actual.rdstate() == expected.rdstate());
            REQUIRE(actual.width() == expected.width());
            REQUIRE(actual_buffer.sync_count == expected_buffer.sync_count);
            REQUIRE(actual_tie.sync_count == expected_tie.sync_count);
          }
        }
      }
    }
  }
}

template <typename Char>
struct CustomCharTraits : std::char_traits<Char> {};

template <typename Value, typename Char>
void require_istream(Value expected, const Char* name) {
  using namespace magic_enum::istream_operators;

  std::basic_istringstream<Char> stream{name};
  std::decay_t<Value> value;
  stream >> value;
  REQUIRE(stream);
  REQUIRE(value == expected);

  std::basic_istringstream<Char, CustomCharTraits<Char>> custom_stream{name};
  value = {};
  custom_stream >> value;
  REQUIRE(custom_stream);
  REQUIRE(value == expected);
}

} // namespace magic_enum_tests
