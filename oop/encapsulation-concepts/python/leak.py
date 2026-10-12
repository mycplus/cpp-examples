"""leak.py - returning the private list hands it to the caller.
LeakyTeam's limit of three members can be broken from outside; Team's cannot."""

LIMIT = 3


class LeakyTeam:
    def __init__(self) -> None:
        self._members: list[str] = []

    def add(self, name: str) -> None:
        if len(self._members) >= LIMIT:
            raise ValueError("team is full")
        self._members.append(name)

    @property
    def members(self) -> list[str]:
        return self._members  # the private list itself


class Team:
    def __init__(self) -> None:
        self._members: list[str] = []

    def add(self, name: str) -> None:
        if len(self._members) >= LIMIT:
            raise ValueError("team is full")
        self._members.append(name)

    @property
    def members(self) -> tuple[str, ...]:
        return tuple(self._members)  # a read-only copy


leaky = LeakyTeam()
for n in ("Ana", "Ben", "Cy"):
    leaky.add(n)
print(f"leaky: added Ana, Ben, Cy -> {len(leaky.members)} members")
leaky.members.append("Dee")
leaky.members.append("Eve")
print(f"leaky: caller appended twice -> {len(leaky.members)} members (limit is {LIMIT})")

team = Team()
for n in ("Ana", "Ben", "Cy"):
    team.add(n)
print(f"safe: added Ana, Ben, Cy -> {len(team.members)} members")
try:
    team.members.append("Dee")
except AttributeError:
    print(f"safe: append rejected (AttributeError), team still has {len(team.members)} members")
