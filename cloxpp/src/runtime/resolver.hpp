#ifndef RESOLVER_HPP
#define RESOLVER_HPP

#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "core/interpreter.hpp"
#include "utils/error.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace CppLox {

enum class FunctionType { NONE, FUNCTION, METHOD, INIT };

enum class ClassType { NONE, CLASS, SUBCLASS };

class Resolver : ExprVisitor, StmtVisitor {
    // We need to store a reference to our interpreter to walk the nodes produced
    Interpreter& interpreter;
    // We create a vector of map objects to store our scopes
    std::vector<std::map<std::string, bool>> scopes;
    FunctionType                             current_function = FunctionType::NONE;
    ClassType                                current_class    = ClassType::NONE;

  public:
    Resolver(Interpreter& interpreter);
    // Function to resolve lists of statements
    void     resolve(const std::vector<std::shared_ptr<Stmt>>& stmts);
    std::any visitBlockStmt(std::shared_ptr<Block> stmt) override;
    std::any visitVarStmt(std::shared_ptr<Var> stmt) override;
    std::any visitIfStmt(std::shared_ptr<IfStmt> stmt) override;
    std::any visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) override;
    std::any visitFunctionStmt(std::shared_ptr<Function> stmt) override;
    std::any visitPrintStmt(std::shared_ptr<Print> stmt) override;
    std::any visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) override;
    std::any visitWhileStmt(std::shared_ptr<WhileStmt> while_stmt) override;
    std::any visitClassStmt(std::shared_ptr<Class> stmt) override;

    std::any visitSuperExpr(std::shared_ptr<Super> expr) override;
    std::any visitThisExpr(std::shared_ptr<This> expr) override;
    std::any visitSetExpr(std::shared_ptr<Set> expr) override;
    std::any visitGetExpr(std::shared_ptr<Get> expr) override;
    std::any visitCallExpr(std::shared_ptr<Call> expr) override;
    std::any visitLogicalExpr(std::shared_ptr<Logical> expr) override;
    std::any visitAssignExpr(std::shared_ptr<Assign> expr) override;
    std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override;
    std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override;
    std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override;
    std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override;
    std::any visitVariableExpr(std::shared_ptr<Variable> expr) override;

  private:
    void begin_scope();
    void end_scope();
    // Overload to resolve individual statements
    void resolve(std::shared_ptr<Stmt> stmt);
    // Overload to resolve expression
    void resolve(std::shared_ptr<Expr> expr);
    // Helper method to resolve local variables
    void resolve_local(const std::shared_ptr<Expr>& expr, Token name);
    // Helper method to resolve functions, their paremeters, and body statements
    void resolve_function(std::shared_ptr<Function> function, FunctionType type);

    // Helper methods to declare and define identifiers into environments
    void declare(Token name);
    void define(Token name);
};

} // namespace CppLox

#endif