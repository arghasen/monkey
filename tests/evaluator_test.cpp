#include "../eval/evaluator.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"

#include <boost/test/unit_test.hpp>
#include <optional>

using namespace monkey::evaluator;

void testIntegerObject(const Object &obj, int64_t expected) {
  BOOST_CHECK_EQUAL(obj.type(), INTEGER_OBJ);
  BOOST_CHECK_EQUAL(dynamic_cast<const Integer &>(obj).value_, expected);
}

void testBooleanObject(const Object &obj, bool expected) {
  BOOST_CHECK_EQUAL(obj.type(), BOOLEAN_OBJ);
  BOOST_CHECK_EQUAL(dynamic_cast<const Boolean &>(obj).value_, expected);
}

void testNullObject(const Object &obj) {
  BOOST_CHECK_EQUAL(obj.type(), NULL_OBJ);
}

ObjectPtr testEval(const std::string &input) {
  auto l = monkey::lexer::Lexer(input);
  auto p = monkey::parser::Parser(&l);
  auto program = p.parseProgram();
  auto evaluator = Evaluator();
  auto env = std::make_shared<EnvironmentImpl>();
  return evaluator.eval(program.get(), env);
}

BOOST_AUTO_TEST_CASE(TestEvalIntegerExpressions) {
  struct Test {
    std::string input;
    int64_t expected;
  };

  std::vector<Test> tests = {{"5", 5},
                             {"10", 10},
                             {"-5", -5},
                             {"-10", -10},
                             {"5 + 5 + 5 + 5 - 10", 10},
                             {"2 * 2 * 2 * 2 * 2", 32},
                             {"-50 + 100 + -50", 0},
                             {"5 * 2 + 10", 20},
                             {"5 + 2 * 10", 25},
                             {"20 + 2 * -10", 0},
                             {"50 / 2 * 2 + 10", 60},
                             {"2 * (5 + 10)", 30},
                             {"3 * 3 * 3 + 10", 37},
                             {"3 * (3 * 3) + 10", 37},
                             {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50}};

  for (auto &[input, expected] : tests) {
    auto evaluated = testEval(input);
    testIntegerObject(*evaluated, expected);
  }
}

BOOST_AUTO_TEST_CASE(TestEvalBooleanExpressions) {
  struct Test {
    std::string input;
    bool expected;
  };

  std::vector<Test> tests = {{"true", true},
                             {"false", false},
                             {"1 < 2", true},
                             {"1 > 2", false},
                             {"1 < 1", false},
                             {"1 > 1", false},
                             {"1 == 1", true},
                             {"1 != 1", false},
                             {"1 == 2", false},
                             {"1 != 2", true},
                             {"true == true", true},
                             {"false == false", true},
                             {"true == false", false},
                             {"true != false", true},
                             {"false != true", true},
                             {"(1 < 2) == true", true},
                             {"(1 < 2) == false", false},
                             {"(1 > 2) == true", false},
                             {"(1 > 2) == false", true}};

  for (auto &[input, expected] : tests) {
    auto evaluated = testEval(input);
    testBooleanObject(*evaluated, expected);
  }
}

BOOST_AUTO_TEST_CASE(TestBangOperator) {
  struct Test {
    std::string input;
    bool expected;
  };

  std::vector<Test> tests = {{"!true", false},   {"!false", true},
                             {"!5", false},      {"!!true", true},
                             {"!!false", false}, {"!!5", true}};

  for (auto &[input, expected] : tests) {
    auto evaluated = testEval(input);
    testBooleanObject(*evaluated, expected);
  }
}

BOOST_AUTO_TEST_CASE(TestIfElseExpressions) {
  struct Test {
    std::string input;
    std::optional<int64_t> expected;
  };

  std::vector<Test> tests = {{"if (true) { 10 }", 10},
                             {"if (false) { 10 }", std::nullopt},
                             {"if (1) { 10 }", 10},
                             {"if (1 < 2) { 10 }", 10},
                             {"if (1 > 2) { 10 }", std::nullopt},
                             {"if (1 > 2) { 10 } else { 20 }", 20},
                             {"if (1 < 2) { 10 } else { 20 }", 10}};

  for (auto &[input, expected] : tests) {
    auto evaluated = testEval(input);
    if (expected) {
      testIntegerObject(*evaluated, *expected);
    } else {
      testNullObject(*evaluated);
    }
  }
}

