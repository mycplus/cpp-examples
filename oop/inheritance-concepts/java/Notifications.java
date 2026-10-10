// Notifications.java - inheritance in Java: the same three classes.
import java.util.List;

public class Notifications {
    static class Notification {
        private final String recipient;

        Notification(String recipient) { this.recipient = recipient; }

        final void send(String message) {
            System.out.println("to " + recipient + ": " + format(message));
        }

        protected String format(String message) { return message; }
    }

    static class Email extends Notification {
        Email(String recipient) { super(recipient); }

        @Override
        protected String format(String message) {
            return "[email] " + super.format(message);
        }
    }

    static class Sms extends Notification {
        Sms(String recipient) { super(recipient); }

        @Override
        protected String format(String message) {
            return "[sms] " + message.substring(0, Math.min(19, message.length()));
        }
    }

    public static void main(String[] args) {
        List<Notification> outbox = List.of(
            new Notification("ops-team"),
            new Email("ada@example.com"),
            new Sms("+1-555-0100"));

        for (Notification n : outbox)
            n.send("Disk usage on db-01 is above 90 percent");
    }
}
