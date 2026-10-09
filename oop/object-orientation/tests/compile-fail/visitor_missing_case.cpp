// visitor_missing_case.cpp - must NOT compile: a third alternative was added
// to the variant, and the visitor has no overload for it.
#include <variant>

struct Circle    { double r; };
struct Rectangle { double w, h; };
struct Triangle  { double b, h; };

using Shape = std::variant<Circle, Rectangle, Triangle>;

struct Area {
    double operator()(const Circle& c) const    { return 3.14159265358979 * c.r * c.r; }
    double operator()(const Rectangle& r) const { return r.w * r.h; }
};

int main()
{
    Shape s = Triangle{2.0, 3.0};
    return static_cast<int>(std::visit(Area{}, s));
}
