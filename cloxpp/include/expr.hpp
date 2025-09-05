#pragma once

#include "tokens.hpp"

#include <any>
#include <memory>
#include <utility>
#include <vector>

using std::any;
using std::move;
using std::unique_ptr;

namespace lox {

// Forward declarations
struct Binary;
struct Unary;
struct Grouping;
struct Literal;

// Visitor for different nodes
struct ExprVisitor {
    virtual ~ExprVisitor() = default;

    /*
        We pass in a reference to the expression
        return type is std::any since it is type safe and allows for storage
        of unknown types similarly to void *
    */
    virtual any visitBinary(Binary& expr)     = 0;
    virtual any visitUnary(Unary& expr)       = 0;
    virtual any visitGrouping(Grouping& expr) = 0;
    virtual any visitLiteral(Literal& expr)   = 0;
};

// Abstract base class, requires at least one virtual method
struct Expr {
    virtual ~Expr() = default;
    // accept() method for visiting nodes, we pass in a reference to ExprVisitor&
    virtual any accept(ExprVisitor& visitor) = 0;
};

// Binary node, inheritting from Expr
// allows us to use Binary anywhere Expr* and Expr& are
struct Binary : Expr {
    /*
        Constructor for Binary class
        left and right are passed as unique_ptr from Expr as the temporary owner
        In our initialization list, we move ownership of left and right
        to the Binary class and copy construct op to operator_, we pass in a
        const-reference since its cheaper than copying the entire token twice
    */
    Binary(unique_ptr<Expr> left, const Token& op, unique_ptr<Expr> right)
        : left(move(left)), operator_(op), right(move(right)) {
    }

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitBinaryExpr(shared_from_this());
    }

    // Left expression
    unique_ptr<Expr> left;
    // Operator token
    Token operator_;
    // Right expression
    unique_ptr<Expr> right;
};
} // namespace lox