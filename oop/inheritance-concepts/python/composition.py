"""composition.py - the counting set again, built by composition: it holds a
Tags object instead of inheriting from it, so add_all cannot call back into it."""
from fragile_base import Tags


class CountingTags:
    def __init__(self):
        self._tags = Tags()
        self.attempts = 0

    def add(self, tag):
        self.attempts += 1
        self._tags.add(tag)

    def add_all(self, tags):
        self.attempts += len(tags)
        self._tags.add_all(tags)

    def size(self):
        return self._tags.size()


if __name__ == "__main__":
    t = CountingTags()
    t.add_all(["urgent", "billing", "eu"])
    print(f"tags stored: {t.size()}, attempts counted: {t.attempts}")
