#include "chunk/chunk.hpp"

// This function formats the OpCode into a string form that makes it easier
// to dissassemble code
auto fmt::formatter<OpCode>::format(OpCode op_code, format_context &ctx) const
    -> format_context::iterator {
    string_view name = "Unknown opcode";
    switch (op_code) {
    case OpCode::OP_RETURN:
        name = "OP_RETURN";
        break;
    case OpCode::OP_CONSTANT:
        name = "OP_CONSTANT";
        break;
    }
    return formatter<string_view>::format(name, ctx);
}

Chunk::Chunk(std::string name) : name(name) {}

void Chunk::write_chunk(OpCode byte) { code.push_back(static_cast<std::uint8_t>(byte)); }

void Chunk::write_chunk(std::uint8_t byte) { code.push_back(byte); }

int Chunk::add_constant(Value value) {
    constants.write_value_array(value);
    // We use find to get the location of the element
    auto it = std::find(constants.values.begin(), constants.values.end(), value);
    // We then use the distance algorithm to calculate the location of the
    int index = std::distance(constants.values.begin(), it);
    return index;
}

void Chunk::dissasemble() {
    // We print an initial header
    // :^ center with = as the fill character
    fmt::print("{:=^30}\n", name);

    // We loop the items in the vector and print to screen
    for (int it = 0; it < code.size(); /* NO INCREMENT */) {
        OpCode instruction = static_cast<OpCode>(code[it]);
        switch (instruction) {
        case OpCode::OP_RETURN: {
            fmt::print("{:04d} {}\n", it, instruction);
            // Instruction is 1 byte so we advance forward
            it += 1;
            break;
        }
        case OpCode::OP_CONSTANT: {
            // Since opcodes are two bytes long, we check if the next byte
            // is out of bounds, if so then we assume that no constant value was
            // passed in
            if (it + 1 >= code.size()) {
                return;
            }
            // We get the constant index by looking ahead
            std::uint8_t constant_index = code[it + 1];
            fmt::print("{:04d} {} {:04d} {}\n", it, instruction, constant_index,
                       constants.values[constant_index]);
            // We now need to advance past the opcode and constant value
            it += 2;
            break;
        }
        default: {
            fmt::print("{:04d} UNKNOWN_OPCODE ({})\n", it, code[it]);
            it += 1;
            break;
        }
        }
    }
}