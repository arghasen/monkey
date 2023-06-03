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
  return evaluator.eval(program.get());
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

  std::vector<Test> tests = {
      {"if (true) { 10 }", 10},
      {"if (false) { 10 }", std::nullopt},
      {"if (1) { 10 }", 10},
      {"if (1 < 2) { 10 }", 10},
      {"if (1 > 2) { 10 }", std::nullopt},
      {"if (1 > 2) { 10 } else { 20 }", 20},
      {"if (1 < 2) { 10 } else { 20 }", 10}
  };

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
      {"if (10 > 1) { if (10 > 1) { return 10; } return 1; }", 10}
  };

  for (auto &[input, expected] : tests) {
    auto evaluated = testEval(input);
    std::cout << (*evaluated).type() << std::endl;
    testIntegerObject(*evaluated, expected);
  }
}