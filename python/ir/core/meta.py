import ffi
from ffi import up_cast, neutral_val
from .utils import *


def min_value(dtype):
    dtype = parseDType(dtype)
    if dtype == DataType.Float32 or dtype == DataType.Float64:
        return -float("inf")
    elif dtype == DataType.Int32:
        return 0x80000000
    else:
        assert False, "Unrecognized data type %s" % dtype


def max_value(dtype):
    dtype = parseDType(dtype)
    if dtype == DataType.Float32 or dtype == DataType.Float64:
        return float("inf")
    elif dtype == DataType.Int32:
        return 0x7fffffff
    else:
        assert False, "Unrecognized data type %s" % dtype


def same_mtype(lhs, rhs):
    lhs = parseMType(lhs)
    rhs = parseMType(rhs)
    assert lhs == rhs or lhs == ffi.MemType.ByValue or rhs == ffi.MemType.ByValue, "Variables must be on the same memory"
    return lhs
