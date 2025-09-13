#ifndef LOX_CLASSES_HPP
#define LOX_CLASSES_HPP

#include "callable/callable.hpp"
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
    LoxClass(std::string name, std::map<std::string, std::shared_ptr<LoxFunction>> methods);

    // Override for call from LoxCallable interface
    std::any call(Interpreter& interpreter, std::vector<std::any> arguments) override;
    // Override for arity from LoxCallable Interface
    int arity() override;
    // Override for to_string from LoxCallable interface
    std::string to_string() override;

    // Function to lookup methods in class object
    std::shared_ptr<LoxFunction> find_method(std::string name);

    // Class name
    std::string name;
    // Map of methods
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
};

struct LoxInstance {
    /*
     * Constructor for LoxInstance, we pass in a pointer to the class
     */
    LoxInstance(std::shared_ptr<LoxClass> klass) : klass(std::move(klass)) {
    }

    // String representation method for each instance
    std::string to_string() {
        return klass->name + " instance";
    }

    // Function to return values from an instances properties
    std::any get(Token name) {
        // We check if the map contains the lexeme
        if (fields.contains(name.lexeme)) {
            // we then return it
            return fields[name.lexeme];
        }

        // We lookup the method in the class object
        std::shared_ptr<LoxFunction> method = klass->find_method(name.lexeme);
        if (method != nullptr) {
            return method;
        }

        // Otherwise we throw an error
        throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
    }

    // Function to set values for an instances properties
    void set(Token name, std::any value) {
        fields[name.lexeme] = value;
    }

    // Pointer to class
    std::shared_ptr<LoxClass>       klass;
    std::map<std::string, std::any> fields;
};

} // namespace CppLox

#endif