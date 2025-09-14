#include "core/interpreter.hpp"

#include "interpreter.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <typeinfo>

using namespace CppLox;
using std::any;
using std::shared_ptr;
using std::string;
using std::vector;

// Constructor fo Interpreter
// We construct/define all our global runtime parameters/native functions
// here
Interpreter::Interpreter() {
    /*
     * we define a variable named clock that stores a pointer to out Native Clock method
     * we use a shared_ptr since they are much more forgiving than unique_ptrs when it comes to
     * ownership
     */
    globals->define("clock", shared_ptr<NativeClock>{});
}

/*
 * Main logic for interpreting a program
 * We pass in a vector of shared_ptrs
 */
void Interpreter::interpret(const vector<shared_ptr<Stmt>>& stmts) {
    try {
        // We then iterate through them and execute one by one
        for (const shared_ptr<Stmt>& stmt : stmts) {
            // We need to dereference our pointer
            execute(stmt);
        }
    } catch (RuntimeError error) {
        LoxError::runtime_error(error);
    }
}

// Function to resolve expressions
void Interpreter::resolve(shared_ptr<Expr> expr, int depth) {
    // We store the expression with its associated depth
    locals[expr] = depth;
}

// Helper function to execute statemtent
void Interpreter::execute(shared_ptr<Stmt> stmt) {
    stmt->accept(*this);
}

// Helper method to send the expression back to visitor
// implementation
any Interpreter::evaluate(shared_ptr<Expr> expr) {
    return expr->accept(*this);
}

/*
 * Function to iterate and execute over each statement in the block statement
 * we apply the same logic as the interpret method here and pass
 * a const ref of pointers so we do not deplete the vector before
 * iteration is finished
 */
void Interpreter::execute_block(const vector<shared_ptr<Stmt>>& stmts,
                                shared_ptr<Environment>         env) {
    // We first need to store the first environment
    shared_ptr<Environment> previous = this->environment;

    // We transfer ownership of the passed in environment
    this->environment = env;

    // We then try to iterate over the stmts in the vector
    try {
        for (const shared_ptr<Stmt>& stmt : stmts) {
            execute(stmt);
        }
        // We try and catch all exceptios
    } catch (...) {
        // We transfer ownership to th environment
        this->environment = previous;
        throw;
    }

    // We transfer ownership back to the main scope
    this->environment = previous;
}

// Function to visit class node
any Interpreter::visitClassStmt(shared_ptr<Class> stmt) {
    // We create an object to store superclass
    any superclass;
    // If the superclass is not empty
    if (stmt->superclass != nullptr) {
        // We evaluate and store the result, should be a LoxClass object
        superclass = evaluate(stmt->superclass);
        // We then test if its a LoxClass object, and throw an error if not
        if (superclass.type() != typeid(shared_ptr<LoxClass>)) {
            throw RuntimeError(stmt->superclass->name, "Superclass must be a class.");
        }
    }

    // We define the class name to the environment
    environment->define(stmt->name.lexeme, nullptr);

    // We create a new environment if we have a superclass
    if (stmt->superclass != nullptr) {
        // We evaluate and store the result, should be a LoxClass object
        superclass = evaluate(stmt->superclass);
        // We then test if its a LoxClass object, and throw an error if not
        if (superclass.type() != typeid(shared_ptr<LoxClass>)) {
            throw RuntimeError(stmt->superclass->name, "Superclass must be a class.");
        }
        // We then store a reference to the superclass
        environment = std::make_shared<Environment>(environment);
        environment->define("super", superclass);
    }

    // We create a map to store our methods
    std::map<string, shared_ptr<LoxFunction>> methods;

    // We iterate over each method in the Class methods vector
    for (shared_ptr<Function> method : stmt->methods) {
        // We create a function for each method
        shared_ptr<LoxFunction> function =
            std::make_shared<LoxFunction>(method, environment, method->name.lexeme == "init");
        // We then add it to the map
        methods[method->name.lexeme] = function;
    }

    // We initialize our superclass object as nullptr
    std::shared_ptr<LoxClass> superklass = nullptr;
    // We can then try to cast if the type id matches a LoxClass
    if (superclass.type() == typeid(std::shared_ptr<LoxClass>)) {
        superklass = std::any_cast<std::shared_ptr<LoxClass>>(superclass);
    }

    // We create a new LoxClass class
    shared_ptr<LoxClass> klass = std::make_shared<LoxClass>(stmt->name.lexeme, superklass, methods);

    // We do one final check to ensure superklass is not a nullptr and return
    // to the enclosing environment
    if (superklass != nullptr) {
        environment = environment->enclosing;
    }

    // We then assign our created LoxClass to the environment
    environment->assign(stmt->name, std::move(klass));
    return {};
}

