#pragma once

#include "../lexer/token.hpp"

#include <memory>
#include <string>
#include <vector>
namespace monkey {
namespace parser {
namespace ast {

class Node {
public:
  virtual ~Node() = default;
  virtual std::string to_string() const = 0;
  virtual std::string TokenLiteral() const = 0;
};

class Statement : public Node {
public:
  virtual ~Statement() = default;
  virtual std::string to_string() const = 0;
  virtual std::string TokenLiteral() const = 0;
};

class Expression : public Node {
public:
  virtual ~Expression() = default;
  virtual std::string to_string() const = 0;
  virtual std::string TokenLiteral() const = 0;
};

class Program : public Node {
public:
  Program() = default;
  ~Program() override = default;
  std::string TokenLiteral() const override;
  std::string to_string() const override;
  std::vector<std::unique_ptr<Statement>> statements;
};

class Identifier : public Expression {
public:
  Identifier() = default;
  explicit Identifier(lexer::Token tok);
  ~Identifier() override = default;
  std::string TokenLiteral() const override;
  std::string to_string() const override;
  lexer::Token token;
  std::string value;
};

class LetStatement : public Statement {
public:
  LetStatement() = default;
  explicit LetStatement(lexer::Token tok);
  ~LetStatement() override = default;
  std::string TokenLiteral() const override;
  std::string to_string() const override;
  lexer::Token token;
  std::unique_ptr<Identifier> name;
  std::unique_ptr<Expression> value;
};

class ReturnStatement : public Statement {
public:
  ReturnStatement() = default;
  explicit ReturnStatement(lexer::Token tok);
  ~ReturnStatement() override = default;
  std::string TokenLiteral() const override;
  std::string to_string() const override;
  lexer::Token token;
  std::unique_ptr<Expression> returnValue;
};

class ExpressionStatement : public Statement {
public:
  ExpressionStatement() = default;
  explicit ExpressionStatement(lexer::Token tok);
  ~ExpressionStatement() override = default;
  std::string TokenLiteral() const override;
  std::string to_string() const override;
  lexer::Token token;
  std::unique_ptr<Expression> expression;
};

class IntegerLiteral : public Expression {
public:
  IntegerLiteral() = default;
  explicit IntegerLiteral(lexer::Token tok);
  ~IntegerLiteral() override = default;
  std::string TokenLiteral() const override;
  std::string to_string() const override;
  lexer::Token token;
  int64_t value;
};

class PrefixExpression : public Expression {
public:
  PrefixExpression() = default;
  PrefixExpression(lexer::Token tok);
  ~PrefixExpression() override = default;
  std::string TokenLiteral() const override;
  std::string to_string() const override;
  lexer::Token token;
  std::string op;
  std::unique_ptr<Expression> right;
};

class InfixExpression : public Expression {
public:
  InfixExpression() = default;
  InfixExpression(lexer::Token tok);
  ~InfixExpression() override = default;
  std::string TokenLiteral() const override;
  std::string to_string() const override;
  lexer::Token token;
  std::unique_ptr<Expression> left;
  std::string op;
  std::unique_ptr<Expression> right;
};

template<typename Y>
concept AstNode = std::is_same<Y, Statement>::value || std::is_same<Y, ast::Expression>::value;

template<typename T>
concept ExpressionNode = std::is_base_of<ast::Expression, T>::value;
} // namespace ast
} // namespace parser
} // namespace monkey
