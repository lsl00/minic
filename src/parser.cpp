#include "parser.hpp"
#include "ast.hpp"
#include "common.hpp"
#include "lexer.hpp"
#include "token.hpp"
#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>
#include <variant>
#include <vector>

using namespace ast;

Parser::Parser(Lexer &lexer) : Current(0) {
  while (true) {
    Token t = lexer.NextToken();
    src.push_back(t);
    if (t.isKind(TokenType::Eof)) {
      break;
    }
  }
}
const int PREC_ASSIGN = 1, PREC_OR = 3, PREC_AND = 5, PREC_EQUAL = 7,
          PREC_COMPARE = 9, PREC_ADD = 11, PREC_MUL = 13, PREC_UNARY = 15,
          PREC_INDEX_CALL = 17;
int8_t PrefixBindPower() { return PREC_UNARY; }
pair<int8_t, int8_t> InfixBindPower(TokenType kind) {
  switch (kind) {
  case TokenType::OpAdd:
  case TokenType::OpSub:
    return make_pair(PREC_ADD, PREC_ADD + 1);
  case TokenType::OpMul:
  case TokenType::OpDiv:
  case TokenType::OpMod:
    return make_pair(PREC_MUL, PREC_MUL + 1);

  case TokenType::OpEqualEqual:
  case TokenType::OpNotEqual:
    return make_pair(PREC_EQUAL, PREC_EQUAL + 1);
  case TokenType::OpGreater:
  case TokenType::OpGreaterEqual:
  case TokenType::OpLess:
  case TokenType::OpLessEqual:
    return make_pair(PREC_COMPARE, PREC_COMPARE + 1);

  case TokenType::OpAnd:
    return make_pair(PREC_AND, PREC_AND + 1);
  case TokenType::OpOr:
    return make_pair(PREC_OR, PREC_OR + 1);
  case TokenType::OpEqual:
    return make_pair(PREC_ASSIGN + 1, PREC_EQUAL);

  default: {
    return make_pair(-1, -1);
  }
  }
}
shared_ptr<Expr> Parser::ParseExpr(int8_t min_bp) {
  shared_ptr<Expr> lhs = nullptr;
  Token token = next();
  if (token.isKind(TokenType::Integer)) {
    // a int literal
    lhs = make_shared<IntegerExpr>(std::get<int>(token.content));
  } else if (token.isKind(TokenType::Float)) {
    // a float literal
    lhs = make_shared<FloatExpr>(get<float>(token.content));
  } else if (token.isKind(TokenType::Identifier)) {
    // variable or function call
    string &VarName = std::get<string>(token.content);
    if (match(TokenType::LeftParen)) {
      vector<shared_ptr<Expr>> params;
      while (!peek(0).isKind(TokenType::RightParen)) {
        params.push_back(ParseExpr());
        if (!peek(0).isKind(TokenType::RightParen)) {
          expect(TokenType::Comma, "Expect ',' between parameters.");
        }
      }
      expect(TokenType::RightParen, "Expect ')' after parameter(s).");
      lhs = make_shared<FunctionCallExpr>(VarName,params);
    } else {
      lhs = make_shared<VariableExpr>(VarName);
    }
  } else if (token.isNotOp()) {
    // Not op
    int8_t bp = PrefixBindPower();
    lhs = make_shared<NotExpr>(ParseExpr(bp));
  } else if (token.isNegOp()) {
    // negate op
    int8_t bp = PrefixBindPower();
    lhs = make_shared<NegateExpr>(ParseExpr(bp));
  } else if (token.isKind(TokenType::LeftParen)) {
    // '('
    lhs = ParseExpr(0);
    expect(TokenType::RightParen, "Expect ')'");
  }

  while (true) {
    Token optoken = peek(0);

    if (optoken.isKind(TokenType::LeftBracket)) {
      next();
      auto index = ParseExpr();
      expect(TokenType::RightBracket, "Expect ']' after index.");

      if (auto base = dynamic_pointer_cast<VariableExpr>(lhs);
          base != nullptr) {
        lhs = make_shared<IndexExpr>(base, index);
      } else if (auto sub = dynamic_pointer_cast<IndexExpr>(lhs);
                 lhs != nullptr) {
        lhs = make_shared<IndexExpr>(sub, index);
      } else {
        throw format("Expect array or subarray in index expr.");
      }
      continue;
    }

    auto bp = InfixBindPower(optoken.type);
    int8_t l_bp = bp.first;
    int8_t r_bp = bp.second;
    if (l_bp < min_bp)
      break;
    next();
    auto rhs = ParseExpr(r_bp);
    if (optoken.isBinOp()) {
      BinOpKind op = BinOpKindFromTokenType(optoken.type);
      lhs = make_shared<BinopExpr>(op, lhs, rhs);
    } else if (optoken.isCmpOp()) {
      CmpOpKind op = CmpKindFromTokenType(optoken.type);
      lhs = make_shared<CmpExpr>(op, lhs, rhs);
    } else if (optoken.isLogicOp()) {
      LogicalOpKind op = LogicalOpKindFromTokenType(optoken.type);
      lhs = make_shared<LogicalExpr>(op, lhs, rhs);
    } else if(optoken.isKind(TokenType::OpEqual)){
      if(auto v = dynamic_pointer_cast<VariableExpr>(lhs); v != nullptr){
        lhs = make_shared<AssignExpr>(v,rhs);
      }else if(auto v = dynamic_pointer_cast<IndexExpr>(lhs); v != nullptr){
        lhs = make_shared<AssignExpr>(v,rhs);
      }else{
        throw string("Can only assign to variable or element of array.");
      }
    }
  }
  return lhs;
}