// Function to handle interpretation of return statements
// Returns are tricky since we need to skip past sections of the call stack
// as soon as we meet the return statement
any Interpreter::visitReturnStmt(shared_ptr<ReturnStmt> stmt) {
    // we initialize a nullptr to start
    any value = nullptr;
    // if the expression is not nullptr we can evaluate the expression
    // and store the value
    if (stmt->expr != nullptr) {
        value = evaluate(stmt->expr);
    }
    // we throw our return value
    throw Return(std::move(value));
}

// Function to handle blockstm logic
any Interpreter::visitBlockStmt(shared_ptr<Block> stmt) {
    // We move ownership since we cannot copy shared_ptrs
    execute_block(std::move(stmt->stmts), std::make_shared<Environment>(environment));
    return {};
}

// Function to handle expression stmt logic
any Interpreter::visitExpressionStmt(shared_ptr<ExpressionStmt> stmt) {
    // We point to our expression and evalute
    evaluate(stmt->expr);
    return {};
}

// Function to visit our Function node
any Interpreter::visitFunctionStmt(shared_ptr<Function> stmt) {
    // we create our function by passing in the statements and current environment
    // as the function is declared
    shared_ptr<LoxFunction> function = std::make_shared<LoxFunction>(stmt, environment, false);
    // we then define the function in the environemt
    environment->define(stmt->name.lexeme, function);
    return {};
}

// Function to handle print stmt logic
any Interpreter::visitPrintStmt(shared_ptr<Print> stmt) {
    // We evaluate the expression and store temporarily
    any value = evaluate(stmt->expr);
    // We then display the value, the variable is destroyed after leaving scope
    std::cout << make_string(value) << std::endl;
    // We then return an empty std::any{}
    return {};
}

// Function to handle if else statements
any Interpreter::visitIfStmt(shared_ptr<IfStmt> stmt) {
    // We evaluate if the condition is truthy then execute the clause
    if (is_truthy(evaluate(stmt->condition))) {
        execute(stmt->then_branch);
        // Otherwise we evaluate the else clause if it exists
    } else if (stmt->else_branch != nullptr) {
        execute(stmt->else_branch);
    }
    return {};
}

