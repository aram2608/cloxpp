// Function to handle multiple expressions separated by commas
unique_ptr<Expr> Parser::comma() {
    unique_ptr<Expr> expr = conditional();

    while (match({TokenType::COMMA})) {
        Token            op    = previous();
        unique_ptr<Expr> right = conditional();
        expr                   = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

unique_ptr<Expr> Parser::conditional() {
    unique_ptr<Expr> expr = equality();

    if (match({TokenType::QUESTION})) {
        Token            op    = previous();
        unique_ptr<Expr> right = expression();
        expr                   = std::make_unique<Binary>(std::move(expr), op, std::move(right));
        if (match({TokenType::COLON})) {
            Token            op    = previous();
            unique_ptr<Expr> right = conditional();
            expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
            return expr;
        } else {
            throw error(peek(), "Expected ':'.");
        }
    }

    return expr;
}