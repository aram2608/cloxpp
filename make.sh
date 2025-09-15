#!/bin/bash

# # We set a variable for the path to vcpkg and export to env
VCPKG_ROOT="${VCPKG_ROOT:-$HOME/dev/tools/vcpkg}"
export VCPKG_ROOT

# Command line arg
OPTION=$1

# Newbie code but we test if the proper args are passed in and run the respective
# build mode
if [[ -z $OPTION ]]; then
    echo "Build script usage: "
    echo "      1 = Tree walk interpreter debug build"
    echo "      2 = Tree walk interpreter release build"
    echo "      3 = VM interpreter debug build"
    echo "      4 = VM interpreter release build"
elif [[ $OPTION == 1 ]]; then
    cmake --preset tw-debug
    cmake --build --preset tw-debug
elif [[ $OPTION == 2 ]]; then
    cmake --preset tw-release
    cmake --build --preset tw-release
elif [[ $OPTION == 3 ]]; then
    cmake --preset vm-debug
    cmake --build --preset vm-debug
elif [[ $OPTION == 4 ]]; then
    cmake --preset vm-release
    cmake --build --preset vm-release
else
    echo "Improper use. Integer arguments only"
fi