#include "runtime/resolver.hpp"

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
        LoxError::error(stmt->keyword, "Can't return from top-level code.");
    }

    // We test if there is a returned expression and resolve it
    if (stmt->expr != nullptr) {
        resolve(stmt->expr);
        if (current_function == FunctionType::INIT) {
            LoxError::error(stmt->keyword, "Can't return a value from an initializer.");
        }
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

// Function to resolve classes
any Resolver::visitClassStmt(shared_ptr<Class> stmt) {
    // We set the enclosing class and current class
    ClassType enclosing_class = current_class;
    current_class             = ClassType::CLASS;

    declare(stmt->name);
    define(stmt->name);

    // If the passed in superclass exists we resolve it
    if (stmt->superclass != nullptr) {
        // We first check to see if the superclass name matches the class name
        if (stmt->name.lexeme == stmt->superclass->name.lexeme) {
            // If it does we throw an error
            LoxError::error(stmt->superclass->name, "A class can't inherit from itself.");
        }
        current_class = ClassType::SUBCLASS;
        // Otherwise we try to resolve
        resolve(stmt->superclass);
    }

    // If we have a superclass
    if (stmt->superclass != nullptr) {
        // We start scope
        begin_scope();
        // We then retrieve the scope and add 'super' as true
        std::map<std::string, bool>& scope = scopes.back();
        scope["super"]                     = true;
    }

    // We start our scope and save this as true
    begin_scope();
    // We use a reference since we want to modify the original
    std::map<std::string, bool>& scope = scopes.back();
    scope["this"]                      = true;

    // We iterate over each method and resolve them
    for (const shared_ptr<Function>& method : stmt->methods) {
        FunctionType declaration = FunctionType::METHOD;
        // If the method is init we change the function type
        if (method->name.lexeme == "init") {
            declaration = FunctionType::INIT;
        }
        resolve_function(method, declaration);
    }

    // After resolving the class we end the scope
    end_scope();

    // We end the scope of the superclass
    if (stmt->superclass != nullptr) {
        end_scope();
    }

    // We return back to the enclosing class
    current_class = enclosing_class;
    return {};
}

// Function to resolve super expression
any Resolver::visitSuperExpr(shared_ptr<Super> expr) {
    // We check to see if we are outside of a class body
    if (current_class == ClassType::NONE) {
        // We throw an error if so
        LoxError::error(expr->keyword, "Can't use 'super' outside of a class.");
        // We then check if we are not a sub class
    } else if (current_class != ClassType::SUBCLASS) {
        // We throw and error if so
        LoxError::error(expr->keyword, "Can't use 'super' in a class with no superclass.");
    }
    // Otherwise we resolve the local variable
    resolve_local(expr, expr->keyword);
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

// Function to resolve this statements
any Resolver::visitThisExpr(shared_ptr<This> expr) {
    // We test to see if we are inside of a class
    if (current_class == ClassType::NONE) {
        // If we are not we throw an error
        LoxError::error(expr->keyword, "Can't use 'this' outside of a class.");
        return {};
    }
    // Otherwise we resolve
    resolve_local(expr, expr->keyword);
    return {};
}

// Function to resolve Setter node
any Resolver::visitSetExpr(shared_ptr<Set> expr) {
    resolve(expr->value);
    resolve(expr->object);
    return {};
}

// Function to resolve the Getter node
any Resolver::visitGetExpr(shared_ptr<Get> expr) {
    resolve(expr->object);
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
        // We use a reference since we want to modify the original
        std::map<std::string, bool>& scope = scopes.back();
        // We then use find to search for our lexeme
        auto it = scope.find(expr->name.lexeme);
        // We test if the iterator is inside the scope and if the name is set to false
        if (it != scope.end() && it->second == false)
            // If so we throw an error
            LoxError::error(expr->name, "Can't read local variable in its own initializer.");
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
void Resolver::resolve_local(const shared_ptr<Expr>& expr, Token name) {
    /*
     * We start at the inner most scope and work outwards to find the name
     * We create a reverse iterator that starts at the last element and
     * works it way to the first element
     */
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        /*
         * If we match the name, we ask the interpreter to resolve it and pass in
         * the number corresponding the scope
         * if its in the current scope we get 0
         * if its the enclosing scope we get 1
         * if its the scope outside of the immediate enclosing scope we get 2
         * etc...
         */
        if (it->contains(name.lexeme)) {
            // We calculate the distance between the beginning of the reverse iterator
            // and the current increment
            interpreter.resolve(expr, std::distance(scopes.rbegin(), it));
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
    std::map<std::string, bool>& scope = scopes.back();

    // We check to see if a variable has already been declared in the local scope
    auto it = scope.find(name.lexeme);
    if (it != scope.end()) {
        // If so we can kick an error
        LoxError::error(name, "Already a variable with this name in this scope.");
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
    // with true as its value, we use a reference since we want to modify
    // the original map
    std::map<std::string, bool>& scope = scopes.back();
    scope[name.lexeme]                 = true;
}
