#include "../loxlib/chunk/chunk.hpp"
#include "../loxlib/vm/vm.hpp"

#include <cxxopts.hpp>
#include <filesystem>
#include <fmt/base.h>
#include <fstream>

// Helper function to get a files contents
static std::string slurp_file(const std::filesystem::path &path) {
    // We create an ifstream object
    std::ifstream file(path);
    // If the file cannot be opened we return an empty string
    if (!file.is_open()) {
        std::cout << "File could not be opened." << std::endl;
        return "";
    }
    // Read entire file into string using iterators
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    // We can then close the file and return the contents
    file.close();
    return content;
}

void repl() {
    while (true) {
        fmt::print(">>> ");
        std::cout.flush();
        std::string code;

        // We need to get the full line
        if (!std::getline(std::cin, code)) {
            break;
        }
        // We retry the loop if no input is entered
        if (code.empty()) {
            continue;
        }
        // Exit loop
        if (code == "exit()") {
            std::exit(EXIT_SUCCESS);
        }

        VM vm = VM(code);
        vm.interpret();
    }
}

void run_file(const std::filesystem::path &path) {
    std::string contents = slurp_file(path);

    VM vm = VM(contents);
    InterpretResult result = vm.interpret();

    if (result == InterpretResult::INTERPRET_COMPILE_ERROR) {
        exit(65);
    }
    if (result == InterpretResult::INTERPRET_RUNTIME_ERROR) {
        exit(70);
    }
}

int main(int argc, const char **argv) {
    cxxopts::Options options("cloxxvm", "Implementation of cloxx in C++");

    options.add_options()("h,help", "This message")("r,repl", "REPL entry point")(
        "f,file", "Lox Script", cxxopts::value<std::string>());

    try {
        // We can now parse our options
        auto result{options.parse(argc, argv)};

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
        } else if (result.count("file")) {
            run_file(result["file"].as<std::string>());
        } else if (result.count("repl")) {
            repl();
        }
    } catch (const std::exception &e) {
        std::cerr << "Error parsing arguments: " << e.what() << std::endl;
        exit(64);
    }
    return 0;
}