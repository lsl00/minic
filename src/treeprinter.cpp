#include "ast.hpp"
#include <iostream>
#include <variant>

namespace ast {

void TreePrinter::Indent() {
  for (int i = 0; i < IndentDepth; ++i) {
    std::cout << '\t';
  }
}

void TreePrinter::accept(Expr &e) { }
void TreePrinter::accept(Stmt &s) { }

void TreePrinter::accept(IntegerExpr &e) { std::cout << e.Value; }
void TreePrinter::accept(FloatExpr &e) { std::cout << e.Value; }
void TreePrinter::accept(NegateExpr &pe) {
  std::cout << '-';
  std::cout << '(';
  pe.operand->accept(*this);
  std::cout << ')';
}
void TreePrinter::accept(BinopExpr &be) {
  std::cout << '(';
  be.lhs->accept(*this);
  std::cout << ')';

  switch (be.op) {
  case BinOpKind::Add:
    std::cout << '+';
    break;
  case BinOpKind::Sub:
    std::cout << '-';
    break;
  case BinOpKind::Mul:
    std::cout << '*';
    break;
  case BinOpKind::Div:
    std::cout << '/';
    break;
  case BinOpKind::Mod:
    std::cout << "%";
    break;
  }

  std::cout << '(';
  be.rhs->accept(*this);
  std::cout << ')';
}

void TreePrinter::accept(CmpExpr &CE) {
  std::cout << '(';
  CE.lhs->accept(*this);
  std::cout << ')';

  switch (CE.op) {
  case CmpOpKind::GT:
    std::cout << '>';
    break;
  case CmpOpKind::GE:
    std::cout << ">=";
    break;
  case CmpOpKind::LT:
    std::cout << '<';
    break;
  case CmpOpKind::LE:
    std::cout << "<=";
    break;
  case CmpOpKind::EE:
    std::cout << "==";
    break;
  case CmpOpKind::NE:
    std::cout << "!=";
    break;
  }

  std::cout << '(';
  CE.rhs->accept(*this);
  std::cout << ')';
}

void TreePrinter::accept(NotExpr &NE) {
  std::cout << "!(";
  NE.operand->accept(*this);
  std::cout << ")";
}

void TreePrinter::accept(LogicalExpr &LE) {
  std::cout << '(';
  LE.lhs->accept(*this);
  std::cout << ')';

  switch (LE.op) {
  case LogicalOpKind::And:
    std::cout << "&&";
    break;
  case LogicalOpKind::Or:
    std::cout << "||";
    break;
  }

  std::cout << '(';
  LE.rhs->accept(*this);
  std::cout << ')';
}

void TreePrinter::accept(VariableExpr &ve) { std::cout << ve.VariName; }

void TreePrinter::accept(IndexExpr &IE) {
  if (IE.BaseArray)
    IE.BaseArray->accept(*this);
  if (IE.SubArray)
    IE.SubArray->accept(*this);
  std::cout << '[';
  IE.Index->accept(*this);
  std::cout << ']';
}

void TreePrinter::accept(AssignExpr &AE) {
  if (AE.Target)
    AE.Target->accept(*this);
  if (AE.ArrayTarget)
    AE.ArrayTarget->accept(*this);
  std::cout << " = ";
  AE.Assignment->accept(*this);
}

void TreePrinter::accept(FunctionCallExpr &FCE) {
  std::cout << FCE.FuncName << '(';
  if (FCE.RealParameters.size() > 0) {
    FCE.RealParameters[0]->accept(*this);
  }
  for (int i = 1; i < FCE.RealParameters.size(); ++i) {
    std::cout << ',';
    FCE.RealParameters[i]->accept(*this);
  }
  std::cout << ')';
}

void TreePrinter::accept(ExprStmt &es) {
  es.E->accept(*this);
  std::cout << ';';
}
void TreePrinter::accept(IfStmt &is) {
  std::cout << "if" << '(';
  is.cond->accept(*this);
  std::cout << ')';
  is.IfBranch->accept(*this);

  if (is.ElseBranch) {
    std::cout << "else ";
    is.ElseBranch->accept(*this);
  }
}

void accept_inits(TreePrinter &tp, InitVals &iv) {
  if (iv.val)
    iv.val->accept(tp);
  else {
    std::cout << '{';
    for (int i = 0; i < iv.vals.size(); ++i) {
      if (i > 0)
        std::cout << ',';
      accept_inits(tp, iv.vals[i]);
    }
    std::cout << '}';
  }
}

void TreePrinter::accept(DeclStmt &DS) {
  if (DS.isConst) {
    std::cout << "const ";
  }
  switch (DS.basetype_) {
  case BaseType::INT:
    std::cout << "int ";
    break;
  case BaseType::FLOAT:
    std::cout << "float ";
    break;
  }
  std::cout << DS.VarName;

  for (int i = 0; i < DS.Dims.size(); ++i) {
    std::cout << '[';
    DS.Dims[i]->accept(*this);
    std::cout << ']';
  }
  if (DS.initvals_) {
    std::cout << " = ";
    accept_inits(*this, *DS.initvals_);
  }
  std::cout << ';';
}

void TreePrinter::accept(ReturnStmt &rs) {
  std::cout << "return ";
  rs.ReturnExpr->accept(*this);
  std::cout << ";";
}
void TreePrinter::accept(BlockStmt &bs) {
  IndentDepth++;
  std::cout << "{\n";
  for (auto &stmt : bs.Stmts) {
    Indent();
    stmt->accept(*this);
    std::cout << "\n";
  }
  IndentDepth--;
  Indent();
  std::cout << "}";
}

void TreePrinter::accept(ContinueStmt &) { std::cout << "continue;"; }
void TreePrinter::accept(BreakStmt &) { std::cout << "break"; }

void TreePrinter::accept(WhileStmt &WS) {
  std::cout << "while(";
  WS.LoopCond->accept(*this);
  std::cout << ")";
  WS.LoopBody->accept(*this);
}

void accept_params(TreePrinter &tp, Param &p) {
  switch (p.basetype_) {
  case BaseType::INT:
    std::cout << "int ";
    break;
  case BaseType::FLOAT:
    std::cout << "float ";
    break;
  }
  std::cout << p.paraname_;
  for (int i = 0; i < p.dims_.size(); ++i) {
    std::cout << '[';
    if (p.dims_[i] != nullptr)
      p.dims_[i]->accept(tp);
    std::cout << ']';
  }
}

void TreePrinter::accept(Func &f) {
  switch (f.return_type_) {
  case ReturnType::INT:
    std::cout << "int";
    break;
  case ReturnType::FLOAT:
    std::cout << "float";
  case ReturnType::VOID:
    std::cout << "void";
  }
  std::cout << ' ' << f.function_name_ << '(';
  for (int i = 0; i < f.formal_paras_.size(); ++i) {
    if (i > 0)
      std::cout << ',';
    accept_params(*this, f.formal_paras_[i]);
  }
  std::cout << ')';
  if (f.body_)
    f.body_->accept(*this);
  else
    std::cout << ';';
}
void TreePrinter::accept(Program &p) {
  for (int i = 0; i < p.instrs.size(); ++i) {
    if (auto f = std::get_if<Func>(&p.instrs[i]); f != nullptr) {
      f->accept(*this);
    } else {
      std::get<DeclStmt>(p.instrs[i]).accept(*this);
    }
    std::cout << '\n';
  }
}
} // namespace ast