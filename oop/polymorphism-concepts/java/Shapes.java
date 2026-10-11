// Shapes.java - runtime polymorphism: the loop calls area() and name()
// through a Shape reference, and each object runs its own version.
abstract class Shape {
    abstract double area();
    abstract String name();
}

class Circle extends Shape {
    private final double radius;
    Circle(double radius) { this.radius = radius; }
    @Override double area() { return Math.PI * radius * radius; }
    @Override String name() { return "Circle"; }
}

class Rectangle extends Shape {
    private final double width, height;
    Rectangle(double width, double height) {
        this.width = width;
        this.height = height;
    }
    @Override double area() { return width * height; }
    @Override String name() { return "Rectangle"; }
}

public class Shapes {
    public static void main(String[] args) {
        Shape[] shapes = { new Circle(2.0), new Rectangle(3.0, 4.0) };
        for (Shape shape : shapes)
            System.out.printf("%s area: %.2f%n", shape.name(), shape.area());
    }
}
