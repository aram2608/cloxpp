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
    void          scan();
    char          advance();
    bool          match(char expected);
    char          peek();
    void          add_string();

    void add_token(TokenType type, std::any literal);
    void add_token(TokenType type);

    LoxError errors;

  private:
    bool is_end();

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