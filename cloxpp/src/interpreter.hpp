#pragma once
#include "environment.hpp"
#include "error.hpp"
#include "expr.hpp"
#include "run_time_error.hpp"
#include "stmt.hpp"
#include "tokens.hpp"

#include <any>
#include <memory>
#include <stdexcept>
#include <vector>

namespace CppLox {
// We inherit the ExprVisitor class so now we need to override each visit method
class Interpreter : ExprVisitor, StmtVisitor {
  public:
    Interpreter();

    void     interpret(const std::vector<std::unique_ptr<Stmt>>& stmts);
    void     execute(Stmt& stmt);
    void     execute_block(const std::vector<std::unique_ptr<Stmt>>& stmts,
                           std::shared_ptr<Environment>              env);
    std::any evaluate(Expr& expr);

    LoxError errors;
    bool     repl{false};

  private:
    std::shared_ptr<Environment> environment = std::make_shared<Environment>();
    std::any                     visitBlockStmt(Block& stmt) override;
    std::any                     visitExpressionStmt(ExpressionStmt& stmt) override;
    std::any                     visitPrintStmt(Print& stmt) override;
    std::any                     visitIfStmt(IfStmt& stmt) override;
    std::any                     visitWhileStmt(WhileStmt& stmt) override;
    std::any                     visitVarStmt(Var& stmt) override;
    std::any                     visitLogicalExpr(Logical& expr) override;
    std::any                     visitAssignExpr(Assign& expr) override;
    std::any                     visitBinaryExpr(Binary& expr) override;
    std::any                     visitUnaryExpr(Unary& expr) override;
    std::any                     visitGroupingExpr(Grouping& expr) override;
    std::any                     visitLiteralExpr(Literal& expr) override;
    std::any                     visitVariableExpr(Variable& var) override;

    bool        is_truthy(const std::any& object);
    bool        is_equal(const std::any& me, const std::any& you);
    void        check_num_operand(const Token& op, const std::any& operand);
    void        check_num_operands(const Token& op, const std::any& op_a, const std::any& op_b);
    std::string make_string(const std::any& object);
};

} // namespace CppLox