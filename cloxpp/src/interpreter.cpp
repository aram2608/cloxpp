#include "interpreter.hpp"

using namespace lox;

Interpreter::Interpreter() {
}

// Helper method to send the expression back to visitor
// implementation
any Interpreter::evaluate(Expr& expr) {
    return expr.accept(*this);
}

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
        return std::any_cast<double>(left) > std::any_cast<double>(right);
    case TokenType::GREATER_EQUAL:
        return std::any_cast<double>(left) >= std::any_cast<double>(right);
    case TokenType::LESS:
        return std::any_cast<double>(left) < std::any_cast<double>(right);
    case TokenType::LESS_EQUAL:
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

        break;
    case TokenType::MINUS:
        // cast to doubles and subtract
        return std::any_cast<double>(left) - std::any_cast<double>(right);
    case TokenType::SLASH:
        // cast to doubles and divide
        return std::any_cast<double>(left) / std::any_cast<double>(right);
    case TokenType::STAR:
        // cast to doubles and multiply
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

// Functio to return a node's literal value
any Interpreter::visitLiteralExpr(Literal& expr) {
    // We stuffed the value after scanning into the token so we can
    // simply retrieve it
    return expr.value;
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
