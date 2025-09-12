#ifndef LOX_HPP
#define LOX_HPP

#include "utils/error.hpp"
#include "core/interpreter.hpp"
#include "core/parser.hpp"
#include "runtime/resolver.hpp"
#include "core/scanner.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

namespace CppLox {

struct Lox {
    static void        run_file(const std::string& filename);
    static void        run_prompt();
    static void        run(std::string code);
    static std::string slurp_file(const std::string& filename);
};

} // namespace CppLox

#endif