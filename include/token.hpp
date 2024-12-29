#ifndef __token_hpp
#define __token_hpp
#include <ostream>
#include <string>
#include <variant>
enum class TokenType {
  // Punctuations
  LeftParen,  // '('
  RightParen, // ')'
  LeftBrace,  // '{'
  RightBrace, // '}'
  LeftBracket, // '['
  RightBracket, // ']'
  Semicolon,  // ';'
  Comma, // ','

  // Operators
  OpAdd,
  OpSub,
  OpMul,
  OpDiv,
  OpMod,

  OpEqual, //'='
  OpEqualEqual, // '=='

  OpGreater, //'>'
  OpGreaterEqual, // '>='
  OpLess, // '<'
  OpLessEqual, // '<='
  OpNotEqual, // '!='

  OpNot, // '!'
  OpAnd, // '&&'
  OpOr, // '||'

  // keywords
  KW_break,
  KW_const,
  KW_continue,
  KW_else,
  KW_float,
  KW_if,
  KW_int,
  KW_return,
  KW_while,
  KW_void,

  // Others
  Identifier,
  Integer,
  Float,
  String,
  Eof,

  Error,
};

struct Empty {};
struct Token {
  TokenType type;
  std::variant<Empty, int, float, std::string> content;

  bool isKind(TokenType type);
  bool isBinOp();
  bool isPreOp();
  bool isNotOp();
  bool isCmpOp();
  bool isLogicOp();
};
std::ostream &operator<<(std::ostream &, Token &);

Token SimpleToken(TokenType);
Token IntegerToken(int);
Token FloatToken(float);
Token IdentifierToken(std::string);
Token StringToken(std::string);
// std::string TokenType2String(TokenType type);

bool isBinOp(TokenType);
bool isPreOp(TokenType);
bool isNotOp(TokenType);
bool isCmpOp(TokenType);
bool isLogicOp(TokenType);
#endif