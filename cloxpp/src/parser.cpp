#include "parser.hpp"

using namespace CppLox;
using std::initializer_list;
using std::shared_ptr;
using std::vector;

// Constructor for Parser class
// We take in a vector of tokens to consume
Parser::Parser(vector<Token> tokens) {
    this->tokens = tokens;
}

// Function to parse code
vector<shared_ptr<Stmt>> Parser::parse() {
    // We create a new vector to store pointers to our statements
    vector<shared_ptr<Stmt>> stmts;
    while (!is_end()) {
        stmts.push_back(declaration());
    }
    return stmts;
}

// Function for handling declarations
shared_ptr<Stmt> Parser::declaration() {
    try {
        // We match a var keyword and return var_declaration
        if (match({TokenType::VAR})) {
            return var_declaration();
        }

        // We match a function declaration
        if (match({TokenType::FUN})) {
            return function("function");
        }

        // By default we return a statement
        return statement();
        // catch erros and get the parser to chill out
    } catch (ParseError error) {
        synchronize();
        // Since we are returning a pointer type
        // we need to return nullptr instead of {}
        return nullptr;
    }
}

// Function to handle var_declar
shared_ptr<Stmt> Parser::var_declaration() {
    // Match an identifier token and consume it
    Token identifier = consume(TokenType::IDENTIFIER, "Expected identifier.");

    // We initialize a value with nullptr
    shared_ptr<Expr> initializer = nullptr;
    // If we match an equal token bind initializer to the output of expression
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    // We match a semicolon and throw an error if the statement is not closed
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<Var>(std::move(identifier), std::move(initializer));
}

// Function to handle parsing of statements
// Lox programs are a series of statements so
// all scripts start here defined by our grammar rules
shared_ptr<Stmt> Parser::statement() {
    // We have a match case for each keyword
    if (match({TokenType::IF}))
        return if_statement();
    if (match({TokenType::PRINT})) {
        return print_statement();
    }

    if (match({TokenType::WHILE})) {
        return while_statement();
    }

    if (match({TokenType::RETURN})) {
        return return_statement();
    }

    if (match({TokenType::FOR})) {
        return for_statement();
    }

    if (match({TokenType::LEFT_BRACE})) {
        // We need to move ownership
        return std::make_shared<Block>(std::move(block()));
    }

    // If we dont reach the predefined stmt types return a base expression stmt
    return expression_statement();
}

// Function to match if statement
shared_ptr<Stmt> Parser::if_statement() {
    // We need to consume the left parenthesis, if statements must use them
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    // we then save the expression inside the parenthesis
    shared_ptr<Expr> condition = expression();
    // We consume the right parenthesis for the same reason
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    // we can then catch any statements for the then clause and default to
    // a nullptr for the else clause
    shared_ptr<Stmt> then_branch = statement();
    shared_ptr<Stmt> else_branch = nullptr;
    /*
     * If we match an else keyword we can add the procdeding statement
     * we avoid the else-problem by immediately attaching the else clause to the
     * related if statement
     *
     * if (maybe()) <- this owns else clause #2
     *   if (sometimes()) <- this owns else clause #1
     *   else (dont()) // #1
     * else(yes()) // #2
     */
    if (match({TokenType::ELSE})) {
        else_branch = statement();
    }

    // We return a shared_ptr to the statement, we must move ownership
    return std::make_shared<IfStmt>(
        std::move(condition), std::move(then_branch), std::move(else_branch));
}

// Function to create a while statement
shared_ptr<Stmt> Parser::while_statement() {
    // we consume the first closing parenthesis
    // and we save the expression
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    shared_ptr<Expr> condition = expression();
    // we consume the last parenethesis, this is important as it signals
    // the end of the expression
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    // we can now save the underlying statement
    shared_ptr<Stmt> body = statement();

    return std::make_shared<WhileStmt>(std::move(condition), std::move(body));
}

