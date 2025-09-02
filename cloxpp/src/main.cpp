#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::ifstream;
using std::istreambuf_iterator;
using std::string;

// Function to slurp a files contents
string slurp_file(const string &filename) {

    // We first create an ifstream object called file()
    ifstream file(filename);

    // Test to ensure file can open, otherwise assume bad file and return an empty string
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;

        // Return empty string on failure
        return "";
    }

    // Read entire file into string using iterators
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    // Close file to prevent leaks
    file.close();
    return content;
}

int main(int argc, char *argv[]) {

    /*
        We need to catch the number of command line arguments
        More than 1 should exit with a failure status
        In C++ argc is argument count and argv is argument vector, a vector containging
        pointers the arguments themselves

        argc is an integer and stores the number of arguments used including the name
        of the program itself. As such the default value of argc is always 1.
    */
    if (argc > 2) {
        cout << "Usage: cloxpp [script] \n";
        std::exit(EXIT_FAILURE);

        // 1 argument should point to a file path
    } else if (argc == 2) {
        cout << "Running a file...\n";
        // Read file contents
        string contents = slurp_file(argv[1]);

        cout << contents << endl;

        // Otherwise we run the REPL
    } else {
        cout << "Runing the REPL...\n";
    }

    cout << argc << endl;
    return 0;
}