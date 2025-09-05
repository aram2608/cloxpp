#pragma once

#include "expr.hpp"

#include <any>
#include <cassert>
#include <iostream>
#include <memory>

namespace lox {
using std::any_cast;
using std::string;
using std::unique_ptr;

class AstPrinter : public ExprVisitor {
  public:
    // We can use any_cast to safely extract a value from std::any
    // We pass a reference to Expr and borrow the ast node
    string print(Expr& expr) {
        return any_cast<string>(expr.accept(*this));
    }
};

} // namespace lox