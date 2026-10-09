// variant_shapes.cpp - the same "one call, many behaviors" without a class
// hierarchy: a closed set of types in a std::variant, dispatched by std::visit.
#include <iostream>
#include <string>
#include <variant>
#include <vector>

struct Circle    { double r; };
struct Rectangle { double w, h; };

using Shape = std::variant<Circle, Rectangle>;

struct Area {
    double operator()(const Circle& c) const    { return 3.14159265358979 * c.r * c.r; }
    double operator()(const Rectangle& r) const { return r.w * r.h; }
};

int main()
{
    std::vector<Shape> shapes{Circle{1.0}, Rectangle{2.0, 3.0}};
    for (const Shape& s : shapes)
        std::cout << "area: " << std::visit(Area{}, s) << '\n';
    std::cout << "sizeof(Shape) = " << sizeof(Shape) << '\n';
}
