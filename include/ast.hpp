#pragma once

#include "token.hpp"
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

using namespace std;

namespace ast {
struct TreeVisitor;

struct Expr {
  virtual ~Expr() {};
  virtual void accept(TreeVisitor &) = 0;
};
struct IntegerExpr : public Expr {
  int Value;
  IntegerExpr(int v) : Value(v) {};

  void accept(TreeVisitor &);
};
struct FloatExpr : public Expr {
  float Value;
  FloatExpr(float v) : Value(v) {};

  void accept(TreeVisitor &);
};

struct NegateExpr : public Expr {
  shared_ptr<Expr> operand;

  NegateExpr(shared_ptr<Expr> operand) : operand(operand) {};
  void accept(TreeVisitor &);
};

enum class BinOpKind {
  Add,
  Sub,
  Mul,
  Div,
  Mod,
  // Maybe shift,xor... in the future
};
BinOpKind BinOpKindFromTokenType(TokenType);

struct BinopExpr : public Expr {
  BinOpKind op;
  shared_ptr<Expr> lhs, rhs;

  BinopExpr(BinOpKind op, shared_ptr<Expr> lhs, shared_ptr<Expr> rhs)
      : op(op), lhs(lhs), rhs(rhs) {};
  void accept(TreeVisitor &);
};

enum class CmpOpKind {
  GT,
  GE,
  LT,
  LE,
  EE,
  NE,
};

CmpOpKind CmpKindFromTokenType(TokenType);
struct CmpExpr : public Expr {
  CmpOpKind op;
  shared_ptr<Expr> lhs, rhs;
  CmpExpr(CmpOpKind op, shared_ptr<Expr> lhs, shared_ptr<Expr> rhs)
      : op(op), lhs(lhs), rhs(rhs) {};
  void accept(TreeVisitor &);
};

struct NotExpr : public Expr {
  shared_ptr<Expr> operand;
  NotExpr(shared_ptr<Expr> operand) : operand(operand) {};
  void accept(TreeVisitor &);
};

enum class LogicalOpKind {
  And,
  Or,
};
LogicalOpKind LogicalOpKindFromTokenType(TokenType);
struct LogicalExpr : public Expr {
  LogicalOpKind op;
  shared_ptr<Expr> lhs, rhs;
  LogicalExpr(LogicalOpKind op, shared_ptr<Expr> lhs, shared_ptr<Expr> rhs)
      : op(op), lhs(lhs), rhs(rhs) {}
  void accept(TreeVisitor &);
};

struct VariableExpr : public Expr {
  string VariName;

  VariableExpr(string &name) : VariName(name) {}
  void accept(TreeVisitor &);
};

struct IndexExpr : public Expr {
  string BaseArrayName;
  shared_ptr<VariableExpr> BaseArray;
  shared_ptr<IndexExpr> SubArray;
  shared_ptr<Expr> Index;

  IndexExpr(shared_ptr<VariableExpr> VE, shared_ptr<Expr> Index)
      : BaseArrayName(VE->VariName), BaseArray(VE), Index(Index) {};
  IndexExpr(shared_ptr<IndexExpr> SubArray, shared_ptr<Expr> Index)
      : BaseArrayName(SubArray->BaseArrayName), SubArray(SubArray),
        Index(Index) {};

  void accept(TreeVisitor &);
};

struct AssignExpr : public Expr {
  shared_ptr<VariableExpr> Target;
  shared_ptr<IndexExpr> ArrayTarget;
  shared_ptr<Expr> Assignment;

  AssignExpr(shared_ptr<VariableExpr> Target, shared_ptr<Expr> Assignment)
      : Target(Target), Assignment(Assignment) {};
  AssignExpr(shared_ptr<IndexExpr> ArrayTarget, shared_ptr<Expr> Assignment)
      : ArrayTarget(ArrayTarget), Assignment(Assignment) {};
  void accept(TreeVisitor &);
};

struct FunctionCallExpr : public Expr {
  string FuncName;
  std::vector<shared_ptr<Expr>> RealParameters;

  FunctionCallExpr(string FuncName, vector<shared_ptr<Expr>> RealParameters)
      : FuncName(FuncName), RealParameters(std::move(RealParameters)) {};
  void accept(TreeVisitor &);
};

struct Stmt {
  virtual ~Stmt() {};
  virtual void accept(TreeVisitor &) = 0;
};

struct ExprStmt : public Stmt {
  shared_ptr<Expr> E;
  ExprStmt(shared_ptr<Expr> E) : E(E) {};

  void accept(TreeVisitor &);
};

struct IfStmt : public Stmt {
  shared_ptr<Expr> cond;
  shared_ptr<Stmt> IfBranch;
  shared_ptr<Stmt> ElseBranch;

  IfStmt(shared_ptr<Expr> cond, shared_ptr<Stmt> IfBranch,
         shared_ptr<Stmt> ElseBranch = nullptr)
      : cond(cond), IfBranch(IfBranch), ElseBranch(ElseBranch) {
    assert(cond != nullptr);
    assert(IfBranch != nullptr);
  };

