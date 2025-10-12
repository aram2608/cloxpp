#ifndef CLOX_VM_HPP
#define CLOX_VM_HPP

#include "../common.hpp"
#include "chunk/chunk.hpp"

#include <functional>
#include <stack>
#include <type_traits>

enum class InterpretResult { INTERPRET_OK, INTERPRET_COMPILE_ERROR, INTERPRET_RUNTIME_ERROR };

struct VM {
    VM(Chunk chunk);
    InterpretResult interpret();
    InterpretResult run();
    void debug_stack();

    template <class Op> inline void binary_op(Op op);
    Chunk chunk;
    std::size_t ip;
    std::stack<Value> stack;
};

#endif