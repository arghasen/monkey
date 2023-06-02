#include "../parser/parser.hpp"
#include <boost/test/unit_test.hpp>

using namespace monkey::parser;
using namespace monkey::parser::ast;

template<typename T>
T* getAs(AstNode auto* s){
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
  checkParserErrors(p);
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 3);
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
  BOOST_REQUIRE_EQUAL(p.getErrors().size(), 5); // TODO: assign, semicolon extra expecting to get parsed
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
  checkParserErrors(p);
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 3);

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
  checkParserErrors(p);
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  auto ident = getAs<Identifier>(exprStmt->expression.get());
  BOOST_REQUIRE_EQUAL(ident->TokenLiteral(), "foobar");
  BOOST_REQUIRE_EQUAL(ident->value, "foobar");
}

BOOST_AUTO_TEST_CASE(TestIntegerLiteralExpression){
  auto input = "5;";
  monkey::lexer::Lexer l(input);
  Parser p(&l);
  auto program = p.parseProgram();
  checkParserErrors(p);
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  auto literal = getAs<IntegerLiteral>(exprStmt->expression.get());
  BOOST_REQUIRE_EQUAL(literal->TokenLiteral(), "5");
  BOOST_REQUIRE_EQUAL(literal->value, 5);
}

BOOST_AUTO_TEST_CASE(TestParsingPrefixExpressions){
  auto input = R"(
    !5;
    -15;
  )";
  monkey::lexer::Lexer l(input);
  Parser p(&l);
  auto program = p.parseProgram();
  checkParserErrors(p);
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 2);
  std::vector<std::pair<std::string,std::string>> tests = {
    {"!", "5"},
    {"-", "15"}
  };
  for(auto counter=0; auto test : tests){
    auto stmt = program->statements[counter].get();
    auto exprStmt = getAs<ExpressionStatement>(stmt);
    auto prefixExpr = getAs<PrefixExpression>(exprStmt->expression.get());
    BOOST_REQUIRE_EQUAL(prefixExpr->op, test.first);
    BOOST_REQUIRE_EQUAL(prefixExpr->TokenLiteral(), test.first);
    auto intLiteral = getAs<IntegerLiteral>(prefixExpr->right.get());
    BOOST_REQUIRE_EQUAL(intLiteral->value, std::stoi(test.second));
    BOOST_REQUIRE_EQUAL(intLiteral->TokenLiteral(), test.second);
    counter++;
  }
}

BOOST_AUTO_TEST_CASE(TestParsingInfixExpressions){

  std::vector<std::tuple<std::string,std::string,std::string, std::string>> tests = {
    {"5+5;","5","+","5"},
    {"5-5","5","-","5"},
    {"5*5","5","*","5"},
    {"5/5","5","/","5"},
    {"5>5","5",">","5"},
    {"5<5","5","<","5"},
    {"5==5","5","==","5"},
    {"5!=5","5","!=","5"},
    // {"foobar + barfoo;", "foobar", "+", "barfoo"},
		// {"foobar - barfoo;", "foobar", "-", "barfoo"},
		// {"foobar * barfoo;", "foobar", "*", "barfoo"},
		// {"foobar / barfoo;", "foobar", "/", "barfoo"},
		// {"foobar > barfoo;", "foobar", ">", "barfoo"},
		// {"foobar < barfoo;", "foobar", "<", "barfoo"},
		// {"foobar == barfoo;", "foobar", "==", "barfoo"},
		// {"foobar != barfoo;", "foobar", "!=", "barfoo"},
		// {"true == true", true, "==", true},
		// {"true != false", true, "!=", false},
		// {"false == false", false, "==", false},
  };

  for(auto [input, expectedLeft, expectedOp, expectedRight ]:tests){
    monkey::lexer::Lexer l(input);
    Parser p(&l);
    auto program = p.parseProgram();
    checkParserErrors(p);
    BOOST_REQUIRE_NE(program, nullptr);
    BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
      auto stmt = program->statements[0].get();
      auto exprStmt = getAs<ExpressionStatement>(stmt);
      auto infixExpr = getAs<InfixExpression>(exprStmt->expression.get());
      BOOST_REQUIRE_EQUAL(infixExpr->op, expectedOp);
      BOOST_REQUIRE_EQUAL(infixExpr->TokenLiteral(), expectedOp);
      auto left = getAs<IntegerLiteral>(infixExpr->left.get());
      BOOST_REQUIRE_EQUAL(left->value,std::stoi(expectedLeft));
      BOOST_REQUIRE_EQUAL(left->TokenLiteral(), expectedLeft);
      auto right = getAs<IntegerLiteral>(infixExpr->right.get());
      BOOST_REQUIRE_EQUAL(right->value, std::stoi(expectedRight));
      BOOST_REQUIRE_EQUAL(right->TokenLiteral(), expectedRight);
  }

}

BOOST_AUTO_TEST_CASE(TestOperatorPrecedenceParsing){
  std::vector<std::pair<std::string, std::string>> tests = {
    {"-a * b", "((-a) * b)"},
    {"!-a", "(!(-a))"},
    {"a + b + c", "((a + b) + c)"},
    {"a + b - c", "((a + b) - c)"},
    {"a * b * c", "((a * b) * c)"},
    {"a * b / c", "((a * b) / c)"},
    {"a + b / c", "(a + (b / c))"},
    {"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
    {"3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"},
    {"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
    {"5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"},
    {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
    // 		{
		// 	"true",
		// 	"true",
		// },
		// {
		// 	"false",
		// 	"false",
		// },
		// {
		// 	"3 > 5 == false",
		// 	"((3 > 5) == false)",
		// },
		// {
		// 	"3 < 5 == true",
		// 	"((3 < 5) == true)",
		// },
		// {
		// 	"1 + (2 + 3) + 4",
		// 	"((1 + (2 + 3)) + 4)",
		// },
		// {
		// 	"(5 + 5) * 2",
		// 	"((5 + 5) * 2)",
		// },
		// {
		// 	"2 / (5 + 5)",
		// 	"(2 / (5 + 5))",
		// },
		// {
		// 	"(5 + 5) * 2 * (5 + 5)",
		// 	"(((5 + 5) * 2) * (5 + 5))",
		// },
		// {
		// 	"-(5 + 5)",
		// 	"(-(5 + 5))",
		// },
		// {
		// 	"!(true == true)",
		// 	"(!(true == true))",
		// },
		// {
		// 	"a + add(b * c) + d",
		// 	"((a + add((b * c))) + d)",
		// },
		// {
		// 	"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
		// 	"add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))",
		// },
		// {
		// 	"add(a + b + c * d / f + g)",
		// 	"add((((a + b) + ((c * d) / f)) + g))",
		// },
  };

  for(auto [input, expectedResults] : tests){
    monkey::lexer::Lexer l(input);
    Parser p(&l);
    auto program = p.parseProgram();
    checkParserErrors(p);
    BOOST_REQUIRE_NE(program, nullptr);
    auto actual = program->to_string();
    BOOST_REQUIRE_EQUAL(actual, expectedResults);
  }
}