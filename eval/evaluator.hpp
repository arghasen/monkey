#pragma once
#include "object.hpp"
#include "../parser/ast.hpp"
#include <iostream>

namespace monkey::evaluator
{
    class Evaluator{
public:
    Evaluator() = default;
    ~Evaluator() = default;
    ObjectPtr eval(auto* node);
private:
    ObjectPtr doEval(const parser::ast::Statements& node);
};


ObjectPtr Evaluator::doEval(const parser::ast::Statements& node){
    std::cout<<"evaluating statements"<<std::endl;
    std::unique_ptr<Object> result;
    for(auto& stmt : node){
        result = eval(stmt.get());
    }
    return result;
}

ObjectPtr Evaluator::eval(auto* node) {
    if constexpr (std::is_same_v<parser::ast::Program, std::decay_t<decltype(*node)>>){
        return doEval(node->statements);
    }
    else if constexpr(std::is_same_v<parser::ast::Statement, std::decay_t<decltype(*node)>>)
    {
        if (node->Type() == parser::ast::StatementType::EXPRESSION)
        {
            return eval(static_cast<parser::ast::ExpressionStatement*>(node)->expression.get());
        }
    }
    else if constexpr(std::is_same_v<parser::ast::Expression, std::decay_t<decltype(*node)>>)
    {
        if (node->Type() == parser::ast::ExpressionType::INTEGER)
        {
            return std::make_unique<Integer>(static_cast<parser::ast::IntegerLiteral*>(node)->value);
        }
        else if (node->Type() == parser::ast::ExpressionType::BOOLEAN)
        {
            return std::make_unique<Boolean>(static_cast<parser::ast::Boolean*>(node)->value);
        }
    }
    else{ 
        std::cout<<"evaluating node"<<std::endl;
        return nullptr;
    }
    return nullptr;
}
} // namespace monkey::evaluator
