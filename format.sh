#!/bin/bash

# A helper script to format because I am lazy
clang-format cloxpp/src/ast/* -i
clang-format cloxpp/src/debug/* -i
clang-format cloxpp/src/runtime/* -i
clang-format cloxpp/src/utils/* -i
clang-format cloxpp/src/*.cpp -i
clang-format cloxpp/src/*.hpp -i