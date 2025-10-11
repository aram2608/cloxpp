#ifndef CALLABLE_HPP
#define CALLABLE_HPP

#include <any>
#include <string>
#include <vector>

namespace CppLox {

class Interpreter;

// Interface for the LoxCallable Class
// we define a handful of virtual methods that we need to override whenever
// we inherit from this class
class LoxCallable {
  public:
    virtual int arity() = 0;
    virtual std::any call(Interpreter &interpreter, std::vector<std::any> arguments) = 0;
    virtual std::string to_string() = 0;
    virtual ~LoxCallable() = default;
};

} // namespace CppLox

#endif