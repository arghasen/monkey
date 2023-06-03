#include "evaluator.hpp"
#include <iostream>
namespace monkey::evaluator{

const static auto TRUE = std::make_shared<Boolean>(true);
const static auto FALSE = std::make_shared<Boolean>(false);
const static auto NullObject = std::make_shared<Null>();

ObjectPtr Evaluator::doEval(const parser::ast::Statements& node){
    std::cout<<"evaluating statements"<<std::endl;
    std::shared_ptr<Object> result;
    for(auto& stmt : node){
        result = eval(stmt.get());
    }
    return result;
}
ObjectPtr Evaluator::doEval(parser::ast::Statement* node) {
    std::cout << "evaluating statement" << std::endl;
    if (node->Type() == parser::ast::StatementType::EXPRESSION)
    {
        return eval(static_cast<parser::ast::ExpressionStatement*>(node)->expression.get());
    }
    return nullptr;
}

ObjectPtr Evaluator::doEval(parser::ast::IntegerLiteral* node){
    return std::make_shared<Integer>(node->value);
}

ObjectPtr Evaluator::doEval(parser::ast::Boolean* node){
    return (node->value) ? TRUE : FALSE;
}

ObjectPtr Evaluator::doEval(parser::ast::InfixExpression* node){
    return nullptr;
}

ObjectPtr Evaluator::doEval(parser::ast::PrefixExpression* node){
    return nullptr;
}

ObjectPtr Evaluator::doEval(parser::ast::IfExpression* node){
    return nullptr;
}

ObjectPtr Evaluator::doEval(parser::ast::FunctionLiteral* node){
    return nullptr;
}

ObjectPtr Evaluator::doEval(parser::ast::CallExpression* node){
    return nullptr;
}

ObjectPtr Evaluator::doEval(parser::ast::Identifier* node){
    return nullptr;
}

ObjectPtr Evaluator::doEval(parser::ast::LetStatement* node){
    return nullptr;
}

ObjectPtr Evaluator::doEval(parser::ast::ReturnStatement* node){
    return nullptr;
}

ObjectPtr Evaluator::doEval(parser::ast::BlockStatement* node){
    return nullptr;
}

ObjectPtr Evaluator::doEval(parser::ast::Expression* node) {
    std::cout<<"evaluating expression"<< (int)node->Type()<<std::endl;
    switch(node->Type()){
        case parser::ast::ExpressionType::IDENTIFIER:
            return doEval(static_cast<parser::ast::Identifier*>(node));
        case parser::ast::ExpressionType::INTEGER:
            return doEval(static_cast<parser::ast::IntegerLiteral*>(node));
        case parser::ast::ExpressionType::BOOLEAN:
            return doEval(static_cast<parser::ast::Boolean*>(node));
        case parser::ast::ExpressionType::PREFIX:
            return doEval(static_cast<parser::ast::PrefixExpression*>(node));
        case parser::ast::ExpressionType::INFIX:
            return doEval(static_cast<parser::ast::InfixExpression*>(node));
        case parser::ast::ExpressionType::IF:
            return doEval(static_cast<parser::ast::IfExpression*>(node));
        case parser::ast::ExpressionType::FUNCTION:
            return doEval(static_cast<parser::ast::FunctionLiteral*>(node));
        case parser::ast::ExpressionType::CALL:
            return doEval(static_cast<parser::ast::CallExpression*>(node));
        default:
            return nullptr;
    }
    return nullptr;
}
} // namespace monkey::evaluator

