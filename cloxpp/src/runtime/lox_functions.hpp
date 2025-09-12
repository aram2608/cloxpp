#ifndef LOX_FUNCTIONS_HPP
#define LOX_FUNCTIONS_HPP

#include "../interpreter.hpp"
#include "callable.hpp"

#include <any>
#include <memory>
#include <string>
#include <vector>

namespace CppLox {

class Environment;
struct Function;

// We create a new Lox function class that is similar to our Native Functions
// we override the same methods to make
class LoxFunction : public LoxCallable {
  public:
    LoxFunction(std::shared_ptr<Function> declaration, std::shared_ptr<Environment> closure);
    std::string to_string() override;
    int         arity() override;
    std::any    call(Interpreter& interpreter, std::vector<std::any> arguments) override;

  private:
    std::shared_ptr<Function>    declaration;
    std::shared_ptr<Environment> closure;
};

} // namespace CppLox

#endif