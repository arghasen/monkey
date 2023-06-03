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
      tok.type = TokenType::EQ;
      tok.literal = "==";
    } else {
      tok.type = TokenType::ASSIGN;
      tok.literal = "=";
    }
    break;
  case '+':
    tok.type = TokenType::PLUS;
    tok.literal = "+";
    break;
  case '-':
    tok.type = TokenType::MINUS;
    tok.literal = "-";
    break;
  case '!':
    if (peekChar() == '=') {
      readChar();
      tok.type = TokenType::NOT_EQ;
      tok.literal = "!=";
    } else {
      tok.type = TokenType::BANG;
      tok.literal = "!";
    }
    break;
  case '/':
    tok.type = TokenType::SLASH;
    tok.literal = "/";
    break;
  case '*':
    tok.type = TokenType::ASTERISK;
    tok.literal = "*";
    break;
  case '<':
    tok.type = TokenType::LT;
    tok.literal = "<";
    break;
  case '>':
    tok.type = TokenType::GT;
    tok.literal = ">";
    break;
  case ';':
    tok.type = TokenType::SEMICOLON;
    tok.literal = ";";
    break;
  case '(':
    tok.type = TokenType::LPAREN;
    tok.literal = "(";
    break;
  case ')':
    tok.type = TokenType::RPAREN;
    tok.literal = ")";
    break;
  case ',':
    tok.type = TokenType::COMMA;
    tok.literal = ",";
    break;
  case '{':
    tok.type = TokenType::LBRACE;
    tok.literal = "{";
    break;
  case '}':
    tok.type = TokenType::RBRACE;
    tok.literal = "}";
    break;
  case '\0':
    tok.type = TokenType::EOFILE;
    tok.literal = "";
    break;
  case '"':
    tok.type = TokenType::STRING;
    tok.literal = readString();
    break;
  default:
    if (std::isalpha(ch_)) {
      tok.literal = readIdentifier();
      tok.type = LookupIdent(tok.literal);
      return tok;
    } else if (isdigit(ch_)) {
      tok.type = TokenType::INT;
      tok.literal = readNumber();
      return tok;
    } else {
      tok.type = TokenType::ILLEGAL;
      tok.literal = "";
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

std::string Lexer::readString() {
  int start = position_ + 1;
  do {
    readChar();
  } while (ch_ != '"' && ch_ != '\0');
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
