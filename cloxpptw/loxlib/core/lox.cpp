#include "core/lox.hpp"

using namespace CppLox;

// The main logic for our Lox program, handles scanning, parsing, etc.
void Lox::run(std::string code) {
    // Create our Interpreter instance
    CppLox::Interpreter interpreter;
    // Create out Scanner instance
    CppLox::Scanner scanner = CppLox::Scanner(code);
    // Create tokens from source code
    std::vector<CppLox::Token> tokens = scanner.scan_tokens();
    CppLox::Parser parser = CppLox::Parser(tokens);
    std::vector<std::shared_ptr<CppLox::Stmt>> stmts = parser.parse();

    // Catch scanner and parser errors
    if (CppLox::LoxError::had_error) {
        return;
    }

    // If there are no syntax errors we can run our resolver
    CppLox::Resolver resolver = CppLox::Resolver(interpreter);
    resolver.resolve(stmts);

    // We catch any resolution errors
    if (CppLox::LoxError::had_error) {
        return;
    }

    // We interpret the AST
    interpreter.interpret(stmts);
}

// Function to wrap the run function around file contents
void Lox::run_file(const std::string &filename) {
    // Slurp up file contents into a string
    std::string contents = slurp_file(filename);

    // Run our main logic
    Lox::run(contents);

    // Catch any errors in our code
    if (CppLox::LoxError::had_error) {
        std::exit(EXIT_FAILURE);
    }

    // Catch runtime errors in file
    if (CppLox::LoxError::had_RuntimeError) {
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
    while (true) {
        // A little prompt to pretty up the repl a bit
        std::cout << ">>> " << std::flush;
        std::string code;
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
            CppLox::LoxError::had_error = false;
            CppLox::LoxError::had_RuntimeError = false;
        }
    }
}

// Function to slurp a files contents
std::string Lox::slurp_file(const std::string &filename) {
    // We first create an ifstream object called file()
    std::ifstream file(filename);

    // Test to ensure file can open, otherwise assume bad file and return an
    // empty string
    if (!file.is_open()) {
        std::cout << "Error: Could not open file " << filename << std::endl;

        // Return empty string on failure
        return "";
    }

    // Read entire file into string using iterators
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Close file to prevent leaks
    file.close();
    return content;
}