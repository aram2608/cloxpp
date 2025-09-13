#include "callable/lox_classes.hpp"

#include "lox_classes.hpp"

using namespace CppLox;

// LoxClass constructor, we initialize with its name as a string and a method map
LoxClass::LoxClass(std::string name, std::map<std::string, std::shared_ptr<LoxFunction>> methods)
    : name(std::move(name)), methods(std::move(methods)) {
}

// Override for call method
std::any LoxClass::call(Interpreter& interpreter, std::vector<std::any> arguments) {
    // We intialize our instance
    std::shared_ptr<LoxInstance> instance = std::make_shared<LoxInstance>(shared_from_this());
    // We search for an init method
    std::shared_ptr<LoxFunction> initializer = find_method("init");
    // If we find one we bind and call it with its arguments
    if (initializer != nullptr) {
        initializer->bind(instance)->call(interpreter, arguments);
    }
    // returns an instance of the class
    return instance;
}

// Override for arity
int LoxClass::arity() {
    // We search for an init method
    std::shared_ptr<LoxFunction> initializer = find_method("init");
    if (initializer == nullptr) {
        // we return 0 if we dont find it
        return 0;
    }
    // If we do find it, we return its arity
    return initializer->arity();
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