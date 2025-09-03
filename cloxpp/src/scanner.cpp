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
    case '!':
        add_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
    case '=':
        add_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
    case '<':
        add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    case '>':
        add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
    // Slashes need to be handled in a special way since comments will be C style
    case '/':
        if (match('/')) {
            // A comment goes until the end of the line
            // so continue until we get to a newline
            while (peek() != '\n' && !is_end())
                advance();
        } else {
            add_token(TokenType::SLASH);
        }
        break;
    // Ignore whitespaces
    case ' ':
    case '\r':
    case '\t':
        break;
    // Handle newlines
    case '\n':
        line++;
        break;
    case '"':
        add_string();
        break;
    // This should be an error but I am not adding that yet
    default:
        errors.error(line, "Unexpected character.");
        break;
    }
}

// Helper function to advance to next character
char Scanner::advance() {
    // We index the source string by incrementing the current position
    return source[current++];
}

// Function to handle multicharacter operators
// Like advance but conditional
bool Scanner::match(char expected) {
    // Test to see if the end of file is reached
    if (is_end()) {
        return false;
    }
    // Test to see if the current tok_type is expected for multichar operator
    if (source[current] != expected) {
        return false;
    }
    // Otherwise pre increment current character and return true
    ++current;
    return true;
}

/*
    Function to peek at next character for longer lexemes
    It is similar to advance but we do not consume the character
    This approach is a lookahead
*/
char Scanner::peek() {
    // If we are at the end of the file we can return thi
    if (is_end())
        return '\0';
    return source[current];
}

// Function to handle string token types
void lox::Scanner::add_string() {
    // We need to peek forward until we meet the end of file or the closing quote
    while (peek() != '"' && !is_end()) {
        // If we reach a new line we increment the line number
        if (peek() == '\n') {
            line++;
        }
        // We advance forwards
        advance();
    }
    if (is_end()) {
        cout << "Error: unterminated string." << endl;
        return;
    }

    // Closing "
    advance();

    // We trim quotes and save the substring
    string value = source.substr(start + 1, current - 2 - start);
    add_token(TokenType::STRING, value);
}

// Overload to handle literal tokens
void Scanner::add_token(TokenType type, std::any literal) {
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
