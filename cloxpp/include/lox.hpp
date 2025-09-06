#pragma once

#include "parser.hpp"
#include "scanner.hpp"
#include "interpreter.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

namespace lox {
// We scope our aliases
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::istreambuf_iterator;
using std::string;

class Lox {

  public:
    Lox();
    void run_file(const string& filename);
    void run_prompt();
    bool had_error();
    void reset_errors();

  private:
    Scanner scanner;
    Parser  parser;
    Interpreter interpreter{};
    void    run(string code);
    string  slurp_file(const string& filename);
};

} // namespace lox