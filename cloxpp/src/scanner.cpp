#include "scanner.hpp"

using namespace lox;

/*
    Constructor for our Scanner class
    We pass in the source code as string
*/
Scanner::Scanner(string source) {
    this->source = source;
}

vector<Token> Scanner::scan_tokens() {
    while (!is_end()) {
        start = current;
        scan();
    }
    tokens.emplace_back(TokenType::eof, "", nullptr, line);
    return tokens;
}

void Scanner::scan() {
    char c = advance();
    switch (c) {
    case '(':
        add_token(TokenType::LEFT_PAREN);
        break;
    case ')':
        add_token(TokenType::RIGHT_PAREN);
        break;
    case '{':
        add_token(TokenType::LEFT_BRACE);
        break;
    case '}':
        add_token(TokenType::RIGHT_BRACE);
        break;
    case ',':
        add_token(TokenType::COMMA);
        break;
    case '.':
        add_token(TokenType::DOT);
        break;
    case '-':
        add_token(TokenType::MINUS);
        break;
    case '+':
        add_token(TokenType::PLUS);
        break;
    case ';':
        add_token(TokenType::SEMICOLON);
        break;
    case '*':
        add_token(TokenType::STAR);
        break;
    // Ignore whitespaces
    case ' ':
    case '\r':
    case '\t':
        break;
    // This should be an error but I am not adding that yet
    default:
        cout << "Unexpected character." << endl;
        break;
    }
}

// Helper function to advance to next character
char Scanner::advance() {
    // We index the source string by incrementing the current position
    return source[current++];
}

// Overload to handle literal tokens
void Scanner::add_token(TokenType type, any literal) {
    string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, literal, line);
}

// Creates new token from lexeme
void Scanner::add_token(TokenType type) {
    add_token(type, nullptr);
}

// A helper function to test if we are at the end of the file
bool Scanner::is_end() {
    return current >= source.length();
}
