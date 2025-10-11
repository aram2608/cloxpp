#ifndef CLOX_CHUNK_HPP
#define CLOX_CHUNK_HPP

#include "../common.hpp"

/*
 * In Bytecode format, each instruction is stored as a single byte object
 * Our Lox bytecode will be stored in this here file. The name is typically
 * operation code but we will use OpCode as a shorthand.
 */

// I am opting for a scoped enum since they are a bit safer
// traditional enums can be implicitly converted to integers which is not ideal
enum class OpCode { OP_RETURN };

// We create a custom formatter for out enum class
template <> struct fmt::formatter<OpCode> : formatter<string_view> {
    auto format(OpCode op_code, format_context &ctx) const -> format_context::iterator;
};

struct Chunk {
    Chunk(std::string name);
    // Funciton to push OpCodes onto our "stack"
    void write_chunk(OpCode byte);
    void dissasemble();
    // C++ has a dynamic array already as a vector so count and capacity are
    // redundant for now
    std::string name;
    std::vector<OpCode> code;
};

#endif
