#include "lox.hpp"

using namespace lox;

// Empty contsructor for now
Lox::Lox() {
}

// The main logic for our Lox program, handles scanning, parsing, etc.
void Lox::run(string code) {
    // Create out Scanner instance
    Scanner scanner(code);
    // Create tokens from source code
    std::vector<Token> tokens = scanner.scan_tokens();

    // For now we will just print the tokens
    for (const Token& token : tokens) {
        std::cout << token.to_string() << "\n";
    }
}

// Function to wrap the run function around file contents
void Lox::run_file(const string& filename) {
    // Slurp up file contents into a string
    string contents = slurp_file(filename);

    // Run our main logic
    run(contents);

    // Catch any errors in our code
    if (scanner.errors.err) {
        std::exit(EXIT_FAILURE);
    }
}

// Function for main REPL logic
void Lox::run_prompt() {
    /*
        We start by running the REPL in an infinite loop
        We exit the loop as soon as exit() is used.
        Otherwise we evalutate the input
    */
    while (true) {
        string code;
        cin >> code;

        // Exit loop
        if (code == "exit()") {
            std::exit(EXIT_SUCCESS);

            // Ignore empty values
        } else if (code == "") {
            continue;

            // Evaulate text contents
        } else {
            run(code);
            scanner.errors.err = false;
        }
    }
}

// Function to slurp a files contents
string Lox::slurp_file(const string& filename) {
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