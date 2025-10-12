#include "../loxlib/chunk/chunk.hpp"

#include <iostream>

int main(int argc, const char **argv) {
    Chunk chunk = Chunk("test chunk");
    int constant = chunk.add_constant(1.2);
    chunk.write_chunk(OpCode::OP_CONSTANT, 123);
    chunk.write_chunk(constant, 123);
    chunk.write_chunk(OpCode::OP_RETURN, 123);

    chunk.dissasemble();
    return 0;
}