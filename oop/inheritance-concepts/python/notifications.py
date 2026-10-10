"""notifications.py - inheritance in Python: the same three classes."""


class Notification:
    def __init__(self, recipient):
        self._recipient = recipient

    def send(self, message):
        print(f"to {self._recipient}: {self.format(message)}")

    def format(self, message):
        return message


class Email(Notification):
    def format(self, message):
        return "[email] " + super().format(message)


class Sms(Notification):
    def format(self, message):
        return "[sms] " + message[:19]


if __name__ == "__main__":
    outbox = [Notification("ops-team"), Email("ada@example.com"), Sms("+1-555-0100")]
    for n in outbox:
        n.send("Disk usage on db-01 is above 90 percent")
