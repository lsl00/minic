#ifndef __parser_hpp
#define __parser_hpp
#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"
#include <cstdint>
#include <memory>
#include <vector>

struct Parser {

  Parser(Lexer &);

  ast::Program ParseProgram();

  vector<Token> src;
  int Current;

  shared_ptr<ast::Expr> ParseExpr();
  shared_ptr<ast::Expr> ParseExpr(int8_t min_bp);

  ast::Func ParseFunction();
  shared_ptr<ast::Stmt> ParseStmt();
  shared_ptr<ast::IfStmt> ParseIfStmt();
  ast::DeclStmt ParseDeclStmt();
  shared_ptr<ast::ReturnStmt> ParseReturnStmt();
  shared_ptr<ast::BlockStmt> ParseBlockStmt();
  shared_ptr<ast::WhileStmt> ParseWhileStmt();

  Token next();
  Token expect(TokenType, const char *);
  bool match(TokenType);
  Token &peek(int);
};

#endif