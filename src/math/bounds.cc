#include <algorithm>
#include <climits>
#include <functional>
#include <type_traits>

#include <math/bounds.h>
#include <math/utils.h>

namespace ir {

static LinearExpr<Rational<int>>
commonDenominator(const LinearExpr<Rational<int>> &_lin) {
    auto lin = _lin;
    auto common = lin.bias_.q_;
    for (auto &&item : lin.coeff_) {
        common = lcm(common, item.second.k_.q_);
    }
    lin.bias_.p_ *= common / lin.bias_.q_;
    lin.bias_.q_ = common;
    for (auto &item : lin.coeff_) {
        item.second.k_.p_ *= common / item.second.k_.q_;
        item.second.k_.q_ = common;
    }
    return lin;
}

static Expr linToExprNumerator(const LinearExpr<Rational<int>> &lin) {
    Expr b = makeIntConst(lin.bias_.p_);

    for (auto &&item : lin.coeff_) {
        auto k = item.second.k_;
        auto a = deepCopy(item.second.a_);

        if (k == 0) {
            continue;
        }
        Expr x;
        if (a->nodeType() == ASTNodeType::IntConst) {
            x = makeIntConst(k.p_ * a.as<IntConstNode>()->val_);
        } else if (k.p_ == 1) {
            x = a;
        } else {
            x = makeMul(makeIntConst(k.p_), a);
        }

        if (x->nodeType() == ASTNodeType::IntConst &&
            b->nodeType() == ASTNodeType::IntConst) {
            x = makeIntConst(x.as<IntConstNode>()->val_ +
                             b.as<IntConstNode>()->val_);
        } else if (b->nodeType() == ASTNodeType::IntConst &&
                   b.as<IntConstNode>()->val_ == 0) {
            // do nothing
        } else {
            x = makeAdd(x, b);
        }

        b = std::move(x);
    }

    return b;
}

template <class T> static T mulImpl(const T &b, int k) {
    auto ret = b.lin();
    if (k == 0) {
        ret.coeff_.clear();
        ret.bias_ = 0;
        return ret;
    }
    for (auto &&item : ret.coeff_) {
        item.second.k_ *= k;
    }
    ret.bias_ *= k;
    return T(std::move(ret));
}

const Expr &UpperBound::expr() {
    if (expr_.isValid()) {
        return expr_;
    }
    auto cdLin = commonDenominator(lin_);
    expr_ = linToExprNumerator(cdLin);
    if (cdLin.bias_.q_ != 1) {
        if (expr_->nodeType() == ASTNodeType::IntConst) {
            expr_ = makeIntConst(
                floorDiv(expr_.as<IntConstNode>()->val_, cdLin.bias_.q_));
        } else {
            expr_ = makeFloorDiv(expr_, makeIntConst(cdLin.bias_.q_));
        }
    }
    return expr_;
}

const Expr &LowerBound::expr() {
    if (expr_.isValid()) {
        return expr_;
    }
    auto cdLin = commonDenominator(lin_);
    expr_ = linToExprNumerator(cdLin);
    if (cdLin.bias_.q_ != 1) {
        if (expr_->nodeType() == ASTNodeType::IntConst) {
            expr_ = makeIntConst(
                ceilDiv(expr_.as<IntConstNode>()->val_, cdLin.bias_.q_));
        } else {
            expr_ = makeCeilDiv(expr_, makeIntConst(cdLin.bias_.q_));
        }
    }
    return expr_;
}

UpperBound add(const UpperBound &b1, const UpperBound &b2) {
    return add(b1.lin(), b2.lin());
}
LowerBound add(const LowerBound &b1, const LowerBound &b2) {
    return add(b1.lin(), b2.lin());
}

UpperBound sub(const UpperBound &b1, const LowerBound &b2) {
    return sub(b1.lin(), b2.lin());
}
LowerBound sub(const LowerBound &b1, const UpperBound &b2) {
    return sub(b1.lin(), b2.lin());
}

UpperBound mul(const UpperBound &b, int k) { return mulImpl(b, k); }
LowerBound mul(const LowerBound &b, int k) { return mulImpl(b, k); }

UpperBound floorDiv(const UpperBound &b, int k) {
    auto ret = b.lin();
    for (auto &&item : ret.coeff_) {
        item.second.k_ /= k;
    }
    ret.bias_ /= k;
    return UpperBound(std::move(ret));
}
LowerBound floorDiv(const LowerBound &b, int k) {
    auto ret = b.lin();
    for (auto &&item : ret.coeff_) {
        item.second.k_ /= k;
    }
    ret.bias_ -= k - 1;
    ret.bias_ /= k;
    return LowerBound(std::move(ret));
}

UpperBound ceilDiv(const UpperBound &b, int k) {
    auto ret = b.lin();
    for (auto &&item : ret.coeff_) {
        item.second.k_ /= k;
    }
    ret.bias_ += k - 1;
    ret.bias_ /= k;
    return UpperBound(std::move(ret));
}
LowerBound ceilDiv(const LowerBound &b, int k) {
    auto ret = b.lin();
    for (auto &&item : ret.coeff_) {
        item.second.k_ /= k;
    }
    ret.bias_ /= k;
    return LowerBound(std::move(ret));
}

} // namespace ir

