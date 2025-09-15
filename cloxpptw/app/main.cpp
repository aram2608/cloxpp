#include "../loxlib/core/lox.hpp"

#include <cstdlib>
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, const char* argv[]) {

    // Create our Lox lang instance
    // CppLox::Lox lox = CppLox::Lox();

    /*
     * We need to catch the number of command line arguments
     * More than 1 should exit with a failure status
     * In C++ argc is argument count and argv is argument vector, a vector
     * containing
     * pointers to the arguments themselves
     *
     * argc is an integer and stores the number of arguments used including
     * the name
     * of the program itself. As such the default value of argc is always 1.
     */
    if (argc > 2) {
        cout << "Usage: cloxpp [script] \n";
        std::exit(EXIT_FAILURE);

        // 1 argument should point to a file path
    } else if (argc == 2) {
        // Run Lox lang from file input
        CppLox::Lox::run_file(argv[1]);

        // Otherwise we run the REPL
    } else {
        cout << "Welcome to the Lox REPL!\n" << endl;
        CppLox::Lox::run_prompt();
        // make_test_tree();
    }
    return 0;
}