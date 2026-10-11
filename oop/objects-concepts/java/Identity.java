// Identity.java - equality, identity and what assignment does in Java.
// A variable of class type holds a reference, so assignment shares the object.
import java.util.Objects;

public class Identity {
    static final class Point {
        int x, y;
        Point(int x, int y) { this.x = x; this.y = y; }
        Point(Point other) { this(other.x, other.y); }      // copy constructor

        @Override public boolean equals(Object o) {
            return o instanceof Point p && p.x == x && p.y == y;
        }
        @Override public int hashCode() { return Objects.hash(x, y); }
    }

    public static void main(String[] args) {
        Point p1 = new Point(1, 2);
        Point p2 = new Point(1, 2);
        System.out.println("p1.equals(p2) (equal values): " + p1.equals(p2));
        System.out.println("p1 == p2 (same object):       " + (p1 == p2));

        Point q = p1;                  // copies the reference, not the object
        q.x = 99;
        System.out.println("after q = p1; q.x = 99:   p1.x = " + p1.x + ", q.x = " + q.x);

        p1.x = 1;
        Point c = new Point(p1);       // an explicit copy
        c.x = 99;
        System.out.println("after c = copy of p1; c.x = 99:   p1.x = " + p1.x);
    }
}
