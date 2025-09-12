#include "callable/lox_classes.hpp"

using namespace CppLox;

// LoxClass constructor, we initialize with its name as a string
LoxClass::LoxClass(std::string name) : name(name) {
}

// Override for call method
std::any LoxClass::call(Interpreter& interpreter, std::vector<std::any> arguments) {
    // returns an instance of the class
    std::shared_ptr<LoxInstance> instance = std::make_shared<LoxInstance>(shared_from_this());
    return instance;
}

// Override for arity
int LoxClass::arity() {
    return 0;
}

// Function to return classes name as a string
std::string LoxClass::to_string() {
    return name;
}