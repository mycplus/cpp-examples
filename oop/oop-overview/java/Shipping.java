// Shipping.java - the object-oriented version: each shipping method is an
// object that knows its own cost, and quote() works with any of them.
import java.util.List;

public final class Shipping {
    interface ShippingMethod {  // abstraction: what every shipping method offers
        String name();
        long costCents(int kg);
    }

    static final class RateShipping implements ShippingMethod {  // inheritance: one implementation
        private final String name;  // encapsulation: the rates are checked once and can't change afterwards
        private final long base;
        private final long perKg;

        RateShipping(String name, long baseCents, long perKgCents) {
            if (baseCents < 0 || perKgCents < 0) throw new IllegalArgumentException("rates must not be negative");
            this.name = name;
            this.base = baseCents;
            this.perKg = perKgCents;
        }
        @Override public String name() { return name; }
        @Override public long costCents(int kg) { return base + perKg * kg; }
    }

    static final class Pickup implements ShippingMethod {
        @Override public String name() { return "pickup"; }
        @Override public long costCents(int kg) { return 0; }
    }

    static final class Courier implements ShippingMethod {  // added later: quote() did not change
        @Override public String name() { return "courier"; }
        @Override public long costCents(int kg) { return 750; }
    }

    static String money(long cents) {
        return String.format("%d.%02d", cents / 100, cents % 100);
    }

    static void quote(ShippingMethod method, int kg) {  // polymorphism: any method works
        System.out.printf("%-8s %d kg: %s%n", method.name(), kg, money(method.costCents(kg)));
    }

    public static void main(String[] args) {
        List<ShippingMethod> methods = List.of(
            new RateShipping("standard", 400, 50),
            new RateShipping("express", 1000, 100),
            new Pickup(),
            new Courier());

        for (ShippingMethod method : methods) quote(method, 2);
    }
}
