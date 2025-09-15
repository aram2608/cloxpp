#include "callable/lox_functions.hpp"

#include "lox_functions.hpp"

using namespace CppLox;
using std::any;
using std::string;
using std::vector;

// Constructor for lox function class, we pass in a declaration and environment
// and move ownership
LoxFunction::LoxFunction(std::shared_ptr<Function>    declaration,
                         std::shared_ptr<Environment> closure,
                         bool                         is_initializer)
    : declaration(std::move(declaration)), closure(std::move(closure)),
      is_initializer(is_initializer) {
}

// A helper method to return the string representation of a function
string LoxFunction::to_string() {
    return "<fn " + declaration->name.lexeme + ">";
}

// We return an integer value of the number of parameters
int LoxFunction::arity() {
    return declaration->params.size();
}

// we override the LoxCallable call method
any LoxFunction::call(Interpreter& interpreter, vector<any> arguments) {
    /*
     * functions need to have their own enviroment, this is to ensure they have
     * their own scope
     * we first need to copy the environment from the closure
     * we do not move the environment since we are going to reuse the old
     * environment as soon as we leave the function body
     */
    std::shared_ptr environment = std::make_shared<Environment>(closure);

    // we can then create an iterator that iterates over the paremeters and
    // defines them in the environment
    for (int i = 0; i < declaration->params.size(); i++) {
        environment->define(declaration->params[i].lexeme, arguments[i]);
    }

    // we need to catch a Return exception if there is one
    try {
        interpreter.execute_block(std::move(declaration->body), std::move(environment));
    } catch (Interpreter::Return value) {
        // If we are in an init method we return the class instance
        if (is_initializer) {
            return closure->get_at(0, "this");
        }
        // otherwise we return the value
        return value.value;
    }
    // If we are in an init method we return the instance
    if (is_initializer) {
        return closure->get_at(0, "this");
    }
    return nullptr;
}

// Function to bind this to class instance
std::shared_ptr<LoxFunction> LoxFunction::bind(std::shared_ptr<LoxInstance> instance) {
    // We create a new environment from the closure
    std::shared_ptr<Environment> environment = std::make_shared<Environment>(closure);
    // We then define this inside the LoxInstance
    environment->define("this", instance);
    // We then return a function with the declaration and environment
    // Thus every method, has a small 'world' with 'this' inside
    return std::make_shared<LoxFunction>(declaration, environment, is_initializer);
}
