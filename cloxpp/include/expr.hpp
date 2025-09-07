#pragma once

#include "tokens.hpp"

#include <any>
#include <memory>
#include <utility> // for std::move
#include <vector>

namespace lox {

// Forward declarations
struct Binary;
struct Unary;
struct Grouping;
struct Literal;
struct Variable;

// Abstract visitor for different nodes
struct ExprVisitor {
    virtual ~ExprVisitor() = default;

    /*
     * We pass in a reference to the expression
     * return type is std::any since it is type safe and allows for storage
     * of unknown types similarly to void *
     */
    virtual std::any visitBinaryExpr(Binary& expr)     = 0;
    virtual std::any visitUnaryExpr(Unary& expr)       = 0;
    virtual std::any visitGroupingExpr(Grouping& expr) = 0;
    virtual std::any visitLiteralExpr(Literal& expr)   = 0;
    virtual std::any visitVariableExpr(Variable& var)  = 0;
};

// Abstract base class, requires at least one virtual method
struct Expr {
    /*
        * We declare a virtual Destructor
        * This ensures that the derived classes Destructor is called

        * Expr* e = new Binary(...);
        * delete e; <- should call Binary::~Binary()
    */
    virtual ~Expr() = default;

    // We need a default constructor to get the compiler to stop complaining
    Expr() = default;

    /*
     * We delete the copy constructor
     * This should prevent making copies of Expr or its derived classes
     * Important because many nodes hold std::unique_ptr<Expr> children,
     * which we do not want to copy
     */
    Expr(const Expr&) = delete;
    // We also delete the copy assignment operator for the same reason
    Expr& operator=(const Expr&) = delete;

    // accept() method for visiting nodes, we pass in a reference to ExprVisitor&
    virtual std::any accept(ExprVisitor& visitor) = 0;
};

// Binary node, inheritting from Expr
// allows us to use Binary anywhere Expr* and Expr& are
struct Binary : Expr {
    /*
     * Constructor for Binary class
     * left and right are passed as unique_ptr from Expr as the temporary owner
     * In our initialization list, we move ownership of left and right
     * to the Binary class and copy construct op to op
     */
    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {
    }

    /*
     * We override the virtual method from the ExprVisitor
     * and pass in a dereferenced pointer to the node object
     * it should return a Binary&
     */
    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitBinaryExpr(*this);
    }

    // Left expression
    std::unique_ptr<Expr> left;
    // Operator token
    Token op;
    // Right expression
    std::unique_ptr<Expr> right;
};

// Grouping node, inheritting from Expr
// allows us to use Binary anywhere Expr* and Expr& are
struct Grouping : Expr {
    /*
     * Constructor for the Grouping expression, we pass in an expression
     * and in our list initialization we move ownership of the expression
     * to our member
     */
    Grouping(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {
    }

    /*
     * We override the virtual method from the ExprVisitor
     * and pass in a dereferenced pointer to the node object,
     * it should return a Grouping&
     */
    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitGroupingExpr(*this);
    }

    // Member expression
    std::unique_ptr<Expr> expr;
};

// Literal node, inheritting from Expr
// allows us to use Binary anywhere Expr* and Expr& are
struct Literal : Expr {
    /*
     * Constructor for our Literal node, it takes in any value and in
     * the list initalization, we move ownership to the member value
     */
    Literal(std::any value) : value(std::move(value)) {
    }

    /*
     * We override the virtual method from the ExprVisitor
     * and pass in a dereferenced pointer to the node object
     * it should return a Literal&
     */
    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitLiteralExpr(*this);
    }

    // Member value
    std::any value;
};

// Unary node, inheritting from Expr
// allows us to use Binary anywhere Expr* and Expr& are
struct Unary : Expr {
    /*
     * Unary op constructor, we pass in a Token and a unique_ptr to the right expression
     * In our list initialization, we move ownership of the token and right expression
     * to the members
     */
    Unary(Token op, std::unique_ptr<Expr> right) : op(std::move(op)), right(std::move(right)) {
    }

    /*
     * We override the virtual method from the ExprVisitor
     * and pass in a dereferenced pointer to the node object
     * it should return a Unary&
     */
    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitUnaryExpr(*this);
    }

    // Operator token
    Token op;
    // Right expression
    std::unique_ptr<Expr> right;
};

// Variable node
struct Variable : Expr {
    /*
     * Constructor for the variable now, we pass in an Identifier token
     */
    Variable(Token identifier) : identifier(std::move(identifier)) {
    }

    // Override for the Expr accept method
    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitVariableExpr(*this);
    }

    // Token for the identifier
    Token identifier;
};

} // namespace lox