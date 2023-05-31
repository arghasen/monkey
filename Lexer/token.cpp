#include "token.hpp"

namespace monkey{
namespace token{
    const std::unordered_map<std::string_view, TokenType> keywords = {
        {"fn"sv, FUNCTION},
        {"let"sv, LET}};

    TokenType LookupIdent(const std::string &ident) {
        if (ident == "fn")
        {
            return FUNCTION;
        }
        else if (ident == "let")
        {
            return LET;
        }
        else
        {
            return IDENT;
        }
    }
} // namespace token
} // namespace monkey