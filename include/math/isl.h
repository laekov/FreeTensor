#ifndef ISL_H
#define ISL_H

#include <iostream>
#include <string>

#include <isl/ctx.h>
#include <isl/ilp.h>
#include <isl/map.h>
#include <isl/options.h>
#include <isl/set.h>
#include <isl/space.h>

#include <debug/profile.h>
#include <except.h>

namespace ir {

// A C++ style wrapper over ISL

template <class T> T *GET_ISL_PTR(T *ptr) {
    ASSERT(ptr != nullptr);
    return ptr;
}

#define COPY_ISL_PTR(ptr, type) _COPY_ISL_PTR(ptr, isl_##type##_copy)
template <class T> T *_COPY_ISL_PTR(const T *ptr, T *(copy)(T *)) {
    ASSERT(ptr != nullptr);
    return copy(const_cast<T *>(ptr));
}

template <class T> T *MOVE_ISL_PTR(T *&ptr) {
    ASSERT(ptr != nullptr);
    auto ret = ptr;
    ptr = nullptr;
    return ret;
}

class ISLCtx {
    isl_ctx *ctx_ = nullptr;

  public:
    ISLCtx() : ctx_(isl_ctx_alloc()) {
        isl_options_set_on_error(ctx_, ISL_ON_ERROR_ABORT);
    }
    ~ISLCtx() { isl_ctx_free(ctx_); }

    ISLCtx(const ISLCtx &other) = delete;
    ISLCtx &operator=(const ISLCtx &other) = delete;

    isl_ctx *get() const { return GET_ISL_PTR(ctx_); }
};

class ISLMap {
    isl_map *map_ = nullptr;

  public:
    ISLMap() {}
    ISLMap(isl_map *map) : map_(map) {}
    ISLMap(const ISLCtx &ctx, const std::string &str)
        : map_(isl_map_read_from_str(ctx.get(), str.c_str())) {
        if (map_ == nullptr) {
            ERROR("Unable to construct an ISLMap from " + str);
        }
    }
    ~ISLMap() {
        if (map_ != nullptr) {
            isl_map_free(map_);
        }
    }

    ISLMap(const ISLMap &other) : map_(other.copy()) {}
    ISLMap &operator=(const ISLMap &other) {
        if (map_ != nullptr) {
            isl_map_free(map_);
        }
        map_ = other.copy();
        return *this;
    }

    ISLMap(ISLMap &&other) : map_(other.move()) {}
    ISLMap &operator=(ISLMap &&other) {
        if (map_ != nullptr) {
            isl_map_free(map_);
        }
        map_ = other.move();
        return *this;
    }

    bool isValid() const { return map_ != nullptr; }

    isl_map *get() const { return GET_ISL_PTR(map_); }
    isl_map *copy() const { return COPY_ISL_PTR(map_, map); }
    isl_map *move() { return MOVE_ISL_PTR(map_); }

    bool empty() const {
        DEBUG_PROFILE("empty");
        return isl_map_is_empty(get());
    }

    isl_size nBasic() const { return isl_map_n_basic_map(map_); }

    friend std::string toString(const ISLMap &map) {
        return isl_map_to_str(map.map_);
    }
};

inline std::ostream &operator<<(std::ostream &os, const ISLMap &map) {
    return os << toString(map);
}

class ISLVal {
    isl_val *val_ = nullptr;

  public:
    ISLVal() {}
    ISLVal(isl_val *val) : val_(val) {}
    ~ISLVal() {
        if (val_ != nullptr) {
            isl_val_free(val_);
        }
    }

    ISLVal(const ISLVal &other) : val_(other.copy()) {}
    ISLVal &operator=(const ISLVal &other) {
        if (val_ != nullptr) {
            isl_val_free(val_);
        }
        val_ = other.copy();
        return *this;
    }

    ISLVal(ISLVal &&other) : val_(other.move()) {}
    ISLVal &operator=(ISLVal &&other) {
        if (val_ != nullptr) {
            isl_val_free(val_);
        }
        val_ = other.move();
        return *this;
    }

    bool isValid() const { return val_ != nullptr; }

    isl_val *get() const { return GET_ISL_PTR(val_); }
    isl_val *copy() const { return COPY_ISL_PTR(val_, val); }
    isl_val *move() { return MOVE_ISL_PTR(val_); }

    bool isRat() const { return isl_val_is_rat(get()); }
    int numSi() const { return isl_val_get_num_si(get()); }
    int denSi() const { return isl_val_get_den_si(get()); }

    friend std::string toString(const ISLVal &val) {
        return isl_val_to_str(val.val_);
    }
};

inline std::ostream &operator<<(std::ostream &os, const ISLVal &val) {
    return os << toString(val);
}

class ISLSet {
    isl_set *set_ = nullptr;

