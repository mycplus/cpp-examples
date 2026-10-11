"""duck_typing.py - polymorphism without inheritance: any object with a
speak() method works, whatever its class."""


class Dog:
    def speak(self) -> str:
        return "Woof"


class Cat:
    def speak(self) -> str:
        return "Meow"


class Duck:
    def speak(self) -> str:
        return "Quack"


def make_it_speak(animal) -> None:
    # No common base class required: only that speak() exists.
    print(animal.speak())


for animal in (Dog(), Cat(), Duck()):
    make_it_speak(animal)
