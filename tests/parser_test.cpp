#include "../parser/parser.hpp"
#include <boost/test/unit_test.hpp>
#include <variant>

using namespace monkey::parser;
using namespace monkey::parser::ast;

using ParsedTypes = std::variant<int64_t, bool, std::string>;

template <typename T> T *getAs(AstChildNode auto *s) {
  auto stmt = dynamic_cast<T *>(s);
  if (stmt == nullptr) {
    BOOST_FAIL("s not *T. Got " + std::string(typeid(s).name()));
  }
  return stmt;
}

void testLetStatement(Statement *s, std::string name) {
  BOOST_REQUIRE_EQUAL(s->TokenLiteral(), "let");
  auto letStmt = getAs<LetStatement>(s);
  BOOST_REQUIRE_EQUAL(letStmt->name->value, name);
  BOOST_REQUIRE_EQUAL(letStmt->name->TokenLiteral(), name);
}

void testIntegerLiteral(ast::Expression *expr, int64_t value) {
  auto literal = getAs<ast::IntegerLiteral>(expr);
  BOOST_REQUIRE_EQUAL(literal->value, value);
  BOOST_REQUIRE_EQUAL(literal->TokenLiteral(), std::to_string(value));
}

void testIdentifier(ast::Expression *expr, std::string value) {
  auto ident = getAs<ast::Identifier>(expr);
  BOOST_REQUIRE_EQUAL(ident->value, value);
  BOOST_REQUIRE_EQUAL(ident->TokenLiteral(), value);
}

void testBooleanLiteral(ast::Expression *expr, bool value) {
  auto boolean = getAs<ast::Boolean>(expr);
  BOOST_REQUIRE_EQUAL(boolean->value, value);
  BOOST_REQUIRE_EQUAL(boolean->TokenLiteral(), value ? "true" : "false");
}

void testLiteralExpression(ast::Expression *expr, auto expected) {
  if constexpr (std::is_same_v<decltype(expected), int64_t>) {
    testIntegerLiteral(expr, expected);
  } else if constexpr (std::is_same_v<decltype(expected), std::string> ||
                       std::is_same_v<decltype(expected), const char *>) {
    testIdentifier(expr, expected);
  } else if constexpr (std::is_same_v<decltype(expected), bool>) {
    testBooleanLiteral(expr, expected);
  } else {
    BOOST_FAIL("type of expr not handled. Got " +
               std::string(typeid(expr).name()) + " expected " +
               std::string(typeid(expected).name()));
  }
}

void testInfixExpression(InfixExpression *expr, auto left, std::string op,
                         auto right) {
  BOOST_REQUIRE_EQUAL(expr->op, op);
  BOOST_REQUIRE_EQUAL(expr->TokenLiteral(), op);
  testLiteralExpression(expr->left.get(), left);
  testLiteralExpression(expr->right.get(), right);
}

void checkParserErrors(Parser &p) {
  auto errors = p.getErrors();
  if (errors.empty()) {
    return;
  }

  for (auto error : errors) {
    BOOST_ERROR("parser error: " + error);
  }
  BOOST_FAIL("parser has " + std::to_string(errors.size()) + " errors");
}

std::unique_ptr<ast::Program> testProgram(std::string input) {
  monkey::lexer::Lexer l(input);
  Parser p(&l);
  auto program = p.parseProgram();
  checkParserErrors(p);
  BOOST_REQUIRE_NE(program, nullptr);
  BOOST_REQUIRE_EQUAL(program->statements.size(), 1);
  return program;
}

