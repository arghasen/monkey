#pragma once

#include "../lexer/lexer.hpp"
#include "ast.hpp"

namespace monkey {
namespace parser {

class Parser {
public:
  explicit Parser(lexer::Lexer *l);
  ~Parser() = default;

  void nextToken();
  ast::Program *ParseProgram();

private:
  lexer::Lexer* l;
  lexer::Token curToken;
  lexer::Token peekToken;
};

} // namespace parser
} // namespace monkey
