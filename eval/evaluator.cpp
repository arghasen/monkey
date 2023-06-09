#include "evaluator.hpp"
#include "builtins.hpp"
#include <iostream>
#include <sstream>

namespace monkey::evaluator {

const static auto TRUE = std::make_shared<Boolean>(true);
const static auto FALSE = std::make_shared<Boolean>(false);
const static auto NullObject = std::make_shared<Null>();

Evaluator::Evaluator() : builtins(create_builtins()) {}

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

bool isError(ObjectPtr obj) {
  if (obj != nullptr) { // TODO: is Null object an error too?
    return obj->type() == ERROR_OBJ;
  }
  return false;
}

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
    return makeError("unknown operator: -", right->type());
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
    return makeError("unknown operator: ", op, left->type(), right->type());
  }
}

ObjectPtr evalStringInfixExpression(const std::string &op, ObjectPtr left,
                                    ObjectPtr right) {
  auto leftVal = static_cast<String *>(left.get())->value_;
  auto rightVal = static_cast<String *>(right.get())->value_;
  if (op == "+") {
    return std::make_shared<String>(leftVal + rightVal);
  } else {
    return makeError("unknown operator:", left->type(), op, right->type());
  }
}

ObjectPtr evalPrefixExpression(const std::string &op, ObjectPtr right) {
  if (op == "!") {
    return evalBangOperatorExpression(right);
  } else if (op == "-") {
    return evalMinusPrefixOperatorExpression(right);
  } else {
    return makeError("unknown operator:", op, right->type());
  }
}

ObjectPtr evalInfixExpression(const std::string &op, ObjectPtr left,
                              ObjectPtr right) {
  if (left->type() == INTEGER_OBJ && right->type() == INTEGER_OBJ) {
    return evalIntegerInfixExpression(op, left, right);
  } else if (left->type() == STRING_OBJ && right->type() == STRING_OBJ) {
    return evalStringInfixExpression(op, left, right);
  } else if (op == "==") {
    return getBoolean(left == right);
  } else if (op == "!=") {
    return getBoolean(left != right);
  } else if (left->type() != right->type()) {
    return makeError("type mismatch:", left->type(), op, right->type());
  } else {
    return makeError("unknown operator:", left->type(), op, right->type());
  }
}

ObjectPtr Evaluator::evalProgram(const parser::ast::Statements &node,
                                 Environment env) {
  //   std::cout << "evaluating statements" << std::endl;
  std::shared_ptr<Object> result;
  for (auto &stmt : node) {
    result = eval(stmt.get(), env);
    if (result->type() == RETURN_VALUE_OBJ) {
      return static_cast<ReturnValue *>(result.get())->value_;
    }
    if (result->type() == ERROR_OBJ) {
      return result;
    }
  }
  return result;
}

ObjectPtr Evaluator::evalBlockStatement(const parser::ast::Statements &node,
                                        Environment env) {
  //   std::cout << "evaluating block statement" << std::endl;
  std::shared_ptr<Object> result;
  for (auto &stmt : node) {
    result = eval(stmt.get(), env);
    if (result) {
      auto res = result->type();
      if (res == RETURN_VALUE_OBJ || res == ERROR_OBJ) {
        return result;
      }
    }
  }
  return result;
}

ObjectPtr Evaluator::doEval(parser::ast::Statement *node, Environment env) {
  //   std::cout << "evaluating statement" << std::endl;
  switch (node->Type()) {
  case parser::ast::StatementType::LET:
    return doEval(static_cast<parser::ast::LetStatement *>(node), env);
  case parser::ast::StatementType::RETURN:
    return doEval(static_cast<parser::ast::ReturnStatement *>(node), env);
  case parser::ast::StatementType::EXPRESSION:
    return doEval(static_cast<parser::ast::ExpressionStatement *>(node), env);
  default:
    return nullptr;
  }
}

ObjectPtr Evaluator::doEval(parser::ast::ExpressionStatement *node,
                            Environment env) {
  return eval(node->expression.get(), env);
}

ObjectPtr Evaluator::doEval(parser::ast::IntegerLiteral *node,
                            Environment env) {
  return std::make_shared<Integer>(node->value);
}

ObjectPtr Evaluator::doEval(parser::ast::Boolean *node, Environment env) {
  return getBoolean(node->value);
}

ObjectPtr Evaluator::doEval(parser::ast::StringLiteral *node, Environment env) {
  return std::make_shared<String>(node->value);
}

ObjectPtr Evaluator::doEval(parser::ast::InfixExpression *node,
                            Environment env) {
  auto left = eval(node->left.get(), env);
  if (isError(left)) {
    return left;
  }
  auto right = eval(node->right.get(), env);
  if (isError(right)) {
    return right;
  }
  return evalInfixExpression(node->op, left, right);
}

ObjectPtr Evaluator::doEval(parser::ast::PrefixExpression *node,
                            Environment env) {
  auto right = eval(node->right.get(), env);
  if (isError(right)) {
    return right;
  }
  return evalPrefixExpression(node->op, right);
}

