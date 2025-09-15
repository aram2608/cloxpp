#ifndef STMT_HPP
#define STMT_HPP

#include "ast/expr.hpp"
#include "utils/tokens.hpp"

#include <any>
#include <memory>
#include <utility>
#include <vector>

namespace CppLox {

struct Block;
struct Function;
struct Class;
struct ExpressionStmt;
struct IfStmt;
struct WhileStmt;
struct Print;
struct Var;
struct ReturnStmt;

// Statement visitor interface
struct StmtVisitor {
    virtual ~StmtVisitor()                                                     = default;
    virtual std::any visitBlockStmt(std::shared_ptr<Block> stmt)               = 0;
    virtual std::any visitReturnStmt(std::shared_ptr<ReturnStmt> stmt)         = 0;
    virtual std::any visitFunctionStmt(std::shared_ptr<Function> stmt)         = 0;
    virtual std::any visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) = 0;
    virtual std::any visitPrintStmt(std::shared_ptr<Print> stmt)               = 0;
    virtual std::any visitVarStmt(std::shared_ptr<Var> stmt)                   = 0;
    virtual std::any visitIfStmt(std::shared_ptr<IfStmt> if_stmt)              = 0;
    virtual std::any visitWhileStmt(std::shared_ptr<WhileStmt> while_stmt)     = 0;
    virtual std::any visitClassStmt(std::shared_ptr<Class> stmt)               = 0;
};

// Statement interface
struct Stmt {
    virtual ~Stmt() = default;
    Stmt()          = default;

    // accept() method for visiting nodes, we pass in a reference to
    // ExprVisitor&
    virtual std::any accept(StmtVisitor& visitor) = 0;
};

// Return statement node
struct ReturnStmt : Stmt, std::enable_shared_from_this<ReturnStmt> {
    // Constructor for the return statement, we pass in a pointer the optional
    // return value
    ReturnStmt(Token keyword, std::shared_ptr<Expr> expr)
        : expr(std::move(expr)), keyword(keyword) {
    }

    // We override the accept method
    std::any accept(StmtVisitor& visitor) {
        return visitor.visitReturnStmt(shared_from_this());
    }

    // a shared_ptr the the optional expression
    std::shared_ptr<Expr> expr;
    // return keyword, useful for error reporting
    Token keyword;
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

struct Class : Stmt, std::enable_shared_from_this<Class> {
    /*
     * Constructor for the Class class, we pass in the identifier for the class
     * and a vector for functions corresponding to the class methods
     * we move ownership of the vector
     */
    Class(Token                                  name,
          std::shared_ptr<Variable>              superclass,
          std::vector<std::shared_ptr<Function>> methods)
        : name(name), superclass(std::move(superclass)), methods(std::move(methods)) {
    }

    // We override the accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitClassStmt(shared_from_this());
    }

    // Identifier token
    Token name;
    // Pointer to superclass
    std::shared_ptr<Variable> superclass;
    // Vector of methods
    std::vector<std::shared_ptr<Function>> methods;
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
     * Constructor for Var statement, we pass in an name token
     * and initializing expression
     */
    Var(Token name, std::shared_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {
    }

    // Override Stmt accept method
    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitVarStmt(shared_from_this());
    }

    // name token
    Token                 name;
    std::shared_ptr<Expr> initializer;
};
} // namespace CppLox

#endif