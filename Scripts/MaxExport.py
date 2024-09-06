from pymxs import runtime as rt

sel = rt.getCurrentSelection()

for obj in sel:
    print(obj)