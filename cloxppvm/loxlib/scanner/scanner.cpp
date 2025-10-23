#include "scanner/scanner.hpp"

Scanner::Scanner(std::string &source) : source_(source) {
    start_ = 0;
    current_ = 0;
    line_ = 1;
}

std::vector<Token> Scanner::scan_tokens() {
    // We loop as long as we are not to the end of the file
    while (!is_end()) {
        // We save the starting position of the new lexeme
        start_ = current_;
        // We then commence scanning
        scan();
    }
    // At the end of scanning we place an EOF token
    tokens_.emplace_back(TokenType::eof, "", line_);
    return tokens_;
}

// The main logic to scan for tokens, we match the current char and pass off
// to different helpers to test the token type
void Scanner::scan() {
    char c = advance();

    if (is_digit(c)) {
        number();
        return;
    }

    if (is_alpha(c)) {
        identifier();
        return;
    }

    switch (c) {
    case ' ':
    case '\r':
    case '\t':
        break;
    case '\n': {
        ++line_;
        break;
    }
    case '(': {
        make_token(TokenType::LEFT_PAREN);
        break;
    }
    case ')': {
        make_token(TokenType::RIGHT_PAREN);
        break;
    }
    case '{': {
        make_token(TokenType::LEFT_BRACE);
        break;
    }
    case '}': {
        make_token(TokenType::RIGHT_BRACE);
        break;
    }
    case ';': {
        make_token(TokenType::SEMICOLON);
        break;
    }
    case '+': {
        make_token(TokenType::PLUS);
        break;
    }
    case '-': {
        make_token(TokenType::MINUS);
        break;
    }
    case '*': {
        make_token(TokenType::STAR);
        break;
    }
    case '/': {
        if (match('/')) {
            while (peek() != '\n' && !is_end()) {
                advance();
            }
        } else {
            make_token(TokenType::SLASH);
        }
        break;
    }
    case ',': {
        make_token(TokenType::COMMA);
        break;
    }
    case '.': {
        make_token(TokenType::DOT);
        break;
    }
    case '!': {
        make_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
    }
    case '>': {
        make_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
    }
    case '<': {
        make_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    }
    case '=': {
        make_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
    }
    case '"': {
        string();
        break;
    }
    }
}

void Scanner::string() {
    // We consume until we reach the end "
    while (peek() != '"' && !is_end()) {
        if (peek() == '\n') {
            ++line_;
        }
        // We advance for as long as we are in the loop
        advance();
    }
    // If we reach the end, we add an error token and return out
    if (is_end()) {
        error_token("Unterminated string");
        return;
    }

    // Otherwise we advance past the end quote and make out token
    advance();
    make_token(TokenType::STRING);
}

void Scanner::number() {
    // We consume all of the digits in the num
    while (is_digit(peek())) {
        advance();
    }

    // We jump over the fractional portions
    if (peek() == '.' && is_digit(peek_next())) {
        advance();

        while (is_digit(peek())) {
            advance();
        }
    }
    // We now make our token
    make_token(TokenType::NUMBER);
}

// Helper to make identifiers
void Scanner::identifier() {
    // We advance forward while we are in a digit or letter
    while (is_alpha(peek()) || is_digit(peek())) {
        advance();
    }
    // We then pass off the main logic to our helper method
    identifier_type();
}