ObjectPtr Evaluator::doEval(parser::ast::IfExpression *node, Environment env) {
  auto condition = eval(node->condition.get(), env);
  if (isError(condition)) {
    return condition;
  }
  if (isTruthy(condition)) {
    return eval(node->consequence.get(), env);
  } else if (node->alternative != nullptr) {
    return eval(node->alternative.get(), env);
  } else {
    return NullObject;
  }
}

ObjectPtr Evaluator::doEval(parser::ast::FunctionLiteral *node,
                            Environment env) {
  return std::make_shared<Function>(std::move(node->parameters),
                                    std::move(node->body), env);
}

Results Evaluator::evalExpressions(const parser::ast::Arguments &args,
                                   Environment env) {
  std::vector<ObjectPtr> result;
  for (auto &arg : args) {
    auto evaluated = eval(arg.get(), env);
    if (isError(evaluated)) {
      return Results{evaluated};
    }
    result.push_back(evaluated);
  }
  return result;
}

Environment extendFunctionEnv(Function *fn, const Results &args) {
  auto env = new_enclosed_environment(fn->env_);
  for (size_t i = 0; i < fn->parameters.size(); i++) {
    env->set(fn->parameters[i]->value, args[i]);
  }
  return env;
}

ObjectPtr unwrapReturnValue(ObjectPtr obj) {
  if (obj->type() == RETURN_VALUE_OBJ) {
    return static_cast<ReturnValue *>(obj.get())->value_;
  }
  return obj;
}

ObjectPtr Evaluator::applyFunction(Function *fn, const Results &args) {
  auto extendedEnv = extendFunctionEnv(fn, args);
  auto evaluated = eval(fn->body.get(), extendedEnv);
  return unwrapReturnValue(evaluated);
}

ObjectPtr Evaluator::applyBuiltin(Builtin *fn, const Results &args) {
  return fn->operator()(args);
}

ObjectPtr Evaluator::applyFunction(ObjectPtr fn, const Results &args) {
  auto fnType = fn->type();
  if (fnType == FUNCTION_OBJ) {
    return applyFunction(static_cast<Function *>(fn.get()), args);
  } else if (fnType == BUILTIN_OBJ) {
    return applyBuiltin(static_cast<Builtin *>(fn.get()), args);
  } else {
    return makeError("not a function", fn->to_string());
  }
}

ObjectPtr Evaluator::doEval(parser::ast::CallExpression *node,
                            Environment env) {
  auto function = eval(node->function.get(), env);
  if (isError(function)) {
    return function;
  }
  auto args = evalExpressions(node->arguments, env);
  if (args.size() == 1 && isError(args[0])) {
    return args[0];
  }
  return applyFunction(function, args);
}

ObjectPtr Evaluator::doEval(parser::ast::Identifier *node, Environment env) {
  auto value = env->get(node->value);
  if (value.found) {
    return value.value;
  }

  auto builtin = builtins.find(node->value);
  if (builtin != builtins.end()) {
    return builtin->second;
  }
  return makeError("identifier not found:", node->value);
}

ObjectPtr Evaluator::doEval(parser::ast::LetStatement *node, Environment env) {
  auto value = eval(node->value.get(), env);
  if (isError(value)) {
    return value;
  }
  env->set(node->name->value, value);
  return value;
}

ObjectPtr Evaluator::doEval(parser::ast::ReturnStatement *node,
                            Environment env) {
  auto value = eval(node->returnValue.get(), env);
  if (isError(value)) {
    return value;
  }
  return std::make_shared<ReturnValue>(value);
}

ObjectPtr Evaluator::doEval(parser::ast::Expression *node, Environment env) {
  // std::cout << "evaluating expression" << (int)node->Type() << std::endl;
  switch (node->Type()) {
  case parser::ast::ExpressionType::IDENTIFIER:
    return doEval(static_cast<parser::ast::Identifier *>(node), env);
  case parser::ast::ExpressionType::INTEGER:
    return doEval(static_cast<parser::ast::IntegerLiteral *>(node), env);
  case parser::ast::ExpressionType::BOOLEAN:
    return doEval(static_cast<parser::ast::Boolean *>(node), env);
  case parser::ast::ExpressionType::PREFIX:
    return doEval(static_cast<parser::ast::PrefixExpression *>(node), env);
  case parser::ast::ExpressionType::INFIX:
    return doEval(static_cast<parser::ast::InfixExpression *>(node), env);
  case parser::ast::ExpressionType::IF:
    return doEval(static_cast<parser::ast::IfExpression *>(node), env);
  case parser::ast::ExpressionType::FUNCTION:
    return doEval(static_cast<parser::ast::FunctionLiteral *>(node), env);
  case parser::ast::ExpressionType::CALL:
    return doEval(static_cast<parser::ast::CallExpression *>(node), env);
  case parser::ast::ExpressionType::STRING:
    return doEval(static_cast<parser::ast::StringLiteral *>(node), env);
  default:
    return nullptr;
  }
}
} // namespace monkey::evaluator
