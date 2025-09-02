#include "lox.hpp"

using namespace lox;

Lox::Lox() {
}

void Lox::run() {
}

// Function to wrap the run function around file contents
void Lox::run_file(const string &filename) {
    string contents = slurp_file(filename);
    cout << contents << endl;
}

void Lox::run_prompt() {
    while (true) {
        string contents;
        cin >> contents;

        if (contents == "exit()") {
            std::exit(EXIT_SUCCESS);
        } else {
            cout << contents << endl;
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