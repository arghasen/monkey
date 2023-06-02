#include "../parser/parser.hpp"
#include <boost/test/unit_test.hpp>

using namespace monkey::parser;
using namespace monkey::parser::ast;

void testLetStatement(Statement* s, std::string name){

  BOOST_REQUIRE_EQUAL(s->TokenLiteral(), "let");
  auto letStmt = dynamic_cast<LetStatement*>(s);
  if(letStmt == nullptr){
    BOOST_FAIL("s not *LetStatement. Got " + std::string(typeid(s).name()));
  }
  BOOST_REQUIRE_EQUAL(letStmt->name->value, name);
  BOOST_REQUIRE_EQUAL(letStmt->name->TokenLiteral(), name);
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
  std::vector expectedIdentifiers = {"x", "y", "foobar"};
  for(auto counter=0; auto expectedIdentifier : expectedIdentifiers){
      auto stmt = program->statements[counter].get();
      testLetStatement(stmt, expectedIdentifier);
      counter++;
  }
}
