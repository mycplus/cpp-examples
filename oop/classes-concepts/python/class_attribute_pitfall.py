"""class_attribute_pitfall.py - a mutable class attribute is shared by every
object; the fix is to create the list in __init__."""


class SharedCart:
    items = []                      # one list, stored on the class

    def add(self, item):
        self.items.append(item)     # finds the class's list and changes it


class Cart:
    def __init__(self):
        self.items = []             # a new list for each object

    def add(self, item):
        self.items.append(item)


a, b = SharedCart(), SharedCart()
a.add("book")
b.add("lamp")
print("SharedCart:", a.items, b.items, "same list:", a.items is b.items)

c, d = Cart(), Cart()
c.add("book")
d.add("lamp")
print("Cart:      ", c.items, d.items, "same list:", c.items is d.items)
