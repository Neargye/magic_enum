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

#if defined(__clang__)
#  pragma clang diagnostic push
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4244) // warning C4244: 'argument': conversion from 'const T' to 'unsigned int', possible loss of data.
#endif

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <magic_enum_containers.hpp>

#include <functional>

enum class Color { RED = 1, GREEN = 2, BLUE = 4 };

enum class Empty {};

struct RGB {

  std::uint8_t r {};
  std::uint8_t g {};
  std::uint8_t b {};

  [[nodiscard]] constexpr bool empty() { return std::equal_to{}(r, g) && std::equal_to{}(g, b) && std::equal_to{}(b, 0); }

  [[nodiscard]] constexpr bool operator==(RGB rgb) const noexcept { return std::equal_to{}(r, rgb.r) && std::equal_to{}(g, rgb.g) && std::equal_to{}(b, rgb.b); }

  friend std::ostream& operator<<(std::ostream& os, RGB rgb) {

    os << "R=" << static_cast<std::uint32_t>(rgb.r) << " G=" << static_cast<std::uint32_t>(rgb.g) << " B=" << static_cast<std::uint32_t>(rgb.b);
    return os;
  }
};

template <typename T> bool check_const([[maybe_unused]]T& x) { return false; }
template <typename T> bool check_const([[maybe_unused]]T const& x) { return true; }

constexpr std::uint8_t color_max = std::numeric_limits<std::uint8_t>::max();

TEST_CASE("containers_array") {

  using namespace magic_enum::bitwise_operators;
  using namespace magic_enum::ostream_operators;

  /* Issue: a sort will not survive the data integration */
  magic_enum::containers::array<Color, std::uint8_t> color_rgb_container_int {{1U, 4U, 2U}};

  // Missing: Direct convert to std::array
  // std::array compare_before {1U, 4U, 2U};
  constexpr magic_enum::containers::array<Color, std::uint8_t> compare_before {{1U, 4U, 2U}};
  REQUIRE(color_rgb_container_int == compare_before);

  constexpr auto colors = magic_enum::enum_values<Color>();
  for (auto color : colors) {

    std::cout << "Key=" << color << " Value=" << static_cast<std::uint32_t>(compare_before[color]) << std::endl;
  }

  REQUIRE(std::make_pair(colors[0], color_rgb_container_int[colors[0]]) == std::make_pair<Color, std::uint8_t>(Color::RED, 1U));
  REQUIRE(std::make_pair(colors[1], color_rgb_container_int[colors[1]]) == std::make_pair<Color, std::uint8_t>(Color::GREEN, 4U));
  REQUIRE(std::make_pair(colors[2], color_rgb_container_int[colors[2]]) == std::make_pair<Color, std::uint8_t>(Color::BLUE, 2U));

  std::sort(std::begin(color_rgb_container_int), std::end(color_rgb_container_int));

  // Missing: Direct convert to std::array
  // std::array compare_after {1U, 2U, 4U};
  constexpr magic_enum::containers::array<Color, std::uint8_t> compare_after {{1U, 2U, 4U}};
  REQUIRE(color_rgb_container_int == compare_after);

  for (auto color : colors) {

    std::cout << "Key=" << color << " Value=" << static_cast<std::uint32_t>(compare_after[color]) << std::endl;
  }

  REQUIRE(std::make_pair(colors[0], color_rgb_container_int[colors[0]]) == std::make_pair<Color, std::uint8_t>(Color::RED, 1U));
  REQUIRE(std::make_pair(colors[1], color_rgb_container_int[colors[1]]) == std::make_pair<Color, std::uint8_t>(Color::GREEN, 2U));
  REQUIRE(std::make_pair(colors[2], color_rgb_container_int[colors[2]]) == std::make_pair<Color, std::uint8_t>(Color::BLUE, 4U));

  auto empty = magic_enum::containers::array<Empty, std::nullptr_t>();
  REQUIRE(empty.empty());
  REQUIRE(empty.size() == 0);
  REQUIRE(magic_enum::enum_count<Empty>() == empty.size());

  auto color_rgb_container = magic_enum::containers::array<Color, RGB>();
  REQUIRE_FALSE(color_rgb_container.empty());
  REQUIRE(color_rgb_container.size() == 3);
  REQUIRE(magic_enum::enum_count<Color>() == color_rgb_container.size());

  REQUIRE(color_rgb_container.at(Color::RED).empty());
  REQUIRE(color_rgb_container.at(Color::GREEN).empty());
  REQUIRE(color_rgb_container.at(Color::BLUE).empty());
  REQUIRE_THROWS(color_rgb_container.at(Color::BLUE|Color::GREEN).empty());

  color_rgb_container[Color::RED] = {color_max, 0, 0};
  color_rgb_container[Color::GREEN] = {0, color_max, 0};
  color_rgb_container[Color::BLUE] = {0, 0, color_max};

  REQUIRE(color_rgb_container.at(Color::RED) == RGB{color_max, 0, 0});
  REQUIRE(color_rgb_container.at(Color::GREEN) == RGB{0, color_max, 0});
  REQUIRE(color_rgb_container.at(Color::BLUE) == RGB{0, 0, color_max});

  auto iterator = color_rgb_container.begin();
  REQUIRE_FALSE(check_const(iterator));
  REQUIRE(check_const(color_rgb_container.begin()));
  REQUIRE(check_const(color_rgb_container.cbegin()));

  auto color_rgb_container_compare = magic_enum::containers::array<Color, RGB>();
  color_rgb_container_compare.fill({color_max, color_max, color_max});
  REQUIRE_FALSE(color_rgb_container == color_rgb_container_compare);

  color_rgb_container_compare[Color::RED] = {color_max, 0, 0};
  color_rgb_container_compare[Color::GREEN] = {0, color_max, 0};
  color_rgb_container_compare[Color::BLUE] = {0, 0, color_max};
  REQUIRE(color_rgb_container == color_rgb_container_compare);

  for (auto color : colors) {

    std::cout << "Key=" << color << " Value=" << color_rgb_container[color] << std::endl;
  }
}

TEST_CASE("containers_set") {

  using namespace magic_enum::bitwise_operators;
  using namespace magic_enum::ostream_operators;

  auto color_set = magic_enum::containers::set<Color>();
  REQUIRE(color_set.empty());
  REQUIRE(color_set.size() == 0);
  REQUIRE_FALSE(magic_enum::enum_count<Color>() == color_set.size());

  color_set.insert(Color::RED);
  color_set.insert(Color::GREEN);
  color_set.insert(Color::BLUE);
  color_set.insert(Color::RED);
  color_set.insert(Color::RED|Color::GREEN);
  color_set.insert(Color::RED|Color::BLUE);
  color_set.insert(Color::GREEN|Color::BLUE);
  color_set.insert(Color::RED|Color::GREEN|Color::BLUE);

  REQUIRE_FALSE(color_set.empty());
  REQUIRE(color_set.size() == 3);
  REQUIRE(magic_enum::enum_count<Color>() == color_set.size());

  /*for (auto color : color_set) {

    std::cout << color << std::endl;
  }*/

  // std::sort(std::begin(color_set), std::end(color_set));

  auto color_set_compare = magic_enum::containers::set<Color>();
  color_set_compare.insert(Color::BLUE);
  color_set_compare.insert(Color::RED);
  color_set_compare.insert(Color::GREEN);

  /*for (auto color : color_set_compare) {

    std::cout << color << std::endl;
  }*/
}
