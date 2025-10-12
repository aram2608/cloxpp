#ifndef CLOX_SCANNER_HPP
#define CLOX_SCANNER_HPP

#include "../common.hpp"
#include "utilities/tokens.hpp"

#include <map>

struct Scanner {
    Scanner(std::string source);
    int start;
    int current;
    int line;
    std::string source;
    const std::map<std::string, TokenType> keywords;
};

#endif