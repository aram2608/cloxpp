#pragma once

#include "run_time_error.hpp"
#include "tokens.hpp"

#include <iostream>

namespace CppLox {

class LoxError {
  public:
    // Default constructor so the compiler does not yell at me
    LoxError()            = default;
    bool had_error        = false;
    bool had_RuntimeError = false;

    // A function to report errors to stderr
    void report(int line, std::string where, std::string message) {
        std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
        had_error = true;
    }

    /*
     * A function to display errors, we use a string so that each error can own
     * its own string copies, we need to do this to ensure that
     * the object we inherit the string from can get destroyed and we still have access
     * to the message
     * We do the same for the tokens
     */
    void error(int line, std::string message) {
        report(line, "", message);
    }
    // Error overload
    void error(Token token, std::string message) {
        if (token.type == TokenType::eof) {
            report(token.line, " at end", message);
        } else {
            report(token.line, " at '" + token.lexeme + "'", message);
        }
    }

    // Function to report runtime errors
    void runtime_error(const RuntimeError& error) {
        std::cout << error.token.to_string() << std::endl;
        std::cerr << error.what() << "\n[line " << error.token.line << "]\n";
        had_RuntimeError = true;
    }
};

} // namespace CppLox