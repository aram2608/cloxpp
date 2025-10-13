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

// Instead of using magic_enum we are going to make a massive formatter for out
// token types to use fmt since its lighter weight
template <> struct fmt::formatter<TokenType> : formatter<string_view> {
    auto format(TokenType ttype, format_context &ctx) const -> format_context::iterator;
};

struct Token {
    Token(TokenType type, std::string lexeme, int line) : type(type), lexeme(lexeme), line(line) {};

    TokenType type;
    std::string lexeme;
    int line;
};

#endif

inline auto fmt::formatter<TokenType>::format(TokenType ttype_t, format_context &ctx) const
    -> format_context::iterator {
    string_view name = "Unknown token type";
    switch (ttype_t) {
    case TokenType::LEFT_PAREN: {
        name = "LEFT_PAREN";
        break;
    }
    case TokenType::RIGHT_PAREN: {
        name = "RIGHT_PAREN";
        break;
    }
    case TokenType::LEFT_BRACE: {
        name = "LEFT_BRACE";
        break;
    }
    case TokenType::RIGHT_BRACE: {
        name = "RIGHT_BRACE";
        break;
    }
    case TokenType::COMMA: {
        name = "COMMA";
        break;
    }
    case TokenType::DOT: {
        name = "DOT";
        break;
    }
    case TokenType::MINUS: {
        name = "MINUS";
        break;
    }
    case TokenType::PLUS: {
        name = "PLUS";
        break;
    }
    case TokenType::SEMICOLON: {
        name = "SEMICOLON";
        break;
    }
    case TokenType::SLASH: {
        name = "SLASH";
        break;
    }
    case TokenType::STAR: {
        name = "STAR";
        break;
    }
    case TokenType::COLON: {
        name = "COLON";
        break;
    }
    case TokenType::BANG: {
        name = "BANG";
        break;
    }
    case TokenType::BANG_EQUAL: {
        name = "BANG_EQUAL";
        break;
    }
    case TokenType::EQUAL: {
        name = "EQUAL";
        break;
    }
    case TokenType::EQUAL_EQUAL: {
        name = "EQUAL_EQUAL";
        break;
    }
    case TokenType::GREATER: {
        name = "GREATER";
        break;
    }
    case TokenType::GREATER_EQUAL: {
        name = "GREATER_EQUAL";
        break;
    }
    case TokenType::LESS: {
        name = "LESS";
        break;
    }
    case TokenType::LESS_EQUAL: {
        name = "LESS_EQUAL";
        break;
    }
    case TokenType::IDENTIFIER: {
        name = "IDENTIFIER";
        break;
    }
    case TokenType::STRING: {
        name = "STRING";
        break;
    }
    case TokenType::NUMBER: {
        name = "NUMBER";
        break;
    }
    case TokenType::AND: {
        name = "AND";
        break;
    }
    case TokenType::CLASS: {
        name = "CLASS";
        break;
    }
    case TokenType::ELSE: {
        name = "ELSE";
        break;
    }
    case TokenType::FALSE: {
        name = "FALSE";
        break;
    }
    case TokenType::FUN: {
        name = "FUN";
        break;
    }
    case TokenType::FOR: {
        name = "FOR";
        break;
    }
    case TokenType::IF: {
        name = "IF";
        break;
    }
    case TokenType::NIL: {
        name = "NIL";
        break;
    }
    case TokenType::OR: {
        name = "OR";
        break;
    }
    case TokenType::PRINT: {
        name = "PRINT";
        break;
    }
    case TokenType::RETURN: {
        name = "RETURN";
        break;
    }
    case TokenType::SUPER: {
        name = "SUPER";
        break;
    }
    case TokenType::THIS: {
        name = "THIS";
        break;
    }
    case TokenType::TRUE: {
        name = "TRUE";
        break;
    }
    case TokenType::VAR: {
        name = "VAR";
        break;
    }
    case TokenType::WHILE: {
        name = "WHILE";
        break;
    }
    case TokenType::ERROR: {
        name = "ERROR";
        break;
    }
    case TokenType::eof: {
        name = "EOF";
        break;
    }
    }
    return formatter<string_view>::format(name, ctx);
}