// Logic for handling for statements
shared_ptr<Stmt> Parser::for_statement() {
    // We consume the first parenethesis
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    // We declare our initialier stmt
    shared_ptr<Stmt> initializer;
    // If we skip past it, we can just defer to a nullptr
    if (match({TokenType::SEMICOLON})) {
        initializer = nullptr;
        // if we match a var, we can add a variable declaration
    } else if (match({TokenType::VAR})) {
        initializer = var_declaration();
        // otherwise, we just pass the expression
    } else {
        initializer = expression_statement();
    }

    // We can now take our condition, we start with a nullptr
    shared_ptr<Expr> condition = nullptr;
    // if we do not match a semicolon, we can save the expression
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }

    // otherwise we continue parsing and consume the semicolon
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    // now we can check for an increment
    shared_ptr<Expr> increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
    // we can now take the statement body
    shared_ptr<Stmt> body = statement();

    // We can now check if our increment is null
    // and replace our statement with a block instead
    if (increment != nullptr) {
        // we create our vector of pointers to statements
        // and create our expression statement
        vector<shared_ptr<Stmt>>   body_stmt;
        shared_ptr<ExpressionStmt> expr = std::make_shared<ExpressionStmt>(std::move(increment));

        // now we can move ownership to the the vector
        body_stmt.push_back(std::move(body));
        body_stmt.push_back(std::move(expr));
        body = std::make_shared<Block>(std::move(body_stmt));
    }

    // If the condition is nullptr we cram a true in to
    // make an infinite while loop
    if (condition == nullptr) {
        condition = std::make_shared<Literal>(true);
    }
    // we create said while loop
    body = std::make_shared<WhileStmt>(std::move(condition), std::move(body));

    // if we come across an initalizer, we run it once
    // and then pass in a final block statement
    if (initializer != nullptr) {
        vector<shared_ptr<Stmt>> body_stmt;
        body_stmt.push_back(std::move(initializer));
        body_stmt.push_back(std::move(body));
        body = std::make_shared<Block>(std::move(body_stmt));
    }

    return body;
}

// Function to handel Lox's built in print statement
shared_ptr<Stmt> Parser::print_statement() {
    // We create our base expression
    shared_ptr<Expr> value = expression();
    // We then consume the semicolon and toss an error if the statement was not
    // finished
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    // We wrap our expression in a statement and return it
    return std::make_shared<Print>(std::move(value));
}

// Function to handle return statement logic
shared_ptr<Stmt> Parser::return_statement() {
    // we save the keyword
    Token keyword = previous();
    // we initialize our value as a nullptr
    shared_ptr<Expr> value = nullptr;
    // if we do not match a semicolon as the proceeding token
    // we return an expression
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }

    // we check for a semicolon and throw an error otherwise
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_shared<ReturnStmt>(std::move(keyword), std::move(value));
}

