#include <iostream>
#include <map>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <magic_enum_map.hpp>

enum Color
{
    RED = 0,
    BLUE = 1,
    GREEN = 2
};

enum KeyState
{
    KEY_PRESSED,
    KEY_RELEASED,
    KEY_UP,
    KEY_DOWN
};

struct RGB
{
    int r, g, b;

    friend std::ostream &operator<<(std::ostream &os, const RGB &rgb)
    {
        os << "{" << rgb.r << ", " << rgb.g << ", " << rgb.b << "}";
        return os;
    }
};

void construct_map()
{
    std::cout << "\nCreate map and then set values:" << std::endl;
    magic_enum::enum_map<Color, RGB, std::map> default_map;
    default_map[Color::RED] = RGB{255, 0, 0};
    default_map[Color::BLUE] = RGB{0, 0, 255};
    default_map[Color::GREEN] = RGB{0, 255, 0};

    std::cout << "Color::RED is: " << default_map[Color::RED] << std::endl;
    std::cout << "Color::BLUE is: " << default_map[Color::BLUE] << std::endl;
    std::cout << "Color::GREEN is: " << default_map[Color::GREEN] << std::endl;

    std::cout << "\nCreate map and pass initial values through constructor:" << std::endl;
    magic_enum::enum_map<Color, RGB, std::map> const_map(RGB{255, 0, 0}, RGB{0, 0, 255}, RGB{0, 255, 0});
    std::cout << "Color::RED is: " << const_map[Color::RED] << std::endl;
    std::cout << "Color::BLUE is: " << const_map[Color::BLUE] << std::endl;
    std::cout << "Color::GREEN is: " << const_map[Color::GREEN] << std::endl;

    std::cout << "\nCreate map and pass iteratable constructor:" << std::endl;
    std::vector<RGB> values = {{255, 0, 0}, {0, 0, 255}, {0, 255, 0}};
    magic_enum::enum_map<Color, RGB, std::map> iterable_map(values);
    std::cout << "Color::RED is: " << iterable_map[Color::RED] << std::endl;
    std::cout << "Color::BLUE is: " << iterable_map[Color::BLUE] << std::endl;
    std::cout << "Color::GREEN is: " << iterable_map[Color::GREEN] << std::endl;

    std::cout << "\nIterable can also be of pairs:" << std::endl;
    std::vector<std::pair<Color, RGB>> pair_values = {
        {Color::RED, {255, 0, 0}},
        {Color::BLUE, {0, 0, 255}},
        {Color::GREEN, {0, 255, 0}}};
    magic_enum::enum_map<Color, RGB, std::map> pair_iterable_map(values);
    std::cout << "Color::RED is: " << pair_iterable_map[Color::RED] << std::endl;
    std::cout << "Color::BLUE is: " << pair_iterable_map[Color::BLUE] << std::endl;
    std::cout << "Color::GREEN is: " << pair_iterable_map[Color::GREEN] << std::endl;

    std::cout << "\nCreate map using initializer list:" << std::endl;
    magic_enum::enum_map<Color, RGB, std::map> initializer_map = {
        {Color::RED, {255, 0, 0}},
        {Color::BLUE, {0, 0, 255}},
        {Color::GREEN, {0, 255, 0}}};
    std::cout << "Color::RED is: " << initializer_map[Color::RED] << std::endl;
    std::cout << "Color::BLUE is: " << initializer_map[Color::BLUE] << std::endl;
    std::cout << "Color::GREEN is: " << initializer_map[Color::GREEN] << std::endl;
}

