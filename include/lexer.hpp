#ifndef __lexer_hpp
#define __lexer_hpp
#include "token.hpp"
#include <cstdint>
#include <string>
using namespace std;
struct Lexer {
  string src;
  uint32_t CurrentPosition;
  uint32_t BeginingPosition;
  uint32_t LineNo;

  Lexer(string &src)
      : src(src), CurrentPosition(0), BeginingPosition(0), LineNo(1) {};

  Token NextToken();

private:
  char peek(int);
  char next();
  bool match(char);
  void SkipWhitespaces();
};

#endif