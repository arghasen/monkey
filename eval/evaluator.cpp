#include "evaluator.hpp"
#include <iostream>
namespace monkey::evaluator{

ObjectPtr Evaluator::doEval(const parser::ast::Statements& node){
    std::cout<<"evaluating statements"<<std::endl;
    std::unique_ptr<Object> result;
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

ObjectPtr Evaluator::doEval(parser::ast::Expression* node) {
    std::cout<<"evaluating expression"<< (int)node->Type()<<std::endl;
    if (node->Type() == parser::ast::ExpressionType::INTEGER)
    {
        return std::make_unique<Integer>(static_cast<parser::ast::IntegerLiteral*>(node)->value);
    }
    else if (node->Type() == parser::ast::ExpressionType::BOOLEAN)
    {
        return std::make_unique<Boolean>(static_cast<parser::ast::Boolean*>(node)->value);
    }
    return nullptr;
}
} // namespace monkey::evaluator

