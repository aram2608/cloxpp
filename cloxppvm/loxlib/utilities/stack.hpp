#ifndef CLOX_STACK_HPP
#define CLOX_STACK_HPP

#include <array>
#include <cstdint>
#include <fmt/base.h>

// Custom Stack to push and pop values for Lox VM
template <typename T, std::size_t N = 256> class Stack {
  private:
    // We use an array as the size is known at compile time
    // We default to 256 since Lox uses unit8_t size instructions
    std::array<T, N> data_{};
    // Top always points to the location above the last item on the stack
    std::size_t top_ = 0;

  public:
    // Main method to push items onto the stack, this method is safe
    // as it has a size check
    void push(const T &value) {
        // We check the upper bounds
        if (top_ >= N) {
            fmt::print(stderr, "Stack overflow");
            exit(64));
        }
        // We post-increment and save the value
        data_[top_++] = value;
    }

    // Main method to pop items from the stack
    T &pop() {
        // We check the lower bounds
        if (top_ > 0) {
            // We pre-increment and return the value
            return data_[top_--];
        }
    }

    // Helper method to check if the stack is empty
    bool empty() const { return top_ == 0; }
    // Helper method to check if the stack is full
    bool full() const { return top_ == N; }
    // Helper method to check the size of the stack
    std::size_t size() const { return top_; }
};

#endif