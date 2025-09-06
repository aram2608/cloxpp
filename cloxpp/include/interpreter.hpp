#pragma once
#include "error.hpp"
#include "expr.hpp"
#include "run_time_error.hpp"
#include "tokens.hpp"

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

    void interpret(Expr& expr);
    any  evaluate(Expr& expr);

    LoxError errors;

  private:
    any visitBinaryExpr(Binary& expr) override;
    any visitUnaryExpr(Unary& expr) override;
    any visitGroupingExpr(Grouping& expr) override;
    any visitLiteralExpr(Literal& expr) override;

    bool   is_truthy(const any& object);
    bool   is_equal(const any& me, const any& you);
    void   check_num_operand(const Token& op, const any& operand);
    void   check_num_operands(const Token& op, const any& op_a, const any& op_b);
    string make_string(const any& object);
};

} // namespace lox