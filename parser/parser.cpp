#include "parser.hpp"
#include "ast.hpp"
#include <memory>

namespace monkey{
namespace parser{

Parser::Parser(lexer::Lexer* l) : l(l) {
    nextToken(); // set curToken
    nextToken(); // set peekToken

    registerPrefix(lexer::TokenType::IDENT, &Parser::parseIdentifier);
    registerPrefix(lexer::TokenType::INT, &Parser::parseIntegerLiteral);
}

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = l->nextToken();
}

std::unique_ptr<ast::Program> Parser::parseProgram() {
    auto program = std::make_unique<ast::Program>();

    while (not curTokenIs(lexer::TokenType::EOFILE)) {
        auto statement = parseStatement();
        if(statement != nullptr){
            program->statements.push_back(std::move(statement));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<ast::Statement> Parser::parseStatement(){
    switch(curToken.type){
        case lexer::TokenType::LET:
            return parseLetStatement();
        case lexer::TokenType::RETURN:
            return parseReturnStatement();
        default:
            return parseExpressionStatement();
    }
    return nullptr;
}

std::unique_ptr<ast::LetStatement> Parser::parseLetStatement(){
    auto letstatement = std::make_unique<ast::LetStatement>(curToken);
    if(!expectPeek(lexer::TokenType::IDENT)){
        return nullptr;
    }
    letstatement->name = std::make_unique<ast::Identifier>(curToken);
    if(!expectPeek(lexer::TokenType::ASSIGN)){
        return nullptr;
    }
    // Todo: We're skipping the expressions until we encounter a semicolon
    while(not curTokenIs(lexer::TokenType::SEMICOLON)){
        nextToken();
    }
    return letstatement;
}

std::unique_ptr<ast::ReturnStatement> Parser::parseReturnStatement(){
    auto returnStatement = std::make_unique<ast::ReturnStatement>(curToken);
    nextToken();
    // Todo: We're skipping the expressions until we encounter a semicolon
    while(not curTokenIs(lexer::TokenType::SEMICOLON)){
        nextToken();
    }
    return returnStatement;
}

std::unique_ptr<ast::ExpressionStatement> Parser::parseExpressionStatement(){
    auto expressionStatement = std::make_unique<ast::ExpressionStatement>(curToken);
    expressionStatement->expression = parseExpression(Precedence::LOWEST);
    if(peekTokenIs(lexer::TokenType::SEMICOLON)){
        nextToken();
    }
    return expressionStatement;
}

std::unique_ptr<ast::Expression> Parser::parseExpression(Precedence precedence){
    auto prefix = prefixParseFns[curToken.type];
    if(prefix == nullptr){
        return nullptr;
    }
    auto leftExp = prefix();
    return leftExp;
}

std::unique_ptr<ast::Expression> Parser::parseIdentifier(){
    return std::make_unique<ast::Identifier>(curToken);
}

std::unique_ptr<ast::Expression> Parser::parseIntegerLiteral(){
    auto literal = std::make_unique<ast::IntegerLiteral>(curToken);
    int value = 0;
    try{
        value = std::stoi(curToken.literal);
    }catch(std::invalid_argument& e){
        std::string msg = "could not parse " + curToken.literal + " as integer";
        errors.push_back(msg);
        return nullptr;
    }
    literal->value = value;
    return literal;
}

bool Parser::curTokenIs(lexer::TokenType type){
    return curToken.type == type;
}

bool Parser::peekTokenIs(lexer::TokenType type){
    return peekToken.type == type;
}

bool Parser::expectPeek(lexer::TokenType type){
    if(peekToken.type == type){
        nextToken();
        return true;
    }else{
        peekError(type);
        return false;
    }
}

std::vector<std::string> Parser::getErrors() const{
    return errors;
}

void Parser::peekError(lexer::TokenType type){
    std::string msg = "expected next token to be " + lexer::to_string(type) + " got " + lexer::to_string(peekToken.type) + " instead";
    errors.push_back(msg);
}

void Parser::registerPrefix(lexer::TokenType type, auto fn){
    prefixParseFns[type] = std::bind(fn, this);
}

void Parser::registerInfix(lexer::TokenType type, auto fn){
    infixParseFns[type] = std::bind(fn, this);
}

} // namespace parser
} // namespace monkey
