#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {

    /* 
        We need to catch the number of command line arguments
        More than 1 should exit with a failure status
        In C++ argc is argument count and argv is argument vector, a vector containging
        pointers the arguments themselves

        argc is an integer and stores the number of arguments used including the name
        of the program itself. As such the default value of argc is always 1.
    */
    if (argc > 2) {
        std::cout << "Usage: cloxpp [script] \n";
        std::exit(EXIT_FAILURE);

    // 1 argument should point to a file path
    } else if (argc == 2) {
        std::cout << "Running a file...\n";

    // Otherwise we run the REPL
    } else {
        std::cout <<"Runing the REPL...\n";
    }

    std::cout << argc << std::endl;
    return 0;
}