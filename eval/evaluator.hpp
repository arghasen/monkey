#pragma once
#include "../parser/ast.hpp"
#include "object.hpp"
#include <iostream>

namespace monkey::evaluator {
class Evaluator {
public:
  Evaluator() = default;
  ~Evaluator() = default;
  ObjectPtr eval(monkey::parser::ast::AstNode auto *node, Environment env);

private:
  ObjectPtr evalProgram(const parser::ast::Statements &node, Environment env);
  ObjectPtr evalBlockStatement(const parser::ast::Statements &node,
                               Environment env);
  Results evalExpressions(const parser::ast::Arguments &node, Environment env);
  ObjectPtr applyFunction(ObjectPtr fn, const Results &args);

  ObjectPtr doEval(parser::ast::Statement *node, Environment env);
  ObjectPtr doEval(parser::ast::Expression *node, Environment env);
  ObjectPtr doEval(parser::ast::IntegerLiteral *node, Environment env);
  ObjectPtr doEval(parser::ast::Boolean *node, Environment env);
  ObjectPtr doEval(parser::ast::PrefixExpression *node, Environment env);
  ObjectPtr doEval(parser::ast::InfixExpression *node, Environment env);
  ObjectPtr doEval(parser::ast::IfExpression *node, Environment env);
  ObjectPtr doEval(parser::ast::FunctionLiteral *node, Environment env);
  ObjectPtr doEval(parser::ast::CallExpression *node, Environment env);
  ObjectPtr doEval(parser::ast::Identifier *node, Environment env);
  ObjectPtr doEval(parser::ast::LetStatement *node, Environment env);
  ObjectPtr doEval(parser::ast::ReturnStatement *node, Environment env);
  ObjectPtr doEval(parser::ast::ExpressionStatement *node, Environment env);
  ObjectPtr doEval(parser::ast::StringLiteral *node, Environment env);
};

ObjectPtr Evaluator::eval(monkey::parser::ast::AstNode auto *node,
                          Environment env) {
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
    return evalProgram(node->statements, env);
  } else if constexpr (isBlockStatements) {
    return evalBlockStatement(node->statements, env);
  } else if constexpr (isStatement || isExpression) {
    return doEval(node, env);
  } else {
    std::cout << "evaluating node" << std::endl;
    return nullptr;
  }
}
} // namespace monkey::evaluator
