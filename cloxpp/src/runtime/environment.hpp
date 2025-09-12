#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "utils/error.hpp"
#include "utils/tokens.hpp"

#include <any>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace CppLox {

class Environment : public std::enable_shared_from_this<Environment> {
    // shared_ptr to the Environment
    std::shared_ptr<Environment> enclosing;
    // Ordered map of keys and values
    std::map<std::string, std::any> values;

  public:
    /*
     * We pass in an Environment pointer and move ownership
     * By default enclosing is a nullptr
     */
    Environment(std::shared_ptr<Environment> enclosing = nullptr)
        : enclosing(std::move(enclosing)) {
    }

    // Function to define and store variables in the map
    void define(std::string name, std::any value) {
        // the value is oftentimes a pointer so we move ownership
        // this method overrides id everytime however, since the [] operator
        // does not care if the object already exists or not
        values[name] = value;
    }

    void assign(Token name, std::any value) {
        /*
         * We check the internal names string lexeme and make sure its
         * in the map

         */
        if (values.contains(name.lexeme)) {
            // We assign the lexeme to the value
            values[name.lexeme] = value;
            return;
        }

        // We can walk the chain of environments and recursivley search for
        // the variable, if we dont find it we can throw an error
        if (enclosing != nullptr) {
            enclosing->assign(name, value);
            return;
        }

        // We catch any undefined variables
        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    // Function to assign at a specific local environment
    void assign_at(int distance, Token name, std::any value) {
        // we use the ancestor helper method to walk the environment chain and add
        // to the proper environment
        ancestor(distance)->values[name.lexeme] = value;
    }

    std::any get(Token name) {
        // We check the internal names string lexeme and make sure its
        // in the map
        if (values.contains(name.lexeme)) {
            // We return the value
            return values[name.lexeme];
        }

        // We can ealk the chain of environments and recursivley search for
        // the variable, if we dont find it we can throw an error
        if (enclosing != nullptr) {
            return enclosing->get(name);
        }

        // Toss an error if the variable does not exists
        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    // Function to return an name given a specific distance
    std::any get_at(int distance, std::string name) {
        // We retrieve the appropriate environment using the helper method and
        // retrieve the stored variable
        return ancestor(distance)->values[name];
    }

    // Function to walk the environment chain and return the appropriate environment
    std::shared_ptr<Environment> ancestor(int distance) {
        // We create a new environment shared from the parent
        std::shared_ptr<Environment> environment = shared_from_this();
        // We create an iterator to walk the distance and return the enclosing environment
        for (int i = 0; i < distance; ++i) {
            environment = environment->enclosing;
        }
        // We can then return the environment
        return environment;
    }
};

} // namespace CppLox

#endif