BOOST_AUTO_TEST_CASE(TestLetStatements) {
  std::vector<std::tuple<std::string, std::string, int64_t>> tests = {
      {"let x = 5;", "x", 5},
      {"let y = 10;", "y", 10},
      {"let foobar = 833333;", "foobar", 833333},
  };

  for (auto &[input, expectedIdentifier, expectedValue] : tests) {
    auto program = testProgram(input);
    auto stmt = program->statements[0].get();
    testLetStatement(stmt, expectedIdentifier);
    auto expr = getAs<LetStatement>(stmt);
    testLiteralExpression(expr->value.get(), expectedValue);
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
  BOOST_REQUIRE_EQUAL(p.getErrors().size(), 8);
}

BOOST_AUTO_TEST_CASE(TestReturnStatements) {
  std::vector<std::tuple<std::string, int64_t>> tests = {
      {"return 5;", 5},
      {"return 10;", 10},
      {"return 993322;", 993322},
  };

  for (auto &[input, expectedValue] : tests) {
    auto program = testProgram(input);
    auto stmt = program->statements[0].get();
    auto returnStmt = getAs<ReturnStatement>(stmt);
    BOOST_REQUIRE_EQUAL(returnStmt->TokenLiteral(), "return");
    testLiteralExpression(returnStmt->returnValue.get(), expectedValue);
  }
}

BOOST_AUTO_TEST_CASE(TestIdentifierExpression) {
  auto input = "foobar;";
  auto program = testProgram(input);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  testIdentifier(exprStmt->expression.get(), "foobar");
}

BOOST_AUTO_TEST_CASE(TestIntegerLiteralExpression) {
  auto input = "5;";
  auto program = testProgram(input);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  testIntegerLiteral(exprStmt->expression.get(), 5);
}

BOOST_AUTO_TEST_CASE(TestParsingPrefixExpressions) {
  std::vector<std::tuple<std::string, std::string, ParsedTypes>> tests = {
      {"!5;", "!", 5},
      {"-15;", "-", 15},
      {"!true;", "!", true},
      {"!false;", "!", false},
  };
  for (auto &[input, op, value] : tests) {
    auto program = testProgram(input);
    auto stmt = program->statements[0].get();
    auto exprStmt = getAs<ExpressionStatement>(stmt);
    auto prefixExpr = getAs<PrefixExpression>(exprStmt->expression.get());
    BOOST_REQUIRE_EQUAL(prefixExpr->op, op);
    BOOST_REQUIRE_EQUAL(prefixExpr->TokenLiteral(), op);
    if (std::holds_alternative<int64_t>(value))
      testIntegerLiteral(prefixExpr->right.get(), std::get<int64_t>(value));
    else if (std::holds_alternative<bool>(value))
      testBooleanLiteral(prefixExpr->right.get(), std::get<bool>(value));
  }
}

BOOST_AUTO_TEST_CASE(TestParsingInfixExpressions) {

  std::vector<std::tuple<std::string, ParsedTypes, std::string, ParsedTypes>>
      tests = {
          {"5+5;", 5, "+", 5},
          {"5-5;", 5, "-", 5},
          {"5*5;", 5, "*", 5},
          {"5/5;", 5, "/", 5},
          {"5>5;", 5, ">", 5},
          {"5<5;", 5, "<", 5},
          {"5==5;", 5, "==", 5},
          {"5!=5;", 5, "!=", 5},
          {"foobar + barfoo;", "foobar", "+", "barfoo"},
          {"foobar - barfoo;", "foobar", "-", "barfoo"},
          {"foobar * barfoo;", "foobar", "*", "barfoo"},
          {"foobar / barfoo;", "foobar", "/", "barfoo"},
          {"foobar > barfoo;", "foobar", ">", "barfoo"},
          {"foobar < barfoo;", "foobar", "<", "barfoo"},
          {"foobar == barfoo;", "foobar", "==", "barfoo"},
          {"foobar != barfoo;", "foobar", "!=", "barfoo"},
          {"true == true", true, "==", true},
          {"true != false", true, "!=", false},
          {"false == false", false, "==", false},
      };
  auto testInfix = [](auto infixExpr, auto left, auto op, auto right) {
    if (std::holds_alternative<int64_t>(left) &&
        std::holds_alternative<int64_t>(right)) {
      testInfixExpression(infixExpr, std::get<int64_t>(left), op,
                          std::get<int64_t>(right));
    } else if (std::holds_alternative<bool>(left) &&
               std::holds_alternative<bool>(right)) {
      testInfixExpression(infixExpr, std::get<bool>(left), op,
                          std::get<bool>(right));
    } else if (std::holds_alternative<std::string>(left) &&
               std::holds_alternative<std::string>(right)) {
      testInfixExpression(infixExpr, std::get<std::string>(left), op,
                          std::get<std::string>(right));
    } else
      throw std::runtime_error("Invalid types");
  };
  for (auto [input, expectedLeft, expectedOp, expectedRight] : tests) {
    auto program = testProgram(input);
    auto stmt = program->statements[0].get();
    auto exprStmt = getAs<ExpressionStatement>(stmt);
    auto infixExpr = getAs<InfixExpression>(exprStmt->expression.get());
    testInfix(infixExpr, expectedLeft, expectedOp, expectedRight);
  }
}

BOOST_AUTO_TEST_CASE(TestOperatorPrecedenceParsing) {
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
      {"true", "true"},
      {"false", "false"},
      {"3 > 5 == false", "((3 > 5) == false)"},
      {"3 < 5 == true", "((3 < 5) == true)"},
      {"1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"},
      {"(5 + 5) * 2", "((5 + 5) * 2)"},
      {"2 / (5 + 5)", "(2 / (5 + 5))"},
      {"(5 + 5) * 2 * (5 + 5)", "(((5 + 5) * 2) * (5 + 5))"},
      {"-(5 + 5)", "(-(5 + 5))"},
      {"!(true == true)", "(!(true == true))"},
      {"a + add(b * c) + d", "((a + add((b * c))) + d)"},
      {"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
       "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"},
      {"add(a + b + c * d / f + g)", "add((((a + b) + ((c * d) / f)) + g))"},
  };

  for (auto [input, expectedResults] : tests) {
    monkey::lexer::Lexer l(input);
    Parser p(&l);
    auto program = p.parseProgram();
    checkParserErrors(p);
    BOOST_REQUIRE_NE(program, nullptr);
    auto actual = program->to_string();
    BOOST_REQUIRE_EQUAL(actual, expectedResults);
  }
}

