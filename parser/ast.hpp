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
  virtual std::string TokenLiteral() const = 0;
};

class Statement : public Node {
public:
  virtual ~Statement() = default;
  virtual std::string TokenLiteral() const = 0;
};

class Expression : public Node {
public:
  virtual ~Expression() = default;
  virtual std::string TokenLiteral() const = 0;
};

class Program : public Node {
public:
  Program() = default;
  ~Program() override = default;
  std::string TokenLiteral() const override;
  std::vector<std::unique_ptr<Statement>> statements;
};

class Identifier : public Expression {
public:
  Identifier() = default;
  explicit Identifier(lexer::Token tok);
  ~Identifier() override = default;
  std::string TokenLiteral() const override;
  lexer::Token token;
  std::string value;
};

class LetStatement : public Statement {
public:
  LetStatement() = default;
  explicit LetStatement(lexer::Token tok);
  ~LetStatement() override = default;
  std::string TokenLiteral() const override;
  lexer::Token token;
  std::unique_ptr<Identifier> name;
  std::unique_ptr<Expression> value;
};

} // namespace ast
} // namespace parser
} // namespace monkey
