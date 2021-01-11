#include <codegen/code_gen_cpu.h>
#include <ffi.h>

namespace ir {

using namespace pybind11::literals;

void init_ffi_codegen(py::module_ &m) {
    m.def("code_gen_cpu", &codeGenCPU, "ast"_a);
}

} // namespace ir

