"""identity.py - equality, identity and what assignment does in Python.
A name refers to an object, so assignment shares it."""
import copy
from dataclasses import dataclass


@dataclass
class Point:          # @dataclass generates __init__ and a value-based __eq__
    x: int
    y: int


p1 = Point(1, 2)
p2 = Point(1, 2)
print("p1 == p2 (equal values):", p1 == p2)
print("p1 is p2 (same object): ", p1 is p2)

q = p1                 # binds a second name to the same object
q.x = 99
print(f"after q = p1; q.x = 99:   p1.x = {p1.x}, q.x = {q.x}")

p1.x = 1
c = copy.copy(p1)      # an explicit copy
c.x = 99
print(f"after c = copy of p1; c.x = 99:   p1.x = {p1.x}")
