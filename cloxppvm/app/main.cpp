#include "../loxlib/chunk/chunk.hpp"
#include "../loxlib/vm/vm.hpp"

#include <iostream>

int main(int argc, const char **argv) {
    Chunk chunk = Chunk("test chunk");
    int constant = chunk.add_constant(1.2);
    chunk.write_chunk(OpCode::OP_CONSTANT, 123);
    chunk.write_chunk(constant, 123);

    constant = chunk.add_constant(3.4);
    chunk.write_chunk(OpCode::OP_CONSTANT, 123);
    chunk.write_chunk(constant, 123);

    chunk.write_chunk(OpCode::OP_ADD, 123);

    constant = chunk.add_constant(5.6);
    chunk.write_chunk(OpCode::OP_CONSTANT, 123);
    chunk.write_chunk(constant, 123);

    chunk.write_chunk(OpCode::OP_DIVIDE, 123);

    chunk.write_chunk(OpCode::OP_NEGATE, 123);
    chunk.write_chunk(OpCode::OP_RETURN, 123);
    VM vm = VM(chunk);
    vm.interpret();
    return 0;
}