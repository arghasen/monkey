#include "lexer.hpp"
#include "token.hpp"
#include <cctype>

namespace monkey {
namespace lexer {

Lexer::Lexer(const std::string &input)
    : input_(input), position_(0), read_position_(0), ch_('\0') {
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

lexer::Token Lexer::nextToken() {
  Token tok;

  skipWhitespace();

  switch (ch_) {
  case '=':
    if (peekChar() == '=') {
      readChar();
      tok.Type = TokenType::EQ;
      tok.Literal = "==";
    } else {
      tok.Type = TokenType::ASSIGN;
      tok.Literal = "=";
    }
    break;
  case '+':
    tok.Type = TokenType::PLUS;
    tok.Literal = "+";
    break;
  case '-':
    tok.Type = TokenType::MINUS;
    tok.Literal = "-";
    break;
  case '!':
    if (peekChar() == '=') {
      readChar();
      tok.Type = TokenType::NOT_EQ;
      tok.Literal = "!=";
    } else {
      tok.Type = TokenType::BANG;
      tok.Literal = "!";
    }
    break;
  case '/':
    tok.Type = TokenType::SLASH;
    tok.Literal = "/";
    break;
  case '*':
    tok.Type = TokenType::ASTERISK;
    tok.Literal = "*";
    break;
  case '<':
    tok.Type = TokenType::LT;
    tok.Literal = "<";
    break;
  case '>':
    tok.Type = TokenType::GT;
    tok.Literal = ">";
    break;
  case ';':
    tok.Type = TokenType::SEMICOLON;
    tok.Literal = ";";
    break;
  case '(':
    tok.Type = TokenType::LPAREN;
    tok.Literal = "(";
    break;
  case ')':
    tok.Type = TokenType::RPAREN;
    tok.Literal = ")";
    break;
  case ',':
    tok.Type = TokenType::COMMA;
    tok.Literal = ",";
    break;
  case '{':
    tok.Type = TokenType::LBRACE;
    tok.Literal = "{";
    break;
  case '}':
    tok.Type = TokenType::RBRACE;
    tok.Literal = "}";
    break;
  case '\0':
    tok.Type = TokenType::EOFILE;
    tok.Literal = "";
    break;
  default:
    if (std::isalpha(ch_)) {
      tok.Literal = readIdentifier();
      tok.Type = LookupIdent(tok.Literal);
      return tok;
    } else if (isdigit(ch_)) {
      tok.Type = TokenType::INT;
      tok.Literal = readNumber();
      return tok;
    } else {
      tok.Type = TokenType::ILLEGAL;
      tok.Literal = "";
    }
  }

  readChar();
  return tok;
}

std::string Lexer::readIdentifier() {
  int start = position_;
  while (std::isalpha(ch_)) {
    readChar();
  }
  return input_.substr(start, position_ - start);
}

void Lexer::skipWhitespace() {
  while (std::isspace(ch_)) {
    readChar();
  }
}

std::string Lexer::readNumber() {
  int start = position_;
  while (std::isdigit(ch_)) {
    readChar();
  }
  return input_.substr(start, position_ - start);
}

char Lexer::peekChar() {
  if (read_position_ >= input_.length()) {
    return '\0';
  } else {
    return input_[read_position_];
  }
}
} // namespace lexer
} // namespace monkey
