// access.cpp - what a derived class can reach in its base class.
#include <iostream>

class Base {
public:
    int pub = 1;
protected:
    int prot = 2;
private:
    int priv = 3;
    friend void show_private(const Base&);
};

void show_private(const Base& b) { std::cout << "friend sees priv = " << b.priv << '\n'; }

class Derived : public Base {
public:
    void show() const {
        std::cout << "Derived sees pub = " << pub << ", prot = " << prot << '\n';
        // priv is not accessible here: see tests/compile-fail/private_member.cpp
    }
};

struct FromStruct : Base {};          // struct: public inheritance by default
class FromClass : Base {              // class: private inheritance by default
public:
    int read_pub() const { return pub; }   // still visible inside the class
};

int main() {
    Derived d;
    d.show();
    std::cout << "outside sees d.pub = " << d.pub << '\n';
    show_private(d);

    FromStruct s;
    std::cout << "struct-derived: s.pub = " << s.pub << '\n';
    FromClass c;
    std::cout << "class-derived: c.read_pub() = " << c.read_pub() << '\n';
}
