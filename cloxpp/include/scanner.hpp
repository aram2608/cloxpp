#pragma once
#include "tokens.hpp"

#include <any>
#include <iostream>
#include <vector>

using std::any;
using std::string;
using std::vector;

namespace lox {
class Scanner {
  public:
    Scanner(string source);

    string        source;
    vector<Token> tokens{};

    vector<Token> scan_tokens();
    void          scan();
    char          advance();

    void add_token(TokenType type, any literal);
    void add_token(TokenType type);

  private:
    bool is_end();

    // Attributes to keep track of string index

    // Start of string
    int start = 0;
    // Current token
    int current = 0;
    // Line number
    int line = 1;
};

} // namespace lox