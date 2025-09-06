#pragma once

#include "tokens.hpp"
#include "run_time_error.hpp"

#include <iostream>
#include <string_view>

namespace lox {
// We scope our aliases
using std::cerr;
using std::string_view;

class LoxError {
  public:
    // Default constructor so the compiler does not yell at me
    LoxError()            = default;
    bool had_error        = false;
    bool had_RuntimeError = false;

    // A function to report errors to stderr
    void report(int line, string_view where, string_view message) {
        cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
        had_error = true;
    }

    // A function to display errors, we use string view since we dont need to access
    // the string itself and therefore do not need a copy
    void error(int line, string_view message) {
        report(line, "", message);
    }
    // Error overload
    void error(Token token, string_view message) {
        if (token.type == TokenType::eof) {
            report(token.line, " at end", message);
        } else {
            report(token.line, " at '" + token.lexeme + "'", message);
        }
    }

    void runtime_error(const RuntimeError& error) {
        std::cerr << error.what() << "\n[line " << error.token.line << "]\n";
        had_RuntimeError = true;
    }
};

} // namespace lox