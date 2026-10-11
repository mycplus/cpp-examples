"""protocol.py - two ways to state an abstraction in Python.
An ABC is checked when an object is created; a Protocol is checked only
by a static type checker, and any object with the right method works."""
from abc import ABC, abstractmethod
from typing import Protocol


class Clock(Protocol):
    def hour(self) -> int: ...


class Sundial:  # no base class: it matches Clock by having hour()
    def hour(self) -> int:
        return 9


def greeting(clock: Clock) -> str:
    h = clock.hour()
    if h < 12:
        return "Good morning"
    if h < 18:
        return "Good afternoon"
    return "Good evening"


print(f"Sundial (no base class) -> {greeting(Sundial())}")


class AbstractClock(ABC):
    @abstractmethod
    def hour(self) -> int: ...


class BrokenClock(AbstractClock):  # forgot to implement hour()
    pass


try:
    BrokenClock()
except TypeError:
    print("BrokenClock() -> TypeError: the abstract method hour() was not implemented")
