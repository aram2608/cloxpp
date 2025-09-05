#pragma once

#include "tokens.hpp"

#include <any>
#include <memory>
#include <utility> // for std::move
#include <vector>

namespace lox {
// We scope our aliases
// We dont alias std::move() to make sure it remains qualified
// It should work in scope, but just to be safe
using std::any;
using std::unique_ptr;

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
    virtual any visitBinaryExpr(Binary& expr)     = 0;
    virtual any visitUnaryExpr(Unary& expr)       = 0;
    virtual any visitGroupingExpr(Grouping& expr) = 0;
    virtual any visitLiteralExpr(Literal& expr)   = 0;
};

// Abstract base class, requires at least one virtual method
struct Expr {
    /*
        We declare a virtual deconstructor
        This ensures that the derived classes deconstructor is called

        Expr* e = new Binary(...);
        delete e; <- should call Binary::~Binary() I think
    */
    virtual ~Expr() = default;

    // We need a default constructor to get the compiler to stop complaining
    Expr() = default;

    /*
        We delete the copy constructor
        This should prevent making copies of Expr or its derived classes
        Important because many nodes hold std::unique_ptr<Expr> children,
        which we do not want to copy
    */
    Expr(const Expr&) = delete;
    // We also delete the copy assignment operator
    Expr& operator=(const Expr&) = delete;

    // Now we can request a move constructor
    Expr(Expr&&) = default;
    // And a move assignment operator
    Expr& operator=(Expr&&) = default;

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
        to the Binary class and copy construct op to operator_
    */
    Binary(unique_ptr<Expr> left, Token op, unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {
    }

    // We override the virtual method from the ExprVisitor
    // and pass in a dereferenced pointer the node object
    // it should return a Binary&
    any accept(ExprVisitor& visitor) override {
        return visitor.visitBinaryExpr(*this);
    }

    // Left expression
    unique_ptr<Expr> left;
    // Operator token
    Token op;
    // Right expression
    unique_ptr<Expr> right;
};

// Grouping node, inheritting from Expr
// allows us to use Binary anywhere Expr* and Expr& are
struct Grouping : Expr {
    /*
        Constructor for the Grouping expression, we pass in an expression
        and in our list initialization we move ownership of the expression
        to our member
    */
    Grouping(unique_ptr<Expr> expr) : expr(std::move(expr)) {
    }

    // We override the virtual method from the ExprVisitor
    // and pass in a dereferenced pointer the node object
    // it should return a Grouping&
    any accept(ExprVisitor& visitor) override {
        return visitor.visitGroupingExpr(*this);
    }

    // Member expression
    unique_ptr<Expr> expr;
};

// Literal node, inheritting from Expr
// allows us to use Binary anywhere Expr* and Expr& are
struct Literal : Expr {
    /*
        Constructor for our Literal node, it takes in any value and in
        the list initalization, we move ownership to the member value
    */
    Literal(any value) : value(std::move(value)) {
    }

    // We override the virtual method from the ExprVisitor
    // and pass in a dereferenced pointer the node object
    // it should return a Literal&
    any accept(ExprVisitor& visitor) override {
        return visitor.visitLiteralExpr(*this);
    }

    // Member value
    any value;
};

// Unary node, inheritting from Expr
// allows us to use Binary anywhere Expr* and Expr& are
struct Unary : Expr {
    /*
        Unary op constructor, we pass in a Token and a unique_ptr to the right expression
        In our list initialization, we move ownership of the token and right expression
        to the members
    */
    Unary(Token op, unique_ptr<Expr> right) : op(std::move(op)), right(std::move(right)) {
    }

    // We override the virtual method from the ExprVisitor
    // and pass in a dereferenced pointer the node object
    // it should return a Unary&
    any accept(ExprVisitor& visitor) override {
        return visitor.visitUnaryExpr(*this);
    }

    // Operator token
    Token op;
    // Right expression
    unique_ptr<Expr> right;
};

} // namespace lox