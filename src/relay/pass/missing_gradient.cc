#include <tvm/relay/expr_functor.h>
#include <tvm/relay/analysis.h>
#include <tvm/relay/transform.h>

namespace tvm {
namespace relay {

using namespace tvm::runtime;


class MissingGradientChecker : private ExprVisitor {
  public:
  MissingGradientChecker() { }

  static void check(const Expr& expr) {
    MissingGradientChecker checker;
    checker(expr);
  }

  private:
    void VisitExpr_(const CallNode* op) final {
        if (const OpNode* op_node = op->op.as<OpNode>()) {
            Op op_ref = GetRef<Op>(op_node);
            if (!(rev_map.count(op_ref))) 
                LOG_INFO.stream() << "Check failed: " "rev_map.count(op_ref)" << ": "
                    << op_node->name << " does not have reverse mode defined";
        }
        ExprVisitor::VisitExpr_(op);
    }

    const OpMap<FPrimalGradient> rev_map = Op::GetAttr<FPrimalGradient>("FPrimalGradient");
};


void MissingGradientCheck(const Expr& re) {
    MissingGradientChecker::check(re);
}

TVM_REGISTER_API("relay._analysis.missing_gradient_check")
.set_body_typed(MissingGradientCheck);

}
}
