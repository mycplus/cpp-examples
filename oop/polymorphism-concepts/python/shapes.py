"""shapes.py - runtime polymorphism: the loop calls area() and name()
on each shape, and each object runs its own version."""
from abc import ABC, abstractmethod
import math


class Shape(ABC):
    @abstractmethod
    def area(self) -> float: ...

    @abstractmethod
    def name(self) -> str: ...


class Circle(Shape):
    def __init__(self, radius: float):
        self.radius = radius

    def area(self) -> float:
        return math.pi * self.radius ** 2

    def name(self) -> str:
        return "Circle"


class Rectangle(Shape):
    def __init__(self, width: float, height: float):
        self.width = width
        self.height = height

    def area(self) -> float:
        return self.width * self.height

    def name(self) -> str:
        return "Rectangle"


shapes = [Circle(2.0), Rectangle(3.0, 4.0)]
for shape in shapes:
    print(f"{shape.name()} area: {shape.area():.2f}")