// Logic to handle while loops
any Interpreter::visitWhileStmt(shared_ptr<WhileStmt> stmt) {
    // while the underlying expression is true
    while (is_truthy(evaluate(stmt->condition))) {
        // we evaluate the statements in the body
        execute(stmt->body);
        // debugging snippet for infitine while loops
        // std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    return {};
}

// Function to handle var stmt logic
any Interpreter::visitVarStmt(shared_ptr<Var> stmt) {
    // We need to initialize a return value with null
    any value = nullptr;
    // We check if our value is null
    if (stmt->initializer != nullptr) {
        // If a vlue
        value = evaluate(stmt->initializer);
    }

    // We add our variable to the environment with its value if it has one
    environment->define(stmt->name.lexeme, value);
    // We then return an empty std::any
    return {};
}

// Function to handle logical and, or operations
any Interpreter::visitLogicalExpr(shared_ptr<Logical> expr) {
    // we first evaluate and store the left expressions value
    any left = evaluate(expr->left);

    // we then test to see if the value is truthy or not
    // we can then short circuit
    if (expr->op.type == TokenType::OR) {
        if (is_truthy(left))
            return left;
    } else {
        if (!is_truthy(left))
            return left;
    }

    // otherwise we return the right value
    return evaluate(expr->right);
}

// Function to visit Assignment nodes
any Interpreter::visitAssignExpr(shared_ptr<Assign> expr) {
    // // We evalute the expression and save its value
    any value = evaluate(expr->value);
    // We first need to search the locals map for our expression
    if (locals.contains(expr)) {
        int distance = locals.at(expr);
        environment->assign_at(distance, expr->name, value);
    } else {
        globals->assign(expr->name, value);
    }
    // We then return our value
    return value;
}

// Function to visit binary expression nodes
any Interpreter::visitBinaryExpr(shared_ptr<Binary> expr) {
    // We evaluate left and right expressions
    // we need to dereference the shared_ptr
    any left  = evaluate(expr->left);
    any right = evaluate(expr->right);

    // We catch each Binary op type
    switch (expr->op.type) {
    case TokenType::BANG_EQUAL:
        return !is_equal(left, right);
    case TokenType::EQUAL_EQUAL:
        return is_equal(left, right);
        // Comparison operation
    case TokenType::GREATER:
        check_num_operands(expr->op, left, right);
        return std::any_cast<double>(left) > std::any_cast<double>(right);
    case TokenType::GREATER_EQUAL:
        check_num_operands(expr->op, left, right);
        return std::any_cast<double>(left) >= std::any_cast<double>(right);
    case TokenType::LESS:
        check_num_operands(expr->op, left, right);
        return std::any_cast<double>(left) < std::any_cast<double>(right);
    case TokenType::LESS_EQUAL:
        check_num_operands(expr->op, left, right);
        return std::any_cast<double>(left) <= std::any_cast<double>(right);
    // + is an overloaded operator that can handle both string concat
    // and division
    case TokenType::PLUS:
        // test if both types are doubles then add
        if (left.type() == typeid(double) && right.type() == typeid(double)) {
            return std::any_cast<double>(left) + std::any_cast<double>(right);
        }

        // test if both types are strings then concatenate
        if (left.type() == typeid(string) && right.type() == typeid(string)) {
            return std::any_cast<string>(left) + std::any_cast<string>(right);
        }

        // Catch operations where strings or nums are not used
        throw RuntimeError(expr->op, "Operands must be two numbers or two strings.");
    case TokenType::MINUS:
        // cast to doubles and subtract
        check_num_operands(expr->op, left, right);
        return std::any_cast<double>(left) - std::any_cast<double>(right);
    case TokenType::SLASH:
        // cast to doubles and divide
        check_num_operands(expr->op, left, right);
        if (std::any_cast<double>(right) == double(0)) {
            throw RuntimeError(expr->op, "Division by 0 not allowed.");
        }
        return std::any_cast<double>(left) / std::any_cast<double>(right);
    case TokenType::STAR:
        // cast to doubles and multiply
        check_num_operands(expr->op, left, right);
        return std::any_cast<double>(left) * std::any_cast<double>(right);
    case TokenType::MOD:
        check_num_operands(expr->op, left, right);
        return std::fmod(std::any_cast<double>(left), std::any_cast<double>(right));
    }

    // Unreachable so we return an empty std::any{}
    return {};
}

// Function to vist unary operation node
any Interpreter::visitUnaryExpr(shared_ptr<Unary> expr) {
    // We first evaluate the right most expression
    any right = evaluate(expr->right);
    // We then match the TokenType
    switch (expr->op.type) {
    case TokenType::BANG:
        return !is_truthy(right);
    case TokenType::MINUS:
        check_num_operand(expr->op, right);
        // We cast the value from the right expression to a double
        // then apply the unary op and return
        return -std::any_cast<double>(right);
    }
    // Unreachable so we return an empty std::any{}
    return {};
}

// Function to visit Getter node
any Interpreter::visitGetExpr(shared_ptr<Get> expr) {
    // We first evaluate and store the underlying object
    std::any object = evaluate(expr->object);
    // We test if the object is a LoxInstance
    if (object.type() == typeid(shared_ptr<LoxInstance>)) {
        // If so we return the value stored in the objects field
        // We need to unwrap the std::any object first however
        return std::any_cast<std::shared_ptr<LoxInstance>>(object)->get(expr->name);
    }

    throw RuntimeError(expr->name, "Only instances have properties.");
}

// Function to interpret super expressions
any Interpreter::visitSuperExpr(shared_ptr<Super> expr) {
    // We return the distance to the expression
    int  distance = locals[expr];
    auto val      = environment->get_at(distance, "super");

    // We then create a pointer to a LoxClass at the given distance
    shared_ptr<LoxClass> superclass =
        std::any_cast<shared_ptr<LoxClass>>(environment->get_at(distance, "super"));

    /*
     * We then create an instance of 'this' with a bit of a hack
     * since we control the layout of the environments 'this' and 'super' are always
     * in the same one, so we reach into the same map and retrive 'this'
     */
    shared_ptr<LoxInstance> object =
        std::any_cast<shared_ptr<LoxInstance>>(environment->get_at(distance - 1, "this"));

    // We can now look up and bind the method starting at the super class
    shared_ptr<LoxFunction> method = superclass->find_method(expr->method.lexeme);

    // If the method is a nullptr, we throw an error
    if (method == nullptr) {
        throw RuntimeError(expr->method, "Undefined property '" + expr->method.lexeme + "'.");
    }

    // Otherwise we bind
    return method->bind(object);
}

// Function to interpret This node
any Interpreter::visitThisExpr(shared_ptr<This> expr) {
    // We simply lookup 'this'
    return variable_lookup(expr->keyword, expr);
}

// Function to handle visit the Setter node
any Interpreter::visitSetExpr(shared_ptr<Set> expr) {
    // We first evaluate the underlying object
    std::any object = evaluate(expr->object);

    // If the object is not a Lox instance we toss an errors
    if (object.type() != typeid(shared_ptr<LoxInstance>)) {
        throw RuntimeError(expr->name, "Only instances have fields.");
    }

    // If our check passes, we evaluate the expression
    std::any value = evaluate(expr->value);
    // Then invoke the setter method and return the value
    std::any_cast<std::shared_ptr<LoxInstance>>(object)->set(expr->name, value);
    return value;
}

// Function to handle function calls
any Interpreter::visitCallExpr(shared_ptr<Call> expr) {
    // we evaluate our calle and save it
    any callee = evaluate(expr->callee);

    // we initialize a vector of any to store our args
    vector<any> args;
    // we iterate over the vector of Expr args
    // we make sure its const ref so that we dont deplete the vector too quickly
    for (const shared_ptr<Expr>& arg : expr->args) {
        args.push_back(evaluate(arg));
    }

    // We forward declare our lox callable object
    shared_ptr<LoxCallable> callable;

    // We add a check to ensure our callable is actually a Function type
    if (callee.type() == typeid(shared_ptr<LoxFunction>)) {
        // If we pass our test we can cast to a LoxFunction
        // This is necessary to unwrap the any object
        callable = std::any_cast<shared_ptr<LoxFunction>>(std::move(callee));
    } else if (callee.type() == typeid(shared_ptr<LoxClass>)) {
        // Or a to a LoxClass
        // Similarly we need to unwrap the any object
        callable = std::any_cast<shared_ptr<LoxClass>>(std::move(callee));
    } else {
        // Otherwise we throw a runtime error
        throw RuntimeError(expr->paren, "Can only call functions and classes.");
    }

    // We need to test our the callables arity to ensure the correct number of args are
    // passed
    if (args.size() != callable->arity()) {
        throw RuntimeError{expr->paren,
                           "Expected " + std::to_string(callable->arity()) + " arguments but got " +
                               std::to_string(args.size()) + "."};
    }

    // we can then return a call to the callable with the arguments
    return callable->call(*this, std::move(args));
}

// To evaluate we recursively evaluate
// since Groupings contain other expressions
any Interpreter::visitGroupingExpr(shared_ptr<Grouping> expr) {
    // Since we are using smart pointers we need to dereference
    return evaluate(expr->expr);
}

// Function to return a node's literal value
any Interpreter::visitLiteralExpr(shared_ptr<Literal> expr) {
    // We stuffed the value after scanning into the token so we can
    // simply retrieve it
    return expr->value;
}

// Function to handle variable expressions
any Interpreter::visitVariableExpr(shared_ptr<Variable> expr) {
    // We return a variable if its defined by looking it up in all the
    // scopes
    return variable_lookup(expr->name, expr);
}

// Function to test logical operations
bool Interpreter::is_truthy(const any& object) {
    // We use the type() method to test if we have a nullptr or
    // a boolean
    if (object.type() == typeid(nullptr)) {
        return false;
    }

    // Check for booleans
    if (object.type() == typeid(bool)) {
        return std::any_cast<bool>(object);
    }
    // Everything else is true
    return true;
}

// Function to handle equality testing for types in Lox
bool Interpreter::is_equal(const any& me, const any& you) {
    // Test two nullptrs are equal
    if (me.type() == typeid(nullptr) && you.type() == typeid(nullptr)) {
        return true;
    }
    // A single null value is falsy
    if (me.type() == typeid(nullptr))
        return false;

    // Test if two strings are equal
    if (me.type() == typeid(string) && you.type() == typeid(string)) {
        return std::any_cast<string>(me) == std::any_cast<string>(you);
    }
    // Test two number types
    if (me.type() == typeid(double) && you.type() == typeid(double)) {
        return std::any_cast<double>(me) == std::any_cast<double>(you);
    }
    // Test two booleans
    if (me.type() == typeid(bool) && you.type() == typeid(bool)) {
        return std::any_cast<bool>(me) == std::any_cast<bool>(you);
    }
    // Everything else is false
    return false;
}

// Function to test for numerical types, unary ops
void Interpreter::check_num_operand(const Token& op, const any& operand) {
    if (operand.type() == typeid(double))
        return;
    throw RuntimeError(op, "Operand must be a number.");
}

// Function to test for multiple numerical types, binary expressions
void Interpreter::check_num_operands(const Token& op, const any& op_a, const any& op_b) {
    if (op_a.type() == typeid(double) && op_b.type() == typeid(double))
        return;
    throw RuntimeError(op, "Operands must be numbers.");
}

// Functions to convert types to strings
string Interpreter::make_string(const any& object) {
    if (object.type() == typeid(nullptr))
        return "nil";

    if (object.type() == typeid(double)) {
        string text = std::to_string(std::any_cast<double>(object));
        if (text[text.length() - 2] == '.' && text[text.length() - 1] == '0') {
            text = text.substr(0, text.length() - 2);
        }
        return text;
    }

    if (object.type() == typeid(string)) {
        return std::any_cast<string>(object);
    }
    if (object.type() == typeid(bool)) {
        return std::any_cast<bool>(object) ? "true" : "false";
    }

    if (object.type() == typeid(std::shared_ptr<LoxFunction>)) {
        return std::any_cast<std::shared_ptr<LoxFunction>>(object)->to_string();
    }
    if (object.type() == typeid(std::shared_ptr<LoxClass>)) {
        return std::any_cast<std::shared_ptr<LoxClass>>(object)->to_string();
    }
    if (object.type() == typeid(std::shared_ptr<LoxInstance>)) {
        return std::any_cast<std::shared_ptr<LoxInstance>>(object)->to_string();
    }

    return "Error in make_string: object type not recognized.";
}

// Function to look up variables
any Interpreter::variable_lookup(Token name, shared_ptr<Expr> expr) {
    // We search the map using find and return an iterator of positions
    auto it = locals.find(expr);
    // We test to see if the distance is found in the map
    if (it != locals.end()) {
        // If it is we return it from the environment
        return environment->get_at(it->second, name.lexeme);
    } else {
        // If not we retrieve it from the global scope
        return globals->get(name);
    }
}
