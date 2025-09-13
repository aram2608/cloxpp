#include "callable/lox_classes.hpp"

#include "lox_classes.hpp"

using namespace CppLox;

// LoxClass constructor, we initialize with its name as a string and a method map
LoxClass::LoxClass(std::string name, std::map<std::string, std::shared_ptr<LoxFunction>> methods)
    : name(std::move(name)), methods(std::move(methods)) {
}

// Override for call method
std::any LoxClass::call(Interpreter& interpreter, std::vector<std::any> arguments) {
    // returns an instance of the class
    return std::make_shared<LoxInstance>(shared_from_this());
}

// Override for arity
int LoxClass::arity() {
    return 0;
}

// Function to return classes name as a string
std::string LoxClass::to_string() {
    return name;
}

// Function to lookup methods in the map
std::shared_ptr<LoxFunction> CppLox::LoxClass::find_method(std::string name) {
    // Test if function is contained inside the map
    if (methods.contains(name)) {
        return methods[name];
    }
    // Otherwise return nullptr
    return nullptr;
}