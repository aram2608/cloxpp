#include "vm/vm.hpp"

#include "vm.hpp"

VM::VM(std::string source) : chunk("test chunk") {}

InterpretResult VM::interpret() {
    ip = 0;
    return InterpretResult::INTERPRET_OK;
}

InterpretResult VM::run() {
    // We create an infinite loop to run the byte code
    for (;;) {
        OpCode instruction = static_cast<OpCode>(chunk.code[ip]);
        switch (instruction) {
        case OpCode::OP_CONSTANT: {
            // We look ahead to snag the index
            Value constant = chunk.constants.values[chunk.code[ip + 1]];
            // We push our value onto the stack
            stack.push(constant);
            ip += 2;
            break;
        }
        case OpCode::OP_ADD: {
            binary_op(std::plus<>{});
            ip += 1;
            break;
        }
        case OpCode::OP_SUBTRACT: {
            binary_op(std::minus<>{});
            ip += 1;
            break;
        }
        case OpCode::OP_DIVIDE: {
            binary_op(std::divides<>{});
            ip += 1;
            break;
        }
        case OpCode::OP_MULTIPLY: {
            binary_op(std::multiplies<>{});
            ip += 1;
            break;
        }
        case OpCode::OP_NEGATE: {
            // We need to store the top value
            Value constant = stack.pop();
            // We can then negate the value
            stack.push((-constant));
            ip += 1;
            break;
        }
        case OpCode::OP_RETURN: {
            // We simply print the top value then pop it off the stack
            fmt::print("{}\n", stack.pop());
            return InterpretResult::INTERPRET_OK;
            break;
        }
        }
    }
}

void VM::debug_stack() {
    for (int it = 0; it < stack.size(); ++it) {
        fmt::print("{}", stack.pop());
    }
}

// A little helper for binary operators
template <class Op> inline void VM::binary_op(Op op) {
    // We get the second value
    double b = stack.top();
    // We then remove it from the stack
    stack.pop();
    // We then get the ssecond value
    double a = stack.top();
    // We then remove it from the stack
    stack.pop();
    // We add a division by zero check
    if constexpr (std::is_same_v<Op, std::divides<void>>) {
        if (b == 0.0) {
            fmt::print(stderr, "Error: Division by zero\n");
            exit(1);
        }
    }
    // We then push the result of the operation onto the stack
    stack.push(op(a, b));
}