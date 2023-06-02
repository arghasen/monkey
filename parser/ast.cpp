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
std::string ReturnStatement::TokenLiteral() const { return token.literal; }

LetStatement::LetStatement(lexer::Token tok) : token(tok) {}
Identifier::Identifier(lexer::Token tok) : token(tok), value(tok.literal) {}
ReturnStatement::ReturnStatement(lexer::Token tok) : token(tok) {}

std::string Program::to_string() const {
  std::string out;
  for (const auto &stmt : statements) {
    out += stmt->to_string();
  }
  return out;
}

std::string Identifier::to_string() const { return value; }
std::string LetStatement::to_string() const {
  std::string out = token.literal + " " + name->to_string() + " = ";
  if (value) {
    out += value->to_string();
  }
  out += ";";
  return out;
}

std::string ReturnStatement::to_string() const {
  std::string out = token.literal + " ";
  if (returnValue) {
    out += returnValue->to_string();
  }
  out += ";";
  return out;
}

std::string ExpressionStatement::to_string() const {
  if (expression) {
    return expression->to_string();
  }
  return "";
}

} // namespace ast
} // namespace parser
} // namespace monkey
