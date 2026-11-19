// alternatives.cpp - one job done three ways: computing the area of mixed
// shapes with dynamic_cast, with a virtual function and with std::variant.
#include <iostream>
#include <memory>
#include <type_traits>
#include <variant>
#include <vector>

namespace with_rtti {
struct Shape { virtual ~Shape() = default; };
struct Circle : Shape { double r; explicit Circle(double r_) : r(r_) {} };
struct Rect : Shape { double w, h; Rect(double w_, double h_) : w(w_), h(h_) {} };

// Every new shape needs a new branch here, and the compiler cannot warn if one is missing.
double area(const Shape& s)
{
    if (auto* c = dynamic_cast<const Circle*>(&s))
        return 3.0 * c->r * c->r;            // pi rounded to 3 keeps the output exact
    if (auto* r = dynamic_cast<const Rect*>(&s))
        return r->w * r->h;
    return 0.0;                              // an unknown shape slips through silently
}
}    // namespace with_rtti

namespace with_virtual {
struct Shape {
    virtual ~Shape() = default;
    virtual double area() const = 0;         // every shape must implement it
};
struct Circle : Shape {
    double r;
    explicit Circle(double r_) : r(r_) {}
    double area() const override { return 3.0 * r * r; }
};
struct Rect : Shape {
    double w, h;
    Rect(double w_, double h_) : w(w_), h(h_) {}
    double area() const override { return w * h; }
};
}    // namespace with_virtual

namespace with_variant {
struct Circle { double r; };
struct Rect { double w, h; };
using Shape = std::variant<Circle, Rect>;    // a closed set of types, no base class

double area(const Shape& s)
{
    return std::visit([](const auto& v) -> double {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, Circle>)
            return 3.0 * v.r * v.r;
        else
            return v.w * v.h;                // a new alternative fails to compile here
    }, s);
}
}    // namespace with_variant

int main()
{
    {
        using namespace with_rtti;
        std::vector<std::unique_ptr<Shape>> shapes;
        shapes.push_back(std::make_unique<Circle>(1.0));
        shapes.push_back(std::make_unique<Rect>(2.0, 3.0));
        double total = 0;
        for (const auto& s : shapes)
            total += area(*s);
        std::cout << "dynamic_cast:     total area " << total << '\n';
    }
    {
        using namespace with_virtual;
        std::vector<std::unique_ptr<Shape>> shapes;
        shapes.push_back(std::make_unique<Circle>(1.0));
        shapes.push_back(std::make_unique<Rect>(2.0, 3.0));
        double total = 0;
        for (const auto& s : shapes)
            total += s->area();
        std::cout << "virtual function: total area " << total << '\n';
    }
    {
        using namespace with_variant;
        std::vector<Shape> shapes{Circle{1.0}, Rect{2.0, 3.0}};
        double total = 0;
        for (const auto& s : shapes)
            total += area(s);
        std::cout << "std::variant:     total area " << total << '\n';
    }
}
