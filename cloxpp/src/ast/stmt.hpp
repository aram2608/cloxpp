#pragma once

#include "expr.hpp"
#include "tokens.hpp"

#include <any>
#include <memory>
#include <utility>
#include <vector>

namespace CppLox {

struct Block;
struct Function;
struct ExpressionStmt;
struct IfStmt;
struct WhileStmt;
struct Print;
struct Var;

// Statement visitor interface
struct StmtVisitor {
    virtual ~StmtVisitor()                                                     = default;
    virtual std::any visitBlockStmt(std::shared_ptr<Block> stmt)               = 0;
    virtual std::any visitFunctionStmt(std::shared_ptr<Function> stmt)         = 0;
    virtual std::any visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) = 0;
    virtual std::any visitPrintStmt(std::shared_ptr<Print> stmt)               = 0;
    virtual std::any visitVarStmt(std::shared_ptr<Var> stmt)                   = 0;
    virtual std::any visitIfStmt(std::shared_ptr<IfStmt> if_stmt)              = 0;
    virtual std::any visitWhileStmt(std::shared_ptr<WhileStmt> while_stmt)     = 0;
};

// Statement interface
struct Stmt {
    virtual ~Stmt() = default;
    Stmt()          = default;

    // accept() method for visiting nodes, we pass in a reference to
    // ExprVisitor&
    virtual std::any accept(StmtVisitor& visitor) = 0;
};

struct Block : Stmt, std::enable_shared_from_this<Block> {
    /*
     * Constructor for Block class, we pass in a vector of statements and move
     * ownership of statements
     */
    Block(std::vector<std::shared_ptr<Stmt>> stmts) : stmts(std::move(stmts)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitBlockStmt(shared_from_this());
    }

    // A vector of pointers to the statements inside the block
    std::vector<std::shared_ptr<Stmt>> stmts;
};

struct Function : Stmt, std::enable_shared_from_this<Function> {
    /*
     * FunctionDeclaration constructor (Function) for readability. We pass in
     * the name of the function as a token, a vector of its parameters as
     * Tokens, and a vector of statements for the body of the function
     */
    Function(Token name, std::vector<Token> params, std::vector<std::shared_ptr<Stmt>> body)
        : name(name), params(params), body(std::move(body)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitFunctionStmt(shared_from_this());
    }

    Token                              name;
    std::vector<Token>                 params;
    std::vector<std::shared_ptr<Stmt>> body;
};

struct ExpressionStmt : Stmt, std::enable_shared_from_this<ExpressionStmt> {
    /*
     * Constructor for expression statements, we pass in an expression and move
     * ownership
     */
    ExpressionStmt(std::shared_ptr<Expr> expr) : expr(std::move(expr)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitExpressionStmt(shared_from_this());
    }

    std::shared_ptr<Expr> expr;
};

struct IfStmt : Stmt, std::enable_shared_from_this<IfStmt> {
    /*
     * If statement constructor, we pass in a pointer to the condtion
     * we also pass a pointer the then clause statement
     * and finally we pass in an else clause statement
     * we need to move ownership
     */
    IfStmt(std::shared_ptr<Expr> condition,
           std::shared_ptr<Stmt> then_branch,
           std::shared_ptr<Stmt> else_branch)
        : condition(std::move(condition)), then_branch(std::move(then_branch)),
          else_branch(std::move(else_branch)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitIfStmt(shared_from_this());
    }

    // Pointer to if expression
    std::shared_ptr<Expr> condition;
    // pointer to then clause
    std::shared_ptr<Stmt> then_branch;
    // pointer to else clause
    std::shared_ptr<Stmt> else_branch;
};

struct WhileStmt : Stmt, std::enable_shared_from_this<WhileStmt> {
    /*
     * WhileStmt constructor, we pass in a pointer to the condition expression
     * and for the body statement
     */
    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitWhileStmt(shared_from_this());
    }

    // shared_ptr to while expression
    std::shared_ptr<Expr> condition;
    // shared_ptr to statements needed to run
    std::shared_ptr<Stmt> body;
};

struct Print : Stmt, std::enable_shared_from_this<Print> {
    /*
     * Constructor for Print statement, we pass in an expression and move
     * ownership
     */
    Print(std::shared_ptr<Expr> expr) : expr(std::move(expr)) {
    }

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitPrintStmt(shared_from_this());
    }

    std::shared_ptr<Expr> expr;
};

struct Var : Stmt, std::enable_shared_from_this<Var> {
    /*
     * Constructor for Var statement, we pass in an identifier token
     * and initializing expression
     */
    Var(Token identifier, std::shared_ptr<Expr> initializer)
        : identifier(std::move(identifier)), initializer(std::move(initializer)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitVarStmt(shared_from_this());
    }

    // Identifier token
    Token                 identifier;
    std::shared_ptr<Expr> initializer;
};
} // namespace CppLox