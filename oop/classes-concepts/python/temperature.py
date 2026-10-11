"""temperature.py - the same class in Python."""


class Temperature:
    created = 0                                   # class attribute: one per class

    def __init__(self, celsius):
        if celsius < -273.15:
            raise ValueError(f"below absolute zero: {celsius:.1f} C")
        self._celsius = celsius                   # instance attribute: one per object
        Temperature.created += 1

    @classmethod
    def from_fahrenheit(cls, f):
        return cls((f - 32) * 5 / 9)

    @property
    def celsius(self):
        return self._celsius

    @property
    def fahrenheit(self):
        return self._celsius * 9 / 5 + 32

    def __str__(self):
        return f"{self.celsius:.1f} C = {self.fahrenheit:.1f} F"


if __name__ == "__main__":
    room = Temperature(21.5)
    boiling = Temperature(100)
    cold = Temperature.from_fahrenheit(-40)

    for t in (room, boiling, cold):
        print(t)
    print("objects created:", Temperature.created)

    try:
        Temperature(-300)
    except ValueError as e:
        print("error:", e)
    print("objects created:", Temperature.created)
