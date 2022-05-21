#include <magic_enum.hpp>

#include <iostream>

enum class Color : int { RED = -10, BLUE = 0, GREEN = 10 };

struct RGB {
    int r, g, b;
};

int main() {
    magic_enum::enum_map<Color, RGB> map = {RGB{255, 0, 0}, RGB{0, 0, 255}, RGB{0, 255, 0}};
    return 0;
}