// override_typo.cpp - must NOT compile: 'override' turns the silent mistake
// in missing_override.cpp into an error.
#include <string>

struct Shape {
    virtual ~Shape() = default;
    virtual std::string name() const { return "shape"; }
};

struct Circle : Shape {
    std::string name() override { return "circle"; }
};

int main() { Circle c; return static_cast<int>(c.name().size()); }
