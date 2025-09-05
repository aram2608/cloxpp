#pragma once

#include <any>
#include <iostream>
// #include <magic_enum/magic_enum.hpp>
#include "/Users/ja1473/dev/tools/vcpkg/installed/arm64-osx/include/magic_enum/magic_enum.hpp"

namespace lox {
// We scope our aliases
using std::cout;
using std::endl;
using std::string;

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    // End of file
    eof
};

class Token {
  public:
    Token(TokenType type, string lexeme, std::any literal, int line);

    // Function to turn Tokens into strings
    // We do not want to modify anything so we declare this as a const member
    string to_string() const;

    TokenType type;
    string    lexeme;
    std::any  literal;
    int       line;

  private:
};
} // namespace lox