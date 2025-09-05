#pragma once

#include "expr.hpp"

#include <any>
#include <initializer_list>
#include <memory>
#include <sstream>
#include <string>
#include <typeinfo>

namespace lox {
// We scope our aliases
using std::initializer_list;
using std::ostringstream;
using std::string;
using std::string_view;
using std::unique_ptr;

class AstPrinter : public ExprVisitor {
  public:
    // We can use any_cast to safely extract a value from std::any
    // We pass a reference to Expr and borrow the ast node
    string print(Expr& expr) {
        return std::any_cast<string>(expr.accept(*this));
    }

    // Function to stringify binary expr
    any visitBinaryExpr(Binary& expr) override {
        return string{parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()})};
    }

    // Function to stringify Unary expr
    any visitUnaryExpr(Unary& expr) override {
        return string{parenthesize(expr.op.lexeme, {expr.right.get()})};
    }

    // Function to stringify Grouping Expr
    any visitGroupingExpr(Grouping& expr) override {
        return string{parenthesize("group", {expr.expr.get()})};
    }

    // Function to stringify Literal Expr
    any visitLiteralExpr(Literal& expr) override {
        // Test to make sure the object has a value
        if (!expr.value.has_value())
            return string{"nil"};
        // We type check for a double
        if (expr.value.type() == typeid(double))
            return std::to_string(std::any_cast<double>(expr.value));
        // We type check for a string
        if (expr.value.type() == typeid(string))
            return std::any_cast<string>(expr.value);
        // We type check for booleans
        if (expr.value.type() == typeid(bool)) {
            return std::any_cast<bool>(expr.value) ? "true" : "false";
        }
        return string{"Error in visitLiteralExpr: literal type not recognized."};
    }

  private:
    // A helper method to add parenthesis to the nodes in our pretty printer
    // This will help us visualize the parse structure of the syntax trees
    // We use an initializer list here since its lightweight and read only
    // same concept for the string_view
    string parenthesize(string_view name, initializer_list<Expr*> children) {
        // We start a string stream, it acts as buffer to store strings
        // the same way that cout << and ofstream do
        ostringstream out;
        // Insert our first parenethsis and name
        out << "(" << name;
        // Iterate through our list
        for (Expr* child : children) {
            // Insert child nodes into buffer
            out << " " << std::any_cast<string>(child->accept(*this));
        }
        // Close off parenthesis
        out << ")";
        // Return the buffer as as string
        return out.str();
    }
};

} // namespace lox