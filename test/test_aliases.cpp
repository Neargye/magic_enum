// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2022 Daniil Goncharov <neargye@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <optional>
#include <string>
#include <string_view>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

template <typename T>
struct MyOpt {
  constexpr MyOpt() : mHasValue{false}, val{} {} // required
  constexpr MyOpt(const T& v) : mHasValue{true}, val{v} {} // required
  constexpr const T& operator*() const { return val; } // required
  constexpr explicit operator bool() const  { return mHasValue; } // required

  constexpr bool has_value() const { return mHasValue; }
  constexpr const T& value() const { return val; }

 private:
  bool mHasValue;
  T val;
};

struct MyString {
  using value_type = char; // required
  static constexpr auto npos = std::string_view::npos; // required

  MyString() : str{} {} // required
  MyString(const char* s, std::size_t l) : str{s, l} {} // required
  bool empty() const { return str.empty(); } // required
  auto begin() const { return str.begin(); } // required
  auto end() const { return str.end(); } // required
  void append(std::size_t count, char c) { str.append(count, c); } // required
  void append(const char* s, std::size_t size) { str.append(s, size); } // required

  std::size_t size() const { return str.size(); }
  int compare(const char* s) const { return str.compare(s); }

 private:
  std::string str;
};

struct MyStringView {
  using value_type = char; // required
  static constexpr auto npos = std::string_view::npos; // required

  constexpr MyStringView() : str{} {} // required
  constexpr MyStringView(const char* cstr, std::size_t size) : str{cstr, size} {} // required
  constexpr bool empty() const { return str.empty(); } // required
  constexpr std::size_t size() const { return str.size(); } // required
  constexpr const char* data() const { return str.data(); } // required
  constexpr const char& operator[](std::size_t i) const { return str[i]; } // required
  constexpr auto begin() const { return str.begin(); } // required
  constexpr auto end() const { return str.end(); } // required
  constexpr std::size_t find(char c) const { return str.find(c); } // required
  constexpr MyStringView substr(std::size_t p, std::size_t n) { return str.substr(p, n); } // required
  constexpr void remove_prefix(std::size_t n) { str.remove_prefix(n); } // required
  friend constexpr bool operator==(MyStringView lhs, MyStringView rhs); // required

  constexpr MyStringView(const char* cstr) : str{ cstr } {}
  constexpr int compare(const char* s) const { return str.compare(s); }

 private:
  std::string_view str;

  constexpr MyStringView(std::string_view s) : str{s} {}
};

constexpr bool operator==(MyStringView lhs, MyStringView rhs) {
  return lhs.str == rhs.str;
}

#define MAGIC_ENUM_USING_ALIAS_OPTIONAL template<typename T> using optional = MyOpt<T>;
#define MAGIC_ENUM_USING_ALIAS_STRING using string = MyString;
#define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = MyStringView;

#include <magic_enum.hpp>
using namespace magic_enum;
using namespace magic_enum::bitwise_operators;

enum class Color { RED = 1, GREEN = 2, BLUE = 4 };

TEST_CASE("optional") {
  constexpr auto cr = enum_cast<Color>("RED");
  REQUIRE(cr.has_value());
  REQUIRE(cr.value() == Color::RED);

  constexpr auto cn = enum_cast<Color>("NONE");
  REQUIRE_FALSE(cn.has_value());
}

TEST_CASE("string") {
  auto cr = enum_flags_name(Color::RED);
  REQUIRE_FALSE(cr.empty());
  REQUIRE(cr.compare("RED") == 0);

  auto crg = enum_flags_name(Color::RED | Color::GREEN);
  REQUIRE_FALSE(crg.empty());
  REQUIRE(crg.compare("RED|GREEN") == 0);

  auto cn = enum_flags_name(Color{0});
  REQUIRE(cn.empty());
  REQUIRE(cn.size() == 0);
}

TEST_CASE("string_view") {
  auto cr = enum_name(Color::RED);
  REQUIRE_FALSE(cr.empty());
  REQUIRE(cr.compare("RED") == 0);

  auto cn = enum_name(Color{0});
  REQUIRE(cn.empty());
  REQUIRE(cn.size() == 0);
}
