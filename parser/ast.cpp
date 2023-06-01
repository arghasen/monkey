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

std::string Identifier::TokenLiteral() const { return token.Literal; }

std::string LetStatement::TokenLiteral() const { return token.Literal; }

} // namespace ast
} // namespace parser
} // namespace monkey
