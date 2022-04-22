#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <string>

// Include minimal headers

#include <ast.h>
#include <debug/logger.h>

namespace ir {

// Explicitly declare some functions, but not include their headers

std::string toString(const AST &op);
std::string toString(const AST &op, bool pretty);
std::string toString(const AST &op, bool pretty, bool printAllId);

bool match(const Stmt &pattern, const Stmt &instance);

inline std::ostream &operator<<(std::ostream &os, const AST &op) {
    os << toString(op, false);
    return os;
}

} // namespace ir

#endif // DEBUG_H
