#include "ir.h"

TLANG_NAMESPACE_BEGIN

// "Type" here does not include vector width
// Variable lookup and Type inference
class TypeCheck : public IRVisitor {
 public:
  TypeCheck() {
    allow_undefined_visitor = true;
  }

  void visit(AllocaStmt *stmt) {
    auto block = stmt->parent;
    auto ident = stmt->ident;
    TC_ASSERT(block->local_variables.find(ident) ==
              block->local_variables.end());
    block->local_variables.insert(std::make_pair(ident, stmt->ret_type));
  }

  void visit(IfStmt *if_stmt) {
    if (if_stmt->true_statements)
      if_stmt->true_statements->accept(this);
    if (if_stmt->false_statements) {
      if_stmt->false_statements->accept(this);
    }
  }

  void visit(Block *stmt_list) {
    for (auto &stmt : stmt_list->statements) {
      stmt->accept(this);
    }
  }

  void visit(LocalLoadStmt *stmt) {
    auto block = stmt->parent;
    auto lookup = block->lookup_var(stmt->ident);
    stmt->ret_type = lookup;
  }

  void visit(FrontendForStmt *stmt) {
    auto block = stmt->parent;
    auto lookup = block->lookup_var(stmt->loop_var_id);
    TC_ASSERT(block->local_variables.find(stmt->loop_var_id) ==
              block->local_variables.end());
    block->local_variables.insert(
        std::make_pair(stmt->loop_var_id, VectorType(1, DataType::i32)));
    stmt->body->accept(this);
  }

  void visit(BinaryOpStmt *stmt) {
    TC_ASSERT(stmt->lhs->ret_type.data_type != DataType::unknown ||
              stmt->rhs->ret_type.data_type != DataType::unknown);
    if (stmt->lhs->ret_type.data_type == DataType::unknown &&
        stmt->lhs->is<ConstStmt>()) {
      stmt->lhs->ret_type = stmt->rhs->ret_type;
    }
    if (stmt->rhs->ret_type.data_type == DataType::unknown &&
        stmt->rhs->is<ConstStmt>()) {
      stmt->rhs->ret_type = stmt->lhs->ret_type;
    }
    TC_ASSERT(stmt->lhs->ret_type.data_type != DataType::unknown);
    TC_ASSERT(stmt->rhs->ret_type.data_type != DataType::unknown);
    TC_ASSERT(stmt->lhs->ret_type == stmt->rhs->ret_type);
    if (is_comparison(stmt->op_type)) {
      stmt->ret_type = VectorType(1, DataType::i32);
    } else {
      stmt->ret_type = stmt->lhs->ret_type;
    }
  }

  static void run(IRNode *node) {
    TypeCheck inst;
    node->accept(&inst);
  }
};

namespace irpass {

void typecheck(IRNode *root) {
  return TypeCheck::run(root);
}

}  // namespace irpass

TLANG_NAMESPACE_END
