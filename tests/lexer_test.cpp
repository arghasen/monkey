#define BOOST_TEST_MODULE MonkeyInterpreterTest
#include "../lexer/lexer.hpp"
#include "../lexer/token.hpp"
#include <boost/test/unit_test.hpp>

using namespace monkey::lexer;

void checkTokens(std::string input, std::vector<Token> expectedTokens) {
  monkey::lexer::Lexer lexer(input);

  for (auto &expectedToken : expectedTokens) {
    auto tok = lexer.nextToken();
    BOOST_CHECK_EQUAL(tok, expectedToken);
  }
}

BOOST_AUTO_TEST_CASE(TestNextToken) {
  auto input = "=+(){},;";

  std::vector<Token> testResults = {
      {TokenType::ASSIGN, "="}, {TokenType::PLUS, "+"},
      {TokenType::LPAREN, "("}, {TokenType::RPAREN, ")"},
      {TokenType::LBRACE, "{"}, {TokenType::RBRACE, "}"},
      {TokenType::COMMA, ","},  {TokenType::SEMICOLON, ";"},
      {TokenType::EOFILE, ""}};
  checkTokens(input, testResults);
}

BOOST_AUTO_TEST_CASE(TestNextToken2) {
  auto input = R"(
        let five = 5;
        let ten = 10;
        let add = fn(x, y) {
            x + y;
        };
        let result = add(five, ten);
    )";

  std::vector<Token> testResults = {
      {TokenType::LET, "let"},     {TokenType::IDENT, "five"},
      {TokenType::ASSIGN, "="},    {TokenType::INT, "5"},
      {TokenType::SEMICOLON, ";"}, {TokenType::LET, "let"},
      {TokenType::IDENT, "ten"},   {TokenType::ASSIGN, "="},
      {TokenType::INT, "10"},      {TokenType::SEMICOLON, ";"},
      {TokenType::LET, "let"},     {TokenType::IDENT, "add"},
      {TokenType::ASSIGN, "="},    {TokenType::FUNCTION, "fn"},
      {TokenType::LPAREN, "("},    {TokenType::IDENT, "x"},
      {TokenType::COMMA, ","},     {TokenType::IDENT, "y"},
      {TokenType::RPAREN, ")"},    {TokenType::LBRACE, "{"},
      {TokenType::IDENT, "x"},     {TokenType::PLUS, "+"},
      {TokenType::IDENT, "y"},     {TokenType::SEMICOLON, ";"},
      {TokenType::RBRACE, "}"},    {TokenType::SEMICOLON, ";"},
      {TokenType::LET, "let"},     {TokenType::IDENT, "result"},
      {TokenType::ASSIGN, "="},    {TokenType::IDENT, "add"},
      {TokenType::LPAREN, "("},    {TokenType::IDENT, "five"},
      {TokenType::COMMA, ","},     {TokenType::IDENT, "ten"},
      {TokenType::RPAREN, ")"},    {TokenType::SEMICOLON, ";"}};
  checkTokens(input, testResults);
}

BOOST_AUTO_TEST_CASE(TestNextTokenOperators) {
  auto input = R"(
    !-/*5;
    5 < 10 > 5;
    10 == 10;
    10 != 9;
    )";

  std::vector<Token> testResults = {
      {TokenType::BANG, "!"},  {TokenType::MINUS, "-"},
      {TokenType::SLASH, "/"}, {TokenType::ASTERISK, "*"},
      {TokenType::INT, "5"},   {TokenType::SEMICOLON, ";"},
      {TokenType::INT, "5"},   {TokenType::LT, "<"},
      {TokenType::INT, "10"},  {TokenType::GT, ">"},
      {TokenType::INT, "5"},   {TokenType::SEMICOLON, ";"},
      {TokenType::INT, "10"},  {TokenType::EQ, "=="},
      {TokenType::INT, "10"},  {TokenType::SEMICOLON, ";"},
      {TokenType::INT, "10"},  {TokenType::NOT_EQ, "!="},
      {TokenType::INT, "9"},   {TokenType::SEMICOLON, ";"},
      {TokenType::EOFILE, ""}};

  checkTokens(input, testResults);
}

BOOST_AUTO_TEST_CASE(TestNextTokenKeywords) {
  auto input = R"(
    if (5 < 10) {
        return true;
    } else {
        return false;
    }
    )";

  std::vector<Token> testResults = {
      {TokenType::IF, "if"},       {TokenType::LPAREN, "("},
      {TokenType::INT, "5"},       {TokenType::LT, "<"},
      {TokenType::INT, "10"},      {TokenType::RPAREN, ")"},
      {TokenType::LBRACE, "{"},    {TokenType::RETURN, "return"},
      {TokenType::TRUE, "true"},   {TokenType::SEMICOLON, ";"},
      {TokenType::RBRACE, "}"},    {TokenType::ELSE, "else"},
      {TokenType::LBRACE, "{"},    {TokenType::RETURN, "return"},
      {TokenType::FALSE, "false"}, {TokenType::SEMICOLON, ";"},
      {TokenType::RBRACE, "}"},    {TokenType::EOFILE, ""}};

  checkTokens(input, testResults);
}

BOOST_AUTO_TEST_CASE(TestNextTokenString) {
  auto input = R"(
    "foobar"
    "foo bar"
    )";

  std::vector<Token> testResults = {{TokenType::STRING, "foobar"},
                                    {TokenType::STRING, "foo bar"},
                                    {TokenType::EOFILE, ""}};

  checkTokens(input, testResults);
}

BOOST_AUTO_TEST_CASE(TestTokenCheckConversionToString) {
  std::unordered_map<TokenType, std::string> testResults = {
      {TokenType::ILLEGAL, "ILLEGAL"},
      {TokenType::EOFILE, "EOFILE"},
      {TokenType::IDENT, "IDENT"},
      {TokenType::INT, "INT"},
      {TokenType::ASSIGN, "ASSIGN"},
      {TokenType::PLUS, "PLUS"},
      {TokenType::SEMICOLON, "SEMICOLON"},
      {TokenType::LPAREN, "LPAREN"},
      {TokenType::RPAREN, "RPAREN"},
      {TokenType::LBRACE, "LBRACE"},
      {TokenType::RBRACE, "RBRACE"},
      {TokenType::FUNCTION, "FUNCTION"},
      {TokenType::LET, "LET"},
      {TokenType::BANG, "BANG"},
      {TokenType::MINUS, "MINUS"},
      {TokenType::SLASH, "SLASH"},
      {TokenType::ASTERISK, "ASTERISK"},
      {TokenType::LT, "LT"},
      {TokenType::GT, "GT"},
      {TokenType::IF, "IF"},
      {TokenType::ELSE, "ELSE"},
      {TokenType::RETURN, "RETURN"},
      {TokenType::TRUE, "TRUE"},
      {TokenType::FALSE, "FALSE"},
      {TokenType::EQ, "EQ"},
      {TokenType::NOT_EQ, "NOT_EQ"},
      {TokenType::STRING, "STRING"},
      {TokenType::SLASH, "SLASH"},
      {TokenType::COMMA, "COMMA"},
      {TokenType::TRUE, "TRUE"},
      {TokenType::FALSE, "FALSE"}};

  for (auto &[tokenType, expectedString] : testResults) {
    BOOST_CHECK_EQUAL(to_string(tokenType), expectedString);
  }
}
