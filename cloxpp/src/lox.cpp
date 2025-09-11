#include "lox.hpp"

using namespace CppLox;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::istreambuf_iterator;
using std::shared_ptr;
using std::string;
using std::vector;

// Empty constructor for now
// We initialize an empty parser to start off
Lox::Lox() : parser(std::vector<Token>{}) {
}

// The main logic for our Lox program, handles scanning, parsing, etc.
void Lox::run(string code) {
    // Create out Scanner instance
    Scanner scanner(code);
    // Create tokens from source code
    vector<Token> tokens           = scanner.scan_tokens();
    parser                         = Parser(tokens);
    vector<shared_ptr<Stmt>> stmts = parser.parse();

    // Catch scanner and parser errors
    if (scanner.errors.had_error) {
        return;
    }
    if (parser.errors.had_error) {
        return;
    }

    // If there are no syntax errors we can run our resolver
    Resolver resolver = Resolver(interpreter);
    resolver.resolve(stmts);

    // We catch any resolution errors
    if (resolver.error.had_error) {
        return;
    }

    // We interpret the AST
    interpreter.interpret(stmts);
}

// Function to wrap the run function around file contents
void Lox::run_file(const string& filename) {
    // Slurp up file contents into a string
    string contents = slurp_file(filename);

    // Run our main logic
    run(contents);

    // Catch any errors in our code
    if (scanner.errors.had_error) {
        std::exit(EXIT_FAILURE);
    }

    // Catch runtime errors in file
    if (interpreter.errors.had_RuntimeError) {
        std::exit(EXIT_FAILURE);
    }
}

// Function for main REPL logic
void Lox::run_prompt() {
    /*
     * We start by running the REPL in an infinite loop
     * We exit the loop as soon as exit() is used.
     * Otherwise we evalutate the input
     */
    interpreter.repl = true;
    while (true) {
        string code;
        /*
         * Read the whole line, spaces included
         * We need to use getline() or else cin splits
         * apart strings on newlines
         * We ensure the getline() is succesful to catch EOF and keyboard exits
         */
        if (!std::getline(std::cin, code))
            break;
        // We retry the loop if no input is entered
        if (code.empty())
            continue;

        // Exit loop
        if (code == "exit()") {
            std::exit(EXIT_SUCCESS);

            // Ignore empty values
        } else if (code == "") {
            continue;

            // Evaulate text contents
        } else {
            run(code);
            reset_errors();
        }
    }
}

// Helper method to reset errors after evaluation
void Lox::reset_errors() {
    scanner.errors.had_error            = false;
    parser.errors.had_error             = false;
    interpreter.errors.had_RuntimeError = false;
}

// Function to slurp a files contents
string Lox::slurp_file(const string& filename) {
    // We first create an ifstream object called file()
    ifstream file(filename);

    // Test to ensure file can open, otherwise assume bad file and return an
    // empty string
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