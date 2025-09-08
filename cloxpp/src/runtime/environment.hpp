#pragma once

#include "run_time_error.hpp"
#include "tokens.hpp"

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
     * explicit prevents implicit conversions
     */
    explicit Environment(std::shared_ptr<Environment> enclosing = nullptr)
        : enclosing(std::move(enclosing)) {
    }

    // Function to define and store variables in the map
    void define(std::string identifier, std::any value) {
        // the value is oftentimes a pointer so we move ownership
        // this method overrides id everytime however, since the [] operator does not
        // care if the object already exists or not
        values[identifier] = std::move(value);
    }

    void assign(Token identifier, std::any value) {
        /*
         * We check the internal identifiers string lexeme and make sure its
         * in the map
         * We use an iterator since the [] operators enable a second lookup
         * With the iterator method we can simply look once with find
         */
        auto element = values.find(identifier.lexeme);
        if (element != values.end()) {
            element->second = std::move(value);
            return;
        }

        // We can ealk the chain of environments and recursivley search for
        // the variable, if we dont find it we can throw an error
        if (enclosing != nullptr) {
            enclosing->assign(identifier, std::move(value));
            return;
        }

        // We catch any undefined variables
        throw RuntimeError(identifier, "Undefined variable '" + identifier.lexeme + "'.");
    }

    std::any get(Token identifier) {
        // We check the internal identifiers string lexeme and make sure its
        // in the map
        auto element = values.find(identifier.lexeme);
        if (element != values.end()) {
            // we return the second value from the id iterator
            return element->second;
        }

        // We can ealk the chain of environments and recursivley search for
        // the variable, if we dont find it we can throw an error
        if (enclosing != nullptr) {
            return enclosing->get(identifier);
        }

        // Toss an error if the variable does not exists
        throw RuntimeError(identifier, "Undefined variable '" + identifier.lexeme + "'.");
    }
};

} // namespace CppLox