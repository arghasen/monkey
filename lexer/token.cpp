#include "token.hpp"
#include <ostream>
#include <unordered_map>

namespace monkey{
namespace lexer{
const std::unordered_map<std::string_view, TokenType> keywords = {
    {"fn"sv, FUNCTION},
    {"let"sv, LET},
    {"true"sv, TRUE},
    {"false"sv, FALSE},
    {"if"sv, IF},
    {"else"sv, ELSE},
    {"return"sv, RETURN}
    };

TokenType LookupIdent(const std::string &ident) {
    auto it = keywords.find(ident);
    if (it != keywords.end())
    {
        return it->second;
    }
    else
    {
        return IDENT;
    }
}

std::ostream& operator<<(std::ostream& os, const Token& tok) {
    os << "{ Type: " << tok.Type << ", Literal: " << tok.Literal << "}"; 
    return os;
}
} // namespace token
} // namespace monkey