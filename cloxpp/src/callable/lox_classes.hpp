#ifndef LOX_CLASSES_HPP
#define LOX_CLASSES_HPP

#include "callable/callable.hpp"

#include <any>
#include <iostream>
#include <memory>
#include <vector>

namespace CppLox {

class Interpreter;
struct LoxInstance;

class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass> {
  public:
    // Constructor for LoxClass, we pass in its name
    LoxClass(std::string name);

    // Override for call from LoxCallable interface
    std::any call(Interpreter& interpreter, std::vector<std::any> arguments) override;
    // Override for arity from LoxCallable Interface
    int arity() override;
    // Override for to_string from LoxCallable interface
    std::string to_string() override;
    // Class name
    std::string name;
};

struct LoxInstance {
    /*
     * Constructor for LoxInstance, we pass in a pointer to the class
     */
    LoxInstance(std::shared_ptr<LoxClass> klass) : klass(std::move(klass)) {
    }

    // String representation method for each instance
    std::string to_string() {
        return klass->name + " instance.";
    }
    // Pointer to class
    std::shared_ptr<LoxClass> klass;
};

} // namespace CppLox

#endif