shared_ptr<Expr> Parser::ParseExpr() { return ParseExpr(0); }

shared_ptr<Stmt> Parser::ParseStmt() {
  switch (peek(0).type) {
  case TokenType::Semicolon:{
    return nullptr;
  }
  case TokenType::KW_continue: {
    next();
    return make_shared<ContinueStmt>();
  }
  case TokenType::KW_break: {
    next();
    return make_shared<BreakStmt>();
  }
  case TokenType::KW_if: {
    return ParseIfStmt();
  }
  case TokenType::KW_while: {
    return ParseWhileStmt();
  }
  case TokenType::KW_return: {
    return ParseReturnStmt();
  }
  case TokenType::KW_const:
  case TokenType::KW_int:
  case TokenType::KW_float: {
    return make_shared<DeclStmt>(ParseDeclStmt());
  }
  case TokenType::LeftBrace: {
    return ParseBlockStmt();
  }
  default: {
    auto e = ParseExpr();
    expect(TokenType::Semicolon, "Expect ';' after expr.");
    return make_shared<ExprStmt>(e);
  }
  }
  stringstream ss;
  ss << peek(0);
  unreachable(format("Unsupported syntax : at {}", ss.str()));
}

shared_ptr<IfStmt> Parser::ParseIfStmt() {
  next();
  expect(TokenType::LeftParen, "Expect '(' after `if`.");
  shared_ptr<Expr> cond = ParseExpr();
  expect(TokenType::RightParen, "Expect ')' after condition.");

  shared_ptr<Stmt> IfBranch = ParseStmt();
  shared_ptr<Stmt> ElseBranch = nullptr;
  if (match(TokenType::KW_else)) {
    ElseBranch = ParseStmt();
  }
  return make_shared<IfStmt>(cond, IfBranch, ElseBranch);
}

InitVals ParseInit(Parser &p) {
  if (p.peek(0).isKind(TokenType::String)){
    vector<InitVals> ivs;
    Token t = p.next();
    for(auto c : get<string>(t.content)){
      ivs.push_back(InitVals(make_shared<IntegerExpr>(c)));
    }
    return ivs;
  }
  if (p.match(TokenType::LeftBrace)) {
    vector<InitVals> ivs;
    while (!p.peek(0).isKind(TokenType::RightBrace)) {
      ivs.push_back(ParseInit(p));
      if (!p.peek(0).isKind(TokenType::RightBrace)) {
        p.expect(TokenType::Comma, "Expect ',' between initvals.");
      }
    }
    p.expect(TokenType::RightBrace, "Expect '}'.");
    return InitVals(std::move(ivs));
  } else {
    return InitVals(p.ParseExpr());
  }
}

DeclStmt Parser::ParseDeclStmt() {
  bool isConst = false;
  if (match(TokenType::KW_const)) {
    isConst = true;
  }

  BaseType basetype;
  switch (next().type) {
  case TokenType::KW_int:
    basetype = BaseType::INT;
    break;
  case TokenType::KW_float:
    basetype = BaseType::FLOAT;
    break;
  default:
    throw string("Expect 'int' or 'float'.");
  }

  string varname = get<string>(
      expect(TokenType::Identifier, "Expect variable name.").content);

  vector<shared_ptr<Expr>> dims;
  while (match(TokenType::LeftBracket)) {
    dims.push_back(ParseExpr());
    expect(TokenType::RightBracket, "Expect ']'");
  }

  shared_ptr<InitVals> initval;
  if (match(TokenType::OpEqual)) {
    initval = make_shared<InitVals>(ParseInit(*this));
  }

  expect(TokenType::Semicolon, "Expect ';' after decl.");

  return DeclStmt(isConst, basetype, varname, dims, initval);
}

