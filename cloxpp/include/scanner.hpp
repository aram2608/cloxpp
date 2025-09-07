#pragma once
#include "error.hpp"
#include "tokens.hpp"

#include <any>
#include <iostream>
#include <map>
#include <vector>

namespace lox {
class Scanner {
    // Source code and tokens
    std::string        source;
    std::vector<Token> tokens{};
    // Attributes to keep track of string index
    //  Start of string
    int start = 0;
    // Current token
    int current = 0;
    // Line number
    int line = 1;

  public:
    // Constructor for parsing code
    Scanner(std::string source);
    // Default constructor
    Scanner() : source("") {
    }

    std::vector<Token> scan_tokens();

    LoxError errors;

  private:
    void scan();
    char advance();
    void add_token(TokenType type, std::any literal);
    void add_token(TokenType type);
    bool match(char expected);
    char peek();
    char peek_next();
    void add_string();
    void comment();
    void multiline_comment();
    void add_number();
    void add_identifier();
    bool is_end();
    bool is_digit(char c);
    bool is_alpha_num(char c);
    bool is_alpha(char c);

    // Reserved keywords
    const std::map<std::string, TokenType> keywords;
};

} // namespace lox