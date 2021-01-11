#include <ffi.h>

namespace ir {

PYBIND11_MODULE(ffi, m) {
    init_ffi_except(m);
    init_ffi_tensor(m);
    init_ffi_buffer(m);
    init_ffi_ast(m);
    init_ffi_schedule(m);
    init_ffi_pass(m);
    init_ffi_codegen(m);
    init_ffi_driver(m);
}

} // namespace ir

