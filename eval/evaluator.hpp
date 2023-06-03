#pragma once
#include "../parser/ast.hpp"
#include "object.hpp"
#include <iostream>

namespace monkey::evaluator {
class Evaluator {
public:
  Evaluator() = default;
  ~Evaluator() = default;
  ObjectPtr eval(monkey::parser::ast::AstNode auto *node);

private:
  ObjectPtr evalProgram(const parser::ast::Statements &node);
  ObjectPtr evalBlockStatement(const parser::ast::Statements &node);

  ObjectPtr doEval(parser::ast::Statement *node);
  ObjectPtr doEval(parser::ast::Expression *node);
  ObjectPtr doEval(parser::ast::IntegerLiteral *node);
  ObjectPtr doEval(parser::ast::Boolean *node);
  ObjectPtr doEval(parser::ast::PrefixExpression *node);
  ObjectPtr doEval(parser::ast::InfixExpression *node);
  ObjectPtr doEval(parser::ast::IfExpression *node);
  ObjectPtr doEval(parser::ast::FunctionLiteral *node);
  ObjectPtr doEval(parser::ast::CallExpression *node);
  ObjectPtr doEval(parser::ast::Identifier *node);
  ObjectPtr doEval(parser::ast::LetStatement *node);
  ObjectPtr doEval(parser::ast::ReturnStatement *node);
  ObjectPtr doEval(parser::ast::ExpressionStatement *node);
};

ObjectPtr Evaluator::eval(monkey::parser::ast::AstNode auto *node) {
  constexpr auto isProram =
      std::is_same_v<parser::ast::Program, std::decay_t<decltype(*node)>>;
  constexpr auto isBlockStatements =
      std::is_same_v<parser::ast::BlockStatement,
                     std::decay_t<decltype(*node)>>;
  constexpr auto isStatement =
      std::is_same_v<parser::ast::Statement, std::decay_t<decltype(*node)>>;
  constexpr auto isExpression =
      std::is_same_v<parser::ast::Expression, std::decay_t<decltype(*node)>>;

  if constexpr (isProram) {
    return evalProgram(node->statements);
    } else if constexpr (isBlockStatements) {
    return evalBlockStatement(node->statements);
  } else if constexpr (isStatement || isExpression) {
    return doEval(node);
  } else {
    std::cout << "evaluating node" << std::endl;
    return nullptr;
  }
  return nullptr;
}
} // namespace monkey::evaluator