  public:
    ISLSet() {}
    ISLSet(isl_set *set) : set_(set) {}
    ISLSet(const ISLCtx &ctx, const std::string &str)
        : set_(isl_set_read_from_str(ctx.get(), str.c_str())) {
        if (set_ == nullptr) {
            ERROR("Unable to construct an ISLSet from " + str);
        }
    }
    ~ISLSet() {
        if (set_ != nullptr) {
            isl_set_free(set_);
        }
    }

    ISLSet(const ISLSet &other) : set_(other.copy()) {}
    ISLSet &operator=(const ISLSet &other) {
        if (set_ != nullptr) {
            isl_set_free(set_);
        }
        set_ = other.copy();
        return *this;
    }

    ISLSet(ISLSet &&other) : set_(other.move()) {}
    ISLSet &operator=(ISLSet &&other) {
        if (set_ != nullptr) {
            isl_set_free(set_);
        }
        set_ = other.move();
        return *this;
    }

    bool isValid() const { return set_ != nullptr; }

    isl_set *get() const { return GET_ISL_PTR(set_); }
    isl_set *copy() const { return COPY_ISL_PTR(set_, set); }
    isl_set *move() { return MOVE_ISL_PTR(set_); }

    isl_size nBasic() const { return isl_set_n_basic_set(set_); }

    friend std::string toString(const ISLSet &set) {
        return isl_set_to_str(set.set_);
    }
};

inline std::ostream &operator<<(std::ostream &os, const ISLSet &set) {
    return os << toString(set);
}

class ISLSpace {
    isl_space *space_ = nullptr;

  public:
    ISLSpace() {}
    ISLSpace(isl_space *space) : space_(space) {}
    ISLSpace(const ISLSet &set) : space_(isl_set_get_space(set.get())) {}
    ~ISLSpace() {
        if (space_ != nullptr) {
            isl_space_free(space_);
        }
    }

    ISLSpace(const ISLSpace &other) : space_(other.copy()) {}
    ISLSpace &operator=(const ISLSpace &other) {
        if (space_ != nullptr) {
            isl_space_free(space_);
        }
        space_ = other.copy();
        return *this;
    }

    ISLSpace(ISLSpace &&other) : space_(other.move()) {}
    ISLSpace &operator=(ISLSpace &&other) {
        if (space_ != nullptr) {
            isl_space_free(space_);
        }
        space_ = other.move();
        return *this;
    }

    bool isValid() const { return space_ != nullptr; }

    isl_space *get() const { return GET_ISL_PTR(space_); }
    isl_space *copy() const { return COPY_ISL_PTR(space_, space); }
    isl_space *move() { return MOVE_ISL_PTR(space_); }

