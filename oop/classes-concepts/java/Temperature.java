// Temperature.java - the same class in Java.
import java.util.List;
import java.util.Locale;

public class Temperature {
    private static int created = 0;                   // one per class
    private final double celsius;                     // one per object

    public Temperature(double celsius) {
        if (celsius < -273.15)
            throw new IllegalArgumentException("below absolute zero: " + format(celsius) + " C");
        this.celsius = celsius;
        created++;
    }

    public static Temperature fromFahrenheit(double f) { return new Temperature((f - 32) * 5 / 9); }
    public static int created() { return created; }

    public double celsius() { return celsius; }
    public double fahrenheit() { return celsius * 9 / 5 + 32; }

    @Override
    public String toString() { return format(celsius()) + " C = " + format(fahrenheit()) + " F"; }

    private static String format(double v) { return String.format(Locale.ROOT, "%.1f", v); }

    public static void main(String[] args) {
        Temperature room = new Temperature(21.5);
        Temperature boiling = new Temperature(100);
        Temperature cold = Temperature.fromFahrenheit(-40);

        for (Temperature t : List.of(room, boiling, cold))
            System.out.println(t);
        System.out.println("objects created: " + Temperature.created());

        try {
            new Temperature(-300);
        } catch (IllegalArgumentException e) {
            System.out.println("error: " + e.getMessage());
        }
        System.out.println("objects created: " + Temperature.created());
    }
}
