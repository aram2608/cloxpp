#include "callable/native_functions.hpp"

using namespace CppLox;

int NativeClock::arity() {
        return 0;
    }

    // to make the call, we pass in a reference to the Interpreter and the list
    // of arguments we wish to run
    std::any NativeClock::call(Interpreter& interpreter, std::vector<std::any> arguments) {
        // We first need to get time since epoch as a duration
        std::chrono::duration ticks = std::chrono::system_clock::now().time_since_epoch();
        // We can then cast the object to milliseconds
        long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(ticks).count();
        // We then cast one last time to a double for compatability with our
        // interpreter
        double result = static_cast<double>(ms);
        return result;
    }

    // A way to represent the method as a string
    std::string NativeClock::to_string() {
        return "<native fn>";
    }