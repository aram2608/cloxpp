#pragma once

#include "callable.hpp"
#include "../interpreter.hpp"

#include <any>
#include <memory>
#include <string>
#include <vector>

namespace CppLox {

class Environment;
class Function;

// We create a new Lox function class that is similar to our Native Functions
// we override the same methods to make
class LoxFunction : public LoxCallable {
  public:
    LoxFunction(std::unique_ptr<Function> declaration);
    std::string to_string() override;
    int         arity() override;
    std::any    call(Interpreter& interpreter, std::vector<std::any> arguments) override;

  private:
    std::unique_ptr<Function> declaration;
};

} // namespace CppLox