#ifndef CLOX_COMPILER_HPP
#define CLOX_COMPILER_HPP

#include "../common.hpp"

struct Compiler {
    Compiler();

    void compile(std::string &source);
};

#endif