BOOST_AUTO_TEST_CASE(TestEvalReturnStatements) {
  struct Test {
    std::string input;
    int64_t expected;
  };

  std::vector<Test> tests = {
      {"return 10;", 10},
      {"return 10; 9;", 10},
      {"return 2 * 5; 9;", 10},
      {"9; return 2 * 5; 9;", 10},
      {"if (10 > 1) { if (10 > 1) { return 10; } return 1; }", 10}};

  for (auto &[input, expected] : tests) {
    auto evaluated = testEval(input);
    // std::cout << (*evaluated).type() << std::endl;
    testIntegerObject(*evaluated, expected);
  }
}

BOOST_AUTO_TEST_CASE(TestErrorHandling) {
  struct Test {
    std::string input;
    std::string expectedMessage;
  };

  std::vector<Test> tests = {
      {"5 + true;", "type mismatch: INTEGER + BOOLEAN"},
      {"5 + true; 5;", "type mismatch: INTEGER + BOOLEAN"},
      {"-true", "unknown operator: - BOOLEAN"},
      {"true + false;", "unknown operator: BOOLEAN + BOOLEAN"},
      {"5; true + false; 5", "unknown operator: BOOLEAN + BOOLEAN"},
      {"if (10 > 1) { true + false; }", "unknown operator: BOOLEAN + BOOLEAN"},
      {"if (10 > 1) { if (10 > 1) { return true + false; } return 1; }",
       "unknown operator: BOOLEAN + BOOLEAN"},
      {"foobar", "identifier not found: foobar"},
      // {"\"Hello\" - \"World\"", "unknown operator: STRING - STRING"},
      // {"{\"name\": \"Monkey\"}[fn(x) { x }];", "unusable as hash key:
      // FUNCTION"}
  };

  for (auto &[input, expectedMessage] : tests) {
    auto evaluated = testEval(input);
    BOOST_CHECK_EQUAL(evaluated->type(), ERROR_OBJ);
    BOOST_CHECK_EQUAL(dynamic_cast<const Error &>(*evaluated).message_,
                      expectedMessage);
  }
}

BOOST_AUTO_TEST_CASE(TestEvalLetStatements) {
  struct Test {
    std::string input;
    int64_t expected;
  };

  std::vector<Test> tests = {
      {"let a = 5; a;", 5},
      {"let a = 5 * 5; a;", 25},
      {"let a = 5; let b = a; b;", 5},
      {"let a = 5; let b = a; let c = a + b + 5; c;", 15}};

  for (auto &[input, expected] : tests) {
    auto evaluated = testEval(input);
    testIntegerObject(*evaluated, expected);
  }
}

BOOST_AUTO_TEST_CASE(TestEvalFunctionObject) {
  auto input = "fn(x) { x + 2; };";
  auto evaluated = testEval(input);
  BOOST_CHECK_EQUAL(evaluated->type(), FUNCTION_OBJ);
  auto fn = dynamic_cast<const Function *>(evaluated.get());
  BOOST_CHECK_EQUAL(fn->parameters.size(), 1);
  BOOST_CHECK_EQUAL(fn->parameters[0]->to_string(), "x");
  BOOST_CHECK_EQUAL(fn->body->to_string(), "(x + 2)");
}

BOOST_AUTO_TEST_CASE(TestEvalFunctionApplication) {
  struct Test {
    std::string input;
    int64_t expected;
  };

  std::vector<Test> tests = {
      {"let identity = fn(x) { x; }; identity(5);", 5},
      {"let identity = fn(x) { return x; }; identity(5);", 5},
      {"let double = fn(x) { x * 2; }; double(5);", 10},
      {"let add = fn(x, y) { x + y; }; add(5, 5);", 10},
      {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
      {"fn(x) { x; }(5)", 5}};

  for (auto &[input, expected] : tests) {
    auto evaluated = testEval(input);
    testIntegerObject(*evaluated, expected);
  }
}

BOOST_AUTO_TEST_CASE(TestClosures) {
  auto input = R"(
        let newAdder = fn(x) {
            fn(y) { x + y };
        };
        let addTwo = newAdder(2);
        addTwo(2);
    )";
  auto evaluated = testEval(input);
  testIntegerObject(*evaluated, 4);
}

BOOST_AUTO_TEST_CASE(TestEvalStringLiteral) {
  auto input = R"("Hello World!")";
  auto evaluated = testEval(input);
  BOOST_CHECK_EQUAL(evaluated->type(), STRING_OBJ);
  auto str = dynamic_cast<const String *>(evaluated.get());
  BOOST_CHECK_EQUAL(str->value_, "Hello World!");
}