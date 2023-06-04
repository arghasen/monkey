#include "../parser/parser.hpp"
#include <boost/test/unit_test.hpp>
#include <variant>

using namespace monkey::parser;
using namespace monkey::parser::ast;

template<typename T>
T* getAs(AstChildNode auto* s){
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

void testIntegerLiteral(ast::Expression* expr, int64_t value){
  auto literal = getAs<ast::IntegerLiteral>(expr);
  BOOST_REQUIRE_EQUAL(literal->value, value);
  BOOST_REQUIRE_EQUAL(literal->TokenLiteral(), std::to_string(value));
}

void testIdentifier(ast::Expression* expr, std::string value){
  auto ident = getAs<ast::Identifier>(expr);
  BOOST_REQUIRE_EQUAL(ident->value, value);
  BOOST_REQUIRE_EQUAL(ident->TokenLiteral(), value);
}

void testBooleanLiteral(ast::Expression* expr, bool value){
  auto boolean = getAs<ast::Boolean>(expr);
  BOOST_REQUIRE_EQUAL(boolean->value, value);
  BOOST_REQUIRE_EQUAL(boolean->TokenLiteral(), std::to_string(value));
}

void testLiteralExpression(ast::Expression* expr, auto expected){
  if constexpr(std::is_same_v<decltype(expected), int64_t>){
    testIntegerLiteral(expr, expected);
  } else if constexpr(std::is_same_v<decltype(expected), std::string>){
    testIdentifier(expr, expected);
  } else if constexpr(std::is_same_v<decltype(expected), bool>){
    testBooleanLiteral(expr, expected);
  } else {
    BOOST_FAIL("type of expr not handled. Got " + std::string(typeid(expr).name()));
  }
}

void testInfixExpression(InfixExpression* expr, auto left, std::string op, auto right){
    BOOST_REQUIRE_EQUAL(expr->op, op);
    BOOST_REQUIRE_EQUAL(expr->TokenLiteral(), op);
    testLiteralExpression(expr->left.get(), left);
    testLiteralExpression(expr->right.get(), right);
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

std::unique_ptr<ast::Program> testProgram(std::string input){
  monkey::lexer::Lexer l(input);
  Parser p(&l);
  auto program = p.parseProgram();
  checkParserErrors(p);
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
  return program;
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
  std::vector<std::pair<std::string, std::string>> expectedIdentifiers = {
    {"x", "5"},
    {"y", "10"},
    {"foobar", "838383"}
  };
  for(auto counter=0; auto [expectedIdentifier, expectedValue] : expectedIdentifiers){
      auto stmt = program->statements[counter].get();
      testLetStatement(stmt, expectedIdentifier);
      auto expr = getAs<LetStatement>(stmt);
      auto literal = getAs<IntegerLiteral>(expr->value.get());
      BOOST_REQUIRE_EQUAL(literal->value, std::stoi(expectedValue));
      BOOST_REQUIRE_EQUAL(literal->TokenLiteral(), expectedValue); 
      counter++;
  }
}

BOOST_AUTO_TEST_CASE(TestParseFailures) {
  auto input = R"(
        let failure;
        let anotherFailure 123;
        let  = 456;
        {"1,2",""}
    )";

  monkey::lexer::Lexer l(input);
  Parser p(&l);

  auto program = p.parseProgram();
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(p.getErrors().size(), 8); // TODO: assign, semicolon extra expecting to get parsed
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
  std::vector<std::string> expectedValues = {
    "5",
    "10",
    "993322"
  };
  for(auto counter =0;auto& stmt : program->statements){
    auto returnStmt = getAs<ReturnStatement>(stmt.get());
    BOOST_REQUIRE_EQUAL(returnStmt->TokenLiteral(), "return");
    auto literal = getAs<IntegerLiteral>(returnStmt->returnValue.get());
    BOOST_REQUIRE_EQUAL(literal->value, std::stoi(expectedValues[counter]));
    BOOST_REQUIRE_EQUAL(literal->TokenLiteral(), expectedValues[counter]); 
    counter++;
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
        //     {"!true;", "!", true},
        // {"!false;", "!", false},
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

  std::vector<std::tuple<std::string,int64_t,std::string, int64_t>> tests = {
    {"5+5;",5,"+",5},
    {"5-5;",5,"-",5},
    {"5*5;",5,"*",5},
    {"5/5;",5,"/",5},
    {"5>5;",5,">",5},
    {"5<5;",5,"<",5},
    {"5==5;",5,"==",5},
    {"5!=5;",5,"!=",5},
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

  auto doTest = [](){

  };
      for(auto [input, expectedLeft, expectedOp, expectedRight ]:tests){
      auto program = testProgram(input);
      auto stmt = program->statements[0].get();
      auto exprStmt = getAs<ExpressionStatement>(stmt);
      auto infixExpr = getAs<InfixExpression>(exprStmt->expression.get());
      testInfixExpression(infixExpr, expectedLeft, expectedOp, expectedRight);
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
    { "true","true", },
    {"false","false",},
    { "3 > 5 == false", "((3 > 5) == false)", },
    { "3 < 5 == true", "((3 < 5) == true)", },
    { "1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)", },
    { "(5 + 5) * 2", "((5 + 5) * 2)", },
    { "2 / (5 + 5)", "(2 / (5 + 5))", },
    { "(5 + 5) * 2 * (5 + 5)", "(((5 + 5) * 2) * (5 + 5))", },
    { "-(5 + 5)", "(-(5 + 5))", },
    { "!(true == true)", "(!(true == true))", },
		{ "a + add(b * c) + d", "((a + add((b * c))) + d)", },
		{ "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))", "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))", },
		{ "add(a + b + c * d / f + g)", "add((((a + b) + ((c * d) / f)) + g))", },
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

BOOST_AUTO_TEST_CASE(TestBooleanParsing){
  std::vector<std::pair<std::string, bool>> tests = {
    {"true;", true},
    {"false;", false},
  };

  for(auto [input, expected] : tests){
    monkey::lexer::Lexer l(input);
    Parser p(&l);
    auto program = p.parseProgram();
    checkParserErrors(p);
    BOOST_REQUIRE_NE(program, nullptr);
    BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
    auto stmt = program->statements[0].get();
    auto exprStmt = getAs<ExpressionStatement>(stmt);
    auto boolean = getAs<Boolean>(exprStmt->expression.get());
    BOOST_REQUIRE_EQUAL(boolean->value, expected);
    BOOST_REQUIRE_EQUAL(boolean->TokenLiteral(), expected ? "true" : "false");
  }
}

BOOST_AUTO_TEST_CASE(TestIfExpression){
  std::string input = "if (x < y) { x }";
  monkey::lexer::Lexer l(input);
  Parser p(&l);
  auto program = p.parseProgram();
  checkParserErrors(p);
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  auto ifExpr = getAs<IfExpression>(exprStmt->expression.get());
  auto condition = getAs<InfixExpression>(ifExpr->condition.get());
  BOOST_REQUIRE_EQUAL(condition->left->TokenLiteral(), "x");
  BOOST_REQUIRE_EQUAL(condition->op, "<");
  BOOST_REQUIRE_EQUAL(condition->right->TokenLiteral(), "y");
  BOOST_REQUIRE_EQUAL(ifExpr->consequence->statements.size(), 1);
  auto consequence = ifExpr->consequence->statements[0].get();
  auto consequenceExpr = getAs<ExpressionStatement>(consequence);
  BOOST_REQUIRE_EQUAL(consequenceExpr->expression->TokenLiteral(), "x");
  BOOST_REQUIRE_EQUAL(ifExpr->alternative, nullptr);
}

BOOST_AUTO_TEST_CASE(TestIfElseExpression){
  std::string input = "if (x < y) { x } else { y }";
  monkey::lexer::Lexer l(input);
  Parser p(&l);
  auto program = p.parseProgram();
  checkParserErrors(p);
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  auto ifExpr = getAs<IfExpression>(exprStmt->expression.get());
  auto condition = getAs<InfixExpression>(ifExpr->condition.get());
  BOOST_REQUIRE_EQUAL(condition->left->TokenLiteral(), "x");
  BOOST_REQUIRE_EQUAL(condition->op, "<");
  BOOST_REQUIRE_EQUAL(condition->right->TokenLiteral(), "y");
  BOOST_REQUIRE_EQUAL(ifExpr->consequence->statements.size(), 1);
  auto consequence = ifExpr->consequence->statements[0].get();
  auto consequenceExpr = getAs<ExpressionStatement>(consequence);
  BOOST_REQUIRE_EQUAL(consequenceExpr->expression->TokenLiteral(), "x");
  BOOST_REQUIRE_EQUAL(ifExpr->alternative->statements.size(), 1);
  auto alternative = ifExpr->alternative->statements[0].get();
  auto alternativeExpr = getAs<ExpressionStatement>(alternative);
  BOOST_REQUIRE_EQUAL(alternativeExpr->expression->TokenLiteral(), "y");
}

BOOST_AUTO_TEST_CASE(TestFunctionLiteralParsing){
  std::string input = "fn(x, y) { x + y; }";
  monkey::lexer::Lexer l(input);
  Parser p(&l);
  auto program = p.parseProgram();
  checkParserErrors(p);
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  auto func = getAs<FunctionLiteral>(exprStmt->expression.get());
  BOOST_REQUIRE_EQUAL(func->parameters.size(), 2);
  BOOST_REQUIRE_EQUAL(func->parameters[0]->TokenLiteral(), "x");
  BOOST_REQUIRE_EQUAL(func->parameters[1]->TokenLiteral(), "y");
  BOOST_REQUIRE_EQUAL(func->body->statements.size(), 1);
  auto bodyStmt = func->body->statements[0].get();
  auto bodyExpr = getAs<ExpressionStatement>(bodyStmt);
  auto bodyInfix = getAs<InfixExpression>(bodyExpr->expression.get());
  BOOST_REQUIRE_EQUAL(bodyInfix->left->TokenLiteral(), "x");
  BOOST_REQUIRE_EQUAL(bodyInfix->op, "+");
  BOOST_REQUIRE_EQUAL(bodyInfix->right->TokenLiteral(), "y");
}

BOOST_AUTO_TEST_CASE(TestFunctionParameters){
  std::vector<std::pair<std::string, std::vector<std::string>>> tests = {
    {"fn() {};", {}},
    {"fn(x) {};", {"x"}},
    {"fn(x, y, z) {};", {"x", "y", "z"}},
  };

  for(auto [input, expected] : tests){
    monkey::lexer::Lexer l(input);
    Parser p(&l);
    auto program = p.parseProgram();
    checkParserErrors(p);
    BOOST_REQUIRE_NE(program, nullptr);
    BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
    auto stmt = program->statements[0].get();
    auto exprStmt = getAs<ExpressionStatement>(stmt);
    auto func = getAs<FunctionLiteral>(exprStmt->expression.get());
    BOOST_REQUIRE_EQUAL(func->parameters.size(), expected.size());
    for(int i = 0; i < expected.size(); i++){
      BOOST_REQUIRE_EQUAL(func->parameters[i]->TokenLiteral(), expected[i]);
    }
  }
}

BOOST_AUTO_TEST_CASE(TestCallExpressions){
  std::string input = "add(1, 2 * 3, 4 + 5);";
  monkey::lexer::Lexer l(input);
  Parser p(&l);
  auto program = p.parseProgram();
  checkParserErrors(p);
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  auto call = getAs<CallExpression>(exprStmt->expression.get());
  BOOST_REQUIRE_EQUAL(call->function->TokenLiteral(), "add");
  BOOST_REQUIRE_EQUAL(call->arguments.size(), 3);
  BOOST_REQUIRE_EQUAL(call->arguments[0]->TokenLiteral(), "1");
  auto arg1 = getAs<InfixExpression>(call->arguments[1].get());
  BOOST_REQUIRE_EQUAL(arg1->left->TokenLiteral(), "2");
  BOOST_REQUIRE_EQUAL(arg1->op, "*");
  BOOST_REQUIRE_EQUAL(arg1->right->TokenLiteral(), "3");
  auto arg2 = getAs<InfixExpression>(call->arguments[2].get());
  BOOST_REQUIRE_EQUAL(arg2->left->TokenLiteral(), "4");
  BOOST_REQUIRE_EQUAL(arg2->op, "+");
  BOOST_REQUIRE_EQUAL(arg2->right->TokenLiteral(), "5");
}

BOOST_AUTO_TEST_CASE(TestCallArgumentsParsing){
  std::vector<std::pair<std::string, std::vector<std::string>>> tests = {
    {"add();", {}},
    {"add(1);", {"1"}},
    {"add(1, 2 * 3, 4 + 5);", {"1", "(2 * 3)", "(4 + 5)"}},
  };

  for(auto [input, expected] : tests){
    monkey::lexer::Lexer l(input);
    Parser p(&l);
    auto program = p.parseProgram();
    checkParserErrors(p);
    BOOST_REQUIRE_NE(program, nullptr);
    BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
    auto stmt = program->statements[0].get();
    auto exprStmt = getAs<ExpressionStatement>(stmt);
    auto call = getAs<CallExpression>(exprStmt->expression.get());
    BOOST_REQUIRE_EQUAL(call->arguments.size(), expected.size());
    for(int i = 0; i < expected.size(); i++){
      BOOST_REQUIRE_EQUAL(call->arguments[i]->to_string(), expected[i]);
    }
  }
}

BOOST_AUTO_TEST_CASE(TestStringLiteral){
  std::string input = R"("hello world";)";
  monkey::lexer::Lexer l(input);
  Parser p(&l);
  auto program = p.parseProgram();
  checkParserErrors(p);
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  auto literal = getAs<StringLiteral>(exprStmt->expression.get());
  BOOST_REQUIRE_EQUAL(literal->value, "hello world");
}