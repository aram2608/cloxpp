#pragma once
#include "error.hpp"
#include "expr.hpp"
#include "run_time_error.hpp"
#include "stmt.hpp"
#include "tokens.hpp"

#include <any>
#include <memory>
#include <stdexcept>
#include <vector>

namespace lox {
// We inherit the ExprVisitor class so now we need to override each visit method
class Interpreter : ExprVisitor, StmtVisitor {
  public:
    Interpreter();

    void     interpret(std::vector<std::unique_ptr<Stmt>> stmts);
    void     execute(Stmt& stmt);
    std::any evaluate(Expr& expr);

    LoxError errors;

  private:
    std::any visitBinaryExpr(Binary& expr) override;
    std::any visitUnaryExpr(Unary& expr) override;
    std::any visitGroupingExpr(Grouping& expr) override;
    std::any visitLiteralExpr(Literal& expr) override;
    std::any visitBlockStmt(Block& stmt) override;
    std::any visitExpressionStmt(Expression& stmt) override;
    std::any visitPrintStmt(Print& stmt) override;
    std::any visitVarStmt(Var& stmt) override;
    std::any visitVariableExpr(Variable& var) override;

    bool        is_truthy(const std::any& object);
    bool        is_equal(const std::any& me, const std::any& you);
    void        check_num_operand(const Token& op, const std::any& operand);
    void        check_num_operands(const Token& op, const std::any& op_a, const std::any& op_b);
    std::string make_string(const std::any& object);
};

} // namespace lox