shared_ptr<ReturnStmt> Parser::ParseReturnStmt() {
  next();
  if (match(TokenType::Semicolon)) {
    return make_shared<ReturnStmt>(nullptr);
  }
  auto e = ParseExpr();
  expect(TokenType::Semicolon, "Expect ';' after return expr.");
  return make_shared<ReturnStmt>(e);
}

shared_ptr<BlockStmt> Parser::ParseBlockStmt() {
  vector<shared_ptr<Stmt>> Stmts;
  expect(TokenType::LeftBrace, "Expect '{'");
  while (!match(TokenType::RightBrace)) {
    shared_ptr<Stmt> R = ParseStmt();
    if (R != nullptr) {
      Stmts.push_back(R);
    }
  }
  return make_shared<BlockStmt>(Stmts);
}

shared_ptr<WhileStmt> Parser::ParseWhileStmt() {
  next();
  expect(TokenType::LeftParen, "Expect '(' after while.");
  auto cond = ParseExpr();
  expect(TokenType::RightParen, "Expect ')' after while condition.");
  auto body = ParseStmt();

  return make_shared<WhileStmt>(cond, body);
}

Func Parser::ParseFunction() {
  ReturnType ReturnType;
  switch (next().type) {
  case TokenType::KW_int:
    ReturnType = ReturnType::INT;
    break;
  case TokenType::KW_float:
    ReturnType = ReturnType::FLOAT;
    break;
  case TokenType::KW_void:
    ReturnType = ReturnType::VOID;
    break;
  default:
    unreachable("Unknown return type.");
  }

  string FunctionName = get<string>(
      expect(TokenType::Identifier, "Expect identifier in function definition.")
          .content);

  // Parse parameters.
  expect(TokenType::LeftParen, "Expect '(' after function name.");

  vector<Param> params_;
  while (!peek(0).isKind(TokenType::RightParen)) {
    BaseType bt;
    switch (next().type) {
    case TokenType::KW_int:
      bt = ast::BaseType::INT;
      break;
    case TokenType::KW_float:
      bt = ast::BaseType::FLOAT;
      break;
    default:
      throw string("Expect 'int' or 'float'.");
    }

    string paraname = get<string>(
        expect(TokenType::Identifier, "Expect identifier name.").content);

    vector<shared_ptr<Expr>> dims;

    while (match(TokenType::LeftBracket)) {
      if (match(TokenType::RightBracket)) {
        dims.push_back(nullptr);
      } else {
        dims.push_back(ParseExpr());
        expect(TokenType::RightBracket, "Expect ']'.");
      }
    }

    if (!peek(0).isKind(TokenType::RightParen)) {
      expect(TokenType::Comma, "Expect ',' between parameters.");
    }

    params_.emplace_back(bt, paraname, dims);
  }

  expect(TokenType::RightParen, "Expect ')'.");
  shared_ptr<BlockStmt> FunctionBody;
  if (match(TokenType::Semicolon)) {
    FunctionBody = nullptr;
  } else {
    FunctionBody = ParseBlockStmt();
  }

  return Func{ReturnType, FunctionName, std::move(params_), FunctionBody};
}

Program Parser::ParseProgram() {
  vector<variant<Func, DeclStmt>> instrs;
  while (!peek(0).isKind(TokenType::Eof)) {
    if (peek(0).isKind(TokenType::KW_const)) {
      instrs.push_back(ParseDeclStmt());
      continue;
    }
    if (peek(0).isKind(TokenType::KW_void)) {
      instrs.push_back(ParseFunction());
      continue;
    }
    if (peek(2).isKind(TokenType::LeftParen)) {
      instrs.push_back(ParseFunction());
      continue;
    }
    instrs.push_back(ParseDeclStmt());
  }
  return Program{instrs};
}

// Implement four auxiliary functions.
bool Parser::match(TokenType type) {
  if (src[Current].isKind(type)) {
    Current++;
    return true;
  }
  return false;
}

Token &Parser::peek(int offset) {
  if (Current + offset >= src.size()) {
    return src.back();
  }
  return src[Current + offset];
}

Token Parser::next() {
  if (Current >= src.size()) {
    return src.back();
  }
  return src[Current++];
}

Token Parser::expect(TokenType type, const char *ErrorMsg) {
  if (src[Current].isKind(type)) {
    return src[Current++];
  } else {
    std::cerr << ErrorMsg << "in the " << Current << "token.";
    for (int i = Current; i < src.size() && i < Current + 2; ++i) {
      std::cout << src[i] << '\n';
    }
    exit(1);
  }
}