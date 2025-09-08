#pragma once

#include "tokens.hpp"

#include <iostream>
#include <stdexcept>

namespace CppLox {

// Class to handle Interpreter errors during runtime
class RuntimeError : public std::runtime_error {
  public:
    Token token;

    // RuntimeError Constructor
    // We overload the runtime_error constructor and pass in a const char * and token
    RuntimeError(Token token, std::string message)
        // string.c_str() returns a pointer to the underlying character
        : std::runtime_error{message.c_str()}, token{token} {
    }
};
} // namespace CppLox