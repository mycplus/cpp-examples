"""types_of_inheritance.py - single, multilevel, hierarchical and multiple
inheritance, and the method resolution order Python uses for each."""


class Device: pass                      # base
class Scanner(Device): pass             # single
class Printer(Device): pass             # hierarchical: two classes from one base
class Copier(Scanner, Printer): pass    # multiple (and a diamond through Device)
class ColorCopier(Copier): pass         # multilevel: Device -> Scanner -> Copier -> ColorCopier

for cls in (Scanner, Copier, ColorCopier):
    print(f"{cls.__name__:12} MRO: " + " -> ".join(c.__name__ for c in cls.__mro__))
print("Copier is a Device:", issubclass(Copier, Device))
