#include "interpreter.hpp"

#include <iostream>

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
     * we define a variable named clock that stores a pointer to out Native
     * Clock method we use a shared_ptr since they are much more forgiving than
     * unique_ptrs when it comes to ownership
     */
    globals->define("clock", std::shared_ptr<NativeClock>{});
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
        errors.runtime_error(error);
    }
}

// Helper function to execute statemtent
void Interpreter::execute(std::shared_ptr<Stmt> stmt) {
    stmt->accept(*this);
}

// Helper method to send the expression back to visitor
// implementation
any Interpreter::evaluate(std::shared_ptr<Expr> expr) {
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

    // We transfer ownership of the passed in environment to the current
    // environment
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

// Function to handle blockstm logic
any Interpreter::visitBlockStmt(std::shared_ptr<Block> stmt) {
    // We move ownership since we cannot copy shared_ptrs
    execute_block(std::move(stmt->stmts), std::make_shared<Environment>(environment));
    return {};
}

// Function to handle expression stmt logic
any Interpreter::visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) {
    // We point to our expression and evalute
    evaluate(stmt->expr);
    return {};
}

// Function to visit our Function node
any Interpreter::visitFunctionStmt(std::shared_ptr<Function> stmt) {
    // we create our function
    shared_ptr<LoxFunction> function = std::make_shared<LoxFunction>(stmt);
    // we then define the function in the environemt
    environment->define(stmt->name.lexeme, function);
    return {};
}

// Function to handle print stmt logic
any Interpreter::visitPrintStmt(std::shared_ptr<Print> stmt) {
    // We evaluate the expression and store temporarily
    any value = evaluate(stmt->expr);
    // We then display the value, the variable is destroyed after leaving scope
    std::cout << make_string(value) << std::endl;
    // We then return an empty std::any{}
    return {};
}

// Function to handle if else statements
any Interpreter::visitIfStmt(std::shared_ptr<IfStmt> stmt) {
    if (is_truthy(evaluate(stmt->condition))) {
        execute(stmt->then_branch);
    } else if (stmt->else_branch != nullptr) {
        execute(stmt->else_branch);
    }
    return {};
}

// Logic to handle while loops
any Interpreter::visitWhileStmt(std::shared_ptr<WhileStmt> stmt) {
    // while the underlying expression is true
    while (is_truthy(evaluate(stmt->condition))) {
        // we evaluate the statements in the body
        execute(stmt->body);
    }
    return {};
}

// Function to handle var stmt logic
any Interpreter::visitVarStmt(std::shared_ptr<Var> stmt) {
    // We need to initialize a return value with null
    any value = nullptr;
    // We check if our value is null
    if (stmt->initializer != nullptr) {
        // If a vlue
        value = evaluate(stmt->initializer);
    }

    // We add our variable to the environment with its value if it has one
    environment->define(stmt->identifier.lexeme, value);
    // We then return an empty std::any
    return {};
}

// Function to handle logical and, or operations
any Interpreter::visitLogicalExpr(std::shared_ptr<Logical> expr) {
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
any Interpreter::visitAssignExpr(std::shared_ptr<Assign> expr) {
    // // We evalute the expression and save its value
    any value = evaluate(expr->value);
    // We need to assign the variable and identifier
    environment->assign(expr->identifier, value);
    // We return the value
    return value;
}

// Function to visit binary expression nodes
any Interpreter::visitBinaryExpr(std::shared_ptr<Binary> expr) {
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
            throw RuntimeError(expr->op, "Division by 0->");
        }
        return std::any_cast<double>(left) / std::any_cast<double>(right);
    case TokenType::STAR:
        // cast to doubles and multiply
        check_num_operands(expr->op, left, right);
        return std::any_cast<double>(left) * std::any_cast<double>(right);
    }

    // Unreachable so we return an empty std::any{}
    return {};
}

//
any Interpreter::visitUnaryExpr(std::shared_ptr<Unary> expr) {
    any right = evaluate(expr->right);

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

// Function to handle function calls
any Interpreter::visitCallExpr(std::shared_ptr<Call> expr) {
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
    shared_ptr<LoxCallable> function;

    // We add a check to ensure our callable is actually a Function type
    if (callee.type() != typeid(shared_ptr<LoxFunction>)) {
        throw RuntimeError(expr->paren, "Can only call functions and classes.");
    }
    function = std::any_cast<std::shared_ptr<LoxFunction>>(std::move(callee));

    if (args.size() != function->arity()) {
        throw RuntimeError{expr->paren,
                           "Expected " + std::to_string(function->arity()) + " arguments but got " +
                               std::to_string(args.size()) + "."};
    }

    return function->call(*this, std::move(args));
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
    // We return a variable if its defined
    return environment->get(expr->identifier);
}

// Function to test logical operations
// Pass in const ref since this is read only
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
    throw RuntimeError{op, "Operand must be a number."};
}

// Function to test for multiple numerical types, binary expressions
void Interpreter::check_num_operands(const Token& op, const any& op_a, const any& op_b) {
    if (op_a.type() == typeid(double) && op_b.type() == typeid(double))
        return;
    throw RuntimeError{op, "Operands must be a numbers."};
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

    return "Error in make_string: object type not recognized.";
}
