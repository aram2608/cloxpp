#ifndef CLOX_VALUE_HPP
#define CLOX_VALUE_HPP

#include "../common.hpp"

using Value = double;

struct ValueArray {
    ValueArray();
    void write_value_array(Value value);
    std::vector<Value> values;
};

#endif