
#pragma once
#include <cstdint>
#include <string>

namespace monkey {
namespace lexer {

enum class TokenType {

  // constants
  ILLEGAL,
  EOFILE,

  // Identifiers + literals
  IDENT,
  INT,
  STRING,

  // operators
  ASSIGN,
  PLUS,
  MINUS,
  BANG,
  ASTERISK,
  SLASH,
  LT,
  GT,
  EQ,
  NOT_EQ,

  // delimiters
  COMMA,
  SEMICOLON,

  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,

  // keywords
  FUNCTION,
  LET,
  TRUE,
  FALSE,
  IF,
  ELSE,
  RETURN
};

struct Token {
  TokenType type;
  std::string literal;
  Token() = default;
  constexpr Token(TokenType type, std::string_view literal)
      : type(type), literal(literal){};
};

TokenType LookupIdent(const std::string &ident);

std::string to_string(TokenType tok);
std::ostream &operator<<(std::ostream &os, const Token &tok);
std::ostream &operator<<(std::ostream &os, TokenType tok);

} // namespace lexer
} // namespace monkey
