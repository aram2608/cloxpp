#ifndef CALLABLE_HPP
#define CALLABLE_HPP

#include <any>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

namespace CppLox {

class Interpreter;

// Interface for the LoxCallable Class
// we define a handful of virtual methods that we need to override whenever
// we inherit from this class
class LoxCallable {
  public:
    virtual int         arity()                                                         = 0;
    virtual std::any    call(Interpreter& interpreter, std::vector<std::any> arguments) = 0;
    virtual std::string to_string()                                                     = 0;
    virtual ~LoxCallable()                                                              = default;
};

// A clock method that is native to Lox itself
class NativeClock : public LoxCallable {
  public:
    // the number of arguments we pass in is always 0;
    int arity() override {
        return 0;
    }

    // to make the call, we pass in a reference to the Interpreter and the list
    // of arguments we wish to run
    std::any call(Interpreter& interpreter, std::vector<std::any> arguments) override {
        // We first need to get time since epoch as a duration
        std::chrono::duration ticks = std::chrono::system_clock::now().time_since_epoch();
        // We can then cast the object to milliseconds
        long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(ticks).count();
        // We then cast one last time to a double for compatability with our
        // interpreter
        double result = static_cast<double>(ms);
        return result;
    }

    // A way to represent the method as a string
    std::string to_string() override {
        return "<native fn>";
    }
};

} // namespace CppLox

#endif