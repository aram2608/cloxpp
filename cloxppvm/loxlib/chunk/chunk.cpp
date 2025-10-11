#include "chunk/chunk.hpp"

// This function formats the OpCode into a string form that makes it easier
// to dissassemble code
auto fmt::formatter<OpCode>::format(OpCode op_code, format_context &ctx) const
    -> format_context::iterator {
    string_view name = "unknown";
    switch (op_code) {
    case OpCode::OP_RETURN:
        name = "OP_RETURN";
        break;
    }
    return formatter<string_view>::format(name, ctx);
}

Chunk::Chunk(std::string name) : name(name) {}

void Chunk::write_chunk(OpCode byte) { code.push_back(byte); }

void Chunk::dissasemble() {
    // We print an initial header
    // :^ center with = as the fill character
    fmt::print("{:=^30}\n", name);

    // We loop the items in the vector and print to screen
    for (int it = 0; it < code.size(); ++it) {
        fmt::print("{:04d} {}\n", it, code[it]);
    }
}