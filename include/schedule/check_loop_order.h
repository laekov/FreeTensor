#ifndef FREE_TENSOR_CHECK_LOOP_ORDER_H
#define FREE_TENSOR_CHECK_LOOP_ORDER_H

#include <string>
#include <vector>

#include <visitor.h>

namespace freetensor {

/**
 * Return loops in nesting order
 */
class CheckLoopOrder : public Visitor {
    std::vector<ID> dstOrder_;
    std::vector<For> curOrder_, outerLoops_, outerLoopStack_;
    std::vector<StmtSeq> stmtSeqStack_, stmtSeqInBetween_;
    bool done_ = false;

  public:
    CheckLoopOrder(const std::vector<ID> &dstOrder) : dstOrder_(dstOrder) {}

    /**
     * All required loops, sorted from outer to inner
     */
    const std::vector<For> &order() const;

    /**
     * All StmtSeq nodes nested inside the outer-most required loop, and nesting
     * the inner-most required loop, sorted from outer to inner
     */
    const std::vector<StmtSeq> &stmtSeqInBetween() const {
        return stmtSeqInBetween_;
    }

    /**
     * Loops surrounding all loops in `dstOrder`
     */
    const std::vector<For> &outerLoops() const { return outerLoops_; }

  protected:
    void visitStmt(const Stmt &stmt) override;
    void visit(const For &op) override;
    void visit(const StmtSeq &op) override;
};

} // namespace freetensor

#endif // FREE_TENSOR_CHECK_LOOP_ORDER_H
