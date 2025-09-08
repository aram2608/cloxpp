#pragma once

#include "expr.hpp"
#include "tokens.hpp"

#include <any>
#include <memory>
#include <utility> // std::move
#include <vector>

namespace CppLox {

struct Block;
struct ExpressionStmt;
struct IfStmt;
struct WhileStmt;
// struct ForStmt;
struct Print;
struct Var;

// Statement visitor interface
struct StmtVisitor {
    virtual ~StmtVisitor()                                     = default;
    virtual std::any visitBlockStmt(Block& stmt)               = 0;
    virtual std::any visitExpressionStmt(ExpressionStmt& stmt) = 0;
    virtual std::any visitPrintStmt(Print& stmt)               = 0;
    virtual std::any visitVarStmt(Var& stmt)                   = 0;
    virtual std::any visitIfStmt(IfStmt& if_stmt)              = 0;
    virtual std::any visitWhileStmt(WhileStmt& if_stmt)        = 0;
    // virtual std::any visitForStmt(ForStmt& for_stmt)       = 0;
};

// Statement interface
struct Stmt {
    virtual ~Stmt() = default;
    Stmt()          = default;
    // Delete copy operator
    Stmt(const Stmt&) = delete;
    // We also delete the copy assignment operator for the same reason
    Stmt& operator=(const Stmt&) = delete;

    // accept() method for visiting nodes, we pass in a reference to ExprVisitor&
    virtual std::any accept(StmtVisitor& visitor) = 0;
};

struct Block : Stmt {
    /*
     * Constructor for Block class, we pass in a vector of statements and move ownership
     * of statements
     */
    Block(std::vector<std::unique_ptr<Stmt>> stmts) : stmts(std::move(stmts)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitBlockStmt(*this);
    }

    // A vector of pointers to the statements inside the block
    std::vector<std::unique_ptr<Stmt>> stmts;
};

struct ExpressionStmt : Stmt {
    /*
     * Constructor for expression statements, we pass in an expression and move ownership
     */
    ExpressionStmt(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitExpressionStmt(*this);
    }

    std::unique_ptr<Expr> expr;
};

struct IfStmt : Stmt {

    IfStmt(std::unique_ptr<Expr> condition,
           std::unique_ptr<Stmt> then_branch,
           std::unique_ptr<Stmt> else_branch)
        : condition(std::move(condition)), then_branch(std::move(then_branch)),
          else_branch(std::move(else_branch)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitIfStmt(*this);
    }

    // Pointer to if expression
    std::unique_ptr<Expr> condition;
    // pointer to then clause
    std::unique_ptr<Stmt> then_branch;
    // pointer to else clause
    std::unique_ptr<Stmt> else_branch;
};

struct WhileStmt : Stmt {
    /*
     * WhileStmt constructor
     */
    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitWhileStmt(*this);
    }

    // Unique_ptr to while expression
    std::unique_ptr<Expr> condition;
    // unique_ptr to statements needed to run
    std::unique_ptr<Stmt> body;
};

struct Print : Stmt {
    /*
     * Constructor for Print statement, we pass in an expression and move ownership
     */
    Print(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {
    }

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitPrintStmt(*this);
    }

    std::unique_ptr<Expr> expr;
};

struct Var : Stmt {
    /*
     * Constructor for Var statement, we pass in an identifier token
     * and initializing expression
     */
    Var(Token identifier, std::unique_ptr<Expr> initializer)
        : identifier(std::move(identifier)), initializer(std::move(initializer)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitVarStmt(*this);
    }

    // Identifier token
    Token                 identifier;
    std::unique_ptr<Expr> initializer;
};
} // namespace CppLox