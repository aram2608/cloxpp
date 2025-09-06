#include "error.hpp"
#include "expr.hpp"
#include "tokens.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace lox {
using std::initializer_list;
using std::unique_ptr;
using std::vector;

class Parser {
    vector<Token> tokens;
    int           current = 0;
    struct ParseError : public std::runtime_error {
        // We inherit all the constructors from std::runtime_error
        using std::runtime_error::runtime_error;
    };

  public:
    Parser(vector<Token> tokens);
    std::unique_ptr<Expr> parse();
    LoxError              errors;

  private:
    unique_ptr<Expr> expression();
    unique_ptr<Expr> equality();
    unique_ptr<Expr> comparison();
    unique_ptr<Expr> term();
    unique_ptr<Expr> factor();
    unique_ptr<Expr> unary();
    unique_ptr<Expr> primary();
    bool             match(initializer_list<TokenType> types);
    Token            consume(TokenType type, std::string message);
    bool             check(TokenType type);
    Token            advance();
    Token            peek();
    Token            previous();
    bool             is_end();
    ParseError       error(Token token, std::string_view message);
    void             synchronize();
};
} // namespace lox