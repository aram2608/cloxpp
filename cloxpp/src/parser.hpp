#pragma once

#include "error.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "tokens.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace CppLox {

class Parser {
    std::vector<Token> tokens;
    int                current = 0;
    struct ParseError : public std::runtime_error {
        // We inherit all the constructors from std::runtime_error
        using std::runtime_error::runtime_error;
    };

  public:
    Parser(std::vector<Token> tokens);
    std::vector<std ::unique_ptr<Stmt>> parse();
    LoxError                            errors;

  private:
    std::unique_ptr<Stmt>              declaration();
    std::unique_ptr<Stmt>              var_declaration();
    std::unique_ptr<Stmt>              statement();
    std::unique_ptr<Stmt>              print_statement();
    std::unique_ptr<Stmt>              expression_statement();
    std::vector<std::unique_ptr<Stmt>> block();
    std::unique_ptr<Expr>              expression();
    std::unique_ptr<Expr>              assignment();
    std::unique_ptr<Expr>              comma();
    std::unique_ptr<Expr>              conditional();
    std::unique_ptr<Expr>              equality();
    std::unique_ptr<Expr>              comparison();
    std::unique_ptr<Expr>              term();
    std::unique_ptr<Expr>              factor();
    std::unique_ptr<Expr>              unary();
    std::unique_ptr<Expr>              primary();
    bool                               match(std::initializer_list<TokenType> types);
    Token                              consume(TokenType type, std::string message);
    bool                               check(TokenType type);
    Token                              advance();
    Token                              peek();
    Token                              previous();
    bool                               is_end();
    ParseError                         error(Token token, std::string message);
    void                               synchronize();
};
} // namespace CppLox