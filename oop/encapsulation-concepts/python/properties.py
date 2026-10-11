"""properties.py - a property adds validation behind an attribute name,
so callers keep writing c.radius. The underscore is only a convention."""
import math


class Circle:
    def __init__(self, radius: float) -> None:
        self.radius = radius  # goes through the setter below

    @property
    def radius(self) -> float:
        return self._radius

    @radius.setter
    def radius(self, value: float) -> None:
        if value < 0:
            raise ValueError(f"radius must be non-negative: {value}")
        self._radius = value

    @property
    def area(self) -> float:
        return math.pi * self._radius ** 2


c = Circle(2.0)
print(f"radius {c.radius}, area {c.area:.2f}")

try:
    c.radius = -1
except ValueError as e:
    print(f"error: {e}")
print(f"radius still {c.radius}")

c._radius = -1  # nothing stops this: Python trusts the caller to respect the underscore
print(f"after c._radius = -1: radius {c.radius}")
