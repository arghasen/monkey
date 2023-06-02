#include "parser.hpp"
#include "ast.hpp"
#include <memory>

namespace monkey{
namespace parser{

Parser::Parser(lexer::Lexer* l) : l(l) {
    nextToken();
    nextToken();
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
            return nullptr;
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


} // namespace parser
} // namespace monkey
