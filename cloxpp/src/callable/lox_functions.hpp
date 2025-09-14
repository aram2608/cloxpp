#ifndef LOX_FUNCTIONS_HPP
#define LOX_FUNCTIONS_HPP

#include "callable/callable.hpp"
#include "callable/lox_classes.hpp"
#include "core/interpreter.hpp"

#include <any>
#include <memory>
#include <string>
#include <vector>

namespace CppLox {

class Environment;
struct Function;
struct LoxInstance;

// We create a new Lox function class that is similar to our Native Functions
// we override the same methods to make
class LoxFunction : public LoxCallable {
  public:
    /*
     * Lox Function constructor, we pass in a pointer to the underlying function
     * and environment
     */
    LoxFunction(std::shared_ptr<Function>    declaration,
                std::shared_ptr<Environment> closure,
                bool                         is_initializer);
    // Override to convert to string
    std::string to_string() override;
    // Override to represent arity()
    int arity() override;
    // Override to call method
    std::any call(Interpreter& interpreter, std::vector<std::any> arguments) override;

    std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> instance);

    bool is_initializer;
    // Pointer to closure (enclosing environment)
    std::shared_ptr<Environment> closure;

  private:
    // Pointer to declaration
    std::shared_ptr<Function> declaration;
};

} // namespace CppLox

#endif