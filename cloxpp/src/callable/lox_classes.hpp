#ifndef LOX_CLASSES_HPP
#define LOX_CLASSES_HPP

#include "callable/callable.hpp"
#include "core/interpreter.hpp"

#include <any>
#include <iostream>
#include <vector>
#include <memory>

namespace CppLox {

class LoxClass : public LoxCallable, std::enable_shared_from_this<LoxClass> {
  public:
    LoxClass(std::string name);

    // Override for call from LoxCallable interface
    std::any    call(Interpreter& interpreter, std::vector<std::any> arguments) override;
    // Override for arity from LoxCallable Interface
    int         arity() override;
    // Override for to_string from LoxCallable interface
    std::string to_string() override;
    // Class name
    std::string name;
};

class LoxInstance : std::enable_shared_from_this<LoxInstance> {
  public:
    /*
    * Constructor for LoxInstance, we pass in a pointer to the function
    */
    LoxInstance(std::shared_ptr<LoxClass> klass) : klass(klass) {
    }

    // String representation method for each instance
    std::any to_string() {
      return klass->name + " instance.";
    }
    // Pointer to class
    std::shared_ptr<LoxClass> klass;
};

} // namespace CppLox

#endif