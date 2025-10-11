#pragma once

#include "expr.hpp"

#include <any>
#include <initializer_list>
#include <memory>
#include <sstream>
#include <string>
#include <typeinfo>

namespace CppLox {

class AstPrinter : public ExprVisitor {
  public:
    // We can use any_cast to safely extract a value from std::any
    // We pass a reference to Expr and borrow the ast node
    std::string print(Expr &expr) { return std::any_cast<std::string>(expr.accept(*this)); }

    // Function to stringify binary expr
    std::any visitBinaryExpr(Binary &expr) override {
        // When used with a unique_ptr or shared_ptr, get() returns a raw
        // pointer to the managed object
        return std::string{parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()})};
    }

    // Function to stringify Unary expr
    std::any visitUnaryExpr(Unary &expr) override {
        // When used with a unique_ptr or shared_ptr, get() returns a raw
        // pointer to the managed object
        return std::string{parenthesize(expr.op.lexeme, {expr.right.get()})};
    }

    // Function to stringify Grouping Expr
    std::any visitGroupingExpr(Grouping &expr) override {
        // When used with a unique_ptr or shared_ptr, get() returns a raw
        // pointer to the managed object
        return std::string{parenthesize("group", {expr.expr.get()})};
    }

    std::any visitVariableExpr(Variable &var) override {
        return std::string{parenthesize(var.identifier.to_string())};
    }

    // Function to stringify Literal Expr
    std::any visitLiteralExpr(Literal &expr) override {
        // Test to make sure the object has a value
        if (!expr.value.has_value())
            return std::string{"nil"};
        // We type check for a double
        if (expr.value.type() == typeid(double))
            return std::to_string(std::any_cast<double>(expr.value));
        // We type check for a string
        if (expr.value.type() == typeid(std::string))
            return std::any_cast<std::string>(expr.value);
        // We type check for booleans
        if (expr.value.type() == typeid(bool)) {
            return std::any_cast<bool>(expr.value) ? "true" : "false";
        }
        return std::string{"Error in visitLiteralExpr: literal type not recognized."};
    }

    std::any visitAssignExpr(Assign &expr) override {
        return std::string{parenthesize(expr.identifier.to_string())};
    }

    std::any visitLogicalExpr(Logical &expr) override {
        return std::string{parenthesize(expr.op.lexeme)};
    }

  private:
    /*
     * A helper method to add parenthesis to the nodes in our pretty printer
     * This will help us visualize the parse structure of the syntax trees
     * We use an initializer list here since its lightweight and read only
     */
    std::string parenthesize(std::string lexeme, std::initializer_list<Expr *> children) {
        // We start a string stream, it acts as buffer to store strings
        // the same way that cout << and ofstream do
        std::ostringstream out;
        // Insert our first parenethsis and name
        out << "(" << lexeme;
        // Iterate through our list
        for (Expr *child : children) {
            // Insert child nodes into buffer
            out << " " << std::any_cast<std::string>(child->accept(*this));
        }
        // Close off parenthesis
        out << ")";
        // Return the buffer as as string
        return out.str();
    }
    // Overload for simply tokens
    std::string parenthesize(std::string lexeme) {
        // We start a string stream, it acts as buffer to store strings
        // the same way that cout << and ofstream do
        std::ostringstream out;
        // Insert our first parenethsis and name
        out << "(" << lexeme;
        // Close off parenthesis
        out << ")";
        // Return the buffer as as string
        return out.str();
    }
};

} // namespace CppLox