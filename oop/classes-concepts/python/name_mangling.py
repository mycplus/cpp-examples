"""name_mangling.py - Python has no private keyword. A leading underscore is a
convention; two leading underscores rename the attribute, which hides it from
accidental use but not from deliberate access."""


class Account:
    def __init__(self):
        self._balance = 100         # "internal": a convention only
        self.__pin = 1234           # stored as _Account__pin


acct = Account()
print("acct._balance =", acct._balance)
try:
    print(acct.__pin)
except AttributeError as e:
    print("AttributeError:", e)
print("acct._Account__pin =", acct._Account__pin)
print("attributes:", sorted(vars(acct)))