BOOST_AUTO_TEST_CASE(TestBooleanParsing) {
  std::vector<std::pair<std::string, bool>> tests = {
      {"true;", true},
      {"false;", false},
  };

  for (auto [input, expected] : tests) {
    auto program = testProgram(input);
    auto stmt = program->statements[0].get();
    auto exprStmt = getAs<ExpressionStatement>(stmt);
    testBooleanLiteral(exprStmt->expression.get(), expected);
  }
}

BOOST_AUTO_TEST_CASE(TestIfExpression) {
  std::string input = "if (x < y) { x }";
  auto program = testProgram(input);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  auto ifExpr = getAs<IfExpression>(exprStmt->expression.get());
  auto condition = getAs<InfixExpression>(ifExpr->condition.get());
  testInfixExpression(condition, "x", "<", "y");
  BOOST_REQUIRE_EQUAL(ifExpr->consequence->statements.size(), 1);
  auto consequence = ifExpr->consequence->statements[0].get();
  auto consequenceExpr = getAs<ExpressionStatement>(consequence);
  BOOST_REQUIRE_EQUAL(consequenceExpr->expression->TokenLiteral(), "x");
  BOOST_REQUIRE_EQUAL(ifExpr->alternative, nullptr);
}

BOOST_AUTO_TEST_CASE(TestIfElseExpression) {
  std::string input = "if (x < y) { x } else { y }";
  auto program = testProgram(input);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  auto ifExpr = getAs<IfExpression>(exprStmt->expression.get());
  auto condition = getAs<InfixExpression>(ifExpr->condition.get());
  testInfixExpression(condition, "x", "<", "y");
  BOOST_REQUIRE_EQUAL(ifExpr->consequence->statements.size(), 1);
  auto consequence = ifExpr->consequence->statements[0].get();
  auto consequenceExpr = getAs<ExpressionStatement>(consequence);
  BOOST_REQUIRE_EQUAL(consequenceExpr->expression->TokenLiteral(), "x");
  BOOST_REQUIRE_EQUAL(ifExpr->alternative->statements.size(), 1);
  auto alternative = ifExpr->alternative->statements[0].get();
  auto alternativeExpr = getAs<ExpressionStatement>(alternative);
  BOOST_REQUIRE_EQUAL(alternativeExpr->expression->TokenLiteral(), "y");
}

