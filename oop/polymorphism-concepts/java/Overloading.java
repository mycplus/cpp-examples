// Overloading.java - compile-time polymorphism: the compiler picks an add()
// from the argument types before the program runs.
public class Overloading {
    static int    add(int a, int b)       { return a + b; }
    static double add(double a, double b) { return a + b; }
    static String add(String a, String b) { return a + b; }

    public static void main(String[] args) {
        System.out.println(add(2, 3));
        System.out.println(add(2.5, 3.5));
        System.out.println(add("poly", "morphism"));
    }
}
