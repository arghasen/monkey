#include "ast.hpp"

namespace monkey {
namespace parser {
namespace ast {

std::string Program::TokenLiteral() const {
  if (statements.empty()) {
    return "";
  }
  return statements[0]->TokenLiteral();
}

std::string Identifier::TokenLiteral() const { return token.literal; }

std::string LetStatement::TokenLiteral() const { return token.literal; }

LetStatement::LetStatement(lexer::Token tok) : token(tok) {}
Identifier::Identifier(lexer::Token tok) : token(tok), value(tok.literal) {}

} // namespace ast
} // namespace parser
} // namespace monkey
