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
string Token::to_string() const {

    // Define our literal text and tok_type as strings
    std::string literal_txt;
    std::string tok_type;

    // We need to set the literal text to the string representation
    // of each data type
    switch (type) {
    case (TokenType::IDENTIFIER):
        literal_txt = lexeme;
        tok_type    = "IDENTIFIER";
        break;
    case (TokenType::STRING):
        literal_txt = std::any_cast<std::string>(literal);
        tok_type    = "STRING";
        break;
    case (TokenType::NUMBER):
        literal_txt = std::to_string(std::any_cast<double>(literal));
        tok_type    = "NUMBER";
        break;
    case (TokenType::TRUE):
        literal_txt = "true";
        tok_type    = "TRUE";
        break;
    case (TokenType::FALSE):
        literal_txt = "false";
        tok_type    = "FALSE";
        break;
    case (TokenType::SEMICOLON):
        literal_txt = "nil";
        tok_type    = "SEMICOLON";
        break;
    case (TokenType::EQUAL):
        literal_txt = "nil";
        tok_type    = "EQUAL";
        break;
    case (TokenType::EQUAL_EQUAL):
        literal_txt = "nil";
        tok_type    = "EQUAL_EQUAL";
        break;
    case (TokenType::LEFT_PAREN):
        literal_txt = "nil";
        tok_type    = "LEFT_PAREN";
        break;
    case (TokenType::RIGHT_PAREN):
        literal_txt = "nil";
        tok_type    = "RIGHT_PAREN";
        break;
    case (TokenType::LEFT_BRACE):
        literal_txt = "nil";
        tok_type    = "LEFT_BRACE";
        break;
    case (TokenType::RIGHT_BRACE):
        literal_txt = "nil";
        tok_type    = "RIGHT_BRACE";
        break;
    case (TokenType::BANG):
        literal_txt = "nil";
        tok_type    = "BANG";
        break;
    case (TokenType::BANG_EQUAL):
        literal_txt = "nil";
        tok_type    = "BANG_EQUAL";
        break;
    case (TokenType::STAR):
        literal_txt = "nil";
        tok_type    = "STAR";
        break;
    case (TokenType::PLUS):
        literal_txt = "nil";
        tok_type    = "PLUS";
        break;
    case (TokenType::MINUS):
        literal_txt = "nil";
        tok_type    = "STAR";
        break;
    case (TokenType::SLASH):
        literal_txt = "nil";
        tok_type    = "SLASH";
        break;
    case (TokenType::GREATER):
        literal_txt = "nil";
        tok_type    = "GREATER";
        break;
    case (TokenType::GREATER_EQUAL):
        literal_txt = "nil";
        tok_type    = "GREATER_EQUAL";
        break;
    case (TokenType::LESS):
        literal_txt = "nil";
        tok_type    = "LESS";
        break;
    case (TokenType::LESS_EQUAL):
        literal_txt = "nil";
        tok_type    = "LESS_EQUAL";
        break;
    case (TokenType::eof):
        literal_txt = "nil";
        tok_type    = "EOF";
        break;
    default:
        literal_txt = "nil";
    }
    return "(" + tok_type + " , " + lexeme + " , " + literal_txt + ")";
}