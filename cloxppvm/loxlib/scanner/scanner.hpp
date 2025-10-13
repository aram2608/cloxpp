#ifndef CLOX_SCANNER_HPP
#define CLOX_SCANNER_HPP

#include "../common.hpp"
#include "utilities/tokens.hpp"

#include <map>

struct Scanner {
    Scanner(std::string source);
    void scan_tokens();
    void scan();
    void make_token(TokenType ttype_t);
    void error_token(std::string message);
    char advance();
    char peek();
    char peek_next();
    bool match(char expected);
    void string();
    void number();
    void identifier();
    void identifier_type();
    void check_keyword(int begin, std::string_view rest, TokenType type);
    bool is_digit(char c);
    bool is_alpha(char c);
    bool is_end();
    void debug();
    int start;
    int current;
    int line;
    std::string source;
    std::vector<Token> tokens;
};

#endif