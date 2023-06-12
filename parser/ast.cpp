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

std::string Statement::TokenLiteral() const { return token.literal; }

std::string Expression::TokenLiteral() const { return token.literal; }

Statement::Statement(lexer::Token tok) : token(tok) {}

Expression::Expression(lexer::Token tok) : token(tok) {}

LetStatement::LetStatement(lexer::Token tok) : Statement(tok) {}

Identifier::Identifier(lexer::Token tok)
    : Expression(tok), value(tok.literal) {}

ReturnStatement::ReturnStatement(lexer::Token tok) : Statement(tok) {}

ExpressionStatement::ExpressionStatement(lexer::Token tok)
    : Statement(tok), expression(nullptr) {}

IntegerLiteral::IntegerLiteral(lexer::Token tok) : Expression(tok) {}

PrefixExpression::PrefixExpression(lexer::Token tok)
    : Expression(tok), op(tok.literal), right(nullptr) {}

InfixExpression::InfixExpression(lexer::Token tok)
    : Expression(tok), left(nullptr), op(tok.literal), right(nullptr) {}

Boolean::Boolean(lexer::Token tok, bool val) : Expression(tok), value(val) {}

IfExpression::IfExpression(lexer::Token tok)
    : Expression(tok), condition(nullptr), consequence(nullptr),
      alternative(nullptr) {}

BlockStatement::BlockStatement(lexer::Token tok) : Statement(tok) {}

FunctionLiteral::FunctionLiteral(lexer::Token tok)
    : Expression(tok), body{nullptr} {}

CallExpression::CallExpression(lexer::Token tok)
    : Expression(tok), function{nullptr}, arguments{} {}

StringLiteral::StringLiteral(lexer::Token tok)
    : Expression(tok), value(tok.literal) {}

ArrayLiteral::ArrayLiteral(lexer::Token tok)
    : Expression(tok), elements{} {}

std::string Expression::to_string() const { return token.literal; }

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

std::string PrefixExpression::to_string() const {
  std::string out = "(" + op;
  if (right) {
    out += right->to_string();
  }
  out += ")";
  return out;
}

std::string InfixExpression::to_string() const {
  std::string out = "(";
  if (left) {
    out += left->to_string();
  }
  out += " " + op + " ";
  if (right) {
    out += right->to_string();
  }
  out += ")";
  return out;
}

std::string IfExpression::to_string() const {
  std::string out = "if";
  if (condition) {
    out += condition->to_string();
  }
  out += " " + consequence->to_string();
  if (alternative) {
    out += "else " + alternative->to_string();
  }
  return out;
}

std::string BlockStatement::to_string() const {
  std::string out;
  for (const auto &stmt : statements) {
    out += stmt->to_string();
  }
  return out;
}

std::string FunctionLiteral::to_string() const {
  std::string out = token.literal + "(";
  if (parameters.size() > 0) {
    for (const auto &param : parameters) {
      out += param->to_string() + ", ";
    }
    out.pop_back();
    out.pop_back();
  }
  out += ") " + body->to_string();
  return out;
}

std::string CallExpression::to_string() const {
  std::string out = function->to_string() + "(";
  if (arguments.size() > 0) {
    for (const auto &arg : arguments) {
      out += arg->to_string() + ", ";
    }
    out.pop_back();
    out.pop_back();
  }
  out += ")";
  return out;
}

std::string StringLiteral::to_string() const { return value; }

std::string ArrayLiteral::to_string() const {
  std::string out = "[";
  if (elements.size() > 0) {
    for (const auto &elem : elements) {
      out += elem->to_string() + ", ";
    }
    out.pop_back();
    out.pop_back();
  }
  out += "]";
  return out;
}

constexpr StatementType LetStatement::Type() const {
  return StatementType::LET;
}
constexpr StatementType ReturnStatement::Type() const {
  return StatementType::RETURN;
}
constexpr StatementType ExpressionStatement::Type() const {
  return StatementType::EXPRESSION;
}
constexpr StatementType BlockStatement::Type() const {
  return StatementType::BLOCK;
}

constexpr ExpressionType Identifier::Type() const {
  return ExpressionType::IDENTIFIER;
}
constexpr ExpressionType IntegerLiteral::Type() const {
  return ExpressionType::INTEGER;
}
constexpr ExpressionType PrefixExpression::Type() const {
  return ExpressionType::PREFIX;
}
constexpr ExpressionType InfixExpression::Type() const {
  return ExpressionType::INFIX;
}
constexpr ExpressionType Boolean::Type() const {
  return ExpressionType::BOOLEAN;
}
constexpr ExpressionType IfExpression::Type() const {
  return ExpressionType::IF;
}
constexpr ExpressionType FunctionLiteral::Type() const {
  return ExpressionType::FUNCTION;
}
constexpr ExpressionType CallExpression::Type() const {
  return ExpressionType::CALL;
}
constexpr ExpressionType StringLiteral::Type() const {
  return ExpressionType::STRING;
}

constexpr ExpressionType ArrayLiteral::Type() const {
  return ExpressionType::ARRAY;
}

} // namespace ast
} // namespace parser
} // namespace monkey
