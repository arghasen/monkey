#define BOOST_TEST_MODULE LexerTest
#include <boost/test/unit_test.hpp>
#include "../Lexer/token.hpp"
#include "../Lexer/lexer.hpp"

using namespace monkey::token;
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
        auto tok = lexer.NextToken();
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

    monkey::lexer::Lexer lexer(input);
    std::cout <<input <<std::endl;

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

    for (auto& [expectedType, expectedLiteral] : testResults) {
        auto tok = lexer.NextToken();
        BOOST_CHECK_EQUAL(tok.Type, expectedType);
        BOOST_CHECK_EQUAL(tok.Literal, expectedLiteral);
    }
}