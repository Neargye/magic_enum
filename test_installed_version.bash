#!/bin/bash
rm -rf ./build
rm -rf ./build2
rm -rf ./build3
rm -rf ./install_dir/
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release --log-level=DEBUG -DCMAKE_INSTALL_PREFIX=install_dir
cmake --build ./build -j$(nproc)
cmake --install ./build/ --prefix ./install_dir
CMAKE_PREFIX_PATH="./install_dir" cmake -S. -Bbuild2 -DMAGIC_ENUM_OPT_TEST_INSTALLED_VERSION=1 --log-level=DEBUG
cmake --build build2 --config Debug -j$(nproc) --verbose 
CMAKE_PREFIX_PATH="./install_dir" cmake -S. -Bbuild3 -DMAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG=1 --log-level=DEBUG
cmake --build build3 --config Debug -j$(nproc) --verbose 

