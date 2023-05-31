#pragma once
#include <string>
#include <vector>
#include "token.hpp"

namespace monkey
{
namespace lexer
{
    class Lexer
    {
    public:
        Lexer(const std::string &input);
        token::Token NextToken();

    private:
        std::string input_;
        int position_;
        int read_position_;
        char ch_;
        void readChar();
        std::string readIdentifier();
        std::string readNumber();
        std::string readString();
        void skipWhitespace();
    };
} // namespace lexer
} // namespace monkey
