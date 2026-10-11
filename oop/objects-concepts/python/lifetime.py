"""lifetime.py - a with statement calls __exit__ when the block ends, so the
cleanup does not depend on when the object is garbage collected."""


class LogFile:
    def __init__(self, name):
        self.name = name
        print("open", name)

    def write(self, line):
        print("write", line)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc, tb):
        print("close", self.name)
        return False                               # do not swallow exceptions


if __name__ == "__main__":
    with LogFile("app.log") as log:
        log.write("started")                       # __exit__ runs after this block
    print("after the block")
