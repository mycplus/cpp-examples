"""account.py - encapsulation: the balance can change only through
deposit() and withdraw(), which never let it go below zero."""


class Account:
    def __init__(self, owner: str, opening: int) -> None:
        self._owner = owner
        self._balance = 0
        self._history: list[int] = []
        self.deposit(opening)

    def deposit(self, amount: int) -> None:
        self._require_positive(amount)
        self._balance += amount
        self._history.append(amount)

    def withdraw(self, amount: int) -> None:
        self._require_positive(amount)
        if amount > self._balance:
            raise ValueError(f"insufficient funds: balance {self._balance}, requested {amount}")
        self._balance -= amount
        self._history.append(-amount)

    @property
    def owner(self) -> str:
        return self._owner

    @property
    def balance(self) -> int:
        return self._balance

    @property
    def history(self) -> tuple[int, ...]:
        return tuple(self._history)  # a read-only copy

    @staticmethod
    def _require_positive(amount: int) -> None:
        if amount <= 0:
            raise ValueError(f"amount must be positive: {amount}")


acct = Account("Ada", 100)
print(f"{acct.owner} opened with {acct.balance}")

acct.deposit(50)
print(f"deposit 50 -> balance {acct.balance}")
acct.withdraw(30)
print(f"withdraw 30 -> balance {acct.balance}")

for amount in (500, -5):
    try:
        acct.withdraw(amount)
    except ValueError as e:
        print(f"error: {e}")
print(f"balance {acct.balance}")

print("history:", " ".join(f"{t:+d}" for t in acct.history))
