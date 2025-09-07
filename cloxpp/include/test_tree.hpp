#pragma once
#include "ast_printer.hpp"

namespace CppLox {

void make_test_tree() {
    // We first create a unique_ptr to Expr
    std::unique_ptr<Expr> expr =
        /*
         * We need to pass in unique_ptrs for each type with the proper constructors
         * Since we default to the move constructor
         * The compiler will complain if we are not incredibly precise
         */
        std::make_unique<Binary>(std::make_unique<Unary>(Token{TokenType::MINUS, "-", nullptr, 1},
                                                         std::make_unique<Literal>(123.)),
                                 Token{TokenType::STAR, "*", nullptr, 1},
                                 std::make_unique<Grouping>(std::make_unique<Literal>(45.67)));
    // Dereference the unique_ptr to get the underlying L-value
    // Print to console
    std::cout << AstPrinter{}.print(*expr) << "\n";
};

} // namespace CppLox