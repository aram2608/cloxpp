#pragma once

#include "error.hpp"
#include "interpreter.hpp"
#include "parser.hpp"
#include "resolver.hpp"
#include "scanner.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>


void        run_file(const std::string& filename);
void        run_prompt();
void        run(std::string code);
std::string slurp_file(const std::string& filename);