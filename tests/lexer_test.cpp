#define BOOST_TEST_MODULE LexerTest
#include <boost/test/unit_test.hpp>
#include "../Lexer/token.hpp"
#include "../Lexer/lexer.hpp"

BOOST_AUTO_TEST_CASE(TestNextToken) {
    auto input = "=+(){},;";

    std::vector testResults = {
        std::make_pair(monkey::token::ASSIGN, "="),
        std::make_pair(monkey::token::PLUS, "+"),
        std::make_pair(monkey::token::LPAREN, "("),
        std::make_pair(monkey::token::RPAREN, ")"),
        std::make_pair(monkey::token::LBRACE, "{"),
        std::make_pair(monkey::token::RBRACE, "}"),
        std::make_pair(monkey::token::COMMA, ","),
        std::make_pair(monkey::token::SEMICOLON, ";"),
        std::make_pair(monkey::token::EOFILE, "")
    };

    monkey::lexer::Lexer lexer(input);

    for (auto& [expectedType, expectedLiteral] : testResults) {
        auto tok = lexer.NextToken();
        BOOST_CHECK_EQUAL(tok.Type, expectedType);
        BOOST_CHECK_EQUAL(tok.Literal, expectedLiteral);
    }
}