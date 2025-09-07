#include "parser.hpp"

using namespace lox;
using std::initializer_list;
using std::unique_ptr;
using std::vector;

// Constructor for Parser class
// We take in a vector of tokens to consume
Parser::Parser(vector<Token> tokens) {
    this->tokens = tokens;
}

// Function to parse code
vector<unique_ptr<Stmt>> Parser::parse() {
    // We create a new vector to store pointers to our statements
    vector<unique_ptr<Stmt>> stmts;
    while (!is_end()) {
        stmts.push_back(declaration());
    }
    return stmts;
}

// Function for handling declarations
unique_ptr<Stmt> Parser::declaration() {
    try {
        // We match a var keyword and return var_declaration
        if (match({TokenType::VAR}))
            return var_declaration();

        // By default we return a statement
        return statement();
        // catch erros and get the parser to chill out
    } catch (ParseError error) {
        synchronize();
        // Since we are returning a pointer type
        // we need to return nullptr instead of {}
        return nullptr;
    }
}

// Function to handle var_declar
unique_ptr<Stmt> lox::Parser::var_declaration() {
    // Match an identifier token and consume it
    Token identifier = consume(TokenType::IDENTIFIER, "Expected identifier.");

    // We initialize a value with nullptr
    unique_ptr<Expr> initializer = nullptr;
    // If we match an equal token bind initializer to the output of expression
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    // We match a semicolon and throw an error if the statement is not closed
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<Var>(std::move(identifier), std::move(initializer));
}

// Function to handle parsing of statements
// Lox programs are a series of statements so
// all scripts start here defined by our grammar rules
unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::PRINT}))
        return print_statement();

    return expression_statement();
}

// Function to handel Lox's built in print statement
unique_ptr<Stmt> Parser::print_statement() {
    // We create our base expression
    unique_ptr<Expr> value = expression();
    // We then consume the semicolon and toss an error if the statement was not
    // finished
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    // We wrap our expression in a statement and return it
    return std::make_unique<Print>(std::move(value));
}

// Function to handle expression statement
unique_ptr<Stmt> Parser::expression_statement() {
    // Creates our base expressions
    unique_ptr<Expr> expr = expression();
    // Consume the end of the statement and toss an error otherwise
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    // We wrap our expression in the Expression statement and return it
    return std::make_unique<Expression>(std::move(expr));
}

// Function to handle the first grammar rule, we simply match an equality
unique_ptr<Expr> Parser::expression() {
    // challenge code
    // return comma();
    return equality();
}

// Function to handle multiple expressions separated by commas
unique_ptr<Expr> lox::Parser::comma() {
    unique_ptr<Expr> expr = conditional();

    while (match({TokenType::COMMA})) {
        Token            op    = previous();
        unique_ptr<Expr> right = conditional();
        expr                   = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

unique_ptr<Expr> Parser::conditional() {
    unique_ptr<Expr> expr = equality();

    if (match({TokenType::QUESTION})) {
        Token            op    = previous();
        unique_ptr<Expr> right = expression();
        expr                   = std::make_unique<Binary>(std::move(expr), op, std::move(right));
        if (match({TokenType::COLON})) {
            Token            op    = previous();
            unique_ptr<Expr> right = conditional();
            expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
            return expr;
        } else {
            throw error(peek(), "Expected ':'.");
        }
    }

    return expr;
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

// Function to handle comparison operatios
unique_ptr<Expr> Parser::comparison() {
    unique_ptr<Expr> expr = term();
    // We look through each comparison type and break when
    // we no longer come across a comparison operator
    while (match(
        {TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token            op    = previous();
        unique_ptr<Expr> right = term();
        // We need to move ownership to the new expression
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    // We return the final expression
    return expr;
}

unique_ptr<Expr> Parser::term() {
    unique_ptr<Expr> expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token            op    = previous();
        unique_ptr<Expr> right = factor();
        // We need to move ownership to the new expression
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

unique_ptr<Expr> Parser::factor() {
    unique_ptr<Expr> expr = unary();
    // Loop until we dont come across multi or division anymore
    while (match({TokenType::STAR, TokenType::SLASH})) {
        Token            op    = previous();
        unique_ptr<Expr> right = unary();
        // We need to move ownership to the new expression
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

unique_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token            op    = previous();
        unique_ptr<Expr> right = unary();
        return std::make_unique<Unary>(op, std::move(right));
    }
    return primary();
}

unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE}))
        return std::make_unique<Literal>(false);
    if (match({TokenType::TRUE}))
        return std::make_unique<Literal>(true);
    if (match({TokenType::NIL}))
        return std::make_unique<Literal>(nullptr);

    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_unique<Literal>(previous().literal);
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<Variable>(previous());
    }

    if (match({TokenType::LEFT_PAREN})) {
        unique_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Grouping>(std::move(expr));
    }
    // Catch bad tokens
    throw error(peek(), "Expect expression.");
}

// Helper function to match token types
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

// Function to a consume a token and return it
// Useful for closing off statements or parenthesis, blocks, etc
Token Parser::consume(TokenType type, std::string message) {
    if (check(type))
        return advance();

    throw error(peek(), message);
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
/*
 * Function to peek at the current token
 * We return a read only reference since we don't
 * want an expensive copy everytime
 */
Token Parser::peek() {
    return tokens.at(current);
}

Token Parser::previous() {
    return tokens.at(current - 1);
}

Parser::ParseError Parser::error(Token token, std::string_view message) {
    errors.error(token, message);
    return ParseError{""};
}

void Parser::synchronize() {
    advance();

    while (!is_end()) {
        if (previous().type == TokenType::SEMICOLON)
            return;

        /*
         * We synchronize on statements in lox
         * After a semicolon, we can assume that we
         * have started a new statement
         */
        switch (peek().type) {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
            return;
        }

        advance();
    }
}
