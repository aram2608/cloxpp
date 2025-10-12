#ifndef TOKENS_HPP
#define TOKENS_HPP

#include <any>
#include <iostream>
#include <magic_enum/magic_enum.hpp>

namespace CppLox {

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
    QUESTION,
    MOD,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    MINUS_MINUS,
    PLUS_PLUS,

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
    // Token class constructor
    Token(TokenType type, std::string lexeme, std::any literal, int line)
        : type(type), lexeme(lexeme), literal(literal), line(line) {}

    // Function to turn Tokens into strings
    // We do not want to modify anything so we declare this as a const member
    std::string to_string() const {
        // Convert enum to string with magic_enum at compile time
        std::string tok_type = std::string(magic_enum::enum_name(type));

        // Default literal text
        std::string literal_txt = "nil";

        switch (type) {
        case TokenType::IDENTIFIER: {
            literal_txt = lexeme;
            break;
        }
        case TokenType::STRING: {
            literal_txt = std::any_cast<std::string>(literal);
            break;
        }
        case TokenType::NUMBER: {
            literal_txt = std::to_string(std::any_cast<double>(literal));
            break;
        }
        case TokenType::TRUE: {
            literal_txt = "true";
            break;
        }
        case TokenType::FALSE: {
            literal_txt = "false";
            break;
        }
        case TokenType::NIL: {
            literal_txt = "nil";
            break;
        }
        // Instead of defaulting to nil we override for each case
        default: {
            break;
        }
        }
        return "(" + tok_type + " , " + lexeme + " , " + literal_txt + ")";
    }

    TokenType type;
    std::string lexeme;
    std::any literal;
    int line = 0;
};
} // namespace CppLox

#endif