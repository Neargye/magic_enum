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

#ifdef _WIN32
#define _ITERATOR_DEBUG_LEVEL 0
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

  friend std::ostream& operator<<(std::ostream& ostream, RGB rgb) {

    ostream << "R=" << static_cast<std::uint32_t>(rgb.r) << " G=" << static_cast<std::uint32_t>(rgb.g) << " B=" << static_cast<std::uint32_t>(rgb.b);
    return ostream;
  }
};

template <typename T> bool check_const([[maybe_unused]]T& element) { return false; }
template <typename T> bool check_const([[maybe_unused]]T const& element) { return true; }

constexpr std::uint8_t color_max = std::numeric_limits<std::uint8_t>::max();

TEST_CASE("containers_array") {

  using namespace magic_enum::bitwise_operators;
  using namespace magic_enum::ostream_operators;

  constexpr magic_enum::containers::array<Color, RGB> color_rgb_initializer {{{{color_max, 0, 0}, {0, color_max, 0}, {0, 0, color_max}}}};
  REQUIRE(color_rgb_initializer.at(Color::RED) == RGB{color_max, 0, 0});
  REQUIRE(color_rgb_initializer.at(Color::GREEN) == RGB{0, color_max, 0});
  REQUIRE(color_rgb_initializer.at(Color::BLUE) == RGB{0, 0, color_max});

  /* BUG: a sort will not survive the data integration */
  magic_enum::containers::array<Color, std::uint8_t> color_rgb_container_int{{1U, 4U, 2U}};

  // ENC: Direct convert to std::array
  // std::array compare_before {1U, 4U, 2U};
  constexpr magic_enum::containers::array<Color, std::uint8_t> compare_before{{1U, 4U, 2U}};
  REQUIRE(color_rgb_container_int == compare_before);

  constexpr auto colors = magic_enum::enum_values<Color>();

  std::ignore = std::get<0>(compare_before);
  std::ignore = std::get<1>(compare_before);
  std::ignore = std::get<2>(compare_before);

  std::ignore = std::get<Color::RED>(compare_before);
  std::ignore = std::get<Color::GREEN>(compare_before);
  std::ignore = std::get<Color::BLUE>(compare_before);

  REQUIRE(std::make_pair(colors[0], color_rgb_container_int[colors[0]]) == std::make_pair<Color, std::uint8_t>(Color::RED, 1U));
  REQUIRE(std::make_pair(colors[1], color_rgb_container_int[colors[1]]) == std::make_pair<Color, std::uint8_t>(Color::GREEN, 4U));
  REQUIRE(std::make_pair(colors[2], color_rgb_container_int[colors[2]]) == std::make_pair<Color, std::uint8_t>(Color::BLUE, 2U));

  std::sort(std::begin(color_rgb_container_int), std::end(color_rgb_container_int));

  // Missing: Direct convert to std::array
  // std::array compare_after {1U, 2U, 4U};
  constexpr magic_enum::containers::array<Color, std::uint8_t> compare_after{{1U, 2U, 4U}};
  REQUIRE(color_rgb_container_int == compare_after);

  std::ignore = std::get<0>(compare_after);
  std::ignore = std::get<1>(compare_after);
  std::ignore = std::get<2>(compare_after);

  std::ignore = std::get<Color::RED>(compare_after);
  std::ignore = std::get<Color::GREEN>(compare_after);
  std::ignore = std::get<Color::BLUE>(compare_after);

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

  REQUIRE(color_rgb_container.front() == RGB{color_max, 0, 0});
  REQUIRE(color_rgb_container.back() == RGB{0, 0, color_max});

  REQUIRE(std::get<Color::RED>(color_rgb_container) == RGB{color_max, 0, 0});
  REQUIRE(std::get<Color::GREEN>(color_rgb_container) == RGB{0, color_max, 0});
  REQUIRE(std::get<Color::BLUE>(color_rgb_container) == RGB{0, 0, color_max});

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

  constexpr auto from_to_array = magic_enum::containers::to_array<Color, RGB>({{color_max, 0, 0}, {0, color_max, 0}, {0, 0, color_max}});
  REQUIRE(from_to_array.at(Color::RED) == RGB{color_max, 0, 0});
  REQUIRE(from_to_array.at(Color::GREEN) == RGB{0, color_max, 0});
  REQUIRE(from_to_array.at(Color::BLUE) == RGB{0, 0, color_max});
}

