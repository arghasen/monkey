#pragma once

namespace monkey{
namespace parser{
namespace ast{

class Node {
public:
    virtual ~Node() = default;
    virtual std::string TokenLiteral() const = 0;
};

class Statement : public Node {
public:
    virtual ~Statement() = default;
    virtual std::string TokenLiteral() const = 0;
};

class Expression : public Node {
public:
    virtual ~Expression() = default;
    virtual std::string TokenLiteral() const = 0;
};

class Program : public Node {
public:
    Program() = default;
    ~Program() override = default;
    std::string TokenLiteral() const override;
    std::string String() const;
    std::vector<Statement*> Statements;
};

class Identifier : public Expression {
public:
    Identifier() = default;
    ~Identifier() override = default;
    std::string TokenLiteral() const override;
    std::string String() const;
    std::string Value;
};

class LetStatement : public Statement {
public:
    LetStatement() = default;
    ~LetStatement() override = default;
    std::string TokenLiteral() const override;
    std::string String() const;
    Identifier* Name;
    Expression* Value;
};

} // namespace ast
} // namespace parser
} // namespace monkey