    friend std::string toString(const ISLSpace &space) {
        return isl_space_to_str(space.space_);
    }
};

inline std::ostream &operator<<(std::ostream &os, const ISLSpace &space) {
    return os << toString(space);
}

inline ISLSet complement(ISLSet &&set) {
    DEBUG_PROFILE("complement");
    return isl_set_complement(set.move());
}
inline ISLSet complement(const ISLSet &set) {
    DEBUG_PROFILE("complement");
    return isl_set_complement(set.copy());
}
inline ISLMap complement(ISLMap &&map) {
    DEBUG_PROFILE("complement");
    return isl_map_complement(map.move());
}
inline ISLMap complement(const ISLMap &map) {
    DEBUG_PROFILE("complement");
    return isl_map_complement(map.copy());
}

inline ISLMap reverse(ISLMap &&map) {
    DEBUG_PROFILE("reverse");
    return isl_map_reverse(map.move());
}
inline ISLMap reverse(const ISLMap &map) {
    DEBUG_PROFILE("reverse");
    return isl_map_reverse(map.copy());
}

inline ISLMap subtract(ISLMap &&lhs, ISLMap &&rhs) {
    DEBUG_PROFILE_VERBOSE("subtract", "nBasic=" + std::to_string(lhs.nBasic()) +
                                          "," + std::to_string(rhs.nBasic()));
    return isl_map_subtract(lhs.move(), rhs.move());
}
inline ISLMap subtract(const ISLMap &lhs, ISLMap &&rhs) {
    DEBUG_PROFILE_VERBOSE("subtract", "nBasic=" + std::to_string(lhs.nBasic()) +
                                          "," + std::to_string(rhs.nBasic()));
    return isl_map_subtract(lhs.copy(), rhs.move());
}
inline ISLMap subtract(ISLMap &&lhs, const ISLMap &rhs) {
    DEBUG_PROFILE_VERBOSE("subtract", "nBasic=" + std::to_string(lhs.nBasic()) +
                                          "," + std::to_string(rhs.nBasic()));
    return isl_map_subtract(lhs.move(), rhs.copy());
}
inline ISLMap subtract(const ISLMap &lhs, const ISLMap &rhs) {
    DEBUG_PROFILE_VERBOSE("subtract", "nBasic=" + std::to_string(lhs.nBasic()) +
                                          "," + std::to_string(rhs.nBasic()));
    return isl_map_subtract(lhs.copy(), rhs.copy());
}

inline ISLMap intersect(ISLMap &&lhs, ISLMap &&rhs) {
    DEBUG_PROFILE_VERBOSE("intersect",
                          "nBasic=" + std::to_string(lhs.nBasic()) + "," +
                              std::to_string(rhs.nBasic()));
    return isl_map_intersect(lhs.move(), rhs.move());
}
inline ISLMap intersect(const ISLMap &lhs, ISLMap &&rhs) {
    DEBUG_PROFILE_VERBOSE("intersect",
                          "nBasic=" + std::to_string(lhs.nBasic()) + "," +
                              std::to_string(rhs.nBasic()));
    return isl_map_intersect(lhs.copy(), rhs.move());
}
inline ISLMap intersect(ISLMap &&lhs, const ISLMap &rhs) {
    DEBUG_PROFILE_VERBOSE("intersect",
                          "nBasic=" + std::to_string(lhs.nBasic()) + "," +
                              std::to_string(rhs.nBasic()));
    return isl_map_intersect(lhs.move(), rhs.copy());
}
inline ISLMap intersect(const ISLMap &lhs, const ISLMap &rhs) {
    DEBUG_PROFILE_VERBOSE("intersect",
                          "nBasic=" + std::to_string(lhs.nBasic()) + "," +
                              std::to_string(rhs.nBasic()));
    return isl_map_intersect(lhs.copy(), rhs.copy());
}

inline ISLMap uni(ISLMap &&lhs, ISLMap &&rhs) {
    DEBUG_PROFILE_VERBOSE("uni", "nBasic=" + std::to_string(lhs.nBasic()) +
                                     "," + std::to_string(rhs.nBasic()));
    return isl_map_union(lhs.move(), rhs.move());
}
inline ISLMap uni(const ISLMap &lhs, ISLMap &&rhs) {
    DEBUG_PROFILE_VERBOSE("uni", "nBasic=" + std::to_string(lhs.nBasic()) +
                                     "," + std::to_string(rhs.nBasic()));
    return isl_map_union(lhs.copy(), rhs.move());
}
inline ISLMap uni(ISLMap &&lhs, const ISLMap &rhs) {
    DEBUG_PROFILE_VERBOSE("uni", "nBasic=" + std::to_string(lhs.nBasic()) +
                                     "," + std::to_string(rhs.nBasic()));
    return isl_map_union(lhs.move(), rhs.copy());
}
inline ISLMap uni(const ISLMap &lhs, const ISLMap &rhs) {
    DEBUG_PROFILE_VERBOSE("uni", "nBasic=" + std::to_string(lhs.nBasic()) +
                                     "," + std::to_string(rhs.nBasic()));
    return isl_map_union(lhs.copy(), rhs.copy());
}

inline ISLMap applyDomain(ISLMap &&lhs, ISLMap &&rhs) {
    DEBUG_PROFILE("applyDomain");
    return isl_map_apply_domain(lhs.move(), rhs.move());
}
inline ISLMap applyDomain(const ISLMap &lhs, ISLMap &&rhs) {
    DEBUG_PROFILE("applyDomain");
    return isl_map_apply_domain(lhs.copy(), rhs.move());
}
inline ISLMap applyDomain(ISLMap &&lhs, const ISLMap &rhs) {
    DEBUG_PROFILE("applyDomain");
    return isl_map_apply_domain(lhs.move(), rhs.copy());
}
inline ISLMap applyDomain(const ISLMap &lhs, const ISLMap &rhs) {
    DEBUG_PROFILE("applyDomain");
    return isl_map_apply_domain(lhs.copy(), rhs.copy());
}

inline ISLMap applyRange(ISLMap &&lhs, ISLMap &&rhs) {
    DEBUG_PROFILE("applyRange");
    return isl_map_apply_range(lhs.move(), rhs.move());
}
inline ISLMap applyRange(const ISLMap &lhs, ISLMap &&rhs) {
    DEBUG_PROFILE("applyRange");
    return isl_map_apply_range(lhs.copy(), rhs.move());
}
inline ISLMap applyRange(ISLMap &&lhs, const ISLMap &rhs) {
    DEBUG_PROFILE("applyRange");
    return isl_map_apply_range(lhs.move(), rhs.copy());
}
inline ISLMap applyRange(const ISLMap &lhs, const ISLMap &rhs) {
    DEBUG_PROFILE("applyRange");
    return isl_map_apply_range(lhs.copy(), rhs.copy());
}

inline ISLMap lexmax(ISLMap &&map) {
    DEBUG_PROFILE_VERBOSE("lexmax", "nBasic=" + std::to_string(map.nBasic()));
    return isl_map_lexmax(map.move());
}
inline ISLMap lexmax(const ISLMap &map) {
    DEBUG_PROFILE_VERBOSE("lexmax", "nBasic=" + std::to_string(map.nBasic()));
    return isl_map_lexmax(map.copy());
}

inline ISLMap lexmin(ISLMap &&map) {
    DEBUG_PROFILE_VERBOSE("lexmin", "nBasic=" + std::to_string(map.nBasic()));
    return isl_map_lexmin(map.move());
}
inline ISLMap lexmin(const ISLMap &map) {
    DEBUG_PROFILE_VERBOSE("lexmin", "nBasic=" + std::to_string(map.nBasic()));
    return isl_map_lexmin(map.copy());
}

inline ISLMap identity(ISLSpace &&space) {
    DEBUG_PROFILE("identity");
    return isl_map_identity(space.move());
}
inline ISLMap identity(const ISLSpace &space) {
    DEBUG_PROFILE("identity");
    return isl_map_identity(space.copy());
}

inline ISLMap lexGE(ISLSpace &&space) {
    DEBUG_PROFILE("lexGE");
    return isl_map_lex_ge(space.move());
}
inline ISLMap lexGE(const ISLSpace &space) {
    DEBUG_PROFILE("lexGE");
    return isl_map_lex_ge(space.copy());
}

inline ISLMap lexGT(ISLSpace &&space) {
    DEBUG_PROFILE("lexGT");
    return isl_map_lex_gt(space.move());
}
inline ISLMap lexGT(const ISLSpace &space) {
    DEBUG_PROFILE("lexGT");
    return isl_map_lex_gt(space.copy());
}

inline ISLSpace spaceAlloc(const ISLCtx &ctx, unsigned nparam, unsigned nIn,
                           unsigned nOut) {
    return isl_space_alloc(ctx.get(), nparam, nIn, nOut);
}

inline ISLSpace spaceSetAlloc(const ISLCtx &ctx, unsigned nparam,
                              unsigned dim) {
    return isl_space_set_alloc(ctx.get(), nparam, dim);
}

inline ISLSet emptySet(ISLSpace &&space) { return isl_set_empty(space.move()); }
inline ISLSet emptySet(const ISLSpace &space) {
    return isl_set_empty(space.copy());
}

inline ISLMap emptyMap(ISLSpace &&space) { return isl_map_empty(space.move()); }
inline ISLMap emptyMap(const ISLSpace &space) {
    return isl_map_empty(space.copy());
}

inline ISLSet universeSet(ISLSpace &&space) {
    return isl_set_universe(space.move());
}
inline ISLSet universeSet(const ISLSpace &space) {
    return isl_set_universe(space.copy());
}

inline ISLMap universeMap(ISLSpace &&space) {
    return isl_map_universe(space.move());
}
inline ISLMap universeMap(const ISLSpace &space) {
    return isl_map_universe(space.copy());
}

inline ISLSet domain(ISLMap &&map) { return isl_map_domain(map.move()); }
inline ISLSet domain(const ISLMap &map) { return isl_map_domain(map.copy()); }

inline ISLSet range(ISLMap &&map) { return isl_map_range(map.move()); }
inline ISLSet range(const ISLMap &map) { return isl_map_range(map.copy()); }

inline ISLVal dimMaxVal(ISLSet &&set, int pos) {
    return isl_set_dim_max_val(set.move(), pos);
}
inline ISLVal dimMaxVal(const ISLSet &set, int pos) {
    return isl_set_dim_max_val(set.copy(), pos);
}

inline ISLVal dimMinVal(ISLSet &&set, int pos) {
    return isl_set_dim_min_val(set.move(), pos);
}
inline ISLVal dimMinVal(const ISLSet &set, int pos) {
    return isl_set_dim_min_val(set.copy(), pos);
}

inline ISLSpace spaceMapFromSet(ISLSpace &&space) {
    return isl_space_map_from_set(space.move());
}
inline ISLSpace spaceMapFromSet(const ISLSpace &space) {
    return isl_space_map_from_set(space.copy());
}

inline bool operator==(const ISLSet &lhs, const ISLSet &rhs) {
    DEBUG_PROFILE_VERBOSE("equal", "nBasic=" + std::to_string(lhs.nBasic()) +
                                       "," + std::to_string(rhs.nBasic()));
    return isl_set_is_equal(lhs.get(), rhs.get());
}

inline bool operator!=(const ISLSet &lhs, const ISLSet &rhs) {
    DEBUG_PROFILE_VERBOSE("inequal", "nBasic=" + std::to_string(lhs.nBasic()) +
                                         "," + std::to_string(rhs.nBasic()));
    return !isl_set_is_equal(lhs.get(), rhs.get());
}

} // namespace ir

#endif // ISL_H
