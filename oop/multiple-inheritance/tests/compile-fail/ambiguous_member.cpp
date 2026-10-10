// ambiguous_member.cpp - must not compile: power() is declared in both bases,
// so the unqualified name is ambiguous.
struct Engine  { int power() const { return 150; } };
struct Battery { int power() const { return 60; } };

struct Hybrid : Engine, Battery {};

int main() {
    Hybrid car;
    return car.power();
}
