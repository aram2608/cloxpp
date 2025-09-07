#pragma once

#include <any>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace lox {

class Environment {
    std::unique_ptr<Environment>    enclosing;
    std::map<std::string, std::any> values;

    // We create two overrides for our environment constructor
    Environment() : enclosing(nullptr) {
    }

    // We pass in an Environment pointer and move ownership
    Environment(std::unique_ptr<Environment> enclosing) : enclosing(std::move(enclosing)) {
    }
};

} // namespace lox