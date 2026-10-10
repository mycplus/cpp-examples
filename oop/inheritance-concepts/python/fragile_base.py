"""fragile_base.py - the same dependency on a base class's implementation."""


class Tags:
    def __init__(self):
        self._tags = set()

    def add(self, tag):
        self._tags.add(tag)

    def add_all(self, tags):
        for t in tags:
            self.add(t)

    def size(self):
        return len(self._tags)


class CountingTags(Tags):
    def __init__(self):
        super().__init__()
        self.attempts = 0

    def add(self, tag):
        self.attempts += 1
        super().add(tag)

    def add_all(self, tags):
        self.attempts += len(tags)
        super().add_all(tags)


if __name__ == "__main__":
    t = CountingTags()
    t.add_all(["urgent", "billing", "eu"])
    print(f"tags stored: {t.size()}, attempts counted: {t.attempts}")
