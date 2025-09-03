#pragma once

#include <any>
#include <iostream>

using std::cout;
using std::endl;
using std::string;

namespace lox {

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