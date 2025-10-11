#include "../loxlib/chunk/chunk.hpp"

#include <iostream>

int main(int argc, const char **argv) {
    Chunk chunk = Chunk("test chunk");
    chunk.write_chunk(OpCode::OP_RETURN);
    int constant = chunk.add_constant(1.2);
    chunk.write_chunk(OpCode::OP_CONSTANT);
    chunk.write_chunk(constant);

    chunk.dissasemble();
    return 0;
}