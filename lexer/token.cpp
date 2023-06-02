#include "token.hpp"
#include <ostream>
#include <unordered_map>

namespace monkey {
namespace lexer {
using namespace std::string_view_literals;
const std::unordered_map<std::string_view, TokenType> keywords = {
    {"fn"sv, TokenType::FUNCTION},  {"let"sv, TokenType::LET},
    {"true"sv, TokenType::TRUE},    {"false"sv, TokenType::FALSE},
    {"if"sv, TokenType::IF},        {"else"sv, TokenType::ELSE},
    {"return"sv, TokenType::RETURN}};

TokenType LookupIdent(const std::string &ident) {
  auto it = keywords.find(ident);
  if (it != keywords.end()) {
    return it->second;
  } else {
    return TokenType::IDENT;
  }
}

std::ostream &operator<<(std::ostream &os, TokenType tok ){

  switch(tok){
    case TokenType::ILLEGAL: os << "ILLEGAL";break;
    case TokenType::EOFILE: os << "EOFILE";break;
    case TokenType::IDENT: os << "IDENT";break;
    case TokenType::INT: os << "INT";break;
    case TokenType::ASSIGN: os << "ASSIGN";break;
    case TokenType::PLUS: os << "PLUS";break;
    case TokenType::MINUS: os << "MINUS";break;
    case TokenType::BANG: os << "BANG";break;
    case TokenType::ASTERISK: os << "ASTERISK";break;
    case TokenType::SLASH: os << "SLASH";break;
    case TokenType::LT: os << "LT";break;
    case TokenType::GT: os << "GT";break;
    case TokenType::EQ: os << "EQ";break;
    case TokenType::NOT_EQ: os << "NOT_EQ";break;
    case TokenType::COMMA: os << "COMMA";break;
    case TokenType::SEMICOLON: os << "SEMICOLON";break;
    case TokenType::LPAREN: os << "LPAREN";break;
    case TokenType::RPAREN: os << "RPAREN";break;
    case TokenType::LBRACE: os << "LBRACE";break;
    case TokenType::RBRACE: os << "RBRACE";break;
    case TokenType::FUNCTION: os << "FUNCTION";break;
    case TokenType::LET: os << "LET";break;
    case TokenType::TRUE: os << "TRUE";break;
    case TokenType::FALSE: os << "FALSE";break;
    case TokenType::IF: os << "IF";break;
    case TokenType::ELSE: os << "ELSE";break;
    case TokenType::RETURN: os << "RETURN";break;
    default: os << "UNKNOWN";break;
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const Token &tok) {
  os << "{ Type: " << tok.type << ", Literal: " << tok.literal << "}";
  return os;
}
} // namespace lexer
} // namespace monkey
