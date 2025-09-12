#ifndef NATIVE_FUNCTIONS_HPP
#define NATIVE_FUNCTIONS_HPP

#include "callable/callable.hpp"
#include <any>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

namespace CppLox {
// A clock method that is native to Lox itself
class NativeClock : public LoxCallable {
  public:
    // the number of arguments we pass in is always 0;
    int arity() override;

    // to make the call, we pass in a reference to the Interpreter and the list
    // of arguments we wish to run
    std::any call(Interpreter& interpreter, std::vector<std::any> arguments) override;

    // A way to represent the method as a string
    std::string to_string() override;
};

}

#endif