#include "tokens.hpp"

using namespace lox;

/*
    Constructor for our Token class
*/
Token::Token(TokenType type, string lexeme, any literal, int line) {
    this->type    = type;
    this->lexeme  = lexeme;
    this->literal = literal;
    this->line    = line;
}

// A helper function to convert our Token into a string
string Token::to_string() const {

    // Define our literal text
    std::string literal_txt;

    // We need to set the literal text to the string representation
    // of each data type
    switch (type) {
    case (TokenType::IDENTIFIER):
        literal_txt = lexeme;
        break;
    case (TokenType::STRING):
        literal_txt = std::any_cast<std::string>(literal);
        break;
    case (TokenType::NUMBER):
        literal_txt = std::to_string(std::any_cast<double>(literal));
        break;
    case (TokenType::TRUE):
        literal_txt = "true";
        break;
    case (TokenType::FALSE):
        literal_txt = "false";
        break;
    default:
        literal_txt = "nil";
    }
    return " " + lexeme + " " + literal_txt;
}