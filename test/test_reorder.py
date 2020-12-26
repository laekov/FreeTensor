import ir

def test_basic():
	with ir.VarDef("y", (4, 8), ir.DataType.Int32, ir.AccessType.Output) as y:
		with ir.For("i", 0, 4, nid="L1") as i:
			with ir.For("j", 0, 8, nid="L2") as j:
				y[i, j] = i + j
	ast = ir.pop_ast()
	print(ast)
	s = ir.Schedule(ast)
	s.reorder(["L2", "L1"])
	ast = s.ast()
	print(ast)
	ast = ir.lower(ast)
	print(ast)

	with ir.VarDef("y", (4, 8), ir.DataType.Int32, ir.AccessType.Output) as y:
		with ir.For("j", 0, 8) as j:
			with ir.For("i", 0, 4) as i:
				y[i, j] = i + j
	std = ir.pop_ast()

	assert std.match(ast)

def test_if_in_between():
	with ir.VarDef([
			("x", (4,), ir.DataType.Int32, ir.AccessType.Input),
			("y", (4, 8), ir.DataType.Int32, ir.AccessType.Output)]) as (x, y):
		with ir.For("i", 0, 4, nid="L1") as i:
			with ir.If(x[i] > 0):
				with ir.For("j", 0, 8, nid="L2") as j:
					y[i, j] = i + j
	ast = ir.pop_ast()
	print(ast)
	s = ir.Schedule(ast)
	s.reorder(["L2", "L1"])
	ast = s.ast()
	print(ast)
	ast = ir.lower(ast)
	print(ast)

	with ir.VarDef([
			("x", (4,), ir.DataType.Int32, ir.AccessType.Input),
			("y", (4, 8), ir.DataType.Int32, ir.AccessType.Output)]) as (x, y):
		with ir.For("j", 0, 8) as j:
			with ir.For("i", 0, 4) as i:
				with ir.If(x[i] > 0):
					y[i, j] = i + j
	std = ir.pop_ast()

	assert std.match(ast)

def test_stmt_in_between():
	with ir.VarDef([
			("y", (4, 8), ir.DataType.Int32, ir.AccessType.Output),
			("z", (4,), ir.DataType.Int32, ir.AccessType.Output)]) as (y, z):
		with ir.For("i", 0, 4, nid="L1") as i:
			z[i] = i
			with ir.For("j", 0, 8, nid="L2") as j:
				y[i, j] = i + j
	ast = ir.pop_ast()
	print(ast)
	s = ir.Schedule(ast)
	s.reorder(["L2", "L1"])
	ast = s.ast()
	print(ast)
	ast = ir.lower(ast)
	print(ast)

	with ir.VarDef([
			("y", (4, 8), ir.DataType.Int32, ir.AccessType.Output),
			("z", (4,), ir.DataType.Int32, ir.AccessType.Output)]) as (y, z):
		with ir.For("j", 0, 8) as j:
			with ir.For("i", 0, 4) as i:
				with ir.If(j == 0):
					z[i] = i
				y[i, j] = i + j
	std = ir.pop_ast()

	assert std.match(ast)

