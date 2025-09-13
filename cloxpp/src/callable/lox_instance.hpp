#ifndef LOX_INSTANCE_HPP
#define LOX_INSTANCE_HPP

#include "callable/lox_classes.hpp"

#include <any>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

namespace CppLox {

class LoxClass;

struct LoxInstance : std::enable_shared_from_this<LoxInstance> {
    /*
     * Constructor for LoxInstance, we pass in a pointer to the class
     */
    LoxInstance(std::shared_ptr<LoxClass> klass);

    // String representation method for each instance
    std::string to_string();

    // Function to return values from an instances properties
    std::any get(Token name);

    // Function to set values for an instances properties
    void set(Token name, std::any value);

    // Pointer to class
    std::shared_ptr<LoxClass>       klass;
    std::map<std::string, std::any> fields;
};

} // namespace CppLox

#endif