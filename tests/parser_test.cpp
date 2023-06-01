#include "../parser/parser.hpp"
#include <boost/test/unit_test.hpp>

using namespace monkey::parser;
using namespace monkey::parser::ast;

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
 // BOOST_REQUIRE_EQUAL(program->statements.size(), 3);
}
