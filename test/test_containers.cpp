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

struct RGB {

  std::uint8_t r {};
  std::uint8_t g {};
  std::uint8_t b {};

  [[nodiscard]] constexpr bool empty() { return std::equal_to{}( r, g ) && std::equal_to{}( g, b ) && std::equal_to{}( b, 0 ); }

  [[nodiscard]] constexpr bool operator==( RGB rgb ) const noexcept { return std::equal_to{}( r, rgb.r ) && std::equal_to{}( g, rgb.g ) && std::equal_to{}( b, rgb.b ); }

  friend std::ostream& operator<< (std::ostream& os, const RGB& rgb) {

    os << "R=" << static_cast<std::uint32_t>( rgb.r ) << " G=" << static_cast<std::uint32_t>( rgb.g ) << " B=" << static_cast<std::uint32_t>( rgb.b );
    return os;
  }
};

TEST_CASE("containers_array") {

  //constexpr magic_enum::containers::array<Color, RGB> con2 = { { {std::numeric_limits<std::uint8_t>::max(), 0, 0}, { 0, std::numeric_limits<std::uint8_t>::max(), 0 }, { 0, 0, std::numeric_limits<std::uint8_t>::max() } } };
  /*constexpr magic_enum::containers::array<Color, std::uint32_t> conI = { { 1, 2, 4 } };
  for ( auto conti : conI ) {

    std::cout << "array " << conti << std::endl;
  }*/

  auto color_rgb_container = magic_enum::containers::array<Color, RGB>();
  REQUIRE(magic_enum::enum_count<Color>() == color_rgb_container.size());
  REQUIRE(color_rgb_container.size() == 3);

  REQUIRE(color_rgb_container.at(Color::RED).empty());
  REQUIRE(color_rgb_container.at(Color::GREEN).empty());
  REQUIRE(color_rgb_container.at(Color::BLUE).empty());

  color_rgb_container[ Color::RED ] = { std::numeric_limits<std::uint8_t>::max(), 0, 0 };
  color_rgb_container[ Color::GREEN ] = { 0, std::numeric_limits<std::uint8_t>::max(), 0 };
  color_rgb_container[ Color::BLUE ] = { 0, 0, std::numeric_limits<std::uint8_t>::max() };

  REQUIRE(color_rgb_container.at(Color::RED) == RGB { std::numeric_limits<std::uint8_t>::max(), 0, 0 });
  REQUIRE(color_rgb_container.at(Color::GREEN) == RGB { 0, std::numeric_limits<std::uint8_t>::max(), 0 });
  REQUIRE(color_rgb_container.at(Color::BLUE) == RGB { 0, 0, std::numeric_limits<std::uint8_t>::max() });

  for ( const auto &rgb : color_rgb_container ) {

    std::cout << "array " << rgb << std::endl;
  }

  /*auto con2 = magic_enum::containers::set<Color>();
  for ( auto co : con2 ) {

    std::cout << co << std::endl;
  } */

/*  auto con3 = magic_enum::containers::bitset<Color>();
  for ( auto co : con3 ) {

    std::cout << co << std::endl;
  } */
}

TEST_CASE("containers_set") {

  auto con2 = magic_enum::containers::set<Color>();
  for ( auto co : con2 ) {

    (void)co;
    //std::cout << co << std::endl;
  }
}
