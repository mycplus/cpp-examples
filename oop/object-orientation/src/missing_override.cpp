// missing_override.cpp - a derived function that differs from the base only by
// a missing const. It compiles, and it does not override anything.
#include <iostream>
#include <memory>
#include <string>

struct Shape {
    virtual ~Shape() = default;
    virtual std::string name() const { return "shape"; }
};

struct Circle : Shape {
    std::string name() { return "circle"; }        // not const: a new function
};

int main()
{
    std::unique_ptr<Shape> s = std::make_unique<Circle>();
    Circle c;
    std::cout << "through Shape*: " << s->name() << '\n';
    std::cout << "on a Circle:    " << c.name() << '\n';
}
