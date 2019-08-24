#include <magic_enum.hpp>

#include <cstdlib>

enum class Color : int { RED = -10, BLUE = 0, GREEN = 10 };

int main() {
  auto res1 = magic_enum::enum_name(Color::RED);
  auto res2 = magic_enum::enum_cast<Color>("BLUE");
  auto res3 = magic_enum::enum_cast<Color>(10);

  bool success = (res1 == "RED") && (res2.value() == Color::BLUE) && (res3.value() == Color::GREEN);

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
