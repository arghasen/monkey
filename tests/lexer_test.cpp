#define BOOST_TEST_MODULE MonkeyInterpreterTest
#include <boost/test/unit_test.hpp>
#include "../lexer/token.hpp"
#include "../lexer/lexer.hpp"

using namespace monkey::lexer;
BOOST_AUTO_TEST_CASE(TestNextToken) {
    auto input = "=+(){},;";

    std::vector<Token> testResults = {
        {TokenType::ASSIGN, "="},
        {TokenType::PLUS, "+"},
        {TokenType::LPAREN, "("},
        {TokenType::RPAREN, ")"},
        {TokenType::LBRACE, "{"},
        {TokenType::RBRACE, "}"},
        {TokenType::COMMA, ","},
        {TokenType::SEMICOLON, ";"},
        {TokenType::EOFILE, ""}
    };

    monkey::lexer::Lexer lexer(input);

    for (auto& [expectedType, expectedLiteral] : testResults) {
        auto tok = lexer.nextToken();
        BOOST_CHECK_EQUAL(tok.type, expectedType);
        BOOST_CHECK_EQUAL(tok.literal, expectedLiteral);
    }
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
        {TokenType::LET, "let"},
        {TokenType::IDENT, "five"},
        {TokenType::ASSIGN, "="},
        {TokenType::INT, "5"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::LET, "let"},
        {TokenType::IDENT, "ten"},
        {TokenType::ASSIGN, "="},
        {TokenType::INT, "10"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::LET, "let"},
        {TokenType::IDENT, "add"},
        {TokenType::ASSIGN, "="},
        {TokenType::FUNCTION, "fn"},
        {TokenType::LPAREN, "("},
        {TokenType::IDENT, "x"},
        {TokenType::COMMA, ","},
        {TokenType::IDENT, "y"},
        {TokenType::RPAREN, ")"},
        {TokenType::LBRACE, "{"},
        {TokenType::IDENT, "x"},
        {TokenType::PLUS, "+"},
        {TokenType::IDENT, "y"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::RBRACE, "}"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::LET, "let"},
        {TokenType::IDENT, "result"},
        {TokenType::ASSIGN, "="},
        {TokenType::IDENT, "add"},
        {TokenType::LPAREN, "("},
        {TokenType::IDENT, "five"},
        {TokenType::COMMA, ","},
        {TokenType::IDENT, "ten"},
        {TokenType::RPAREN, ")"},
        {TokenType::SEMICOLON, ";"},
    };

    monkey::lexer::Lexer lexer(input);
    for (auto& [expectedType, expectedLiteral] : testResults) {
        auto tok = lexer.nextToken();
        BOOST_CHECK_EQUAL(tok.type, expectedType);
        BOOST_CHECK_EQUAL(tok.literal, expectedLiteral);
    }
}

BOOST_AUTO_TEST_CASE(TestNextTokenOperators) {
    auto input = R"(
    !-/*5;
    5 < 10 > 5;
    10 == 10;
    10 != 9;
    )";

    std::vector<Token> testResults = {
        {TokenType::BANG, "!"},
        {TokenType::MINUS, "-"},
        {TokenType::SLASH, "/"},
        {TokenType::ASTERISK, "*"},
        {TokenType::INT, "5"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::INT, "5"},
        {TokenType::LT, "<"},
        {TokenType::INT, "10"},
        {TokenType::GT, ">"},
        {TokenType::INT, "5"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::INT, "10"},
        {TokenType::EQ, "=="},
        {TokenType::INT, "10"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::INT, "10"},
        {TokenType::NOT_EQ, "!="},
        {TokenType::INT, "9"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::EOFILE, ""}
    };

    monkey::lexer::Lexer lexer(input);
    for (auto& [expectedType, expectedLiteral] : testResults) {
        auto tok = lexer.nextToken();
        BOOST_CHECK_EQUAL(tok.type, expectedType);
        BOOST_CHECK_EQUAL(tok.literal, expectedLiteral);
    }
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
        {TokenType::IF, "if"},
        {TokenType::LPAREN, "("},
        {TokenType::INT, "5"},
        {TokenType::LT, "<"},
        {TokenType::INT, "10"},
        {TokenType::RPAREN, ")"},
        {TokenType::LBRACE, "{"},
        {TokenType::RETURN, "return"},
        {TokenType::TRUE, "true"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::RBRACE, "}"},
        {TokenType::ELSE, "else"},
        {TokenType::LBRACE, "{"},
        {TokenType::RETURN, "return"},
        {TokenType::FALSE, "false"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::RBRACE, "}"},
        {TokenType::EOFILE, ""}
    };

    monkey::lexer::Lexer lexer(input);
    for (auto& [expectedType, expectedLiteral] : testResults) {
        auto tok = lexer.nextToken();
        BOOST_CHECK_EQUAL(tok.type, expectedType);
        BOOST_CHECK_EQUAL(tok.literal, expectedLiteral);
    }
}

BOOST_AUTO_TEST_CASE(TestNextTokenString) {
    auto input = R"(
    "foobar"
    "foo bar"
    )";

    std::vector<Token> testResults = {
        {TokenType::STRING, "foobar"},
        {TokenType::STRING, "foo bar"},
        {TokenType::EOFILE, ""}
    };

    monkey::lexer::Lexer lexer(input);
    for (auto& [expectedType, expectedLiteral] : testResults) {
        auto tok = lexer.nextToken();
        BOOST_CHECK_EQUAL(tok.type, expectedType);
        BOOST_CHECK_EQUAL(tok.literal, expectedLiteral);
    }
}