// Main logic to check for the identifier type,
// An easier implementation is possible in C++ but this is closer to Rob's
// funky trie (DFA) state machine type thing
void Scanner::identifier_type() {
    // We match against the character at the start of our lexeme
    switch (source_[start_]) {
    case 'a': {
        check_keyword(1, "nd", TokenType::AND);
        break;
    }
    case 'c': {
        check_keyword(1, "lass", TokenType::CLASS);
        break;
    }
    case 'e': {
        check_keyword(1, "lse", TokenType::ELSE);
        break;
    }
    case 'f': {
        if (current_ - start_ > 1) {
            switch (source_[start_ + 1]) {
            case 'a': {
                check_keyword(2, "lse", TokenType::FALSE);
                break;
            }
            case 'o': {
                check_keyword(2, "or", TokenType::FOR);
                break;
            }
            case 'u': {
                check_keyword(2, "un", TokenType::FUN);
                break;
            }
            }
        }
        break;
    }
    case 'i': {
        check_keyword(1, "f", TokenType::IF);
        break;
    }
    case 'n': {
        check_keyword(1, "il", TokenType::NIL);
        break;
    }
    case 'o': {
        check_keyword(1, "r", TokenType::OR);
        break;
    }
    case 'p': {
        check_keyword(1, "rint", TokenType::PRINT);
        break;
    }
    case 'r': {
        check_keyword(1, "eturn", TokenType::RETURN);
        break;
    }
    case 's': {
        check_keyword(1, "uper", TokenType::SUPER);
        break;
    }
    case 't': {
        if (current_ - start_ > 1) {
            switch (source_[start_ + 1]) {
            case 'h': {
                check_keyword(2, "is", TokenType::THIS);
                break;
            }
            case 'r': {
                check_keyword(2, "ue", TokenType::TRUE);
                break;
            }
            }
        }
        break;
    }
    case 'v': {
        check_keyword(1, "ar", TokenType::VAR);
        break;
    }
    case 'w': {
        check_keyword(1, "hile", TokenType::WHILE);
        break;
    }
    }
}

// Function to check for our keywords
void Scanner::check_keyword(int start, std::string_view rest, TokenType type) {
    // We calculate the current position from the current position and
    // the start of this lexeme
    std::size_t len = current_ - start_;
    // We check if the length is as big as the start character plus the size
    // of the string we are viewing
    if (len == start + rest.size()) {
        /*
         * We get a raw pointer to the very first character in our source string
         * we then add the start position of the lexeme and the first char
         * we give it a length of rest.size()
         *
         * We are trying to satisfy the following constructor
         * string_view(const char* data, size_t len)
         */
        std::string_view tail(source_.data() + start_ + start, rest.size());
        // We then compare our substring with the rest of the lexeme we hope to match
        if (tail == rest) {
            // If our test passes we make a token from the type we passed in
            make_token(type);
            return;
        }
    }
    // Otherwise we make an identifier token
    make_token(TokenType::IDENTIFIER);
}

// Method to add tokens to the vector
void Scanner::make_token(TokenType ttype_t) {
    // We substring out the lexeme and stuff it into the token
    // this is a bit wasteful since a string_view would be more space efficient
    // We can't guarantee the source string will be around long enough for that
    // to not be an issue so this is a bit safer
    std::string lexeme = source_.substr(start_, current_ - start_);
    tokens_.emplace_back(ttype_t, lexeme, line_);
}

// Method to create error tokens
void Scanner::error_token(std::string message) {
    tokens_.emplace_back(TokenType::ERROR, message, line_);
}

// Simmple helper to advance forward in the source string
char Scanner::advance() { return source_[current_++]; }

// Simple helper to peek at the current char without consuming
char Scanner::peek() { return is_end() ? '\0' : source_[current_]; }

// Simple helper to peek a single char forward
char Scanner::peek_next() {
    if (current_ + 1 >= source_.length()) {
        return '\0';
    }
    return source_[current_ + 1];
}

// Helper method to match an expected token to our source string
bool Scanner::match(char expected) {
    // If we are at the end then we return false
    if (is_end()) {
        return false;
    }
    // Test to see if the current tok_type is expected for multichar operator
    if (source_[current_] != expected) {
        return false;
    }
    // Otherwise pre increment current character and return true
    ++current_;
    return true;
}

// Helper method to check for digits
bool Scanner::is_digit(char c) { return c >= '0' && c <= '9'; }

// Simple method to check for letters
bool Scanner::is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

// Helper to check if we are at the end of the source code
bool Scanner::is_end() { return current_ >= source_.length(); }

// Helper method to print out our tokens during debugging
void Scanner::debug() {
    for (auto t : tokens_) {
        fmt::println("Token: {} {} {}", t.type_, t.lexeme_, t.line_);
    }
}