#!/bin/bash

# We set a variable for the path to vcpkg
VCPKG_ROOT="${VCPKG_ROOT:-$HOME/dev/tools/vcpkg}"

# We can then run our cmake build
# We pass in the toolchain file and set the correct build directory
cmake -B build -S . \
    -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
    -DTREE_WALK=OFF \
    -DVM_BUILD=ON

# We can now build the project
cmake --build build