#pragma once

#include <iostream>
#include <string_view>

using std::cerr;
using std::string_view;

namespace lox {

class LoxError {
  public:
    // Default constructor so the compiler does not yell at me
    LoxError() = default;
    bool err   = false;

    // A function to report errors to stderr
    void report(int line, string_view where, string_view message) {
        cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
        err = true;
    }

    // A function to display errors, we use string view since we dont need to access
    // the string itself and therefore do not need a copy
    void error(int line, string_view message) {
        report(line, "", message);
    }
};

} // namespace lox