#include "chunk/chunk.hpp"

// This function formats the OpCode into a string form that makes it easier
// to dissassemble code using fmt
inline auto fmt::formatter<OpCode>::format(OpCode op_code, format_context &ctx) const
    -> format_context::iterator {
    string_view name = "Unknown opcode";
    switch (op_code) {
    case OpCode::OP_RETURN: {
        name = "OP_RETURN";
        break;
    }
    case OpCode::OP_CONSTANT: {
        name = "OP_CONSTANT";
        break;
    }
    case OpCode::OP_NEGATE: {
        name = "OP_NEGATE";
        break;
    }
    case OpCode::OP_ADD: {
        name = "OP_ADD";
        break;
    }
    case OpCode::OP_SUBTRACT: {
        name = "OP_SUBTRACT";
        break;
    }
    case OpCode::OP_DIVIDE: {
        name = "OP_DIVIDE";
        break;
    }
    case OpCode::OP_MULTIPLY: {
        name = "OP_MULTIPLY";
        break;
    }
    }
    return formatter<string_view>::format(name, ctx);
}

// Constructor - we simply initialize the chunk name
Chunk::Chunk(std::string name) : name_(name) {}

// Function overload to write chunk, we pass in the OpCode byte and current line
void Chunk::write_chunk(OpCode byte, int line) {
    code_.push_back(static_cast<std::uint8_t>(byte));
    lines_.push_back(line);
}

// Function overload to write chunk, we pass in the constant byte value and line
void Chunk::write_chunk(std::uint8_t byte, int line) {
    code_.push_back(byte);
    lines_.push_back(line);
}

// Function to add constant values to our constants vector
int Chunk::add_constant(Value value) {
    // We use the wrapper method to append the value
    constants_.write_value_array(value);
    // We use find to get the location of the element
    auto it = std::find(constants_.values.begin(), constants_.values.end(), value);
    // We then use the distance algorithm to calculate the location of the element
    int index = std::distance(constants_.values.begin(), it);
    // We can then return that index
    return index;
}

void Chunk::dissasemble() {
    // We print an initial header
    // :^ center with = as the fill character
    fmt::print("{:=^30}\n", name_);

    // We loop the items in the vector and print to screen
    for (int it = 0; it < code_.size(); /* NO INCREMENT */) {

        // We print the bytecode offset
        fmt::print("{:04d} ", it);

        // We track the line number in our lines vector
        // We compare the current line to the previous line
        if (it > 0 && lines_[it] == lines_[it - 1]) {
            // We print the continuation marker
            fmt::print("   | ");
            // We print the new line number
        } else {
            fmt::print("{:4d} ", lines_[it]);
        }

        // We need to cast the byte back to an OpCode so we can catch it in the
        // switch statement
        OpCode instruction = static_cast<OpCode>(code_[it]);
        switch (instruction) {
        case OpCode::OP_RETURN: {
            fmt::print("{}\n", instruction);
            // Instruction is 1 byte so we advance forward
            it += 1;
            break;
        }
        case OpCode::OP_CONSTANT: {
            // Since opcodes are two bytes long, we check if the next byte
            // is out of bounds, if so then we assume that no constant value was
            // passed in
            if (it + 1 >= code_.size()) {
                fmt::print(stderr, "Error: OP_CONSTANT missing operand.\n");
                return;
            }
            // We get the constant index by looking ahead
            std::uint8_t constant_index = code_[it + 1];
            fmt::print("{} {:>12} {}\n", instruction, constant_index,
                       constants_.values[constant_index]);
            // We now need to advance past the opcode and constant value
            it += 2;
            break;
        }
        case OpCode::OP_ADD: {
            fmt::print("{}\n", instruction);
            it += 1;
            break;
        }
        case OpCode::OP_SUBTRACT: {
            fmt::print("{}\n", instruction);
            it += 1;
            break;
        }
        case OpCode::OP_MULTIPLY: {
            fmt::print("{}\n", instruction);
            it += 1;
            break;
        }
        case OpCode::OP_DIVIDE: {
            fmt::print("{}\n", instruction);
            it += 1;
            break;
        }
        case OpCode::OP_NEGATE: {
            fmt::print("{}\n", instruction);
            it += 1;
            break;
        }
        default: {
            fmt::print("UNKNOWN_OPCODE ({})\n", code_[it]);
            it += 1;
            break;
        }
        }
    }
}