#include "../loxlib/core/lox.hpp"

#include <cstdlib>
#include <cxxopts.hpp>
#include <iostream>

int main(int argc, const char *argv[]) {

    // We create our argument parser
    cxxopts::Options options("cloxpp", "Implementation of jlox in C++");

    // We add our options
    // By default options are added as booleans so we need to add a type
    // in order to properly parse args
    options.add_options()("h,help", "help")("repl", "REPL Entry Point")(
        "f,file", "Lox Script", cxxopts::value<std::string>());

    // We use a try block in case the user makes a crazy input for some reason
    try {
        // We can now parse our options
        auto result{options.parse(argc, argv)};

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
        } else if (result.count("file")) {
            CppLox::Lox::run_file(result["file"].as<std::string>());
            return 0;
        } else if (result.count("repl")) {
            CppLox::Lox::run_prompt();
        }
    } catch (const std::exception &e) {
        std::cerr << "Error parsing arguments: " << e.what() << std::endl;
    }

    return 0;
}