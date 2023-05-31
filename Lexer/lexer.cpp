#include "lexer.hpp"
#include <cctype>

namespace monkey {
namespace lexer {

Lexer::Lexer(const std::string &input) : input_(input), position_(0), read_position_(0), ch_('\0') {
    readChar();
}

void Lexer::readChar() {
    if (read_position_ >= input_.length()) {
        ch_ = '\0';
    } else {
        ch_ = input_[read_position_];
    }
    position_ = read_position_;
    read_position_++;
}

token::Token Lexer::NextToken() {
    token::Token tok;
    switch (ch_) {
        case '=':
            tok.Type = token::ASSIGN;
            tok.Literal = "=";
            break;
        case '+':
            tok.Type = token::PLUS;
            tok.Literal = "+";
            break;
        case ';':
            tok.Type = token::SEMICOLON;
            tok.Literal = ";";
            break;
        case '(':
            tok.Type = token::LPAREN;
            tok.Literal = "(";
            break;
        case ')':
            tok.Type = token::RPAREN;
            tok.Literal = ")";
            break;
        case ',':
            tok.Type = token::COMMA;
            tok.Literal = ",";
            break;
        case '{':
            tok.Type = token::LBRACE;
            tok.Literal = "{";
            break;
        case '}':
            tok.Type = token::RBRACE;
            tok.Literal = "}";
            break;
        case '\0':
            tok.Type = token::EOFILE;
            tok.Literal = "";
            break;
        default:
            if (std::isalpha(ch_)) {
                tok.Literal = readIdentifier();
                tok.Type = token::LookupIdent(tok.Literal);
                return tok;
            } else {
                tok.Type = token::ILLEGAL;
                tok.Literal = "";
            }
    }

    readChar();
    return tok;
}
std::string Lexer::readIdentifier() {
    int start = position_;
    while (isalpha(ch_)) {
        readChar();
    }
    return input_.substr(start, position_ - start);
}

} // namespace lexer
} // namespace monkey