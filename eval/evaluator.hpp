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
    ObjectPtr doEval(parser::ast::Statement* node);
    ObjectPtr doEval(parser::ast::Expression* node);
};

ObjectPtr Evaluator::eval(auto* node) {
    if constexpr (std::is_same_v<parser::ast::Program, std::decay_t<decltype(*node)>>){
        return doEval(node->statements);
    }
    else if constexpr(std::is_same_v<parser::ast::Statement, std::decay_t<decltype(*node)>>)
    {
        return doEval(node);
    }
    else if constexpr(std::is_same_v<parser::ast::Expression, std::decay_t<decltype(*node)>>)
    {
        return doEval(node);
    }
    else{ 
        std::cout<<"evaluating node"<<std::endl;
        return nullptr;
    }
    return nullptr;
}
} // namespace monkey::evaluator
