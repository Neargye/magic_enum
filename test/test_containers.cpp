// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.
// Copyright (c) 2022 - 2023 Bela Schaum <schaumb@gmail.com>.
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

#include <new>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <magic_enum/magic_enum_containers.hpp>
#include <magic_enum/magic_enum_iostream.hpp>

#include <functional>

enum class Color { RED = 1, GREEN = 2, BLUE = 4 };
template <>
struct magic_enum::customize::enum_range<Color> {
  static constexpr bool is_flags = true;
};

enum class Empty {};

// Enums used by containers_bitset_iterator tests.
enum class Numbers { ONE, TWO, THREE, FOUR };           // 4 values, uint8_t base_type, not_interested=4
enum class Nibble  { B0, B1, B2, B3, B4, B5, B6, B7 }; // 8 values, uint8_t base_type, not_interested=0 (edge case)
enum class Flags9  { A, B, C, D, E, F, G, H, I };      // 9 values, uint16_t base_type

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

  std::ignore = magic_enum::containers::get<0>(compare_before);
  std::ignore = magic_enum::containers::get<1>(compare_before);
  std::ignore = magic_enum::containers::get<2>(compare_before);

  std::ignore = magic_enum::containers::get<Color::RED>(compare_before);
  std::ignore = magic_enum::containers::get<Color::GREEN>(compare_before);
  std::ignore = magic_enum::containers::get<Color::BLUE>(compare_before);

  REQUIRE(std::make_pair(colors[0], color_rgb_container_int[colors[0]]) == std::make_pair<Color, std::uint8_t>(Color::RED, 1U));
  REQUIRE(std::make_pair(colors[1], color_rgb_container_int[colors[1]]) == std::make_pair<Color, std::uint8_t>(Color::GREEN, 4U));
  REQUIRE(std::make_pair(colors[2], color_rgb_container_int[colors[2]]) == std::make_pair<Color, std::uint8_t>(Color::BLUE, 2U));

  std::sort(std::begin(color_rgb_container_int), std::end(color_rgb_container_int));

  // Missing: Direct convert to std::array
  // std::array compare_after {1U, 2U, 4U};
  constexpr magic_enum::containers::array<Color, std::uint8_t> compare_after{{1U, 2U, 4U}};
  REQUIRE(color_rgb_container_int == compare_after);

  std::ignore = magic_enum::containers::get<0>(compare_after);
  std::ignore = magic_enum::containers::get<1>(compare_after);
  std::ignore = magic_enum::containers::get<2>(compare_after);

  std::ignore = magic_enum::containers::get<Color::RED>(compare_after);
  std::ignore = magic_enum::containers::get<Color::GREEN>(compare_after);
  std::ignore = magic_enum::containers::get<Color::BLUE>(compare_after);

  REQUIRE(std::make_pair(colors[0], color_rgb_container_int[colors[0]]) == std::make_pair<Color, std::uint8_t>(Color::RED, 1U));
  REQUIRE(std::make_pair(colors[1], color_rgb_container_int[colors[1]]) == std::make_pair<Color, std::uint8_t>(Color::GREEN, 2U));
  REQUIRE(std::make_pair(colors[2], color_rgb_container_int[colors[2]]) == std::make_pair<Color, std::uint8_t>(Color::BLUE, 4U));

  auto color_rgb_container = magic_enum::containers::array<Color, RGB>();
  REQUIRE_FALSE(color_rgb_container.empty());
  REQUIRE(color_rgb_container.size() == 3);
  REQUIRE(magic_enum::enum_count<Color>() == color_rgb_container.size());

  REQUIRE(color_rgb_container.at(Color::RED).empty());
  REQUIRE(color_rgb_container.at(Color::GREEN).empty());
  REQUIRE(color_rgb_container.at(Color::BLUE).empty());
  REQUIRE_THROWS(color_rgb_container.at(Color::BLUE|Color::GREEN));

  color_rgb_container[Color::RED] = {color_max, 0, 0};
  color_rgb_container[Color::GREEN] = {0, color_max, 0};
  color_rgb_container[Color::BLUE] = {0, 0, color_max};

  REQUIRE(color_rgb_container.at(Color::RED) == RGB{color_max, 0, 0});
  REQUIRE(color_rgb_container.at(Color::GREEN) == RGB{0, color_max, 0});
  REQUIRE(color_rgb_container.at(Color::BLUE) == RGB{0, 0, color_max});

  REQUIRE(color_rgb_container.front() == RGB{color_max, 0, 0});
  REQUIRE(color_rgb_container.back() == RGB{0, 0, color_max});

  REQUIRE(magic_enum::containers::get<Color::RED>(color_rgb_container) == RGB{color_max, 0, 0});
  REQUIRE(magic_enum::containers::get<Color::GREEN>(color_rgb_container) == RGB{0, color_max, 0});
  REQUIRE(magic_enum::containers::get<Color::BLUE>(color_rgb_container) == RGB{0, 0, color_max});

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

  constexpr auto from_make_array = magic_enum::containers::make_array<Color>(RGB{color_max, 0, 0}, RGB{0, color_max, 0}, RGB{0, 0, color_max});
  REQUIRE(from_make_array.at(Color::RED) == RGB{color_max, 0, 0});
  REQUIRE(from_make_array.at(Color::GREEN) == RGB{0, color_max, 0});
  REQUIRE(from_make_array.at(Color::BLUE) == RGB{0, 0, color_max});
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
  color_set.erase(Color::RED);
  color_set.erase(Color::GREEN);
  REQUIRE(magic_enum::enum_count<Color>() - 2 == color_set.size());
  REQUIRE(color_set.count(Color::RED) == 0);
  REQUIRE_FALSE(color_set.contains(Color::GREEN));
  REQUIRE(color_set.contains(Color::BLUE));

  auto color_set_compare = magic_enum::containers::set<Color>();
  color_set_compare.insert(Color::BLUE);
  color_set_compare.insert(Color::RED);
  color_set_compare.insert(Color::GREEN);

  constexpr magic_enum::containers::set color_set_filled = {Color::RED, Color::GREEN, Color::BLUE};
  REQUIRE_FALSE(color_set_filled.empty());
  REQUIRE(color_set_filled.size() == 3);
  REQUIRE(magic_enum::enum_count<Color>() == color_set_filled.size());

  magic_enum::containers::set color_set_not_const = {Color::RED, Color::GREEN, Color::BLUE};
  REQUIRE_FALSE(color_set_not_const.empty());
  REQUIRE(color_set_not_const.size() == 3);
  REQUIRE(magic_enum::enum_count<Color>() == color_set_not_const.size());
  color_set_not_const.clear();
  REQUIRE(color_set_not_const.empty());
  REQUIRE(color_set_not_const.size() == 0);
  REQUIRE_FALSE(magic_enum::enum_count<Color>() == color_set_not_const.size());

  magic_enum::containers::set<Color> color_set_assign {Color::RED, Color::BLUE};
  color_set_assign = {Color::GREEN};
  REQUIRE(color_set_assign.size() == 1);
  REQUIRE(color_set_assign.contains(Color::GREEN));
  REQUIRE_FALSE(color_set_assign.contains(Color::RED));
  REQUIRE_FALSE(color_set_assign.contains(Color::BLUE));
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