TEST_CASE("containers_bitset") {

  using namespace magic_enum::bitwise_operators;

  auto color_bitset = magic_enum::containers::bitset<Color>();
  REQUIRE(color_bitset.to_string().empty());
  REQUIRE(color_bitset.size() == 3);
  REQUIRE(magic_enum::enum_count<Color>() == color_bitset.size());
  REQUIRE_FALSE(color_bitset.all());
  REQUIRE_FALSE(color_bitset.any());
  REQUIRE(color_bitset.none());
  REQUIRE(color_bitset.count() == 0);

  color_bitset.set(Color::GREEN);
  REQUIRE_FALSE(color_bitset.all());
  REQUIRE(color_bitset.any());
  REQUIRE_FALSE(color_bitset.none());
  REQUIRE(color_bitset.count() == 1);
  REQUIRE_FALSE(color_bitset.test(Color::RED));
  REQUIRE(color_bitset.test(Color::GREEN));
  REQUIRE_FALSE(color_bitset.test(Color::BLUE));

  color_bitset.set(Color::BLUE);
  REQUIRE_FALSE(color_bitset.all());
  REQUIRE(color_bitset.any());
  REQUIRE_FALSE(color_bitset.none());
  REQUIRE(color_bitset.count() == 2);
  REQUIRE_FALSE(color_bitset.test(Color::RED));
  REQUIRE(color_bitset.test(Color::GREEN));
  REQUIRE(color_bitset.test(Color::BLUE));

  color_bitset.set(Color::RED);
  REQUIRE(color_bitset.all());
  REQUIRE(color_bitset.any());
  REQUIRE_FALSE(color_bitset.none());
  REQUIRE(color_bitset.count() == 3);
  REQUIRE(color_bitset.test(Color::RED));
  REQUIRE(color_bitset.test(Color::GREEN));
  REQUIRE(color_bitset.test(Color::BLUE));

  color_bitset.reset();
  REQUIRE_FALSE(color_bitset.all());
  REQUIRE_FALSE(color_bitset.any());
  REQUIRE(color_bitset.none());
  REQUIRE(color_bitset.count() == 0);
  REQUIRE_FALSE(color_bitset.test(Color::RED));
  REQUIRE_FALSE(color_bitset.test(Color::GREEN));
  REQUIRE_FALSE(color_bitset.test(Color::BLUE));

  color_bitset.set(Color::RED);
  REQUIRE(color_bitset.test(Color::RED));
  REQUIRE_FALSE(color_bitset.test(Color::GREEN));
  REQUIRE_FALSE(color_bitset.test(Color::BLUE));

  color_bitset.flip();
  REQUIRE_FALSE(color_bitset.test(Color::RED));
  REQUIRE(color_bitset.test(Color::GREEN));
  REQUIRE(color_bitset.test(Color::BLUE));

  constexpr magic_enum::containers::bitset<Color> color_bitset_all {Color::RED|Color::GREEN|Color::BLUE};
  REQUIRE(color_bitset_all.to_string() == "RED|GREEN|BLUE");
  REQUIRE(color_bitset_all.to_string( {}, '0', '1' ) == "111");
  REQUIRE(color_bitset_all.to_ulong( {} ) == 7);
  REQUIRE(color_bitset_all.to_ullong( {} ) == 7);
  REQUIRE(color_bitset_all.all());
  REQUIRE(color_bitset_all.any());
  REQUIRE_FALSE(color_bitset_all.none());

  constexpr magic_enum::containers::bitset<Color> color_bitset_red_green {Color::RED|Color::GREEN};
  REQUIRE(color_bitset_red_green.to_string() == "RED|GREEN");
  REQUIRE(color_bitset_red_green.to_string( {}, '0', '1' ) == "110");
  REQUIRE(color_bitset_red_green.to_ulong( {} ) == 3);
  REQUIRE(color_bitset_red_green.to_ullong( {} ) == 3);
  REQUIRE_FALSE(color_bitset_red_green.all());
  REQUIRE(color_bitset_red_green.any());
  REQUIRE_FALSE(color_bitset_red_green.none());
}

TEST_CASE("containers_set") {

  using namespace magic_enum::bitwise_operators;
  using namespace magic_enum::ostream_operators;

  auto color_set = magic_enum::containers::set<Color>();
  REQUIRE(color_set.empty());
  REQUIRE(color_set.size() == 0);
  REQUIRE_FALSE(magic_enum::enum_count<Color>() == color_set.size());

  color_set.insert(Color::RED);
  std::ignore = color_set.insert(Color::RED);
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

  auto color_set_compare = magic_enum::containers::set<Color>();
  color_set_compare.insert(Color::BLUE);
  color_set_compare.insert(Color::RED);
  color_set_compare.insert(Color::GREEN);

  constexpr magic_enum::containers::set color_set_filled = {Color::RED, Color::GREEN, Color::BLUE};
  REQUIRE_FALSE(color_set_filled.empty());
  REQUIRE(color_set_filled.size() == 3);
  REQUIRE(magic_enum::enum_count<Color>() == color_set_filled.size());

  magic_enum::containers::set color_set_not_const {Color::RED, Color::GREEN, Color::BLUE};
  REQUIRE_FALSE(color_set_not_const.empty());
  REQUIRE(color_set_not_const.size() == 3);
  REQUIRE(magic_enum::enum_count<Color>() == color_set_not_const.size());
  color_set_not_const.clear();
  REQUIRE(color_set_not_const.empty());
  REQUIRE(color_set_not_const.size() == 0);
  REQUIRE_FALSE(magic_enum::enum_count<Color>() == color_set_not_const.size());
}

TEST_CASE("containers_flat_set") {

  // constexpr magic_enum::containers::flat_set color_flat_set_filled = {Color::RED, Color::GREEN, Color::BLUE};
  // REQUIRE_FALSE(color_flat_set_filled.empty());
  // REQUIRE(color_flat_set_filled.size() == 3);
  // REQUIRE(magic_enum::enum_count<Color>() == color_flat_set_filled.size());
}

TEST_CASE("map_like_container") {

  using namespace magic_enum::ostream_operators;

  std::vector<std::pair<Color, RGB>> map {{Color::GREEN, {0, color_max, 0}}, {Color::BLUE, {0, 0, color_max}}, {Color::RED, {color_max, 0, 0}}};
  for (auto [key, value] : map) {

    std::cout << "Key=" << key << " Value=" << value << std::endl;
  }
  auto compare = [](std::pair<Color, RGB>& lhs,
                    std::pair<Color, RGB>& rhs) {
     return static_cast<std::int32_t>(lhs.first) < static_cast<std::int32_t>(rhs.first);
  };
  std::sort(std::begin(map), std::end(map), compare);
  for (auto [key, value] : map) {

    std::cout << "Key=" << key << " Value=" << value << std::endl;
  }
}
