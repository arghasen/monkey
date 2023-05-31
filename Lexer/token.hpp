
#pragma once
#include <string>
#include <unordered_map>

namespace monkey{
namespace token
{
    using TokenType = std::string_view;
    using namespace std::string_view_literals;
    struct Token
    {
        TokenType Type;
        std::string Literal;
        Token() = default;
        constexpr Token(TokenType type, std::string_view literal) : Type(type), Literal(literal) {};
    };

    // constants
    constexpr TokenType ILLEGAL = "ILLEGAL"sv;
    constexpr TokenType EOFILE = "EOF"sv;
    
    // Identifiers + literals
    constexpr TokenType IDENT = "IDENT"sv;
    constexpr TokenType INT = "INT"sv;
    
    // operators
    constexpr TokenType ASSIGN = "="sv;
    constexpr TokenType PLUS = "+"sv;

    // delimiters
    constexpr TokenType COMMA = ","sv;
    constexpr TokenType SEMICOLON = ";"sv;

    constexpr TokenType LPAREN = "("sv;
    constexpr TokenType RPAREN = ")"sv;
    constexpr TokenType LBRACE = "{"sv;
    constexpr TokenType RBRACE = "}"sv;

    // keywords
    constexpr TokenType FUNCTION = "FUNCTION";
    constexpr TokenType LET = "LET";

    TokenType LookupIdent(const std::string &ident);

} // namespace token
} // namespace monkey
