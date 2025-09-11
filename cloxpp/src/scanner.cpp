#include "scanner.hpp"

using namespace CppLox;
using std::string;
using std::vector;

/*
 * Constructor for our Scanner class
 * We pass in the source code as string
 */
Scanner::Scanner(string source)
    // We initialize our reserved keywords map
    : keywords{
          {"and", TokenType::AND},
          {"class", TokenType::CLASS},
          {"else", TokenType::ELSE},
          {"false", TokenType::FALSE},
          {"for", TokenType::FOR},
          {"fun", TokenType::FUN},
          {"if", TokenType::IF},
          {"nil", TokenType::NIL},
          {"or", TokenType::OR},
          {"print", TokenType::PRINT},
          {"return", TokenType::RETURN},
          {"super", TokenType::SUPER},
          {"this", TokenType::THIS},
          {"true", TokenType::TRUE},
          {"var", TokenType::VAR},
          {"while", TokenType::WHILE},
      } {
    this->source = source;
}

// Function to scan tokens and return them as a vector
vector<Token> Scanner::scan_tokens() {
    // We run the function until we meet the end of the file
    while (!is_end()) {
        // Our start position is recorded as the current position
        start = current;
        // We can now call our scan method for each token
        scan();
    }
    // At the end we append an EOF token
    tokens.emplace_back(TokenType::eof, "", nullptr, line);
    return tokens;
}

// Function to handle scanning of tokens
void Scanner::scan() {
    // The current character is the next character which is then consumed
    char c = advance();
    // Match the character to each case and add to vector
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
    case ':':
        add_token(TokenType::COLON);
        break;
    case '?':
        add_token(TokenType::QUESTION);
        break;
    // Multicharacter lexemes need the match helper method
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
    // Slashes need to be handled in a special way since comments will be C
    // style
    case '/':
        if (match('/')) {
            comment();
        } else if (match('*')) {
            multiline_comment();
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
        // Rob stuffed the numbers in the default case which kinda bugs me
        // but I can't think of a solution so ill leave it here for now too
        if (is_digit(c)) {
            add_number();
        } else if (is_alpha(c)) {
            add_identifier();
        } else {
            LoxError::error(line, "Unexpected character.");
        }
        break;
    }
}

// Helper function to advance to next character
char Scanner::advance() {
    // We index the source string by incrementing the current position
    return source[current++];
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
 * Function to peek at next character for longer lexemes
 * It is similar to advance but we do not consume the character
 * This approach is a lookahead
 */
char Scanner::peek() {
    // If we are at the end of the file we can return thi
    if (is_end())
        return '\0';
    return source[current];
}

// Function to peek ahead two characters for longer lexemes
char Scanner::peek_next() {
    if (current + 1 >= source.length())
        return '\0';
    return source[current + 1];
}

// Function to handle string token types
void Scanner::add_string() {
    // We need to peek forward until we meet the end of file or the closing
    // quote
    while (peek() != '"' && !is_end()) {
        // If we reach a new line we increment the line number
        if (peek() == '\n') {
            line++;
        }
        // We advance forwards
        advance();
    }
    // If we reach the end of file with no terminating "
    if (is_end()) {
        LoxError::error(line, "Unterminated string.");
        return;
    }

    // Closing "
    advance();

    // We trim quotes and save the substring to use downstream
    string value = source.substr(start + 1, current - 2 - start);
    add_token(TokenType::STRING, value);
}

// Function to wrap comment method
void Scanner::comment() {
    // A comment goes until the end of the line
    // so continue until we get to a newline
    while (peek() != '\n' && !is_end())
        advance();
}

// Function to handle multiline comments
void Scanner::multiline_comment() {
    // We store the start of the multiline comment
    int mlc_line = line;
    // Multiline comments should not go until the end of the file
    while (!is_end()) {
        // Skip past new lines
        if (peek() == '\n') {
            line++;
            advance();
        }
        // We try to catch the full "*/" sequence
        else if (peek() == '*' && peek_next() == '/') {
            // consume '*'
            advance();
            // consume '/'
            advance();
            // Break out of comment
            return;
        } else {
            // We skip everything else
            advance();
        }
    }
    // We throw an error if we don't close comments
    LoxError::error(mlc_line, "Multiline comment unterminated.");
}

// Function to handle adding number tokens
void Scanner::add_number() {
    // We peek ahead and confirm the next character is a digit and advance
    while (is_digit(peek()))
        advance();

    // We first need to look for the fractional part
    if (peek() == '.' && is_digit(peek_next())) {

        // Consume the .
        advance();

        while (is_digit(peek()))
            advance();
    }

    // We add the token
    add_token(TokenType::NUMBER,
              // We first collect the substring
              // We need to use the stod method to parse a string and return a double
              std::stod(std::string{source.substr(start, current - start)}));
}

// Function to handle tokenization of identifier
void Scanner::add_identifier() {
    // Check if character is alphanumeric and advance
    while (is_alpha_num(peek()))
        advance();

    // We first need to save the substring
    string text = string{source.substr(start, current - start)};

    // We define a new type variable
    TokenType type;

    /*
     * We search our reserved keywords map to find the saved string
     * map.find() takes a key as an arg and searches for an element with a
     * matching key If found, .find() returns an iterator pointing to the
     * element (a std::pair of key and value) If not found, .find() returns an
     * iterator to std::map::end().
     */
    auto match = keywords.find(text);
    /*
     * The map.end() method returns an interator that points past the end of the
     * map Meaning if our match is equal to it, then it is not found in the map
     * at all We can consider that as an identifier
     */
    if (match == keywords.end()) {
        type = TokenType::IDENTIFIER;
    } else {
        // If it does match we have a keyword
        // We assign match to the second value of the std::pair
        type = match->second;
    }

    // We can now add the token
    add_token(type);
}

// A helper function to test if we are at the end of the file
bool Scanner::is_end() {
    return current >= source.length();
}

// Utility function to test if a value is a digit
bool Scanner::is_digit(char c) {
    return c >= '0' && c <= '9';
}

// Helper function to catch characters that are letters
bool Scanner::is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

// Helper function to catch both numbers and letters
bool Scanner::is_alpha_num(char c) {
    // We use the or operator || to satisfy either conditions
    return is_alpha(c) || is_digit(c);
}