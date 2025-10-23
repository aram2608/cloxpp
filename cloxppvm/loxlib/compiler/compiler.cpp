#include "compiler/compiler.hpp"

#include "compiler.hpp"

Compiler::Compiler(std::string source) : source_{source} {}

void Compiler::compile() {
    // We create a scanner instance and populate our tokens vector
    Scanner scanner = Scanner(source_);
    toks_ = scanner.scan_tokens();
    // We can then compile the tokens into bytecode for usage in the vm
    while (!is_end()) {
        try {
            expression();
        } catch (CompilerError &e) {
            report_error(e);
        }
    }
    //consume(TokenType::eof, "Expeected the end of an expression.");
}

void Compiler::expression() {
    if (!is_end()) {
        fmt::println("{}", advance().type_);
    }
}

void Compiler::consume(TokenType tok_t, std::string message) {
    if (peek().type_ == tok_t) {
        advance();
        return;
    }

    throw CompilerError{message, peek()};
}

Token Compiler::advance() {
    if (!is_end()) {
        check_error();
        return toks_.at(++current_);
    }
}

Token Compiler::previous() { return toks_.at(current_ - 1); }

void Compiler::check_error() {
    if (toks_.at(current_ + 1).type_ == TokenType::ERROR) {
        throw CompilerError{"Compiler error", toks_.at(current_ + 1)};
    }
}

void Compiler::report_error(CompilerError &error) {
    fmt::println("{} at {}", error.what(), error.tok_.line_);
}

bool Compiler::is_end() { return peek().type_ == TokenType::eof; }

Token &Compiler::peek() { return toks_.at(current_); }