"""shipping.py - the object-oriented version: each shipping method is an
object that knows its own cost, and quote() works with any of them."""
from abc import ABC, abstractmethod


class ShippingMethod(ABC):  # abstraction: what every shipping method offers
    @abstractmethod
    def name(self) -> str: ...

    @abstractmethod
    def cost_cents(self, kg: int) -> int: ...


class RateShipping(ShippingMethod):  # inheritance: one implementation
    def __init__(self, name: str, base_cents: int, per_kg_cents: int) -> None:
        if base_cents < 0 or per_kg_cents < 0:
            raise ValueError("rates must not be negative")
        self._name = name  # encapsulation: internal by convention, set once here
        self._base = base_cents
        self._per_kg = per_kg_cents

    def name(self) -> str:
        return self._name

    def cost_cents(self, kg: int) -> int:
        return self._base + self._per_kg * kg


class Pickup(ShippingMethod):
    def name(self) -> str:
        return "pickup"

    def cost_cents(self, kg: int) -> int:
        return 0


class Courier(ShippingMethod):  # added later: quote() did not change
    def name(self) -> str:
        return "courier"

    def cost_cents(self, kg: int) -> int:
        return 750


def money(cents: int) -> str:
    return f"{cents // 100}.{cents % 100:02d}"


def quote(method: ShippingMethod, kg: int) -> None:  # polymorphism: any method works
    print(f"{method.name():<8} {kg} kg: {money(method.cost_cents(kg))}")


methods: list[ShippingMethod] = [
    RateShipping("standard", 400, 50),
    RateShipping("express", 1000, 100),
    Pickup(),
    Courier(),
]
for method in methods:
    quote(method, 2)
