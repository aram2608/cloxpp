#include "tokens.hpp"

using namespace lox;

/*
    Constructor for our Token class
*/
Token::Token(TokenType type, string lexeme, std::any literal, int line) {
    this->type    = type;
    this->lexeme  = lexeme;
    this->literal = literal;
    this->line    = line;
}

// A helper function to convert our Token into a string
std::string Token::to_string() const {
    // Convert enum to string with magic_enum at compile time
    string tok_type = string(magic_enum::enum_name(type));

    // Default literal text
    string literal_txt = "nil";

    switch (type) {
    case TokenType::IDENTIFIER:
        literal_txt = lexeme;
        break;
    case TokenType::STRING:
        literal_txt = std::any_cast<string>(literal);
        break;
    case TokenType::NUMBER:
        literal_txt = std::to_string(std::any_cast<double>(literal));
        break;
    case TokenType::TRUE:
        literal_txt = "true";
        break;
    case TokenType::FALSE:
        literal_txt = "false";
        break;
    case TokenType::NIL:
        literal_txt = "nil";
        break;
    // Instead of defaulting to nil we override for each case
    default:
        break;
    }

    return "(" + tok_type + " , " + lexeme + " , " + literal_txt + ")";
}