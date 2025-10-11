#ifndef LOX_CLASSES_HPP
#define LOX_CLASSES_HPP

#include "callable/callable.hpp"
#include "callable/lox_functions.hpp"
#include "utils/error.hpp"

#include <any>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

namespace CppLox {

class Interpreter;
struct LoxInstance;
class LoxFunction;

class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass> {
  public:
    // Constructor for LoxClass, we pass in its name
    LoxClass(std::string name, std::shared_ptr<LoxClass> superclass,
             std::map<std::string, std::shared_ptr<LoxFunction>> methods);

    // Override for call from LoxCallable interface
    std::any call(Interpreter &interpreter, std::vector<std::any> arguments) override;
    // Override for arity from LoxCallable Interface
    int arity() override;
    // Override for to_string from LoxCallable interface
    std::string to_string() override;

    // Function to lookup methods in class object
    std::shared_ptr<LoxFunction> find_method(std::string name);

    // Class name
    std::string name;
    // Pointer to superclass
    std::shared_ptr<LoxClass> superclass;
    // Map of methods
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
};

} // namespace CppLox

#endif