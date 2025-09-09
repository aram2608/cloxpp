#include "lox_functions.hpp"

using namespace CppLox;
using std::any;
using std::string;
using std::vector;

// Constructor for lox function class, we pass in a declaration and move ownership
LoxFunction::LoxFunction(std::unique_ptr<Function> declaration)
    : declaration(std::move(declaration)) {
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
    // functions need to have their own enviroment, this is to ensure they have their own scope
    // we first need to copy the interpreters global environment into the function
    // we do not move the environment since we are going to reuse the old environment as soon as
    // we leave the function body, this is one of the benefits of shared_ptr vs unique_ptr for
    managing
    // our environments
    */
    std::shared_ptr environment = std::make_shared<Environment>(interpreter.globals);

    // we can then create an iterator that iterates over the paremeters and defines them
    // in the environment
    for (int i = 0; i < declaration->params.size(); i++) {
        environment->define(declaration->params[i].lexeme, arguments[i]);
    }

    // we move ownership of the body and environment
    // since the environment will be destroyed after exiting the function
    // the body is a unique_ptr so there is no choice but to move ownership
    interpreter.execute_block(std::move(declaration->body), std::move(environment));
    return nullptr;
}
