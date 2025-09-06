#include "expr.hpp"
#include "tokens.hpp"

#include <iostream>
#include <memory>
#include <vector>

namespace lox {
using std::initializer_list;
using std::unique_ptr;
using std::vector;

class Parser {
  public:
    Parser(vector<Token> tokens);

  private:
    unique_ptr<Expr> expression();
    unique_ptr<Expr> equality();
    bool             match(initializer_list<TokenType> types);
    bool             check(TokenType type);
    Token            advance();
    const Token&     peek() const;
    Token            previous();
    bool             is_end();
    vector<Token>    tokens;
    int              current;
};
} // namespace lox