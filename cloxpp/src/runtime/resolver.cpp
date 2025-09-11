#include "resolver.hpp"

using namespace CppLox;
using std::any;
using std::shared_ptr;
using std::vector;

Resolver::Resolver(Interpreter& interpreter) : interpreter(interpreter) {
}

// Overload to resolve vectors of statements
void Resolver::resolve(const vector<shared_ptr<Stmt>>& stmts) {
    // We loop over const refs of each statement and resolve them
    for (const shared_ptr<Stmt>& stmt : stmts) {
        resolve(stmt);
    }
}

// Function to resolve block statements
any Resolver::visitBlockStmt(shared_ptr<Block> stmt) {
    // We start the scope
    begin_scope();
    // We then resolve the statements with our helper method
    resolve(stmt->stmts);
    // We end the scope
    end_scope();
    return {};
}

// Function to resolve Variable statements
any Resolver::visitVarStmt(shared_ptr<Var> stmt) {
    // We first declare the name in the statement
    declare(stmt->name);
    // We check if the intializer is a nullptr
    if (stmt->initializer != nullptr) {
        // If not we resolve its initializer
        resolve(stmt->initializer);
    }
    // We then define the name
    define(stmt->name);
    return {};
}

// Function to resolve if statements
any Resolver::visitIfStmt(shared_ptr<IfStmt> stmt) {
    // We first resolve the condition and then_branch
    resolve(stmt->condition);
    resolve(stmt->then_branch);
    // If the else_branch exists we resolve it
    if (stmt->else_branch != nullptr) {
        resolve(stmt->else_branch);
    }
    return {};
}

// Function to resolve expression statements
any Resolver::visitExpressionStmt(shared_ptr<ExpressionStmt> stmt) {
    resolve(stmt->expr);
    return {};
}

// Function to resolve function statements
any Resolver::visitFunctionStmt(shared_ptr<Function> stmt) {
    // We declare and define the function names
    declare(stmt->name);
    define(stmt->name);

    // We then resolve the stmt
    resolve_function(stmt, FunctionType::FUNCTION);
    return {};
}

// Function to resolve print statements
any Resolver::visitPrintStmt(shared_ptr<Print> stmt) {
    // We resolve the internal expression
    resolve(stmt->expr);
    return {};
}

// Function to resolve return statements
any Resolver::visitReturnStmt(shared_ptr<ReturnStmt> stmt) {
    // We need to ensure that the user is not using return outside of a block
    if (current_function == FunctionType::NONE) {
        error.error(stmt->keyword, "Can't return from top-level code.");
    }

    // We test if there is a returned expression and resolve it
    if (stmt->expr != nullptr) {
        resolve(stmt->expr);
    }
    return {};
}

// Function to resolve while statements
any Resolver::visitWhileStmt(shared_ptr<WhileStmt> while_stmt) {
    // We resolve the condtion and body for each while loop
    resolve(while_stmt->condition);
    resolve(while_stmt->body);
    return {};
}

// Function to resolve variable assignment
any Resolver::visitAssignExpr(shared_ptr<Assign> expr) {
    // We first resolve the expression
    resolve(expr->value);
    // We then resolve the name
    resolve_local(expr, expr->name);
    return {};
}

// Function to resolve call expressions
any Resolver::visitCallExpr(shared_ptr<Call> expr) {
    // We first need to resolve the callee
    resolve(expr->callee);

    // We then loop over each argument in the arguments vector and resolve
    // them one by one
    for (const shared_ptr<Expr>& args : expr->args) {
        resolve(args);
    }
    return {};
}

// Function to resolve logical expressions
any Resolver::visitLogicalExpr(shared_ptr<Logical> expr) {
    // We resolve both left and right expressions
    resolve(expr->left);
    resolve(expr->right);
    return {};
}

// Function to resolve binary expression
any Resolver::visitBinaryExpr(shared_ptr<Binary> expr) {
    // We resolve both left and right expressions
    resolve(expr->left);
    resolve(expr->right);
    return {};
}

