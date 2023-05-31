
#include <string>

namespace monkey{
namespace token
{
    using TokenType = std::string;

    struct Token
    {
        TokenType Type;
        std::string Literal;
    };

    // constants
    constexpr TokenType ILLEGAL = "ILLEGAL";
    constexpr TokenType EOFILE = "EOF";
    
    // Identifiers + literals
    constexpr TokenType IDENT = "IDENT";
    constexpr TokenType INT = "INT";
    
    // operators
    constexpr TokenType ASSIGN = "=";
    constexpr TokenType PLUS = "+";

    // delimiters
    constexpr TokenType COMMA = ",";
    constexpr TokenType SEMICOLON = ";";

    constexpr TokenType LPAREN = "(";
    constexpr TokenType RPAREN = ")";
    constexpr TokenType LBRACE = "{";
    constexpr TokenType RBRACE = "}";

    // keywords
    constexpr TokenType FUNCTION = "FUNCTION";
    constexpr TokenType LET = "LET";
    
} // namespace token
} // namespace monkey