shared_ptr<Function> Parser::function(std::string kind) {
    // we consume the first token and throw an error if we do not come across a
    // name
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    // we now consume the first parenthesis and throw an error if not present
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    // we initialize a vector of tokens
    vector<Token> parameters;
    // we check for the right parenthesis
    if (!check(TokenType::RIGHT_PAREN)) {
        // and we add parameters as long as we match a comme token
        // if we encounter more than 255 arguments we throw an error
        do {
            if (parameters.size() >= 255) {
                error(peek(), "Can't have more than 255 parameters.");
            }
            parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    // we consume the first brace and kick an error, block assumes the first
    // brace token has already been matched
    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    vector<shared_ptr<Stmt>> body = block();

    return std::make_shared<Function>(std::move(name), std::move(parameters), std::move(body));
}

// Function to handle expression statement
shared_ptr<Stmt> Parser::expression_statement() {
    // Creates our base expressions
    shared_ptr<Expr> expr = expression();
    // Consume the end of the statement and toss an error otherwise
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    // We wrap our expression in the Expression statement and return it
    return std::make_shared<ExpressionStmt>(std::move(expr));
}

// Function to handle block scopes
vector<shared_ptr<Stmt>> Parser::block() {
    // we create a list of statements
    vector<shared_ptr<Stmt>> stmts;

    // While we have not reached a right base add declarations
    while (!check(TokenType::RIGHT_BRACE) && !is_end()) {
        stmts.push_back(declaration());
    }

    // If a right brace is never reached toss an error
    // If it is we consume it
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    // We can return our list of statments
    return stmts;
}

// Function to handle the parsing of expressions
shared_ptr<Expr> Parser::expression() {
    return assignment();
}

/*
 * Function to handle parsing assignments
 * This is fairly tricky as we are using a single token recursive descent parser
 * meaning we can not look far enough ahead to know we are parsing an assignment
 * until after we reach the =
 *
 * var a = "before";
 * a = "value";
 *
 * In this expression if we evaluate 'a' we would return "before" which is not
 * what we want This is the difference between an rvalue and an lvalue, an
 * lvalue is a storage location for a value while an rvalue is simply a
 * transient value we have evaluated
 */
shared_ptr<Expr> Parser::assignment() {
    // We first need to initialize our expression
    std::shared_ptr<Expr> expr = _or();

    // We need to match and =
    if (match({TokenType::EQUAL})) {
        // If we match an = sign we need to save the token
        Token equals = previous();
        // We call assignment to recursivle parse the right hand side
        std::shared_ptr<Expr> value = assignment();

        /*
         * We try and cast the expression, if it works we extract the token
         * and create a new assignment node
         * if it fails it returns a nullptr and that is falsy in C++
         * In the process of this we convert the rvalue expression to an lvalue
         * expression
         */
        if (Variable* var = dynamic_cast<Variable*>(expr.get())) {
            Token identifier = var->identifier;
            return std::make_shared<Assign>(std::move(identifier), std::move(value));
        }

        error(equals, "Invalid assignment target.");
    }
    // We return the expression
    return expr;
}

// Function to handle logical operations
shared_ptr<Expr> Parser::_or() {
    // We first assign an expression to an and expression
    shared_ptr<Expr> expr = _and();

    // If we match the or keyword, we add a token and the right most expression
    // through the _and() method
    while (match({TokenType::OR})) {
        Token            op    = previous();
        shared_ptr<Expr> right = _and();
        // we create a new pointer to a Logical node and move ownership
        expr = std::make_shared<Logical>(std::move(expr), std::move(op), std::move(right));
    }

    // We then can return the final expression
    return expr;
}

shared_ptr<Expr> Parser::_and() {
    // We first assign an expression to an and expression
    shared_ptr<Expr> expr = equality();

    // If we match the and keyword, we add a token and the right most expression
    // through the equality() method
    while (match({TokenType::AND})) {
        Token            op    = previous();
        shared_ptr<Expr> right = equality();
        // we create a new pointer to a Logical node and move ownership
        expr = std::make_shared<Logical>(std::move(expr), std::move(op), std::move(right));
    }

    // We then can return the final expression
    return expr;
}

// Function to handle second grammar rule,
shared_ptr<Expr> Parser::equality() {
    // We first need to match a comparison
    shared_ptr<Expr> expr = comparison();

    // then enter a while loop for the (...) rule
    // We search for a != or a == then break, we do this with the match method
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        // We consume tokens and expressions until we break the loop
        Token            op    = previous();
        shared_ptr<Expr> right = comparison();
        // We have to move since shared_ptrs are read only
        expr = std::make_shared<Binary>(std::move(expr), op, std::move(right));
    }
    // We return the final expression
    return expr;
}

// Function to handle comparison operatios
shared_ptr<Expr> Parser::comparison() {
    shared_ptr<Expr> expr = term();
    // We look through each comparison type and break when
    // we no longer come across a comparison operator
    while (match(
        {TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token            op    = previous();
        shared_ptr<Expr> right = term();
        // We need to move ownership to the new expression
        expr = std::make_shared<Binary>(std::move(expr), op, std::move(right));
    }
    // We return the final expression
    return expr;
}

shared_ptr<Expr> Parser::term() {
    shared_ptr<Expr> expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token            op    = previous();
        shared_ptr<Expr> right = factor();
        // We need to move ownership to the new expression
        expr = std::make_shared<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// Function to visit factor node
shared_ptr<Expr> Parser::factor() {
    // We initialize our first expression
    shared_ptr<Expr> expr = unary();
    // Loop until we dont come across multi or division anymore
    while (match({TokenType::STAR, TokenType::SLASH})) {
        Token            op    = previous();
        shared_ptr<Expr> right = unary();
        // We need to move ownership to the new expression
        expr = std::make_shared<Binary>(std::move(expr), op, std::move(right));
    }

    // Return og expr otherwise
    return expr;
}

// Function to visit the unary operation node
shared_ptr<Expr> Parser::unary() {
    // We can match either the - or ! operators
    if (match({TokenType::BANG, TokenType::MINUS})) {
        // We store the token types and make a new unary node
        Token            op    = previous();
        shared_ptr<Expr> right = unary();
        return std::make_shared<Unary>(op, std::move(right));
    }
    return call();
}

shared_ptr<Expr> Parser::finish_call(shared_ptr<Expr> callee) {
    // By default we set a match number of args
    const size_t MAX_ARGS = 255;

    // we create our vector of unique ptrs to our arguments
    vector<shared_ptr<Expr>> args;
    // we check if we have met a right parenthesis
    if (!check(TokenType::RIGHT_PAREN)) {
        // if we havent we continuously push_back arguments after matching a
        // comma
        do {
            if (args.size() >= MAX_ARGS) {
                error(peek(), "Can't have more than 255 arguments.");
            }
            args.push_back(std::move(expression()));
        } while (match({TokenType::COMMA}));
    }

    // we consume the closing parenthesis
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

    // we return our call node
    return std::make_shared<Call>(std::move(callee), paren, std::move(args));
}

shared_ptr<Expr> Parser::call() {
    // we initialize our expr from primary
    shared_ptr<Expr> expr = primary();

    // we continuously match left parenethesis and
    // call the finish_call method
    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finish_call(std::move(expr));
            // we break as soon as we dont match any more opening
            // parenthesis
        } else {
            break;
        }
    }
    // we then return our expression
    return expr;
}

// Function to handle the atomic units of Lox
shared_ptr<Expr> Parser::primary() {
    // Booleans
    if (match({TokenType::FALSE}))
        return std::make_shared<Literal>(false);
    if (match({TokenType::TRUE}))
        return std::make_shared<Literal>(true);
    // Null value
    if (match({TokenType::NIL}))
        return std::make_shared<Literal>(nullptr);

    // Strings and nums
    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_shared<Literal>(previous().literal);
    }

    // Identifiers
    if (match({TokenType::IDENTIFIER})) {
        return std::make_shared<Variable>(previous());
    }

    // Parenthesis
    if (match({TokenType::LEFT_PAREN})) {
        shared_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Grouping>(std::move(expr));
    }
    // Catch bad tokens
    throw error(peek(), "Expect expression.");
}

// Helper function to match token types
bool Parser::match(initializer_list<TokenType> types) {
    // Iterate through each item in list
    for (TokenType type : types) {
        // Check and consume
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

// Function to a consume a token and return it
// Useful for closing off statements or parenthesis, blocks, etc
Token Parser::consume(TokenType type, std::string message) {
    if (check(type))
        return advance();

    throw error(peek(), message);
}

// Function to return true if the current token is of the given type
bool Parser::check(TokenType type) {
    if (is_end())
        return false;
    return peek().type == type;
}

// Function consumes the current token and returns it
// Similar to scanner's advance()
Token Parser::advance() {
    if (!is_end())
        current++;
    return previous();
}

// Function to check if we have reach the EOF token
bool Parser::is_end() {
    return peek().type == TokenType::eof;
}
/*
 * Function to peek at the current token
 * We return a read only reference since we don't
 * want an expensive copy everytime
 */
Token Parser::peek() {
    return tokens.at(current);
}

Token Parser::previous() {
    return tokens.at(current - 1);
}

Parser::ParseError Parser::error(Token token, std::string message) {
    errors.error(token, message);
    return ParseError{""};
}

void Parser::synchronize() {
    advance();

    while (!is_end()) {
        if (previous().type == TokenType::SEMICOLON)
            return;

        /*
         * We synchronize on statements in lox
         * After a semicolon, we can assume that we
         * have started a new statement
         */
        switch (peek().type) {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
            return;
        }

        advance();
    }
}