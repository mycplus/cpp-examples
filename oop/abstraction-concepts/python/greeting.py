"""greeting.py - abstraction: greeting() depends on what a clock does
(report the hour), not on how any particular clock finds it out."""
from abc import ABC, abstractmethod
from datetime import datetime


class Clock(ABC):
    @abstractmethod
    def hour(self) -> int:
        """0 to 23; every concrete clock must say how."""


class SystemClock(Clock):
    def hour(self) -> int:
        return datetime.now().hour


class FixedClock(Clock):
    def __init__(self, hour: int) -> None:
        self._hour = hour

    def hour(self) -> int:
        return self._hour


def greeting(clock: Clock) -> str:
    h = clock.hour()
    if h < 12:
        return "Good morning"
    if h < 18:
        return "Good afternoon"
    return "Good evening"


for h in (8, 14, 21):
    print(f"{h:02d}:00 -> {greeting(FixedClock(h))}")

now = SystemClock()
known = greeting(now) in ("Good morning", "Good afternoon", "Good evening")
print(f"system clock: hour in 0..23 = {str(0 <= now.hour() <= 23).lower()}, greeting known = {str(known).lower()}")

# Clock()  # raises TypeError: Clock is abstract
