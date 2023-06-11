#include "lexer.hpp"
#include "token.hpp"
#include <cctype>

namespace monkey::lexer {

Lexer::Lexer(const std::string &input)
    : input_(input), position_(0), read_position_(0), ch_('\0') {
  readChar();
}

void Lexer::readChar() {
  if (read_position_ >= input_.length()) {
    ch_ = '\0';
  } else [[likely]]{
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
      tok = Token(TokenType::EQ, "==");
    } else {
     tok = Token(TokenType::ASSIGN, "=");
    }
    break;
  case '+':
    tok = Token(TokenType::PLUS, "+");
    break;
  case '-':
    tok = Token(TokenType::MINUS, "-");
    break;
  case '!':
    if (peekChar() == '=') {
      readChar();
      tok = Token(TokenType::NOT_EQ, "!=");
    } else {
        tok = Token(TokenType::BANG, "!");
    }
    break;
  case '/':
    tok = Token(TokenType::SLASH, "/");
    break;
  case '*':
    tok = Token(TokenType::ASTERISK, "*");
    break;
  case '<':
    tok = Token(TokenType::LT, "<");
    break;
  case '>':
    tok = Token(TokenType::GT, ">");
    break;
  case ';':
    tok = Token(TokenType::SEMICOLON, ";");
    break;
  case '(':
    tok = Token(TokenType::LPAREN, "(");
    break;
  case ')':
    tok = Token(TokenType::RPAREN, ")");
    break;
  case ',':
    tok = Token(TokenType::COMMA, ",");
    break;
  case '{':
    tok = Token(TokenType::LBRACE, "{");
    break;
  case '}':
    tok = Token(TokenType::RBRACE, "}");
    break;
  case '\0':
    tok = Token(TokenType::EOFILE, "");
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
} // namespace monkey::lexer
