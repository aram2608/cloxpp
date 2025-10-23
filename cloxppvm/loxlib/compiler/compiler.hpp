#ifndef CLOX_COMPILER_HPP
#define CLOX_COMPILER_HPP

#include "../common.hpp"
#include "chunk/chunk.hpp"
#include "scanner/scanner.hpp"
#include "utilities/tokens.hpp"

#include <stdexcept>

struct CompilerError : public std::runtime_error {
    Token tok_;
    CompilerError(std::string message, Token tok)
        : std::runtime_error{message.c_str()}, tok_(tok) {}
};

struct Compiler {
    Compiler(std::string source);

    void compile();
    void expression();
    void consume(TokenType tok_t, std::string message);
    Token advance();
    Token previous();
    void check_error();
    void report_error(CompilerError &error);
    bool is_end();
    Token &peek();
    std::string source_;
    Chunk chunk_;
    std::vector<Token> toks_;
    int current_{0};
};

#endif