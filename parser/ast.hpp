#pragma once

#include "../lexer/token.hpp"
#include <memory>
#include <string>
#include <vector>
namespace monkey::parser::ast {

class Identifier;
class Expression;
using Parameters = std::vector<std::unique_ptr<Identifier>>;
using Arguments = std::vector<std::unique_ptr<Expression>>;

enum class StatementType {
  LET,
  RETURN,
  EXPRESSION,
  BLOCK,
};

enum class ExpressionType {
  IDENTIFIER,
  INTEGER,
  BOOLEAN,
  PREFIX,
  INFIX,
  IF,
  FUNCTION,
  CALL,
  STRING,
  ARRAY,
};

class Node {
public:
  virtual ~Node() = default;
  virtual std::string to_string() const = 0;
  virtual std::string TokenLiteral() const = 0;

protected:
  Node() = default;
};

class Statement : public Node {
public:
  virtual ~Statement() = default;
  std::string to_string() const override = 0;
  std::string TokenLiteral() const override;
  lexer::Token token;
  constexpr virtual StatementType Type() const = 0;

protected:
  explicit Statement(lexer::Token tok);
};

using Statements = std::vector<std::unique_ptr<Statement>>;

class Expression : public Node {
public:
  virtual ~Expression() = default;
  std::string to_string() const override;
  std::string TokenLiteral() const override;
  lexer::Token token;
  constexpr virtual ExpressionType Type() const = 0;

protected:
  explicit Expression(lexer::Token tok);
};

class Program : public Node {
public:
  Program() = default;
  ~Program() override = default;
  std::string to_string() const override;
  std::string TokenLiteral() const override;
  Statements statements;
};

class Identifier : public Expression {
public:
  explicit Identifier(lexer::Token tok);
  ~Identifier() override = default;
  std::string to_string() const override;
  constexpr ExpressionType Type() const override{
    return ExpressionType::IDENTIFIER;
  }
  std::string value;
};

class LetStatement : public Statement {
public:
  explicit LetStatement(lexer::Token tok);
  ~LetStatement() override = default;
  std::string to_string() const override;
  constexpr StatementType Type() const override{
    return StatementType::LET;
  }
  std::unique_ptr<Identifier> name;
  std::unique_ptr<Expression> value;
};

class ReturnStatement : public Statement {
public:
  explicit ReturnStatement(lexer::Token tok);
  ~ReturnStatement() override = default;
  std::string to_string() const override;
  constexpr StatementType Type() const override{
    return StatementType::RETURN;
  }
  std::unique_ptr<Expression> returnValue;
};

class ExpressionStatement : public Statement {
public:
  explicit ExpressionStatement(lexer::Token tok);
  ~ExpressionStatement() override = default;
  std::string to_string() const override;
  constexpr StatementType Type() const override{
    return StatementType::EXPRESSION;
  }
  std::unique_ptr<Expression> expression;
};

class IntegerLiteral : public Expression {
public:
  explicit IntegerLiteral(lexer::Token tok);
  ~IntegerLiteral() override = default;
  constexpr ExpressionType Type() const override{
    return ExpressionType::INTEGER;
  }
  int64_t value;
};

class PrefixExpression : public Expression {
public:
  explicit PrefixExpression(lexer::Token tok);
  ~PrefixExpression() override = default;
  std::string to_string() const override;
  constexpr ExpressionType Type() const override{
    return ExpressionType::PREFIX;
  }
  std::string op;
  std::unique_ptr<Expression> right;
};

class InfixExpression : public Expression {
public:
  explicit InfixExpression(lexer::Token tok);
  ~InfixExpression() override = default;
  std::string to_string() const override;
  constexpr ExpressionType Type() const override{
    return ExpressionType::INFIX;
  }
  std::unique_ptr<Expression> left;
  std::string op;
  std::unique_ptr<Expression> right;
};

class Boolean : public Expression {
public:
  Boolean(lexer::Token tok, bool val);
  ~Boolean() override = default;
  constexpr ExpressionType Type() const override{
    return ExpressionType::BOOLEAN;
  }
  bool value;
};

class BlockStatement : public Statement {
public:
  explicit BlockStatement(lexer::Token tok);
  ~BlockStatement() override = default;
  std::string to_string() const override;
  constexpr StatementType Type() const override{
    return StatementType::BLOCK;
  }
  Statements statements;
};

class IfExpression : public Expression {
public:
  explicit IfExpression(lexer::Token tok);
  ~IfExpression() override = default;
  std::string to_string() const override;
  constexpr ExpressionType Type() const override{
    return ExpressionType::IF;
  }
  std::unique_ptr<Expression> condition;
  std::unique_ptr<BlockStatement> consequence;
  std::unique_ptr<BlockStatement> alternative;
};

class FunctionLiteral : public Expression {
public:
  explicit FunctionLiteral(lexer::Token tok);
  ~FunctionLiteral() override = default;
  std::string to_string() const override;
  constexpr ExpressionType Type() const override{
    return ExpressionType::FUNCTION;
  }
  Parameters parameters;
  std::unique_ptr<BlockStatement> body;
};

class CallExpression : public Expression {
public:
  explicit CallExpression(lexer::Token tok);
  ~CallExpression() override = default;
  std::string to_string() const override;
  constexpr ExpressionType Type() const override{
    return ExpressionType::CALL;
  }
  std::unique_ptr<Expression> function;
  Arguments arguments;
};

class StringLiteral : public Expression {
public:
  explicit StringLiteral(lexer::Token tok);
  ~StringLiteral() override = default;
  std::string to_string() const override;
  constexpr ExpressionType Type() const override{
    return ExpressionType::STRING;
  }
  std::string value;
};

class ArrayLiteral : public Expression {
public:
  explicit ArrayLiteral(lexer::Token tok);
  ~ArrayLiteral() override = default;
  std::string to_string() const override;
  constexpr ExpressionType Type() const override{
    return ExpressionType::ARRAY;
  }
  Arguments elements;
};


template <typename T>
concept AstNode = std::is_base_of<ast::Node, T>::value;
template <typename Y>
concept AstChildNode = std::is_same<Y, Statement>::value ||
                       std::is_same<Y, ast::Expression>::value;

template <typename T>
concept ExpressionNode = std::is_base_of<ast::Expression, T>::value;
} // namespace monkey::parser::ast
