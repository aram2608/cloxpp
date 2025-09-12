#ifndef ERROR_HPP
#define ERROR_HPP

#include "utils/tokens.hpp"

#include <iostream>
#include <stdexcept>

namespace CppLox {

// Class to handle Interpreter errors during runtime
class RuntimeError : public std::runtime_error {
  public:
    Token token;

    // RuntimeError Constructor
    // We overload the runtime_error constructor and pass in a const char * and
    // token
    RuntimeError(Token token, std::string message)
        // string.c_str() returns a pointer to the underlying character
        : std::runtime_error{message.c_str()}, token{token} {
    }
};

class LoxError {
    friend class Interpreter;
    friend class Parser;
    friend class Scanner;
    friend class Resolver;
    friend struct Lox;

  public:
    // Default constructor so the compiler does not yell at me
    LoxError() = default;
    static bool had_error;
    static bool had_RuntimeError;

    // A function to report errors to stderr
    static void report(int line, std::string where, std::string message) {
        std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
        had_error = true;
    }

    /*
     * A function to display errors, we use a string so that each error can own
     * its own string copies, we need to do this to ensure that
     * the object we inherit the string from can get destroyed and we still have
     * access to the message We do the same for the tokens
     */
    static void error(int line, std::string message) {
        report(line, "", message);
    }
    // Error overload
    static void error(Token token, std::string message) {
        if (token.type == TokenType::eof) {
            report(token.line, " at end", message);
        } else {
            report(token.line, " at '" + token.lexeme + "'", message);
        }
    }

    // Function to report runtime errors
    static void runtime_error(const RuntimeError& error) {
        std::cerr << error.what() << "\n[line " << error.token.line << "]\n";
        had_RuntimeError = true;
    }
};

} // namespace CppLox

// Define static member variables
inline bool CppLox::LoxError::had_error        = false;
inline bool CppLox::LoxError::had_RuntimeError = false;

#endif