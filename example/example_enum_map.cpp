#include <iostream>
#include <map>
#include <vector>

#include <magic_enum_map.hpp>

enum Color
{
    RED = 0,
    BLUE = 1,
    GREEN = 2
};

struct RGB
{
    int r, g, b;
};

std::ostream &operator<<(std::ostream &os, const RGB &rgb)
{
    os << "{" << rgb.r << ", " << rgb.g << ", " << rgb.b << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const magic_enum::enum_map<Color, int, std::map> &map)
{
    os << "map[Color::RED] -> " << map[Color::RED] << ", ";
    os << "map[Color::BLUE] -> " << map[Color::BLUE] << ", ";
    os << "map[Color::GREEN] -> " << map[Color::GREEN] << std::endl;
    return os;
}

void default_constructor()
{
    magic_enum::enum_map<Color, int, std::map> map;
    map[Color::RED] = 1;
    // map[Color::RED] -> 1, map[Color::BLUE] -> 0, map[Color::GREEN] -> 0
}

void variadic_constructor()
{
    magic_enum::enum_map<Color, int, std::map> map(1, 2, 3);
    // map[Color::RED] -> 1, map[Color::BLUE] -> 2, map[Color::GREEN] -> 3
}

void iterable_constructor()
{
    std::vector<int> values = {1, 2, 3};
    magic_enum::enum_map<Color, int, std::map> map(values);
    // map[Color::RED] -> 1, map[Color::BLUE] -> 2, map[Color::GREEN] -> 3

    std::vector<std::pair<Color, int>> v_values = {
        {Color::RED, 1}, {Color::GREEN, 3}, {Color::BLUE, 2}};
    magic_enum::enum_map<Color, int, std::map> v_map(v_values);
    // v_map[Color::RED] -> 1, v_map[Color::BLUE] -> 2, v_map[Color::GREEN] -> 3
}

void initializer_constructor()
{
    magic_enum::enum_map<Color, int, std::map> map = {
        {Color::RED, 1}, {Color::BLUE, 2}, {Color::GREEN, 3}};
    // map[Color::RED] -> 1, map[Color::BLUE] -> 2, map[Color::GREEN] -> 3
}

void partial_variadic_constructor()
{
    magic_enum::enum_map<Color, int, std::map> map(1, 2);
    // map[Color::RED] -> 1, map[Color::BLUE] -> 2, map[Color::GREEN] -> 0
}

void partial_iterable_constructor()
{
    std::vector<int> values = {2, 4, 8, 16};
    magic_enum::enum_map<Color, int, std::map> map(values);
    // map[Color::RED] -> 2, map[Color::BLUE] -> 4, map[Color::GREEN] -> 8

    std::vector<std::pair<Color, int>> v_values = {{Color::BLUE, 2}};
    magic_enum::enum_map<Color, int, std::map> v_map(v_values);
    // v_map[Color::RED] -> 0, v_map[Color::BLUE] -> 2, v_map[Color::GREEN] -> 0
}

void partial_initializer_constructor()
{
    magic_enum::enum_map<Color, int, std::map> map = {
        {Color::RED, 1}, {Color::GREEN, 3}};
    // map[Color::RED] -> 1, map[Color::BLUE] -> 0, map[Color::GREEN] -> 3
}

void iterate_map()
{
    magic_enum::enum_map<Color, int, std::map> map = {
        {Color::RED, 1}, {Color::BLUE, 2}, {Color::GREEN, 3}};

    for (auto [color, value] : map)
    {
        std::cout << magic_enum::enum_name(color) << " " << value << " ";
    }
    std::cout << std::endl;
    // prints:
    // RED 1 BLUE 2 GREEN 3
}

void insert_value()
{
    magic_enum::enum_map<Color, int, std::map> map;
    map.insert(map.begin(), 1);
    // map[Color::RED] -> 1, map[Color::BLUE] -> 0, map[Color::GREEN] -> 0

    map.insert(std::pair<Color, int>{Color::BLUE, 2});
    // map[Color::RED] -> 1, map[Color::BLUE] -> 2, map[Color::GREEN] -> 0
}

void insert_variadic_value()
{
    magic_enum::enum_map<Color, int, std::map> map;
    map.insert(map.begin(), 1, 2, 4, 8);
    // map[Color::RED] -> 1, map[Color::BLUE] -> 2, map[Color::GREEN] -> 4

    map.insert(std::pair<Color, int>{Color::BLUE, 1}, std::pair<Color, int>{Color::GREEN, 1});
    // map[Color::RED] -> 1, map[Color::BLUE] -> 1, map[Color::GREEN] -> 1
}

void insert_iterable_value()
{
    std::vector<int> values = {2, 4, 8, 16};
    magic_enum::enum_map<Color, int, std::map> map;
    map.insert(map.begin(), values);
    // map[Color::RED] -> 2, map[Color::BLUE] -> 4, map[Color::GREEN] -> 8

    std::vector<std::pair<Color, int>> v_values = {{Color::BLUE, 2}, {Color::GREEN, 2}};
    map.insert(v_values);
    // map[Color::RED] -> 2, map[Color::BLUE] -> 2, map[Color::GREEN] -> 2
}

void insert_intializer_value()
{
    magic_enum::enum_map<Color, int, std::map> map;
    map.insert(map.begin(), {1, 2, 3, 4});
    // map[Color::RED] -> 1, map[Color::BLUE] -> 2, map[Color::GREEN] -> 3

    map.insert({{Color::RED, 3}, {Color::GREEN, 1}});
    // map[Color::RED] -> 1, map[Color::BLUE] -> 2, map[Color::GREEN] -> 3
}

void complex_example()
{
    struct RGB
    {
        int r, g, b;
    };

    magic_enum::enum_map<Color, RGB, std::map> color_map = {
        {Color::RED, {255, 0, 0}},
        {Color::GREEN, {0, 255, 0}},
        {Color::BLUE, {0, 0, 255}}};

    color_map[Color::RED].r = 200;
    color_map.insert({Color::BLUE, {0, 0, 225}});

    // map[Color::RED] -> RGB(200, 0, 0),
    // map[Color::GREEN] -> RGB(0, 255, 0),
    // map[Color::BLUE] -> RGB(0, 0, 225)
}

struct test {
    int i;
    float j;
};

int main()
{
    // constructors
    default_constructor();
    variadic_constructor();
    iterable_constructor();
    partial_variadic_constructor();
    partial_iterable_constructor();
    partial_initializer_constructor();

    // iterate
    iterate_map();

    // insert
    insert_value();
    insert_variadic_value();
    insert_iterable_value();
    insert_intializer_value();

    // more detailed
    complex_example();
}