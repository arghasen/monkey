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
    registerPrefix(lexer::TokenType::BANG, &Parser::parsePrefixExpression);
    registerPrefix(lexer::TokenType::MINUS, &Parser::parsePrefixExpression);
    registerInfix(lexer::TokenType::PLUS, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::MINUS, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::SLASH, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::ASTERISK, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::EQ, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::NOT_EQ, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::LT, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::GT, &Parser::parseInfixExpression);
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

void Parser::noPrefixParseFnError(lexer::TokenType type){
    std::string msg = "no prefix parse function for " + lexer::to_string(type) + " found";
    errors.push_back(msg);
}

Expression Parser::parseExpression(Precedence precedence){
    auto prefix = prefixParseFns[curToken.type];
    if(prefix == nullptr){
        noPrefixParseFnError(curToken.type);
        return nullptr;
    }
    auto leftExp = prefix();

    while(not peekTokenIs(lexer::TokenType::SEMICOLON) and precedence < peekPrecedence()){
        auto infix = infixParseFns[peekToken.type];
        if(infix == nullptr){
            return leftExp;
        }
        nextToken();
        leftExp = infix(std::move(leftExp));
    }
    return leftExp;
}

Expression Parser::parseIdentifier(){
    return std::make_unique<ast::Identifier>(curToken);
}

Expression Parser::parseIntegerLiteral(){
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

Expression Parser::parsePrefixExpression(){
    auto expression = std::make_unique<ast::PrefixExpression>(curToken);
    nextToken();
    expression->right = parseExpression(Precedence::PREFIX);
    return expression;
}

Expression Parser::parseInfixExpression(Expression left){
    auto expression = std::make_unique<ast::InfixExpression>(curToken);
    expression->left = std::move(left);
    auto precedence = curPrecedence();
    nextToken();
    expression->right = parseExpression(precedence);
    return expression;
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

Errors Parser::getErrors() const{
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
    infixParseFns[type] = std::bind(fn, this, std::placeholders::_1);
}

Precedence Parser::peekPrecedence(){
    auto it = precedences.find(peekToken.type);
    if(it != precedences.end()){
        return it->second;
    }
    return Precedence::LOWEST;
}

Precedence Parser::curPrecedence(){
    auto it = precedences.find(curToken.type);
    if(it != precedences.end()){
        return it->second;
    }
    return Precedence::LOWEST;
}

} // namespace parser
} // namespace monkey
