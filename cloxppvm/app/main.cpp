#include "../loxlib/chunk/chunk.hpp"

#include <iostream>

int main(int argc, const char **argv) {
    Chunk chunk = Chunk("test chunk");
    chunk.write_chunk(OpCode::OP_RETURN);

    chunk.dissasemble();
    return 0;
}