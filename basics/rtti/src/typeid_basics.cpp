// typeid_basics.cpp - what typeid reports for polymorphic and
// non-polymorphic classes, and how std::type_index makes types usable as keys.
#include "type_name.h"

#include <iostream>
#include <map>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <vector>

// Polymorphic: has a virtual function, so objects carry their dynamic type.
struct Shape {
    virtual ~Shape() = default;
};
struct Circle : Shape {};
struct Square : Shape {};

// Not polymorphic: no virtual functions, so typeid sees only the static type.
struct Plain {};
struct PlainDerived : Plain {};

int main()
{
    Circle circle;
    Shape& shape = circle;
    std::cout << "static type of shape:        " << readable_name(typeid(Shape)) << '\n';
    std::cout << "typeid(shape) (polymorphic): " << readable_name(typeid(shape)) << '\n';

    PlainDerived derived;
    Plain& plain = derived;
    std::cout << "typeid(plain) (not polymorphic): " << readable_name(typeid(plain)) << '\n';

    // Top-level const and references are ignored.
    std::cout << std::boolalpha;
    std::cout << "typeid(const Circle) == typeid(Circle):  " << (typeid(const Circle) == typeid(Circle)) << '\n';
    std::cout << "typeid(Circle&) == typeid(Circle):       " << (typeid(Circle&) == typeid(Circle)) << '\n';
    std::cout << "typeid(shape) == typeid(Circle):         " << (typeid(shape) == typeid(Circle)) << '\n';

    // typeid on a null pointer to a polymorphic type throws std::bad_typeid.
    Shape* nothing = nullptr;
    try {
        std::cout << readable_name(typeid(*nothing)) << '\n';
    } catch (const std::bad_typeid&) {
        std::cout << "typeid(*nullptr) threw std::bad_typeid\n";
    }

    // std::type_index wraps type_info so it can be a map key.
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>());
    shapes.push_back(std::make_unique<Square>());
    shapes.push_back(std::make_unique<Circle>());
    std::map<std::type_index, int> counts;
    for (const auto& s : shapes) {
        const Shape& current = *s;    // typeid(*s) would call unique_ptr::operator* inside typeid
        ++counts[typeid(current)];
    }
    std::cout << "circles: " << counts[typeid(Circle)] << ", squares: " << counts[typeid(Square)] << '\n';
}
