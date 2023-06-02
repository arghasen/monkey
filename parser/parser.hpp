#pragma once

#include "../lexer/lexer.hpp"
#include "ast.hpp"
#include <memory>

namespace monkey {
namespace parser {

class Parser {
public:
  explicit Parser(lexer::Lexer *l);
  ~Parser() = default;

  void nextToken();
  std::unique_ptr<ast::Program> parseProgram();
  
private:
  std::unique_ptr<ast::Statement> parseStatement();
  std::unique_ptr<ast::LetStatement> parseLetStatement();
  bool expectPeek(lexer::TokenType type);

  lexer::Lexer *l;
  lexer::Token curToken;
  lexer::Token peekToken;
};

} // namespace parser
} // namespace monkey
