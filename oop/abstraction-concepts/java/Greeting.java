// Greeting.java - abstraction: greeting() depends on what a clock does
// (report the hour), not on how any particular clock finds it out.
import java.time.LocalTime;
import java.util.List;

public final class Greeting {
    interface Clock {
        int hour();  // 0 to 23; every implementation must say how
    }

    static final class SystemClock implements Clock {
        @Override
        public int hour() { return LocalTime.now().getHour(); }
    }

    record FixedClock(int hour) implements Clock {}  // the record's hour() implements Clock

    static String greeting(Clock clock) {
        int h = clock.hour();
        if (h < 12) return "Good morning";
        if (h < 18) return "Good afternoon";
        return "Good evening";
    }

    public static void main(String[] args) {
        for (int h : new int[] {8, 14, 21}) {
            System.out.printf("%02d:00 -> %s%n", h, greeting(new FixedClock(h)));
        }

        Clock now = new SystemClock();
        String g = greeting(now);
        boolean known = List.of("Good morning", "Good afternoon", "Good evening").contains(g);
        System.out.println("system clock: hour in 0..23 = " + (now.hour() >= 0 && now.hour() <= 23)
                           + ", greeting known = " + known);

        // Clock c = new Clock();  // does not compile: Clock is abstract
    }
}
