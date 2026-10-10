// order.cpp - the order of construction and destruction in a class hierarchy.
#include <iostream>
#include <string>

struct Note {
    std::string text;
    explicit Note(std::string t) : text(std::move(t)) { std::cout << "  construct " << text << '\n'; }
    ~Note() { std::cout << "  destroy   " << text << '\n'; }
};

struct Base {
    Note base_member{"Base member"};
    Base() { std::cout << "  Base constructor body\n"; }
    virtual ~Base() { std::cout << "  Base destructor body\n"; }
};

struct Derived : Base {
    Note derived_member{"Derived member"};
    Derived() { std::cout << "  Derived constructor body\n"; }
    ~Derived() override { std::cout << "  Derived destructor body\n"; }
};

int main() {
    std::cout << "creating:\n";
    {
        Derived d;
        std::cout << "destroying:\n";
    }
}
