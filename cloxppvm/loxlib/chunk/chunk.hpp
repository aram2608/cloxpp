#ifndef CLOX_CHUNK_HPP
#define CLOX_CHUNK_HPP

#include "../common.hpp"
#include "value/value.hpp"

#include <algorithm>

/*
 * In Bytecode format, each instruction is stored as a single byte object
 * Our Lox bytecode will be stored in this here file. The name is typically
 * operation code but we will use OpCode as a shorthand.
 */

// I am opting for a scoped enum since they are a bit safer
enum class OpCode {
    OP_RETURN,
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE
};

// We create a custom formatter for our enum class
template <> struct fmt::formatter<OpCode> : formatter<string_view> {
    auto format(OpCode op_code, format_context &ctx) const -> format_context::iterator;
};

struct Chunk {
    Chunk(std::string name);
    // Funciton to push OpCodes onto our "stack"
    void write_chunk(OpCode byte, int line);
    void write_chunk(std::uint8_t byte, int line);
    int add_constant(Value value);
    void dissasemble();
    // C++ has a dynamic array already as a vector so count and capacity are
    // redundant for now
    std::string name_;
    std::vector<std::uint8_t> code_;
    std::vector<int> lines_;
    ValueArray constants_;
};

#endif
