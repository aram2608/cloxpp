#include "callable/lox_instance.hpp"

#include "callable/lox_functions.hpp"
#include "lox_instance.hpp"

using namespace CppLox;

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> klass) : klass(std::move(klass)) {}

std::string LoxInstance::to_string() { return klass->name + " instance"; }

std::any LoxInstance::get(Token name) {
    // We check if the map contains the lexeme
    if (fields.contains(name.lexeme)) {
        // we then return it
        return fields[name.lexeme];
    }

    // We lookup the method in the class object
    std::shared_ptr<LoxFunction> method = klass->find_method(name.lexeme);
    if (method != nullptr) {
        return method->bind(shared_from_this());
    }

    // Otherwise we throw an error
    throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
}

void LoxInstance::set(Token name, std::any value) { fields[name.lexeme] = value; }
