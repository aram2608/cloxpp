#pragma once

#include "expr.hpp"
#include "tokens.hpp"

#include <any>
#include <memory>
#include <utility> // std::move
#include <vector>

namespace lox {

struct Block;
struct Expression;
struct Print;
struct Var;

// Statement visitor interface
struct StmtVisitor {
    virtual ~StmtVisitor()                                 = default;
    virtual std::any visitBlockStmt(Block& stmt)           = 0;
    virtual std::any visitExpressionStmt(Expression& stmt) = 0;
    virtual std::any visitPrintStmt(Print& stmt)           = 0;
    virtual std::any visitVarStmt(Var& stmt)               = 0;
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

    std::vector<std::unique_ptr<Stmt>> stmts;
};

struct Expression : Stmt {
    /*
     * Constructor for expression statements, we pass in an expression and move ownership
     */
    Expression(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitExpressionStmt(*this);
    }

    std::unique_ptr<Expr> expr;
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
        : identifier{std::move(identifier)}, initializer(std::move(initializer)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitVarStmt(*this);
    }

    // Identifier token
    Token                 identifier;
    std::unique_ptr<Expr> initializer;
};
} // namespace lox