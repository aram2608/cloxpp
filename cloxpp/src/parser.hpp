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
    std::vector<std ::shared_ptr<Stmt>> parse();
    LoxError                            errors;

  private:
    std::shared_ptr<Stmt>              declaration();
    std::shared_ptr<Stmt>              var_declaration();
    std::shared_ptr<Stmt>              statement();
    std::shared_ptr<Stmt>              if_statement();
    std::shared_ptr<Stmt>              while_statement();
    std::shared_ptr<Stmt>              for_statement();
    std::shared_ptr<Stmt>              print_statement();
    std::shared_ptr<Stmt>              return_statement();
    std::shared_ptr<Function>          function(std::string kind);
    std::shared_ptr<Stmt>              expression_statement();
    std::vector<std::shared_ptr<Stmt>> block();
    std::shared_ptr<Expr>              expression();
    std::shared_ptr<Expr>              assignment();
    std::shared_ptr<Expr>              _or();
    std::shared_ptr<Expr>              _and();
    std::shared_ptr<Expr>              comma();
    std::shared_ptr<Expr>              conditional();
    std::shared_ptr<Expr>              equality();
    std::shared_ptr<Expr>              comparison();
    std::shared_ptr<Expr>              term();
    std::shared_ptr<Expr>              factor();
    std::shared_ptr<Expr>              unary();
    std::shared_ptr<Expr>              finish_call(std::shared_ptr<Expr> callee);
    std::shared_ptr<Expr>              call();
    std::shared_ptr<Expr>              primary();
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