TEST_CASE("containers_bitset_iterator") {
  using namespace magic_enum::bitwise_operators;

  SUBCASE("empty - begin equals end") {
    magic_enum::containers::bitset<Color> bs;
    REQUIRE(bs.begin() == bs.end());
    REQUIRE(bs.cbegin() == bs.cend());
    std::size_t n = 0;
    for ([[maybe_unused]] Color e : bs) { ++n; }
    REQUIRE(n == 0);
  }

  SUBCASE("single element - first") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::RED);
    REQUIRE(bs.begin() != bs.end());
    REQUIRE(*bs.begin() == Color::RED);
    auto it = bs.begin();
    ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("single element - middle") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::GREEN);
    REQUIRE(bs.begin() != bs.end());
    REQUIRE(*bs.begin() == Color::GREEN);
    auto it = bs.begin();
    ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("single element - last") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::BLUE);
    REQUIRE(bs.begin() != bs.end());
    REQUIRE(*bs.begin() == Color::BLUE);
    auto it = bs.begin();
    ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("forward - order matches enum_values") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    constexpr auto expected = magic_enum::enum_values<Color>();
    std::size_t i = 0;
    for (Color e : bs) {
      REQUIRE(i < expected.size());
      REQUIRE(e == expected[i++]);
    }
    REQUIRE(i == expected.size());
  }

  SUBCASE("forward - partial - first and last only") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::RED);
    bs.set(Color::BLUE);
    auto it = bs.begin();
    REQUIRE(*it == Color::RED);  ++it;
    REQUIRE(*it == Color::BLUE); ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("forward - post-increment") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    auto it   = bs.begin();
    auto prev = it++;
    REQUIRE(*prev == Color::RED);
    REQUIRE(*it   == Color::GREEN);
  }

  SUBCASE("backward - all set") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    auto it = bs.end();
    --it; REQUIRE(*it == Color::BLUE);
    --it; REQUIRE(*it == Color::GREEN);
    --it; REQUIRE(*it == Color::RED);
  }

  SUBCASE("backward - partial - first and last only") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::RED);
    bs.set(Color::BLUE);
    auto it = bs.end();
    --it; REQUIRE(*it == Color::BLUE);
    --it; REQUIRE(*it == Color::RED);
  }

  SUBCASE("backward - post-decrement") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    auto it   = bs.end();
    --it;
    auto prev = it--;
    REQUIRE(*prev == Color::BLUE);
    REQUIRE(*it   == Color::GREEN);
  }

  SUBCASE("bidirectional - round-trip") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    auto it = bs.begin();
    REQUIRE(*it == Color::RED);   ++it;
    REQUIRE(*it == Color::GREEN); ++it;
    REQUIRE(*it == Color::BLUE);  ++it;
    REQUIRE(it == bs.end());
    --it; REQUIRE(*it == Color::BLUE);
    --it; REQUIRE(*it == Color::GREEN);
    --it; REQUIRE(*it == Color::RED);
  }

  SUBCASE("find - present values") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::GREEN);
    bs.set(Color::BLUE);
    auto it = bs.find(Color::GREEN);
    REQUIRE(it != bs.end());
    REQUIRE(*it == Color::GREEN);
    it = bs.find(Color::BLUE);
    REQUIRE(it != bs.end());
    REQUIRE(*it == Color::BLUE);
  }

  SUBCASE("find - absent values") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::GREEN);
    REQUIRE(bs.find(Color::RED)  == bs.end());
    REQUIRE(bs.find(Color::BLUE) == bs.end());
  }

  SUBCASE("find - empty bitset") {
    magic_enum::containers::bitset<Color> bs;
    for (Color e : magic_enum::enum_values<Color>())
      REQUIRE(bs.find(e) == bs.end());
  }

  SUBCASE("find - all set, const bitset") {
    const magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    for (Color e : magic_enum::enum_values<Color>()) {
      auto it = bs.find(e);
      REQUIRE(it != bs.end());
      REQUIRE(*it == e);
    }
  }

  SUBCASE("find - iterator can be incremented to next element") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    auto it = bs.find(Color::GREEN);
    REQUIRE(*it == Color::GREEN);
    ++it;
    REQUIRE(*it == Color::BLUE);
  }

  SUBCASE("non-const begin returns mutable iterator") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::RED);
    auto it = bs.begin();
    REQUIRE_FALSE(check_const(it));
  }

  SUBCASE("cbegin/cend usable on non-const bitset") {
    magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    magic_enum::containers::bitset<Color>::const_iterator cit = bs.cbegin();
    std::size_t n = 0;
    while (cit != bs.cend()) { ++n; ++cit; }
    REQUIRE(n == 3);
  }

  SUBCASE("iterator to const_iterator implicit conversion") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::RED);
    magic_enum::containers::bitset<Color>::iterator       it  = bs.begin();
    magic_enum::containers::bitset<Color>::const_iterator cit = it;
    REQUIRE(*it == *cit);
    REQUIRE(it == cit);
  }

  SUBCASE("const range-for") {
    const magic_enum::containers::bitset<Color> bs{ Color::RED | Color::GREEN | Color::BLUE };
    std::size_t n = 0;
    for ([[maybe_unused]] Color e : bs) { ++n; }
    REQUIRE(n == 3);
  }

  SUBCASE("operator->") {
    magic_enum::containers::bitset<Color> bs;
    bs.set(Color::RED);
    auto it = bs.begin();
    REQUIRE(it.operator->() == &(*it));
  }

  // Regression check for the B7/MSB path used by reverse iteration.
  SUBCASE("Nibble - single MSB element B7 (countl_zero regression)") {
    magic_enum::containers::bitset<Nibble> bs;
    bs.set(Nibble::B7);
    REQUIRE(*bs.begin() == Nibble::B7);
    auto it = bs.end();
    --it;
    REQUIRE(*it == Nibble::B7);
  }

  SUBCASE("Nibble - first and last (B0 and B7)") {
    magic_enum::containers::bitset<Nibble> bs;
    bs.set(Nibble::B0);
    bs.set(Nibble::B7);
    auto it = bs.begin();
    REQUIRE(*it == Nibble::B0); ++it;
    REQUIRE(*it == Nibble::B7); ++it;
    REQUIRE(it == bs.end());
    --it; REQUIRE(*it == Nibble::B7);
    --it; REQUIRE(*it == Nibble::B0);
  }

  SUBCASE("Nibble - forward iteration all 8 values") {
    magic_enum::containers::bitset<Nibble> bs;
    for (Nibble e : magic_enum::enum_values<Nibble>()) { bs.set(e); }
    constexpr auto expected = magic_enum::enum_values<Nibble>();
    std::size_t i = 0;
    for (Nibble e : bs) { REQUIRE(e == expected[i++]); }
    REQUIRE(i == expected.size());
  }

  SUBCASE("Nibble - backward iteration all 8 values") {
    magic_enum::containers::bitset<Nibble> bs;
    for (Nibble e : magic_enum::enum_values<Nibble>()) { bs.set(e); }
    constexpr auto expected = magic_enum::enum_values<Nibble>();
    auto it = bs.end();
    for (std::size_t j = expected.size(); j > 0; --j) {
      --it;
      REQUIRE(*it == expected[j - 1]);
    }
  }

  SUBCASE("Nibble - partial middle elements") {
    magic_enum::containers::bitset<Nibble> bs;
    bs.set(Nibble::B2);
    bs.set(Nibble::B5);
    auto it = bs.begin();
    REQUIRE(*it == Nibble::B2); ++it;
    REQUIRE(*it == Nibble::B5); ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("Numbers - forward iteration partial") {
    magic_enum::containers::bitset<Numbers> bs;
    bs.set(Numbers::ONE);
    bs.set(Numbers::THREE);
    auto it = bs.begin();
    REQUIRE(*it == Numbers::ONE);   ++it;
    REQUIRE(*it == Numbers::THREE); ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("Numbers - backward iteration partial") {
    magic_enum::containers::bitset<Numbers> bs;
    bs.set(Numbers::ONE);
    bs.set(Numbers::THREE);
    auto it = bs.end();
    --it; REQUIRE(*it == Numbers::THREE);
    --it; REQUIRE(*it == Numbers::ONE);
  }

  SUBCASE("Numbers - all values forward and backward") {
    magic_enum::containers::bitset<Numbers> bs;
    for (Numbers e : magic_enum::enum_values<Numbers>()) { bs.set(e); }
    constexpr auto expected = magic_enum::enum_values<Numbers>();
    std::size_t i = 0;
    for (Numbers e : bs) { REQUIRE(e == expected[i++]); }
    REQUIRE(i == expected.size());
    auto it = bs.end();
    for (std::size_t j = expected.size(); j > 0; --j) {
      --it; REQUIRE(*it == expected[j - 1]);
    }
  }

  SUBCASE("Flags9 - forward iteration partial") {
    magic_enum::containers::bitset<Flags9> bs;
    bs.set(Flags9::A);
    bs.set(Flags9::E);
    bs.set(Flags9::I);
    auto it = bs.begin();
    REQUIRE(*it == Flags9::A); ++it;
    REQUIRE(*it == Flags9::E); ++it;
    REQUIRE(*it == Flags9::I); ++it;
    REQUIRE(it == bs.end());
  }

  SUBCASE("Flags9 - backward from end") {
    magic_enum::containers::bitset<Flags9> bs;
    bs.set(Flags9::A);
    bs.set(Flags9::I);
    auto it = bs.end();
    --it; REQUIRE(*it == Flags9::I);
    --it; REQUIRE(*it == Flags9::A);
  }

  SUBCASE("Flags9 - all 9 values forward and backward") {
    magic_enum::containers::bitset<Flags9> bs;
    for (Flags9 e : magic_enum::enum_values<Flags9>()) { bs.set(e); }
    constexpr auto expected = magic_enum::enum_values<Flags9>();
    std::size_t i = 0;
    for (Flags9 e : bs) { REQUIRE(e == expected[i++]); }
    REQUIRE(i == expected.size());
    auto it = bs.end();
    for (std::size_t j = expected.size(); j > 0; --j) {
      --it; REQUIRE(*it == expected[j - 1]);
    }
  }

  SUBCASE("Flags9 - empty bitset") {
    magic_enum::containers::bitset<Flags9> bs;
    REQUIRE(bs.begin() == bs.end());
    REQUIRE(bs.find(Flags9::A) == bs.end());
    REQUIRE(bs.find(Flags9::I) == bs.end());
  }
}
