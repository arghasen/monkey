#include "evaluator.hpp"
#include <iostream>
namespace monkey::evaluator {

const static auto TRUE = std::make_shared<Boolean>(true);
const static auto FALSE = std::make_shared<Boolean>(false);
const static auto NullObject = std::make_shared<Null>();

bool isTruthy(ObjectPtr obj) {
  if (obj == NullObject) {
    return false;
  } else if (obj == TRUE) {
    return true;
  } else if (obj == FALSE) {
    return false;
  } else {
    return true;
  }
}

ObjectPtr getBoolean(bool value) { return value ? TRUE : FALSE; }

ObjectPtr evalBangOperatorExpression(ObjectPtr right) {
  if (right == TRUE) {
    return FALSE;
  } else if (right == FALSE) {
    return TRUE;
  } else if (right == NullObject) {
    return TRUE;
  } else {
    return FALSE;
  }
}

ObjectPtr evalMinusPrefixOperatorExpression(ObjectPtr right) {
  if (right->type() != INTEGER_OBJ) {
    return nullptr;
  }
  auto value = static_cast<Integer *>(right.get())->value_;
  return std::make_shared<Integer>(-value);
}

ObjectPtr evalIntegerInfixExpression(const std::string &op, ObjectPtr left,
                                     ObjectPtr right) {
  auto leftVal = static_cast<Integer *>(left.get())->value_;
  auto rightVal = static_cast<Integer *>(right.get())->value_;
  if (op == "+") {
    return std::make_shared<Integer>(leftVal + rightVal);
  } else if (op == "-") {
    return std::make_shared<Integer>(leftVal - rightVal);
  } else if (op == "*") {
    return std::make_shared<Integer>(leftVal * rightVal);
  } else if (op == "/") {
    return std::make_shared<Integer>(leftVal / rightVal);
  } else if (op == "<") {
    return getBoolean(leftVal < rightVal);
  } else if (op == ">") {
    return getBoolean(leftVal > rightVal);
  } else if (op == "==") {
    return getBoolean(leftVal == rightVal);
  } else if (op == "!=") {
    return getBoolean(leftVal != rightVal);
  } else {
    return NullObject;
  }
}

ObjectPtr evalPrefixExpression(const std::string &op, ObjectPtr right) {
  if (op == "!") {
    return evalBangOperatorExpression(right);
  } else if (op == "-") {
    return evalMinusPrefixOperatorExpression(right);
  } else {
    return NullObject;
  }
}

ObjectPtr evalInfixExpression(const std::string &op, ObjectPtr left,
                              ObjectPtr right) {
  if (left->type() == INTEGER_OBJ && right->type() == INTEGER_OBJ) {
    return evalIntegerInfixExpression(op, left, right);
  } else if (op == "==") {
    return getBoolean(left == right);
  } else if (op == "!=") {
    return getBoolean(left != right);
  } else if (left->type() != right->type()) {
    return NullObject;
  } else {
    return NullObject;
  }
}

ObjectPtr Evaluator::evalProgram(const parser::ast::Statements &node) {
  std::cout << "evaluating statements" << std::endl;
  std::shared_ptr<Object> result;
  for (auto &stmt : node) {
    result = eval(stmt.get());
    if (result->type() == RETURN_VALUE_OBJ){
      return static_cast<ReturnValue*>(result.get())->value_;
    }
  }
  return result;
}

ObjectPtr Evaluator::evalBlockStatement(const parser::ast::Statements &node) {
  std::cout << "evaluating block statement" << std::endl;
  std::shared_ptr<Object> result;
  for (auto &stmt : node) {
    result = eval(stmt.get());
    if (result && result->type() == RETURN_VALUE_OBJ){
      return result;
    }
  }
  return result;
}

ObjectPtr Evaluator::doEval(parser::ast::Statement *node) {
  std::cout << "evaluating statement" << std::endl;
  switch(node->Type()){
    case parser::ast::StatementType::LET:
      return doEval(static_cast<parser::ast::LetStatement *>(node));
    case parser::ast::StatementType::RETURN:
      return doEval(static_cast<parser::ast::ReturnStatement *>(node));
    case parser::ast::StatementType::EXPRESSION:
      return doEval(static_cast<parser::ast::ExpressionStatement *>(node));
    default:
      return nullptr;
  }
}

ObjectPtr Evaluator::doEval(parser::ast::ExpressionStatement *node) {
  return eval(node->expression.get());
}

ObjectPtr Evaluator::doEval(parser::ast::IntegerLiteral *node) {
  return std::make_shared<Integer>(node->value);
}

ObjectPtr Evaluator::doEval(parser::ast::Boolean *node) {
  return getBoolean(node->value);
}

ObjectPtr Evaluator::doEval(parser::ast::InfixExpression *node) {
  auto left = eval(node->left.get());
  auto right = eval(node->right.get());
  return evalInfixExpression(node->op, left, right);
}

ObjectPtr Evaluator::doEval(parser::ast::PrefixExpression *node) {
  auto right = eval(node->right.get());
  return evalPrefixExpression(node->op, right);
}

ObjectPtr Evaluator::doEval(parser::ast::IfExpression *node) {
  auto condition = eval(node->condition.get());
  if (isTruthy(condition)) {
    return eval(node->consequence.get());
  } else if (node->alternative != nullptr) {
    return eval(node->alternative.get());
  } else {
    return NullObject;
  }
}

ObjectPtr Evaluator::doEval(parser::ast::FunctionLiteral *node) {
  return nullptr;
}

ObjectPtr Evaluator::doEval(parser::ast::CallExpression *node) {
  return nullptr;
}

ObjectPtr Evaluator::doEval(parser::ast::Identifier *node) { return nullptr; }

ObjectPtr Evaluator::doEval(parser::ast::LetStatement *node) { return nullptr; }

ObjectPtr Evaluator::doEval(parser::ast::ReturnStatement *node) {
    auto value = eval(node->returnValue.get());
    return std::make_shared<ReturnValue>(value);
}

ObjectPtr Evaluator::doEval(parser::ast::Expression *node) {
  std::cout << "evaluating expression" << (int)node->Type() << std::endl;
  switch (node->Type()) {
  case parser::ast::ExpressionType::IDENTIFIER:
    return doEval(static_cast<parser::ast::Identifier *>(node));
  case parser::ast::ExpressionType::INTEGER:
    return doEval(static_cast<parser::ast::IntegerLiteral *>(node));
  case parser::ast::ExpressionType::BOOLEAN:
    return doEval(static_cast<parser::ast::Boolean *>(node));
  case parser::ast::ExpressionType::PREFIX:
    return doEval(static_cast<parser::ast::PrefixExpression *>(node));
  case parser::ast::ExpressionType::INFIX:
    return doEval(static_cast<parser::ast::InfixExpression *>(node));
  case parser::ast::ExpressionType::IF:
    return doEval(static_cast<parser::ast::IfExpression *>(node));
  case parser::ast::ExpressionType::FUNCTION:
    return doEval(static_cast<parser::ast::FunctionLiteral *>(node));
  case parser::ast::ExpressionType::CALL:
    return doEval(static_cast<parser::ast::CallExpression *>(node));
  default:
    return nullptr;
  }
  return nullptr;
}
} // namespace monkey::evaluator
