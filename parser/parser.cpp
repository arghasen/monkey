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

    while (curToken.Type != lexer::TokenType::EOFILE) {
        auto statement = parseStatement();
        if(statement != nullptr){
            program->statements.push_back(std::move(statement));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<ast::Statement> Parser::parseStatement(){
    switch(curToken.Type){
        case lexer::TokenType::LET:
            return parseLetStatement();
        default:
            return nullptr;
    }
    return nullptr;
}

std::unique_ptr<ast::LetStatement> Parser::parseLetStatement(){
    auto letstatement = std::make_unique<ast::LetStatement>(curToken);
    return nullptr;
}
} // namespace parser
} // namespace monkey
