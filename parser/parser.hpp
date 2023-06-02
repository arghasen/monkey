#pragma once

#include "../lexer/lexer.hpp"
#include "ast.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace monkey {
namespace parser {
class Parser;

using PrefixParseFn = std::function<std::unique_ptr<ast::Expression>()>;
using InfixParseFn = std::function<std::unique_ptr<ast::Expression>(
                                                                   std::unique_ptr<ast::Expression>)>;

enum class Precedence{
  LOWEST,
  EQUALS, // ==
  LESSGREATER, // > or <
  SUM, // +
  PRODUCT, // *
  PREFIX, // -X or !X
  CALL, // myFunction(X)
  INDEX // array[index]
};

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
  std::unique_ptr<ast::ReturnStatement> parseReturnStatement();
  std::unique_ptr<ast::ExpressionStatement> parseExpressionStatement();
  std::unique_ptr<ast::Expression> parseExpression(Precedence precedence);
  std::unique_ptr<ast::Expression> parseIdentifier();
  std::unique_ptr<ast::Expression> parseIntegerLiteral();
  std::unique_ptr<ast::Expression> parsePrefixExpression();

  void noPrefixParseFnError(lexer::TokenType type);
  
  bool expectPeek(lexer::TokenType type);
  bool curTokenIs(lexer::TokenType type);
  bool peekTokenIs(lexer::TokenType type);
  void peekError(lexer::TokenType type);
  void registerPrefix(lexer::TokenType type, auto fn);
  void registerInfix(lexer::TokenType type, auto fn);

  lexer::Lexer *l;
  lexer::Token curToken;
  lexer::Token peekToken;
  std::vector<std::string> errors;
  std::unordered_map<lexer::TokenType, PrefixParseFn> prefixParseFns;
  std::unordered_map<lexer::TokenType, InfixParseFn> infixParseFns;

};



} // namespace parser
} // namespace monkey
