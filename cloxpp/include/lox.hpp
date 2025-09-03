#pragma once

#include "scanner.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::istreambuf_iterator;
using std::string;

namespace lox {

class Lox {

  public:
    Lox();
    void run_file(const string &filename);
    void run_prompt();

  private:
    Scanner scanner;
    void    run(string code);
    string  slurp_file(const string &filename);
    void    error(int line, string message);
    void    report(int line, string where, string message);

    bool err;
};

} // namespace lox