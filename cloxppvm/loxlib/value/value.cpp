#include "value/value.hpp"

ValueArray::ValueArray() {}

void ValueArray::write_value_array(Value value) { values.push_back(value); }