// Function to resolve unary expressions
any Resolver::visitUnaryExpr(shared_ptr<Unary> expr) {
    // We resolve the single expression
    resolve(expr->right);
    return {};
}

// Function to resolve grouping expression
any Resolver::visitGroupingExpr(shared_ptr<Grouping> expr) {
    // We simply resolve the internal expression
    resolve(expr->expr);
    return {};
}

// Function to resolve literals
any Resolver::visitLiteralExpr(shared_ptr<Literal> expr) {
    return {};
}

// Function to resolve variable expression
any Resolver::visitVariableExpr(shared_ptr<Variable> expr) {
    // We first test if the scopes are empty and return
    if (!scopes.empty()) {
        // Next we look back into the scopes and store the first one
        std::map<std::string, bool> scope = scopes.back();
        // We then use find to search for our lexeme
        auto it = scope.find(expr->name.lexeme);
        // We test if the iterator is inside the scope and if the name is set to false
        if (it != scope.end() && it->second == false)
            // If so we throw an error
            error.error(expr->name, "Can't read local variable in its own initializer.");
    }
    // Otherwise we resolve the local variable
    resolve_local(expr, expr->name);
    return {};
}

// Function to begin store the start of a scope into our vector of maps
void Resolver::begin_scope() {
    scopes.push_back(std::map<std::string, bool>());
}

// Function to delete the last element in a vector
void Resolver::end_scope() {
    scopes.pop_back();
}

// Helper method to resolve statements
void Resolver::resolve(shared_ptr<Stmt> stmt) {
    stmt->accept(*this);
}

// Overload for resolve helper method to resolve expression instead
void Resolver::resolve(shared_ptr<Expr> expr) {
    expr->accept(*this);
}

// Function used to resolve local variables
void Resolver::resolve_local(shared_ptr<Expr> expr, Token name) {
    // We start at the inner most scope and work outwards to find the name
    for (int i = scopes.size() - 1; i >= 0; --i) {
        /*
         * If we match it, we ask the interpreter to resolve it and pass in
         * the number corresponding the scope
         * if its in the current scope we get 0
         * if its the enclosing scope - 1
         * if its the scope outside of the immediate enclosing scope - 2
         * etc...
         */
        if (scopes[i].find(name.lexeme) != scopes[i].end()) {
            interpreter.resolve(expr, scopes.size() - 1 - i);
            return;
        }
    }
}

// Function to statically resolve lox functions
void Resolver::resolve_function(shared_ptr<Function> function, FunctionType type) {
    // We store the current_function into the bodies enclosing function
    FunctionType enclosing_function = current_function;
    // We then save the current type
    current_function = type;
    // We declare the define the name in the current scope
    begin_scope();
    // We then loop over all the function parameters and declare/define them
    for (Token param : function->params) {
        declare(param);
        define(param);
    }
    // We then resolve the function body after declaration
    // this allows a function to refer to itself recursively
    resolve(function->body);
    // We then end the scope
    end_scope();
    // We can then return to the enclosing function
    current_function = enclosing_function;
}

// Function to declare variables in scope
void Resolver::declare(Token name) {
    // we first check if the scopes are empty and return if so
    if (scopes.empty()) {
        return;
    }
    // otherwise, we look at the last element in the map and add a new map
    std::map<std::string, bool> scope = scopes.back();

    // We check to see if a variable has already been declared in the local scope
    auto it = scope.find(name.lexeme);
    if (it != scope.end()) {
        // If so we can kick an error
        error.error(name, "Already a variable with this name in this scope.");
    }

    // we store the name with false
    scope[name.lexeme] = false;
}

// Function to define variables to the scope
void Resolver::define(Token name) {
    // we first check if scopes is empty and return if so
    if (scopes.empty()) {
        return;
    }
    // otherwise we look to the last element and add the token as the key
    // with true as its value
    scopes.back()[name.lexeme] = true;
}
