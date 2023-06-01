#include "lexer.hpp"
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

lexer::Token Lexer::NextToken() {
  Token tok;

  skipWhitespace();

  switch (ch_) {
  case '=':
    if (peekChar() == '=') {
      readChar();
      tok.Type = EQ;
      tok.Literal = "==";
    } else {
      tok.Type = ASSIGN;
      tok.Literal = "=";
    }
    break;
  case '+':
    tok.Type = PLUS;
    tok.Literal = "+";
    break;
  case '-':
    tok.Type = MINUS;
    tok.Literal = "-";
    break;
  case '!':
    if (peekChar() == '=') {
      readChar();
      tok.Type = NOT_EQ;
      tok.Literal = "!=";
    } else {
      tok.Type = BANG;
      tok.Literal = "!";
    }
    break;
  case '/':
    tok.Type = SLASH;
    tok.Literal = "/";
    break;
  case '*':
    tok.Type = ASTERISK;
    tok.Literal = "*";
    break;
  case '<':
    tok.Type = LT;
    tok.Literal = "<";
    break;
  case '>':
    tok.Type = GT;
    tok.Literal = ">";
    break;
  case ';':
    tok.Type = SEMICOLON;
    tok.Literal = ";";
    break;
  case '(':
    tok.Type = LPAREN;
    tok.Literal = "(";
    break;
  case ')':
    tok.Type = RPAREN;
    tok.Literal = ")";
    break;
  case ',':
    tok.Type = COMMA;
    tok.Literal = ",";
    break;
  case '{':
    tok.Type = LBRACE;
    tok.Literal = "{";
    break;
  case '}':
    tok.Type = RBRACE;
    tok.Literal = "}";
    break;
  case '\0':
    tok.Type = EOFILE;
    tok.Literal = "";
    break;
  default:
    if (std::isalpha(ch_)) {
      tok.Literal = readIdentifier();
      tok.Type = LookupIdent(tok.Literal);
      return tok;
    } else if (isdigit(ch_)) {
      tok.Type = INT;
      tok.Literal = readNumber();
      return tok;
    } else {
      tok.Type = ILLEGAL;
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
