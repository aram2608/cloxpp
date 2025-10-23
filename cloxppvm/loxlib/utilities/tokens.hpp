#ifndef TOKENS_HPP
#define TOKENS_HPP

#include "../common.hpp"

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
    COLON,

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

    ERROR,
    // End of file
    eof
};

// Instead of using magic_enum we are going to make a massive formatter for our
// token types to use fmt since its lighter weight
template <> struct fmt::formatter<TokenType> : formatter<string_view> {
    auto format(TokenType ttype, format_context &ctx) const -> format_context::iterator;
};

struct Token {
    Token(TokenType type, std::string lexeme, int line)
        : type_(type), lexeme_(lexeme), line_(line) {};

    TokenType type_;
    std::string lexeme_;
    int line_;
};

#endif