void partial_map()
{
    std::cout << "\nCreate map defining only some defaults" << std::endl;
    std::cout << "With the constructor:" << std::endl;
    magic_enum::enum_map<Color, RGB, std::map> partial_const_map(RGB{255, 0, 0}, RGB{0, 0, 255});
    std::cout << "Color::RED is: " << partial_const_map[Color::RED] << std::endl;
    std::cout << "Color::BLUE is: " << partial_const_map[Color::BLUE] << std::endl;
    std::cout << "Color::GREEN is: " << partial_const_map[Color::GREEN] << std::endl;

    std::cout << "\nWith an iterable:" << std::endl;
    std::vector<RGB> partial_values = {{255, 0, 0}, {0, 0, 255}};
    magic_enum::enum_map<Color, RGB, std::map> partial_iterable_map(partial_values);
    std::cout << "Color::RED is: " << partial_iterable_map[Color::RED] << std::endl;
    std::cout << "Color::BLUE is: " << partial_iterable_map[Color::BLUE] << std::endl;
    std::cout << "Color::GREEN is: " << partial_iterable_map[Color::GREEN] << std::endl;

    std::cout << "\nAnd with an initializer list:" << std::endl;
    magic_enum::enum_map<Color, RGB, std::map> partial_initializer_map = {
        {Color::RED, {255, 0, 0}},
        {Color::GREEN, {0, 255, 0}}};
    std::cout << "Color::RED is: " << partial_initializer_map[Color::RED] << std::endl;
    std::cout << "Color::BLUE is: " << partial_initializer_map[Color::BLUE] << std::endl;
    std::cout << "Color::GREEN is: " << partial_initializer_map[Color::GREEN] << std::endl;
}

void iterate_map()
{
    std::cout << "\nIterate through map:" << std::endl;
    magic_enum::enum_map<Color, RGB, std::map> map = {
        {Color::RED, {255, 0, 0}},
        {Color::BLUE, {0, 0, 255}},
        {Color::GREEN, {0, 255, 0}}};

    for (auto [color, rgb] : map)
        std::cout << magic_enum::enum_name(color) << " " << rgb << std::endl;
}

void insert_map()
{
    std::cout << "\nInsert into map" << std::endl;
    magic_enum::enum_map<Color, RGB, std::map> map;

    std::cout << "By passing arguments directly:" << std::endl;
    map.insert(map.begin(), RGB{255, 0, 0}, RGB{0, 0, 255});
    std::cout << "Color::RED is: " << map[Color::RED] << std::endl;
    std::cout << "Color::BLUE is: " << map[Color::BLUE] << std::endl;
    std::cout << "Color::GREEN is: " << map[Color::GREEN] << std::endl;

    std::cout << "\nBy passing an iterable:" << std::endl;
    std::vector<RGB> values = {{255, 5, 5}, {5, 5, 255}};
    map.insert(map.begin(), values);
    std::cout << "Color::RED is: " << map[Color::RED] << std::endl;
    std::cout << "Color::BLUE is: " << map[Color::BLUE] << std::endl;
    std::cout << "Color::GREEN is: " << map[Color::GREEN] << std::endl;

    std::cout << "\nOr by passing another map/iterable pair:" << std::endl;
    std::map<Color, RGB> normal_map = {
        {Color::RED, {255, 10, 10}},
        {Color::BLUE, {10, 255, 10}}};
    map.insert(normal_map);
    std::cout << "Color::RED is: " << map[Color::RED] << std::endl;
    std::cout << "Color::BLUE is: " << map[Color::BLUE] << std::endl;
    std::cout << "Color::GREEN is: " << map[Color::GREEN] << std::endl;
}

void types_map()
{
    std::cout << "\nFinally, you can specify the map object used," << std::endl;
    std::cout << "just make sure your map is an iterable:" << std::endl;

    magic_enum::enum_map<Color, int, std::unordered_map> unordered_map;
    std::cout << "enum_type: " << "enum Color" << std::endl;
    std::cout << "mapped_type: " << "int" << std::endl;
    std::cout << "value_type: " << "struct std::pair<enum Color, int>" << std::endl;
    std::cout << "map_type: " << "class std::unordered_map<Color, int>" << std::endl;
}

int main()
{
    construct_map();
    partial_map();
    iterate_map();
    insert_map();
    types_map();
}