#include "ast.hpp"
#include "common.hpp"
#include "token.hpp"

namespace ast {

void IntegerExpr::accept(TreeVisitor &tv) { tv.accept(*this); }
void FloatExpr::accept(TreeVisitor &tv) { tv.accept(*this); }
void NegateExpr::accept(TreeVisitor &tv) { tv.accept(*this); }
void BinopExpr::accept(TreeVisitor &tv) { tv.accept(*this); }
void CmpExpr::accept(TreeVisitor &tv) { tv.accept(*this); }
void NotExpr::accept(TreeVisitor &tv) { tv.accept(*this); }
void LogicalExpr::accept(TreeVisitor &tv) { tv.accept(*this); }
void VariableExpr::accept(TreeVisitor &tv) { tv.accept(*this); }
void IndexExpr::accept(TreeVisitor &tv) { tv.accept(*this); }
void AssignExpr::accept(TreeVisitor &tv) { tv.accept(*this); }
void FunctionCallExpr::accept(TreeVisitor &tv) { tv.accept(*this); }

void ExprStmt::accept(TreeVisitor &tv) { tv.accept(*this); }
void IfStmt::accept(TreeVisitor &tv) { tv.accept(*this); }
void DeclStmt::accept(TreeVisitor &tv) { tv.accept(*this); }
void ReturnStmt::accept(TreeVisitor &tv) { tv.accept(*this); }
void BlockStmt::accept(TreeVisitor &tv) { tv.accept(*this); }
void ContinueStmt::accept(TreeVisitor &tv) { tv.accept(*this); }
void BreakStmt::accept(TreeVisitor &tv) { tv.accept(*this); }
void WhileStmt::accept(TreeVisitor &tv) { tv.accept(*this); }
void Func::accept(TreeVisitor &tv) { tv.accept(*this); }
void Program::accept(TreeVisitor &tv) { tv.accept(*this); }

BinOpKind BinOpKindFromTokenType(TokenType type) {
  switch (type) {
  case TokenType::OpAdd:
    return BinOpKind::Add;
  case TokenType::OpSub:
    return BinOpKind::Sub;
  case TokenType::OpMul:
    return BinOpKind::Mul;
  case TokenType::OpDiv:
    return BinOpKind::Div;
  case TokenType::OpMod:
    return BinOpKind::Mod;
  default:
    unreachable("Should be unreachable.");
  }
}
CmpOpKind CmpKindFromTokenType(TokenType type) {
  switch (type) {
  case TokenType::OpGreater:
    return CmpOpKind::GT;
  case TokenType::OpGreaterEqual:
    return CmpOpKind::GE;
  case TokenType::OpLess:
    return CmpOpKind::LT;
  case TokenType::OpLessEqual:
    return CmpOpKind::LE;
  case TokenType::OpEqualEqual:
    return CmpOpKind::EE;
  case TokenType::OpNotEqual:
    return CmpOpKind::NE;
  default:
    unreachable("Shoule be unreachable.");
  }
}

LogicalOpKind LogicalOpKindFromTokenType(TokenType type) {
  switch (type) {
  case TokenType::OpAnd:
    return LogicalOpKind::And;
  case TokenType::OpOr:
    return LogicalOpKind::Or;
  default:
    unreachable("Shoule be unreachable.");
  }
}

} // namespace ast