#pragma once

#include "interpreter.hpp"
#include "parser.hpp"
#include "resolver.hpp"
#include "scanner.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

namespace CppLox {

class Lox {

  public:
    Lox();
    void run_file(const std::string& filename);
    void run_prompt();
    bool had_error();
    void reset_errors();

  private:
    Scanner     scanner;
    Parser      parser;
    Interpreter interpreter{};
    void        run(std::string code);
    std::string slurp_file(const std::string& filename);
};

} // namespace CppLox