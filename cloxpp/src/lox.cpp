#include "lox.hpp"

using namespace lox;

// Empty contsructor for now
Lox::Lox() {
}

// The main logic for our Lox program, handles scanning, parsing, etc.
void Lox::run(string code) {
    // For now just print out
    cout << code << endl;
}

// Function to wrap the run function around file contents
void Lox::run_file(const string &filename) {
    // Slurp up file contents into a string
    string contents = slurp_file(filename);

    // Run our main logic
    run(contents);
}

// Function for main REPL logic
void Lox::run_prompt() {
    /*
        We start by running the REPL in an infinite loop
        We exit the loop as soon as exit() is used.
        Otherwise we evalutate the input
    */
    while (true) {
        string contents;
        cin >> contents;

        // Exit loop
        if (contents == "exit()") {
            std::exit(EXIT_SUCCESS);

            // Ignore empty values
        } else if (contents == "") {
            continue;

            // Evaulate text contents
        } else {
            run(contents);
        }
    }
}

// Function to slurp a files contents
string Lox::slurp_file(const string &filename) {
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