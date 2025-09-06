#pragma once
#include "expr.hpp"

#include <any>
#include <memory>
#include <stdexcept>
#include <vector>

namespace lox {
// We scope our aliases to the namespace
using std::any;
using std::string;
// We inherit the ExprVisitor class so now we need to override each visit method
class Interpreter : ExprVisitor {
  public:
    Interpreter();

    any evaluate(Expr& expr);

  private:
    any visitBinaryExpr(Binary& expr) override;
    any visitUnaryExpr(Unary& expr) override;
    any visitGroupingExpr(Grouping& expr) override;
    any visitLiteralExpr(Literal& expr) override;

    bool   is_truthy(const any& object);
    bool   is_equal(const any& me, const any& you);
    string make_string(const any& object);
};

} // namespace lox