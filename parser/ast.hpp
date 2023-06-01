#pragma once

#include "../lexer/token.hpp"

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
  std::vector<Statement *> Statements;
};

class Identifier : public Expression {
public:
  Identifier() = default;
  ~Identifier() override = default;
  std::string TokenLiteral() const override;
  lexer::Token token;
  std::string Value;
};

class LetStatement : public Statement {
public:
  LetStatement() = default;
  ~LetStatement() override = default;
  std::string TokenLiteral() const override;
  lexer::Token token;
  Identifier *Name;
  Expression *Value;
};

} // namespace ast
} // namespace parser
} // namespace monkey
