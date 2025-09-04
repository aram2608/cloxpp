#pragma once
#include "error.hpp"
#include "tokens.hpp"

#include <any>
#include <iostream>
#include <vector>

using std::string;
using std::vector;

namespace lox {
class Scanner {
  public:
    // Constructor for parsing code
    Scanner(string source);
    // Default constructor
    Scanner() : source("") {
    }

    string        source;
    vector<Token> tokens{};

    vector<Token> scan_tokens();

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
    void add_number();
    void add_identifier();
    bool is_end();
    bool is_digit(char c);
    bool is_alpha_num(char c);
    bool is_alpha(char c);

    /*
      Attributes to keep track of string index
    */
    // Start of string
    int start = 0;
    // Current token
    int current = 0;
    // Line number
    int line = 1;
};

} // namespace lox