#pragma once
#include "callable.hpp"
#include "environment.hpp"
#include "error.hpp"
#include "expr.hpp"
#include "lox_functions.hpp"
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
  // We make LoxFunction a friend class so we can throw it the return value
  friend class LoxFunction;
    struct Return {
        Return(std::any value) : value(value) {
        }
        std::any value;
    };

  public:
    std::shared_ptr<Environment> globals = std::make_shared<Environment>();

  private:
    std::shared_ptr<Environment> environment = globals;

  public:
    Interpreter();

    void     interpret(const std::vector<std::shared_ptr<Stmt>>& stmts);
    void     execute(std::shared_ptr<Stmt> stmt);
    void     execute_block(const std::vector<std::shared_ptr<Stmt>>& stmts,
                           std::shared_ptr<Environment>              env);
    std::any evaluate(std::shared_ptr<Expr> expr);

    LoxError errors;
    bool     repl{false};

  private:
    std::any visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) override;
    std::any visitBlockStmt(std::shared_ptr<Block> stmt) override;
    std::any visitFunctionStmt(std::shared_ptr<Function> stmt) override;
    std::any visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) override;
    std::any visitPrintStmt(std::shared_ptr<Print> stmt) override;
    std::any visitVarStmt(std::shared_ptr<Var> stmt) override;
    std::any visitIfStmt(std::shared_ptr<IfStmt> if_stmt) override;
    std::any visitWhileStmt(std::shared_ptr<WhileStmt> while_stmt) override;
    std::any visitCallExpr(std::shared_ptr<Call> expr) override;
    std::any visitLogicalExpr(std::shared_ptr<Logical> expr) override;
    std::any visitAssignExpr(std::shared_ptr<Assign> expr) override;
    std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override;
    std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override;
    std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override;
    std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override;
    std::any visitVariableExpr(std::shared_ptr<Variable> expr) override;

    bool        is_truthy(const std::any& object);
    bool        is_equal(const std::any& me, const std::any& you);
    void        check_num_operand(const Token& op, const std::any& operand);
    void        check_num_operands(const Token& op, const std::any& op_a, const std::any& op_b);
    std::string make_string(const std::any& object);
};

} // namespace CppLox