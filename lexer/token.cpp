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

std::string to_string(TokenType tok) {
  switch (tok) {
  case TokenType::ILLEGAL:
    return "ILLEGAL";
  case TokenType::EOFILE:
    return "EOFILE";
  case TokenType::IDENT:
    return "IDENT";
  case TokenType::INT:
    return "INT";
  case TokenType::ASSIGN:
    return "ASSIGN";
  case TokenType::PLUS:
    return "PLUS";
  case TokenType::MINUS:
    return "MINUS";
  case TokenType::BANG:
    return "BANG";
  case TokenType::ASTERISK:
    return "ASTERISK";
  case TokenType::SLASH:
    return "SLASH";
  case TokenType::LT:
    return "LT";
  case TokenType::GT:
    return "GT";
  case TokenType::EQ:
    return "EQ";
  case TokenType::NOT_EQ:
    return "NOT_EQ";
  case TokenType::COMMA:
    return "COMMA";
  case TokenType::SEMICOLON:
    return "SEMICOLON";
  case TokenType::LPAREN:
    return "LPAREN";
  case TokenType::RPAREN:
    return "RPAREN";
  case TokenType::LBRACE:
    return "LBRACE";
  case TokenType::RBRACE:
    return "RBRACE";
  case TokenType::FUNCTION:
    return "FUNCTION";
  case TokenType::LET:
    return "LET";
  case TokenType::TRUE:
    return "TRUE";
  case TokenType::FALSE:
    return "FALSE";
  case TokenType::IF:
    return "IF";
  case TokenType::ELSE:
    return "ELSE";
  case TokenType::RETURN:
    return "RETURN";
  case TokenType::STRING:
    return "STRING";
  default:
    return "";
  }
}

std::ostream &operator<<(std::ostream &os, TokenType tok) {
  os << to_string(tok);
  return os;
}

std::ostream &operator<<(std::ostream &os, const Token &tok) {
  os << "{ Type: " << tok.type << ", Literal: " << tok.literal << "}";
  return os;
}
} // namespace lexer
} // namespace monkey
