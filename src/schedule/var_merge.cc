#include <schedule.h>
#include <schedule/var_merge.h>

namespace freetensor {

Stmt VarMerge::visit(const VarDef &_op) {
    if (_op->id() == def_) {
        found_ = true;

        if (dim_ + 1 >= (int)_op->buffer_->tensor()->shape().size()) {
            throw InvalidSchedule(
                "There is no dimension " + std::to_string(dim_) + " ~ " +
                std::to_string(dim_ + 1) + " in variable " + _op->name_);
        }
        factor_ = _op->buffer_->tensor()->shape()[dim_ + 1];
        var_ = _op->name_;
        newVar_ =
            _op->buffer_->atype() == AccessType::Cache ? var_ : var_ + ".view";
        auto __op = Mutator::visit(_op);
        ASSERT(__op->nodeType() == ASTNodeType::VarDef);
        auto op = __op.as<VarDefNode>();
        var_.clear();
        newVar_.clear();

        auto &shape = op->buffer_->tensor()->shape();
        shape[dim_] = makeMul(shape[dim_], shape[dim_ + 1]);
        shape.erase(shape.begin() + dim_ + 1);

        if (op->buffer_->atype() != AccessType::Cache) {
            op->name_ += ".view";
            op->viewOf_ = _op->name_;
            op->buffer_->setAtype(AccessType::Cache);
            return makeVarDef(_op->name_, _op->buffer_, std::nullopt, op,
                              false);
        } else {
            return op;
        }
    } else {
        return Mutator::visit(_op);
    }
}

Stmt VarMerge::visit(const Store &_op) {
    auto __op = Mutator::visit(_op);
    ASSERT(__op->nodeType() == ASTNodeType::Store);
    auto op = __op.as<StoreNode>();
    return mergeMemAcc(op);
}

Stmt VarMerge::visit(const ReduceTo &_op) {
    auto __op = Mutator::visit(_op);
    ASSERT(__op->nodeType() == ASTNodeType::ReduceTo);
    auto op = __op.as<ReduceToNode>();
    return mergeMemAcc(op);
}

Expr VarMerge::visit(const Load &_op) {
    auto __op = Mutator::visit(_op);
    ASSERT(__op->nodeType() == ASTNodeType::Load);
    auto op = __op.as<LoadNode>();
    return mergeMemAcc(op);
}

Stmt varMerge(const Stmt &_ast, const ID &def, int dim) {
    VarMerge mutator(def, dim);
    auto ast = mutator(_ast);
    if (!mutator.found()) {
        throw InvalidSchedule(toString(def) + " not found");
    }
    return ast;
}

void Schedule::varMerge(const ID &def, int dim) {
    beginTransaction();
    auto log =
        appendLog(MAKE_SCHEDULE_LOG(VarMerge, freetensor::varMerge, def, dim));
    try {
        applyLog(log);
        commitTransaction();
    } catch (const InvalidSchedule &e) {
        abortTransaction();
        throw InvalidSchedule(log, ast(), e.what());
    }
}

} // namespace freetensor
