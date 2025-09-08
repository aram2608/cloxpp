#include "interpreter.hpp"

#include <iostream>

using namespace CppLox;
using std::any;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

Interpreter::Interpreter() {
}

// Main logic for interpreting a program
// We pass in a vector of unique_ptrs to Stmts
void Interpreter::interpret(vector<unique_ptr<Stmt>> stmts) {
    try {
        // We then iterate through them and execute one by one
        for (const unique_ptr<Stmt>& stmt : stmts) {
            // We need to dereference our pointer
            execute(*stmt);
        }
    } catch (RuntimeError error) {
        errors.runtime_error(error);
    }
}

// Helper function to execute statemtent
void Interpreter::execute(Stmt& stmt) {
    stmt.accept(*this);
}

// Function to iterate and execute over each statement in the block statement
void Interpreter::execute_block(vector<unique_ptr<Stmt>> stmts, shared_ptr<Environment> env) {
    // We first need to store the first environment
    shared_ptr<Environment> previous = this->environment;

    // We transfer ownership of the passed in environment to the current
    // environment
    this->environment = std::move(env);

    // We then try to iterate over the stmts in the vector
    try {
        for (const unique_ptr<Stmt>& stmt : stmts) {
            execute(*stmt);
        }
        // We try and catch all exceptios
    } catch (...) {
        // We transfer ownership to this environment
        this->environment = std::move(previous);
        throw;
    }

    // We once again transfer ownership back to the main scope
    this->environment = std::move(previous);
}

// Function to handle blockstm logic
any Interpreter::visitBlockStmt(Block& stmt) {
    // We move ownership since we cannot copy unique_ptrs
    execute_block(std::move(stmt.stmts), std::make_shared<Environment>(environment));
    return {};
}

// Function to handle expression stmt logic
any Interpreter::visitExpressionStmt(Expression& stmt) {
    // We dereference the pointer and evaulate the underlying expression
    // If in repl mode evaluate the expression and print to screen
    if (repl) {
        any value = evaluate(*stmt.expr);
        std::cout << make_string(value) << std::endl;
    }
    // otherwise evaluate then destroy the value
    evaluate(*stmt.expr);
    // we then return an empty std::any{}
    return {};
}

// Function to handle print stmt logic
any Interpreter::visitPrintStmt(Print& stmt) {
    // We evaluate the expression and store temporarily
    any value = evaluate(*stmt.expr);
    // We then display the value, the variable is destroyed after leaving scope
    std::cout << make_string(value) << std::endl;
    // We then return an empty std::any{}
    return {};
}

// Function to handle if else statements
any Interpreter::visitIfStmt(IfStmt& stmt) {
    if (is_truthy(evaluate(*stmt.condition))) {
        execute(*stmt.then_branch);
    } else if (stmt.else_branch != nullptr) {
        execute(*stmt.else_branch);
    }
    return {};
}

// Function to handle var stmt logic
any Interpreter::visitVarStmt(Var& stmt) {
    // We need to initialize a return value with null
    any value = nullptr;
    // We check if our value is null
    if (stmt.initializer != nullptr) {
        // If a vlue
        value = evaluate(*stmt.initializer);
    }

    // We add our variable to the environment with its value if it has one
    environment->define(stmt.identifier.lexeme, std::move(value));
    // We then return an empty std::any
    return {};
}

// Function to handle logical and, or operations
any CppLox::Interpreter::visitLogicalExpr(Logical& expr) {
    // we first evaluate and store the left expressions value
    any left = evaluate(*expr.left);

    // we then test to see if the value is truthy or not
    // we can then short circuit
    if (expr.op.type == TokenType::OR) {
      if (is_truthy(left)) return left;
    } else {
      if (!is_truthy(left)) return left;
    }

    // otherwise we return the right value
    return evaluate(*expr.right);
}

// Helper method to send the expression back to visitor
// implementation
any Interpreter::evaluate(Expr& expr) {
    return expr.accept(*this);
}

// Function to visit Assignment nodes
any Interpreter::visitAssignExpr(Assign& expr) {
    // We evalute the expression and save its value
    any value = evaluate(*expr.value);
    // We need to assign the variable and identifier
    environment->assign(expr.identifier, value);
    // We return the value
    return value;
}

// Function to visit binary expression nodes
any Interpreter::visitBinaryExpr(Binary& expr) {
    // We evaluate left and right expressions
    // we need to dereference the unique_ptr
    any left  = evaluate(*expr.left);
    any right = evaluate(*expr.right);

    // We catch each Binary op type
    switch (expr.op.type) {
    case TokenType::BANG_EQUAL:
        return !is_equal(left, right);
    case TokenType::EQUAL_EQUAL:
        return is_equal(left, right);
        // Comparison operation
    case TokenType::GREATER:
        check_num_operands(expr.op, left, right);
        return std::any_cast<double>(left) > std::any_cast<double>(right);
    case TokenType::GREATER_EQUAL:
        check_num_operands(expr.op, left, right);
        return std::any_cast<double>(left) >= std::any_cast<double>(right);
    case TokenType::LESS:
        check_num_operands(expr.op, left, right);
        return std::any_cast<double>(left) < std::any_cast<double>(right);
    case TokenType::LESS_EQUAL:
        check_num_operands(expr.op, left, right);
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
        throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");
    case TokenType::MINUS:
        // cast to doubles and subtract
        check_num_operands(expr.op, left, right);
        return std::any_cast<double>(left) - std::any_cast<double>(right);
    case TokenType::SLASH:
        // cast to doubles and divide
        check_num_operands(expr.op, left, right);
        if (std::any_cast<double>(right) == double(0)) {
            throw RuntimeError(expr.op, "Division by 0.");
        }
        return std::any_cast<double>(left) / std::any_cast<double>(right);
    case TokenType::STAR:
        // cast to doubles and multiply
        check_num_operands(expr.op, left, right);
        return std::any_cast<double>(left) * std::any_cast<double>(right);
    }

    // Unreachable so we return an empty std::any{}
    return {};
}

//
any Interpreter::visitUnaryExpr(Unary& expr) {
    any right = evaluate(*expr.right);

    switch (expr.op.type) {
    case TokenType::BANG:
        return !is_truthy(right);
    case TokenType::MINUS:
        check_num_operand(expr.op, right);
        // We cast the value from the right expression to a double
        // then apply the unary op and return
        return -std::any_cast<double>(right);
    }
    // Unreachable so we return an empty std::any{}
    return {};
}

// To evaluate we recursively evaluate
// since Groupings contain other expressions
any Interpreter::visitGroupingExpr(Grouping& expr) {
    // Since we are using smart pointers we need to dereference
    return evaluate(*expr.expr);
}

// Function to return a node's literal value
any Interpreter::visitLiteralExpr(Literal& expr) {
    // We stuffed the value after scanning into the token so we can
    // simply retrieve it
    return expr.value;
}

// Function to handle variable expressions
any Interpreter::visitVariableExpr(Variable& var) {
    // We return a variable if its defined
    return environment->get(var.identifier);
}

// Function to test logical operations
// Pass in const ref since this is read only
bool Interpreter::is_truthy(const any& object) {
    // We use the type() method to test if we have a nullptr or
    // a boolean
    if (object.type() == typeid(nullptr))
        return false;
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
