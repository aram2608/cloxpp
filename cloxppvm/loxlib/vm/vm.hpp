#ifndef CLOX_VM_HPP
#define CLOX_VM_HPP

#include "../common.hpp"
#include "chunk/chunk.hpp"
#include "compiler/compiler.hpp"
#include "utilities/stack.hpp"

#include <functional>
#include <type_traits>

enum class InterpretResult { INTERPRET_OK, INTERPRET_COMPILE_ERROR, INTERPRET_RUNTIME_ERROR };

struct VM {
    VM(std::string source);
    InterpretResult interpret();
    InterpretResult run();
    void debug_stack();

    template <class Op> inline void binary_op(Op op);
    Chunk chunk_;
    std::size_t ip_;
    Stack<Value, 256> stack_;
};

#endif