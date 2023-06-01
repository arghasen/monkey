#define BOOST_TEST_MODULE MonkeyInterpreterTest
#include <boost/test/unit_test.hpp>
#include "../lexer/token.hpp"
#include "../lexer/lexer.hpp"

using namespace monkey::lexer;
BOOST_AUTO_TEST_CASE(TestNextToken) {
    auto input = "=+(){},;";

    std::vector<Token> testResults = {
        {ASSIGN, "="},
        {PLUS, "+"},
        {LPAREN, "("},
        {RPAREN, ")"},
        {LBRACE, "{"},
        {RBRACE, "}"},
        {COMMA, ","},
        {SEMICOLON, ";"},
        {EOFILE, ""}
    };

    monkey::lexer::Lexer lexer(input);

    for (auto& [expectedType, expectedLiteral] : testResults) {
        auto tok = lexer.nextToken();
        BOOST_CHECK_EQUAL(tok.Type, expectedType);
        BOOST_CHECK_EQUAL(tok.Literal, expectedLiteral);
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
        {LET, "let"},
        {IDENT, "five"},
        {ASSIGN, "="},
        {INT, "5"},
        {SEMICOLON, ";"},
        {LET, "let"},
        {IDENT, "ten"},
        {ASSIGN, "="},
        {INT, "10"},
        {SEMICOLON, ";"},
        {LET, "let"},
        {IDENT, "add"},
        {ASSIGN, "="},
        {FUNCTION, "fn"},
        {LPAREN, "("},
        {IDENT, "x"},
        {COMMA, ","},
        {IDENT, "y"},
        {RPAREN, ")"},
        {LBRACE, "{"},
        {IDENT, "x"},
        {PLUS, "+"},
        {IDENT, "y"},
        {SEMICOLON, ";"},
        {RBRACE, "}"},
        {SEMICOLON, ";"},
        {LET, "let"},
        {IDENT, "result"},
        {ASSIGN, "="},
        {IDENT, "add"},
        {LPAREN, "("},
        {IDENT, "five"},
        {COMMA, ","},
        {IDENT, "ten"},
        {RPAREN, ")"},
        {SEMICOLON, ";"},
    };

    monkey::lexer::Lexer lexer(input);
    for (auto& [expectedType, expectedLiteral] : testResults) {
        auto tok = lexer.nextToken();
        BOOST_CHECK_EQUAL(tok.Type, expectedType);
        BOOST_CHECK_EQUAL(tok.Literal, expectedLiteral);
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
        {BANG, "!"},
        {MINUS, "-"},
        {SLASH, "/"},
        {ASTERISK, "*"},
        {INT, "5"},
        {SEMICOLON, ";"},
        {INT, "5"},
        {LT, "<"},
        {INT, "10"},
        {GT, ">"},
        {INT, "5"},
        {SEMICOLON, ";"},
        {INT, "10"},
        {EQ, "=="},
        {INT, "10"},
        {SEMICOLON, ";"},
        {INT, "10"},
        {NOT_EQ, "!="},
        {INT, "9"},
        {SEMICOLON, ";"},
        {EOFILE, ""}
    };

    monkey::lexer::Lexer lexer(input);
    for (auto& [expectedType, expectedLiteral] : testResults) {
        auto tok = lexer.nextToken();
        BOOST_CHECK_EQUAL(tok.Type, expectedType);
        BOOST_CHECK_EQUAL(tok.Literal, expectedLiteral);
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
        {IF, "if"},
        {LPAREN, "("},
        {INT, "5"},
        {LT, "<"},
        {INT, "10"},
        {RPAREN, ")"},
        {LBRACE, "{"},
        {RETURN, "return"},
        {TRUE, "true"},
        {SEMICOLON, ";"},
        {RBRACE, "}"},
        {ELSE, "else"},
        {LBRACE, "{"},
        {RETURN, "return"},
        {FALSE, "false"},
        {SEMICOLON, ";"},
        {RBRACE, "}"},
        {EOFILE, ""}
    };

    monkey::lexer::Lexer lexer(input);
    for (auto& [expectedType, expectedLiteral] : testResults) {
        auto tok = lexer.nextToken();
        BOOST_CHECK_EQUAL(tok.Type, expectedType);
        BOOST_CHECK_EQUAL(tok.Literal, expectedLiteral);
    }
}