  void accept(TreeVisitor &);
};

enum class BaseType { INT, FLOAT };

struct InitVals {
  shared_ptr<Expr> val;
  vector<InitVals> vals;

  InitVals(InitVals &) = default;
  InitVals(InitVals &&) = default;
  InitVals(shared_ptr<Expr> val) : val(val) {};
  InitVals(vector<InitVals> &&vals) : vals(vals) {};
};

struct DeclStmt : public Stmt {
  bool isConst;
  BaseType basetype_;
  string VarName;
  std::vector<shared_ptr<Expr>> Dims;
  shared_ptr<InitVals> initvals_;

  DeclStmt(bool isConst, BaseType basetype_, string VarName,
           vector<shared_ptr<Expr>> Dims, shared_ptr<InitVals> initvals_)
      : isConst(isConst), basetype_(basetype_), VarName(VarName), Dims(Dims),
        initvals_(initvals_) {};

  void accept(TreeVisitor &);
};

struct ReturnStmt : public Stmt {
  shared_ptr<Expr> ReturnExpr;

  ReturnStmt(shared_ptr<Expr> e) : ReturnExpr(e) {};
  void accept(TreeVisitor &);
};
struct BlockStmt : public Stmt {
  vector<shared_ptr<Stmt>> Stmts;

  BlockStmt(vector<shared_ptr<Stmt>> Stmts) : Stmts(std::move(Stmts)) {};
  void accept(TreeVisitor &);
};
struct ContinueStmt : public Stmt {
  void accept(TreeVisitor &);
};
struct BreakStmt : public Stmt {
  void accept(TreeVisitor &);
};
struct WhileStmt : public Stmt {
  shared_ptr<Expr> LoopCond;
  shared_ptr<Stmt> LoopBody;

  WhileStmt(shared_ptr<Expr> LoopCond, shared_ptr<Stmt> LoopBody)
      : LoopCond(LoopCond), LoopBody(LoopBody) {};
  void accept(TreeVisitor &);
};

enum class ReturnType { INT, FLOAT, VOID };
struct Param {
  BaseType basetype_;
  string paraname_;
  vector<shared_ptr<Expr>> dims_; // only first element can be nullptr.
};

struct Func {
  ReturnType return_type_;
  string function_name_;
  vector<Param> formal_paras_;
  shared_ptr<BlockStmt> body_; // nullptr if it's a function declaration

  Func(ReturnType return_type_, string function_name_,
       vector<Param> formal_paras_, shared_ptr<BlockStmt> body_)
      : return_type_(return_type_), function_name_(function_name_),
        formal_paras_(formal_paras_), body_(body_) {};
  void accept(TreeVisitor &);
};

struct Program {
  vector<variant<Func, DeclStmt>> instrs;

  void accept(TreeVisitor &);
};

struct TreeVisitor {
  virtual void accept(Expr &) = 0;
  virtual void accept(IntegerExpr &) = 0;
  virtual void accept(FloatExpr &) = 0;
  virtual void accept(NegateExpr &) = 0;
  virtual void accept(BinopExpr &) = 0;
  virtual void accept(CmpExpr &) = 0;
  virtual void accept(NotExpr &) = 0;
  virtual void accept(LogicalExpr &) = 0;
  virtual void accept(VariableExpr &) = 0;
  virtual void accept(IndexExpr &) = 0;
  virtual void accept(AssignExpr &) = 0;
  virtual void accept(FunctionCallExpr &) = 0;

  virtual void accept(Stmt &) = 0;
  virtual void accept(ExprStmt &) = 0;
  virtual void accept(IfStmt &) = 0;
  virtual void accept(DeclStmt &) = 0;
  virtual void accept(ReturnStmt &) = 0;
  virtual void accept(BlockStmt &) = 0;
  virtual void accept(ContinueStmt &) = 0;
  virtual void accept(BreakStmt &) = 0;
  virtual void accept(WhileStmt &) = 0;
  virtual void accept(Func &) = 0;
  virtual void accept(Program &) = 0;
};

struct TreePrinter : public TreeVisitor {
  int IndentDepth = 0;

  void Indent();

  void accept(Expr &);
  void accept(IntegerExpr &);
  void accept(FloatExpr &);
  void accept(NegateExpr &);
  void accept(BinopExpr &);
  void accept(CmpExpr &);
  void accept(NotExpr &);
  void accept(LogicalExpr &);
  void accept(VariableExpr &);
  void accept(IndexExpr &);
  void accept(AssignExpr &);
  void accept(FunctionCallExpr &);

  void accept(Stmt &);
  void accept(ExprStmt &);
  void accept(IfStmt &);
  void accept(DeclStmt &);
  void accept(ReturnStmt &);
  void accept(BlockStmt &);
  void accept(ContinueStmt &);
  void accept(BreakStmt &);
  void accept(WhileStmt &);
  void accept(Func &);
  void accept(Program &);
};
} // namespace ast