#include "lexer.hpp"
#include "token.hpp"
#include <cmath>
#include <cstdio>
#include <format>

bool isalpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
bool isdigit(char c) { return '0' <= c && c <= '9'; }
bool check(Lexer &lexer, const char *Content, int length) {
  string &src = lexer.src;
  if (length != lexer.CurrentPosition - lexer.BeginingPosition) {
    return false;
  }
  for (int i = 0; i < length; ++i) {
    if (src[lexer.BeginingPosition + i] != Content[i]) {
      return false;
    }
  }
  return true;
}
Token Lexer::NextToken() {
  SkipWhitespaces();

  BeginingPosition = CurrentPosition;
  char head = next();

  switch (head) { // Punctuation or operators.
  case ',':
    return SimpleToken(TokenType::Comma);
  case '[':
    return SimpleToken(TokenType::LeftBracket);
  case ']':
    return SimpleToken(TokenType::RightBracket);
  case '(':
    return SimpleToken(TokenType::LeftParen);
  case ')':
    return SimpleToken(TokenType::RightParen);
  case '{':
    return SimpleToken(TokenType::LeftBrace);
  case '}':
    return SimpleToken(TokenType::RightBrace);
  case ';':
    return SimpleToken(TokenType::Semicolon);
  case '+':
    return SimpleToken(TokenType::OpAdd);
  case '-':
    return SimpleToken(TokenType::OpSub);
  case '*':
    return SimpleToken(TokenType::OpMul);
  case '/':
    return SimpleToken(TokenType::OpDiv);
  case '%':
    return SimpleToken(TokenType::OpMod);

  case '=': {
    if (match('=')) {
      return SimpleToken(TokenType::OpEqualEqual);
    }
    return SimpleToken(TokenType::OpEqual);
  }
  case '>': {
    if (match('=')) {
      return SimpleToken(TokenType::OpGreaterEqual);
    }
    return SimpleToken(TokenType::OpGreater);
  }
  case '<': {
    if (match('=')) {
      return SimpleToken(TokenType::OpLessEqual);
    }
    return SimpleToken(TokenType::OpLess);
  }
  case '!': {
    if (match('=')) {
      return SimpleToken(TokenType::OpNotEqual);
    }
    return SimpleToken(TokenType::OpNot);
  }
  case '&': {
    // TODO : bitwise and
    if (match('&')) {
      return SimpleToken(TokenType::OpAnd);
    }
    throw format("Expect '&'.");
  }
  case '|': {
    if (match('|')) {
      return SimpleToken(TokenType::OpOr);
    }
    throw format("Expect '|'.");
  }

  case EOF:
    return SimpleToken(TokenType::Eof);
  default:
    break;
  }
  if (head == '"') {
    while (peek(0) != '"') {
      next();
    }
    next();
    return StringToken(src.substr(BeginingPosition + 1,
                                  CurrentPosition - BeginingPosition - 2));
  }
  if (isalpha(head)) { // Keywords or identifier.
    while (isalpha(peek(0)) || isdigit(peek(0)))
      next();

    switch (head) {
    case 'b': {
      if (check(*this, "break", 5)) {
        return SimpleToken(TokenType::KW_break);
      }
    } break;
    case 'c': {
      if (check(*this, "const", 5)) {
        return SimpleToken(TokenType::KW_const);
      }
      if (check(*this, "continue", 8)) {
        return SimpleToken(TokenType::KW_continue);
      }
    } break;
    case 'e': {
      if (check(*this, "else", 4)) {
        return SimpleToken(TokenType::KW_else);
      }
    } break;
    case 'f': {
      if (check(*this, "float", 5)) {
        return SimpleToken(TokenType::KW_float);
      }
    } break;
    case 'i': {
      if (check(*this, "int", 3)) {
        return SimpleToken(TokenType::KW_int);
      }
      if (check(*this, "if", 2)) {
        return SimpleToken(TokenType::KW_if);
      }
    } break;
    case 'r': {
      if (check(*this, "return", 6)) {
        return SimpleToken(TokenType::KW_return);
      }
    } break;
    case 'v': {
      if (check(*this, "void", 4)) {
        return SimpleToken(TokenType::KW_void);
      }
    } break;
    case 'w': {
      if (check(*this, "while", 5)) {
        return SimpleToken(TokenType::KW_while);
      }
    } break;
    }
    return IdentifierToken(
        src.substr(BeginingPosition, CurrentPosition - BeginingPosition));
  }

  if (isdigit(head)) {
    int Result = head - '0';
    while (isdigit(peek(0))) {
      char d = next();
      Result = 10 * Result + (d - '0');
    }
    float fs = Result;
    if (match('.')) {
      float frac = 0.1;
      while (isdigit(peek(0))) {
        char d = next();
        fs += frac * (d - '0');
        frac *= 0.1;
      }
      if (peek(0) != 'e') {
        return FloatToken(fs);
      }
    }
    if (match('e')) {
      int exp10 = 0, neg = 1;
      if (match('-'))
        neg = -1;
      while (isdigit(peek(0))) {
        char d = next();
        exp10 = 10 * exp10 + (d - '0');
      }
      return FloatToken(fs * exp(log(10) * neg * exp10));
    }
    return IntegerToken(Result);
  }

  return SimpleToken(TokenType::Eof);
}

char Lexer::peek(int step) {
  if (CurrentPosition + step >= src.size()) {
    return EOF;
  }
  return src[CurrentPosition + step];
}
bool Lexer::match(char c) {
  if (src[CurrentPosition] == c) {
    CurrentPosition++;
    return true;
  }
  return false;
}
char Lexer::next() {
  char Result = peek(0);
  CurrentPosition++;
  return Result;
}

void Lexer::SkipWhitespaces() {
  while (true) {
    switch (peek(0)) {
    case '\n':
      LineNo++;
    case '\t':
    case ' ':
      CurrentPosition++;
      continue;
    }
    break;
  }
}