BOOST_AUTO_TEST_CASE(TestFunctionLiteralParsing) {
  std::string input = "fn(x, y) { x + y; }";
  auto program = testProgram(input);
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
  testInfixExpression(bodyInfix, "x", "+", "y");
}

BOOST_AUTO_TEST_CASE(TestFunctionParameters) {
  std::vector<std::pair<std::string, std::vector<std::string>>> tests = {
      {"fn() {};", {}},
      {"fn(x) {};", {"x"}},
      {"fn(x, y, z) {};", {"x", "y", "z"}},
  };

  for (auto [input, expected] : tests) {
    auto program = testProgram(input);
    auto stmt = program->statements[0].get();
    auto exprStmt = getAs<ExpressionStatement>(stmt);
    auto func = getAs<FunctionLiteral>(exprStmt->expression.get());
    BOOST_REQUIRE_EQUAL(func->parameters.size(), expected.size());
    for (int i = 0; i < expected.size(); i++) {
      BOOST_REQUIRE_EQUAL(func->parameters[i]->TokenLiteral(), expected[i]);
    }
  }
}

BOOST_AUTO_TEST_CASE(TestCallExpressions) {
  std::string input = "add(1, 2 * 3, 4 + 5);";
  auto program = testProgram(input);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  auto call = getAs<CallExpression>(exprStmt->expression.get());
  BOOST_REQUIRE_EQUAL(call->function->TokenLiteral(), "add");
  BOOST_REQUIRE_EQUAL(call->arguments.size(), 3);
  BOOST_REQUIRE_EQUAL(call->arguments[0]->TokenLiteral(), "1");
  auto arg1 = getAs<InfixExpression>(call->arguments[1].get());
  testInfixExpression(arg1, int64_t(2), "*", int64_t(3));
  auto arg2 = getAs<InfixExpression>(call->arguments[2].get());
  testInfixExpression(arg2, int64_t(4), "+", int64_t(5));
}

BOOST_AUTO_TEST_CASE(TestCallArgumentsParsing) {
  std::vector<std::pair<std::string, std::vector<std::string>>> tests = {
      {"add();", {}},
      {"add(1);", {"1"}},
      {"add(1, 2 * 3, 4 + 5);", {"1", "(2 * 3)", "(4 + 5)"}},
  };

  for (auto [input, expected] : tests) {
    auto program = testProgram(input);
    auto stmt = program->statements[0].get();
    auto exprStmt = getAs<ExpressionStatement>(stmt);
    auto call = getAs<CallExpression>(exprStmt->expression.get());
    BOOST_REQUIRE_EQUAL(call->arguments.size(), expected.size());
    for (int i = 0; i < expected.size(); i++) {
      BOOST_REQUIRE_EQUAL(call->arguments[i]->to_string(), expected[i]);
    }
  }
}

BOOST_AUTO_TEST_CASE(TestStringLiteral) {
  std::string input = R"("hello world";)";
  auto program = testProgram(input);
  auto stmt = program->statements[0].get();
  auto exprStmt = getAs<ExpressionStatement>(stmt);
  auto literal = getAs<StringLiteral>(exprStmt->expression.get());
  BOOST_REQUIRE_EQUAL(literal->value, "hello world");
}

BOOST_AUTO_TEST_CASE(TestArrayLiterals){
    std::string input = "[1, 2 * 2, 3 + 3]";
    auto program = testProgram(input);
    auto stmt = program->statements[0].get();
    auto exprStmt = getAs<ExpressionStatement>(stmt);
    auto array = getAs<ArrayLiteral>(exprStmt->expression.get());
    BOOST_REQUIRE_EQUAL(array->elements.size(), 3);
    testIntegerLiteral(array->elements[0].get(), 1);
    auto arg1 = getAs<InfixExpression>(array->elements[1].get());
    testInfixExpression(arg1, int64_t(2), "*", int64_t(2));
    auto arg2 = getAs<InfixExpression>(array->elements[2].get());
    testInfixExpression(arg2, int64_t(3), "+", int64_t(3));
}
