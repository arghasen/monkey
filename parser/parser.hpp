#pragma once

#include "../lexer/lexer.hpp"
#include "ast.hpp"
#include <memory>
#include <string>
#include <vector>

namespace monkey {
namespace parser {

class Parser {
public:
  explicit Parser(lexer::Lexer *l);
  ~Parser() = default;

  void nextToken();
  std::unique_ptr<ast::Program> parseProgram();
  std::vector<std::string> getErrors() const;

private:
  std::unique_ptr<ast::Statement> parseStatement();
  std::unique_ptr<ast::LetStatement> parseLetStatement();
  bool expectPeek(lexer::TokenType type);
bool curTokenIs(lexer::TokenType type);
bool peekTokenIs(lexer::TokenType type);
void peekError(lexer::TokenType type);

  lexer::Lexer *l;
  lexer::Token curToken;
  lexer::Token peekToken;
  std::vector<std::string> errors;
};

} // namespace parser
} // namespace monkey
