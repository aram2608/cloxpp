#include "parser.hpp"

using namespace lox;

// Constructor for Parser class
// We take in a vector of tokens to consume
Parser::Parser(vector<Token> tokens) {
    this->tokens = tokens;
}

// Function to handle the first grammar rule, we simply match an equality
unique_ptr<Expr> Parser::expression() {
    return equality();
}

// Function to handle second grammar rule,
unique_ptr<Expr> Parser::equality() {
    // We first need to match a comparison
    unique_ptr<Expr> expr = comparison();

    // then enter a while loop for the (...) rule
    // We search for a != or a == then break, we do this with the match method
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        // We consume tokens and expressions until we break the loop
        Token            op    = previous();
        unique_ptr<Expr> right = comparison();
        // We have to move since unique_ptrs are read only
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    // We return the final expression
    return expr;
}

// Helper function to match != and == tokens in equality
bool Parser::match(initializer_list<TokenType> types) {
    // Iterate through each item in list
    for (TokenType type : types) {
        // Check and consume
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

// Function to return true if the current token is of the given type
bool Parser::check(TokenType type) {
    if (is_end())
        return false;
    return peek().type == type;
}

// Function consumes the current token and returns it
// Similar to scanner's advance()
Token Parser::advance() {
    if (!is_end())
        current++;
    return previous();
}

// Function to check if we have reach the EOF token
bool Parser::is_end() {
    return peek().type == TokenType::eof;
}

// Function to peek at the current token
// We return a read only reference since we don't
// want an expensive copy everytime
const Token& Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}