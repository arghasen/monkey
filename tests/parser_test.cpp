#include "../parser/parser.hpp"
#include <boost/test/unit_test.hpp>

using namespace monkey::parser;
using namespace monkey::parser::ast;

template<typename T, typename Y>
T* getAs(Y* s){
  auto stmt = dynamic_cast<T*>(s);
  if(stmt == nullptr){
    BOOST_FAIL("s not *T. Got " + std::string(typeid(s).name()));
  }
  return stmt;
}

void testLetStatement(Statement* s, std::string name){
  BOOST_REQUIRE_EQUAL(s->TokenLiteral(), "let");
  auto letStmt = getAs<LetStatement>(s);
  BOOST_REQUIRE_EQUAL(letStmt->name->value, name);
  BOOST_REQUIRE_EQUAL(letStmt->name->TokenLiteral(), name);
}

void checkParserErrors(Parser& p){
  auto errors = p.getErrors();
  if(errors.empty()){
    return;
  }
  
  for(auto error : errors){
    BOOST_ERROR("parser error: " + error);
  }
  BOOST_FAIL("parser has " + std::to_string(errors.size()) + " errors");
}

BOOST_AUTO_TEST_CASE(TestLetStatements) {
  auto input = R"(
        let x = 5;
        let y = 10;
        let foobar = 838383;
    )";

  monkey::lexer::Lexer l(input);
  Parser p(&l);

  auto program = p.parseProgram();
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 3);
  checkParserErrors(p);
  std::vector expectedIdentifiers = {"x", "y", "foobar"};
  for(auto counter=0; auto expectedIdentifier : expectedIdentifiers){
      auto stmt = program->statements[counter].get();
      testLetStatement(stmt, expectedIdentifier);
      counter++;
  }
}

BOOST_AUTO_TEST_CASE(TestLetStatementsFailures) {
  auto input = R"(
        let failure;
        let anotherFailure 123;
        let  = 456;
    )";

  monkey::lexer::Lexer l(input);
  Parser p(&l);

  auto program = p.parseProgram();
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(p.getErrors().size(), 3);
}

BOOST_AUTO_TEST_CASE(TestReturnStatements){
  auto input = R"(
    return 5;
    return 10;
    return 993322;
  )";

  monkey::lexer::Lexer l(input);
  Parser p(&l);

  auto program = p.parseProgram();
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 3);
  checkParserErrors(p);

  for(auto& stmt : program->statements){
    auto returnStmt = getAs<ReturnStatement>(stmt.get());
    BOOST_REQUIRE_EQUAL(returnStmt->TokenLiteral(), "return");
  }
}

BOOST_AUTO_TEST_CASE(TestIdentifierExpression){
  auto input = "foobar;";
  monkey::lexer::Lexer l(input);
  Parser p(&l);
  auto program = p.parseProgram();
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
  checkParserErrors(p);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  auto ident = getAs<Identifier>(exprStmt->expression.get());
  BOOST_REQUIRE_EQUAL(ident->TokenLiteral(), "foobar");
}