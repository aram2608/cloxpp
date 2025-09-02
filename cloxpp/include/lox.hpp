#pragma once

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
    void run(string code);
    void run_file(const string &filename);
    void run_prompt();

    string slurp_file(const string &filename);

  private:
};

} // namespace lox