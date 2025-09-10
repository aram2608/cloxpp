#pragma once

#include "tokens.hpp"

#include <any>
#include <memory>
#include <utility> // for std::move
#include <vector>

namespace CppLox {

// Forward declarations
struct Call;
struct Logical;
struct Assign;
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
    virtual std::any visitCallExpr(std::shared_ptr<Call> expr)         = 0;
    virtual std::any visitLogicalExpr(std::shared_ptr<Logical> expr)   = 0;
    virtual std::any visitAssignExpr(std::shared_ptr<Assign> expr)     = 0;
    virtual std::any visitBinaryExpr(std::shared_ptr<Binary> expr)     = 0;
    virtual std::any visitUnaryExpr(std::shared_ptr<Unary> expr)       = 0;
    virtual std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) = 0;
    virtual std::any visitLiteralExpr(std::shared_ptr<Literal> expr)   = 0;
    virtual std::any visitVariableExpr(std::shared_ptr<Variable> expr) = 0;
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

    // accept() method for visiting nodes, we pass in a reference to
    // ExprVisitor&
    virtual std::any accept(ExprVisitor& visitor) = 0;
};

struct Call : Expr, std::enable_shared_from_this<Call> {
    Call(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> args)
        : callee(std::move(callee)), paren(paren), args(std::move(args)) {
    }

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitCallExpr(shared_from_this());
    }

    std::shared_ptr<Expr>              callee;
    Token                              paren;
    std::vector<std::shared_ptr<Expr>> args;
};

struct Logical : Expr, std::enable_shared_from_this<Logical> {
    /*
     * Logical node constructor
     * We pass in the left expression and "and" or "or" token and then the right
     * expression we wish to compare
     */
    Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {
    }

    // Override the accept method from expr
    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitLogicalExpr(shared_from_this());
    }

    // pointer to leftmost expression
    std::shared_ptr<Expr> left;
    // logical operator tokens and, or
    Token op;
    // pointer to rightmost expression
    std::shared_ptr<Expr> right;
};

// Assignment node,
struct Assign : Expr, std::enable_shared_from_this<Assign> {
    /*
     * Assignment node constructor
     *  We pass in an identifer token and the value of an expression as its
     * bound variable
     */
    Assign(Token identifier, std::shared_ptr<Expr> value)
        : identifier(std::move(identifier)), value(std::move(value)) {
    }

    // Override the accept method from expr
    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitAssignExpr(shared_from_this());
    }

    // Token name
    Token identifier;
    // Pointer to value
    std::shared_ptr<Expr> value;
};

// Binary node, inheritting from Expr
// allows us to use Binary anywhere Expr* and Expr& are
struct Binary : Expr, std::enable_shared_from_this<Binary> {
    /*
     * Constructor for Binary class
     * left and right are passed as shared_ptr from Expr as the temporary owner
     * In our initialization list, we move ownership of left and right
     * to the Binary class and copy construct op to op
     */
    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {
    }

    /*
     * We override the virtual method from the ExprVisitor
     * and pass in a dereferenced pointer to the node object
     * it should return a Binary&
     */
    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitBinaryExpr(shared_from_this());
    }

    // Left expression
    std::shared_ptr<Expr> left;
    // Operator token
    Token op;
    // Right expression
    std::shared_ptr<Expr> right;
};

// Grouping node, inheritting from Expr
// allows us to use Binary anywhere Expr* and Expr& are
struct Grouping : Expr, std::enable_shared_from_this<Grouping> {
    /*
     * Constructor for the Grouping expression, we pass in an expression
     * and in our list initialization we move ownership of the expression
     * to our member
     */
    Grouping(std::shared_ptr<Expr> expr) : expr(std::move(expr)) {
    }

    /*
     * We override the virtual method from the ExprVisitor
     * and pass in a dereferenced pointer to the node object,
     * it should return a Grouping&
     */
    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitGroupingExpr(shared_from_this());
    }

    // Member expression
    std::shared_ptr<Expr> expr;
};

// Literal node, inheritting from Expr
// allows us to use Binary anywhere Expr* and Expr& are
struct Literal : Expr, std::enable_shared_from_this<Literal> {
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
        return visitor.visitLiteralExpr(shared_from_this());
    }

    // Member value
    std::any value;
};

// Unary node, inheritting from Expr
// allows us to use Binary anywhere Expr* and Expr& are
// we need to enable sharing from this Node type
// lets use use shared_from_this()
struct Unary : Expr, std::enable_shared_from_this<Unary> {
    /*
     * Unary op constructor, we pass in a Token and a shared_ptr to the right
     * expression In our list initialization, we move ownership of the token and
     * right expression to the members
     */
    Unary(Token op, std::shared_ptr<Expr> right) : op(std::move(op)), right(std::move(right)) {
    }

    /*
     * We override the virtual method from the ExprVisitor
     * and pass in a dereferenced pointer to the node object
     * it should return a Unary&
     */
    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitUnaryExpr(shared_from_this());
    }

    // Operator token
    Token op;
    // Right expression
    std::shared_ptr<Expr> right;
};

// Variable node
struct Variable : Expr, std::enable_shared_from_this<Variable> {
    /*
     * Constructor for the variable now, we pass in an Identifier token
     */
    Variable(Token identifier) : identifier(std::move(identifier)) {
    }

    // Override for the Expr accept method
    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitVariableExpr(shared_from_this());
    }

    // Token for the identifier
    Token identifier;
};

} // namespace CppLox