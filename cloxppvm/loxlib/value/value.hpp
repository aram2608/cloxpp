#ifndef CLOX_VALUE_HPP
#define CLOX_VALUE_HPP

#include "../common.hpp"

// We create an alias for doubles to help our Lox mindset
using Value = double;

// A simple data container for our Values
struct ValueArray {
    ValueArray();
    void write_value_array(Value value);
    std::vector<Value> values;
};

#endif