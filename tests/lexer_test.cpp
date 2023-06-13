#define BOOST_TEST_MODULE MonkeyInterpreterTest
#include "../lexer/lexer.hpp"
#include "../lexer/token.hpp"
#include <boost/test/unit_test.hpp>

using namespace monkey::lexer;
using TT = TokenType;
using TokenNames = std::pair<TT, std::string>;

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
      {TT::ASSIGN, "="}, {TT::PLUS, "+"},      {TT::LPAREN, "("},
      {TT::RPAREN, ")"}, {TT::LBRACE, "{"},    {TT::RBRACE, "}"},
      {TT::COMMA, ","},  {TT::SEMICOLON, ";"}, {TT::EOFILE, ""}};
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
      {TT::LET, "let"},      {TT::IDENT, "five"},  {TT::ASSIGN, "="},
      {TT::INT, "5"},        {TT::SEMICOLON, ";"}, {TT::LET, "let"},
      {TT::IDENT, "ten"},    {TT::ASSIGN, "="},    {TT::INT, "10"},
      {TT::SEMICOLON, ";"},  {TT::LET, "let"},     {TT::IDENT, "add"},
      {TT::ASSIGN, "="},     {TT::FUNCTION, "fn"}, {TT::LPAREN, "("},
      {TT::IDENT, "x"},      {TT::COMMA, ","},     {TT::IDENT, "y"},
      {TT::RPAREN, ")"},     {TT::LBRACE, "{"},    {TT::IDENT, "x"},
      {TT::PLUS, "+"},       {TT::IDENT, "y"},     {TT::SEMICOLON, ";"},
      {TT::RBRACE, "}"},     {TT::SEMICOLON, ";"}, {TT::LET, "let"},
      {TT::IDENT, "result"}, {TT::ASSIGN, "="},    {TT::IDENT, "add"},
      {TT::LPAREN, "("},     {TT::IDENT, "five"},  {TT::COMMA, ","},
      {TT::IDENT, "ten"},    {TT::RPAREN, ")"},    {TT::SEMICOLON, ";"}};
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
      {TT::BANG, "!"},      {TT::MINUS, "-"},     {TT::SLASH, "/"},
      {TT::ASTERISK, "*"},  {TT::INT, "5"},       {TT::SEMICOLON, ";"},
      {TT::INT, "5"},       {TT::LT, "<"},        {TT::INT, "10"},
      {TT::GT, ">"},        {TT::INT, "5"},       {TT::SEMICOLON, ";"},
      {TT::INT, "10"},      {TT::EQ, "=="},       {TT::INT, "10"},
      {TT::SEMICOLON, ";"}, {TT::INT, "10"},      {TT::NOT_EQ, "!="},
      {TT::INT, "9"},       {TT::SEMICOLON, ";"}, {TT::EOFILE, ""}};

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
      {TT::IF, "if"},       {TT::LPAREN, "("},      {TT::INT, "5"},
      {TT::LT, "<"},        {TT::INT, "10"},        {TT::RPAREN, ")"},
      {TT::LBRACE, "{"},    {TT::RETURN, "return"}, {TT::TRUE, "true"},
      {TT::SEMICOLON, ";"}, {TT::RBRACE, "}"},      {TT::ELSE, "else"},
      {TT::LBRACE, "{"},    {TT::RETURN, "return"}, {TT::FALSE, "false"},
      {TT::SEMICOLON, ";"}, {TT::RBRACE, "}"},      {TT::EOFILE, ""}};

  checkTokens(input, testResults);
}

BOOST_AUTO_TEST_CASE(TestNextTokenString) {
  auto input = R"(
    "foobar"
    "foo bar"
    )";

  std::vector<Token> testResults = {
      {TT::STRING, "foobar"}, {TT::STRING, "foo bar"}, {TT::EOFILE, ""}};

  checkTokens(input, testResults);
}

BOOST_AUTO_TEST_CASE(TestTokenCheckConversionToString) {
  std::vector<TokenNames> testResults = {{TT::ILLEGAL, "ILLEGAL"},
                                         {TT::EOFILE, "EOFILE"},
                                         {TT::IDENT, "IDENT"},
                                         {TT::INT, "INT"},
                                         {TT::ASSIGN, "ASSIGN"},
                                         {TT::PLUS, "PLUS"},
                                         {TT::SEMICOLON, "SEMICOLON"},
                                         {TT::LPAREN, "LPAREN"},
                                         {TT::RPAREN, "RPAREN"},
                                         {TT::LBRACE, "LBRACE"},
                                         {TT::RBRACE, "RBRACE"},
                                         {TT::FUNCTION, "FUNCTION"},
                                         {TT::LET, "LET"},
                                         {TT::BANG, "BANG"},
                                         {TT::MINUS, "MINUS"},
                                         {TT::SLASH, "SLASH"},
                                         {TT::ASTERISK, "ASTERISK"},
                                         {TT::LT, "LT"},
                                         {TT::GT, "GT"},
                                         {TT::IF, "IF"},
                                         {TT::ELSE, "ELSE"},
                                         {TT::RETURN, "RETURN"},
                                         {TT::TRUE, "TRUE"},
                                         {TT::FALSE, "FALSE"},
                                         {TT::EQ, "EQ"},
                                         {TT::NOT_EQ, "NOT_EQ"},
                                         {TT::STRING, "STRING"},
                                         {TT::SLASH, "SLASH"},
                                         {TT::COMMA, "COMMA"},
                                         {TT::TRUE, "TRUE"},
                                         {TT::FALSE, "FALSE"}};

  for (auto &[tokenType, expectedString] : testResults) {
    BOOST_CHECK_EQUAL(to_string(tokenType), expectedString);
  }
}

BOOST_AUTO_TEST_CASE(TestArray) {
  auto input = R"(
        [1, 2];
        )";

  std::vector<Token> testResults = {{TT::LBRACKET, "["}, {TT::INT, "1"},
                                    {TT::COMMA, ","},    {TT::INT, "2"},
                                    {TT::RBRACKET, "]"}, {TT::SEMICOLON, ";"},
                                    {TT::EOFILE, ""}};

  checkTokens(input, testResults);
}
