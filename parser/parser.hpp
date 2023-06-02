#pragma once

#include "../lexer/lexer.hpp"
#include "ast.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace monkey {
namespace parser {
class Parser;

using PrefixParseFn = std::function<std::unique_ptr<ast::Expression>()>;
using InfixParseFn = std::function<std::unique_ptr<ast::Expression>(
    std::unique_ptr<ast::Expression>)>;
using Expression = std::unique_ptr<ast::Expression>;
using Errors = std::vector<std::string>;

enum class Precedence {
  LOWEST,
  EQUALS,      // ==
  LESSGREATER, // > or <
  SUM,         // +
  PRODUCT,     // *
  PREFIX,      // -X or !X
  CALL,        // myFunction(X)
  INDEX        // array[index]
};

const std::unordered_map<lexer::TokenType, Precedence> precedences = {
    {lexer::TokenType::EQ, Precedence::EQUALS},
    {lexer::TokenType::NOT_EQ, Precedence::EQUALS},
    {lexer::TokenType::LT, Precedence::LESSGREATER},
    {lexer::TokenType::GT, Precedence::LESSGREATER},
    {lexer::TokenType::PLUS, Precedence::SUM},
    {lexer::TokenType::MINUS, Precedence::SUM},
    {lexer::TokenType::SLASH, Precedence::PRODUCT},
    {lexer::TokenType::ASTERISK, Precedence::PRODUCT},
};

class Parser {
public:
  explicit Parser(lexer::Lexer *l);
  ~Parser() = default;

  void nextToken();
  std::unique_ptr<ast::Program> parseProgram();
  Errors getErrors() const;

private:
  std::unique_ptr<ast::Statement> parseStatement();
  std::unique_ptr<ast::LetStatement> parseLetStatement();
  std::unique_ptr<ast::ReturnStatement> parseReturnStatement();
  std::unique_ptr<ast::ExpressionStatement> parseExpressionStatement();
  Expression parseExpression(Precedence precedence);
  Expression parseIdentifier();
  Expression parseIntegerLiteral();
  Expression parsePrefixExpression();
  Expression parseInfixExpression(Expression left);
  Expression parseBoolean();
  Expression parseGroupedExpression();
  Expression parseIfExpression();
  std::unique_ptr<ast::BlockStatement> parseBlockStatement();

  void noPrefixParseFnError(lexer::TokenType type);

  Precedence peekPrecedence();
  Precedence curPrecedence();

  bool expectPeek(lexer::TokenType type);
  bool curTokenIs(lexer::TokenType type);
  bool peekTokenIs(lexer::TokenType type);
  void peekError(lexer::TokenType type);
  void registerPrefix(lexer::TokenType type, auto fn);
  void registerInfix(lexer::TokenType type, auto fn);

  lexer::Lexer *l;
  lexer::Token curToken;
  lexer::Token peekToken;
  Errors errors;
  std::unordered_map<lexer::TokenType, PrefixParseFn> prefixParseFns;
  std::unordered_map<lexer::TokenType, InfixParseFn> infixParseFns;
};

} // namespace parser
} // namespace monkey
