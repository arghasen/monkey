#include "parser.hpp"
#include "ast.hpp"

namespace monkey{
namespace parser{

Parser::Parser(lexer::Lexer* l) : l(l) {
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = l->NextToken();
}

ast::Program* Parser::ParseProgram() {
    // while (curToken.Type != lexer::EOFILE) {
    //     nextToken();
    // }
    return nullptr;
}

} // namespace parser
} // namespace monkey
