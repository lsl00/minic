#include "token.hpp"
#include <string>

std::ostream &operator<<(std::ostream &os, Token &token) {
  switch (token.type) {
  case TokenType::LeftParen:
    os << "LeftParen";
    break;
  case TokenType::RightParen:
    os << "RightParen";
    break;
  case TokenType::LeftBrace:
    os << "LeftBrace";
    break;
  case TokenType::RightBrace:
    os << "RightBrace";
    break;
  case TokenType::LeftBracket:
    os << "LeftBracket";
    break;
  case TokenType::RightBracket:
    os << "RightBracket";
    break;
  case TokenType::Semicolon:
    os << "Semicolon";
    break;
  case TokenType::Comma:
    os << "Comma";
    break;

  case TokenType::OpAdd:
    os << "OpAdd";
    break;
  case TokenType::OpSub:
    os << "OpSub";
    break;
  case TokenType::OpMul:
    os << "OpMul";
    break;
  case TokenType::OpDiv:
    os << "OpDiv";
    break;
  case TokenType::OpMod:
    os << "OpMod";
    break;

  case TokenType::OpEqual:
    os << "OpEqual";
    break;
  case TokenType::OpEqualEqual:
    os << "OpEqualEqual";
    break;

  case TokenType::OpGreater:
    os << "OpGreater";
    break;
  case TokenType::OpGreaterEqual:
    os << "OpGreaterEqual";
    break;
  case TokenType::OpLess:
    os << "OpLess";
    break;
  case TokenType::OpLessEqual:
    os << "OpLessEqual";
    break;
  case TokenType::OpNotEqual:
    os << "OpNotEqual";
    break;
  case TokenType::OpNot:
    os << "OpNot";
    break;
  case TokenType::OpAnd:
    os << "OpAnd";
    break;
  case TokenType::OpOr:
    os << "OpOr";
    break;


  case TokenType::KW_int:
    os << "int";
    break;
  case TokenType::KW_float:
    os << "float";
    break;
  case TokenType::KW_return:
    os << "return";
    break;
  case TokenType::KW_if:
    os << "if";
    break;
  case TokenType::KW_else:
    os << "else";
    break;
  case TokenType::KW_void:
    os << "void";
    break;
  case TokenType::KW_break:
    os << "break";
    break;
  case TokenType::KW_continue:
    os << "continue";
    break;
  case TokenType::KW_const:
    os << "const";
    break;
  case TokenType::KW_while:
    os << "while";
    break;

  case TokenType::Integer:
    os << "Integer(" << std::get<int>(token.content) << ")";
    break;
  case TokenType::Float:
    os << "Float(" << std::get<float>(token.content) << ")";
    break;
  case TokenType::Identifier:
    os << "Identifier(" << std::get<std::string>(token.content) << ")";
    break;
  case TokenType::String:
    os << "String(" << std::get<std::string>(token.content) << ")";
    break;
  case TokenType::Eof:
    break;
  case TokenType::Error:
    os << "LexError!";
    break;
  }

  return os;
}
Token SimpleToken(TokenType type) {
  Token Result;
  Result.type = type;
  return Result;
}

Token IntegerToken(int v) {
  Token Result;
  Result.type = TokenType::Integer;
  Result.content = v;
  return Result;
}

Token FloatToken(float v) {
  Token Result;
  Result.type = TokenType::Float;
  Result.content = v;
  return Result;
}

Token IdentifierToken(std::string name) {
  Token Result;
  Result.type = TokenType::Identifier;
  Result.content = name;
  return Result;
}

Token StringToken(std::string str) {
  Token Result;
  Result.type = TokenType::String;
  Result.content = str;
  return Result;
}

bool Token::isKind(TokenType type) { return this->type == type; }
bool Token::isBinOp() { return ::isBinOp(this->type); }
bool Token::isPreOp() { return ::isPreOp(this->type); }
bool Token::isNotOp() { return ::isNotOp(this->type); }
bool Token::isCmpOp() { return ::isCmpOp(this->type); }
bool Token::isLogicOp() { return ::isLogicOp(this->type); }


bool isBinOp(TokenType type) {
  switch (type) {
  case TokenType::OpAdd:
  case TokenType::OpSub:
  case TokenType::OpMul:
  case TokenType::OpDiv:
  case TokenType::OpMod:
    return true;
  default:
    return false;
  }
}
bool isPreOp(TokenType type) {
  switch (type) {
  case TokenType::OpAdd:
  case TokenType::OpSub:
    return true;
  default:
    return false;
  }
}
bool isCmpOp(TokenType type) {
  switch (type) {
  case TokenType::OpGreater:
  case TokenType::OpGreaterEqual:
  case TokenType::OpLess:
  case TokenType::OpLessEqual:
  case TokenType::OpEqualEqual:
  case TokenType::OpNotEqual:
    return true;
  default:
    return false;
  }
}

bool isLogicOp(TokenType type) {
  switch (type) {
  case TokenType::OpAnd:
  case TokenType::OpOr:
    return true;
  default:
    return false;
  }
}

bool isNotOp(TokenType type){
  if(type == TokenType::OpNot){
    return true;
  }else {
    return false;
  }
}