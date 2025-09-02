#!/bin/bash

export VCPKG_ROOT=~/dev/tools/vcpkg

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build build