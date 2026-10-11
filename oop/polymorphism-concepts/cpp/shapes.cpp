// shapes.cpp - runtime polymorphism: the loop calls area() and name()
// through a Shape pointer, and each object runs its own version.
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

constexpr double kPi = 3.141592653589793;

class Shape {
public:
    virtual double area() const = 0;        // pure virtual: must be overridden
    virtual std::string name() const = 0;
    virtual ~Shape() = default;             // a polymorphic base needs a virtual destructor
};

class Circle : public Shape {
    double radius;
public:
    explicit Circle(double r) : radius(r) {}
    double area() const override { return kPi * radius * radius; }
    std::string name() const override { return "Circle"; }
};

class Rectangle : public Shape {
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    double area() const override { return width * height; }
    std::string name() const override { return "Rectangle"; }
};

int main() {
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(2.0));
    shapes.push_back(std::make_unique<Rectangle>(3.0, 4.0));

    std::cout << std::fixed << std::setprecision(2);
    for (const auto& shape : shapes)
        std::cout << shape->name() << " area: " << shape->area() << '\n';
}
