// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2020 - 2026 Daniil Goncharov <neargye@gmail.com>.
// Copyright (c) 2020 - 2023 Uruha Komachin <uruhakomachin@gmail.com>.
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

#include <iostream>

#include <magic_enum/magic_enum.hpp>

enum class Language : int {
    日本語 = 10,
    한국어 = 20,
    English = 30,
    😃 = 40,
};

int main() {
  std::cout << magic_enum::enum_name(Language::日本語) << std::endl; // Japanese
  std::cout << magic_enum::enum_name(Language::한국어) << std::endl; // Korean
  std::cout << magic_enum::enum_name(Language::English) << std::endl; // English
  std::cout << magic_enum::enum_name(Language::😃) << std::endl; // Emoji

  std::cout << std::boolalpha;
  std::cout << (magic_enum::enum_cast<Language>("日本語").value() == Language::日本語) << std::endl; // true

  return 0;
}
