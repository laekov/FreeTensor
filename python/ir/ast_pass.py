from typing import Optional

import ffi

def lower(ast, target: Optional[ffi.Target]=None):
	ast = ffi.simplify_pass(ast)
	ast = ffi.sink_var(ast)
	ast = ffi.shrink_var(ast)
	ast = ffi.shrink_for(ast)
	ast = ffi.merge_if(ast)
	ast = ffi.seperate_tail(ast)

	if target is None:
		return ast

	if target.type() == ffi.TargetType.GPU:
		ast = ffi.gpu_make_sync(ast)

		# No more shrink_var after this pass
		ast = ffi.gpu_correct_shared(ast)

		# After gpu_make_sync and gpu_correct_shared. Otherwise, these 2 passes
		# cannot get the right thread info
		ast = ffi.gpu_normalize_threads(ast)

	return ast

