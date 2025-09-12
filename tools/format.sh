#!/bin/bash

# Script to format all tracked files since Im lazy
git ls-files '*.cpp' '*.hpp' | xargs -r clang-format -i