#include "../parser/ast.hpp"
#include <boost/test/unit_test.hpp>

using namespace monkey::parser::ast;
using namespace monkey::lexer;

BOOST_AUTO_TEST_CASE(TestAST){

    auto  program = std::make_unique<Program>();
    auto  letStmt = std::make_unique<LetStatement>();

    auto ident1 = std::make_unique<Identifier>(Token{TokenType::IDENT, "myVar"});
    auto ident2 = std::make_unique<Identifier>(Token{TokenType::IDENT, "anotherVar"});
    letStmt->token = Token{TokenType::LET, "let"};
    letStmt->name = std::move(ident1);
    letStmt->value = std::move(ident2);
    program->statements.push_back(std::move(letStmt));
    BOOST_CHECK_EQUAL(program->to_string(), "let myVar = anotherVar;");
}